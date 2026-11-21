#include "battle_fields_1.h"

enum { HANDLER_KIND = 0x38, LAYER_BYTE = 0xc, ROWS = 17, SIDES = 2 };

static const struct px_battle_ability_api *base(const struct px_battle_fields_1_api *api)
{
    return api->items->base;
}

static void *user(const struct px_battle_fields_1_api *api)
{
    return base(api)->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_fields_1_api *api)
{
    return base(api)->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_fields_1_api *api)
{
    return px_battle_event_get(key, base(api)->events);
}

static uint32_t rewrite(uint32_t key, const struct px_battle_fields_1_api *api)
{
    return px_battle_event_rewrite(key, 1, base(api)->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_fields_1_api *api)
{
    return px_battle_pokemon_by_id(context, id, base(api));
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_fields_1_api *api)
{
    return api->items->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t at, const struct px_battle_fields_1_api *api)
{
    api->items->commit_actions(user(api), context, at);
}

static void text(uint32_t at, uint32_t kind, uint32_t message, const struct px_battle_fields_1_api *api)
{
    api->items->message(user(api), at, kind, message);
}

static void arg(uint32_t at, uint32_t value, const struct px_battle_fields_1_api *api)
{
    api->items->message_arg(user(api), at, value);
}

uint32_t px_battle_slot_side(uint32_t slot)
{
    return slot >= 12 ? 1 : 0;
}

uint32_t px_battle_major_status(uint32_t status)
{
    return status < 6 ? 1 : 0;
}

uint32_t px_battle_mode(uint32_t context, const struct px_pokemon_api *m)
{
    const uint32_t outer = m->load32(m->user, context + 4);
    const uint32_t inner = m->load32(m->user, outer + 0x10);
    return m->load8(m->user, inner + 1);
}

void px_battle_message_flag(uint32_t buffer, const struct px_pokemon_api *m)
{
    m->store16(m->user, buffer + 2, (uint16_t)(m->load16(m->user, buffer + 2) | 0x8000));
}

/* A battler key (low byte) on the registered side. */
static int on_side(uint32_t key, uint32_t side, const struct px_battle_fields_1_api *api)
{
    return px_battle_slot_side(get(key, api) & UINT32_C(0xff)) == side;
}

/* Reflect and Light Screen halve a non-critical hit of their category on
 * their side; in doubles and the like (any mode but 0 and 3) the factor is
 * the literal 0xaac (2/3). */
static void screen(uint32_t context, uint32_t side, uint32_t category, const struct px_battle_fields_1_api *api)
{
    if (!on_side(0x04, side, api)) return;
    if (get(0x1b, api) != category) return;
    if (get(0x46, api) != 0) return;
    const uint32_t mode = px_battle_mode(context, memory(api));
    const uint32_t factor = mode == 0 || mode == 3 ? 0x800 : 0xaac;
    px_battle_event_multiply(0x36, factor, base(api)->events);
}

PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_reflect) { (void)handler, (void)work; screen(context, side, 1, api); }
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_light_screen) { (void)handler, (void)work; screen(context, side, 2, api); }

/* Safeguard: a major status, confusion (6) or 0xe aimed at the side by
 * another battler is blocked (key 0x42); work word 0 keeps whether that
 * took, and event 0x70 shows it once (message 0x34a, literal 108acc90). */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_safeguard)
{
    (void)handler, (void)context;
    const struct px_pokemon_api *m = memory(api);
    const uint32_t target = get(0x04, api) & UINT32_C(0xff);
    if (px_battle_slot_side(target) != side) return;
    if (get(0x03, api) == target) return;
    const uint32_t status = get(0x1e, api) & UINT32_C(0xffff);
    if (!px_battle_major_status(status) && status != 6 && status != 0xe) return;
    m->store32(m->user, work, rewrite(0x42, api));
}

PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_safeguard_show)
{
    (void)handler, (void)side;
    const struct px_pokemon_api *m = memory(api);
    if (m->load32(m->user, work) == 0) return;
    const uint32_t target = get(0x04, api) & UINT32_C(0xff);
    const uint32_t at = record(context, 3, target, api);
    text(at + 4, 2, 0x34a, api);
    arg(at + 4, target, api);
    commit(context, at, api);
    m->store32(m->user, work, 0);
}

