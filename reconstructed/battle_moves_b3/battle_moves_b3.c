#include "battle_moves_b3.h"

enum { FLAG_RECORD = 0x1000000 };

static void *user(const struct px_battle_moves_b3_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_b3_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_b3_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_moves_b3_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static void store8(uint32_t address, uint32_t value, const struct px_battle_moves_b3_api *api)
{
    memory(api)->store8(memory(api)->user, address, (uint8_t)value);
}

static void store16(uint32_t address, uint32_t value, const struct px_battle_moves_b3_api *api)
{
    memory(api)->store16(memory(api)->user, address, (uint16_t)value);
}

static void store32(uint32_t address, uint32_t value, const struct px_battle_moves_b3_api *api)
{
    memory(api)->store32(memory(api)->user, address, value);
}

static uint32_t load8(uint32_t address, const struct px_battle_moves_b3_api *api)
{
    return memory(api)->load8(memory(api)->user, address);
}

static uint32_t load16(uint32_t address, const struct px_battle_moves_b3_api *api)
{
    return memory(api)->load16(memory(api)->user, address);
}

static uint32_t load32(uint32_t address, const struct px_battle_moves_b3_api *api)
{
    return memory(api)->load32(memory(api)->user, address);
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_moves_b3_api *api)
{
    return api->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t rec, const struct px_battle_moves_b3_api *api)
{
    api->commit_actions(user(api), context, rec);
}

/* A kind-3 record with message 'message' naming the user. The record is returned, committed. */
static uint32_t announce(uint32_t context, uint32_t id, uint32_t message, const struct px_battle_moves_b3_api *api)
{
    const uint32_t rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 2, message);
    api->message_arg(user(api), rec + 4, id);
    return rec;
}

