#include "battle_moves_19.h"

enum { ATTACKER = 0x03, TARGET = 0x06, DONE = 0x53, FLAG24 = 0x1000000u };

static void *user(const struct px_battle_moves_19_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_19_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_19_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_moves_19_api *api)
{
    return api->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t rec, const struct px_battle_moves_19_api *api)
{
    api->commit_actions(user(api), context, rec);
}

static uint32_t load8(uint32_t at, const struct px_battle_moves_19_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load8(m->user, at);
}

static uint32_t load16(uint32_t at, const struct px_battle_moves_19_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load16(m->user, at);
}

static uint32_t load32(uint32_t at, const struct px_battle_moves_19_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load32(m->user, at);
}

static void store8(uint32_t at, uint32_t value, const struct px_battle_moves_19_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store8(m->user, at, (uint8_t)value);
}

static void store16(uint32_t at, uint32_t value, const struct px_battle_moves_19_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store16(m->user, at, (uint16_t)value);
}

static void store32(uint32_t at, uint32_t value, const struct px_battle_moves_19_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store32(m->user, at, value);
}

static uint32_t battler(uint32_t context, uint32_t id, const struct px_battle_moves_19_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static uint32_t stage(uint32_t pokemon, uint32_t index, const struct px_battle_moves_19_api *api)
{
    return api->base->pokemon_value(user(api), pokemon, index);
}

/* A message (kind 2) at `at` naming up to three values. */
static void say(uint32_t at, uint32_t text, const struct px_battle_moves_19_api *api)
{
    api->message(user(api), at, 2, text);
}

/* Protect and Detect: the protect streak grows (record 0x27, counter 3, minus bit 24 of the record's first
 * word) after record 0x15 (kind 7); message 0x205 (literal 1084be38). The thunk at 1084bd3c shifts the
 * arguments and enters the routine. */
PX_BATTLE_MOVES_19_HANDLER(px_battle_protect_streak_up)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    (void)battler(context, id, api);
    uint32_t rec = record(context, 0x15, id, api);
    store8(rec + 5, id, api);
    store8(rec + 4, 7, api);
    commit(context, rec, api);
    const uint32_t pokemon = battler(context, id, api);
    rec = record(context, 0x27, id, api);
    const uint32_t streak = api->counter(user(api), pokemon, 3);
    const uint32_t first = load32(rec, api);
    store8(rec + 4, id, api);
    store8(rec + 5, 3, api);
    store8(rec + 6, streak + 1, api);
    store32(rec, first & ~UINT32_C(0x1000000), api);
    commit(context, rec, api);
    rec = record(context, 3, id, api);
    say(rec + 4, 0x205, api);
    api->message_arg(user(api), rec + 4, id);
    commit(context, rec, api);
}

/* Curse: a Ghost-type user with HP left and a target (key 0x06, not 0x1f) cuts half its HP (records 0xb and 7,
 * message 0x42e, literal 108adf10); a Ghost-type always raises the effect flag. Other users lower Speed by
 * one and raise Attack and Defense by one (three records 0xd). */
PX_BATTLE_MOVES_19_HANDLER(px_battle_curse_effect)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t pokemon = battler(context, id, api);
    uint32_t flag = 0;
    if (api->has_type(user(api), pokemon, 7) != 0) {
        if ((int32_t)api->base->pokemon_value(user(api), pokemon, 0xd) > 0) {
            const uint32_t target = get(TARGET, api) & 0xff;
            if (target != 0x1f) {
                uint32_t rec = record(context, 0xb, id, api);
                store8(rec + 0xf, target, api);
                store16(rec + 4, 0xa, api);
                store32(rec + 8, 1, api);
                say(rec + 0x14, 0x42e, api);
                api->message_arg(user(api), rec + 0x14, id);
                api->message_arg(user(api), rec + 0x14, target);
                commit(context, rec, api);
                rec = record(context, 7, id, api);
                store8(rec + 7, id, api);
                const uint32_t half = api->hp_fraction(user(api), pokemon, 2);
                const uint32_t first = load32(rec, api);
                store32(rec + 0x14, 0 - half, api);
                store8(rec + 4, 1, api);
                store32(rec, first | FLAG24, api);
                commit(context, rec, api);
            }
        }
        flag = 1;
    } else {
        static const uint8_t stat[3] = { 5, 1, 2 };
        static const uint8_t amount[3] = { 0xff, 1, 1 };
        for (unsigned i = 0; i < 3; ++i) {
            const uint32_t rec = record(context, 0xd, id, api);
            const uint32_t flags = load8(rec + 0x18, api);
            store8(rec + 4, stat[i], api);
            store8(rec + 0x16, 1, api);
            store8(rec + 0x17, amount[i], api);
            store8(rec + 0x18, flags | 4, api);
            store8(rec + 0xc, id, api);
            commit(context, rec, api);
        }
    }
    api->set_effect_flag(user(api), context, flag);
}