/* Mist: a stat drop (key 0x21 negative) by another battler is blocked. */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_mist)
{
    (void)handler, (void)context;
    const struct px_pokemon_api *m = memory(api);
    const uint32_t target = get(0x02, api) & UINT32_C(0xff);
    if (px_battle_slot_side(target) != side) return;
    if ((int32_t)get(0x21, api) >= 0) return;
    if (get(0x03, api) == target) return;
    m->store32(m->user, work, rewrite(0x42, api));
}

/* Event 0x65: show it once per move: work word 1 remembers key 0x1a;
 * message 0x34d (literal 108acbc4) and the flag of 108802a8. */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_mist_show)
{
    (void)handler, (void)side;
    const struct px_pokemon_api *m = memory(api);
    if (m->load32(m->user, work) == 0) return;
    const uint32_t move = get(0x1a, api);
    if (move == 0 || m->load32(m->user, work + 4) != move) {
        const uint32_t target = get(0x02, api) & UINT32_C(0xff);
        const uint32_t at = record(context, 3, target, api);
        text(at + 4, 2, 0x34d, api);
        arg(at + 4, target, api);
        px_battle_message_flag(at + 4, m);
        commit(context, at, api);
        m->store32(m->user, work + 4, move);
    }
    m->store32(m->user, work, 0);
}

/* Tailwind and Swamp: the side's speed. */
static void speed(uint32_t side, uint32_t factor, const struct px_battle_fields_1_api *api)
{
    if (on_side(0x02, side, api)) px_battle_event_multiply(0x36, factor, base(api)->events);
}

PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_tailwind) { (void)handler, (void)context, (void)work; speed(side, 0x2000, api); }
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_swamp) { (void)handler, (void)context, (void)work; speed(side, 0x400, api); }

PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_lucky_chant)
{
    (void)handler, (void)context, (void)work;
    if (on_side(0x04, side, api)) rewrite(0x42, api);
}

PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_rainbow_75)
{
    (void)handler, (void)context, (void)work;
    if (on_side(0x03, side, api)) rewrite(0x46, api);
}

/* The attacker's side doubles key 0x27 (kept to 16 bits). */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_rainbow_chance)
{
    (void)handler, (void)context, (void)work;
    if (!on_side(0x03, side, api)) return;
    px_battle_event_rewrite(0x27, (get(0x27, api) << 1) & UINT32_C(0xffff), base(api)->events);
}

/* The layers of the handler's row on this side. A row of 17 or a side of 2
 * faults and reads as 0. */
static uint32_t layers_of(uint32_t handler, uint32_t side, const struct px_battle_fields_1_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t kind = m->load16(m->user, handler + HANDLER_KIND) & UINT32_C(0xff);
    if (kind >= ROWS || side >= SIDES) {
        base(api)->events->abort(user(api));
        return 0;
    }
    return m->load32(m->user, api->layers + (side * 17 + kind) * 16 + LAYER_BYTE) & UINT32_C(0xff);
}

/* The damage record of a hazard: kind 6 for slot 0x1f, +6 the battler, +4
 * max HP / divisor, a message at +0xc naming the battler. `looked_up` is the
 * battler's Pokemon when the handler already fetched it, else UINT32_MAX and
 * it is fetched after the record. */
static void hazard_damage(uint32_t context, uint32_t battler, uint32_t looked_up, uint32_t divisor,
                          uint32_t message, const struct px_battle_fields_1_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t at = record(context, 6, 0x1f, api);
    m->store8(m->user, at + 6, (uint8_t)battler);
    const uint32_t p = looked_up == UINT32_MAX ? pokemon(context, battler, api) : looked_up;
    m->store16(m->user, at + 4, (uint16_t)px_battle_hp_share(p, divisor, api->abilities));
    text(at + 0xc, 2, message, api);
    arg(at + 0xc, battler, api);
    commit(context, at, api);
}

