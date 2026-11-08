#include "battle_moves_5.h"

static void *user(const struct px_battle_moves_5_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_5_api *api)
{
    return api->base->events->memory;
}

static uint32_t attacking(uint32_t id, const struct px_battle_moves_5_api *api)
{
    return px_battle_event_get(0x03, api->base->events) == id;
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_moves_5_api *api)
{
    return api->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t rec, const struct px_battle_moves_5_api *api)
{
    api->commit_actions(user(api), context, rec);
}

static void store8(uint32_t at, uint32_t value, const struct px_battle_moves_5_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store8(m->user, at, (uint8_t)value);
}

static void store32(uint32_t at, uint32_t value, const struct px_battle_moves_5_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store32(m->user, at, value);
}

/* Bits 3..8 hold the turns, bit 1 marks a count; bits 9..24 (literal 1084be54, 0x01fffe00) are clear. */
uint32_t px_battle_turn_count(uint32_t turns)
{
    return ((turns << 3 & UINT32_C(0x1f8)) | 2) & ~UINT32_C(0x01fffe00);
}

void px_battle_side_condition(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, uint32_t side,
                              uint32_t condition, uint32_t count, uint32_t message, const struct px_battle_moves_5_api *api)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t rec = record(context, 0x19, id, api);
    store8(rec + 4, condition, api);
    store8(rec + 0xc, side, api);
    store32(rec + 8, count, api);   /* a single-precision register copy of the stack word */
    api->message(user(api), rec + 0x10, 1, message & 0xffff);
    api->message_arg(user(api), rec + 0x10, side);
    commit(context, rec, api);
}

/* A field condition (kind 0x1c) for `turns` turns with a message. */
static void field_condition(uint32_t context, uint32_t id, uint32_t condition, uint32_t turns, uint32_t message,
                            const struct px_battle_moves_5_api *api)
{
    const uint32_t rec = record(context, 0x1c, id, api);
    store8(rec + 4, condition, api);
    store32(rec + 8, px_battle_turn_count(turns), api);
    api->message(user(api), rec + 0xc, 1, message);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_5_HANDLER(px_battle_mud_sport)
{
    (void)handler, (void)work;
    if (attacking(id, api)) field_condition(context, id, 5, 5, 0x78, api);
}

PX_BATTLE_MOVES_5_HANDLER(px_battle_water_sport)
{
    (void)handler, (void)work;
    if (attacking(id, api)) field_condition(context, id, 4, 5, 0x76, api);
}

PX_BATTLE_MOVES_5_HANDLER(px_battle_fairy_lock)
{
    (void)handler, (void)work;
    if (attacking(id, api)) field_condition(context, id, 9, 2, 0x102, api);   /* literal 108b1768 */
}

PX_BATTLE_MOVES_5_HANDLER(px_battle_ion_deluge)
{
    (void)handler, (void)work;
    if (attacking(id, api)) field_condition(context, id, 8, 1, 0x101, api);   /* literal 108b62fc */
}

/* Not while gravity is in effect; the record also carries halfword 0x7b at +0xc
 * and the low 7 bits of +0xe set to 1; then effect 0x36. */
PX_BATTLE_MOVES_5_HANDLER(px_battle_gravity)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    if (api->field_active(user(api), context, 2) != 0) return;
    const uint32_t rec = record(context, 0x1c, id, api);
    store8(rec + 4, 2, api);
    const uint32_t count = px_battle_turn_count(5);
    const uint16_t flags = m->load16(m->user, rec + 0xe);
    store32(rec + 8, count, api);
    m->store16(m->user, rec + 0xc, 0x7b);
    m->store16(m->user, rec + 0xe, (uint16_t)((flags & ~0x7fu) | 1));
    commit(context, rec, api);
    api->effect(user(api), context, 0x36, id);
}

/* Rooms toggle: an active one ends (kind 0x1d), otherwise it starts for 5 turns. */
static void room(uint32_t context, uint32_t id, uint32_t condition, uint32_t message, const struct px_battle_moves_5_api *api)
{
    if (!attacking(id, api)) return;
    if (api->field_active(user(api), context, condition) != 0) {
        const uint32_t rec = record(context, 0x1d, id, api);
        store8(rec + 4, condition, api);
        commit(context, rec, api);
        return;
    }
    field_condition(context, id, condition, 5, message, api);
}

PX_BATTLE_MOVES_5_HANDLER(px_battle_wonder_room) { (void)handler, (void)work; room(context, id, 6, 0xb8, api); }
PX_BATTLE_MOVES_5_HANDLER(px_battle_magic_room) { (void)handler, (void)work; room(context, id, 7, 0xba, api); }

/* Trick Room's message (kind 2) also names the battler. */
PX_BATTLE_MOVES_5_HANDLER(px_battle_trick_room)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if (api->field_active(user(api), context, 1) != 0) {
        const uint32_t rec = record(context, 0x1d, id, api);
        store8(rec + 4, 1, api);
        commit(context, rec, api);
        return;
    }
    const uint32_t rec = record(context, 0x1c, id, api);
    store8(rec + 4, 1, api);
    store32(rec + 8, px_battle_turn_count(5), api);
    api->message(user(api), rec + 0xc, 2, 0x35c);
    api->message_arg(user(api), rec + 0xc, id);
    commit(context, rec, api);
}

