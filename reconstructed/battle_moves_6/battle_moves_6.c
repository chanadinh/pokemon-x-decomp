#include "battle_moves_6.h"

static void *user(const struct px_battle_moves_6_api *api)
{
    return api->base->events->user;
}

static void store8(uint32_t at, uint32_t value, const struct px_battle_moves_6_api *api)
{
    const struct px_pokemon_api *m = api->base->events->memory;
    m->store8(m->user, at, (uint8_t)value);
}

static uint32_t attacking(uint32_t id, const struct px_battle_moves_6_api *api)
{
    return px_battle_event_get(0x03, api->base->events) == id;
}

/* A message record (kind 3) whose message (kind 2) names the battler. */
static void announce(uint32_t context, uint32_t id, uint32_t message, const struct px_battle_moves_6_api *api)
{
    const uint32_t rec = api->action_record(user(api), context, 3, id);
    api->message(user(api), rec + 4, 2, message);
    api->message_arg(user(api), rec + 4, id);
    api->commit_actions(user(api), context, rec);
}

/* The battler becomes semi-invulnerable (kind 0x17), then the message. */
static void vanish(uint32_t context, uint32_t id, uint32_t state, uint32_t message, const struct px_battle_moves_6_api *api)
{
    if (!attacking(id, api)) return;
    const uint32_t rec = api->action_record(user(api), context, 0x17, id);
    store8(rec + 5, id, api);
    store8(rec + 4, state, api);
    api->commit_actions(user(api), context, rec);
    announce(context, id, message, api);
}

PX_BATTLE_MOVES_6_HANDLER(px_battle_fly_charge) { (void)handler, (void)work; vanish(context, id, 3, 0x211, api); }
PX_BATTLE_MOVES_6_HANDLER(px_battle_bounce_charge) { (void)handler, (void)work; vanish(context, id, 3, 0x220, api); }
PX_BATTLE_MOVES_6_HANDLER(px_battle_dive_charge) { (void)handler, (void)work; vanish(context, id, 4, 0x217, api); }
PX_BATTLE_MOVES_6_HANDLER(px_battle_dig_charge) { (void)handler, (void)work; vanish(context, id, 5, 0x21a, api); }
PX_BATTLE_MOVES_6_HANDLER(px_battle_shadow_force_charge) { (void)handler, (void)work; vanish(context, id, 6, 0x21d, api); }

static void charge(uint32_t context, uint32_t id, uint32_t message, const struct px_battle_moves_6_api *api)
{
    if (attacking(id, api)) announce(context, id, message, api);
}

PX_BATTLE_MOVES_6_HANDLER(px_battle_razor_wind_charge) { (void)handler, (void)work; charge(context, id, 0x223, api); }
PX_BATTLE_MOVES_6_HANDLER(px_battle_sky_attack_charge) { (void)handler, (void)work; charge(context, id, 0x226, api); }
PX_BATTLE_MOVES_6_HANDLER(px_battle_solar_beam_charge) { (void)handler, (void)work; charge(context, id, 0x229, api); }
PX_BATTLE_MOVES_6_HANDLER(px_battle_geomancy_charge) { (void)handler, (void)work; charge(context, id, 0x4bd, api); }

/* Freeze Shock (553, literal 108ba9c4) or any other move id (literal 108ba9c8). */
PX_BATTLE_MOVES_6_HANDLER(px_battle_freeze_shock_charge)
{
    (void)work;
    const struct px_pokemon_api *m = api->base->events->memory;
    if (!attacking(id, api)) return;
    const uint32_t move = m->load16(m->user, handler + UINT32_C(0x38));
    announce(context, id, move == 553 ? 0x362 : 0x365, api);
}

/* Message 0x22c, then Defense +1 for the battler (kind 0xd). */
PX_BATTLE_MOVES_6_HANDLER(px_battle_skull_bash_charge)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = api->base->events->memory;
    if (!attacking(id, api)) return;
    announce(context, id, 0x22c, api);
    const uint32_t rec = api->action_record(user(api), context, 0xd, id);
    const uint32_t flags = m->load8(m->user, rec + 0x18);
    store8(rec + 0xc, id, api);
    store8(rec + 0x16, 1, api);
    store8(rec + 4, 2, api);
    store8(rec + 0x17, 1, api);
    store8(rec + 0x18, flags | 6, api);
    api->commit_actions(user(api), context, rec);
}

#define RELEASE(name) \
    PX_BATTLE_MOVES_6_HANDLER(name) { (void)handler, (void)context, (void)id, (void)work, (void)api; }
RELEASE(px_battle_fly_release)
RELEASE(px_battle_bounce_release)
RELEASE(px_battle_dive_release)
RELEASE(px_battle_dig_release)
RELEASE(px_battle_shadow_force_release)
