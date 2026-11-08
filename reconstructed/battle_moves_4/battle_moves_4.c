#include "battle_moves_4.h"
#include "../battle_modifier/battle_modifier.h"

enum { BATTLER = 0x02, ATTACKER = 0x03, DEFENDER = 0x04, STATE = 0x22, MISS = 0x43, SURE_HIT = 0x53, CHANCE = 0x20, STAGES = 0x21 };

static void *user(const struct px_battle_moves_4_api *api)
{
    return api->base->events->user;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_4_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t attacking(uint32_t id, const struct px_battle_moves_4_api *api)
{
    return get(ATTACKER, api) == id;
}

static uint32_t defender(uint32_t context, const struct px_battle_moves_4_api *api)
{
    return px_battle_pokemon_by_id(context, get(DEFENDER, api) & 0xff, api->base);
}

static void set(uint32_t key, uint32_t value, const struct px_battle_moves_4_api *api)
{
    px_battle_event_rewrite(key, value, api->base->events);
}

/* Hits the defender in the given semi-invulnerable state. */
static void hits_state(uint32_t id, uint32_t state, const struct px_battle_moves_4_api *api)
{
    if (!attacking(id, api)) return;
    if (get(STATE, api) == state) set(MISS, 0, api);
}

PX_BATTLE_MOVES_4_HANDLER(px_battle_sky_uppercut_hit) { (void)handler, (void)context, (void)work; hits_state(id, 3, api); }
PX_BATTLE_MOVES_4_HANDLER(px_battle_thunder_hit) { (void)handler, (void)context, (void)work; hits_state(id, 3, api); }
PX_BATTLE_MOVES_4_HANDLER(px_battle_gust_hit) { (void)handler, (void)context, (void)work; hits_state(id, 3, api); }
PX_BATTLE_MOVES_4_HANDLER(px_battle_earthquake_hit) { (void)handler, (void)context, (void)work; hits_state(id, 5, api); }
PX_BATTLE_MOVES_4_HANDLER(px_battle_whirlpool_hit) { (void)handler, (void)context, (void)work; hits_state(id, 4, api); }
PX_BATTLE_MOVES_4_HANDLER(px_battle_surf_hit) { (void)handler, (void)context, (void)work; hits_state(id, 4, api); }

PX_BATTLE_MOVES_4_HANDLER(px_battle_heal_bell_hit)
{
    (void)handler, (void)context, (void)work;
    if (attacking(id, api)) set(MISS, 0, api);
}

PX_BATTLE_MOVES_4_HANDLER(px_battle_rototiller_hit)
{
    (void)handler, (void)context, (void)work;
    if (attacking(id, api)) set(0x50, 0, api);
}

/* Never misses in the given weather. */
static void sure_in_weather(uint32_t context, uint32_t id, uint32_t weather, const struct px_battle_moves_4_api *api)
{
    if (!attacking(id, api)) return;
    if (api->weather(user(api), context) == weather) set(SURE_HIT, 1, api);
}

PX_BATTLE_MOVES_4_HANDLER(px_battle_blizzard_accuracy) { (void)handler, (void)work; sure_in_weather(context, id, 3, api); }
PX_BATTLE_MOVES_4_HANDLER(px_battle_thunder_accuracy) { (void)handler, (void)work; sure_in_weather(context, id, 2, api); }

PX_BATTLE_MOVES_4_HANDLER(px_battle_pursuit_accuracy)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if (api->target_switching(user(api), context) != 0) set(SURE_HIT, 1, api);
}

PX_BATTLE_MOVES_4_HANDLER(px_battle_stomp_accuracy)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if (px_battle_pokemon_state(defender(context, api), 8, api->base->events->memory) != 0) set(SURE_HIT, 1, api);
}

/* In sun, 1 stage becomes 2; key 0x21 is rewritten either way. */
PX_BATTLE_MOVES_4_HANDLER(px_battle_growth_stages)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if (api->weather(user(api), context) != 1) return;
    const uint32_t stages = get(STAGES, api);
    set(STAGES, stages == 1 ? 2 : stages, api);
}

PX_BATTLE_MOVES_4_HANDLER(px_battle_rototiller_target)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if (api->has_type(user(api), defender(context, api), 11) == 0) set(CHANCE, 0, api);
}

PX_BATTLE_MOVES_4_HANDLER(px_battle_venom_drench_target)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if (api->has_status(user(api), defender(context, api), 5) == 0) set(CHANCE, 0, api);
}

PX_BATTLE_MOVES_4_HANDLER(px_battle_magnetic_flux_target)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t ability = api->base->pokemon_value(user(api), defender(context, api), 0x11);
    if (ability == 57 || ability == 58) return;
    set(CHANCE, 0, api);
}

/* Key 0x04 tested first: the effect on the user itself. */
PX_BATTLE_MOVES_4_HANDLER(px_battle_ancient_power_chance)
{
    (void)handler, (void)context, (void)work;
    if (get(DEFENDER, api) != id) return;
    if (get(ATTACKER, api) != id) return;
    set(CHANCE, 10, api);
}

/* Removes this handler when key 0x02 is its battler. */
static void end_for(uint32_t handler, uint32_t battler, const struct px_battle_moves_4_api *api)
{
    if (get(BATTLER, api) == battler) px_battle_handler_remove(handler, api->handlers);
}

PX_BATTLE_MOVES_4_HANDLER(px_battle_endure_end) { (void)context, (void)work; end_for(handler, id, api); }
PX_BATTLE_MOVES_4_HANDLER(px_battle_helping_hand_end) { (void)context, (void)work; end_for(handler, id, api); }
PX_BATTLE_MOVES_4_HANDLER(px_battle_snatch_end) { (void)context, (void)work; end_for(handler, id, api); }
PX_BATTLE_MOVES_4_HANDLER(px_battle_magic_coat_end) { (void)context, (void)work; end_for(handler, id, api); }
PX_BATTLE_MOVES_4_HANDLER(px_battle_kings_shield_end) { (void)context, (void)work, (void)id; end_for(handler, 0x1f, api); }

/* Every registered move handler of this battler with the same move id (no key test). */
PX_BATTLE_MOVES_4_HANDLER(px_battle_follow_me_end)
{
    (void)context, (void)work;
    const struct px_pokemon_api *m = api->base->events->memory;
    const uint32_t move = m->load16(m->user, handler + UINT32_C(0x38));
    for (uint32_t h = px_battle_handler_find(0, id, api->handlers); h != 0;) {
        if (m->load16(m->user, h + UINT32_C(0x38)) == move) {
            px_battle_handler_remove(h, api->handlers);
            h = px_battle_handler_find(0, id, api->handlers);
        } else {
            h = px_battle_handler_find_next(h, api->handlers);
        }
    }
}
