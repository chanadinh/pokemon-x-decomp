#include "cgfx.h"

#include <stdlib.h>
#include <string.h>

/* Format evidence and bounded support: reproduction/cgfx-textures/README.txt. */
enum { MAX_BYTES = 64 * 1024 * 1024, MAX_ENTRIES = 65536, MAX_NAME = 1024 };
struct px_cgfx {
    const uint8_t *data;
    size_t size, data_start, data_end, image_start, image_end, count;
    struct px_cgfx_texture *textures;
};

static uint32_t u32(const uint8_t *data)
{
    return (uint32_t)data[0] | ((uint32_t)data[1] << 8) |
           ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);
}
static int span(size_t offset, size_t size, size_t start, size_t end)
{
    return offset >= start && offset <= end && size <= end - offset;
}
static int relative(const struct px_cgfx *cgfx, size_t field, size_t size,
                     size_t start, size_t end, size_t *target)
{
    if (!span(field, 4, cgfx->data_start, cgfx->data_end)) return 0;
    const uint32_t raw = u32(cgfx->data + field);
    if (raw == 0) return 0;
    const int64_t delta = raw < UINT32_C(0x80000000) ? (int64_t)raw :
                         (int64_t)raw - INT64_C(0x100000000);
    const int64_t resolved = (int64_t)field + delta;
    if (resolved < 0 || (uint64_t)resolved > (uint64_t)end) return 0;
    *target = (size_t)resolved;
    return span(*target, size, start, end);
}
static int name(const struct px_cgfx *cgfx, size_t field,
                 const char **text, size_t *length)
{
    size_t offset;
    if (!relative(cgfx, field, 1, cgfx->data_start, cgfx->data_end, &offset)) return 0;
    size_t available = cgfx->data_end - offset;
    if (available > MAX_NAME + 1) available = MAX_NAME + 1;
    const uint8_t *end = memchr(cgfx->data + offset, 0, available);
    if (end == NULL) return 0;
    *text = (const char *)cgfx->data + offset;
    *length = (size_t)(end - (cgfx->data + offset));
    return 1;
}
static int power_of_two(uint32_t value)
{
    return value >= 8 && (value & (value - 1)) == 0;
}

static enum px_cgfx_result texture_header(const struct px_cgfx *cgfx,
                                           size_t object, struct px_cgfx_texture *texture)
{
    texture->object_offset = (uint32_t)object;
    texture->flags = u32(cgfx->data + object);
    if (memcmp(cgfx->data + object + 4, "TXOB", 4) != 0 ||
        !span(object, 0x18, cgfx->data_start, cgfx->data_end))
        return PX_CGFX_INVALID_HEADER;
    texture->revision = u32(cgfx->data + object + 8);
    if (!name(cgfx, object + 12, &texture->name, &texture->name_size))
        return PX_CGFX_INVALID_HEADER;
    texture->decode_status = PX_CGFX_UNSUPPORTED;
    if (texture->flags != UINT32_C(0x20000011) ||
        texture->revision != UINT32_C(0x05000000)) return PX_CGFX_OK;
    if (!span(object, 0x3c, cgfx->data_start, cgfx->data_end))
        return PX_CGFX_INVALID_HEADER;
    size_t descriptor, payload;
    if (!relative(cgfx, object + 0x38, 0x20,
                  cgfx->data_start, cgfx->data_end, &descriptor))
        return PX_CGFX_INVALID_HEADER;
    texture->image_offset = (uint32_t)descriptor;
    texture->height = u32(cgfx->data + object + 0x18);
    texture->width = u32(cgfx->data + object + 0x1c);
    texture->mipmap_levels = u32(cgfx->data + object + 0x28);
    texture->format = u32(cgfx->data + object + 0x34);
    texture->stored_height = u32(cgfx->data + descriptor);
    texture->stored_width = u32(cgfx->data + descriptor + 4);
    texture->data_size = u32(cgfx->data + descriptor + 8);
    texture->bits_per_pixel = u32(cgfx->data + descriptor + 0x14);
    if (texture->data_size == 0 ||
        !relative(cgfx, descriptor + 12, texture->data_size,
                  cgfx->image_start, cgfx->image_end, &payload))
        return PX_CGFX_INVALID_HEADER;
    texture->data_offset = (uint32_t)payload;

    /* Metadata can be listed even when pixel interpretation is unsupported. */
    const unsigned bits = px_texture_bits_per_pixel(texture->format);
    if (bits == 0 || texture->mipmap_levels != 1 ||
        u32(cgfx->data + object + 0x2c) != 0 ||
        u32(cgfx->data + object + 0x30) != 0 ||
        u32(cgfx->data + descriptor + 0x10) != 0 ||
        u32(cgfx->data + descriptor + 0x18) != 0 ||
        u32(cgfx->data + descriptor + 0x1c) != 0) return PX_CGFX_OK;
    if (texture->width == 0 || texture->height == 0 ||
        texture->stored_width == 0 || texture->stored_height == 0) {
        texture->decode_status = PX_CGFX_INVALID_HEADER;
        return PX_CGFX_OK;
    }
    if (texture->width > 4096 || texture->height > 4096 ||
        texture->stored_width > 4096 || texture->stored_height > 4096) {
        texture->decode_status = PX_CGFX_LIMIT_EXCEEDED;
        return PX_CGFX_OK;
    }
    if (texture->width > texture->stored_width || texture->height > texture->stored_height ||
        !power_of_two(texture->stored_width) || !power_of_two(texture->stored_height))
        return PX_CGFX_OK;
    const size_t expected = (size_t)texture->stored_width * texture->stored_height * bits / 8;
    texture->decode_status = texture->bits_per_pixel == bits && texture->data_size == expected ?
                            PX_CGFX_OK : PX_CGFX_INVALID_HEADER;
    return PX_CGFX_OK;
}

