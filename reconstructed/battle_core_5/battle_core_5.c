#include "battle_core_5.h"

static const struct px_pokemon_api *mem(const struct px_battle_core_5_api *api)
{
    return api->core4->handler->memory;
}

static void *user(const struct px_battle_core_5_api *api)
{
    return api->core4->handler->user;
}

static void bad(const struct px_battle_core_5_api *api)
{
    api->core4->handler->abort(user(api));
}

uint32_t px_battle_random(uint32_t limit, const struct px_battle_core_5_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    return api->random(user(api), m->load32(m->user, api->battle + 0x54), limit);
}

uint32_t px_battle_item_value(uint32_t item, uint32_t param, const struct px_battle_core_5_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    return api->item_value(user(api), m->load32(m->user, api->battle + 0x5c), item, param & 0xff);
}

uint32_t px_battle_chance(uint32_t percent, const struct px_battle_core_5_api *api)
{
    return px_battle_random(100, api) < percent;
}

uint32_t px_battle_ring_length(uint32_t context, uint32_t back, const struct px_battle_core_5_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    if (back >= 3) {
        bad(api);
        return 0;
    }
    int32_t position = (int32_t)m->load8(m->user, context + 0x205) - (int32_t)back;
    if (position < 0) position += 3;
    return m->load8(m->user, context + (uint32_t)position + 0x202);
}

uint32_t px_battle_group_count(uint32_t context, uint32_t group, const struct px_battle_core_5_api *api)
{
    if (group >= 4) {
        bad(api);
        return 0;
    }
    return mem(api)->load8(mem(api)->user, context + 49 * group);
}

uint32_t px_battle_entry_flag_121(uint32_t context, uint32_t index, const struct px_battle_core_5_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    if (!(m->load8(m->user, context + 0x152) > index)) bad(api);
    return (m->load8(m->user, context + index * 14 + 0x121) & 0xf) != 0;
}

uint32_t px_battle_entry_83c(uint32_t object, uint32_t index, const struct px_battle_core_5_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    const uint32_t inner = m->load32(m->user, object + 4);
    if (index >= 4) {
        bad(api);
        return 0;
    }
    return m->load32(m->user, inner + index * 56 + 0x83c) != 0;
}

uint32_t px_battle_byte_or_6(uint32_t object, uint32_t index, const struct px_battle_core_5_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    if (m->load8(m->user, object + 0x19) > index) return m->load8(m->user, object + index);
    return 6;
}

uint32_t px_battle_condition_word(uint32_t battler, uint32_t index, const struct px_battle_core_5_api *api)
{
    if (index >= 0x26) bad(api);
    return mem(api)->load32(mem(api)->user, battler + index * 4 + 0x1c);
}

uint32_t px_battle_byte_7ec(uint32_t context, uint32_t index, const struct px_battle_core_5_api *api)
{
    if (index >= 10) bad(api);
    return mem(api)->load8(mem(api)->user, context + index + 0x7ec);
}

uint32_t px_battle_status_is_0x78(uint32_t battler, const struct px_battle_core_5_api *api)
{
    return px_battle_is_kind1_0x78(mem(api)->load32(mem(api)->user, battler + 0x30));
}

uint32_t px_battle_slot_half(uint32_t object, uint32_t index, const struct px_battle_core_5_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    const uint32_t base = object + index * 56;
    if (m->load32(m->user, base + 0x838) != 0) return 0;
    return m->load16(m->user, base + 0x84c);
}

uint32_t px_battle_next_pair(uint32_t context, const struct px_battle_core_5_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    const uint32_t next = (m->load8(m->user, context + 0x815) + 2) & 3;
    return px_battle_pair_search(context, next, m) != 0 ? next : 4;
}

void px_battle_clear_18(uint32_t address, const struct px_battle_core_5_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    m->store8(m->user, address + 0x18, 0);
    m->store8(m->user, address + 0x19, 0);
    for (uint32_t offset = 0; offset <= 0x14; offset += 4) m->store32(m->user, address + offset, 0);
}

void px_battle_clear_10(uint32_t address, const struct px_battle_core_5_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    m->store32(m->user, address, 0);
    m->store32(m->user, address + 4, 0);
    m->store16(m->user, address + 8, 0);
}

void px_battle_fill_10(uint32_t address, uint32_t value, const struct px_battle_core_5_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    uint32_t word = value & 0xff;
    word |= word << 8;
    word |= word << 16;
    m->store32(m->user, address, word);
    m->store32(m->user, address + 4, word);
    m->store16(m->user, address + 8, (uint16_t)word);
}

void px_battle_copy_7(uint32_t destination, uint32_t source, const struct px_battle_core_5_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    m->store32(m->user, destination, m->load32(m->user, source));
    m->store16(m->user, destination + 4, m->load16(m->user, source + 4));
    m->store8(m->user, destination + 6, m->load8(m->user, source + 6));
}

