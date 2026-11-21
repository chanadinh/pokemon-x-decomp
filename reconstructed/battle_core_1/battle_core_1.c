#include "battle_core_1.h"
#include "../battle_modifier/battle_modifier.h"

enum { ENTRY_COUNT = 0x108c, ENTRIES = 0x10bc, RING_LAST = 399 };

uint32_t px_battle_same_side(uint32_t a, uint32_t b)
{
    if (a == 0x1f || b == 0x1f) return 0;
    return (a >= 12) == (b >= 12);
}

uint32_t px_battle_pack_1(uint32_t value)
{
    return 1 | value << 9;
}

uint32_t px_battle_pack_3(uint32_t value)
{
    return (value << 3 & UINT32_C(0x1f8)) | 3;
}

uint32_t px_battle_has_type(uint32_t pokemon, uint32_t type, const struct px_pokemon_api *m)
{
    if (type == PX_BATTLE_NO_TYPE) return 0;
    const uint32_t flagged = (m->load32(m->user, pokemon + 0x7c) & 7) != 0;
    uint32_t first = m->load8(m->user, pokemon + 0x100);
    uint32_t second = PX_BATTLE_NO_TYPE;
    if (first == 2 && flagged) first = PX_BATTLE_NO_TYPE;
    const uint32_t other = m->load8(m->user, pokemon + 0x101);
    if (!(other == 2 && flagged)) second = other;
    if (first != PX_BATTLE_NO_TYPE) {
        if (second == PX_BATTLE_NO_TYPE) second = first;
    } else {
        first = second;
        if (first == PX_BATTLE_NO_TYPE) {
            second = 0;
            first = 0;
        }
    }
    if (first == type || second == type) return 1;
    return m->load8(m->user, pokemon + 0x102) == type;
}

uint32_t px_battle_pokemon_status(uint32_t pokemon, const struct px_pokemon_api *m)
{
    for (uint32_t i = 1; i <= 5; ++i)
        if (m->load32(m->user, pokemon + UINT32_C(0x1c) + 4 * i) & 7) return i;
    return 0;
}

uint32_t px_battle_find_entry(uint32_t array, uint32_t value, const struct px_pokemon_api *m)
{
    for (uint32_t i = 0; i < 10; ++i) {
        const uint32_t at = array + 3 * i;
        if (m->load8(m->user, at) != 0 && m->load8(m->user, at + 2) == value) return i;
    }
    return 10;
}

uint32_t px_battle_first_set_id(uint32_t context, uint32_t table, const struct px_pokemon_api *m)
{
    for (uint32_t i = 0; i < 4; ++i) {
        const uint32_t id = m->load8(m->user, table + i);
        if (m->load8(m->user, context + UINT32_C(0x169) + (id >> 3)) & (1u << (id & 7)) & 0xff) return id;
    }
    return PX_BATTLE_NO_TYPE;
}

uint32_t px_battle_byte_9(uint32_t pokemon, const struct px_pokemon_api *m)
{
    return m->load8(m->user, m->load32(m->user, pokemon + 0x1c) + 9);
}

uint32_t px_battle_bit_5_of_1b(uint32_t pokemon, const struct px_pokemon_api *m)
{
    return (m->load8(m->user, pokemon + 0x1b) & 0x20) >> 5;
}

uint32_t px_battle_field_16(uint32_t word_address, const struct px_pokemon_api *m)
{
    const uint32_t word = m->load32(m->user, word_address);
    if ((word & 0xf) != 1) return 0;
    return (word << 8) >> 16;
}

uint32_t px_battle_array_word(uint32_t array, uint32_t index, const struct px_pokemon_api *m)
{
    if (m->load8(m->user, array + 0x18) <= index) return 0;
    return m->load32(m->user, array + index * 4);
}

uint32_t px_battle_low_half(uint32_t address, const struct px_pokemon_api *m)
{
    return m->load32(m->user, address + 0x14) & UINT32_C(0xffff);
}

uint32_t px_battle_count_matches(uint32_t log, uint32_t id, uint32_t value, const struct px_pokemon_api *m)
{
    const uint32_t start = m->load32(m->user, log);
    uint32_t index = start, count = 0;
    for (;;) {
        index = index - 1;
        if ((int32_t)index < 0) index = RING_LAST;
        if (start == index) break;
        const uint32_t at = log + index * 8;
        const uint32_t kind = m->load16(m->user, at + 8);
        if (kind == 0) break;
        if (kind == id && m->load32(m->user, at + 4) == value) ++count;
    }
    return count;
}

uint32_t px_battle_bit_set(uint32_t context, uint32_t index, const struct px_pokemon_api *m)
{
    const uint32_t byte = m->load8(m->user, context + UINT32_C(0x166) + (index >> 3));
    return ((1u << (index & 7)) & 0xff & byte) != 0;
}

