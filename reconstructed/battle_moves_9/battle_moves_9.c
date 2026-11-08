#include "battle_moves_9.h"
#include "../battle_moves_5/battle_moves_5.h"

enum { BATTLER = 0x02, ATTACKER = 0x03, LOCK = 0x3f, WORD6 = 0x18 };

static void *user(const struct px_battle_moves_9_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_9_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_9_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t battler(uint32_t context, uint32_t id, const struct px_battle_moves_9_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_moves_9_api *api)
{
    return api->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t rec, const struct px_battle_moves_9_api *api)
{
    api->commit_actions(user(api), context, rec);
}

static uint32_t load32(uint32_t at, const struct px_battle_moves_9_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load32(m->user, at);
}

static void store32(uint32_t at, uint32_t value, const struct px_battle_moves_9_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store32(m->user, at, value);
}

static void store16(uint32_t at, uint32_t value, const struct px_battle_moves_9_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store16(m->user, at, (uint16_t)value);
}

static void store8(uint32_t at, uint32_t value, const struct px_battle_moves_9_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store8(m->user, at, (uint8_t)value);
}

static uint32_t move_of(uint32_t handler, const struct px_battle_moves_9_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load16(m->user, handler + UINT32_C(0x38));
}

static uint32_t using(uint32_t id, const struct px_battle_moves_9_api *api)
{
    return get(BATTLER, api) == id;
}

/* Bide: word 0 counts the turns; the first stores (and locks) the battler,
 * the second keeps the lock, the third releases it (effect flag 1) and ends
 * the lock condition when word 6 is set. */
PX_BATTLE_MOVES_9_HANDLER(px_battle_bide_use)
{
    if (!using(id, api)) return;
    const uint32_t turn = load32(work, api);
    if (turn == 0) {
        const uint32_t rec = record(context, 0xb, id, api);
        store16(rec + 4, 0x19, api);
        store32(rec + 8, api->move_count(user(api), move_of(handler, api)), api);
        store8(rec + 0xf, id, api);
        commit(context, rec, api);
        px_battle_event_rewrite(LOCK, 2, api->base->events);
        store32(work + WORD6, 1, api);
        store32(work, 1, api);
        store32(work + 4, 0, api);
        return;
    }
    if (turn == 1) {
        px_battle_event_rewrite(LOCK, 2, api->base->events);
        store32(work, 2, api);
        return;
    }
    api->set_effect_flag(user(api), context, 1);
    if (load32(work + WORD6, api) == 0) return;
    const uint32_t rec = record(context, 0xa, id, api);
    store8(rec + 4, 0x19, api);
    store8(rec + 0x11, 1, api);
    store8(rec + 5, id, api);
    store8(rec + 0x12, 1, api);
    commit(context, rec, api);
    store32(work + WORD6, 0, api);
}

/* Locks the battler into its move for `turns` turns (condition 0x19). */
static void lock(uint32_t handler, uint32_t context, uint32_t id, uint32_t turns, const struct px_battle_moves_9_api *api)
{
    const uint32_t rec = record(context, 0xb, id, api);
    store16(rec + 4, 0x19, api);
    store32(rec + 8, api->lock_count(user(api), turns, move_of(handler, api)), api);
    store8(rec + 0xc, 0, api);
    store8(rec + 0xf, id, api);
    commit(context, rec, api);
}

/* Thrash, Petal Dance, Outrage: 2 or 3 turns unless already locked. */
PX_BATTLE_MOVES_9_HANDLER(px_battle_thrash_use)
{
    if (!using(id, api)) return;
    if (api->has_status(user(api), battler(context, id, api), 0x19) != 0 || load32(work + WORD6, api) != 0) return;
    const uint32_t turns = (api->random(user(api), 2) + 2) & 0xff;
    lock(handler, context, id, turns, api);
    store32(work + WORD6, 1, api);
    store32(work, turns, api);
}

/* Rollout and Ice Ball: 5 turns unless locked or asleep; word 1 = 5, word 0 = 0. */
PX_BATTLE_MOVES_9_HANDLER(px_battle_rollout_use)
{
    if (!using(id, api)) return;
    const uint32_t pokemon = battler(context, id, api);
    if (api->has_status(user(api), pokemon, 0x19) != 0) return;
    if (api->has_status(user(api), pokemon, 2) != 0 || load32(work + WORD6, api) != 0) return;
    lock(handler, context, id, 5, api);
    store32(work + WORD6, 1, api);
    store32(work + 4, 5, api);
    store32(work, 0, api);
}

/* Rage: kept while the battler uses it again (word 6 set); any other move
 * removes this battler's Rage handlers. */
PX_BATTLE_MOVES_9_HANDLER(px_battle_rage_use)
{
    const struct px_pokemon_api *m = memory(api);
    if (!using(id, api)) return;
    if (get(0x12, api) == move_of(handler, api)) {
        store32(work + WORD6, 1, api);
        return;
    }
    const uint32_t pokemon = battler(context, id, api);
    const uint32_t move = move_of(handler, api);
    const uint32_t own = m->load8(m->user, pokemon + UINT32_C(0x19));
    for (uint32_t h = px_battle_handler_find(0, own, api->handlers); h != 0;) {
        if (m->load16(m->user, h + UINT32_C(0x38)) == move) {
            px_battle_handler_remove(h, api->handlers);
            h = px_battle_handler_find(0, own, api->handlers);
        } else {
            h = px_battle_handler_find_next(h, api->handlers);
        }
    }
}

/* Charge: after its own use, message 0x298, lock state 1 and words 6 and 0 set. */
PX_BATTLE_MOVES_9_HANDLER(px_battle_charge_use)
{
    if (!using(id, api)) return;
    if (get(0x12, api) != move_of(handler, api)) return;
    const uint32_t rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 2, 0x298);
    api->message_arg(user(api), rec + 4, id);
    commit(context, rec, api);
    px_battle_event_rewrite(LOCK, 1, api->base->events);
    store32(work + WORD6, 1, api);
    store32(work, 1, api);
}

