#include "battle_fields_2.h"
#include "../battle_fields_1/battle_fields_1.h"
#include "../battle_modifier/battle_modifier.h"

static const struct px_battle_ability_api *base(const struct px_battle_fields_2_api *api)
{
    return api->items->base;
}

static void *user(const struct px_battle_fields_2_api *api)
{
    return base(api)->events->user;
}

static uint32_t get(uint32_t key, const struct px_battle_fields_2_api *api)
{
    return px_battle_event_get(key, base(api)->events);
}

static uint32_t rewrite(uint32_t key, uint32_t value, const struct px_battle_fields_2_api *api)
{
    return px_battle_event_rewrite(key, value, base(api)->events);
}

static void multiply(uint32_t key, uint32_t factor, const struct px_battle_fields_2_api *api)
{
    px_battle_event_multiply(key, factor, base(api)->events);
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_fields_2_api *api)
{
    return api->items->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t at, const struct px_battle_fields_2_api *api)
{
    api->items->commit_actions(user(api), context, at);
}

PX_BATTLE_FIELDS_2_HANDLER(px_battle_position_noop_4f)
{
    (void)handler, (void)context, (void)id, (void)work, (void)api;
}

PX_BATTLE_FIELDS_2_HANDLER(px_battle_position_noop)
{
    (void)handler, (void)context, (void)id, (void)work, (void)api;
}

PX_BATTLE_FIELDS_2_HANDLER(px_battle_position_flag_4b)
{
    (void)handler, (void)context, (void)id, (void)work;
    rewrite(0x4b, 1, api);
}

/* Literal 108bc68c. */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_position_accuracy)
{
    (void)handler, (void)context, (void)id, (void)work;
    multiply(0x36, 0x1ab8, api);
}

PX_BATTLE_FIELDS_2_HANDLER(px_battle_position_ground)
{
    (void)handler, (void)context, (void)id, (void)work;
    if (get(0x16, api) == 4) rewrite(0x4c, 1, api);
}

/* Literal 108bc288 and 108bc25c: 0x548 (about one third). */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_position_water_sport)
{
    (void)handler, (void)context, (void)id, (void)work;
    if (get(0x16, api) == 9) multiply(0x32, 0x548, api);
}

PX_BATTLE_FIELDS_2_HANDLER(px_battle_position_mud_sport)
{
    (void)handler, (void)context, (void)id, (void)work;
    if (get(0x16, api) == 0xc) multiply(0x32, 0x548, api);
}

PX_BATTLE_FIELDS_2_HANDLER(px_battle_position_count_3e)
{
    (void)handler, (void)context, (void)id, (void)work;
    rewrite(0x3e, get(0x3e, api) + 1, api);
}

PX_BATTLE_FIELDS_2_HANDLER(px_battle_position_ion_deluge)
{
    (void)handler, (void)context, (void)id, (void)work;
    if (get(0x16, api) == 0) rewrite(0x16, 0xc, api);
}

/* A terrain's effect reaches a battler that is neither airborne nor exempt. */
static int grounded(uint32_t context, uint32_t battler, const struct px_battle_fields_2_api *api)
{
    if (api->airborne(user(api), context, battler) != 0) return 0;
    return api->exempt(user(api), context, battler) == 0;
}

/* A move of `type` from a grounded attacker: key 0x32 x `factor`. */
static void power(uint32_t context, uint32_t type, uint32_t factor, const struct px_battle_fields_2_api *api)
{
    if (get(0x16, api) != type) return;
    if (!grounded(context, get(0x03, api) & UINT32_C(0xff), api)) return;
    multiply(0x32, factor, api);
}

/* Grassy Terrain: Grass x1.5, and Earthquake (0x59), Bulldoze (0xde) and the
 * move 0x20b halved against a grounded target. */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_grassy_power)
{
    (void)handler, (void)id, (void)work;
    power(context, 0xb, 0x1800, api);
    const uint32_t move = get(0x12, api) & UINT32_C(0xffff);
    if (move != 0x59 && move != 0xde && move != 0x20b) return;
    if (!grounded(context, get(0x04, api) & UINT32_C(0xff), api)) return;
    multiply(0x32, 0x800, api);
}

