#include "battle_moves_16.h"

enum { ATTACKER = 0x03, DEFENDER = 0x04, MOVE = 0x12, USED = 0x14, FLAG = 0x47, FLAG2 = 0x48, BONUS = 0x33, DONE = 0x53 };

static void *user(const struct px_battle_moves_16_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_16_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_16_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t set(uint32_t key, uint32_t value, const struct px_battle_moves_16_api *api)
{
    return px_battle_event_rewrite(key, value, api->base->events);
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_moves_16_api *api)
{
    return api->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t rec, const struct px_battle_moves_16_api *api)
{
    api->commit_actions(user(api), context, rec);
}

static uint32_t load8(uint32_t at, const struct px_battle_moves_16_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load8(m->user, at);
}

static uint32_t load16(uint32_t at, const struct px_battle_moves_16_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load16(m->user, at);
}

static uint32_t load32(uint32_t at, const struct px_battle_moves_16_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load32(m->user, at);
}

static void store8(uint32_t at, uint32_t value, const struct px_battle_moves_16_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store8(m->user, at, (uint8_t)value);
}

static void store16(uint32_t at, uint32_t value, const struct px_battle_moves_16_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store16(m->user, at, (uint16_t)value);
}

static void store32(uint32_t at, uint32_t value, const struct px_battle_moves_16_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store32(m->user, at, value);
}

static uint32_t battler(uint32_t context, uint32_t id, const struct px_battle_moves_16_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

/* A message naming a battler (kind 2) at `at` in a record. */
static void say(uint32_t at, uint32_t text, uint32_t value, const struct px_battle_moves_16_api *api)
{
    api->message(user(api), at, 2, text);
    api->message_arg(user(api), at, value);
}

/* Counter, Mirror Coat and Metal Burst (one routine, the category passed:
 * 1 physical, 2 special, 0 any): the latest damaging hit on the user from a
 * non-ally (of that category) names the target; an unreachable one is
 * replaced by the move's chosen target, or the hitter itself. */
static void hit_back(uint32_t context, uint32_t id, uint32_t category, const struct px_battle_moves_16_api *api)
{
    if (get(ATTACKER, api) != id) return;
    const uint32_t pokemon = battler(context, id, api);
    const uint32_t self = load8(pokemon + 0x19, api);
    const uint32_t out = api->scratch;
    uint32_t index = 0;
    for (;;) {
        const uint32_t at = index;
        index = (index + 1) & 0xff;
        if (api->damage_record(user(api), pokemon, 0, at, out) == 0) return;
        if (api->is_ally(user(api), self, load8(out + 5, api)) != 0) continue;
        if (category != 0 && api->base->move_category(user(api), load16(out, api)) != category) continue;
        break;
    }
    uint32_t target = load8(out + 5, api);
    if (api->opposing_selector(user(api), context, target) == 0xa) {
        const uint32_t selector = load8(out + 6, api);
        if (selector != 0xa) {
            const uint32_t t = api->target_for(user(api), context, selector);
            if (t != 0x1f) {
                set(DEFENDER, t, api);
                return;
            }
        }
        const uint32_t chosen = api->choose_target(user(api), context, id, 1);
        if (chosen != 0xa) {
            const uint32_t t = api->target_for(user(api), context, chosen);
            if (t != 0x1f) {
                set(DEFENDER, t, api);
                return;
            }
        }
        target = load8(out + 5, api);
    }
    set(DEFENDER, target, api);
}

PX_BATTLE_MOVES_16_HANDLER(px_battle_counter_target) { (void)handler, (void)work; hit_back(context, id, 1, api); }
PX_BATTLE_MOVES_16_HANDLER(px_battle_mirror_coat_target) { (void)handler, (void)work; hit_back(context, id, 2, api); }
PX_BATTLE_MOVES_16_HANDLER(px_battle_metal_burst_target) { (void)handler, (void)work; hit_back(context, id, 0, api); }

/* Bide releases (work word 0 at least 2, signed): the target is the first
 * damage kind (0-2) with a record; with no selector it is picked at random among
 * the opposing side's battlers, else it is 0x1f. Work word 2 keeps it. */
PX_BATTLE_MOVES_16_HANDLER(px_battle_bide_target)
{
    (void)handler;
    if (get(ATTACKER, api) != id) return;
    if ((int32_t)load32(work, api) < 2) return;
    const uint32_t pokemon = battler(context, id, api);
    const uint32_t buffer = api->scratch, out = api->scratch + 8;
    uint32_t target = 0x1f;
    for (uint32_t kind = 0; kind < 3; ++kind) {
        if (api->slot_used(user(api), pokemon, kind & 0xff) == 0) continue;
        api->damage_record(user(api), pokemon, kind & 0xff, 0, out);
        target = load8(out + 5, api);
        if (api->opposing_selector(user(api), context, target) == 0xa) {
            const uint32_t selector = api->base->side_of(user(api), context, id) | 0x100;
            const uint32_t count = api->base->side_battlers(user(api), context, selector, buffer);
            target = count != 0 ? load8(buffer + (api->random(user(api), count) & 0xff), api) : 0x1f;
        }
        break;
    }
    store32(work + 8, target, api);
    set(DEFENDER, target, api);
}

/* Sky Drop: the carried battler (counter 4, minus 1) becomes the target when it is a battler. */
PX_BATTLE_MOVES_16_HANDLER(px_battle_sky_drop_target)
{
    (void)handler, (void)work;
    if (api->battle_mode(user(api), context) == 0 || api->battle_mode(user(api), context) == 3) return;
    const uint32_t carried = api->counter(user(api), battler(context, id, api), 4);
    if (carried == 0) return;
    const uint32_t target = (carried - 1) & 0xff;
    if (target >= 0x18) return;
    set(DEFENDER, target, api);
}

/* Follow Me and Rage Powder: the user draws the move aimed at another battler (key 0x04) unless the
 * attacker is an ally, the move is move 507, the user is asleep (status 0x21), a switch is pending, or
 * (for the move 0x1dc) the defender is Grass, has ability 0x8e or holds item 0x28a; or the redirect is blocked. */
PX_BATTLE_MOVES_16_HANDLER(px_battle_follow_me_target)
{
    (void)work;
    if (api->battle_mode(user(api), context) == 0 || api->battle_mode(user(api), context) == 3) return;
    const uint32_t attacker = get(ATTACKER, api) & 0xff;
    const uint32_t pokemon = battler(context, id, api);
    const uint32_t other = battler(context, attacker, api);
    if (api->is_ally(user(api), id, attacker) != 0) return;
    if (get(MOVE, api) - 0x100 - 0xfb == 0) return;
    if (api->has_status(user(api), pokemon, 0x21) != 0) return;
    if (api->switch_pending(user(api), context) != 0) return;
    if (load16(handler + 0x38, api) == 0x1dc) {
        if (api->has_type(user(api), other, 0xb) != 0) return;
        if (api->base->pokemon_value(user(api), other, 0x11) == 0x8e) return;
        if (api->held_item(user(api), context, attacker) - 0x200 - 0x8a == 0) return;
    }
    if (api->redirect_blocked(user(api), context, attacker, id, get(MOVE, api) & 0xffff) != 0) return;
    set(DEFENDER, id, api);
}

/* Present: a 20% chance sets key 0x53 (once: work word 0); the result raises the effect flag. */
PX_BATTLE_MOVES_16_HANDLER(px_battle_present_flag)
{
    (void)handler;
    if (get(ATTACKER, api) != id) return;
    uint32_t value;
    if (api->chance(user(api), 0x14) == 0) {
        value = load32(work, api);
    } else {
        value = set(DONE, 1, api);
        store32(work, value, api);
    }
    api->set_effect_flag(user(api), context, value != 0);
}

/* Present's effect (work word 0 set): a full-HP defender gets message 0xd2 (kind 2), otherwise record 4 heals
 * a quarter, message 0x183 (literal 108b52b8). */
PX_BATTLE_MOVES_16_HANDLER(px_battle_present_effect)
{
    (void)handler;
    if (get(ATTACKER, api) != id || load32(work, api) == 0) return;
    const uint32_t target = get(DEFENDER, api) & 0xff;
    const uint32_t pokemon = battler(context, target, api);
    if (api->full_hp(user(api), pokemon) != 0) {
        const uint32_t rec = record(context, 3, id, api);
        say(rec + 4, 0xd2, target, api);
        commit(context, rec, api);
    } else {
        const uint32_t rec = record(context, 4, id, api);
        store8(rec + 6, target, api);
        store16(rec + 4, api->hp_fraction(user(api), pokemon, 4), api);
        say(rec + 8, 0x183, target, api);
        commit(context, rec, api);
    }
}

/* Thunder and Hurricane: weather 1 (1084be58) sets key 0x2c (the accuracy) to 0x32. */
PX_BATTLE_MOVES_16_HANDLER(px_battle_weather_accuracy)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    if (api->weather(user(api), context) != 1) return;
    set(0x2c, 0x32, api);
}

/* Beat Up: counts the party members that can still attack (one extra round per
 * qualifying position, up to six rounds): key 0x2b is the count, at least 1, and
 * work word 0 clears. A member qualifies when it is not the user and can battle
 * with a status; the round-th qualifying member is the one counted. */
PX_BATTLE_MOVES_16_HANDLER(px_battle_beat_up)
{
    (void)handler;
    if (get(ATTACKER, api) != id) return;
    uint32_t counted = 0;
    for (uint32_t round = 0; round < 6; ++round) {
        uint32_t skip = round & 0xff;
        const uint32_t party = api->party(user(api), context, id);
        const uint32_t size = load8(party + 0x18, api);
        uint32_t member = 0;
        if (size <= skip || size == 0) continue;
        for (uint32_t i = 0; i < size; ++i) {
            member = api->party_member(user(api), party, i & 0xff);
            int take = load8(member + 0x19, api) == id;
            if (!take && api->can_battle(user(api), member) != 0) take = api->base->pokemon_status(user(api), member) == 0;
            if (!take) continue;
            if (skip != 0) {
                skip = (skip - 1) & 0xff;
                continue;
            }
            if (member != 0) ++counted;
            break;
        }
    }
    if (counted == 0) counted = 1;
    set(0x2b, counted, api);
    store32(work, 0, api);
}

/* Triple Kick: key 0x43 set. */
PX_BATTLE_MOVES_16_HANDLER(px_battle_fixed_flag)
{
    (void)handler, (void)context, (void)work;
    if (get(ATTACKER, api) != id) return;
    set(0x43, 1, api);
}

/* Heal Bell and Aromatherapy: key 0x53 set. */
PX_BATTLE_MOVES_16_HANDLER(px_battle_support_ready)
{
    (void)handler, (void)context, (void)work;
    if (get(ATTACKER, api) != id) return;
    set(DONE, 1, api);
}

/* The pledges: key 0x53 set when work word 0 is set (battler key 0x02). */
PX_BATTLE_MOVES_16_HANDLER(px_battle_pledge_ready)
{
    (void)handler, (void)context;
    if (get(0x02, api) != id || load32(work, api) == 0) return;
    set(DONE, 1, api);
}

/* Clear Smog: unless key 0x47 is set, record 0xf (kind 1) and message 0xc3 name the defender. */
PX_BATTLE_MOVES_16_HANDLER(px_battle_move_message_499)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id || get(FLAG, api) != 0) return;
    const uint32_t target = get(DEFENDER, api) & 0xff;
    uint32_t rec = record(context, 0xf, id, api);
    store8(rec + 5, target, api);
    store8(rec + 4, 1, api);
    commit(context, rec, api);
    rec = record(context, 3, id, api);
    say(rec + 4, 0xc3, target, api);
    commit(context, rec, api);
}