/* Round: the first user this turn records the move (kind 0x31). */
PX_BATTLE_MOVES_9_HANDLER(px_battle_round_use)
{
    (void)work;
    if (!using(id, api)) return;
    const uint32_t history = context + UINT32_C(0x20);
    const uint32_t move = move_of(handler, api);
    if (api->move_users(user(api), history, move, api->turn(user(api), context)) != 1) return;
    const uint32_t rec = record(context, 0x31, id, api);
    store16(rec + 4, move, api);
    commit(context, rec, api);
}

/* Toxic used by a Poison type: condition 0x1c on the user for one turn. */
PX_BATTLE_MOVES_9_HANDLER(px_battle_toxic_use)
{
    (void)handler, (void)work;
    if (!using(id, api)) return;
    if (api->has_type(user(api), battler(context, id, api), 3) == 0) return;
    const uint32_t rec = record(context, 0xb, id, api);
    store8(rec + 0xf, id, api);
    store16(rec + 4, 0x1c, api);
    store32(rec + 8, px_battle_turn_count(1), api);
    commit(context, rec, api);
}

static void state(uint32_t context, uint32_t id, uint32_t value, const struct px_battle_moves_9_api *api)
{
    if (!using(id, api)) return;
    const uint32_t rec = record(context, 0x17, id, api);
    store8(rec + 5, id, api);
    store8(rec + 4, value, api);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_9_HANDLER(px_battle_defense_curl_use) { (void)handler, (void)work; state(context, id, 7, api); }
PX_BATTLE_MOVES_9_HANDLER(px_battle_minimize_use) { (void)handler, (void)work; state(context, id, 8, api); }

/* Table 108d4aa8 (literal 108b8400): (random limit, power) for magnitudes 4..10,
 * followed by the two bytes after it. */
static const uint8_t magnitude_table[8][2] = {{5, 10}, {15, 30}, {35, 50}, {65, 70}, {85, 90}, {95, 110}, {100, 150}, {0, 0}};

PX_BATTLE_MOVES_9_HANDLER(px_battle_magnitude_use)
{
    (void)handler;
    if (!using(id, api)) return;
    const uint32_t roll = api->random(user(api), 100) & 0xff;
    uint32_t i = 0;
    while (magnitude_table[i][0] <= roll) {
        i = (i + 1) & 0xff;
        if (i >= 7) break;
    }
    store32(work, magnitude_table[i][1], api);
    const uint32_t rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 1, i + 0x6c);
    commit(context, rec, api);
}

/* A combined pledge (word 0 set) chooses the follow-up move by word 1:
 * 1 518 (literal 108b9ffc), 2 519 (literal 108ba000), 3 520, otherwise 0. */
