#include "pokemon_resources.h"
#include <stddef.h>

static const struct px_pokemon_api *memory_of(const struct px_resource_api *api) { return api->index->archive->memory; }

/* 0015e914 / 00301a9c: compiler clear helpers, represented by byte stores. */
static void clear(uint32_t address, uint32_t size, const struct px_pokemon_api *m)
{
    for (uint32_t i = 0; i < size; ++i) m->store8(m->user, address + i, 0);
}

uint32_t px_garc_parser_count_offset(void) { return UINT32_C(0x24); }

void px_resource_abort(const struct px_pokemon_api *m, const struct px_archive_api *api)
{
    for (;;) {
        const uint32_t hook = m->load32(m->user, PX_ARCHIVE_PANIC_HOOK);
        if (hook != 0) api->panic(api->user, hook, PX_ARCHIVE_PANIC_MESSAGE);
    }
}

void px_archive_member_info_streamed(uint32_t archive, uint32_t index, uint32_t info[3],
                                     const struct px_archive_index_api *api)
{
    const struct px_pokemon_api *m = api->archive->memory;
    const uint32_t probe = px_create_garc_parser(m->load32(m->user, archive + UINT32_C(4)), api);
    const uint32_t length_offset = px_garc_parser_length_offset();
    if (probe != 0) px_destroy_garc_parser(probe, api);
    m->store32(m->user, api->scratch, 0);
    px_archive_read_at(archive, length_offset, 4, api->scratch, NULL, api->archive);
    const uint32_t heap = m->load32(m->user, archive + UINT32_C(4));
    const uint32_t length = m->load32(m->user, api->scratch);
    /* The negated alignment requests an allocation from the heap's tail. */
    const uint32_t buffer = api->allocate(api->user, heap, length, UINT32_C(0) - api->alignment(api->user, heap), 1);
    px_archive_read_at(archive, 0, length, buffer, NULL, api->archive);
    const uint32_t parser = px_create_garc_parser(m->load32(m->user, archive + UINT32_C(4)), api);
    px_garc_parser_parse(parser, buffer, m);
    if (px_garc_parser_count(parser, m) <= index) px_resource_abort(m, api->archive);
    info[2] = px_garc_parser_offset(parser, index, m->load32(m->user, PX_ARCHIVE_SELECTOR_CELL), m);
    info[0] = px_garc_parser_span(parser, index, m->load32(m->user, PX_ARCHIVE_SELECTOR_CELL), m);
    info[1] = px_garc_parser_length(parser, index, m->load32(m->user, PX_ARCHIVE_SELECTOR_CELL), m);
    if (parser != 0) px_destroy_garc_parser(parser, api);
    api->release(api->user, buffer);
}

uint32_t px_archive_member_count(uint32_t archive, const struct px_archive_index_api *api)
{
    const struct px_pokemon_api *m = api->archive->memory;
    if (m->load32(m->user, archive + UINT32_C(0x4c)) < UINT32_C(0x80000000))
        return m->load16(m->user, archive + UINT32_C(0x34));
    const uint32_t probe = px_create_garc_parser(m->load32(m->user, archive + UINT32_C(4)), api);
    const uint32_t count_offset = px_garc_parser_count_offset();
    if (probe != 0) px_destroy_garc_parser(probe, api);
    m->store16(m->user, api->scratch, 0);
    px_archive_read_at(archive, count_offset, 2, api->scratch, NULL, api->archive);
    return m->load16(m->user, api->scratch);
}

uint32_t px_archive_member_size(uint32_t archive, uint32_t index, const struct px_archive_api *api)
{
    uint32_t info[3];
    px_archive_member_info(archive, index, info, api);
    return info[0];
}

uint32_t px_allocate_aligned(uint32_t size, uint32_t heap, const struct px_archive_index_api *api)
{
    const uint32_t alignment = api->alignment(api->user, heap);
    return api->allocate(api->user, heap, size, alignment, 1);
}

static uint32_t allocate(uint32_t size, uint32_t heap, const struct px_resource_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    return m->allocate(m->user, size, heap); /* 00122920 */
}

uint32_t px_construct_personal_descriptor(uint32_t object, uint32_t heap, const struct px_resource_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    m->store32(m->user, object, UINT32_C(0x005989c0));
    m->store16(m->user, object + UINT32_C(4), 0);
    m->store8(m->user, object + UINT32_C(6), 0);
    m->store32(m->user, object + UINT32_C(8), 0);
    const uint32_t payload = allocate(PX_PERSONAL_RECORD_BYTES, heap, api);
    if (payload != 0)
        for (uint32_t i = 0; i < PX_PERSONAL_RECORD_BYTES; i += 4) m->store32(m->user, payload + i, 0);
    m->store32(m->user, object + UINT32_C(8), payload);
    return object;
}

