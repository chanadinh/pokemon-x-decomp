#include "pokemon_learnset.h"

#define SELECTED_LEARNSET UINT32_C(0x005d17b4)
#define LEARNSET_CACHE UINT32_C(0x005d17b8)
#define PERSONAL_RECORD UINT32_C(0x005d17e8)
#define LEARNSET_ARCHIVE UINT32_C(0x005d17ec)

uint32_t px_learnset_record_count(uint32_t record, const struct px_pokemon_api *m)
{
    return m->load8(m->user, record + UINT32_C(8)); /* 0016b1e0 */
}

uint32_t px_learnset_record_level(uint32_t record, uint32_t index, const struct px_pokemon_api *m)
{
    if (px_learnset_record_count(record, m) <= index) index = 0;
    const uint32_t buffer = m->load32(m->user, record + UINT32_C(4));
    return m->load16(m->user, buffer + UINT32_C(2) + index * UINT32_C(4)); /* 0016b1e8 */
}

uint32_t px_learnset_record_move(uint32_t record, uint32_t index, const struct px_pokemon_api *m)
{
    if (px_learnset_record_count(record, m) <= index) index = 0;
    const uint32_t buffer = m->load32(m->user, record + UINT32_C(4));
    return m->load16(m->user, buffer + index * UINT32_C(4)); /* 0016b224 */
}

uint32_t px_current_learnset_count(const struct px_pokemon_api *m)
{
    return px_learnset_record_count(m->load32(m->user, SELECTED_LEARNSET), m);
}
uint32_t px_current_learnset_level(uint32_t index, const struct px_pokemon_api *m)
{
    return px_learnset_record_level(m->load32(m->user, SELECTED_LEARNSET), index, m);
}
uint32_t px_current_learnset_move(uint32_t index, const struct px_pokemon_api *m)
{
    return px_learnset_record_move(m->load32(m->user, SELECTED_LEARNSET), index, m);
}

void px_copy_learnset_record(uint32_t source, uint32_t destination, const struct px_pokemon_api *m)
{
    const uint32_t from = m->load32(m->user, source + UINT32_C(4));
    const uint32_t to = m->load32(m->user, destination + UINT32_C(4));
    for (uint32_t i = 0; i < PX_LEARNSET_BUFFER_SIZE; ++i)
        m->store8(m->user, to + i, m->load8(m->user, from + i));
    m->store16(m->user, destination, m->load16(m->user, source));
    m->store8(m->user, destination + UINT32_C(2), m->load8(m->user, source + UINT32_C(2)));
    m->store8(m->user, destination + UINT32_C(8), m->load8(m->user, source + UINT32_C(8)));
}

uint32_t px_find_cached_learnset(uint32_t cache, uint32_t species, uint32_t form,
                                uint32_t destination, const struct px_pokemon_api *m)
{
    if (m->load32(m->user, cache + UINT32_C(8)) == 0) return 0;
    uint32_t index = 0;
    do {
        uint32_t table = m->load32(m->user, cache + UINT32_C(4));
        uint32_t record = m->load32(m->user, table + index * UINT32_C(4));
        if (m->load16(m->user, record) == species) {
            table = m->load32(m->user, cache + UINT32_C(4));
            record = m->load32(m->user, table + index * UINT32_C(4));
            if (m->load8(m->user, record + UINT32_C(2)) == form) {
                table = m->load32(m->user, cache + UINT32_C(4));
                record = m->load32(m->user, table + index * UINT32_C(4));
                px_copy_learnset_record(record, destination, m);
                return 1;
            }
        }
        ++index;
    } while (m->load32(m->user, cache + UINT32_C(8)) > index);
    return 0;
}

void px_store_cached_learnset(uint32_t cache, uint32_t source, const struct px_pokemon_api *m)
{
    const uint32_t species = m->load16(m->user, source);
    const uint32_t form = m->load8(m->user, source + UINT32_C(2));
    uint32_t index = 0;
    if (m->load32(m->user, cache + UINT32_C(8)) != 0) {
        do {
            uint32_t table = m->load32(m->user, cache + UINT32_C(4));
            uint32_t record = m->load32(m->user, table + index * UINT32_C(4));
            if (m->load16(m->user, record) == species) {
                table = m->load32(m->user, cache + UINT32_C(4));
                record = m->load32(m->user, table + index * UINT32_C(4));
                if (m->load8(m->user, record + UINT32_C(2)) == form) return;
            }
            ++index;
        } while (m->load32(m->user, cache + UINT32_C(8)) > index);
    }
    const uint32_t table = m->load32(m->user, cache + UINT32_C(4));
    const uint32_t cursor = m->load32(m->user, cache + UINT32_C(0x0c));
    const uint32_t destination = m->load32(m->user, table + cursor * UINT32_C(4));
    px_copy_learnset_record(source, destination, m);
    const uint32_t old_cursor = m->load32(m->user, cache + UINT32_C(0x0c));
    const uint32_t count = m->load32(m->user, cache + UINT32_C(8));
    m->store32(m->user, cache + UINT32_C(0x0c), (old_cursor + UINT32_C(1)) % count);
}

void px_load_learnset_record(uint32_t record, uint32_t species, uint32_t form, const struct px_learnset_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    if (m->load16(m->user, record) == species &&
        m->load8(m->user, record + UINT32_C(2)) == form) return;

    uint32_t index;
    if (species == UINT32_C(0x2d2) || species == UINT32_C(0x2d3) || species > UINT32_C(0x2d1)) {
        index = 0;
    } else {
        index = species;
        if (form != 0) {
            api->select_personal(api->user, m->load32(m->user, PERSONAL_RECORD), species, 0);
            const uint32_t first = api->personal_value(api->user, m->load32(m->user, PERSONAL_RECORD), UINT32_C(0x1e));
            const uint32_t count = api->personal_value(api->user, m->load32(m->user, PERSONAL_RECORD), UINT32_C(0x20));
            if (first != 0 && count > form) index = first + form - UINT32_C(1);
        }
    }
    /* ARM loads the destination before reloading the archive handle. */
    const uint32_t destination = m->load32(m->user, record + UINT32_C(4));
    const uint32_t archive = m->load32(m->user, LEARNSET_ARCHIVE);
    api->read_member(api->user, archive, index, destination);
    const uint32_t buffer = m->load32(m->user, record + UINT32_C(4));
    uint32_t count = 0;
    while (count < PX_LEARNSET_ENTRY_LIMIT) {
        if (m->load16(m->user, buffer + count * UINT32_C(4)) == UINT16_MAX &&
            m->load16(m->user, buffer + count * UINT32_C(4) + UINT32_C(2)) == UINT16_MAX) break;
        ++count;
    }
    m->store8(m->user, record + UINT32_C(8), (uint8_t)count);
    m->store16(m->user, record, (uint16_t)species);
    m->store8(m->user, record + UINT32_C(2), (uint8_t)form);
}

void px_select_pokemon_learnset(uint32_t species, uint32_t form, const struct px_learnset_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const uint32_t selected = m->load32(m->user, SELECTED_LEARNSET);
    const uint32_t cache = m->load32(m->user, LEARNSET_CACHE);
    if (px_find_cached_learnset(cache, species, form, selected, m)) return;
    px_load_learnset_record(selected, species, form, api);
    px_store_cached_learnset(m->load32(m->user, LEARNSET_CACHE), selected, m);
}