uint32_t px_battle_half_160(uint32_t context, const struct px_pokemon_api *m)
{
    return m->load16(m->user, context + 0x160);
}

uint32_t px_battle_half_15c(uint32_t context, const struct px_pokemon_api *m)
{
    return m->load16(m->user, context + 0x15c);
}

uint32_t px_battle_word_1080_set(uint32_t context, const struct px_pokemon_api *m)
{
    return m->load32(m->user, context + 0x1080) != 0;
}

uint32_t px_battle_first_kind_word(uint32_t context, uint32_t slot, uint32_t out, const struct px_pokemon_api *m)
{
    const uint32_t count = m->load8(m->user, context + ENTRY_COUNT);
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t entry = context + ENTRIES + i * 16;
        if (m->load8(m->user, m->load32(m->user, entry) + 0x19) != slot) continue;
        const uint32_t word = m->load32(m->user, entry + 4);
        if ((word & 0xf) == 6 || (word & 0xf) == 8) continue;
        m->store32(m->user, out, word);
        return 1;
    }
    return 0;
}

uint32_t px_battle_others_flagged(uint32_t context, uint32_t slot, const struct px_pokemon_api *m)
{
    const uint32_t count = m->load8(m->user, context + ENTRY_COUNT);
    uint32_t others = 0, flagged = 0;
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t entry = context + ENTRIES + i * 16;
        if (m->load8(m->user, m->load32(m->user, entry) + 0x19) == slot) continue;
        ++others;
        if (m->load8(m->user, entry + 0xd) != 0) ++flagged;
    }
    return others == flagged;
}

void px_battle_normalize_word(uint32_t address, const struct px_pokemon_api *m)
{
    uint32_t word = m->load32(m->user, address);
    word = word >> 10 << 10;
    const uint32_t field = (word << 13) >> 23;
    word &= ~UINT32_C(0xc000000);
    word &= ~UINT32_C(0x3f80000);
    word |= field << 19;
    word &= ~UINT32_C(0x70000000);
    m->store32(m->user, address, word);
}

void px_battle_set_kind_bit(uint32_t address, uint32_t bit, const struct px_pokemon_api *m)
{
    uint32_t word = m->load32(m->user, address);
    const uint32_t kind = word & 7;
    if (kind == 1 || kind == 3 || kind == 2) {
        m->store32(m->user, address, (word & ~UINT32_C(0x2000000)) | bit << 25);
    } else if (kind == 4) {
        m->store32(m->user, address, (word & ~UINT32_C(0x80000000)) | bit << 31);
    }
}

void px_battle_mark_if_small(uint32_t address, uint32_t value, const struct px_pokemon_api *m)
{
    uint32_t half = m->load16(m->user, address + 2);
    if ((uint32_t)(half << 18) >> 25 >= 8) return;
    half |= 0x4000;
    m->store32(m->user, address + 0x20, value);
    m->store16(m->user, address + 2, (uint16_t)half);
}

void px_battle_store_byte_db4(uint32_t context, uint32_t value, const struct px_pokemon_api *m)
{
    m->store8(m->user, m->load32(m->user, context + 0xdb4) + 4, (uint8_t)value);
}

void px_battle_nop(void)
{
}

uint32_t px_battle_event_0x83(uint32_t context, const struct px_battle_core_api *api)
{
    const struct px_battle_event_api *events = api->items->base->events;
    px_battle_event_push(events);
    px_battle_event_set_value(0x42, 0, events);
    px_battle_event_dispatch(context, 0x83, events);
    const uint32_t blocked = px_battle_event_get(0x42, events);
    px_battle_event_pop(events);
    if (blocked != 0) return 0;
    return events->memory->load8(events->memory->user, events->memory->load32(events->memory->user, context + 0x1c) + 1);
}

void px_battle_record_change(uint32_t unused, uint32_t context, uint32_t id, uint32_t value, uint32_t flag,
                             const struct px_battle_core_api *api)
{
    (void)unused;
    const struct px_battle_items_api *items = api->items;
    const struct px_pokemon_api *m = items->base->events->memory;
    void *user = items->base->events->user;
    const uint32_t battler = px_battle_pokemon_by_id(context, id, items->base);
    if (m->load8(m->user, battler + 0x153) == value) return;
    const uint32_t record = items->action_record(user, context, 0x3a, id);
    const uint32_t header = m->load32(m->user, record);
    m->store8(m->user, record + 5, (uint8_t)value);
    m->store8(m->user, record + 4, (uint8_t)id);
    m->store32(m->user, record, (header & ~UINT32_C(0x800000)) | ((flag << 23) & UINT32_C(0x800000)));
    items->message(user, record + 8, 2, 0xde);
    items->message_arg(user, record + 8, id);
    items->commit_actions(user, context, record);
}
