#include "battle_core_2.h"

enum { FLAG_BIT = 0x2000000, RING_LAST = 399 };

uint32_t px_battle_is_kind1_0x78(uint32_t word)
{
    return (word & 7) == 1 && (word & 0x1f8) == 0x78;
}

uint32_t px_battle_pack_4(uint32_t high, uint32_t low)
{
    return (high << 9 & UINT32_C(0x7e00)) | (low << 3 & UINT32_C(0x1f8)) | 4;
}

uint32_t px_battle_double_plus(uint32_t flag, uint32_t value)
{
    if (flag & 1) return (1 + (value << 1)) & 0xff;
    return (value << 1) & 0xff;
}

uint32_t px_battle_four_halves_match(uint32_t base, uint32_t value, const struct px_pokemon_api *m)
{
    const uint32_t at = base + 0x100;
    return m->load16(m->user, at + 0x1c) == value || m->load16(m->user, at + 0x2a) == value ||
           m->load16(m->user, at + 0x38) == value || m->load16(m->user, at + 0x46) == value;
}

uint32_t px_battle_group_slot(uint32_t base, uint32_t group, uint32_t index, const struct px_pokemon_api *m)
{
    if (group >= 4) return 0x1f;
    const uint32_t at = base + 49 * group;
    if (index >= m->load8(m->user, at)) return 0x1f;
    return m->load8(m->user, at + index + 0x19);
}

uint32_t px_battle_default_byte(uint32_t context, uint32_t index, const struct px_pokemon_api *m)
{
    const uint32_t area = context + 0x1244;
    const uint32_t pointer = m->load32(m->user, area);
    if (pointer != 0 && m->load32(m->user, area + 4) > index) return m->load8(m->user, pointer + index);
    return m->load8(m->user, area + 8);
}

uint32_t px_battle_has_entry_120c(uint32_t context, uint32_t value, const struct px_pokemon_api *m)
{
    return px_battle_find_entry(context + 0x120c, value, m) != 10;
}

uint32_t px_battle_has_set_id(uint32_t context, uint32_t table, const struct px_pokemon_api *m)
{
    return px_battle_first_set_id(context, table, m) != PX_BATTLE_NO_TYPE;
}

uint32_t px_battle_half_in_array(uint32_t value, uint32_t array, uint32_t count, const struct px_pokemon_api *m)
{
    for (uint32_t i = 0; i < count; ++i)
        if (m->load16(m->user, array + 2 * i) == value) return 1;
    return 0;
}

uint32_t px_battle_log_contains(uint32_t log, uint32_t id, uint32_t value, const struct px_pokemon_api *m)
{
    const uint32_t start = m->load32(m->user, log);
    uint32_t index = start;
    for (;;) {
        index = index - 1;
        if ((int32_t)index < 0) index = RING_LAST;
        if (start == index) return 0;
        const uint32_t at = log + index * 8;
        const uint32_t kind = m->load16(m->user, at + 8);
        if (kind == 0) return 0;
        if (kind == id && m->load32(m->user, at + 4) == value) return 1;
    }
}

uint32_t px_battle_find_word_index(uint32_t array, uint32_t value, const struct px_pokemon_api *m)
{
    const uint32_t count = m->load8(m->user, array + 0x18);
    for (uint32_t i = 0; i < count; ++i)
        if (m->load32(m->user, array + 4 * i) == value) return i;
    return UINT32_C(0xffffffff);
}

uint32_t px_battle_side_record_set(uint32_t unused, uint32_t side, uint32_t id, const struct px_battle_tables_api *api)
{
    (void)unused;
    const struct px_pokemon_api *m = api->handler->memory;
    return m->load32(m->user, api->layers + (side * UINT32_C(17) + id) * UINT32_C(16)) != 0;
}

void px_battle_set_field_3(uint32_t address, uint32_t value, const struct px_pokemon_api *m)
{
    uint32_t word = m->load32(m->user, address);
    const uint32_t field = (value << 3) & UINT32_C(0x1f8);
    const uint32_t kind = word & 7;
    if ((kind == 2 || kind == 4) && (word << 23) >> 26 < 8) {
        m->store32(m->user, address, (word & ~UINT32_C(0x1f8)) | field);
    }
}

void px_battle_set_field_9(uint32_t address, uint32_t value, const struct px_pokemon_api *m)
{
    const uint32_t word = m->load32(m->user, address);
    const uint32_t kind = word & 7;
    if (kind == 1 || kind == 2 || kind == 3) {
        m->store32(m->user, address, (word & ~UINT32_C(0x1fffe00)) | value << 9);
    } else if (kind == 4) {
        m->store32(m->user, address, (word & UINT32_C(0x80007fff)) | value << 15);
    }
}

