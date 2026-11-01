#include "battle_power.h"

static void *user(const struct px_battle_power_api *api)
{
    return api->base->events->user;
}

static uint32_t get(uint32_t key, const struct px_battle_power_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static void multiply(uint32_t factor, const struct px_battle_power_api *api)
{
    px_battle_event_multiply(0x32, factor, api->base->events);
}

static uint32_t move(const struct px_battle_power_api *api)
{
    return get(0x12, api) & UINT32_C(0xffff);
}

PX_BATTLE_POWER_HANDLER(px_battle_rivalry)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    const uint32_t attacker = px_battle_pokemon_by_id(context, id, api->base);
    const uint32_t defender = px_battle_pokemon_by_id(context, get(0x04, api) & UINT32_C(0xff), api->base);
    const uint32_t mine = api->base->pokemon_value(user(api), attacker, 0x12) & UINT32_C(0xff);
    const uint32_t theirs = api->base->pokemon_value(user(api), defender, 0x12) & UINT32_C(0xff);
    /* Gender 2 (none) on either side: no change. */
    if (mine == 2 || theirs == 2) return;
    multiply(mine == theirs ? 0x1400 : 0xc00, api);
}

PX_BATTLE_POWER_HANDLER(px_battle_technician)
{
    (void)handler, (void)context, (void)work;
    if (get(0x03, api) == id && get(0x31, api) <= 0x3c) multiply(0x1800, api);
}

PX_BATTLE_POWER_HANDLER(px_battle_iron_fist)
{
    (void)handler, (void)context, (void)work;
    if (get(0x03, api) == id && api->move_flag(user(api), move(api), 7) != 0) multiply(0x1333, api);  /* literal 108c131c */
}

PX_BATTLE_POWER_HANDLER(px_battle_reckless)
{
    (void)handler, (void)context, (void)work;
    if (get(0x03, api) != id) return;
    const uint32_t m = move(api);
    /* Recoil moves, plus Jump Kick (26) and High Jump Kick (136). */
    if (api->move_value(user(api), m, 0x1e) != 0 || m == 0x1a || m == 0x88) multiply(0x1333, api);  /* literal 108bd338 */
}

PX_BATTLE_POWER_HANDLER(px_battle_heatproof)
{
    (void)handler, (void)context, (void)work;
    if (get(0x04, api) == id && get(0x16, api) == 9) multiply(0x800, api);
}

PX_BATTLE_POWER_HANDLER(px_battle_dry_skin_power)
{
    (void)handler, (void)context, (void)work;
    if (get(0x04, api) == id && get(0x16, api) == 9) multiply(0x1400, api);
}

PX_BATTLE_POWER_HANDLER(px_battle_sheer_force)
{
    (void)handler, (void)context, (void)work;
    if (get(0x03, api) == id && api->sheer_force_move(user(api), move(api)) != 0) multiply(0x14cd, api);  /* literal 108c474c */
}

/* Toxic Boost and Flare Boost: a status and a move category byte. */
static void status_boost(uint32_t context, uint32_t id, uint32_t status, uint32_t category,
                         const struct px_battle_power_api *api)
{
    if (get(0x03, api) != id) return;
    if (api->has_status(user(api), px_battle_pokemon_by_id(context, id, api->base), status) == 0) return;
    if (get(0x1b, api) == category) multiply(0x1800, api);
}

PX_BATTLE_POWER_HANDLER(px_battle_toxic_boost) { (void)handler, (void)work; status_boost(context, id, 5, 1, api); }
PX_BATTLE_POWER_HANDLER(px_battle_flare_boost) { (void)handler, (void)work; status_boost(context, id, 4, 2, api); }

PX_BATTLE_POWER_HANDLER(px_battle_analytic)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id || api->analytic_excluded(user(api), move(api)) != 0) return;
    if (api->moved_last(user(api), context, id) != 0) multiply(0x14cd, api);  /* literal 108bd25c */
}

PX_BATTLE_POWER_HANDLER(px_battle_sand_force)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id || api->weather(user(api), context) != 4) return;
    const uint32_t type = get(0x16, api) & UINT32_C(0xff);
    if (type == 4 || type == 5 || type == 8) multiply(0x14cd, api);  /* literal 108c12a4 */
}

/* Dark Aura and Fairy Aura act for any battler. Key 0x58 is a value entry, so
 * only the first aura handler can rewrite it; Aura Break (key 0x57) turns
 * the boost into a reduction. */
static void aura(uint32_t type, const struct px_battle_power_api *api)
{
    if (get(0x16, api) != type) return;
    if (px_battle_event_rewrite(0x58, 1, api->base->events) == 0) return;
    multiply(get(0x57, api) == 0 ? 0x1548 : 0xc00, api);  /* literals 108be1d8, 108bf0ac */
}

PX_BATTLE_POWER_HANDLER(px_battle_dark_aura) { (void)handler, (void)context, (void)id, (void)work; aura(16, api); }
PX_BATTLE_POWER_HANDLER(px_battle_fairy_aura) { (void)handler, (void)context, (void)id, (void)work; aura(17, api); }

PX_BATTLE_POWER_HANDLER(px_battle_strong_jaw)
{
    (void)handler, (void)context, (void)work;
    if (get(0x03, api) == id && api->biting_move(user(api), move(api)) != 0) multiply(0x1800, api);
}

PX_BATTLE_POWER_HANDLER(px_battle_tough_claws)
{
    (void)handler, (void)context, (void)work;
    if (get(0x03, api) == id && api->move_flag(user(api), move(api), 0) != 0) multiply(0x14cd, api);  /* literal 108bf38c */
}

/* Aerilate, Pixilate and Refrigerate: the type their event 0x28 entry
 * converted the move to, and the work word that entry set. */
static void converted(uint32_t id, uint32_t work, uint32_t type, const struct px_battle_power_api *api)
{
    const struct px_pokemon_api *m = api->base->events->memory;
    if (get(0x03, api) != id || get(0x16, api) != type) return;
    if (m->load32(m->user, work) != 0) multiply(0x14cd, api);  /* literals 108c0a70, 108c15f4, 108c1644 */
}

PX_BATTLE_POWER_HANDLER(px_battle_aerilate) { (void)handler, (void)context; converted(id, work, 2, api); }
PX_BATTLE_POWER_HANDLER(px_battle_pixilate) { (void)handler, (void)context; converted(id, work, 17, api); }
PX_BATTLE_POWER_HANDLER(px_battle_refrigerate) { (void)handler, (void)context; converted(id, work, 14, api); }

PX_BATTLE_POWER_HANDLER(px_battle_mega_launcher)
{
    (void)handler, (void)context, (void)work;
    if (get(0x03, api) != id) return;
    /* Water Pulse (352), Dark Pulse (399), Dragon Pulse (406), Aura Sphere (396). */
    const uint32_t m = move(api);
    if (m == 0x160 || m == 0x18f || m == 0x196 || m == 0x18c) multiply(0x1800, api);
}