uint32_t px_battle_find_matching_entry(uint32_t unused, uint32_t table, uint32_t entry, const struct px_battle_core_5_api *api)
{
    (void)unused;
    const struct px_pokemon_api *m = mem(api);
    if (!(m->load8(m->user, entry + 0x1b) & 0x40)) return entry;
    const uint32_t group = px_battle_slot_group(m->load8(m->user, entry + 0x19), api->core4->handler);
    uint32_t key = m->load32(m->user, entry + 4);
    if (key == 0 || !(m->load8(m->user, entry + 0x1b) & 0x40)) key = m->load32(m->user, entry);
    if (group >= 4) bad(api);
    const uint32_t list = table + group * 28 + 4;
    const uint32_t count = m->load8(m->user, list + 0x18);
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t candidate = m->load32(m->user, list + i * 4);
        if (m->load32(m->user, candidate) == key) return candidate;
    }
    return entry;
}

void px_battle_add_unique(uint32_t list, uint32_t value, const struct px_battle_core_5_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    const uint32_t count = m->load8(m->user, list + 0x6e);
    if (count >= 10) {
        bad(api);
        return;
    }
    for (uint32_t i = 0; i < count; ++i)
        if (m->load32(m->user, list + i * 4) == value) return;
    m->store32(m->user, list + count * 4, value);
    m->store8(m->user, list + 0x50 + m->load8(m->user, list + 0x6e), 0);
    m->store16(m->user, list + 0x28 + 2 * m->load8(m->user, list + 0x6e), 0);
    m->store16(m->user, list + 0x3c + 2 * m->load8(m->user, list + 0x6e), 0);
    const uint32_t next = (m->load8(m->user, list + 0x6e) + 1) & 0xff;
    m->store8(m->user, list + 0x6e, (uint8_t)next);
    if (next > m->load8(m->user, list + 0x6f)) m->store8(m->user, list + 0x6f, (uint8_t)next);
}

uint32_t px_battle_add_unique_slot(uint32_t context, uint32_t index, uint32_t value, const struct px_battle_core_5_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    if (index >= 11) bad(api);
    if (index >= 11) {
        bad(api);
        return 0;
    }
    if (m->load32(m->user, context + index * 4 + 0x274) == 0) return 0;
    const uint32_t count = m->load32(m->user, context + index * 4 + 0x248);
    if (count >= 10) return 0;
    const uint32_t base = context + index * 40;
    for (uint32_t i = 0; i < count; ++i)
        if (m->load32(m->user, base + 0x90 + i * 4) == value) return 0;
    m->store32(m->user, base + 0x90 + count * 4, value);
    m->store32(m->user, context + index * 4 + 0x248, count + 1);
    return 1;
}

static uint32_t battler_for(uint32_t context, uint32_t id, const struct px_battle_core_5_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    const uint32_t table = m->load32(m->user, context + 8);
    if (id >= 0x18) bad(api);
    if (m->load32(m->user, table + id * 4 + 0x84) == 0) bad(api);
    return m->load32(m->user, table + id * 4 + 0x84);
}

uint32_t px_battle_scaled_hp(uint32_t context, uint32_t id, const struct px_battle_core_5_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    const struct px_battle_event_api *e = api->core4->core->items->base->events;
    const uint32_t battler = battler_for(context, id, api);
    px_battle_event_push(e);
    px_battle_event_set_id(2, m->load8(m->user, battler + 0x19), e);
    px_battle_event_set_multiplier(0x36, 0x1000, 0x19a, 0x20000, e);
    px_battle_event_dispatch(context, 0x84, e);
    const uint32_t multiplier = px_battle_event_get(0x36, e);
    px_battle_event_pop(e);
    const uint32_t product = m->load16(m->user, battler + 0x150) * multiplier;
    uint32_t result = product >> 12;
    if ((product & 0xfff) > 0x800) ++result;
    return result == 0 ? 1 : result;
}

uint32_t px_battle_event_blocked(uint32_t context, uint32_t handler, uint32_t event, const struct px_battle_core_5_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    const uint32_t slot = m->load8(m->user, handler + 0x3b);
    uint32_t battler = 0;
    if (slot != 0x1f) battler = battler_for(context, slot, api);
    const uint32_t owner = m->load8(m->user, handler + 0x10);
    if (owner == 4) {
        if (battler != 0 && (m->load32(m->user, battler + 0x5c) & 7)) return 1;
    } else if (owner == 5) {
        if (m->load32(m->user, m->load32(m->user, context + 0x1c) + 0x290) != 0) return 1;
        if (battler != 0) {
            if (m->load32(m->user, battler + 0x68) & 7) return 1;
            if (m->load8(m->user, battler + 0x167) & 2) return 1;
        }
    }
    for (uint32_t node = m->load32(m->user, api->battle + 0x60); node != 0; node = m->load32(m->user, node + 4)) {
        if (m->load32(m->user, node + 0xc) == 0) continue;
        if (m->load8(m->user, node + 0x10) == 4) {
            const uint32_t other = battler_for(context, m->load8(m->user, node + 0x3b), api);
            if (other != 0 && (m->load32(m->user, other + 0x5c) & 7)) continue;
        }
        if (m->load32(m->user, node + 0x18) & UINT32_C(0x40000000)) continue;
        if (api->checker(user(api), node, context, m->load8(m->user, handler + 0x10), event,
                         m->load16(m->user, handler + 0x38), m->load8(m->user, handler + 0x3a)) != 0)
            return 1;
    }
    return 0;
}