enum px_cgfx_result px_cgfx_open(const uint8_t *data, size_t size,
                                struct px_cgfx **out)
{
    if (out == NULL) return PX_CGFX_INVALID_ARGUMENT;
    *out = NULL;
    if (data == NULL) return PX_CGFX_INVALID_ARGUMENT;
    if (size > MAX_BYTES) return PX_CGFX_LIMIT_EXCEEDED;
    if (size < 20) return PX_CGFX_TRUNCATED;
    if (memcmp(data, "CGFX", 4) != 0) return PX_CGFX_INVALID_HEADER;
    if (memcmp(data + 4, "\xff\xfe\x14\x00", 4) != 0 ||
        u32(data + 8) != UINT32_C(0x05000000)) return PX_CGFX_UNSUPPORTED;
    const uint32_t declared_size = u32(data + 12), sections = u32(data + 16);
    if (declared_size > size) return PX_CGFX_TRUNCATED;
    if (declared_size != size) return PX_CGFX_INVALID_HEADER;
    if (sections != 1 && sections != 2) return PX_CGFX_UNSUPPORTED;
    struct px_cgfx parsed = {0};
    parsed.data = data;
    parsed.size = size;
    size_t cursor = 20;
    for (uint32_t section = 0; section < sections; ++section) {
        if (!span(cursor, 8, 0, size)) return PX_CGFX_TRUNCATED;
        if (memcmp(data + cursor, section == 0 ? "DATA" : "IMAG", 4) != 0)
            return PX_CGFX_UNSUPPORTED;
        const uint32_t length = u32(data + cursor + 4);
        if (length < 8) return PX_CGFX_INVALID_HEADER;
        if (!span(cursor, length, 0, size)) return PX_CGFX_TRUNCATED;
        if (section == 0) {
            parsed.data_start = cursor + 8;
            parsed.data_end = cursor + length;
            if (!span(parsed.data_start, 16 * 8, parsed.data_start, parsed.data_end))
                return PX_CGFX_INVALID_HEADER;
        } else {
            parsed.image_start = cursor + 8;
            parsed.image_end = cursor + length;
        }
        cursor += length;
    }
    if (cursor != size) return PX_CGFX_INVALID_HEADER;

