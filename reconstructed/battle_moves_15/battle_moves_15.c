#include "battle_moves_15.h"

enum { BATTLER = 0x02, ATTACKER = 0x03, DEFENDER = 0x04, MOVE = 0x12, STATE = 0x23, FAILED = 0x42, DONE = 0x53 };

static void *user(const struct px_battle_moves_15_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_15_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_15_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t set(uint32_t key, uint32_t value, const struct px_battle_moves_15_api *api)
{
    return px_battle_event_rewrite(key, value, api->base->events);
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_moves_15_api *api)
{
    return api->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t rec, const struct px_battle_moves_15_api *api)
{
    api->commit_actions(user(api), context, rec);
}

static uint32_t load8(uint32_t at, const struct px_battle_moves_15_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load8(m->user, at);
}

static void store8(uint32_t at, uint32_t value, const struct px_battle_moves_15_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store8(m->user, at, (uint8_t)value);
}

static void store16(uint32_t at, uint32_t value, const struct px_battle_moves_15_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store16(m->user, at, (uint16_t)value);
}

static uint32_t load32(uint32_t at, const struct px_battle_moves_15_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load32(m->user, at);
}

static void store32(uint32_t at, uint32_t value, const struct px_battle_moves_15_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store32(m->user, at, value);
}

static uint32_t battler(uint32_t context, uint32_t id, const struct px_battle_moves_15_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

/* A message naming the battler (record 3, kind 2), committed. */
static void message(uint32_t context, uint32_t id, uint32_t text, const struct px_battle_moves_15_api *api)
{
    const uint32_t rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 2, text);
    api->message_arg(user(api), rec + 4, id);
    commit(context, rec, api);
}

/* Focus Punch tightens its focus (record 0x15, kind 5); unless the state
 * check holds, record 0x38 (halfword 0x2ad, literal 108b23b8, the user's
 * side); then message 0x268. */
PX_BATTLE_MOVES_15_HANDLER(px_battle_focus_punch_start)
{
    (void)handler, (void)work;
    if (get(BATTLER, api) != id) return;
    uint32_t rec = record(context, 0x15, id, api);
    store8(rec + 5, id, api);
    store8(rec + 4, 5, api);
    commit(context, rec, api);
    if (api->state_check(user(api), battler(context, id, api)) == 0) {
        rec = record(context, 0x38, id, api);
        store16(rec + 4, 0x2ad, api);
        store8(rec + 6, api->base->side_of(user(api), context, id), api);
        store8(rec + 7, 0xa, api);
        store8(rec + 0xb, 1, api);
        commit(context, rec, api);
    }
    message(context, id, 0x268, api);
}

/* Snatch takes another battler's move with flag 5, unless the user has
 * status 0x21: keys 0x02 and 0x04 become the user, and work word 0 is set
 * (once key 0x02 is rewritten). */
PX_BATTLE_MOVES_15_HANDLER(px_battle_snatch_steal)
{
    (void)handler;
    if ((get(ATTACKER, api) & 0xff) == id) return;
    if (api->move_flag(user(api), get(MOVE, api) & 0xffff, 5) == 0) return;
    if (api->has_status(user(api), battler(context, id, api), 0x21) != 0) return;
    if (set(BATTLER, id, api) == 0) return;
    set(DEFENDER, id, api);
    store32(work, 1, api);
}

/* Heal Bell, Helping Hand and Aromatherapy: key 0x53 set. */
PX_BATTLE_MOVES_15_HANDLER(px_battle_support_done)
{
    (void)handler, (void)context, (void)work;
    if (get(ATTACKER, api) != id) return;
    set(DONE, 1, api);
}

/* Snore and Sleep Talk fail when the user is awake (key 0x23 byte 2). */
PX_BATTLE_MOVES_15_HANDLER(px_battle_snore_awake)
{
    (void)handler, (void)context, (void)work;
    if (get(BATTLER, api) != id) return;
    if ((get(STATE, api) & 0xff) != 2) return;
    set(FAILED, 1, api);
}

/* Struggle: message 0x347 (literal 108b9e74). */
PX_BATTLE_MOVES_15_HANDLER(px_battle_struggle_message)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    message(context, id, 0x347, api);
}

