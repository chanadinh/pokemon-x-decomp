#include "message_construction.h"

static const struct px_archive_index_api *index_of(const struct px_message_construction_api *api)
{
    return api->resources->index;
}
static const struct px_pokemon_api *memory_of(const struct px_message_construction_api *api)
{
    return index_of(api)->archive->memory;
}
static uint32_t heap_allocate(uint32_t heap, uint32_t size, const struct px_message_construction_api *api)
{
    const struct px_archive_index_api *index = index_of(api);
    const uint32_t alignment = index->alignment(index->user, heap);
    return index->allocate(index->user, heap, size, alignment, 1); /* 0012b0c8 */
}

void px_open_message_member(uint32_t resource, uint32_t member, uint32_t heap,
                            const struct px_message_construction_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    const struct px_archive_api *io = index_of(api)->archive;
    const uint32_t archive = m->load32(m->user, resource + UINT32_C(8));
    if (archive == 0) px_resource_abort(m, io);
    m->store16(m->user, resource + UINT32_C(0x24), 0);
    m->store32(m->user, resource + UINT32_C(0x20), 0);
    m->store16(m->user, resource + UINT32_C(0x26), 0);
    px_seek_archive_member(archive, member, 0, io);
    (void)px_archive_read(archive, 12, api->header_scratch, io);
    const uint32_t sections = m->load16(m->user, api->header_scratch);
    const uint32_t header = heap_allocate(heap, UINT32_C(0x0c) + sections * UINT32_C(4), api);
    if (header != 0) {
        for (uint32_t i = 0; i < 12; i += 4)
            m->store32(m->user, header + i, m->load32(m->user, api->header_scratch + i));
        (void)px_archive_read(archive, (uint32_t)m->load16(m->user, header) * UINT32_C(4),
                              header + UINT32_C(0x0c), io);
    }
    m->store32(m->user, resource + UINT32_C(0x18), header);
    m->store32(m->user, resource + UINT32_C(0x10), member);
    /* The section count is read through the header even if allocation failed. */
    const uint32_t count = m->load16(m->user, header);
    uint32_t language = m->load32(m->user, PX_MESSAGE_LANGUAGE_CELL);
    const uint32_t mode = m->load8(m->user, resource + UINT32_C(0x0c));
    if (count <= language) language = 0;
    m->store32(m->user, resource + UINT32_C(0x14), language);
    if (mode == 0) {
        m->store32(m->user, resource + UINT32_C(0x1c), 0);
        return;
    }
    const uint32_t largest = m->load32(m->user, header + UINT32_C(4));
    m->store32(m->user, resource + UINT32_C(0x1c), heap_allocate(heap, largest, api));
    const uint32_t selected = m->load32(m->user, resource + UINT32_C(0x14));
    const uint32_t current_header = m->load32(m->user, resource + UINT32_C(0x18));
    const uint32_t current_archive = m->load32(m->user, resource + UINT32_C(8));
    const uint32_t current_member = m->load32(m->user, resource + UINT32_C(0x10));
    const uint32_t offset = m->load32(m->user, current_header + selected * UINT32_C(4) + UINT32_C(0x0c));
    if (current_archive == 0) {
        m->store32(m->user, resource + UINT32_C(0x1c), current_header + offset);
        return;
    }
    /* Read the section's size word, seek back, then read the whole section. */
    px_seek_archive_member(current_archive, current_member, offset, io);
    (void)px_archive_read(current_archive, 4, m->load32(m->user, resource + UINT32_C(0x1c)), io);
    px_seek_archive_member(current_archive, current_member, offset, io);
    const uint32_t section = m->load32(m->user, resource + UINT32_C(0x1c));
    (void)px_archive_read(current_archive, m->load32(m->user, section), section, io);
}

typedef uint32_t (*archive_constructor)(void *user, uint32_t object, uint32_t heap, uint32_t id, uint32_t flags);
static uint32_t construct(uint32_t object, uint32_t archive_id, uint32_t member, uint32_t heap, uint32_t mode,
                          uint32_t flags, archive_constructor make, void *make_user,
                          const struct px_message_construction_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    m->store32(m->user, object + UINT32_C(8), 0);
    m->store32(m->user, object, PX_MESSAGE_RESOURCE_VTABLE);
    m->store32(m->user, object + UINT32_C(4), heap);
    m->store8(m->user, object + UINT32_C(0x0c), (uint8_t)mode);
    for (uint32_t offset = 0x10; offset <= 0x1c; offset += 4) m->store32(m->user, object + offset, 0);
    uint32_t archive = heap_allocate(heap, PX_ARCHIVE_OBJECT_SIZE, api);
    if (archive != 0) archive = make(make_user, archive, heap, archive_id, flags);
    m->store32(m->user, object + UINT32_C(8), archive);
    px_open_message_member(object, member, heap, api);
    return object;
}

