#include "battle_moves_b6.h"

enum { FLAG_RECORD = 0x1000000 };

/* The seven stats Acupressure picks from, in table order (original data at 108d2c46, segment_1 + 19526). */
static const uint8_t acupressure_stats[7] = {1, 2, 5, 3, 4, 6, 7};

static void *user(const struct px_battle_moves_b6_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_b6_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_b6_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_moves_b6_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static uint32_t value(uint32_t pokemon_address, uint32_t key, const struct px_battle_moves_b6_api *api)
{
    return api->base->pokemon_value(user(api), pokemon_address, key);
}

static void store8(uint32_t address, uint32_t v, const struct px_battle_moves_b6_api *api)
{
    memory(api)->store8(memory(api)->user, address, (uint8_t)v);
}

static void store16(uint32_t address, uint32_t v, const struct px_battle_moves_b6_api *api)
{
    memory(api)->store16(memory(api)->user, address, (uint16_t)v);
}

static void store32(uint32_t address, uint32_t v, const struct px_battle_moves_b6_api *api)
{
    memory(api)->store32(memory(api)->user, address, v);
}

static uint32_t load8(uint32_t address, const struct px_battle_moves_b6_api *api)
{
    return memory(api)->load8(memory(api)->user, address);
}

static uint32_t load32(uint32_t address, const struct px_battle_moves_b6_api *api)
{
    return memory(api)->load32(memory(api)->user, address);
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_moves_b6_api *api)
{
    return api->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t rec, const struct px_battle_moves_b6_api *api)
{
    api->commit_actions(user(api), context, rec);
}

/* A kind-0xd record: stat 'stat' by 'stages' stages for 'battler', flags |= 'flags'. */
static uint32_t stat_record(uint32_t context, uint32_t id, uint32_t stat, uint32_t stages, uint32_t flags,
                            uint32_t battler, const struct px_battle_moves_b6_api *api)
{
    const uint32_t rec = record(context, 0xd, id, api);
    store8(rec + 0xc, battler, api);
    store8(rec + 0x16, 1, api);
    store8(rec + 4, stat, api);
    store8(rec + 0x17, stages, api);
    store8(rec + 0x18, load8(rec + 0x18, api) | flags, api);
    return rec;
}

/* Kind 0x1a: the side bits of Defog's hazards (+5 / +6 bits, each when the count at +4 exceeds a minimum). */
static void side_bits(uint32_t rec, uint32_t offset, uint32_t bit, uint32_t minimum, const struct px_battle_moves_b6_api *api)
{
    if (load8(rec + 4, api) > minimum) store8(rec + offset, load8(rec + offset, api) | bit, api);
}

static uint32_t side_record(uint32_t context, uint32_t id, uint32_t battler, const struct px_battle_moves_b6_api *api)
{
    const uint32_t rec = record(context, 0x1a, id, api);
    store8(rec + 8, api->side_mask(user(api), battler), api);
    store8(rec + 4, 4, api);
    store8(rec + 5, 0, api);
    store8(rec + 6, 0, api);
    store8(rec + 7, 0, api);
    return rec;
}

PX_BATTLE_MOVES_B6_HANDLER(px_battle_defog_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Evasion -1 for the target, then the hazards go from both sides. */
    const uint32_t target = get(0x06, api) & 0xff;
    (void)pokemon(context, target, api);
    uint32_t rec = stat_record(context, id, 7, 0xff, 0x26, target, api);
    commit(context, rec, api);
    rec = side_record(context, id, target, api);
    side_bits(rec, 5, 0x01, 1, api);
    side_bits(rec, 5, 0x02, 1, api);
    side_bits(rec, 5, 0x08, 1, api);
    side_bits(rec, 5, 0x04, 1, api);
    side_bits(rec, 5, 0x40, 1, api);
    side_bits(rec, 5, 0x80, 1, api);
    side_bits(rec, 6, 0x01, 2, api);
    side_bits(rec, 6, 0x40, 2, api);
    commit(context, rec, api);
    rec = side_record(context, id, id, api);
    side_bits(rec, 5, 0x40, 1, api);
    side_bits(rec, 5, 0x80, 1, api);
    side_bits(rec, 6, 0x01, 2, api);
    side_bits(rec, 6, 0x40, 2, api);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B6_HANDLER(px_battle_power_swap_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Attack and Special Attack stages of both battlers are exchanged (two kind-0xe records). */
    const uint32_t target = get(0x06, api) & 0xff;
    const uint32_t theirs = pokemon(context, target, api);
    const uint32_t mine = pokemon(context, id, api);
    const uint32_t my_attack = value(mine, 1, api);
    const uint32_t my_special = value(mine, 3, api);
    uint32_t rec = record(context, 0xe, id, api);
    store8(rec + 4, id, api);
    store8(rec + 5, value(theirs, 1, api), api);
    store8(rec + 7, value(theirs, 3, api), api);
    store8(rec + 6, value(mine, 2, api), api);
    store8(rec + 8, value(mine, 4, api), api);
    store8(rec + 9, value(mine, 5, api), api);
    store8(rec + 0xa, value(mine, 6, api), api);
    store8(rec + 0xb, value(mine, 7, api), api);
    store8(rec + 0xc, load8(mine + 0x155, api), api);
    commit(context, rec, api);
    rec = record(context, 0xe, id, api);
    store8(rec + 5, my_attack, api);
    store8(rec + 4, target, api);
    store8(rec + 7, my_special, api);
    store8(rec + 6, value(theirs, 2, api), api);
    store8(rec + 8, value(theirs, 4, api), api);
    store8(rec + 9, value(theirs, 5, api), api);
    store8(rec + 0xa, value(theirs, 6, api), api);
    store8(rec + 0xb, value(theirs, 7, api), api);
    store8(rec + 0xc, load8(theirs + 0x155, api), api);
    commit(context, rec, api);
    rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 2, 0x2a4);
    api->message_arg(user(api), rec + 4, id);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B6_HANDLER(px_battle_stockpile_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Up to three stockpiles: counter 0, then Defense and Special Defense +1, counters 1 and 2. */
    const uint32_t self = pokemon(context, id, api);
    const uint32_t count = api->counter(user(api), self, 0);
    if (count >= 3) return;
    uint32_t rec = record(context, 0x27, id, api);
    store8(rec + 4, id, api);
    store8(rec + 5, 0, api);
    store8(rec + 6, count + 1, api);
    commit(context, rec, api);
    rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 2, 0x2d1);  /* literal 108b3084 */
    api->message_arg(user(api), rec + 4, id);
    api->message_arg(user(api), rec + 4, count + 1);
    commit(context, rec, api);
    commit(context, stat_record(context, id, 2, 1, 6, id, api), api);
    rec = record(context, 0x27, id, api);
    store8(rec + 4, id, api);
    store8(rec + 5, 1, api);
    store32(rec, load32(rec, api) | FLAG_RECORD, api);
    store8(rec + 6, api->counter(user(api), self, 1) + 1, api);
    commit(context, rec, api);
    commit(context, stat_record(context, id, 4, 1, 6, id, api), api);
    rec = record(context, 0x27, id, api);
    store8(rec + 4, id, api);
    store8(rec + 5, 2, api);
    store32(rec, load32(rec, api) | FLAG_RECORD, api);
    store8(rec + 6, api->counter(user(api), self, 2) + 1, api);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B6_HANDLER(px_battle_baton_pass_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    if (api->can_switch(user(api), context, id) == 0) return;
    if (api->can_pass(user(api), context) == 0) return;
    uint32_t rec = record(context, 0x1f, id, api);
    store8(rec + 4, 4, api);
    store8(rec + 5, api->base->side_of(user(api), context, id), api);
    store32(rec + 8, id, api);
    store8(rec + 0x18, 1, api);
    commit(context, rec, api);
    rec = record(context, 0x17, id, api);
    store8(rec + 5, id, api);
    store8(rec + 4, 0xe, api);
    store32(rec, load32(rec, api) | FLAG_RECORD, api);
    commit(context, rec, api);
    rec = record(context, 0x2a, id, api);
    store8(rec + 0x4c, id, api);
    store8(rec + 0x4d, 1, api);
    store32(rec, load32(rec, api) | FLAG_RECORD, api);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B6_HANDLER(px_battle_psycho_shift_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* The user's status moves to a target that has none (condition record, then the cure, then key 0x53). */
    const uint32_t self = pokemon(context, id, api);
    const uint32_t status = api->base->pokemon_status(user(api), self);
    if (status == 0) return;
    const uint32_t target = get(0x06, api) & 0xff;
    if (api->base->pokemon_status(user(api), pokemon(context, target, api)) != 0) return;
    const uint32_t data = api->status_data(user(api), self, status);
    uint32_t rec = record(context, 0xb, id, api);
    store16(rec + 4, status, api);
    if (status == 5 && api->poison_flag(user(api), data) != 0) store32(rec + 8, 0x79, api);
    else store32(rec + 8, api->status_word(user(api), status & 0xff), api);
    store8(rec + 0xf, target, api);
    store8(rec + 0xc, 1, api);
    commit(context, rec, api);
    rec = record(context, 0xa, id, api);
    store8(rec + 0x11, 1, api);
    store8(rec + 5, id, api);
    store32(rec, load32(rec, api) | FLAG_RECORD, api);
    store8(rec + 4, status, api);
    commit(context, rec, api);
    px_battle_event_rewrite(0x53, 0, api->base->events);
}

PX_BATTLE_MOVES_B6_HANDLER(px_battle_ally_switch_effect)
{
    (void)handler, (void)work;
    const uint32_t side = api->base->side_of(user(api), context, id);
    uint32_t partner = id;
    const uint32_t mode = api->battle_mode(user(api), context);
    if (mode == 1) {
        /* Double battle: the partner of the owner's other slot, when the owner state is 4. */
        const uint32_t owner = api->owner_of(user(api), id);
        if (api->owner_state(user(api), context, owner) == 4) {
            uint32_t slot = api->side_slot(user(api), side, 0);
            if (slot == side) slot = api->side_slot(user(api), side, 1);
            partner = api->battler_at(user(api), context, slot);
        }
    } else if (mode == 2) {
        /* Triple battle: the slot beside the user's, unless the side check 10814f9c holds. */
        if (api->side_active(user(api), side) == 0) {
            uint32_t slot = api->side_slot(user(api), side, 0);
            if (slot == side) slot = api->side_slot(user(api), side, 2);
            partner = api->battler_at(user(api), context, slot);
        }
    }
    if (partner == id) return;
    const uint32_t rec = record(context, 0x33, id, api);
    store8(rec + 4, id, api);
    store8(rec + 5, partner, api);
    api->message(user(api), rec + 8, 2, 0x477);  /* literal 108b43f0 */
    api->message_arg(user(api), rec + 8, id);
    api->message_arg(user(api), rec + 8, partner);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B6_HANDLER(px_battle_acupressure_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* A random stat of the target that can still rise by 2 goes up by 2. */
    const uint32_t target = get(0x06, api) & 0xff;
    const uint32_t other = pokemon(context, target, api);
    uint32_t count = 0;
    for (uint32_t i = 0; i < 7; ++i) {
        if (api->stat_can_rise(user(api), other, acupressure_stats[i], 2) != 0) count = (count + 1) & 0xff;
    }
    if (count == 0) return;
    uint32_t pick = api->random(user(api), count) & 0xff;
    for (uint32_t i = 0; i < 7; ++i) {
        if (api->stat_can_rise(user(api), other, acupressure_stats[i], 2) == 0) continue;
        if (pick != 0) {
            --pick;
            continue;
        }
        const uint32_t rec = record(context, 0xd, id, api);
        const uint32_t old = load8(rec + 0x18, api);
        store8(rec + 0xc, target, api);
        store8(rec + 0x16, 1, api);
        store8(rec + 0x17, 2, api);
        store8(rec + 4, acupressure_stats[i], api);
        store8(rec + 0x18, (old & ~2u) | 0x14, api);
        commit(context, rec, api);
        return;
    }
}

PX_BATTLE_MOVES_B6_HANDLER(px_battle_spiky_shield_effect)
{
    (void)handler;
    api->protect_start(user(api), context, id);
    store32(work + 0x18, 1, api);
}