uint32_t px_construct_growth_record(uint32_t object, const struct px_resource_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    m->store32(m->user, object, UINT32_C(0x00598a40));
    m->store32(m->user, object + UINT32_C(4), UINT32_C(0xffffffff));
    clear(object + UINT32_C(8), UINT32_C(0x194), m);
    return object;
}

uint32_t px_construct_learnset_record(uint32_t object, uint32_t heap, const struct px_resource_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    m->store16(m->user, object, 0);
    m->store8(m->user, object + UINT32_C(2), 0);
    m->store32(m->user, object + UINT32_C(4), 0);
    m->store8(m->user, object + UINT32_C(8), 0);
    const uint32_t payload = allocate(UINT32_C(0x68), heap, api);
    if (payload != 0) clear(payload, UINT32_C(0x68), m);
    m->store32(m->user, object + UINT32_C(4), payload);
    return object;
}

uint32_t px_construct_record_30(uint32_t object, uint32_t heap, const struct px_resource_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    m->store16(m->user, object + UINT32_C(4), 0);
    m->store32(m->user, object + UINT32_C(8), 0);
    m->store32(m->user, object, UINT32_C(0x005989d0));
    const uint32_t payload = allocate(UINT32_C(0x30), heap, api);
    if (payload != 0) clear(payload, UINT32_C(0x30), m);
    m->store32(m->user, object + UINT32_C(8), payload);
    return object;
}

uint32_t px_construct_record_18(uint32_t object, uint32_t heap, const struct px_resource_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    m->store32(m->user, object, UINT32_C(0x005989f0));
    m->store16(m->user, object + UINT32_C(4), 0);
    m->store32(m->user, object + UINT32_C(8), 0);
    const uint32_t payload = allocate(UINT32_C(0x18), heap, api);
    if (payload != 0)
        for (uint32_t i = 0; i < UINT32_C(0x18); i += 4) m->store32(m->user, payload + i, 0);
    m->store32(m->user, object + UINT32_C(8), payload);
    return object;
}

uint32_t px_construct_halfword_buffer(uint32_t object, uint32_t count, uint32_t heap, const struct px_resource_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    const struct px_archive_index_api *index = api->index;
    count &= UINT32_C(0xffff);
    m->store32(m->user, object, UINT32_C(0x00598570));
    m->store8(m->user, object + UINT32_C(0x0c), 0);
    m->store16(m->user, object + UINT32_C(0x0a), 0);
    m->store16(m->user, object + UINT32_C(8), (uint16_t)count);
    const uint32_t alignment = index->alignment(index->user, heap);
    const uint32_t buffer = index->allocate(index->user, heap, count * UINT32_C(2), alignment, 1);
    m->store32(m->user, object + UINT32_C(4), buffer);
    if (buffer != 0) {
        m->store16(m->user, buffer, 0);
        m->store8(m->user, object + UINT32_C(0x0c), 1);
    }
    return object;
}

typedef uint32_t (*slot_constructor)(uint32_t object, uint32_t heap, const struct px_resource_api *api);
static uint32_t growth_slot(uint32_t object, uint32_t heap, const struct px_resource_api *api)
{
    (void)heap;
    return px_construct_growth_record(object, api);
}

/* Shared slot loop: allocate each slot, construct it when nonzero, and store
 * it through a reloaded table pointer. The count is the full argument. */
static void fill_slots(uint32_t object, uint32_t heap, uint32_t count, uint32_t slot_size,
                       slot_constructor construct, const struct px_resource_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    m->store32(m->user, object + UINT32_C(4), px_allocate_aligned(count * UINT32_C(4), heap, api->index));
    for (uint32_t i = 0; i < count; ++i) {
        uint32_t slot = allocate(slot_size, heap, api);
        if (slot != 0) slot = construct(slot, heap, api);
        m->store32(m->user, m->load32(m->user, object + UINT32_C(4)) + i * UINT32_C(4), slot);
    }
}

uint32_t px_construct_growth_cache(uint32_t object, uint32_t heap, uint32_t capacity, const struct px_resource_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    m->store32(m->user, object + UINT32_C(4), 0);
    m->store32(m->user, object, UINT32_C(0x005989e0));
    m->store8(m->user, object + UINT32_C(8), (uint8_t)capacity);
    m->store8(m->user, object + UINT32_C(9), 0);
    fill_slots(object, heap, capacity, UINT32_C(0x19c), growth_slot, api);
    return object;
}

uint32_t px_construct_personal_cache(uint32_t object, uint32_t heap, uint32_t capacity, const struct px_resource_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    m->store32(m->user, object + UINT32_C(4), 0);
    m->store32(m->user, object, UINT32_C(0x00598a00));
    m->store8(m->user, object + UINT32_C(9), 0);
    m->store8(m->user, object + UINT32_C(8), (uint8_t)capacity);
    fill_slots(object, heap, capacity, UINT32_C(0x0c), px_construct_personal_descriptor, api);
    return object;
}