/* Rage: a hit on the user (key 0x04), unless key 0x47 is set or it has no HP, raises its Attack
 * (record 0xd, bit 2 cleared and bit 0 and 2 set), message 0x214. */
PX_BATTLE_MOVES_16_HANDLER(px_battle_rage_boost)
{
    (void)handler, (void)work;
    if (get(DEFENDER, api) != id || get(FLAG, api) != 0) return;
    if (api->no_hp(user(api), battler(context, id, api)) != 0) return;
    const uint32_t rec = record(context, 0xd, id, api);
    const uint32_t flags = load8(rec + 0x18, api);
    store8(rec + 0xc, id, api);
    store8(rec + 4, 1, api);
    store8(rec + 0x17, 1, api);
    store8(rec + 0x16, 1, api);
    store8(rec + 0x18, (flags & ~UINT32_C(2)) | 5, api);
    say(rec + 0x1c, 0x214, id, api);
    commit(context, rec, api);
}

/* Bide stores the damage taken: key 0x33 is added to work word 1 (while Bide's flag, word 6, is set). */
PX_BATTLE_MOVES_16_HANDLER(px_battle_bide_damage)
{
    (void)handler, (void)context;
    if (get(DEFENDER, api) != id || get(FLAG, api) != 0) return;
    if (load32(work + 0x18, api) == 0) return;
    store32(work + 4, get(BONUS, api) + load32(work + 4, api), api);
}