/* Spikes: an unprotected (not airborne) battler loses 1/8, 1/6 or 1/4 for
 * one, two or three layers (message 0x356, literal 108ac564). */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_spikes)
{
    (void)work;
    const uint32_t battler = get(0x02, api) & UINT32_C(0xff);
    if (px_battle_slot_side(battler) != side) return;
    if (api->airborne(user(api), context, battler) != 0) return;
    const uint32_t layers = layers_of(handler, side, api);
    const uint32_t divisor = layers == 2 ? 6 : layers == 3 ? 4 : 8;
    hazard_damage(context, battler, UINT32_MAX, divisor, 0x356, api);
}

/* Toxic Spikes: a Poison battler absorbs them (kind 0x1a), another is
 * poisoned: badly (condition 0x79) from the second layer. */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_toxic_spikes)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    const uint32_t battler = get(0x02, api) & UINT32_C(0xff);
    if (px_battle_slot_side(battler) != side) return;
    if (api->airborne(user(api), context, battler) != 0) return;
    const uint32_t p = pokemon(context, battler, api);
    if (api->has_type(user(api), p, 3) != 0) {
        /* Kind 0x1a: +8 the side, +4 4, +5 0x80, +6 and +7 0. The original
         * writes the fields through a pre-indexed store sequence. */
        const uint32_t at = record(context, 0x1a, battler, api);
        m->store8(m->user, at + 8, (uint8_t)side);
        m->store8(m->user, at + 4, 4);
        m->store8(m->user, at + 5, 0);
        m->store8(m->user, at + 6, 0);
        m->store8(m->user, at + 7, 0);
        if (m->load8(m->user, at + 4) > 1) m->store8(m->user, at + 5, (uint8_t)(m->load8(m->user, at + 5) | 0x80));
        commit(context, at, api);
        return;
    }
    const uint32_t at = record(context, 0xb, battler, api);
    m->store16(m->user, at + 4, 5);
    uint32_t condition = 1;
    const uint32_t kind = m->load16(m->user, handler + HANDLER_KIND) & UINT32_C(0xff);
    if (kind < ROWS && side < SIDES) {
        if ((m->load32(m->user, api->layers + (side * 17 + kind) * 16 + LAYER_BYTE) & UINT32_C(0xff)) > 1) condition = 0x79;
    } else {
        base(api)->events->abort(user(api));
    }
    m->store32(m->user, at + 8, condition);
    m->store8(m->user, at + 0xf, (uint8_t)battler);
    commit(context, at, api);
}

/* Stealth Rock: 1/8 of max HP scaled by the Rock effectiveness against the
 * battler's types, through the table of divisors (1/2 at four times
 * effective and 1/32 at a quarter and so on). */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_stealth_rock)
{
    (void)handler, (void)work;
    const uint32_t battler = get(0x02, api) & UINT32_C(0xff);
    if (px_battle_slot_side(battler) != side) return;
    const uint32_t p = pokemon(context, battler, api);
    const uint32_t code = api->effectiveness(user(api), 5, api->packed_types(user(api), p));
    static const uint16_t divisors[14] = {8, 0x100, 0x100, 0x80, 0x40, 0x20, 0x10, 8, 4, 2, 1, 1, 1, 1};
    const uint32_t divisor = code < 14 ? divisors[code] : 8;
    hazard_damage(context, battler, p, divisor, 0x359, api);
}

/* The guards: the buffer of key 0x40 gets message `text` naming the
 * defender once key 0x41 (move blocked) took. */
static void block_message(uint32_t message, uint32_t defender, const struct px_battle_fields_1_api *api)
{
    const uint32_t buffer = get(0x40, api);
    text(buffer, 2, message, api);
    arg(buffer, defender, api);
}

/* Wide Guard: a damaging move aimed at all opponents or all battlers
 * (target class 4 or 5) against this side. */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_wide_guard)
{
    (void)handler, (void)context, (void)work;
    const uint32_t defender = get(0x04, api) & UINT32_C(0xff);
    if (px_battle_slot_side(defender) != side) return;
    const uint32_t move = get(0x12, api) & UINT32_C(0xffff);
    const uint32_t target = api->move_value(user(api), move, 0x1b) & UINT32_C(0xff);
    if (api->damaging_move(user(api), move) == 0) return;
    if (target != 5 && target != 4) return;
    if (rewrite(0x41, api) == 0) return;
    block_message(0x31d, defender, api);
}

