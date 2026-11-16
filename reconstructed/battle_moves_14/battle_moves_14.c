#include "battle_moves_14.h"

enum { BATTLER = 0x02, ATTACKER = 0x03, KIND = 0x0c, TARGET = 0x0d, MOVE = 0x12, FAILED = 0x42, DONE = 0x53, ACTIVE = 0x18 };

static void *user(const struct px_battle_moves_14_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_14_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_14_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t set(uint32_t key, uint32_t value, const struct px_battle_moves_14_api *api)
{
    return px_battle_event_rewrite(key, value, api->base->events);
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_moves_14_api *api)
{
    return api->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t rec, const struct px_battle_moves_14_api *api)
{
    api->commit_actions(user(api), context, rec);
}

static uint32_t load8(uint32_t at, const struct px_battle_moves_14_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load8(m->user, at);
}

static uint32_t load16(uint32_t at, const struct px_battle_moves_14_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load16(m->user, at);
}

static void store8(uint32_t at, uint32_t value, const struct px_battle_moves_14_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store8(m->user, at, (uint8_t)value);
}

static void store16(uint32_t at, uint32_t value, const struct px_battle_moves_14_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store16(m->user, at, (uint16_t)value);
}

static uint32_t load32(uint32_t at, const struct px_battle_moves_14_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load32(m->user, at);
}

static void store32(uint32_t at, uint32_t value, const struct px_battle_moves_14_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store32(m->user, at, value);
}

static uint32_t battler(uint32_t context, uint32_t id, const struct px_battle_moves_14_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

/* Every handler of owner 0 and value byte `value` whose move (+0x38) is this
 * handler's is removed; the search restarts after each removal. */
static void remove_move(uint32_t handler, uint32_t value, const struct px_battle_moves_14_api *api)
{
    const uint32_t move = load16(handler + 0x38, api);
    for (uint32_t h = px_battle_handler_find(0, value, api->handlers); h != 0;) {
        if (load16(h + 0x38, api) == move) {
            px_battle_handler_remove(h, api->handlers);
            h = px_battle_handler_find(0, value, api->handlers);
        } else {
            h = px_battle_handler_find_next(h, api->handlers);
        }
    }
}

/* The lock of condition 0x19 ends (record 0xa). */
static void cure_lock(uint32_t context, uint32_t id, const struct px_battle_moves_14_api *api)
{
    const uint32_t rec = record(context, 0xa, id, api);
    store8(rec + 4, 0x19, api);
    store8(rec + 5, id, api);
    store8(rec + 0x11, 1, api);
    commit(context, rec, api);
}

/* A message naming the battler (record 3, kind 2). */
static uint32_t message(uint32_t context, uint32_t id, uint32_t text, const struct px_battle_moves_14_api *api)
{
    const uint32_t rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 2, text);
    api->message_arg(user(api), rec + 4, id);
    return rec;
}

/* Rage ends (work word 6 set) when its user acts with an action kind other than 8. */
PX_BATTLE_MOVES_14_HANDLER(px_battle_rage_end)
{
    if (load32(work + ACTIVE, api) == 0) return;
    if (get(BATTLER, api) != id) return;
    if (get(KIND, api) == 8) return;
    remove_move(handler, load8(battler(context, id, api) + 0x19, api), api);
}

/* Destiny Bond and Grudge end in the same case (their own handler only). */
PX_BATTLE_MOVES_14_HANDLER(px_battle_destiny_bond_end)
{
    (void)context, (void)work;
    if (get(BATTLER, api) != id) return;
    if (get(KIND, api) == 8) return;
    px_battle_handler_remove(handler, api->handlers);
}

/* Charge: using another move marks the charge to end (work word 0 = 2). */
PX_BATTLE_MOVES_14_HANDLER(px_battle_charge_check)
{
    (void)context;
    if (get(ATTACKER, api) != id) return;
    if (get(MOVE, api) != load16(handler + 0x38, api)) store32(work, 2, api);
}

/* The protect streak resets (record 0x27, counter 3) unless the user's last
 * move is one of the eight protect-type moves (tested two per round). */
PX_BATTLE_MOVES_14_HANDLER(px_battle_protect_streak)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t move = api->last_move(user(api), battler(context, id, api));
    for (uint32_t i = 0; i < 8; i += 2) {
        if (load16(api->protect_moves + 2 * i, api) == move || load16(api->protect_moves + 2 * i + 2, api) == move) return;
    }
    const uint32_t rec = record(context, 0x27, id, api);
    store8(rec + 5, 3, api);
    store8(rec + 4, id, api);
    store8(rec + 6, 0, api);
    commit(context, rec, api);
}

/* Charge ends once marked. */
PX_BATTLE_MOVES_14_HANDLER(px_battle_charge_end)
{
    (void)context;
    if (get(BATTLER, api) != id) return;
    if (load32(work, api) != 2) return;
    px_battle_handler_remove(handler, api->handlers);
}

/* Uproar interrupted (key 0x53 clear): the lock ends. */
PX_BATTLE_MOVES_14_HANDLER(px_battle_uproar_interrupted)
{
    (void)handler, (void)work;
    if (get(BATTLER, api) != id) return;
    if (get(DONE, api) != 0) return;
    cure_lock(context, id, api);
}

/* Thrash, Petal Dance, Outrage: a turn of the rampage passes (work word 0
 * counts down); interrupted (key 0x53 clear), the lock ends. On the last turn
 * the user becomes confused (condition 6, message 0x168) and the move's
 * handlers go; interrupted earlier, they go without confusion. */
