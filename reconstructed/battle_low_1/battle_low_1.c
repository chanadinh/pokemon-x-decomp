#include "battle_low_1.h"

enum {
    MONEY_CAP = 99999,          /* literal 1081caec */
    LIST_OFFSET = 0x189c,       /* literal 10810d84 */
    ENTRY_COUNT_OFFSET = 0x108c /* literal 1081fd40 */
};

static const struct px_pokemon_api *mem(const struct px_battle_low_api *api)
{
    return api->base->events->memory;
}

static void *user(const struct px_battle_low_api *api)
{
    return api->base->events->user;
}

static uint32_t load8(uint32_t a, const struct px_pokemon_api *m)
{
    return m->load8(m->user, a);
}

static uint32_t load32(uint32_t a, const struct px_pokemon_api *m)
{
    return m->load32(m->user, a);
}

static void store8(uint32_t a, uint32_t v, const struct px_pokemon_api *m)
{
    m->store8(m->user, a, (uint8_t)v);
}

static void abort_call(const struct px_battle_low_api *api)
{
    api->base->events->abort(api->base->events->user);   /* 10800394(0, 0, 0) */
}

uint32_t px_battle_is_mode_2_or_3(uint32_t value)
{
    return value == 2 || value == 3;
}

uint32_t px_battle_context_0ca4(uint32_t context)
{
    return context + 0xca4;
}

uint32_t px_battle_half_4674(uint32_t context, const struct px_pokemon_api *memory)
{
    return memory->load16(memory->user, context + 0x4674);
}

uint32_t px_battle_next_counter(uint32_t context, const struct px_pokemon_api *memory)
{
    /* The counter at +0x1084 steps by one; a wrap to 0 is stored as 1. */
    uint32_t next = load32(context + 0x1084, memory) + 1;
    memory->store32(memory->user, context + 0x1084, next);
    if (next == 0) {
        next = 1;
        memory->store32(memory->user, context + 0x1084, 1);
    }
    return next;
}

void px_battle_push_byte(uint32_t context, uint32_t value, const struct px_pokemon_api *memory)
{
    const uint32_t count = load8(context + 0x1091, memory);
    if (count >= 10) return;
    store8(context + 0x1091, count + 1, memory);
    store8(context + 0x1094 + count, value, memory);
}

uint32_t px_battle_add_capped(uint32_t object, uint32_t amount, const struct px_pokemon_api *memory)
{
    /* Only for a state byte of 0 or 1; the amount is added to the word at +0x804 unless flag 8 of +0x81c is set. */
    const uint32_t state = load8(load32(load32(object + 4, memory) + 0x10, memory), memory);
    if (state != 0 && state != 1) return 0;
    const uint32_t target = load32(load32(object, memory) + 0x10, memory);
    if ((load8(target + 0x81c, memory) & 8) == 0) {
        uint32_t total = amount + load32(target + 0x804, memory);
        memory->store32(memory->user, target + 0x804, total);
        if (total > MONEY_CAP) total = MONEY_CAP;
        memory->store32(memory->user, target + 0x804, total);
    }
    return 1;
}

void px_battle_add_unique(uint32_t context, uint32_t value, const struct px_battle_low_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    const uint32_t count_address = context + 0x18a6;
    if (load8(count_address, m) >= 6) abort_call(api);
    const uint32_t count = load8(count_address, m);
    for (uint32_t i = 0; i < count; ++i) {
        if (load8(context + LIST_OFFSET + i, m) == value) return;
    }
    store8(count_address, count + 1, m);
    store8(context + LIST_OFFSET + count, value, m);
}

uint32_t px_battle_slot_state(uint32_t context, uint32_t index, const struct px_battle_low_api *api)
{
    const uint32_t slot = (index + 2) & 3;
    const uint32_t present = api->slot_present(user(api), load32(context + 4, mem(api)), slot);
    return present != 0 ? slot : 4;
}

uint32_t px_battle_partner_entry(uint32_t context, uint32_t id, const struct px_battle_low_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    const uint32_t owner = api->owner_of(user(api), id);
    uint32_t slot = (owner + 2) & 3;
    if (api->slot_present(user(api), load32(context + 4, m), slot) == 0) slot = 4;
    if (api->slot_present(user(api), load32(context + 4, m), slot) == 0) return 0;
    if (slot >= 4) abort_call(api);
    return load32(context + 8, m) + 28 * slot + 4;
}

void px_battle_record_0x37(uint32_t context, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e,
                           const struct px_battle_low_api *api)
{
    api->record_event(user(api), load32(context + 0xc, mem(api)), 0x37, a, b, c, d, e);
}