/* 0011eb84 / 0011ec00 / 0011ec7c: word capacity at +8 and word cursor at +0x0c. */
static uint32_t word_cache(uint32_t object, uint32_t heap, uint32_t capacity, uint32_t vtable,
                           slot_constructor construct, const struct px_resource_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    m->store32(m->user, object + UINT32_C(4), 0);
    m->store32(m->user, object, vtable);
    m->store32(m->user, object + UINT32_C(0x0c), 0);
    m->store32(m->user, object + UINT32_C(8), capacity);
    fill_slots(object, heap, capacity, UINT32_C(0x0c), construct, api);
    return object;
}

uint32_t px_construct_learnset_cache(uint32_t object, uint32_t heap, uint32_t capacity, const struct px_resource_api *api)
{
    return word_cache(object, heap, capacity, UINT32_C(0x00598a10), px_construct_learnset_record, api);
}

uint32_t px_construct_cache_30(uint32_t object, uint32_t heap, uint32_t capacity, const struct px_resource_api *api)
{
    return word_cache(object, heap, capacity, UINT32_C(0x00598a20), px_construct_record_30, api);
}

uint32_t px_construct_cache_18(uint32_t object, uint32_t heap, uint32_t capacity, const struct px_resource_api *api)
{
    return word_cache(object, heap, capacity, UINT32_C(0x00598a30), px_construct_record_18, api);
}

static uint32_t open_archive(uint32_t heap, uint32_t id, const struct px_resource_api *api)
{
    uint32_t archive = allocate(PX_ARCHIVE_OBJECT_SIZE, heap, api);
    if (archive != 0) archive = api->index->construct(api->index->user, archive, heap, id, 1);
    return archive;
}

void px_load_personal_table(uint32_t heap, uint32_t id, const struct px_resource_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    const struct px_archive_api *io = api->index->archive;
    if (m->load32(m->user, PX_PERSONAL_ARCHIVE_CELL) != 0) return;
    const uint32_t archive = open_archive(heap, id, api);
    m->store32(m->user, PX_PERSONAL_ARCHIVE_CELL, archive);
    const uint32_t count = px_archive_member_count(archive, api->index);
    const uint32_t last = count - UINT32_C(1);
    const uint32_t size = px_archive_member_size(m->load32(m->user, PX_PERSONAL_ARCHIVE_CELL), last, io);
    const uint32_t expected = (count << 6) - PX_PERSONAL_RECORD_BYTES;
    if (expected != size) px_resource_abort(m, io);
    const uint32_t table = api->allocate_block(api->user, heap, size);
    if (px_archive_member_size(m->load32(m->user, PX_PERSONAL_ARCHIVE_CELL), 0, io) != PX_PERSONAL_RECORD_BYTES)
        px_resource_abort(m, io);
    px_read_archive_member_part(m->load32(m->user, PX_PERSONAL_ARCHIVE_CELL), last, 0, expected, table, io);
    m->store32(m->user, PX_PERSONAL_ARCHIVE_CELL + UINT32_C(4), table);
}

void px_open_learnset_resources(uint32_t heap, uint32_t id, const struct px_resource_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    if (m->load32(m->user, PX_LEARNSET_RESOURCE_CELL + UINT32_C(4)) == 0) {
        const uint32_t archive = open_archive(heap, id, api);
        m->store32(m->user, PX_LEARNSET_RESOURCE_CELL + UINT32_C(4), archive);
        px_build_archive_index(archive, heap, api->index);
    }
    if (m->load32(m->user, PX_LEARNSET_RESOURCE_CELL) != 0) return;
    uint32_t descriptor = allocate(UINT32_C(0x0c), heap, api);
    if (descriptor != 0) descriptor = px_construct_personal_descriptor(descriptor, heap, api);
    m->store32(m->user, PX_LEARNSET_RESOURCE_CELL, descriptor);
}

static uint32_t config(uint32_t word, const struct px_resource_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    return m->load32(m->user, m->load32(m->user, PX_RESOURCE_CONFIG_CELL) + word * UINT32_C(4));
}