PX_BATTLE_MOVES_B3_HANDLER(px_battle_mind_reader_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Condition 0x1d on the user, with the target's turn word; message 0x28b names both. */
    const uint32_t target = get(0x06, api) & 0xff;
    const uint32_t rec = record(context, 0xb, id, api);
    store8(rec + 0xf, id, api);
    store16(rec + 4, 0x1d, api);
    store32(rec + 8, api->target_condition(user(api), target, 2), api);
    api->message(user(api), rec + 0x14, 2, 0x28b);
    api->message_arg(user(api), rec + 0x14, id);
    api->message_arg(user(api), rec + 0x14, target);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B3_HANDLER(px_battle_electrify_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Fails when the target has already moved (turn flag 1). Condition 0x24 for a turn. */
    const uint32_t target = get(0x06, api) & 0xff;
    if (api->turn_flag(user(api), pokemon(context, target, api), 1) != 0) return;
    const uint32_t rec = record(context, 0xb, id, api);
    store8(rec + 0xf, target, api);
    store16(rec + 4, 0x24, api);
    store32(rec + 8, api->condition_word(user(api), 1, 0xc), api);
    api->message(user(api), rec + 0x14, 2, 0x4ab);
    api->message_arg(user(api), rec + 0x14, target);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B3_HANDLER(px_battle_endure_effect)
{
    if (get(0x03, api) != id) return;
    (void)handler;
    commit(context, announce(context, id, 0x1ff, api), api);
    store32(work + 0x18, 1, api);
    /* The endure counter (counter 3) goes up by one in a kind-0x27 record. */
    const uint32_t self = pokemon(context, id, api);
    const uint32_t rec = record(context, 0x27, id, api);
    store8(rec + 6, api->counter(user(api), self, 3) + 1, api);
    store8(rec + 4, id, api);
    store8(rec + 5, 3, api);
    store32(rec, load32(rec, api) & ~(uint32_t)FLAG_RECORD, api);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B3_HANDLER(px_battle_recycle_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* The user's consumed item (+0x14) comes back when it holds none (+0x12). */
    const uint32_t self = pokemon(context, id, api);
    if (load16(self + 0x12, api) != 0) return;
    const uint32_t item = load16(self + 0x14, api);
    if (item == 0) return;
    const uint32_t rec = record(context, 0x21, id, api);
    store8(rec + 6, id, api);
    store16(rec + 4, item, api);
    store8(rec + 7, 1, api);
    api->message(user(api), rec + 0xc, 2, 0x2dd);
    api->message_arg(user(api), rec + 0xc, id);
    api->message_arg(user(api), rec + 0xc, item);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B3_HANDLER(px_battle_focus_energy_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    if (api->battle_flag(user(api), pokemon(context, id, api), 9) != 0) return;
    uint32_t rec = record(context, 0x17, id, api);
    store8(rec + 5, id, api);
    store8(rec + 4, 9, api);
    commit(context, rec, api);
    rec = announce(context, id, 0x417, api);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B3_HANDLER(px_battle_role_play_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* The user copies the target's ability (key 0x10, low byte) unless it cannot be copied. */
    const uint32_t target = get(0x06, api) & 0xff;
    const uint32_t ability = api->base->pokemon_value(user(api), pokemon(context, target, api), 0x10) & 0xff;
    if (api->ability_blocked(user(api), ability) != 0) return;
    const uint32_t rec = record(context, 0x20, id, api);
    store8(rec + 6, id, api);
    store16(rec + 4, ability, api);
    api->message(user(api), rec + 0xc, 2, 0x26b);
    api->message_arg(user(api), rec + 0xc, id);
    api->message_arg(user(api), rec + 0xc, target);
    api->message_arg(user(api), rec + 0xc, load16(rec + 4, api));
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B3_HANDLER(px_battle_disable_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* The target's last move (not 0 or 0xa5, and usable) is disabled: condition 0xd. */
    const uint32_t target = get(0x06, api) & 0xff;
    const uint32_t self = pokemon(context, target, api);
    if (api->has_status(user(api), self, 0xd) != 0) return;
    const uint32_t move = api->last_move(user(api), self);
    if (move == 0 || move == 0xa5) return;
    if (api->move_usable(user(api), self, move) == 0) return;
    uint32_t turns = 4;
    if (api->turn_flag(user(api), self, 1) != 0) turns = 5;
    const uint32_t rec = record(context, 0xb, id, api);
    store16(rec + 4, 0xd, api);
    store32(rec + 8, api->condition_word(user(api), turns, move), api);
    store8(rec + 0xf, target, api);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B3_HANDLER(px_battle_reflect_type_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* The user takes the target's types, then a message with flag 0x1000000 names both. */
    const uint32_t target = get(0x06, api) & 0xff;
    const uint32_t other = pokemon(context, target, api);
    uint32_t rec = record(context, 0x13, id, api);
    store8(rec + 6, id, api);
    store16(rec + 4, api->types(user(api), other), api);
    store8(rec + 7, 1, api);
    commit(context, rec, api);
    rec = record(context, 3, id, api);
    store32(rec, load32(rec, api) | FLAG_RECORD, api);
    api->message(user(api), rec + 4, 2, 0x447);
    api->message_arg(user(api), rec + 4, id);
    api->message_arg(user(api), rec + 4, target);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B3_HANDLER(px_battle_psych_up_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Kind 0x10 copies the target's raw stats 9 and 8 (flags +0xf |= 3); kind 0x17 follows. */
    const uint32_t self = pokemon(context, id, api);
    uint32_t rec = record(context, 0x10, id, api);
    store8(rec + 0xe, id, api);
    store16(rec + 4, api->raw_stat(user(api), self, 9), api);
    store16(rec + 6, api->raw_stat(user(api), self, 8), api);
    store8(rec + 0xf, load8(rec + 0xf, api) | 3, api);
    api->message(user(api), rec + 0x10, 2, 0x305);
    api->message_arg(user(api), rec + 0x10, id);
    commit(context, rec, api);
    rec = record(context, 0x17, id, api);
    store8(rec + 5, id, api);
    store8(rec + 4, 0xa, api);
    commit(context, rec, api);
}

/* Healing Wish and Lunar Dance differ only in the kind-0x1f field (2 or 1). */
static void wish_replacement(uint32_t context, uint32_t id, uint32_t field, const struct px_battle_moves_b3_api *api)
{
    if (get(0x03, api) != id) return;
    if (api->can_switch(user(api), context, id) == 0) return;
    uint32_t rec = record(context, 0x1f, id, api);
    store8(rec + 4, field, api);
    store8(rec + 5, api->base->side_of(user(api), context, id), api);
    commit(context, rec, api);
    rec = record(context, 0x12, id, api);
    store8(rec + 4, id, api);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B3_HANDLER(px_battle_healing_wish_effect)
{
    (void)handler, (void)work;
    wish_replacement(context, id, 2, api);
}

PX_BATTLE_MOVES_B3_HANDLER(px_battle_lunar_dance_effect)
{
    (void)handler, (void)work;
    wish_replacement(context, id, 1, api);
}

PX_BATTLE_MOVES_B3_HANDLER(px_battle_helping_hand_effect)
{
    /* Needs a partner (the owner of the user has more than one battler). */
    if (api->owner_count(user(api), context, api->owner_of(user(api), id)) <= 1) return;
    if (get(0x03, api) != id) return;
    const uint32_t target = get(0x06, api) & 0xff;
    const uint32_t other = pokemon(context, target, api);
    if (api->fainted(user(api), other) != 0) return;
    if (api->turn_flag(user(api), other, 1) != 0) return;
    const uint32_t rec = announce(context, id, 0x41a, api);
    api->message_arg(user(api), rec + 4, target);
    commit(context, rec, api);
    store32(work, target, api);
    px_battle_item_mark_consumed(handler, memory(api));
}
