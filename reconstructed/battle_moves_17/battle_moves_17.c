#include "battle_moves_17.h"

enum { ATTACKER = 0x03, DEFENDER = 0x04, TARGET = 0x06, MOVE = 0x12, USED = 0x14, KIND = 0x1e, VALUE = 0x1f, FLAG = 0x47, DONE = 0x53,
       AREA = 0x40, EXTRA = 0x27 };

static void *user(const struct px_battle_moves_17_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_17_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_17_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t set(uint32_t key, uint32_t value, const struct px_battle_moves_17_api *api)
{
    return px_battle_event_rewrite(key, value, api->base->events);
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_moves_17_api *api)
{
    return api->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t rec, const struct px_battle_moves_17_api *api)
{
    api->commit_actions(user(api), context, rec);
}

static uint32_t load8(uint32_t at, const struct px_battle_moves_17_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load8(m->user, at);
}

static uint32_t load16(uint32_t at, const struct px_battle_moves_17_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load16(m->user, at);
}

static uint32_t load32(uint32_t at, const struct px_battle_moves_17_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load32(m->user, at);
}

static void store8(uint32_t at, uint32_t value, const struct px_battle_moves_17_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store8(m->user, at, (uint8_t)value);
}

static void store16(uint32_t at, uint32_t value, const struct px_battle_moves_17_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store16(m->user, at, (uint16_t)value);
}

static void store32(uint32_t at, uint32_t value, const struct px_battle_moves_17_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store32(m->user, at, value);
}

static uint32_t battler(uint32_t context, uint32_t id, const struct px_battle_moves_17_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

/* A message (kind 2) at `at` naming up to two values. */
static void say(uint32_t at, uint32_t text, const struct px_battle_moves_17_api *api)
{
    api->message(user(api), at, 2, text);
}

/* Incinerate: unless key 0x47 is set, a defender's held item (+0x12) that can burn (108007a4) or flames
 * (10801cfc) is destroyed: record 0x21 (halfword 0), message 0x45a (literal 108b4ae8) naming the defender and the item. */
PX_BATTLE_MOVES_17_HANDLER(px_battle_incinerate_item)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id || get(FLAG, api) != 0) return;
    const uint32_t target = get(DEFENDER, api) & 0xff;
    const uint32_t item = load16(battler(context, target, api) + 0x12, api);
    if (api->item_burnable(user(api), item) == 0 && api->item_flammable(user(api), item) == 0) return;
    const uint32_t rec = record(context, 0x21, id, api);
    store8(rec + 6, target, api);
    store16(rec + 4, 0, api);
    say(rec + 0xc, 0x45a, api);
    api->message_arg(user(api), rec + 0xc, target);
    api->message_arg(user(api), rec + 0xc, item);
    commit(context, rec, api);
}

/* Destiny Bond: once its user has fainted (key 0x04) from a move that can be disabled by a non-ally attacker
 * with HP left, the attacker faints too (record 0x12: the move used in halfword +6) and the user's own record
 * (message 0x275, literal 108bacf8) follows; the handler then removes itself (also when the checks fail
 * after the user fainted). */
PX_BATTLE_MOVES_17_HANDLER(px_battle_destiny_bond_faint)
{
    (void)work;
    if (get(DEFENDER, api) != id) return;
    if (api->no_hp(user(api), battler(context, id, api)) == 0) return;
    if (api->disable_blocked(user(api), get(MOVE, api) & 0xffff) == 0) {
        const uint32_t attacker = get(ATTACKER, api) & 0xff;
        const uint32_t pokemon = battler(context, attacker, api);
        if (api->is_ally(user(api), id, attacker) == 0 && api->no_hp(user(api), pokemon) == 0) {
            const uint32_t move = get(USED, api);
            api->note_handler(user(api), handler);
            uint32_t rec = record(context, 0x12, attacker, api);
            store16(rec + 6, move, api);
            store8(rec + 4, id, api);
            store8(rec + 5, 1, api);
            commit(context, rec, api);
            rec = record(context, 0x12, id, api);
            store8(rec + 4, attacker, api);
            store16(rec + 6, load16(handler + 0x38, api), api);
            say(rec + 8, 0x275, api);
            api->message_arg(user(api), rec + 8, id);
            commit(context, rec, api);
        }
    }
    px_battle_handler_remove(handler, api->handlers);
}

/* Side-condition mask bit i (size byte at +4): byte 1 + i / 8, bit i % 8, while that byte is within the size. */
static void mask_add(uint32_t rec, uint32_t i, const struct px_battle_moves_17_api *api)
{
    const uint32_t at = 1 + (i >> 3);
    if (load8(rec + 4, api) <= at) return;
    store8(rec + 4 + at, load8(rec + 4 + at, api) | UINT32_C(1) << (i & 7), api);
}

/* Rapid Spin: the user is freed of status 0x12 and status 8 (record 0xa each) and its side's conditions 6, 7, 8 and
 * 14 are lifted (record 0x1a). */
PX_BATTLE_MOVES_17_HANDLER(px_battle_rapid_spin_clear)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t pokemon = battler(context, id, api);
    static const uint8_t statuses[2] = { 0x12, 0x08 };
    for (unsigned i = 0; i < 2; ++i) {
        if (api->has_status(user(api), pokemon, statuses[i]) == 0) continue;
        const uint32_t rec = record(context, 0xa, id, api);
        store8(rec + 0x11, 1, api);
        store8(rec + 5, id, api);
        store8(rec + 4, statuses[i], api);
        commit(context, rec, api);
    }
    const uint32_t rec = record(context, 0x1a, id, api);
    store8(rec + 8, api->side_id(user(api), id), api);
    store8(rec + 4, 4, api);
    store8(rec + 5, 0, api);
    store8(rec + 6, 0, api);
    store8(rec + 7, 0, api);
    mask_add(rec, 6, api);
    mask_add(rec, 7, api);
    mask_add(rec, 8, api);
    mask_add(rec, 14, api);
    commit(context, rec, api);
}