PX_BATTLE_MOVES_14_HANDLER(px_battle_thrash_turn)
{
    if (get(BATTLER, api) != id) return;
    const uint32_t pokemon = battler(context, id, api);
    if (load32(work + ACTIVE, api) == 0) return;
    if (load32(work, api) != 0) store32(work, load32(work, api) - 1, api);
    int ended = 0;
    if (get(DONE, api) == 0) {
        cure_lock(context, id, api);
        ended = 1;
    }
    if (load32(work, api) == 0) {
        const uint32_t rec = record(context, 0xb, id, api);
        store16(rec + 4, 6, api);
        api->condition_for(user(api), 6, pokemon, rec + 8);
        store8(rec + 0xf, id, api);
        store8(rec + 0xd, 1, api);
        api->message(user(api), rec + 0x14, 2, 0x168);
        api->message_arg(user(api), rec + 0x14, id);
        commit(context, rec, api);
    } else if (!ended) {
        return;
    }
    remove_move(handler, load8(pokemon + 0x19, api), api);
}

/* Rollout and Ice Ball interrupted: the lock ends, work word 6 clears and the move's handlers go. */
PX_BATTLE_MOVES_14_HANDLER(px_battle_rollout_interrupted)
{
    if (get(BATTLER, api) != id) return;
    if (get(DONE, api) != 0) return;
    cure_lock(context, id, api);
    store32(work + ACTIVE, 0, api);
    remove_move(handler, id, api);
}

/* Future Sight and Doom Desire (one routine twice): once announced (key 0x53
 * set), the attack is scheduled for the turn after next (record 0x1f); with
 * no target (0xa) one is chosen for the user's side and written to key 0x0d. */
static void schedule(uint32_t handler, uint32_t context, uint32_t id, const struct px_battle_moves_14_api *api)
{
    if (get(ATTACKER, api) != id) return;
    uint32_t target = get(TARGET, api) & 0xff;
    if (get(ATTACKER, api) != id) return;
    if (set(DONE, 1, api) == 0) return;
    const uint32_t side = api->base->side_of(user(api), context, id);
    if (target == 0xa) {
        target = api->target_selector(user(api), api->battle_mode(user(api), context), side, 0);
        set(TARGET, target, api);
    }
    const uint32_t rec = record(context, 0x1f, id, api);
    store8(rec + 5, target, api);
    store8(rec + 4, 3, api);
    store32(rec + 8, api->turn(user(api), context) + 2, api);
    store32(rec + 0xc, load16(handler + 0x38, api), api);
    store8(rec + 0x18, 3, api);
    store32(rec + 0x10, side, api);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_14_HANDLER(px_battle_future_sight_set) { (void)work; schedule(handler, context, id, api); }
PX_BATTLE_MOVES_14_HANDLER(px_battle_doom_desire_set) { (void)work; schedule(handler, context, id, api); }

/* Their announcements: message 0x438 (literal 108b8478) and 0x43b (literal 108bab1c). */
static void announce(uint32_t context, uint32_t id, uint32_t text, const struct px_battle_moves_14_api *api)
{
    if (get(ATTACKER, api) != id) return;
    commit(context, message(context, id, text, api), api);
}

PX_BATTLE_MOVES_14_HANDLER(px_battle_future_sight_message) { (void)handler, (void)work; announce(context, id, 0x438, api); }
PX_BATTLE_MOVES_14_HANDLER(px_battle_doom_desire_message) { (void)handler, (void)work; announce(context, id, 0x43b, api); }

/* Snatch takes a move (work word 0 set): message 0x2f2 (literal 108b6524)
 * naming the user and the attacker; the handler removes itself. */
PX_BATTLE_MOVES_14_HANDLER(px_battle_snatch_taken)
{
    if (get(BATTLER, api) != id) return;
    if (load32(work, api) == 0) return;
    const uint32_t rec = message(context, id, 0x2f2, api);
    api->message_arg(user(api), rec + 4, get(ATTACKER, api));
    commit(context, rec, api);
    px_battle_handler_remove(handler, api->handlers);
}

/* Magic Coat reflects (key 0x53 taken), unless the user cannot (1084b804): message 0x2fc naming the user and the move. */
PX_BATTLE_MOVES_14_HANDLER(px_battle_magic_coat_reflect)
{
    (void)handler, (void)work;
    if (get(BATTLER, api) != id) return;
    if (set(DONE, 1, api) == 0) return;
    if (get(BATTLER, api) != id) return;
    if (api->reflection_blocked(user(api), battler(context, id, api)) != 0) return;
    const uint32_t rec = message(context, id, 0x2fc, api);
    api->message_arg(user(api), rec + 4, get(MOVE, api));
    commit(context, rec, api);
}

/* Teleport (no key test): once key 0x53 is taken, message 0x2ff (literal 108b79bc). */
PX_BATTLE_MOVES_14_HANDLER(px_battle_teleport)
{
    (void)handler, (void)work;
    if (set(DONE, 1, api) == 0) return;
    commit(context, message(context, id, 0x2ff, api), api);
}

/* Uproar already started (work word 6): the move fails (key 0x42). */
PX_BATTLE_MOVES_14_HANDLER(px_battle_uproar_fail)
{
    (void)handler, (void)context, (void)id;
    if (load32(work + ACTIVE, api) != 0) set(FAILED, 1, api);
}
