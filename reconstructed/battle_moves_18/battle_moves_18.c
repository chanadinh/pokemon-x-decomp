#include "battle_moves_18.h"

enum { BATTLER = 0x02, ATTACKER = 0x03, DEFENDER = 0x04, TARGET = 0x06, POWER = 0x36, BOOST = 0x3b, FAILED = 0x42, DONE = 0x53, ACTIVE = 0x18 };

static void *user(const struct px_battle_moves_18_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_18_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_18_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t set(uint32_t key, uint32_t value, const struct px_battle_moves_18_api *api)
{
    return px_battle_event_rewrite(key, value, api->base->events);
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_moves_18_api *api)
{
    return api->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t rec, const struct px_battle_moves_18_api *api)
{
    api->commit_actions(user(api), context, rec);
}

static uint32_t load8(uint32_t at, const struct px_battle_moves_18_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load8(m->user, at);
}

static uint32_t load16(uint32_t at, const struct px_battle_moves_18_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load16(m->user, at);
}

static uint32_t load32(uint32_t at, const struct px_battle_moves_18_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load32(m->user, at);
}

static void store8(uint32_t at, uint32_t value, const struct px_battle_moves_18_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store8(m->user, at, (uint8_t)value);
}

static void store16(uint32_t at, uint32_t value, const struct px_battle_moves_18_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store16(m->user, at, (uint16_t)value);
}

static void store32(uint32_t at, uint32_t value, const struct px_battle_moves_18_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store32(m->user, at, value);
}

static uint32_t battler(uint32_t context, uint32_t id, const struct px_battle_moves_18_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

/* A message naming a battler (record 3 or the given record), kind 2. */
static void say(uint32_t at, uint32_t text, const struct px_battle_moves_18_api *api)
{
    api->message(user(api), at, 2, text);
}

/* Every handler of owner 0 and value byte `value` whose move (+0x38) is `move` is removed; the search restarts after each removal. */
static void remove_move(uint32_t move, uint32_t value, const struct px_battle_moves_18_api *api)
{
    for (uint32_t h = px_battle_handler_find(0, value, api->handlers); h != 0;) {
        if (load16(h + 0x38, api) == move) {
            px_battle_handler_remove(h, api->handlers);
            h = px_battle_handler_find(0, value, api->handlers);
        } else {
            h = px_battle_handler_find_next(h, api->handlers);
        }
    }
}

/* Moves 206 and 610: key 0x3b becomes 2 once key 0x53 is set. */
PX_BATTLE_MOVES_18_HANDLER(px_battle_support_boost)
{
    (void)handler, (void)context, (void)work;
    if (get(ATTACKER, api) != id) return;
    if (get(DONE, api) == 0) return;
    set(BOOST, 2, api);
}

/* Move 203: the move's flag (work word 6) and key 0x53 set: key 0x3b becomes 1 (key 0x04 names the user). */
PX_BATTLE_MOVES_18_HANDLER(px_battle_flagged_boost)
{
    (void)handler, (void)context;
    if (get(DEFENDER, api) != id) return;
    if (load32(work + ACTIVE, api) == 0) return;
    if (get(DONE, api) == 0) return;
    set(BOOST, 1, api);
}

/* Uproar ends (work word 6 set, the user with HP left): with the lock (status 0x19) the message
 * 0x2cb (literal 108b86dc) is the last word; without it, message 0x2ce (literal 108b86e0) and the move's handlers go. */
PX_BATTLE_MOVES_18_HANDLER(px_battle_uproar_end)
{
    if (get(BATTLER, api) != id) return;
    if (load32(work + ACTIVE, api) == 0) return;
    const uint32_t pokemon = battler(context, id, api);
    if (api->no_hp(user(api), pokemon) != 0) return;
    const uint32_t locked = api->has_status(user(api), pokemon, 0x19);
    const uint32_t rec = record(context, 3, id, api);
    say(rec + 4, locked != 0 ? 0x2cb : 0x2ce, api);
    api->message_arg(user(api), rec + 4, id);
    commit(context, rec, api);
    if (locked != 0) return;
    remove_move(load16(handler + 0x38, api), load8(pokemon + 0x19, api), api);
}

/* Thrash-type moves: a sleeping user wakes (record 0xa, condition 0x19 ends) and the move's handlers go. */
PX_BATTLE_MOVES_18_HANDLER(px_battle_sleep_wake)
{
    (void)work;
    if (get(BATTLER, api) != id) return;
    const uint32_t pokemon = battler(context, id, api);
    if (api->has_status(user(api), pokemon, 2) == 0) return;
    const uint32_t rec = record(context, 0xa, id, api);
    store8(rec + 4, 0x19, api);
    store8(rec + 5, id, api);
    store8(rec + 0x11, 1, api);
    commit(context, rec, api);
    remove_move(load16(handler + 0x38, api), load8(pokemon + 0x19, api), api);
}

/* Work word 0 records whether the user holds an item. */
PX_BATTLE_MOVES_18_HANDLER(px_battle_item_flag)
{
    (void)handler;
    if (get(ATTACKER, api) != id) return;
    store32(work, load16(battler(context, id, api) + 0x12, api) != 0, api);
}

/* Fling: a held item (+0x12) is thrown: message 0x30b (literal 108bbc60) naming the user and the item, record 0x15 (kind 0xf). */
PX_BATTLE_MOVES_18_HANDLER(px_battle_fling_start)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t item = load16(battler(context, id, api) + 0x12, api);
    if (item == 0) return;
    uint32_t rec = record(context, 3, id, api);
    say(rec + 4, 0x30b, api);
    api->message_arg(user(api), rec + 4, id);
    api->message_arg(user(api), rec + 4, item);
    commit(context, rec, api);
    rec = record(context, 0x15, id, api);
    store8(rec + 5, id, api);
    store8(rec + 4, 0xf, api);
    commit(context, rec, api);
}

/* Move 547: a user of the form 0x288 (halfword +0xc), with the flag byte +0x153 clear or set, gets record 0x3a and message 0xde. */
PX_BATTLE_MOVES_18_HANDLER(px_battle_form_check)
{
    (void)handler, (void)work;
    const uint32_t pokemon = battler(context, id, api);
    if (load16(pokemon + 0xc, api) != 0x288) return;
    if (get(ATTACKER, api) != id) return;
    const uint32_t rec = record(context, 0x3a, id, api);
    store8(rec + 5, load8(pokemon + 0x153, api) != 0 ? 0 : 1, api);
    store8(rec + 4, id, api);
    say(rec + 8, 0xde, api);
    api->message_arg(user(api), rec + 8, id);
    commit(context, rec, api);
}

/* The pledges (work word 0 set, word 1 the form 1-3): a condition (record 0x19: kind 0xb, 0xc or 0xd) lasting
 * 1084be3c(4) turns on the user's side (forms 2 and 3: the target's side), a message (0xaa, 0xae or 0xb2 as kind 1,
 * naming the side) and, when the party's position (1084b590) exists, record 0x38 (halfwords 0x27b, 0x27c, 0x27d). */
PX_BATTLE_MOVES_18_HANDLER(px_battle_pledge_effect)
{
    (void)handler;
    if (get(ATTACKER, api) != id) return;
    if (load32(work, api) == 0) return;
    uint32_t side = api->side_id(user(api), id);
    const uint32_t target_side = api->side_id(user(api), get(TARGET, api) & 0xff);
    uint32_t kind, text, halfword, target;
    switch (load32(work + 4, api)) {
    case 1:
        kind = 0xb; target = id; text = 0xaa; halfword = 0x27b;
        break;
    case 2:
        side = target_side; kind = 0xc; text = 0xae; halfword = 0x27c;
        target = get(TARGET, api) & 0xff;
        break;
    case 3:
        side = target_side; kind = 0xd; text = 0xb2; halfword = 0x27d;
        target = get(TARGET, api) & 0xff;
        break;
    default:
        return;
    }
    uint32_t rec = record(context, 0x19, id, api);
    store8(rec + 4, kind, api);
    store8(rec + 0xc, side, api);
    store32(rec + 8, api->turn_word(user(api), 4), api);
    api->message(user(api), rec + 0x10, 1, text);
    api->message_arg(user(api), rec + 0x10, side);
    commit(context, rec, api);
    if (target == 0x1f) return;
    const uint32_t position = api->position(user(api), context, target);
    if (position == 0xa) return;
    rec = record(context, 0x38, id, api);
    store8(rec + 7, 0xa, api);
    store8(rec + 6, position, api);
    store16(rec + 4, halfword, api);
    commit(context, rec, api);
}

/* U-turn, Volt Switch and Parting Shot: when the user can be replaced (1084ca78) and this turn's switch is
 * claimed (1084c000), record 0x2a (message 0x302, literal 108b477c, naming the user and its owner) and record 0x38
 * (halfword 0x2bf for move 369, else 0x2c0; the user's side; flag bit 0x1000000 of its first word). */
PX_BATTLE_MOVES_18_HANDLER(px_battle_switch_out)
{
    (void)work;
    if (get(ATTACKER, api) != id) return;
    if (api->can_switch(user(api), context, id) == 0) return;
    if (api->claim_switch(user(api), context) == 0) return;
    const uint32_t owner = api->owner_of(user(api), id);
    uint32_t rec = record(context, 0x2a, id, api);
    store8(rec + 0x4c, id, api);
    say(rec + 4, 0x302, api);
    api->message_arg(user(api), rec + 4, id);
    api->message_arg(user(api), rec + 4, owner);
    commit(context, rec, api);
    rec = record(context, 0x38, id, api);
    store16(rec + 4, load16(handler + 0x38, api) - 0x100 - 0x71 == 0 ? 0x2bf : 0x2c0, api);
    store8(rec + 6, api->base->side_of(user(api), context, id), api);
    store8(rec + 7, 0xa, api);
    store32(rec, load32(rec, api) | UINT32_C(0x1000000), api);
    commit(context, rec, api);
}

/* Weather-based power: weather 1 sets key 0x36 to 0xaac (literal 108b4c18); 2, 3 and 4 to 0x400; any other to 0x800. */
PX_BATTLE_MOVES_18_HANDLER(px_battle_weather_power)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t weather = api->weather(user(api), context);
    uint32_t value;
    if (weather == 1) value = 0xaac;
    else if (weather == 2 || weather == 3 || weather == 4) value = 0x400;
    else value = 0x800;
    set(POWER, value, api);
}

/* Spit Up: the stockpile (counter 0) of 2 gives 0x800, 3 gives 0x1000, any other 0x400 (key 0x36). */
PX_BATTLE_MOVES_18_HANDLER(px_battle_stockpile_power)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t count = api->counter(user(api), battler(context, id, api), 0);
    set(POWER, count == 2 ? 0x800 : count == 3 ? 0x1000 : 0x400, api);
}

/* Move 507: a target (key 0x04, not 0x1f) who is an ally fails the move (key 0x42). */
PX_BATTLE_MOVES_18_HANDLER(px_battle_ally_target)
{
    (void)handler, (void)context, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t target = get(DEFENDER, api) & 0xff;
    if (target == 0x1f) return;
    if (api->is_ally(user(api), id, target) == 0) return;
    set(FAILED, 1, api);
}

/* Move 76: weather 1 sets key 0x53. */
PX_BATTLE_MOVES_18_HANDLER(px_battle_sun_ready)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    if (api->weather(user(api), context) != 1) return;
    set(DONE, 1, api);
}
