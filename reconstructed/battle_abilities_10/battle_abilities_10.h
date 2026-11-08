#ifndef PX_BATTLE_ABILITIES_10_H
#define PX_BATTLE_ABILITIES_10_H

#include "../battle_ability/battle_ability.h"

/* DllBattle form abilities: Forecast (Castform, species 351), Flower Gift
 * (Cherrim, 421) and Zen Mode (Darmanitan, 555) (static analysis addresses;
 * runtime addresses are module-relative). Analyst names. Battlers: species
 * +0xc, form byte +0x153. Weather: 1 sun, 2 rain, 3 hail. Keys: 0x02 the
 * battler concerned, 0x03 attacker, 0x04 target, 0x10 the new ability,
 * 0x12 move, 0x1b category, 0x36 multiplier. Form records are kind 0x3a
 * (+4 the battler, +5 the form). */
struct px_battle_abilities_10_api {
    /* Event stack, battler lookup, pokemon_value (10874420; 0xd HP, 0xe max
     * HP) and move_category (108007dc). */
    const struct px_battle_ability_api *base;
    uint32_t (*weather)(void *user, uint32_t context);                                /* 1084be58 */
    uint32_t (*is_ally)(void *user, uint32_t id, uint32_t other);                     /* 1084c5a8 */
    void (*flower_gift)(void *user, uint32_t handler, uint32_t context, uint32_t id,
                        uint32_t sunny, uint32_t flag);                               /* 1084a944; flag on the stack */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);        /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);            /* 10867b48 */
};

#define PX_BATTLE_ABILITIES_10_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_abilities_10_api *api)
/* Forecast (59). */
PX_BATTLE_ABILITIES_10_HANDLER(px_battle_forecast_leave);       /* 108c22fc, event 0x73 */
PX_BATTLE_ABILITIES_10_HANDLER(px_battle_forecast_suppressed);  /* 108c2c58, event 0x82 */
PX_BATTLE_ABILITIES_10_HANDLER(px_battle_forecast_weather);     /* 108c2cec, events 0x02, 0x81 and 0x87 */
PX_BATTLE_ABILITIES_10_HANDLER(px_battle_forecast_lost);        /* 108c3dc8, event 0x93 */
PX_BATTLE_ABILITIES_10_HANDLER(px_battle_forecast_start);       /* 108c50b4, events 0x60 and 0x61 */
/* Flower Gift (122). */
PX_BATTLE_ABILITIES_10_HANDLER(px_battle_flower_gift_defense);  /* 108c2fac, event 0x41 */
PX_BATTLE_ABILITIES_10_HANDLER(px_battle_flower_gift_attack);   /* 108c3024, event 0x40 */
PX_BATTLE_ABILITIES_10_HANDLER(px_battle_flower_gift_leave);    /* 108c39b0, event 0x73 */
PX_BATTLE_ABILITIES_10_HANDLER(px_battle_flower_gift_suppressed); /* 108c40b0, event 0x82 */
PX_BATTLE_ABILITIES_10_HANDLER(px_battle_flower_gift_weather);  /* 108c4108, events 0x87, 0x02 and 0x81 */
PX_BATTLE_ABILITIES_10_HANDLER(px_battle_flower_gift_lost);     /* 108c4d34, event 0x93 */
PX_BATTLE_ABILITIES_10_HANDLER(px_battle_flower_gift_start);    /* 108c56d8, events 0x60 and 0x61 */
/* Zen Mode (161). */
PX_BATTLE_ABILITIES_10_HANDLER(px_battle_zen_mode);             /* 108bfbe8, event 0x81 */
PX_BATTLE_ABILITIES_10_HANDLER(px_battle_zen_mode_leave);       /* 108c4cac, events 0x73 and 0x93 */

#endif