/* Quick Guard: a move of positive priority (key 0x19) that move flag 3
 * covers, from another battler. */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_quick_guard)
{
    (void)handler, (void)context, (void)work;
    const uint32_t defender = get(0x04, api) & UINT32_C(0xff);
    if (px_battle_slot_side(defender) != side) return;
    if (get(0x03, api) == defender) return;
    const uint32_t move = get(0x12, api) & UINT32_C(0xffff);
    if ((int32_t)get(0x19, api) < 1) return;
    if (api->move_flag(user(api), move, 3) == 0) return;
    if (rewrite(0x41, api) == 0) return;
    block_message(0x320, defender, api);
}

/* Crafty Shield: a non-damaging move whose category (key 0x1c) is not 8 or
 * 6, from another battler. */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_crafty_shield)
{
    (void)handler, (void)context, (void)work;
    const uint32_t defender = get(0x04, api) & UINT32_C(0xff);
    if (px_battle_slot_side(defender) != side) return;
    const uint32_t move = get(0x12, api) & UINT32_C(0xffff);
    const uint32_t category = get(0x1c, api) & UINT32_C(0xff);
    if (api->damaging_move(user(api), move) != 0) return;
    if (get(0x03, api) == defender) return;
    if (category == 8 || category == 6) return;
    if (rewrite(0x41, api) == 0) return;
    block_message(0x323, defender, api);
}

/* Mat Block: a damaging move; message 0xd5 names the move, then key 0x53. */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_mat_block)
{
    (void)handler, (void)context, (void)work;
    if (!on_side(0x04, side, api)) return;
    const uint32_t move = get(0x12, api) & UINT32_C(0xffff);
    if (api->damaging_move(user(api), move) == 0) return;
    if (rewrite(0x41, api) == 0) return;
    const uint32_t buffer = get(0x40, api);
    text(buffer, 1, 0xd5, api);
    arg(buffer, move, api);
    rewrite(0x53, api);
}

/* Sea of Fire: at the end of the turn a present, non-Fire battler on the
 * side takes 1/8, after an animation record (kind 0x38: +6 its position, +7
 * 0xa, +4 0x27c). Message 0x48a (literal 108ac2d4). */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_sea_of_fire)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    const uint32_t battler = get(0x02, api) & UINT32_C(0xff);
    if (battler == 0x1f) return;
    if (api->slot_present(user(api), context, battler) == 0) return;
    if (api->battle_decided(user(api), context) != 0) return;
    if (px_battle_slot_side(battler) != side) return;
    const uint32_t p = pokemon(context, battler, api);
    if (api->has_type(user(api), p, 9) != 0) return;
    uint32_t at = record(context, 0x38, battler, api);
    m->store8(m->user, at + 6, (uint8_t)api->side_of_position(user(api), context, battler));
    m->store8(m->user, at + 7, 0xa);
    m->store16(m->user, at + 4, 0x27c);
    commit(context, at, api);
    at = record(context, 6, 0x1f, api);
    m->store8(m->user, at + 6, (uint8_t)battler);
    m->store16(m->user, at + 4, (uint16_t)px_battle_hp_share(p, 8, api->abilities));
    text(at + 0xc, 2, 0x48a, api);
    arg(at + 0xc, battler, api);
    commit(context, at, api);
}

/* Sticky Web: an unprotected battler's Speed falls by one stage (kind 0xd:
 * +4 5, +0x16 1, +0x17 0xff, +0x18 bits 3 and 4 set, +0xc the battler);
 * message 0x4c6 (literal 108ac8f8) at +0x1c. */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_sticky_web)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    const uint32_t battler = get(0x02, api) & UINT32_C(0xff);
    if (px_battle_slot_side(battler) != side) return;
    if (api->airborne(user(api), context, battler) != 0) return;
    const uint32_t at = record(context, 0xd, 0x1f, api);
    const uint8_t flags = m->load8(m->user, at + 0x18);
    m->store8(m->user, at + 0x16, 1);
    m->store8(m->user, at + 4, 5);
    m->store8(m->user, at + 0x17, 0xff);
    m->store8(m->user, at + 0xc, (uint8_t)battler);
    m->store8(m->user, at + 0x18, (uint8_t)(flags | 0x18));
    text(at + 0x1c, 2, 0x4c6, api);
    arg(at + 0x1c, battler, api);
    commit(context, at, api);
}