/* Trick and Switcheroo: both battlers' items (+0x12) swap unless the exchange is forbidden: not when 108544ec
 * is clear and the user's owner is 1, when neither holds an item, when either item is locked (1084becc) or
 * either form cannot hold the other item (1085441c, four tests); record 0x25 and its messages (0x2aa, 0x2ad). */
PX_BATTLE_MOVES_19_HANDLER(px_battle_trick_swap)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t target = get(TARGET, api) & 0xff;
    if (api->trade_forbidden(user(api), context) == 0 && api->owner_of(user(api), id) == 1) return;
    const uint32_t me = battler(context, id, api);
    const uint32_t other = battler(context, target, api);
    const uint32_t mine = load16(me + 0x12, api);
    const uint32_t theirs = load16(other + 0x12, api);
    if ((mine | theirs) == 0) return;
    const uint32_t my_form = load16(me + 0xc, api);
    const uint32_t their_form = load16(other + 0xc, api);
    if (api->item_locked(user(api), mine) != 0) return;
    if (api->item_locked(user(api), theirs) != 0) return;
    if (api->item_blocked(user(api), context, my_form, theirs) != 0) return;
    if (api->item_blocked(user(api), context, their_form, mine) != 0) return;
    if (api->item_blocked(user(api), context, my_form, mine) != 0) return;
    if (api->item_blocked(user(api), context, their_form, theirs) != 0) return;
    const uint32_t rec = record(context, 0x25, id, api);
    store8(rec + 4, target, api);
    say(rec + 8, 0x2aa, api);
    api->message_arg(user(api), rec + 8, id);
    if (theirs != 0) {
        say(rec + 0x50, 0x2ad, api);
        api->message_arg(user(api), rec + 0x50, id);
        api->message_arg(user(api), rec + 0x50, theirs);
    }
    if (mine != 0) {
        say(rec + 0x2c, 0x2ad, api);
        api->message_arg(user(api), rec + 0x2c, target);
        api->message_arg(user(api), rec + 0x2c, mine);
    }
    commit(context, rec, api);
}

/* Spite: the target's last move (1084c278) loses up to 4 PP (record 9): none when it is in no slot or has no PP;
 * message 0x281 (literal 108ae280) naming the target, the move and the PP. */
PX_BATTLE_MOVES_19_HANDLER(px_battle_spite_drain)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t target = get(TARGET, api) & 0xff;
    const uint32_t pokemon = battler(context, target, api);
    const uint32_t move = api->last_used(user(api), pokemon);
    const uint32_t slot = api->move_slot(user(api), pokemon, move);
    if (slot == 4) return;
    uint32_t pp = api->move_pp(user(api), pokemon, slot) & 0xff;
    if (pp > 4) pp = 4;
    if (pp == 0) return;
    const uint32_t rec = record(context, 9, id, api);
    store8(rec + 6, slot, api);
    store8(rec + 5, target, api);
    store8(rec + 4, pp, api);
    say(rec + 8, 0x281, api);
    api->message_arg(user(api), rec + 8, target);
    api->message_arg(user(api), rec + 8, move);
    api->message_arg(user(api), rec + 8, pp);
    commit(context, rec, api);
}

/* Rest: unless at full HP, sleep (condition 2, 1084be3c(3) turns, flags at +0xc, +0xe, +0x10; message 0x27e, literal
 * 108aeca8), then the missing HP is healed (record 4, bit 24 set), record 0x22 (kind 4, bit 24) and key 0x53 becomes 0. */
PX_BATTLE_MOVES_19_HANDLER(px_battle_rest_heal)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t pokemon = battler(context, id, api);
    if (api->full_hp(user(api), pokemon) != 0) return;
    uint32_t rec = record(context, 0xb, id, api);
    store8(rec + 0xf, id, api);
    store16(rec + 4, 2, api);
    store32(rec + 8, api->turn_word(user(api), 3), api);
    store8(rec + 0xc, 1, api);
    store8(rec + 0x10, 1, api);
    store8(rec + 0xe, 1, api);
    say(rec + 0x14, 0x27e, api);
    api->message_arg(user(api), rec + 0x14, id);
    commit(context, rec, api);
    rec = record(context, 4, id, api);
    store8(rec + 6, id, api);
    const uint32_t max = stage(pokemon, 0xe, api);
    const uint32_t hp = stage(pokemon, 0xd, api);
    const uint32_t first = load32(rec, api);
    store16(rec + 4, max - hp, api);
    store32(rec, first | FLAG24, api);
    commit(context, rec, api);
    rec = record(context, 0x22, id, api);
    const uint32_t word = load32(rec, api);
    store8(rec + 4, id, api);
    store8(rec + 5, 4, api);
    store32(rec, word | FLAG24, api);
    commit(context, rec, api);
    px_battle_event_rewrite(DONE, 0, api->base->events);
}

