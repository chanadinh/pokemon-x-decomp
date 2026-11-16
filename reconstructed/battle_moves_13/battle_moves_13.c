#include "battle_moves_13.h"

enum { ATTACKER = 0x03, DEFENDER = 0x04, TARGET = 0x06, BYPASS = 0x53, STARTED = 0x18 };

static void *user(const struct px_battle_moves_13_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_13_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_13_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_moves_13_api *api)
{
    return api->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t rec, const struct px_battle_moves_13_api *api)
{
    api->commit_actions(user(api), context, rec);
}

static uint32_t load8(uint32_t at, const struct px_battle_moves_13_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load8(m->user, at);
}

static void store8(uint32_t at, uint32_t value, const struct px_battle_moves_13_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store8(m->user, at, (uint8_t)value);
}

static uint32_t load32(uint32_t at, const struct px_battle_moves_13_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load32(m->user, at);
}

static uint32_t battler(uint32_t context, uint32_t id, const struct px_battle_moves_13_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

/* Feint's family: when the target's side has a protection condition up (or
 * the target protected itself this turn, turn flag 7), the protection ends
 * with the move's message. The search skips condition 0x10 only when its
 * flag (r6) is clear; every handler sets it to 1 first. */
static void break_protection(uint32_t key, uint32_t context, uint32_t id, uint32_t message,
                             const struct px_battle_moves_13_api *api)
{
    if (get(ATTACKER, api) != id) return;
    const uint32_t target = get(key, api) & 0xff;
    const uint32_t pokemon = battler(context, target, api);
    const uint32_t every = 1;
    const uint32_t side = api->side_of(user(api), target);
    uint32_t found = 0;
    for (uint32_t i = 0; i < 0x11; ++i) {
        if (api->side_condition(user(api), context, side, i) == 0) continue;
        if (api->protection_condition(user(api), i) == 0) continue;
        if (every == 0 && i == 0x10) continue;
        found = 1;
        break;
    }
    if ((api->turn_flag(user(api), pokemon, 7) | found) == 0) return;
    api->break_protection(user(api), context, id, pokemon, message);
}

/* Literal 108b8b54. */
PX_BATTLE_MOVES_13_HANDLER(px_battle_feint_break) { (void)handler, (void)work; break_protection(DEFENDER, context, id, 0x20e, api); }
PX_BATTLE_MOVES_13_HANDLER(px_battle_hyperspace_fury_break) { (void)handler, (void)work; break_protection(DEFENDER, context, id, 0x208, api); }
PX_BATTLE_MOVES_13_HANDLER(px_battle_shadow_force_break) { (void)handler, (void)work; break_protection(TARGET, context, id, 0x208, api); }

/* The move goes through protection: key 0x53 is set. */
PX_BATTLE_MOVES_13_HANDLER(px_battle_feint_bypass)
{
    (void)handler, (void)context, (void)work;
    if (get(ATTACKER, api) != id) return;
    px_battle_event_rewrite(BYPASS, 1, api->base->events);
}

/* Side-condition mask: size byte 4, then three clear mask bytes. */
static void mask_clear(uint32_t rec, const struct px_battle_moves_13_api *api)
{
    store8(rec + 4, 4, api);
    store8(rec + 5, 0, api);
    store8(rec + 6, 0, api);
    store8(rec + 7, 0, api);
}

/* Bit i % 8 of mask byte 1 + i / 8, while that byte is within the size. */
static void mask_add(uint32_t rec, uint32_t i, const struct px_battle_moves_13_api *api)
{
    const uint32_t at = (1 + (i >> 3)) & 0xff;
    if (load8(rec + 4, api) <= at) return;
    store8(rec + 4 + at, load8(rec + 4 + at, api) | UINT32_C(1) << (i & 7), api);
}

/* No key test: every protection condition of the opposing side is lifted
 * (record 0x1b; the early handler leaves byte +9 as it is). */
static void unprotect(uint32_t context, uint32_t id, int flag, const struct px_battle_moves_13_api *api)
{
    const uint32_t side = api->opposing_side(user(api), api->side_of(user(api), id));
    const uint32_t rec = record(context, 0x1b, id, api);
    store8(rec + 8, side, api);
    if (flag) store8(rec + 9, 1, api);
    mask_clear(rec, api);
    for (uint32_t i = 0; i < 0x11; ++i) {
        if (api->protection_condition(user(api), i & 0xff) != 0) mask_add(rec, i, api);
    }
    commit(context, rec, api);
}

PX_BATTLE_MOVES_13_HANDLER(px_battle_feint_unprotect) { (void)handler, (void)work; unprotect(context, id, 1, api); }
PX_BATTLE_MOVES_13_HANDLER(px_battle_feint_unprotect_early) { (void)handler, (void)work; unprotect(context, id, 0, api); }

/* Brick Break: with Reflect or Light Screen up on the defender's side, both
 * are removed (record 0x1a) and record 0x3b follows. */
PX_BATTLE_MOVES_13_HANDLER(px_battle_brick_break)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t side = api->side_of(user(api), get(DEFENDER, api) & 0xff);
    const uint32_t at = api->side_conditions + side * 0x110;
    if (load32(at, api) == 0 && load32(at + 0x10, api) == 0) return;
    const uint32_t rec = record(context, 0x1a, id, api);
    store8(rec + 8, side, api);
    mask_clear(rec, api);
    mask_add(rec, 0, api);
    mask_add(rec, 1, api);
    commit(context, rec, api);
    const uint32_t done = record(context, 0x3b, id, api);
    store8(done + 4, 1, api);
    commit(context, done, api);
}