typedef uint32_t (*cache_constructor)(uint32_t object, uint32_t heap, uint32_t capacity, const struct px_resource_api *api);
static void create_object(uint32_t cell, uint32_t size, uint32_t heap, slot_constructor construct,
                          const struct px_resource_api *api)
{
    uint32_t object = allocate(size, heap, api);
    if (object != 0) object = construct(object, heap, api);
    memory_of(api)->store32(memory_of(api)->user, cell, object);
}
static void create_cache(uint32_t cell, uint32_t size, uint32_t heap, uint32_t word, uint32_t mask,
                         cache_constructor construct, const struct px_resource_api *api)
{
    uint32_t object = allocate(size, heap, api);
    if (object != 0) object = construct(object, heap, config(word, api) & mask, api);
    memory_of(api)->store32(memory_of(api)->user, cell, object);
}
static void create_message(uint32_t cell, uint32_t heap, uint32_t member_word, uint32_t mode,
                           const struct px_resource_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    uint32_t object = allocate(UINT32_C(0x28), heap, api);
    if (object != 0) {
        /* One configuration-pointer load serves both words. */
        const uint32_t table = m->load32(m->user, PX_RESOURCE_CONFIG_CELL);
        const uint32_t archive_id = m->load32(m->user, table + UINT32_C(0x44));
        const uint32_t member = m->load32(m->user, table + member_word * UINT32_C(4));
        object = api->construct_message(api->user, object, archive_id, member, heap, mode, 5);
    }
    memory_of(api)->store32(memory_of(api)->user, cell, object);
}

void px_initialize_pokemon_resources(uint32_t heap, const struct px_resource_api *api)
{
    const struct px_pokemon_api *m = memory_of(api);
    const struct px_archive_api *io = api->index->archive;
    const uint32_t base = PX_RESOURCE_BASE;
    px_load_personal_table(heap, config(0, api), api);
    px_open_learnset_resources(heap, config(1, api), api);
    px_open_resource_archive(PX_GROWTH_ARCHIVE_CELL, heap, config(2, api), api->index);  /* 0011ecf8 */
    px_open_resource_archive(PX_ARCHIVE_5E5288_CELL, heap, config(3, api), api->index); /* 0011e9dc */
    px_open_resource_archive(PX_ARCHIVE_5D1760_CELL, heap, config(4, api), api->index); /* 0011eab0 */
    uint32_t words = allocate(UINT32_C(0x10), heap, api);
    if (words != 0) words = px_construct_halfword_buffer(words, UINT32_C(0x100), heap, api);
    m->store32(m->user, base, words);
    create_message(base + UINT32_C(0x04), heap, 0x12, 1, api);
    create_message(base + UINT32_C(0x08), heap, 0x13, 0, api);
    create_message(base + UINT32_C(0x0c), heap, 0x14, 0, api);
    create_object(base + UINT32_C(0x14), UINT32_C(0x0c), heap, px_construct_personal_descriptor, api);
    create_cache(base + UINT32_C(0x10), UINT32_C(0x0c), heap, 7, UINT32_C(0xff), px_construct_personal_cache, api);
    create_object(base + UINT32_C(0x1c), UINT32_C(0x19c), heap, growth_slot, api);
    create_cache(base + UINT32_C(0x20), UINT32_C(0x0c), heap, 8, UINT32_C(0xffffffff), px_construct_growth_cache, api);
    create_object(base + UINT32_C(0x24), UINT32_C(0x0c), heap, px_construct_learnset_record, api);
    create_cache(base + UINT32_C(0x28), UINT32_C(0x10), heap, 9, UINT32_C(0xffffffff), px_construct_learnset_cache, api);
    create_object(base + UINT32_C(0x2c), UINT32_C(0x0c), heap, px_construct_record_30, api);
    create_cache(base + UINT32_C(0x30), UINT32_C(0x10), heap, 10, UINT32_C(0xffffffff), px_construct_cache_30, api);
    create_object(base + UINT32_C(0x38), UINT32_C(0x0c), heap, px_construct_record_18, api);
    create_cache(base + UINT32_C(0x3c), UINT32_C(0x10), heap, 11, UINT32_C(0xffffffff), px_construct_cache_18, api);
    create_object(base + UINT32_C(0x18), UINT32_C(0x0c), heap, px_construct_personal_descriptor, api);
    create_object(base + UINT32_C(0x34), UINT32_C(0x0c), heap, px_construct_record_18, api);
    uint32_t archive = allocate(PX_ARCHIVE_OBJECT_SIZE, heap, api);
    if (archive != 0) archive = api->index->construct(api->index->user, archive, heap, config(6, api), 1);
    m->store32(m->user, base + UINT32_C(0x40), archive);
    /* The final member is read whole into a block; the original's size
     * comparison with 0x5a4 has no effect. */
    const uint32_t last = px_archive_member_count(archive, api->index) - UINT32_C(1);
    const uint32_t size = px_archive_member_size(m->load32(m->user, base + UINT32_C(0x40)), last, io);
    m->store32(m->user, base + UINT32_C(0x44), api->allocate_block(api->user, heap, size));
    const uint32_t block = m->load32(m->user, base + UINT32_C(0x44));
    px_read_archive_member_part(m->load32(m->user, base + UINT32_C(0x40)), last, 0, size, block, io);
}