/* Mimic: the target's last move (1084c278, after reading 1084c7fc) is copied into the user's Mimic slot, unless it
 * is none, Mimic (0xa6), Struggle (0xa5), 0x1c0, banned (108c8548), the user is blocked (10851c28) or has no
 * slot for the handler's move (+0x38); record 0x26 and message 0x2b0. */
PX_BATTLE_MOVES_19_HANDLER(px_battle_mimic_copy)
{
    (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t me = battler(context, id, api);
    const uint32_t target = get(TARGET, api) & 0xff;
    const uint32_t pokemon = battler(context, target, api);
    uint32_t move = api->last_move(user(api), pokemon);
    const uint32_t used = api->last_used(user(api), pokemon);
    if (used != move) move = used;
    if (move == 0 || move == 0xa6) return;
    if (move == 0xa5 || move == 0x1c0) return;
    if (api->mimic_banned(user(api), me, move) != 0) return;
    if (api->mimic_blocked(user(api), me) != 0) return;
    const uint32_t slot = api->move_slot(user(api), me, load16(handler + 0x38, api));
    if (slot == 4) return;
    uint32_t rec = record(context, 0x26, id, api);
    store8(rec + 6, 0, api);
    store8(rec + 5, slot, api);
    store8(rec + 7, 1, api);
    store8(rec + 4, id, api);
    store16(rec + 8, move, api);
    commit(context, rec, api);
    rec = record(context, 3, id, api);
    say(rec + 4, 0x2b0, api);
    api->message_arg(user(api), rec + 4, id);
    api->message_arg(user(api), rec + 4, move);
    commit(context, rec, api);
}

/* Bestow: the user's item (+0x12) goes to a target without one unless it is locked, or either form cannot hold
 * it (1085441c, three tests): records 0x21 (the user loses it) and 0x21 (the target gets it, message 0x45d,
 * literal 108b03d0). */
PX_BATTLE_MOVES_19_HANDLER(px_battle_bestow_give)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t target = get(TARGET, api) & 0xff;
    const uint32_t other = battler(context, target, api);
    const uint32_t item = load16(battler(context, id, api) + 0x12, api);
    if (item == 0) return;
    if (api->item_locked(user(api), item) != 0) return;
    if (load16(other + 0x12, api) != 0) return;
    const uint32_t me = battler(context, id, api);
    if (api->item_blocked(user(api), context, load16(me + 0xc, api), load16(me + 0x12, api)) != 0) return;
    const uint32_t again = battler(context, target, api);
    if (api->item_blocked(user(api), context, load16(again + 0xc, api), load16(again + 0x12, api)) != 0) return;
    if (api->item_blocked(user(api), context, load16(other + 0xc, api), item) != 0) return;
    uint32_t rec = record(context, 0x21, id, api);
    store8(rec + 6, id, api);
    store16(rec + 4, 0, api);
    commit(context, rec, api);
    rec = record(context, 0x21, id, api);
    store8(rec + 6, target, api);
    store16(rec + 4, item, api);
    say(rec + 0xc, 0x45d, api);
    api->message_arg(user(api), rec + 0xc, target);
    api->message_arg(user(api), rec + 0xc, id);
    api->message_arg(user(api), rec + 0xc, item);
    commit(context, rec, api);
}

/* Psych Up: the target's seven stat stages and its flag byte (+0x155) are copied to the user (record 0xe); then
 * record 0x17 or 0x18 (kind 9) by 1084ca4c, and message 0x41d (literal 108b05f0). */
PX_BATTLE_MOVES_19_HANDLER(px_battle_psych_up_copy)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t target = get(TARGET, api) & 0xff;
    const uint32_t pokemon = battler(context, target, api);
    uint32_t rec = record(context, 0xe, id, api);
    store8(rec + 4, id, api);
    for (uint32_t i = 1; i <= 7; ++i) store8(rec + 4 + i, stage(pokemon, i, api), api);
    store8(rec + 0xc, load8(pokemon + 0x155, api), api);
    commit(context, rec, api);
    const uint32_t kind = api->stage_copy_flag(user(api), pokemon, 9) != 0 ? 0x17 : 0x18;
    rec = record(context, kind, id, api);
    store8(rec + 5, id, api);
    store8(rec + 4, 9, api);
    commit(context, rec, api);
    rec = record(context, 3, id, api);
    say(rec + 4, 0x41d, api);
    api->message_arg(user(api), rec + 4, id);
    api->message_arg(user(api), rec + 4, target);
    commit(context, rec, api);
}

