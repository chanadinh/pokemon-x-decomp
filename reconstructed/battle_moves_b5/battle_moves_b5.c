#include "battle_moves_b5.h"

static void *user(const struct px_battle_moves_b5_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_b5_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_b5_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_moves_b5_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static void store8(uint32_t address, uint32_t v, const struct px_battle_moves_b5_api *api)
{
    memory(api)->store8(memory(api)->user, address, (uint8_t)v);
}

static void store16(uint32_t address, uint32_t v, const struct px_battle_moves_b5_api *api)
{
    memory(api)->store16(memory(api)->user, address, (uint16_t)v);
}

static void store32(uint32_t address, uint32_t v, const struct px_battle_moves_b5_api *api)
{
    memory(api)->store32(memory(api)->user, address, v);
}

static uint32_t load8(uint32_t address, const struct px_battle_moves_b5_api *api)
{
    return memory(api)->load8(memory(api)->user, address);
}

static uint32_t load16(uint32_t address, const struct px_battle_moves_b5_api *api)
{
    return memory(api)->load16(memory(api)->user, address);
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_moves_b5_api *api)
{
    return api->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t rec, const struct px_battle_moves_b5_api *api)
{
    api->commit_actions(user(api), context, rec);
}

/* Kind 0xb on 'target' for 'condition' with message at +0x14: the shared tail of the condition handlers. */
static void condition_record(uint32_t context, uint32_t id, uint32_t target, uint32_t condition, uint32_t word,
                             uint32_t message, const struct px_battle_moves_b5_api *api)
{
    const uint32_t rec = record(context, 0xb, id, api);
    store8(rec + 0xf, target, api);
    store16(rec + 4, condition, api);
    store32(rec + 8, word, api);
    api->message(user(api), rec + 0x14, 2, message);
    api->message_arg(user(api), rec + 0x14, target);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B5_HANDLER(px_battle_gastro_acid_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    const uint32_t target = get(0x06, api) & 0xff;
    condition_record(context, id, target, 0x10, 1, 0x235, api);  /* literal 108ad6c0 */
}

PX_BATTLE_MOVES_B5_HANDLER(px_battle_celebrate_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    px_battle_event_rewrite(0x53, 0, api->base->events);
    api->header_record(user(api), context, 0x3c, id);
    /* The message names the user's owner, or 0 when the owner is not valid. */
    uint32_t owner = api->owner_of(user(api), id);
    if (api->owner_valid(user(api), context, owner) == 0) owner = 0;
    const uint32_t rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 1, 0x10b);  /* literal 108adfb0 */
    api->message_arg(user(api), rec + 4, owner);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B5_HANDLER(px_battle_powder_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Condition 0x25 for one turn on every target. */
    const uint32_t count = get(0x05, api);
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t target = get((i + 6) & 0xff, api) & 0xff;
        const uint32_t rec = record(context, 0xb, id, api);
        store8(rec + 0xf, target, api);
        store16(rec + 4, 0x25, api);
        store32(rec + 8, px_battle_turn_count(1), api);
        api->message(user(api), rec + 0x14, 2, 0x4ba);  /* literal 108ae7fc */
        api->message_arg(user(api), rec + 0x14, target);
        commit(context, rec, api);
    }
}

PX_BATTLE_MOVES_B5_HANDLER(px_battle_splash_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    const uint32_t rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 1, 0x6a);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B5_HANDLER(px_battle_aqua_ring_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    if (api->has_status(user(api), pokemon(context, id, api), 0x23) != 0) return;
    const uint32_t rec = record(context, 0xb, id, api);
    store16(rec + 4, 0x23, api);
    store32(rec + 8, 1, api);
    store8(rec + 0xf, id, api);
    api->message(user(api), rec + 0x14, 2, 0x259);  /* literal 108b009c */
    api->message_arg(user(api), rec + 0x14, id);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B5_HANDLER(px_battle_taunt_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Condition 0xb for 3 turns, 4 when the target has already moved. */
    const uint32_t target = get(0x06, api) & 0xff;
    const uint32_t other = pokemon(context, target, api);
    if (api->has_status(user(api), other, 0xb) != 0) return;
    uint32_t turns = 3;
    if (api->turn_flag(user(api), other, 1) != 0) turns = 4;
    const uint32_t rec = record(context, 0xb, id, api);
    store16(rec + 4, 0xb, api);
    store32(rec + 8, px_battle_turn_count(turns), api);
    store8(rec + 0xf, target, api);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B5_HANDLER(px_battle_trap_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Condition 0x16 on every target that is not trapped and not Ghost (type 7). */
    const uint32_t count = get(0x05, api);
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t target = get(0x06, api) & 0xff;
        const uint32_t other = pokemon(context, target, api);
        if (api->has_status(user(api), other, 0x16) != 0) continue;
        if (api->has_type(user(api), other, 7) != 0) continue;
        const uint32_t rec = record(context, 0xb, id, api);
        store8(rec + 0xf, target, api);
        store16(rec + 4, 0x16, api);
        store32(rec + 8, api->trap_condition(user(api), context), api);
        api->message(user(api), rec + 0x14, 2, 0x36b);  /* literal 108b0928 */
        api->message_arg(user(api), rec + 0x14, target);
        commit(context, rec, api);
    }
}

PX_BATTLE_MOVES_B5_HANDLER(px_battle_trick_or_treat_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* The target gains the Ghost type (7) unless it already has it. */
    const uint32_t target = get(0x06, api) & 0xff;
    if (api->has_type(user(api), pokemon(context, target, api), 7) != 0) return;
    const uint32_t rec = record(context, 0x14, id, api);
    store8(rec + 5, target, api);
    store8(rec + 4, 7, api);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B5_HANDLER(px_battle_simple_beam_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Ability 0x56 for every target that does not have ability 0x36. */
    const uint32_t count = get(0x05, api);
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t key = (i + 6) & 0xff;
        const uint32_t target = get(key, api) & 0xff;
        if (api->base->pokemon_value(user(api), pokemon(context, target, api), 0x10) == 0x36) continue;
        const uint32_t rec = record(context, 0x20, id, api);
        store8(rec + 6, get(key, api), api);
        store16(rec + 4, 0x56, api);
        api->message(user(api), rec + 0xc, 2, 0x195);  /* literal 108b44c4 */
        api->message_arg(user(api), rec + 0xc, load8(rec + 6, api));
        api->message_arg(user(api), rec + 0xc, load16(rec + 4, api));
        commit(context, rec, api);
    }
}

PX_BATTLE_MOVES_B5_HANDLER(px_battle_flower_shield_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Defense (stat 2) +1 for every Grass-type (11) target. */
    const uint32_t count = get(0x05, api);
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t target = get((i + 6) & 0xff, api) & 0xff;
        if (api->has_type(user(api), pokemon(context, target, api), 0xb) == 0) continue;
        const uint32_t rec = record(context, 0xd, id, api);
        store8(rec + 0xc, target, api);
        store8(rec + 0x16, 1, api);
        store8(rec + 4, 2, api);
        store8(rec + 0x17, 1, api);
        store8(rec + 0x18, load8(rec + 0x18, api) | 0x26, api);
        commit(context, rec, api);
    }
}

PX_BATTLE_MOVES_B5_HANDLER(px_battle_entrainment_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* The user's ability (halfword) goes to every target that does not have ability 0x36. */
    const uint32_t ability = api->base->pokemon_value(user(api), pokemon(context, id, api), 0x10) & 0xffff;
    if (api->ability_blocked(user(api), ability) != 0) return;
    const uint32_t count = get(0x05, api);
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t key = (i + 6) & 0xff;
        const uint32_t target = get(key, api) & 0xff;
        if (api->base->pokemon_value(user(api), pokemon(context, target, api), 0x10) == 0x36) continue;
        const uint32_t rec = record(context, 0x20, id, api);
        store16(rec + 4, ability, api);
        store8(rec + 6, get(key, api), api);
        api->message(user(api), rec + 0xc, 2, 0x195);  /* literal 108b6198 */
        api->message_arg(user(api), rec + 0xc, load8(rec + 6, api));
        api->message_arg(user(api), rec + 0xc, load16(rec + 4, api));
        commit(context, rec, api);
    }
}

PX_BATTLE_MOVES_B5_HANDLER(px_battle_worry_seed_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Ability 0xf for the (unmasked) first target. */
    const uint32_t target = get(0x06, api);
    const uint32_t rec = record(context, 0x20, id, api);
    store8(rec + 6, target, api);
    store16(rec + 4, 0xf, api);
    api->message(user(api), rec + 0xc, 2, 0x195);  /* literal 108b6220 */
    api->message_arg(user(api), rec + 0xc, load8(rec + 6, api));
    api->message_arg(user(api), rec + 0xc, load16(rec + 4, api));
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B5_HANDLER(px_battle_after_you_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    const uint32_t rec = record(context, 0x30, id, api);
    store8(rec + 4, get(0x06, api), api);
    api->message(user(api), rec + 8, 2, 0x474);  /* literal 108b6290 */
    api->message_arg(user(api), rec + 8, load8(rec + 4, api));
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B5_HANDLER(px_battle_destiny_bond_effect)
{
    (void)handler;
    if (get(0x03, api) != id) return;
    const uint32_t rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 2, 0x272);  /* literal 108b7eb8 */
    api->message_arg(user(api), rec + 4, id);
    commit(context, rec, api);
    store32(work + 0x18, 1, api);
}