/* Flame Burst: the other battlers of the target's side (selector | 0x900) each take a sixteenth (record 6, flag 1),
 * message 0x457 (literal 108b6aec). */
PX_BATTLE_MOVES_16_HANDLER(px_battle_chip_allies)
{
    (void)handler, (void)work;
    if (get(ATTACKER, api) != id) return;
    const uint32_t target = get(DEFENDER, api) & 0xff;
    const uint32_t position = api->position(user(api), context, target);
    if (position == 0xa) return;
    const uint32_t buffer = api->scratch;
    const uint32_t count = api->base->side_battlers(user(api), context, position | 0x900, buffer);
    for (uint32_t i = 0; i < count; i = (i + 1) & 0xff) {
        const uint32_t other = load8(buffer + i, api);
        if (other == target) continue;
        const uint32_t pokemon = battler(context, other, api);
        const uint32_t rec = record(context, 6, id, api);
        store8(rec + 6, other, api);
        store16(rec + 4, api->hp_fraction(user(api), pokemon, 0x10), api);
        store8(rec + 7, load8(rec + 7, api) | 1, api);
        say(rec + 0xc, 0x457, load8(rec + 6, api), api);
        commit(context, rec, api);
    }
}

/* Grudge: the user fainted (key 0x04, a move that may disable): the attacker's move used (key 0x14) loses
 * all its PP (record 9, minus the PP, flag 2), message 0x27b (literal 108b858c); the handler removes itself. */