uint32_t px_battle_count_better(uint32_t context, uint32_t entry, uint32_t arg, const struct px_battle_low_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    const uint32_t reference = api->entry_score(user(api), context, entry, arg);
    const uint32_t id = load8(entry + 0x19, m);
    uint32_t count = 0;
    api->entry_first(user(api), context, api->iterator);
    while (api->entry_next(user(api), context, api->iterator, api->iterator + 4) != 0) {
        const uint32_t other = load32(api->iterator + 4, m);
        if (load8(other + 0x19, m) == id) continue;
        if (api->entry_score(user(api), context, other, arg) > reference) count = (count + 1) & 0xffff;
    }
    return count;
}

uint32_t px_battle_event_0xb_allows(uint32_t context, uint32_t id, const struct px_battle_low_api *api)
{
    const struct px_battle_event_api *events = api->base->events;
    const uint32_t pokemon = px_battle_pokemon_by_id(context, id, api->base);
    /* A Ghost (type 7) is always allowed; a battler without HP never. */
    if (px_battle_has_type(pokemon, 7, events->memory) != 0) return 1;
    if (api->base->pokemon_value(user(api), pokemon, 0xd) == 0) return 0;
    px_battle_event_push(events);
    px_battle_event_set_id(0x02, load8(pokemon + 0x19, events->memory), events);
    px_battle_event_set_value(0x53, 0, events);
    px_battle_event_dispatch(context, 0xb, events);
    const uint32_t allowed = px_battle_event_get(0x53, events) != 0;
    px_battle_event_pop(events);
    return allowed;
}

uint32_t px_battle_chance_event(uint32_t context, uint32_t key3, uint32_t key4, uint32_t value,
                                const struct px_battle_low_api *api)
{
    const struct px_battle_event_api *events = api->base->events;
    px_battle_event_push(events);
    px_battle_event_set_id(0x04, key4, events);
    px_battle_event_set_id(0x03, key3, events);
    px_battle_event_set_scaled(0x27, value, events);
    px_battle_event_dispatch(context, 0xb5, events);
    const uint32_t chance = px_battle_event_get(0x27, events);
    px_battle_event_pop(events);
    /* A roll below 100 against the (possibly rewritten) chance. */
    const uint32_t roll = api->random_below(user(api), load32(api->rng_object + 0x54, events->memory), 100);
    return roll < chance;
}

void px_battle_set_move_slot(uint32_t unused, uint32_t slot_object, uint32_t move, uint32_t limit, uint32_t first,
                             const struct px_battle_low_api *api)
{
    (void)unused;
    const struct px_pokemon_api *m = mem(api);
    /* The first slot (+0, count +2/+3) or the second (+6, count +8/+9) takes the move and its PP,
     * capped at 'limit' when that is nonzero. */
    const uint32_t base = first != 0 ? slot_object : slot_object + 6;
    if (first != 0) store8(slot_object + 5, 0, m);
    else store8(slot_object + 0xb, 0, m);
    m->store16(m->user, base, (uint16_t)move);
    if (move == 0) store8(base + 3, 0, m);
    else store8(base + 3, api->base_pp(user(api), move, 0), m);
    if (limit != 0 && load8(base + 3, m) > limit) store8(base + 3, limit, m);
    store8(base + 2, load8(base + 3, m), m);
    if (first != 0) {
        if (load8(slot_object + 0xc, m) != 0) api->commit_slot(user(api), slot_object + 6, slot_object);
    } else {
        store8(slot_object + 0xc, 0, m);
    }
}

uint32_t px_battle_clear_bit_flag(uint32_t context, uint32_t battler, uint32_t index, uint32_t quiet,
                                  const struct px_battle_low_api *api)
{
    const struct px_pokemon_api *m = mem(api);
    const uint32_t byte_index = index >> 3;
    const uint32_t bit = 1u << (index & 7);
    const uint32_t current = load8(battler + 0x169 + byte_index, m);
    if ((bit & current) == 0) return 0;
    const uint32_t id = load8(battler + 0x19, m);
    store8(battler + 0x169 + (byte_index & 0xff), current & ~bit, m);
    api->record_event(user(api), load32(context + 0xc, m), 0x1c, load8(battler + 0x19, m), index, 0, 0, 0);
    if ((load32(battler + 0x84, m) & 7) != 0) api->queue_change(user(api), context, battler, 0x1a, 0);
    api->record_event(user(api), load32(context + 0xc, m), 0x38, id, 0, 0, 0, 0);
    if (quiet != 0) return 1;
    const uint32_t count = load8(context + ENTRY_COUNT_OFFSET, m);
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t entry = context + 0x10bc + 16 * i;
        if ((load32(entry + 4, m) & 0xf) == 8) continue;
        if (load8(load32(entry, m) + 0x19, m) != id) continue;
        store8(entry + 0xd, 1, m);
        return 1;
    }
    return 1;
}
