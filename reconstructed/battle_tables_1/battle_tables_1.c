#include "battle_tables_1.h"

enum { FIRST = 0x60, NEXT = 4, OWNER = 0x10, FLAGS = 0x18, BYTE_3B = 0x3b };

uint32_t px_battle_table_stub(uint32_t count_out, uint32_t count_word, uint32_t entries,
                              const struct px_pokemon_api *memory)
{
    memory->store32(memory->user, count_out, count_word);
    return entries;
}

uint32_t px_battle_terrain_table(uint32_t count_out, uint32_t parameter,
                                 const struct px_battle_terrain_tables *tables,
                                 const struct px_pokemon_api *memory)
{
    if (parameter == 1) return px_battle_table_stub(count_out, 2, tables->grassy, memory);
    if (parameter == 2) return px_battle_table_stub(count_out, 3, tables->misty, memory);
    if (parameter == 3) return px_battle_table_stub(count_out, 3, tables->electric, memory);
    return px_battle_table_stub(count_out, 1, tables->fallback, memory);
}

uint32_t px_battle_handlers_unmark(uint32_t slot, uint32_t owner, const struct px_battle_handler_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    uint32_t found = 0;
    for (uint32_t handler = m->load32(m->user, api->battle + FIRST); handler != 0;
         handler = m->load32(m->user, handler + NEXT)) {
        if (m->load8(m->user, handler + BYTE_3B) != slot || m->load8(m->user, handler + OWNER) != owner) continue;
        found = 1;
        m->store32(m->user, handler + FLAGS, m->load32(m->user, handler + FLAGS) & ~UINT32_C(0x40000000));
    }
    return found;
}

void px_battle_handler_mark_26(uint32_t handler, const struct px_battle_handler_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    m->store32(m->user, handler + FLAGS, m->load32(m->user, handler + FLAGS) | UINT32_C(0x4000000));
}

uint32_t px_battle_register_side(uint32_t side, uint32_t id, uint32_t value, const struct px_battle_tables_api *api)
{
    const struct px_battle_handler_api *h = api->handler;
    const struct px_pokemon_api *m = h->memory;
    if (side >= 2) h->abort(h->user);
    if (id >= 0x11) h->abort(h->user);
    const uint32_t record = api->layers + (side * UINT32_C(17) + id) * UINT32_C(16);
    const uint32_t layers = m->load32(m->user, record + 0xc);
    for (uint32_t row = 0; row < 0x11; ++row) {
        const uint32_t at = api->side_table + row * UINT32_C(12);
        if (m->load8(m->user, at) != id) continue;
        if (layers != 0) {
            /* Another layer, up to the row's maximum. */
            if (m->load32(m->user, at + 8) <= layers) continue;
            const uint32_t existing = m->load32(m->user, record);
            m->store32(m->user, record + 0xc, layers + 1);
            return existing;
        }
        const uint32_t entries = h->entries(h->user, m->load32(m->user, at + 4), h->count_scratch);
        const uint32_t count = m->load32(m->user, h->count_scratch) & UINT32_C(0xffff);
        const uint32_t handler = px_battle_handler_register(2, id, 2, 0, side, entries, count, h);
        if (handler == 0) api->overflow(h->user);
        px_battle_handler_set_work(handler, 6, value, h);
        m->store32(m->user, record + 8, 0);
        m->store32(m->user, record + 0xc, 1);
        m->store32(m->user, record, handler);
        m->store32(m->user, record + 4, value);
        return handler;
    }
    return 0;
}

uint32_t px_battle_register_position(uint32_t id, uint32_t value, const struct px_battle_tables_api *api)
{
    const struct px_battle_handler_api *h = api->handler;
    const struct px_pokemon_api *m = h->memory;
    for (uint32_t row = 0; row < 11; ++row) {
        const uint32_t at = api->position_table + row * UINT32_C(8);
        if (m->load8(m->user, at) != id) continue;
        const uint32_t entries = api->position_entries(h->user, m->load32(m->user, at + 4), h->count_scratch, value);
        const uint32_t count = m->load32(m->user, h->count_scratch) & UINT32_C(0xffff);
        return px_battle_handler_register(3, id, 3, 0, value, entries, count, h);
    }
    return 0;
}

void px_battle_register_item_checked(uint32_t pokemon, const struct px_battle_tables_api *api)
{
    const struct px_battle_handler_api *h = api->handler;
    const struct px_pokemon_api *m = h->memory;
    if (px_battle_handlers_unmark(m->load8(m->user, pokemon + UINT32_C(0x19)), 5, h) != 0) return;
    const uint32_t item = m->load16(m->user, pokemon + UINT32_C(0x12));
    if (item == 0) return;
    for (uint32_t row = 0; row < 0xb6; ++row) {
        const uint32_t at = h->item_table + row * UINT32_C(8);
        if (m->load32(m->user, at) != item) continue;
        const uint32_t speed = h->raw_value(h->user, pokemon, 0xc) & UINT32_C(0xffff);
        const uint32_t id = m->load8(m->user, pokemon + UINT32_C(0x19));
        const uint32_t entries = h->entries(h->user, m->load32(m->user, at + 4), h->count_scratch);
        const uint32_t count = m->load32(m->user, h->count_scratch) & UINT32_C(0xffff);
        px_battle_handler_register(5, item, 6, speed, id, entries, count, h);
        return;
    }
}

uint32_t px_battle_register_item_given(uint32_t pokemon, uint32_t item, const struct px_battle_tables_api *api)
{
    const struct px_battle_handler_api *h = api->handler;
    const struct px_pokemon_api *m = h->memory;
    if (item == 0) return 0;
    for (uint32_t row = 0; row < 0xb6; ++row) {
        const uint32_t at = h->item_table + row * UINT32_C(8);
        if (m->load32(m->user, at) != item) continue;
        const uint32_t speed = h->raw_value(h->user, pokemon, 0xc) & UINT32_C(0xffff);
        const uint32_t id = m->load8(m->user, pokemon + UINT32_C(0x19));
        const uint32_t entries = h->entries(h->user, m->load32(m->user, at + 4), h->count_scratch);
        const uint32_t count = m->load32(m->user, h->count_scratch) & UINT32_C(0xffff);
        const uint32_t handler = px_battle_handler_register(5, item, 6, speed, id, entries, count, h);
        if (handler == 0) return 0;
        px_battle_handler_mark_26(handler, h);
        px_battle_handler_set_work(handler, 6, 1, h);
        return handler;
    }
    return 0;
}