PX_BATTLE_MOVES_9_HANDLER(px_battle_pledge_use)
{
    (void)handler, (void)context;
    if (!using(id, api)) return;
    if (load32(work, api) == 0) return;
    const uint32_t kind = load32(work + 4, api);
    px_battle_event_rewrite(0x13, kind == 1 ? 0x206 : kind == 2 ? 0x207 : kind == 3 ? 0x208 : 0, api->base->events);
}

PX_BATTLE_MOVES_9_HANDLER(px_battle_secret_power_use)
{
    (void)handler, (void)work;
    if (!using(id, api)) return;
    api->set_effect_flag(user(api), context, px_battle_secret_power_effect(context, 0, 0, api));
}

/* Selfdestruct and Explosion (key 0x03): effect flag 1, then 1081cf14 with the user's side twice. */
PX_BATTLE_MOVES_9_HANDLER(px_battle_explosion_use)
{
    (void)work;
    if (get(ATTACKER, api) != id) return;
    api->set_effect_flag(user(api), context, 1);
    const uint32_t move = move_of(handler, api);
    const uint32_t side = api->base->side_of(user(api), context, id);
    api->explode(user(api), context, side, side, move, 0, 0);
}

/* Environment 0..7 through the jump table at 108bb8cc (relocated words), then
 * terrain 1, 2, 3 override: (class, value, animation). */
uint32_t px_battle_secret_power_effect(uint32_t context, uint32_t out_class, uint32_t out_value,
                                       const struct px_battle_moves_9_api *api)
{
    uint32_t cls, value, animation;
    const uint32_t environment = api->environment(user(api), context);
    switch (environment) {
    case 1: cls = 1, value = 6, animation = 1; break;
    case 2: cls = 2, value = 0, animation = 6; break;
    case 3: cls = 0, value = 2, animation = 7; break;
    case 4: cls = 1, value = 1, animation = 5; break;
    case 5: cls = 1, value = 5, animation = 2; break;
    case 6: case 7: cls = 0, value = 3, animation = environment == 6 ? 3 : 4; break;
    default: cls = 0, value = 1, animation = 0; break;
    }
    switch (api->terrain(user(api), context)) {
    case 1: cls = 0, value = 2, animation = 7; break;
    case 2: cls = 1, value = 3, animation = 8; break;
    case 3: cls = 0, value = 1, animation = 9; break;
    default: break;
    }
    if (out_class != 0) store8(out_class, cls, api);
    if (out_value != 0) store8(out_value, value, api);
    return animation;
}

/* Event 0x8c: when the added effect rolls (30%), a condition, a stat drop or a flinch. */
PX_BATTLE_MOVES_9_HANDLER(px_battle_secret_power_hit)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    const uint32_t out_value = api->effect_scratch, out_class = api->effect_scratch + 4;
    if (get(ATTACKER, api) != id) return;
    if (get(0x48, api) != 0) return;
    const uint32_t t = get(0x06, api) & 0xff;
    if (api->effect_roll(user(api), context, id, t, 0x1e) == 0) return;
    const uint32_t attacker = battler(context, id, api);
    (void)px_battle_secret_power_effect(context, out_class, out_value, api);
    const uint32_t cls = m->load8(m->user, out_class);
    if (cls == 0) {
        const uint32_t rec = record(context, 0xb, id, api);
        store8(rec + 0xf, t, api);
        const uint32_t status = m->load8(m->user, out_value);
        store16(rec + 4, status, api);
        api->status_value(user(api), status, attacker, rec + 8);
        commit(context, rec, api);
    } else if (cls == 1) {
        const uint32_t rec = record(context, 0xd, id, api);
        store8(rec + 0xc, t, api);
        store8(rec + 0x16, 1, api);
        const uint32_t stat = m->load8(m->user, out_value);
        const uint32_t flags = m->load8(m->user, rec + 0x18);
        store8(rec + 4, stat, api);
        store8(rec + 0x17, 0xff, api);
        store8(rec + 0x18, flags | 4, api);
        commit(context, rec, api);
    } else if (cls == 2) {
        const uint32_t rec = record(context, 0x2c, id, api);
        store8(rec + 4, t, api);
        store8(rec + 5, 100, api);
        commit(context, rec, api);
    }
}