/* Uproar's first turn (unless locked, status 0x19, or started already):
 * the lock (condition 0x19, count 1084beac(3, move)), message 0x2bf
 * (literal 108aeff0), and every sleeping opponent wakes (status 2, message
 * 0x2c2, literal 108aeff4). The buffer's first byte holds the count; the
 * first test uses the returned count, later ones the stored byte. */
PX_BATTLE_MOVES_13_HANDLER(px_battle_uproar_start)
{
    const struct px_pokemon_api *m = memory(api);
    if (get(ATTACKER, api) != id) return;
    if (api->has_status(user(api), battler(context, id, api), 0x19) != 0 || load32(work + STARTED, api) != 0) return;
    uint32_t rec = record(context, 0xb, id, api);
    m->store16(m->user, rec + 4, 0x19);
    const uint32_t count = api->lock_count(user(api), 3, m->load16(m->user, handler + 0x38));
    m->store32(m->user, rec + 8, count);
    store8(rec + 0xc, 0, api);
    store8(rec + 0xf, id, api);
    commit(context, rec, api);
    rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 2, 0x2bf);
    api->message_arg(user(api), rec + 4, id);
    commit(context, rec, api);
    const uint32_t selector = api->opposing_selector(user(api), context, id) | 0x800;
    const uint32_t buffer = api->temp_buffer(user(api), context, 0xb);
    const uint32_t found = api->base->side_battlers(user(api), context, selector, buffer + 1);
    store8(buffer, found, api);
    if (found != 0) {
        uint32_t i = 0;
        do {
            if (api->has_status(user(api), battler(context, load8(buffer + 1 + i, api), api), 2) != 0) {
                rec = record(context, 0xa, id, api);
                store8(rec + 4, 2, api);
                store8(rec + 0x11, 1, api);
                store8(rec + 5, load8(buffer + 1 + i, api), api);
                api->message(user(api), rec + 0x14, 2, 0x2c2);
                api->message_arg(user(api), rec + 0x14, load8(buffer + 1 + i, api));
                commit(context, rec, api);
            }
            ++i;
        } while (load8(buffer, api) > i);
    }
    m->store32(m->user, work + STARTED, 1);
}

/* Pay Day: coins of five times the user's level (value 0xf), message 0x80. */
PX_BATTLE_MOVES_13_HANDLER(px_battle_pay_day)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t level = api->base->pokemon_value(user(api), battler(context, id, api), 0xf);
    if (api->scatter_money(user(api), context, level * 5, id) == 0) return;
    const uint32_t rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 1, 0x80);
    commit(context, rec, api);
}

/* Self-Destruct and Explosion: the user faints (record 0x12). */
PX_BATTLE_MOVES_13_HANDLER(px_battle_explosion_faint)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t rec = record(context, 0x12, id, api);
    store8(rec + 4, id, api);
    commit(context, rec, api);
}
