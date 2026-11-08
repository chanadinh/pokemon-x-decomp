#ifndef PX_BATTLE_ABILITIES_2_H
#define PX_BATTLE_ABILITIES_2_H

#include "../battle_abilities_1/battle_abilities_1.h"

/* DllBattle weather abilities (static analysis addresses; runtime addresses
 * are module-relative). Analyst names. Weather: 1 sun, 2 rain, 3 hail,
 * 4 sandstorm. Event keys: 0x02 the battler concerned, 0x03 attacker,
 * 0x04 defender, 0x12 move, 0x33 weather damage, 0x36 multiplier, 0x3a the
 * event's weather, 0x42 the effect is cancelled (rewritten to 1).
 * Events: 0x88 end-of-turn weather, 0x13 speed, 0x39 evasion, 0x40 attack,
 * 0x7f end-of-turn status, 0x0e status immunity, 0x5e ability takes effect,
 * 0x83 weather suppression. */
struct px_battle_abilities_2_api {
    /* Event stack, battler lookup, pokemon_value (10874420), move_category
     * (108007dc) and pokemon_status (1086d8ac). */
    const struct px_battle_ability_api *base;
    uint32_t (*weather)(void *user, uint32_t context);                                /* 1084be58 */
    void (*ability_popup)(void *user, uint32_t context, uint32_t phase, uint32_t id); /* 1084c948; 1 show, 2 hide */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);        /* 1084d2d4 */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);            /* 10867b48 */
};

#define PX_BATTLE_ABILITIES_2_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_abilities_2_api *api)
/* Event 0x88. */
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_ice_body);          /* 108bd8fc, ability 115 */
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_rain_dish);         /* 108bfb58, ability 44 */
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_dry_skin_weather);  /* 108c4280, ability 87 */
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_solar_power_weather); /* 108c340c, ability 94 */
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_overcoat);          /* 108c368c, ability 142 */
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_sand_immunity);     /* 108c4584, abilities 8, 146 and 159 */
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_snow_cloak_weather); /* 108c4750, ability 81 */
/* Event 0x13. */
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_swift_swim);        /* 108bd29c, ability 33 */
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_chlorophyll);       /* 108c029c, ability 34 */
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_sand_rush);         /* 108beac0, ability 146 */
/* Event 0x39. */
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_sand_veil);         /* 108c01d8, ability 8 */
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_snow_cloak);        /* 108c02f4, ability 81 */
/* Event 0x40. */
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_solar_power);       /* 108c3d0c, ability 94 */
/* Event 0x7f. */
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_hydration);         /* 108bf9fc, ability 93 */
/* Event 0x0e. */
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_leaf_guard);        /* 108c55c0, ability 102 */
/* Events 0x5e and 0x83. */
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_air_lock);          /* 108c2ee0, abilities 13 and 76 */
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_air_lock_suppress); /* 108c51bc, abilities 13 and 76 */

#endif