/* A terrain (condition 10) unless that terrain is already up. */
static void terrain(uint32_t context, uint32_t id, uint32_t kind, uint32_t message, const struct px_battle_moves_5_api *api)
{
    if (!attacking(id, api)) return;
    if (api->terrain(user(api), context) == kind) return;
    const uint32_t rec = record(context, 0x1c, id, api);
    store8(rec + 5, kind, api);
    store8(rec + 4, 10, api);
    store32(rec + 8, px_battle_turn_count(5), api);
    api->message(user(api), rec + 0xc, 1, message);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_5_HANDLER(px_battle_grassy_terrain) { (void)handler, (void)work; terrain(context, id, 1, 0xcd, api); }
PX_BATTLE_MOVES_5_HANDLER(px_battle_misty_terrain) { (void)handler, (void)work; terrain(context, id, 2, 0xcf, api); }
PX_BATTLE_MOVES_5_HANDLER(px_battle_electric_terrain) { (void)handler, (void)work; terrain(context, id, 3, 0xd1, api); }

/* Haze: a record listing the battlers of selector (opposing | 0x800), then a message. */
PX_BATTLE_MOVES_5_HANDLER(px_battle_haze)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t rec = record(context, 0xf, id, api);
    const uint32_t selector = api->opposing_selector(user(api), context, id) | 0x800;
    store8(rec + 4, api->base->side_battlers(user(api), context, selector, rec + 5), api);
    commit(context, rec, api);
    const uint32_t note = record(context, 3, id, api);
    api->message(user(api), note + 4, 1, 0x69);
    commit(context, note, api);
}

/* Side conditions of the user's side for some turns. */
static void own_side(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, uint32_t turns, uint32_t condition,
                     uint32_t message, const struct px_battle_moves_5_api *api)
{
    const uint32_t count = px_battle_turn_count(turns);
    px_battle_side_condition(handler, context, id, work, api->side_of(user(api), id), condition, count, message, api);
}

/* Hazards on the opposing side with a count of 1. */
static void hazard(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, uint32_t condition, uint32_t message,
                   const struct px_battle_moves_5_api *api)
{
    px_battle_side_condition(handler, context, id, work, api->opposing_side_of(user(api), id), condition, 1, message, api);
}

PX_BATTLE_MOVES_5_HANDLER(px_battle_reflect) { own_side(handler, context, id, work, 5, 0, 0x82, api); }
PX_BATTLE_MOVES_5_HANDLER(px_battle_light_screen) { own_side(handler, context, id, work, 5, 1, 0x86, api); }
PX_BATTLE_MOVES_5_HANDLER(px_battle_mist) { own_side(handler, context, id, work, 5, 3, 0x8e, api); }
PX_BATTLE_MOVES_5_HANDLER(px_battle_tailwind) { own_side(handler, context, id, work, 4, 4, 0x92, api); }
PX_BATTLE_MOVES_5_HANDLER(px_battle_lucky_chant) { own_side(handler, context, id, work, 5, 5, 0x96, api); }
PX_BATTLE_MOVES_5_HANDLER(px_battle_crafty_shield) { own_side(handler, context, id, work, 1, 0x10, 0xd3, api); }

/* Safeguard looks its side up before the turn count. */
PX_BATTLE_MOVES_5_HANDLER(px_battle_safeguard)
{
    const uint32_t side = api->side_of(user(api), id);
    px_battle_side_condition(handler, context, id, work, side, 2, px_battle_turn_count(5), 0x8a, api);
}

PX_BATTLE_MOVES_5_HANDLER(px_battle_spikes) { hazard(handler, context, id, work, 6, 0x9a, api); }
PX_BATTLE_MOVES_5_HANDLER(px_battle_toxic_spikes) { hazard(handler, context, id, work, 7, 0x9e, api); }
PX_BATTLE_MOVES_5_HANDLER(px_battle_stealth_rock) { hazard(handler, context, id, work, 8, 0xa2, api); }
PX_BATTLE_MOVES_5_HANDLER(px_battle_sticky_web) { hazard(handler, context, id, work, 0xe, 0xd6, api); }

/* Quick and Wide Guard: the side condition for one turn, then (without a key
 * check) a kind-0x27 record raising the battler's counter 3. */
static void guard(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, uint32_t condition, uint32_t message,
                  const struct px_battle_moves_5_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    own_side(handler, context, id, work, 1, condition, message, api);
    const uint32_t pokemon = px_battle_pokemon_by_id(context, id, api->base);
    const uint32_t rec = record(context, 0x27, id, api);
    const uint32_t uses = api->counter(user(api), pokemon, 3);
    const uint32_t flags = m->load32(m->user, rec);
    store8(rec + 4, id, api);
    store8(rec + 5, 3, api);
    store8(rec + 6, uses + 1, api);
    store32(rec, flags | UINT32_C(0x1000000), api);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_5_HANDLER(px_battle_quick_guard) { guard(handler, context, id, work, 0xa, 0xa8, api); }
PX_BATTLE_MOVES_5_HANDLER(px_battle_wide_guard) { guard(handler, context, id, work, 9, 0xa6, api); }

/* Mat Block builds its side record itself: message kind 2 naming the battler. */
PX_BATTLE_MOVES_5_HANDLER(px_battle_mat_block)
{
    (void)handler, (void)work;
    const uint32_t count = px_battle_turn_count(1);
    const uint32_t side = api->side_of(user(api), id);
    if (!attacking(id, api)) return;
    const uint32_t rec = record(context, 0x19, id, api);
    store8(rec + 4, 0xf, api);
    store8(rec + 0xc, side, api);
    store32(rec + 8, count, api);
    api->message(user(api), rec + 0x10, 2, 0x4a5);   /* literal 108b6394 */
    api->message_arg(user(api), rec + 0x10, id);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_5_HANDLER(px_battle_happy_hour)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    api->happy_hour(user(api), context, id, 1);
    const uint32_t rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 1, 0xff);
    commit(context, rec, api);
}