uint32_t px_construct_message_resource(uint32_t object, uint32_t archive_id, uint32_t member, uint32_t heap,
                                       uint32_t mode, uint32_t flags, const struct px_message_construction_api *api)
{
    const struct px_archive_index_api *index = index_of(api);
    return construct(object, archive_id, member, heap, mode, flags, index->construct, index->user, api);
}

uint32_t px_construct_listed_message_resource(uint32_t object, uint32_t archive_id, uint32_t member, uint32_t heap,
                                              uint32_t mode, uint32_t flags,
                                              const struct px_message_construction_api *api)
{
    return construct(object, archive_id, member, heap, mode, flags, api->construct_listed, api->user, api);
}

uint32_t px_destroy_message_resource(uint32_t resource, const struct px_message_construction_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    const uint32_t archive = m->load32(m->user, resource + UINT32_C(8));
    m->store32(m->user, resource, PX_MESSAGE_RESOURCE_VTABLE);
    if (archive == 0) return resource;
    const uint32_t section = m->load32(m->user, resource + UINT32_C(0x1c));
    if (section != 0) api->release_block(api->user, section);
    const uint32_t header = m->load32(m->user, resource + UINT32_C(0x18));
    if (header != 0) api->release_block(api->user, header);
    const uint32_t current = m->load32(m->user, resource + UINT32_C(8));
    if (current != 0) api->destroy_archive(api->user, current);
    return resource;
}

void px_delete_message_resource(uint32_t resource, const struct px_message_construction_api *api)
{
    const struct px_archive_index_api *index = index_of(api);
    index->release(index->user, px_destroy_message_resource(resource, api)); /* 002ff520 */
}

uint32_t px_construct_move_record(uint32_t object, uint32_t heap, const struct px_message_construction_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    m->store32(m->user, object, UINT32_C(0x00598aac));
    m->store16(m->user, object + UINT32_C(4), 0);
    m->store32(m->user, object + UINT32_C(8), 0);
    const uint32_t payload = m->allocate(m->user, UINT32_C(0x24), heap); /* 00122920 */
    if (payload != 0)
        for (uint32_t i = 0; i < UINT32_C(0x24); i += 4) m->store32(m->user, payload + i, 0);
    m->store32(m->user, object + UINT32_C(8), payload);
    return object;
}

uint32_t px_construct_move_cache(uint32_t object, uint32_t heap, uint32_t capacity,
                                 const struct px_message_construction_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    m->store32(m->user, object + UINT32_C(4), 0);
    m->store32(m->user, object, UINT32_C(0x00598a9c));
    m->store8(m->user, object + UINT32_C(9), 0);
    m->store8(m->user, object + UINT32_C(8), (uint8_t)capacity);
    m->store32(m->user, object + UINT32_C(4), px_allocate_aligned(capacity * UINT32_C(4), heap, index_of(api)));
    for (uint32_t i = 0; i < capacity; ++i) {
        uint32_t slot = m->allocate(m->user, UINT32_C(0x0c), heap);
        if (slot != 0) slot = px_construct_move_record(slot, heap, api);
        m->store32(m->user, m->load32(m->user, object + UINT32_C(4)) + i * UINT32_C(4), slot);
    }
    return object;
}

void px_initialize_move_resources(uint32_t heap, const struct px_message_construction_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    const uint32_t base = PX_MOVE_RESOURCE_BASE;
    px_open_resource_archive(PX_MOVE_ARCHIVE_CELL, heap,
                             m->load32(m->user, m->load32(m->user, PX_RESOURCE_CONFIG_CELL) + UINT32_C(0x30)),
                             index_of(api)); /* 0011ee68 */
    uint32_t message = m->allocate(m->user, PX_MESSAGE_RESOURCE_SIZE, heap);
    if (message != 0) {
        const uint32_t table = m->load32(m->user, PX_RESOURCE_CONFIG_CELL);
        const uint32_t archive_id = m->load32(m->user, table + UINT32_C(0x44));
        const uint32_t member = m->load32(m->user, table + UINT32_C(0x54));
        message = px_construct_message_resource(message, archive_id, member, heap, 1, 5, api);
    }
    m->store32(m->user, base, message);
    uint32_t record = m->allocate(m->user, UINT32_C(0x0c), heap);
    if (record != 0) record = px_construct_move_record(record, heap, api);
    m->store32(m->user, base + UINT32_C(4), record);
    uint32_t cache = m->allocate(m->user, UINT32_C(0x0c), heap);
    if (cache != 0)
        cache = px_construct_move_cache(cache, heap,
                                        m->load32(m->user, m->load32(m->user, PX_RESOURCE_CONFIG_CELL) + UINT32_C(0x34))
                                            & UINT32_C(0xff), api);
    m->store32(m->user, base + UINT32_C(8), cache);
}