void px_battle_clear_slot_tables(uint32_t base, uint32_t index, const struct px_pokemon_api *m)
{
    const uint32_t word = base + index * 4;
    m->store32(m->user, word + 0xc, 0);
    m->store32(m->user, word + 0x38, 0);
    m->store32(m->user, word + 0x64, 0);
    m->store32(m->user, word + 0x248, 0);
    m->store32(m->user, word + 0x274, 0);
    const uint32_t group = base + 40 * index;
    for (uint32_t i = 0; i < 10; ++i) m->store32(m->user, group + 0x90 + 4 * i, 0x1f);
}

void px_battle_clear_entries(uint32_t base, uint32_t first, const struct px_pokemon_api *m)
{
    if (first >= 6) {
        m->store32(m->user, base + first * 4 + 0x1c, 0);
        m->store8(m->user, base + first + 0xb4, 0);
        return;
    }
    for (uint32_t i = 1; i < 6; ++i) {
        m->store32(m->user, base + i * 4 + 0x1c, 0);
        m->store8(m->user, base + i + 0xb4, 0);
        if (i == 2) {
            m->store32(m->user, base + 0x40, 0);
            m->store8(m->user, base + 0xbd, 0);
        }
    }
}

void px_battle_clear_tables_10(uint32_t base, const struct px_pokemon_api *m)
{
    for (uint32_t i = 0; i < 10; ++i) {
        m->store32(m->user, base + i * 4, 0);
        m->store16(m->user, base + 0x28 + 2 * i, 0);
        m->store16(m->user, base + 0x3c + 2 * i, 0);
        m->store8(m->user, base + 0x50 + i, 0);
        m->store16(m->user, base + 0x5a + 2 * i, 0);
    }
    m->store8(m->user, base + 0x6e, 0);
    m->store8(m->user, base + 0x6f, 0);
    m->store8(m->user, base + 0x70, 0);
    m->store8(m->user, base + 0x71, 0);
}

void px_battle_push_log(uint32_t context, uint32_t first, uint32_t second, const struct px_pokemon_api *m)
{
    const uint32_t found = px_battle_find_entry(context + 0x120c, second, m);
    const uint32_t log = m->load32(m->user, context + 0xdc0);
    const uint32_t count = m->load8(m->user, log);
    if (count >= 10) return;
    m->store8(m->user, log + 1 + count, (uint8_t)first);
    m->store8(m->user, log + 0x15 + m->load8(m->user, log), (uint8_t)second);
    m->store8(m->user, log + 0xb + m->load8(m->user, log), (uint8_t)found);
    m->store8(m->user, log, (uint8_t)(m->load8(m->user, log) + 1));
}

static uint32_t side_handler(uint32_t side, uint32_t id, const struct px_battle_tables_api *api)
{
    const struct px_pokemon_api *m = api->handler->memory;
    return m->load32(m->user, api->layers + (side * UINT32_C(17) + id) * UINT32_C(16));
}

uint32_t px_battle_side_flag_clear(uint32_t side, uint32_t id, const struct px_battle_tables_api *api)
{
    const struct px_pokemon_api *m = api->handler->memory;
    const uint32_t handler = side_handler(side, id, api);
    if (handler == 0) return 0;
    const uint32_t flags = m->load32(m->user, handler + 0x18);
    if (!(flags & FLAG_BIT)) return 0;
    m->store32(m->user, handler + 0x18, flags & ~(uint32_t)FLAG_BIT);
    return 1;
}

uint32_t px_battle_side_flag_set(uint32_t side, uint32_t id, const struct px_battle_tables_api *api)
{
    const struct px_pokemon_api *m = api->handler->memory;
    const uint32_t handler = side_handler(side, id, api);
    if (handler == 0) return 0;
    const uint32_t flags = m->load32(m->user, handler + 0x18);
    if (flags & FLAG_BIT) return 0;
    m->store32(m->user, handler + 0x18, flags | FLAG_BIT);
    return 1;
}

void px_battle_remove_move_handlers(uint32_t pokemon, const struct px_battle_handler_api *api)
{
    const uint32_t slot = api->memory->load8(api->memory->user, pokemon + 0x19);
    for (;;) {
        const uint32_t handler = px_battle_handler_find(0, slot, api);
        if (handler == 0) return;
        px_battle_handler_remove(handler, api);
    }
}