PX_BATTLE_MOVES_16_HANDLER(px_battle_grudge_drain)
{
    (void)work;
    if (get(DEFENDER, api) != id) return;
    if (api->disable_blocked(user(api), get(MOVE, api) & 0xffff) != 0) return;
    if (api->no_hp(user(api), battler(context, id, api)) == 0) return;
    const uint32_t attacker = get(ATTACKER, api) & 0xff;
    const uint32_t pokemon = battler(context, attacker, api);
    const uint32_t move = get(USED, api) & 0xffff;
    const uint32_t slot = api->move_slot(user(api), pokemon, move);
    if (slot == 4) return;
    const uint32_t rec = record(context, 9, id, api);
    store8(rec + 5, attacker, api);
    store8(rec + 6, slot & 0xff, api);
    store8(rec + 4, 0 - api->move_pp(user(api), pokemon, slot & 0xff), api);
    store8(rec + 7, load8(rec + 7, api) | 2, api);
    api->message(user(api), rec + 8, 2, 0x27b);
    api->message_arg(user(api), rec + 8, attacker);
    api->message_arg(user(api), rec + 8, move);
    commit(context, rec, api);
    px_battle_handler_remove(handler, api->handlers);
}

/* Fling: the held item (+0x12) is used up (record 0x24; work word 0 set); unless keys 0x47/0x48 are set,
 * and the item has an effect (parameter 5), record 0x23 (the defender, the item). */
PX_BATTLE_MOVES_16_HANDLER(px_battle_fling_effect)
{
    (void)handler;
    if (get(ATTACKER, api) != id) return;
    const uint32_t item = load16(battler(context, id, api) + 0x12, api);
    if (item == 0) return;
    uint32_t rec = record(context, 0x24, id, api);
    store8(rec + 4, 1, api);
    store8(rec + 5, 1, api);
    commit(context, rec, api);
    store32(work, 1, api);
    if (get(FLAG, api) != 0 || get(FLAG2, api) != 0) return;
    if (api->item_param(user(api), item, 5) == 0) return;
    rec = record(context, 0x23, id, api);
    store8(rec + 4, get(DEFENDER, api), api);
    store16(rec + 6, item, api);
    store8(rec + 5, 1, api);
    commit(context, rec, api);
}
