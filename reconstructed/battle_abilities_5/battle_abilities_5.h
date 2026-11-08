#ifndef PX_BATTLE_ABILITIES_5_H
#define PX_BATTLE_ABILITIES_5_H

#include "../battle_immunity/battle_immunity.h"

/* DllBattle redirect, absorb and blocking abilities (static analysis
 * addresses; runtime addresses are module-relative). Analyst names. Keys:
 * 0x02 the battler concerned, 0x03 attacker, 0x04 target, 0x05 target count
 * with the targets from key 0x06 on, 0x12 move, 0x16 type, 0x21 drained HP,
 * 0x23 failure reason, 0x41 move blocked, 0x42 the effect is prevented,
 * 0x53 reflected. Types: 9 Fire, 10 Water, 11 Grass, 12 Electric.
 * The shared absorb test 10865824, the stat raise, reflection, move kind and
 * flag, ally, popup, message and action services come from battle_immunity. */
struct px_battle_abilities_5_api {
    const struct px_battle_immunity_api *immunity;
    uint32_t (*redirect_blocked)(void *user, uint32_t context, uint32_t attacker, uint32_t id, uint32_t move); /* 1084b67c */
    uint32_t (*fixed_target)(void *user, uint32_t move);                              /* 1084c720 */
    uint32_t (*ball_move)(void *user, uint32_t move);                                 /* 108a46c4 */
    uint32_t (*bounce_blocked)(void *user, uint32_t context, uint32_t id);            /* 1084af30 */
    uint32_t (*has_type)(void *user, uint32_t pokemon, uint32_t type);                /* 1086f960 */
    uint32_t (*battle_flag)(void *user, uint32_t pokemon, uint32_t flag);             /* 1084ca4c; 13 Flash Fire */
};

#define PX_BATTLE_ABILITIES_5_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_abilities_5_api *api)
PX_BATTLE_ABILITIES_5_HANDLER(px_battle_lightning_rod_redirect); /* 108be440, ability 31, event 0x2b */
PX_BATTLE_ABILITIES_5_HANDLER(px_battle_storm_drain_redirect);   /* 108bebf8, ability 114, event 0x2b */
PX_BATTLE_ABILITIES_5_HANDLER(px_battle_redirect_message);       /* 108c5270, abilities 31 and 114, event 0x24 */
PX_BATTLE_ABILITIES_5_HANDLER(px_battle_sap_sipper);             /* 108c5600, ability 157, event 0x30 */
PX_BATTLE_ABILITIES_5_HANDLER(px_battle_flash_fire_leave);       /* 108c20d4, ability 18, event 0x93 */
PX_BATTLE_ABILITIES_5_HANDLER(px_battle_flash_fire_power);       /* 108c33b0, ability 18, event 0x40 */
PX_BATTLE_ABILITIES_5_HANDLER(px_battle_bulletproof);            /* 108bcdbc, ability 171, event 0x2e */
PX_BATTLE_ABILITIES_5_HANDLER(px_battle_damp_check);             /* 108bea04, ability 6, event 0x1f */
PX_BATTLE_ABILITIES_5_HANDLER(px_battle_damp_message);           /* 108c4408, ability 6, event 0x21 */
PX_BATTLE_ABILITIES_5_HANDLER(px_battle_magic_bounce_reflect);   /* 108c4eb4, ability 156, event 0x1f */
PX_BATTLE_ABILITIES_5_HANDLER(px_battle_magic_bounce_message);   /* 108c4a88, ability 156, event 0x09 */
PX_BATTLE_ABILITIES_5_HANDLER(px_battle_sweet_veil);             /* 108c3d90, ability 175, event 0x0e */
PX_BATTLE_ABILITIES_5_HANDLER(px_battle_insomnia_block);         /* 108c4170, abilities 15 and 72, event 0x0e */
PX_BATTLE_ABILITIES_5_HANDLER(px_battle_flower_veil);            /* 108c573c, ability 166, event 0x0e */
PX_BATTLE_ABILITIES_5_HANDLER(px_battle_liquid_ooze);            /* 108bf18c, ability 64, event 0x97 */

#endif
