#ifndef PX_BATTLE_MOVES_6_H
#define PX_BATTLE_MOVES_6_H

#include "../battle_ability/battle_ability.h"

/* DllBattle move handlers of two-turn moves (static analysis addresses;
 * runtime addresses are module-relative). Analyst names. Event 0xa0 runs on
 * the charging turn, 0xa1 for Skull Bash's charge, 0xa3 on the release turn.
 * Each charging handler requires key 0x03 (attacker) to be its battler and
 * commits action records (10869f7c / 10867b48):
 *   kind 0x17: +4 semi-invulnerable state (3 in the air, 4 underwater,
 *        5 underground, 6 vanished), +5 battler;
 *   kind 0x3: message (kind 2) at +4 naming the battler;
 *   kind 0xd: stat change, +4 stat (2 Defense), +0xc battler, +0x16 count,
 *        +0x17 stages, +0x18 flags (bits 1 and 2 set).
 * Sky Drop (507), which also has handlers on other events, is not part of
 * this milestone. */
struct px_battle_moves_6_api {
    /* Event stack. */
    const struct px_battle_ability_api *base;
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id);    /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);                  /* 10867b48 */
    void (*message)(void *user, uint32_t at, uint32_t kind, uint32_t message);              /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t at, uint32_t value);                           /* 1084d29c */
};

#define PX_BATTLE_MOVES_6_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_6_api *api)
/* Event 0xa0: semi-invulnerable charge (state record and message). */
PX_BATTLE_MOVES_6_HANDLER(px_battle_fly_charge);         /* 108baf70, move 19, in the air, message 0x211 */
PX_BATTLE_MOVES_6_HANDLER(px_battle_bounce_charge);      /* 108baffc, move 340, in the air, message 0x220 */
PX_BATTLE_MOVES_6_HANDLER(px_battle_dive_charge);        /* 108b8174, move 291, underwater, message 0x217 */
PX_BATTLE_MOVES_6_HANDLER(px_battle_dig_charge);         /* 108b9f10, move 91, underground, message 0x21a */
PX_BATTLE_MOVES_6_HANDLER(px_battle_shadow_force_charge);/* 108baee4, moves 467 and 566 (Phantom Force), vanished, message 0x21d */
/* Event 0xa0: charge message only. */
PX_BATTLE_MOVES_6_HANDLER(px_battle_razor_wind_charge);  /* 108ba2a0, move 13, message 0x223 */
PX_BATTLE_MOVES_6_HANDLER(px_battle_sky_attack_charge);  /* 108b8bd4, move 143, message 0x226 */
PX_BATTLE_MOVES_6_HANDLER(px_battle_solar_beam_charge);  /* 108ba794, move 76, message 0x229 */
PX_BATTLE_MOVES_6_HANDLER(px_battle_freeze_shock_charge);/* 108ba948, moves 553 (message 0x362) and 554 (Ice Burn, 0x365) */
PX_BATTLE_MOVES_6_HANDLER(px_battle_geomancy_charge);    /* 108ba9cc, move 601, message 0x4bd */
/* Event 0xa1. */
PX_BATTLE_MOVES_6_HANDLER(px_battle_skull_bash_charge);  /* 108bb5e8, move 130: message 0x22c and Defense +1 */
/* Event 0xa3: release turns with nothing to do (a lone bx lr each). */
PX_BATTLE_MOVES_6_HANDLER(px_battle_fly_release);        /* 108bb7d8, move 19 */
PX_BATTLE_MOVES_6_HANDLER(px_battle_bounce_release);     /* 108bb8a8, move 340 */
PX_BATTLE_MOVES_6_HANDLER(px_battle_dive_release);       /* 108b94e8, move 291 */
PX_BATTLE_MOVES_6_HANDLER(px_battle_dig_release);        /* 108bb0c4, move 91 */
PX_BATTLE_MOVES_6_HANDLER(px_battle_shadow_force_release);/* 108bb768, moves 467 and 566 */

#endif