/* Roost: condition 0x18 (record 0xb), data 1084beac(1, 2) flagged by 10876ca8(+8, 1). */
PX_BATTLE_MOVES_15_HANDLER(px_battle_roost_land)
{
    (void)handler, (void)work;
    if (get(BATTLER, api) != id) return;
    const uint32_t rec = record(context, 0xb, id, api);
    store8(rec + 0xf, id, api);
    store8(rec + 0xd, 1, api);
    store16(rec + 4, 0x18, api);
    store32(rec + 8, api->lock_count(user(api), 1, 2), api);
    api->condition_flag(user(api), rec + 8, 1);
    commit(context, rec, api);
}

/* Perish Song: message 0x7d (kind 1, no argument). */
PX_BATTLE_MOVES_15_HANDLER(px_battle_perish_song_message)
{
    (void)handler, (void)work;
    if (get(BATTLER, api) != id) return;
    const uint32_t rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 1, 0x7d);
    commit(context, rec, api);
}

/* A stockpiled stat boost is undone (record 0xd: stat, minus the stages, flag bit 2). */
static void unboost(uint32_t context, uint32_t id, uint32_t stat, uint32_t stages, const struct px_battle_moves_15_api *api)
{
    const uint32_t rec = record(context, 0xd, id, api);
    const uint32_t flags = load8(rec + 0x18, api);
    store8(rec + 4, stat, api);
    store8(rec + 0x16, 1, api);
    store8(rec + 0x17, 0 - stages, api);
    store8(rec + 0x18, flags | 4, api);
    store8(rec + 0xc, id, api);
    commit(context, rec, api);
}

static void reset_counter(uint32_t context, uint32_t id, uint32_t index, const struct px_battle_moves_15_api *api)
{
    const uint32_t rec = record(context, 0x27, id, api);
    store8(rec + 5, index, api);
    store8(rec + 4, id, api);
    store8(rec + 6, 0, api);
    commit(context, rec, api);
}

/* Spit Up and Swallow: the Defense (counter 1, stat 2) and Sp. Def (counter
 * 2, stat 4) stages Stockpile gave are removed and their counters reset;
 * with a stockpile (counter 0) it resets too, with message 0x2d4. All three
 * counters are read first. */
PX_BATTLE_MOVES_15_HANDLER(px_battle_stockpile_release)
{
    (void)handler, (void)work;
    if (get(BATTLER, api) != id) return;
    const uint32_t pokemon = battler(context, id, api);
    const uint32_t defense = api->counter(user(api), pokemon, 1);
    const uint32_t special = api->counter(user(api), pokemon, 2);
    const uint32_t stock = api->counter(user(api), pokemon, 0);
    if (defense != 0) {
        unboost(context, id, 2, defense, api);
        reset_counter(context, id, 1, api);
    }
    if (special != 0) {
        unboost(context, id, 4, special, api);
        reset_counter(context, id, 2, api);
    }
    if (stock == 0) return;
    reset_counter(context, id, 0, api);
    message(context, id, 0x2d4, api);
}

/* The held item is used up (record 0x24). */
static void use_item(uint32_t context, uint32_t id, const struct px_battle_moves_15_api *api)
{
    const uint32_t rec = record(context, 0x24, id, api);
    store8(rec + 4, 1, api);
    store8(rec + 5, 1, api);
    commit(context, rec, api);
}

/* Fling: unless already handled (work word 0), a held item (+0x12) is used up. */
PX_BATTLE_MOVES_15_HANDLER(px_battle_fling_item)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    if (get(BATTLER, api) != id || load32(work, api) != 0) return;
    if (m->load16(m->user, battler(context, id, api) + 0x12) == 0) return;
    use_item(context, id, api);
}

/* Natural Gift: the item is used up. */
PX_BATTLE_MOVES_15_HANDLER(px_battle_natural_gift_item)
{
    (void)handler, (void)work;
    if (get(BATTLER, api) != id) return;
    use_item(context, id, api);
}

/* Self-Destruct and Explosion: record 0x12 (the user faints) after the move. */
PX_BATTLE_MOVES_15_HANDLER(px_battle_explosion_done)
{
    (void)handler, (void)work;
    if (get(BATTLER, api) != id) return;
    const uint32_t rec = record(context, 0x12, id, api);
    store8(rec + 4, id, api);
    commit(context, rec, api);
}