/* Grassy Terrain at the end of the turn: a grounded battler restores 1/16
 * (kind 4, message 0x183, literal 108bc5ac). */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_grassy_heal)
{
    (void)handler, (void)id, (void)work;
    const struct px_pokemon_api *m = base(api)->events->memory;
    const uint32_t battler = get(0x02, api) & UINT32_C(0xff);
    if (battler == 0x1f) return;
    if (!grounded(context, battler, api)) return;
    const uint32_t p = px_battle_pokemon_by_id(context, battler, base(api));
    const uint32_t at = record(context, 4, battler, api);
    m->store8(m->user, at + 6, (uint8_t)battler);
    m->store16(m->user, at + 4, (uint16_t)px_battle_hp_share(p, 0x10, api->abilities));
    api->items->message(user(api), at + 8, 2, 0x183);
    api->items->message_arg(user(api), at + 8, battler);
    commit(context, at, api);
}

PX_BATTLE_FIELDS_2_HANDLER(px_battle_misty_power)
{
    (void)handler, (void)id, (void)work;
    if (get(0x16, api) != 0xf) return;
    if (!grounded(context, get(0x04, api) & UINT32_C(0xff), api)) return;
    multiply(0x32, 0x800, api);
}

PX_BATTLE_FIELDS_2_HANDLER(px_battle_electric_power)
{
    (void)handler, (void)id, (void)work;
    power(context, 0xc, 0x1800, api);
}

/* A shown block: kind 0x17 (+5 the battler, +4 `reason`). */
static void blocked(uint32_t context, uint32_t battler, uint32_t reason, const struct px_battle_fields_2_api *api)
{
    const struct px_pokemon_api *m = base(api)->events->memory;
    const uint32_t at = record(context, 0x17, battler, api);
    m->store8(m->user, at + 5, (uint8_t)battler);
    m->store8(m->user, at + 4, (uint8_t)reason);
    commit(context, at, api);
}

/* Misty Terrain: a major status given to a grounded battler is blocked. */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_misty_block)
{
    (void)handler, (void)id, (void)work;
    const uint32_t battler = get(0x04, api) & UINT32_C(0xff);
    if (!grounded(context, battler, api)) return;
    if (px_battle_major_status(get(0x1e, api) & UINT32_C(0xffff)) == 0) return;
    if (rewrite(0x42, 1, api) == 0) return;
    blocked(context, battler, 0x11, api);
}

/* Electric Terrain: sleep (2) or the status 0xe is blocked; sleep is tried
 * first and falls back to the second test when its rewrite does not take. */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_electric_block)
{
    (void)handler, (void)id, (void)work;
    const uint32_t battler = get(0x04, api) & UINT32_C(0xff);
    if (!grounded(context, battler, api)) return;
    if (get(0x04, api) == battler && (get(0x1e, api) & UINT32_C(0xffff)) == 2 && rewrite(0x42, 1, api) != 0) {
        blocked(context, battler, 0x10, api);
        return;
    }
    if (get(0x04, api) != battler) return;
    if ((get(0x1e, api) & UINT32_C(0xffff)) != 0xe) return;
    if (rewrite(0x42, 1, api) == 0) return;
    blocked(context, battler, 0x10, api);
}

/* The shows: when the battler has the terrain's state bit (`state`), kind
 * 0x18 (+4 the state, +5 the battler) and kind 3 with `message`. The final
 * commit passes the first record again, not the message record. */
static void shown(uint32_t context, uint32_t state, uint32_t message, const struct px_battle_fields_2_api *api)
{
    const struct px_pokemon_api *m = base(api)->events->memory;
    const uint32_t battler = get(0x04, api) & UINT32_C(0xff);
    const uint32_t p = px_battle_pokemon_by_id(context, battler, base(api));
    if (px_battle_pokemon_state(p, state, m) == 0) return;
    const uint32_t first = record(context, 0x18, battler, api);
    m->store8(m->user, first + 4, (uint8_t)state);
    m->store8(m->user, first + 5, (uint8_t)battler);
    commit(context, first, api);
    const uint32_t at = record(context, 3, battler, api);
    api->items->message(user(api), at + 4, 2, message);
    api->items->message_arg(user(api), at + 4, battler);
    commit(context, first, api);
}

PX_BATTLE_FIELDS_2_HANDLER(px_battle_misty_show) { (void)handler, (void)id, (void)work; shown(context, 0x11, 0x4c9, api); }
PX_BATTLE_FIELDS_2_HANDLER(px_battle_electric_show) { (void)handler, (void)id, (void)work; shown(context, 0x10, 0x4b7, api); }
