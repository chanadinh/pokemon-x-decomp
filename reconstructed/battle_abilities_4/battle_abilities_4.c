#include "battle_abilities_4.h"

static void *user(const struct px_battle_abilities_4_api *api)
{
    return api->base->events->user;
}

static uint32_t get(uint32_t key, const struct px_battle_abilities_4_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static void multiply(uint32_t factor, const struct px_battle_abilities_4_api *api)
{
    px_battle_event_multiply(0x36, factor, api->base->events);
}

static void rewrite(uint32_t key, uint32_t value, const struct px_battle_abilities_4_api *api)
{
    px_battle_event_rewrite(key, value, api->base->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_abilities_4_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static uint32_t held_item(uint32_t context, uint32_t id, const struct px_battle_abilities_4_api *api)
{
    const struct px_pokemon_api *m = api->base->events->memory;
    return m->load16(m->user, pokemon(context, id, api) + UINT32_C(0x12));
}

/* The battler of key `key` is this one: multiply key 0x36, or rewrite `target`. */
#define MULTIPLY(name, key, factor) \
    PX_BATTLE_ABILITIES_4_HANDLER(name) \
    { \
        (void)handler, (void)context, (void)work; \
        if (get(key, api) == id) multiply(factor, api); \
    }
#define REWRITE(name, key, target, value) \
    PX_BATTLE_ABILITIES_4_HANDLER(name) \
    { \
        (void)handler, (void)context, (void)work; \
        if (get(key, api) == id) rewrite(target, value, api); \
    }

/* Accuracy x1.3 (literal 108bd7f8). */
MULTIPLY(px_battle_compound_eyes, 0x03, 0x14cd)

PX_BATTLE_ABILITIES_4_HANDLER(px_battle_victory_star)
{
    (void)handler, (void)context, (void)work;
    /* The attacker or an ally of it: accuracy x1.1 (literal 108be2f8). */
    if (api->is_ally(user(api), id, get(0x03, api) & UINT32_C(0xff)) != 0) multiply(0x119a, api);
}

PX_BATTLE_ABILITIES_4_HANDLER(px_battle_hustle_accuracy)
{
    (void)handler, (void)context, (void)work;
    /* Physical moves (category 1): accuracy x0.8 (literal 108c3afc). */
    if (get(0x03, api) != id) return;
    if (api->base->move_category(user(api), get(0x12, api) & UINT32_C(0xffff)) == 1) multiply(0xccd, api);
}

PX_BATTLE_ABILITIES_4_HANDLER(px_battle_tangled_feet)
{
    (void)handler, (void)work;
    /* Confused (status 6): accuracy against it x0.5. */
    if (get(0x04, api) != id) return;
    if (api->has_status(user(api), pokemon(context, id, api), 6) != 0) multiply(0x800, api);
}

PX_BATTLE_ABILITIES_4_HANDLER(px_battle_wonder_skin)
{
    (void)handler, (void)context, (void)work;
    /* Status moves (category 0) against it: accuracy key 0x2c capped at 50. */
    if (get(0x04, api) != id || get(0x1b, api) != 0) return;
    if (get(0x2c, api) > 0x32) rewrite(0x2c, 0x32, api);
}

REWRITE(px_battle_keen_eye, 0x03, 0x4c, 1)

PX_BATTLE_ABILITIES_4_HANDLER(px_battle_unaware_stages)
{
    (void)handler, (void)context, (void)work;
    /* The other battler's stages count as neutral (6): evasion key 0x29 when
     * attacking, accuracy key 0x28 when defending. */
    if (get(0x03, api) == id) {
        rewrite(0x29, 6, api);
        return;
    }
    if (get(0x04, api) == id) rewrite(0x28, 6, api);
}

REWRITE(px_battle_unaware_defense, 0x04, 0x53, 1)
REWRITE(px_battle_unaware_attack, 0x03, 0x53, 1)

PX_BATTLE_ABILITIES_4_HANDLER(px_battle_scrappy)
{
    (void)handler, (void)context, (void)work;
    /* Against Ghost (key 0x15 = 7) the immunity is ignored. */
    if (get(0x03, api) == id && get(0x15, api) == 7) rewrite(0x4c, 1, api);
}

PX_BATTLE_ABILITIES_4_HANDLER(px_battle_super_luck)
{
    (void)handler, (void)context, (void)work;
    /* One more critical-hit stage (byte). */
    if (get(0x03, api) == id) rewrite(0x2d, (get(0x2d, api) + 1) & UINT32_C(0xff), api);
}

REWRITE(px_battle_battle_armor, 0x04, 0x42, 1)
REWRITE(px_battle_stall, 0x02, 0x11, 0)

PX_BATTLE_ABILITIES_4_HANDLER(px_battle_quick_feet)
{
    (void)handler, (void)work;
    /* With a status: Speed x1.5 and key 0x53 cleared (no paralysis cut). */
    if (get(0x02, api) != id) return;
    if (api->base->pokemon_status(user(api), pokemon(context, id, api)) == 0) return;
    multiply(0x1800, api);
    rewrite(0x53, 0, api);
}

PX_BATTLE_ABILITIES_4_HANDLER(px_battle_unburden)
{
    (void)handler;
    const struct px_pokemon_api *m = api->base->events->memory;
    /* Work word 1 (an item was held) and none held now: Speed x2. */
    if (get(0x02, api) != id || m->load32(m->user, work) != 1) return;
    if (held_item(context, id, api) == 0) multiply(0x2000, api);
}

PX_BATTLE_ABILITIES_4_HANDLER(px_battle_slow_start_speed)
{
    (void)handler, (void)context;
    const struct px_pokemon_api *m = api->base->events->memory;
    /* Work word +0xc: Slow Start still active. */
    if (get(0x02, api) == id && m->load32(m->user, work + UINT32_C(0xc)) != 0) multiply(0x800, api);
}

PX_BATTLE_ABILITIES_4_HANDLER(px_battle_unburden_item)
{
    (void)handler;
    const struct px_pokemon_api *m = api->base->events->memory;
    /* Event 0xa8 with key 0x2e clear: remember that an item is held. */
    if (get(0x02, api) != id || get(0x2e, api) != 0) return;
    if (held_item(context, id, api) != 0) m->store32(m->user, work, 1);
}

MULTIPLY(px_battle_heavy_metal, 0x02, 0x2000)
MULTIPLY(px_battle_light_metal, 0x02, 0x800)
REWRITE(px_battle_magic_guard, 0x04, 0x53, 0)
REWRITE(px_battle_rock_head, 0x03, 0x42, 1)
REWRITE(px_battle_skill_link, 0x03, 0x53, 1)

PX_BATTLE_ABILITIES_4_HANDLER(px_battle_parental_bond)
{
    (void)handler, (void)context, (void)work;
    /* A single-hit move (key 0x2a = 1, key 0x4e clear) not in the excluded
     * list and without move flag 1 strikes twice (key 0x2b = 2). */
    if (get(0x03, api) != id || get(0x2a, api) != 1 || get(0x4e, api) != 0) return;
    const uint32_t move = get(0x12, api) & UINT32_C(0xffff);
    if (api->bond_excluded(user(api), move) != 0 || api->move_flag(user(api), move, 1) != 0) return;
    rewrite(0x2b, 2, api);
}

PX_BATTLE_ABILITIES_4_HANDLER(px_battle_adaptability)
{
    (void)handler, (void)context, (void)work;
    /* Same-type moves (key 0x45 set): the multiplier key 0x36 is rewritten to x2. */
    if (get(0x02, api) == id && get(0x45, api) != 0) rewrite(0x36, 0x2000, api);
}

REWRITE(px_battle_inner_focus, 0x04, 0x42, 1)

PX_BATTLE_ABILITIES_4_HANDLER(px_battle_serene_grace)
{
    (void)handler, (void)context, (void)work;
    /* The effect chance key 0x27 doubled, as a halfword (literal 108c0ae8). */
    if (get(0x03, api) == id) rewrite(0x27, get(0x27, api) << 1 & UINT32_C(0xffff), api);
}

REWRITE(px_battle_serene_grace_flinch, 0x03, 0x46, 1)