/* Guard Swap: the user's and the target's Defense and Sp. Def stages (values 2 and 4) trade (two records 0xe, the
 * other stages kept), message 0x2a7 (literal 108b1a68). */
PX_BATTLE_MOVES_19_HANDLER(px_battle_guard_swap)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t target = get(TARGET, api) & 0xff;
    const uint32_t other = battler(context, target, api);
    const uint32_t me = battler(context, id, api);
    const uint32_t my_defense = stage(me, 2, api);
    const uint32_t my_special = stage(me, 4, api);
    uint32_t rec = record(context, 0xe, id, api);
    store8(rec + 4, id, api);
    store8(rec + 5, stage(me, 1, api), api);
    store8(rec + 7, stage(me, 3, api), api);
    store8(rec + 6, stage(other, 2, api), api);
    store8(rec + 8, stage(other, 4, api), api);
    store8(rec + 9, stage(me, 5, api), api);
    store8(rec + 0xa, stage(me, 6, api), api);
    store8(rec + 0xb, stage(me, 7, api), api);
    store8(rec + 0xc, load8(me + 0x155, api), api);
    commit(context, rec, api);
    rec = record(context, 0xe, id, api);
    store8(rec + 4, target, api);
    store8(rec + 5, stage(other, 1, api), api);
    store8(rec + 7, stage(other, 3, api), api);
    store8(rec + 6, my_defense, api);
    store8(rec + 8, my_special, api);
    store8(rec + 9, stage(other, 5, api), api);
    store8(rec + 0xa, stage(other, 6, api), api);
    store8(rec + 0xb, stage(other, 7, api), api);
    store8(rec + 0xc, load8(other + 0x155, api), api);
    commit(context, rec, api);
    rec = record(context, 3, id, api);
    say(rec + 4, 0x2a7, api);
    api->message_arg(user(api), rec + 4, id);
    commit(context, rec, api);
}

/* Heart Swap: all seven stat stages and the flag byte (+0x155) trade (two records 0xe, the user's saved in a
 * 28-byte buffer first), message 0x2a1 (literal 108b1e48). */
PX_BATTLE_MOVES_19_HANDLER(px_battle_heart_swap)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t target = get(TARGET, api) & 0xff;
    const uint32_t other = battler(context, target, api);
    const uint32_t me = battler(context, id, api);
    const uint32_t buffer = api->temp_buffer(user(api), context, 0x1c);
    const uint32_t flag = load8(me + 0x155, api);
    for (uint32_t i = 1; i <= 7; ++i) store32(buffer + 4 * (i - 1), stage(me, i, api), api);
    uint32_t rec = record(context, 0xe, id, api);
    store8(rec + 4, id, api);
    for (uint32_t i = 1; i <= 7; ++i) store8(rec + 4 + i, stage(other, i, api), api);
    store8(rec + 0xc, flag, api);
    commit(context, rec, api);
    rec = record(context, 0xe, id, api);
    store8(rec + 4, target, api);
    for (uint32_t i = 0; i < 7; ++i) store8(rec + 5 + i, load32(buffer + 4 * i, api), api);
    store8(rec + 0xc, load8(other + 0x155, api), api);
    commit(context, rec, api);
    rec = record(context, 3, id, api);
    say(rec + 4, 0x2a1, api);
    api->message_arg(user(api), rec + 4, id);
    commit(context, rec, api);
}

/* Camouflage: the user's type word (10870994) becomes the one of the terrain (jump table 108b1e80: 0 none,
 * 1 and 5 type 4, 2 type 5, 3 type 0xb, 4 type 0xa, 6 and 7 type 0xe) refined by the environment (jump table
 * 108b1ed8: 1 type 0xb, 2 type 0x11, 3 type 0xc, 4 type 0xa, 5 type 0xe; others keep it), as a triple of the type
 * in three five-bit fields below the bits 0x4800; unless it already holds that word (record 0x13). */
PX_BATTLE_MOVES_19_HANDLER(px_battle_camouflage_type)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    static const uint8_t by_terrain[8] = { 0, 4, 5, 0xb, 0xa, 4, 0xe, 0xe };
    static const uint8_t by_environment[6] = { 0, 0xb, 0x11, 0xc, 0xa, 0xe };
    const uint32_t terrain = api->terrain(user(api), context);
    uint32_t type = terrain < 8 ? by_terrain[terrain] : 0;
    const uint32_t environment = api->environment(user(api), context);
    if (environment < 6 && environment != 0) type = by_environment[environment];
    uint32_t word = type & 0x1f;
    word |= word << 5;
    word |= 0x4800;
    if (api->type_word(user(api), battler(context, id, api)) == word) return;
    const uint32_t rec = record(context, 0x13, id, api);
    store16(rec + 4, word, api);
    store8(rec + 6, id, api);
    commit(context, rec, api);
}
