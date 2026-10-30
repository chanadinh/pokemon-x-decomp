#include "archive_io.h"

static int failed(uint32_t result) { return (result & UINT32_C(0x80000000)) != 0; }
static int64_t as_signed(uint64_t value)
{
    return value < UINT64_C(0x8000000000000000) ? (int64_t)value
                                                 : -(int64_t)(UINT64_C(0xffffffffffffffff) - value) - 1;
}
static uint64_t sign_extend(uint32_t value)
{
    return value < UINT32_C(0x80000000) ? value : UINT64_C(0xffffffff00000000) | value;
}
static uint64_t load64(uint32_t address, const struct px_pokemon_api *m)
{
    const uint64_t low = m->load32(m->user, address);
    return low | (uint64_t)m->load32(m->user, address + UINT32_C(4)) << 32;
}

uint32_t px_file_get_size(uint32_t handle, uint64_t *size, const struct px_archive_api *api)
{
    if (handle == 0) return PX_FS_RESULT_NULL_ARGUMENT;
    return api->get_size(api->user, handle, size);
}

/* Refresh the cached size at +0x0c, high word first; a failure clears it. */
static uint32_t refresh_size(uint32_t stream, uint64_t *size, const struct px_archive_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const uint32_t handle = m->load32(m->user, stream) & ~UINT32_C(1);
    const uint32_t result = px_file_get_size(handle, size, api);
    if (failed(result)) {
        m->store32(m->user, stream + UINT32_C(0x0c), 0);
        m->store32(m->user, stream + UINT32_C(0x10), 0);
    } else {
        m->store32(m->user, stream + UINT32_C(0x10), (uint32_t)(*size >> 32));
        m->store32(m->user, stream + UINT32_C(0x0c), (uint32_t)*size);
    }
    return result;
}

uint32_t px_file_stream_set_position(uint32_t stream, uint64_t position, const struct px_archive_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    if (as_signed(position) < 0) return PX_FS_RESULT_BAD_POSITION;
    if (as_signed(position) >= as_signed(load64(stream + UINT32_C(0x0c), m))) {
        uint64_t size = 0;
        const uint32_t result = refresh_size(stream, &size, api);
        if (failed(result)) return result;
        if (as_signed(size) < as_signed(position)) return PX_FS_RESULT_BAD_POSITION;
    }
    m->store32(m->user, stream + UINT32_C(4), (uint32_t)position);
    m->store32(m->user, stream + UINT32_C(8), (uint32_t)(position >> 32));
    return 0;
}

uint32_t px_file_stream_seek(uint32_t stream, uint64_t offset, uint32_t whence, const struct px_archive_api *api)
{
    if (whence == 1) {
        offset += load64(stream + UINT32_C(4), api->memory);
    } else if (whence == 2) {
        uint64_t size = 0;
        const uint32_t result = refresh_size(stream, &size, api);
        if (failed(result)) return result;
        offset += size;
    } else if (whence != 0) {
        return PX_FS_RESULT_BAD_WHENCE;
    }
    return px_file_stream_set_position(stream, offset, api);
}

uint32_t px_file_stream_read(uint32_t stream, uint32_t buffer, uint32_t size,
                             uint32_t *count, const struct px_archive_api *api)
{
    /* Each platform read starts at the stored position, which advances by the
     * sign-extended byte count. Failure returns without writing *count. */
    const struct px_pokemon_api *m = api->memory;
    uint32_t total = 0;
    while (size != 0) {
        const uint32_t handle = m->load32(m->user, stream) & ~UINT32_C(1);
        uint32_t got = 0;
        const uint32_t result = api->read(api->user, handle, load64(stream + UINT32_C(4), m), buffer, size, &got);
        if (failed(result)) return result;
        const uint64_t position = load64(stream + UINT32_C(4), m) + sign_extend(got);
        total += got;
        m->store32(m->user, stream + UINT32_C(4), (uint32_t)position);
        m->store32(m->user, stream + UINT32_C(8), (uint32_t)(position >> 32));
        if (got == size || got == 0) break;
        buffer += got;
        size -= got;
    }
    *count = total;
    return 0;
}

void px_archive_member_info(uint32_t archive, uint32_t index, uint32_t info[3], const struct px_archive_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    if (m->load32(m->user, archive + UINT32_C(0x4c)) >= UINT32_C(0x80000000)) {
        api->stream_info(api->user, archive, index, info);
        return;
    }
    (void)m->load32(m->user, archive + UINT32_C(0x4c));
    const uint32_t count = m->load16(m->user, archive + UINT32_C(0x34));
    const uint32_t table = m->load32(m->user, archive + UINT32_C(0x3c));
    if (count <= index) {
        /* The original never leaves this loop, rereading the hook each pass. */
        for (;;) {
            const uint32_t hook = m->load32(m->user, PX_ARCHIVE_PANIC_HOOK);
            if (hook != 0) api->panic(api->user, hook, PX_ARCHIVE_PANIC_MESSAGE);
        }
    }
    const uint32_t row = table + index * UINT32_C(12);
    info[0] = m->load32(m->user, row);
    info[1] = m->load32(m->user, row + UINT32_C(4));
    info[2] = m->load32(m->user, row + UINT32_C(8));
}

uint32_t px_archive_read(uint32_t archive, uint32_t size, uint32_t buffer, const struct px_archive_api *api)
{
    if (!api->is_open(api->user, archive + UINT32_C(8))) return 0;
    uint32_t count = 0;
    const uint32_t result = px_file_stream_read(archive + UINT32_C(0x18), buffer, size, &count, api);
    if (failed(result)) {
        api->fatal(api->user, result, UINT32_C(0x00127bcc));
        return 0;
    }
    return count;
}

void px_archive_read_at(uint32_t archive, uint32_t offset, uint32_t size, uint32_t buffer,
                        uint32_t *count, const struct px_archive_api *api)
{
    api->lock(api->user, archive + UINT32_C(0x60));
    const uint32_t result = px_file_stream_seek(archive + UINT32_C(0x18), sign_extend(offset), 0, api);
    if (failed(result)) api->fatal(api->user, result, UINT32_C(0x00127ca4));
    const uint32_t read = px_archive_read(archive, size, buffer, api);
    if (count) *count = read;
    api->unlock(api->user, archive + UINT32_C(0x60));
}

uint32_t px_read_archive_member(uint32_t archive, uint32_t index, uint32_t buffer, const struct px_archive_api *api)
{
    uint32_t info[3], count = 0;
    px_archive_member_info(archive, index, info, api);
    px_archive_read_at(archive, info[2], info[0], buffer, &count, api);
    return count;
}

void px_seek_archive_member(uint32_t archive, uint32_t index, uint32_t offset, const struct px_archive_api *api)
{
    /* No lock is taken here; the 32-bit sum is sign-extended. */
    uint32_t info[3];
    px_archive_member_info(archive, index, info, api);
    const uint32_t result = px_file_stream_seek(archive + UINT32_C(0x18), sign_extend(info[2] + offset), 0, api);
    if (failed(result)) api->fatal(api->user, result, UINT32_C(0x00138208));
}
