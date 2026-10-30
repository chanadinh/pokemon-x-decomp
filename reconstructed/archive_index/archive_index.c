#include "archive_index.h"
#include <stddef.h>

/* ARM register shift: only the low byte counts, and 32 or more yields zero. */
static uint32_t bit(uint32_t amount)
{
    amount &= UINT32_C(0xff);
    return amount < 32 ? UINT32_C(1) << amount : 0;
}

/* Record slot for a selector: the number of set bitmap bits below it, with
 * 16-bit counters, or zero when the selector's own bit is clear. */
static uint32_t record_slot(uint32_t bits, uint32_t selector)
{
    if ((bits & bit(selector)) == 0) return 0;
    uint32_t slot = 0;
    for (uint32_t i = 0; i < selector; i = (i + 1) & UINT32_C(0xffff))
        if (bits & bit(i)) slot = (slot + 1) & UINT32_C(0xffff);
    return slot;
}

/* Shared prefix of the three record readers: FATO then FATB pointers, the
 * entry's FATB offset, then its bitmap word. Returns the record array. */
static uint32_t entry_records(uint32_t parser, uint32_t entry, uint32_t *bits,
                              const struct px_pokemon_api *m)
{
    entry &= UINT32_C(0xffff);
    const uint32_t fato = m->load32(m->user, parser + UINT32_C(8));
    const uint32_t fatb = m->load32(m->user, parser + UINT32_C(0x0c));
    const uint32_t offset = m->load32(m->user, fato + UINT32_C(0x0c) + entry * UINT32_C(4));
    *bits = m->load32(m->user, fatb + offset + UINT32_C(0x0c));
    return fatb + offset + UINT32_C(0x10);
}

void px_garc_parser_parse(uint32_t parser, uint32_t buffer, const struct px_pokemon_api *m)
{
    const uint32_t fato = buffer + UINT32_C(0x1c);
    m->store32(m->user, parser + UINT32_C(4), buffer);
    m->store32(m->user, parser + UINT32_C(8), fato);
    const uint32_t entries = m->load16(m->user, buffer + UINT32_C(0x24));
    const uint32_t fatb = fato + UINT32_C(0x0c) + entries * UINT32_C(4);
    m->store32(m->user, parser + UINT32_C(0x0c), fatb);
    const uint32_t records = m->load32(m->user, fatb + UINT32_C(8));
    m->store32(m->user, parser + UINT32_C(0x10),
               entries * UINT32_C(4) + records * UINT32_C(12) + UINT32_C(0x0c) + fatb);
}

uint32_t px_garc_parser_count(uint32_t parser, const struct px_pokemon_api *m)
{
    return m->load16(m->user, m->load32(m->user, parser + UINT32_C(8)) + UINT32_C(8));
}

uint32_t px_garc_parser_largest(uint32_t parser, const struct px_pokemon_api *m)
{
    return m->load32(m->user, m->load32(m->user, parser + UINT32_C(4)) + UINT32_C(0x18));
}

uint32_t px_garc_parser_span(uint32_t parser, uint32_t entry, uint32_t selector, const struct px_pokemon_api *m)
{
    /* The slot is computed twice in the original; both use the same bitmap. */
    uint32_t bits;
    const uint32_t records = entry_records(parser, entry, &bits, m);
    const uint32_t start = m->load32(m->user, records + record_slot(bits, selector) * UINT32_C(12));
    const uint32_t end = m->load32(m->user, records + record_slot(bits, selector) * UINT32_C(12) + UINT32_C(4));
    return end - start;
}

uint32_t px_garc_parser_length(uint32_t parser, uint32_t entry, uint32_t selector, const struct px_pokemon_api *m)
{
    uint32_t bits;
    const uint32_t records = entry_records(parser, entry, &bits, m);
    return m->load32(m->user, records + record_slot(bits, selector) * UINT32_C(12) + UINT32_C(8));
}

uint32_t px_garc_parser_offset(uint32_t parser, uint32_t entry, uint32_t selector, const struct px_pokemon_api *m)
{
    uint32_t bits;
    const uint32_t records = entry_records(parser, entry, &bits, m);
    const uint32_t slot = record_slot(bits, selector);
    const uint32_t header = m->load32(m->user, parser + UINT32_C(4));
    const uint32_t start = m->load32(m->user, records + slot * UINT32_C(12));
    return m->load32(m->user, header + UINT32_C(0x10)) + start;
}

