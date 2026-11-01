#ifndef PX_BATTLE_POWER_H
#define PX_BATTLE_POWER_H

#include "../battle_ability/battle_ability.h"

/* DllBattle ability handlers for the move-power event 0x3d (static analysis
 * addresses; runtime addresses are module-relative). Analyst names.
 * Additional event keys: 0x31 move power, 0x32 power multiplier, 0x57 Aura
 * Break, 0x58 aura applied. Types: 2 Flying, 4 Ground, 5 Rock, 8 Steel,
 * 9 Fire, 14 Ice, 16 Dark, 17 Fairy. Value 0x12 is the gender (2 none). */
struct px_battle_power_api {
    /* Event stack, battler lookup and pokemon_value (10874420). */
    const struct px_battle_ability_api *base;
    uint32_t (*move_flag)(void *user, uint32_t move, uint32_t flag);    /* 1080049c; 0 contact, 7 punch */
    uint32_t (*move_value)(void *user, uint32_t move, uint32_t field);  /* 10800494; 0x1e recoil */
    uint32_t (*sheer_force_move)(void *user, uint32_t move);            /* 1084a890 */
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status); /* 10856a88; 4 burn, 5 poison */
    uint32_t (*analytic_excluded)(void *user, uint32_t move);           /* 1084c808 */
    uint32_t (*moved_last)(void *user, uint32_t context, uint32_t id);  /* 1084b8e0 */
    uint32_t (*weather)(void *user, uint32_t context);                  /* 1084be58; 4 sandstorm */
    uint32_t (*biting_move)(void *user, uint32_t move);                 /* 108a4714 */
};

#define PX_BATTLE_POWER_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_power_api *api)
PX_BATTLE_POWER_HANDLER(px_battle_rivalry);        /* 108bf95c, ability 79 */
PX_BATTLE_POWER_HANDLER(px_battle_technician);     /* 108c021c, ability 101 */
PX_BATTLE_POWER_HANDLER(px_battle_iron_fist);      /* 108c12d4, ability 89 */
PX_BATTLE_POWER_HANDLER(px_battle_reckless);       /* 108bd2dc, ability 120 */
PX_BATTLE_POWER_HANDLER(px_battle_heatproof);      /* 108c34f4, ability 85 */
PX_BATTLE_POWER_HANDLER(px_battle_dry_skin_power); /* 108c4244, ability 87 */
PX_BATTLE_POWER_HANDLER(px_battle_sheer_force);    /* 108c4708, ability 125 */
PX_BATTLE_POWER_HANDLER(px_battle_toxic_boost);    /* 108bfc9c, ability 137 */
PX_BATTLE_POWER_HANDLER(px_battle_flare_boost);    /* 108c00b8, ability 138 */
PX_BATTLE_POWER_HANDLER(px_battle_analytic);       /* 108bd200, ability 148 */
PX_BATTLE_POWER_HANDLER(px_battle_sand_force);     /* 108c1244, ability 159 */
PX_BATTLE_POWER_HANDLER(px_battle_dark_aura);      /* 108be18c, ability 186 */
PX_BATTLE_POWER_HANDLER(px_battle_fairy_aura);     /* 108bf060, ability 187 */
PX_BATTLE_POWER_HANDLER(px_battle_strong_jaw);     /* 108bf148, ability 173 */
PX_BATTLE_POWER_HANDLER(px_battle_tough_claws);    /* 108bf344, ability 181 */
PX_BATTLE_POWER_HANDLER(px_battle_aerilate);       /* 108c0a24, ability 184 */
PX_BATTLE_POWER_HANDLER(px_battle_pixilate);       /* 108c15a8, ability 182 */
PX_BATTLE_POWER_HANDLER(px_battle_refrigerate);    /* 108c15f8, ability 174 */
PX_BATTLE_POWER_HANDLER(px_battle_mega_launcher);  /* 108c32e4, ability 178 */

#endif
