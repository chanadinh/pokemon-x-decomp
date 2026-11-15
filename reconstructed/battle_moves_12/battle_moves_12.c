#include "battle_moves_12.h"

enum { BATTLER = 0x02, ATTACKER = 0x03, LOCK = 0x18 };

static void *user(const struct px_battle_moves_12_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_12_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_12_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t set(uint32_t key, uint32_t value, const struct px_battle_moves_12_api *api)
{
    return px_battle_event_rewrite(key, value, api->base->events);
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_moves_12_api *api)
{
    return api->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t rec, const struct px_battle_moves_12_api *api)
{
    api->commit_actions(user(api), context, rec);
}

static void store8(uint32_t at, uint32_t value, const struct px_battle_moves_12_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store8(m->user, at, (uint8_t)value);
}

static uint32_t load32(uint32_t at, const struct px_battle_moves_12_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load32(m->user, at);
}

static void store32(uint32_t at, uint32_t value, const struct px_battle_moves_12_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store32(m->user, at, value);
}

/* Charging turn: the action data (key 0x40, five words) is copied to the
 * global state; the target is lifted (message literal 108b96a8, naming both)
 * or the move fails (key 0x42, and key 0x50 when the lift reported a block). */
PX_BATTLE_MOVES_12_HANDLER(px_battle_sky_drop_lift)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(ATTACKER, api) != id) return;
    const uint32_t source = get(0x40, api);
    uint32_t words[5];   /* ldmia then stmia: all five read before any is written */
    for (uint32_t i = 0; i < 5; ++i) words[i] = load32(source + 4 * i, api);
    for (uint32_t i = 0; i < 5; ++i) store32(api->sky_drop_state + 4 * i, words[i], api);
    const uint32_t target = get(0x06, api) & 0xff;
    store8(api->blocked_scratch, 0, api);
    if (target != 0x1f &&
        api->sky_drop_lift(user(api), context, id, target, api->sky_drop_state, api->blocked_scratch) != 0) {
        if (set(0x53, 1, api) == 0) return;
        const uint32_t rec = record(context, 3, id, api);
        api->message(user(api), rec + 4, 2, 0x464);
        api->message_arg(user(api), rec + 4, id);
        api->message_arg(user(api), rec + 4, target);
        commit(context, rec, api);
        return;
    }
    set(0x42, 1, api);
    if (m->load8(m->user, api->blocked_scratch) != 0) set(0x50, 1, api);
}

/* Release turn: the carried battler (counter 4, minus 1) must still be there
 * and have an opposing side; otherwise the move fails. Either way the user
 * lands and work words 0 and 1 record the drop. */
PX_BATTLE_MOVES_12_HANDLER(px_battle_sky_drop_drop)
{
    (void)handler;
    if (get(ATTACKER, api) != id) return;
    const uint32_t carried = api->counter(user(api), px_battle_pokemon_by_id(context, id, api->base), 4);
    uint32_t target = 0x1f;
    int fails = 1;
    if (carried != 0) {
        const uint32_t candidate = (carried - 1) & 0xff;
        if (candidate < 0x18) {
            target = candidate;
            fails = api->opposing_selector(user(api), context, candidate) == 0xa;
        }
    }
    if (fails) set(0x42, 1, api);
    api->sky_drop_land(user(api), context, id, 0, 0);
    store32(work, 1, api);
    store32(work + 4, target, api);
}

/* Missed (key 0x02) or interrupted (key 0x03): the carried battler (word 1) is released once. */
static void release(uint32_t key, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_12_api *api)
{
    if (get(key, api) != id) return;
    if (load32(work, api) == 0) return;
    api->sky_drop_release(user(api), context, load32(work + 4, api) & 0xff);
    store32(work, 0, api);
}

PX_BATTLE_MOVES_12_HANDLER(px_battle_sky_drop_missed) { (void)handler; release(BATTLER, context, id, work, api); }
PX_BATTLE_MOVES_12_HANDLER(px_battle_sky_drop_cancelled) { (void)handler; release(ATTACKER, context, id, work, api); }

/* Bide's lock ends with the move (when word 6 is set). */
PX_BATTLE_MOVES_12_HANDLER(px_battle_bide_end)
{
    (void)handler;
    if (get(BATTLER, api) != id) return;
    if (load32(work + LOCK, api) == 0) return;
    const uint32_t rec = record(context, 0xa, id, api);
    store8(rec + 4, 0x19, api);
    store8(rec + 0x11, 1, api);
    store8(rec + 5, id, api);
    store8(rec + 0x12, 1, api);
    commit(context, rec, api);
    store32(work + LOCK, 0, api);
}

/* The protect streak resets and the handler is removed. */
PX_BATTLE_MOVES_12_HANDLER(px_battle_protect_end)
{
    (void)work;
    if (get(BATTLER, api) != id) return;
    const uint32_t rec = record(context, 0x27, id, api);
    store8(rec + 5, 3, api);
    store8(rec + 4, id, api);
    store8(rec + 6, 0, api);
    commit(context, rec, api);
    px_battle_handler_remove(handler, api->handlers);
}

/* Rollout and Ice Ball: the lock ends and every handler of the move for this battler goes. */
PX_BATTLE_MOVES_12_HANDLER(px_battle_rollout_end)
{
    const struct px_pokemon_api *m = memory(api);
    if (get(BATTLER, api) != id) return;
    const uint32_t rec = record(context, 0xa, id, api);
    store8(rec + 5, id, api);
    store8(rec + 0x11, 1, api);
    store8(rec + 4, 0x19, api);
    commit(context, rec, api);
    store32(work + LOCK, 0, api);
    const uint32_t move = m->load16(m->user, handler + UINT32_C(0x38));
    for (uint32_t h = px_battle_handler_find(0, id, api->handlers); h != 0;) {
        if (m->load16(m->user, h + UINT32_C(0x38)) == move) {
            px_battle_handler_remove(h, api->handlers);
            h = px_battle_handler_find(0, id, api->handlers);
        } else {
            h = px_battle_handler_find_next(h, api->handlers);
        }
    }
}

/* Jump Kick and High Jump Kick: crash damage of half the user's HP (1084d2f4), message 0x38c. */
PX_BATTLE_MOVES_12_HANDLER(px_battle_jump_kick_crash)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(BATTLER, api) != id) return;
    const uint32_t damage = api->hp_fraction(user(api), px_battle_pokemon_by_id(context, id, api->base), 2);
    const uint32_t rec = record(context, 6, id, api);
    store8(rec + 6, id, api);
    m->store16(m->user, rec + 4, (uint16_t)damage);
    api->message(user(api), rec + 0xc, 2, 0x38c);
    api->message_arg(user(api), rec + 0xc, id);
    commit(context, rec, api);
}

/* Heal Bell and Aromatherapy (no key test): their message (kind 1). */
static void done_message(uint32_t context, uint32_t id, uint32_t message, const struct px_battle_moves_12_api *api)
{
    const uint32_t rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 1, message);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_12_HANDLER(px_battle_heal_bell_done) { (void)handler, (void)work; done_message(context, id, 0x73, api); }
PX_BATTLE_MOVES_12_HANDLER(px_battle_aromatherapy_done) { (void)handler, (void)work; done_message(context, id, 0x74, api); }

/* A combined pledge (word 0) announces itself (message 0xc1). */
PX_BATTLE_MOVES_12_HANDLER(px_battle_pledge_done)
{
    (void)handler;
    if (get(ATTACKER, api) != id) return;
    if (load32(work, api) == 0) return;
    done_message(context, id, 0xc1, api);
}