/* Struggle: key 0x53 set. */
PX_BATTLE_MOVES_17_HANDLER(px_battle_struggle_ready)
{
    (void)handler, (void)context, (void)work;
    if (get(ATTACKER, api) != id) return;
    set(DONE, 1, api);
}

/* Pursuit: when a pursuit is possible (1084b784 fills a word, unused here), and the target (key 0x06) is not an ally,
 * the strike is carried out (1081d748) unless the redirect is blocked. The two side lookups are made and their
 * results discarded. */
PX_BATTLE_MOVES_17_HANDLER(px_battle_pursuit_strike)
{
    (void)handler, (void)work;
    if (api->can_pursue(user(api), context, id, api->scratch) == 0) return;
    const uint32_t target = get(TARGET, api) & 0xff;
    if (api->is_ally(user(api), target, id) != 0) return;
    (void)api->base->side_of(user(api), context, target);
    (void)api->base->side_of(user(api), context, id);
    if (api->redirect_blocked(user(api), context, id, target, 0) != 0) return;
    api->pursue(user(api), context, id);
}

/* Move 475: a streak above 1 (108c8754) restores its excess over 1000 (at least 1; literal 108b13fc = 1001),
 * record 0x2e, message 0x454 (literal 108b1400). */
PX_BATTLE_MOVES_17_HANDLER(px_battle_streak_heal)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t streak = api->streak(user(api), battler(context, id, api));
    if (streak <= 1) return;
    const uint32_t rec = record(context, 0x2e, id, api);
    store8(rec + 4, id, api);
    store16(rec + 6, streak <= 1001 ? 1 : streak - 1000, api);
    say(rec + 8, 0x454, api);
    api->message_arg(user(api), rec + 8, id);
    commit(context, rec, api);
}

/* Move 477: key 0x1e becomes 0x20; message 0x47a (literal 108b55d0) in the message area (key 0x40), naming key 0x04. */
PX_BATTLE_MOVES_17_HANDLER(px_battle_effect_message)
{
    (void)handler, (void)context, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t area = get(AREA, api);
    set(KIND, 0x20, api);
    say(area, 0x47a, api);
    api->message_arg(user(api), area, get(DEFENDER, api));
}

/* Eight moves' effect texts (key 0x1e = 8): the move's message in the area (key 0x40), naming key 0x04 and the user. */
PX_BATTLE_MOVES_17_HANDLER(px_battle_effect_text)
{
    (void)work, (void)context;
    if (get(ATTACKER, api) != id) return;
    if (get(KIND, api) != 8) return;
    const uint32_t area = get(AREA, api);
    const uint32_t target = get(DEFENDER, api) & 0xff;
    uint32_t text;
    switch (load16(handler + 0x38, api)) {
    case 0xfa: text = 0x33b; break;   /* literals 108b608c-108b60a0 */
    case 0x14: text = 0x326; break;
    case 0x23: text = 0x32d; break;
    case 0x53: text = 0x33e; break;
    case 0x80: text = 0x334; break;
    case 0x148: text = 0x344; break;
    case 0x1cf: text = 0x341; break;
    case 0x263: text = 0x4d2; break;
    default: return;
    }
    say(area, text, api);
    api->message_arg(user(api), area, target);
    api->message_arg(user(api), area, id);
}

/* Moves 193, 316 and 357: the effect value (key 0x1f, a word) is scaled with parameter 7 or 0x10. */
static void scale(uint32_t id, uint32_t parameter, const struct px_battle_moves_17_api *api)
{
    if (get(ATTACKER, api) != id) return;
    store32(api->scratch, get(VALUE, api), api);
    api->scale_value(user(api), api->scratch, parameter);
    set(VALUE, load32(api->scratch, api), api);
}

PX_BATTLE_MOVES_17_HANDLER(px_battle_scale_193) { (void)handler, (void)context, (void)work; scale(id, 7, api); }
PX_BATTLE_MOVES_17_HANDLER(px_battle_scale_357) { (void)handler, (void)context, (void)work; scale(id, 0x10, api); }

/* The eight moves of event 0x6d scale it by their own move id (the handler's +0x38). */
PX_BATTLE_MOVES_17_HANDLER(px_battle_scale_by_move)
{
    (void)context, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t move = load16(handler + 0x38, api);
    scale(id, move, api);
}

/* Move 73: the effect value becomes the one of the user's opposing selector (1084c3b0). */
PX_BATTLE_MOVES_17_HANDLER(px_battle_selector_value)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    set(VALUE, api->selector_value(user(api), api->opposing_selector(user(api), context, id)), api);
}

/* Tri Attack: one of three effect kinds (table 108d2c34) is picked at random (range 3, masked to a byte): its
 * kind goes to key 0x1e, its value (10860e00) to key 0x1f and key 0x27 is 0x14. */
PX_BATTLE_MOVES_17_HANDLER(px_battle_tri_attack_effect)
{
    (void)handler, (void)context, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t kind = load8(api->tri_attack_table + (api->random(user(api), 3) & 0xff), api);
    const uint32_t value = api->type_value(user(api), kind);
    set(KIND, kind, api);
    set(VALUE, value, api);
    set(EXTRA, 0x14, api);
}