    size_t dictionaries[16] = {0};
    uint32_t counts[16] = {0};
    size_t total = 0;
    for (unsigned slot = 0; slot < 16; ++slot) {
        const size_t field = parsed.data_start + 8 * slot;
        counts[slot] = u32(data + field);
        if (counts[slot] > MAX_ENTRIES - total) return PX_CGFX_LIMIT_EXCEEDED;
        total += counts[slot];
        if (counts[slot] == 0 && u32(data + field + 4) == 0) continue;
        size_t dictionary;
        if (!relative(&parsed, field + 4, 28, parsed.data_start, parsed.data_end, &dictionary) ||
            memcmp(data + dictionary, "DICT", 4) != 0) return PX_CGFX_INVALID_HEADER;
        const size_t length = 28 + (size_t)counts[slot] * 16;
        if (u32(data + dictionary + 4) != length || u32(data + dictionary + 8) != counts[slot] ||
            !span(dictionary, length, parsed.data_start, parsed.data_end))
            return PX_CGFX_INVALID_HEADER;
        dictionaries[slot] = dictionary;
    }
    parsed.count = counts[1];
    if (parsed.count != 0) {
        parsed.textures = calloc(parsed.count, sizeof *parsed.textures);
        if (parsed.textures == NULL) return PX_CGFX_NO_MEMORY;
    }
    enum px_cgfx_result result = PX_CGFX_OK;
    for (unsigned slot = 0; slot < 16; ++slot) {
        for (uint32_t index = 0; index < counts[slot]; ++index) {
            const size_t record = dictionaries[slot] + 28 + (size_t)index * 16;
            size_t object, name_size;
            const char *key;
            if (!name(&parsed, record + 8, &key, &name_size) ||
                !relative(&parsed, record + 12, 8, parsed.data_start, parsed.data_end, &object)) {
                result = PX_CGFX_INVALID_HEADER;
                goto fail;
            }
            if (slot == 1) {
                struct px_cgfx_texture *texture = parsed.textures + index;
                texture->dictionary_name = key;
                texture->dictionary_name_size = name_size;
                result = texture_header(&parsed, object, texture);
                if (result != PX_CGFX_OK) goto fail;
            }
        }
    }
    struct px_cgfx *cgfx = malloc(sizeof *cgfx);
    if (cgfx == NULL) { result = PX_CGFX_NO_MEMORY; goto fail; }
    *cgfx = parsed;
    *out = cgfx;
    return PX_CGFX_OK;
fail:
    free(parsed.textures);
    return result;
}

size_t px_cgfx_texture_count(const struct px_cgfx *cgfx)
{
    return cgfx == NULL ? 0 : cgfx->count;
}
const struct px_cgfx_texture *px_cgfx_texture(const struct px_cgfx *cgfx, size_t index)
{
    return cgfx == NULL || index >= cgfx->count ? NULL : cgfx->textures + index;
}
enum px_cgfx_result px_cgfx_decode_texture(const struct px_cgfx *cgfx,
    size_t index, struct px_texture_image *out)
{
    if (out == NULL) return PX_CGFX_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (cgfx == NULL) return PX_CGFX_INVALID_ARGUMENT;
    const struct px_cgfx_texture *texture = px_cgfx_texture(cgfx, index);
    if (texture == NULL) return PX_CGFX_NOT_FOUND;
    if (texture->decode_status != PX_CGFX_OK) return texture->decode_status;
    const enum px_texture_result result = px_texture_decode_payload(
        cgfx->data + texture->data_offset, texture->data_size,
        texture->width, texture->height, texture->stored_width, texture->stored_height,
        texture->format, out);
    switch (result) {
    case PX_TEXTURE_OK: return PX_CGFX_OK;
    case PX_TEXTURE_INVALID_ARGUMENT: return PX_CGFX_INVALID_HEADER;
    case PX_TEXTURE_TRUNCATED: return PX_CGFX_TRUNCATED;
    case PX_TEXTURE_UNSUPPORTED: return PX_CGFX_UNSUPPORTED;
    case PX_TEXTURE_LIMIT_EXCEEDED: return PX_CGFX_LIMIT_EXCEEDED;
    case PX_TEXTURE_NO_MEMORY: return PX_CGFX_NO_MEMORY;
    }
    return PX_CGFX_UNSUPPORTED;
}
void px_cgfx_close(struct px_cgfx *cgfx)
{
    if (cgfx == NULL) return;
    free(cgfx->textures);
    free(cgfx);
}
const char *px_cgfx_result_string(enum px_cgfx_result result)
{
    switch (result) {
    case PX_CGFX_OK: return "success";
    case PX_CGFX_INVALID_ARGUMENT: return "invalid CGFX argument";
    case PX_CGFX_TRUNCATED: return "truncated CGFX data";
    case PX_CGFX_INVALID_HEADER: return "invalid CGFX texture metadata or extent";
    case PX_CGFX_UNSUPPORTED: return "unsupported CGFX revision, texture variant, or layout";
    case PX_CGFX_LIMIT_EXCEEDED: return "CGFX size, entry, or texture dimension limit exceeded";
    case PX_CGFX_NO_MEMORY: return "cannot allocate CGFX index or texture";
    case PX_CGFX_NOT_FOUND: return "CGFX texture index does not exist";
    }
    return "unknown CGFX result";
}