uint32_t px_garc_parser_length_offset(void) { return UINT32_C(0x10); }

uint32_t px_create_garc_parser(uint32_t heap, const struct px_archive_index_api *api)
{
    const struct px_pokemon_api *m = api->archive->memory;
    const uint32_t alignment = api->alignment(api->user, heap);
    const uint32_t parser = api->allocate(api->user, heap, PX_GARC_PARSER_SIZE, alignment, 1);
    if (parser != 0) {
        m->store32(m->user, parser, PX_GARC_PARSER_VTABLE);
        for (uint32_t i = 4; i < PX_GARC_PARSER_SIZE; i += 4) m->store32(m->user, parser + i, 0);
    }
    return parser;
}

void px_destroy_garc_parser(uint32_t parser, const struct px_archive_index_api *api)
{
    const struct px_pokemon_api *m = api->archive->memory;
    m->store32(m->user, parser, PX_GARC_PARSER_VTABLE);
    api->release(api->user, parser);
}

void px_build_archive_index(uint32_t archive, uint32_t heap, const struct px_archive_index_api *api)
{
    const struct px_pokemon_api *m = api->archive->memory;
    if (!api->try_enter(api->user, archive + UINT32_C(0x6c))) return;
    const uint32_t temporary = api->temporary_heap(api->user, m->load32(m->user, archive + UINT32_C(4)));
    const uint32_t probe = px_create_garc_parser(m->load32(m->user, archive + UINT32_C(4)), api);
    const uint32_t length_offset = px_garc_parser_length_offset();
    if (probe != 0) px_destroy_garc_parser(probe, api);
    m->store32(m->user, api->scratch, 0);
    px_archive_read_at(archive, length_offset, 4, api->scratch, NULL, api->archive);
    const uint32_t length = m->load32(m->user, api->scratch);
    const uint32_t buffer = api->allocate(api->user, temporary, length,
                                          api->alignment(api->user, temporary), 1);
    px_archive_read_at(archive, 0, length, buffer, NULL, api->archive);
    const uint32_t parser = px_create_garc_parser(temporary, api);
    px_garc_parser_parse(parser, buffer, m);
    const uint32_t previous = m->load32(m->user, archive + UINT32_C(0x3c));
    if (previous != 0) {
        api->release(api->user, previous);
        m->store32(m->user, archive + UINT32_C(0x3c), 0);
    }
    const uint32_t count = px_garc_parser_count(parser, m);
    const uint32_t table = api->allocate(api->user, heap, count * UINT32_C(12),
                                         api->alignment(api->user, heap), 1);
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t row = table + i * UINT32_C(12);
        m->store32(m->user, row, px_garc_parser_span(parser, i, m->load32(m->user, PX_ARCHIVE_SELECTOR_CELL), m));
        m->store32(m->user, row + UINT32_C(4),
                   px_garc_parser_length(parser, i, m->load32(m->user, PX_ARCHIVE_SELECTOR_CELL), m));
        m->store32(m->user, row + UINT32_C(8),
                   px_garc_parser_offset(parser, i, m->load32(m->user, PX_ARCHIVE_SELECTOR_CELL), m));
    }
    m->store32(m->user, archive + UINT32_C(0x3c), table);
    m->store16(m->user, archive + UINT32_C(0x34), (uint16_t)px_garc_parser_count(parser, m));
    m->store32(m->user, archive + UINT32_C(0x38), px_garc_parser_largest(parser, m));
    if (parser != 0) px_destroy_garc_parser(parser, api);
    api->release(api->user, buffer);
    api->signal(api->user, archive + UINT32_C(0x4c));
}

void px_read_archive_member_part(uint32_t archive, uint32_t index, uint32_t offset, uint32_t size,
                                 uint32_t buffer, const struct px_archive_api *api)
{
    uint32_t info[3];
    px_archive_member_info(archive, index, info, api);
    px_archive_read_at(archive, info[2] + offset, size, buffer, NULL, api);
}

void px_open_resource_archive(uint32_t cell, uint32_t heap, uint32_t id, const struct px_archive_index_api *api)
{
    const struct px_pokemon_api *m = api->archive->memory;
    if (m->load32(m->user, cell) != 0) return;
    uint32_t archive = m->allocate(m->user, PX_ARCHIVE_OBJECT_SIZE, heap);
    if (archive != 0) archive = api->construct(api->user, archive, heap, id, 1);
    m->store32(m->user, cell, archive);
    px_build_archive_index(archive, heap, api);
}
