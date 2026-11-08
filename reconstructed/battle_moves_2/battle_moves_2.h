#ifndef PX_BATTLE_MOVES_2_H
#define PX_BATTLE_MOVES_2_H

#include "../battle_ability/battle_ability.h"

/* DllBattle move handlers that feed the damage calculation (static analysis
 * addresses; runtime addresses are module-relative). Analyst names. Events:
 * 0x3c base power (key 0x31), 0x3d power multiplier (key 0x32), 0x3e attack
 * source (key 0x3c, the battler whose attack is used), 0x3f defense
 * selection (keys 0x3d, 0x3e; key 0x53 ignores stat stages), 0x40 attack
 * multiplier (key 0x54 burn exception). Key 0x03 is the attacker, 0x04 the
 * defender, 0x16 the move type (12 Electric). Handler +0x38 holds the move id.
 * Statuses: 1 paralysis, 4 burn, 5 poison. Weather: 2 rain, 3 hail, 4 sand. */
struct px_battle_moves_2_api {
    /* Event stack, battler lookup, pokemon_value and pokemon_status (1086d8ac). */
    const struct px_battle_ability_api *base;
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);              /* 10856a88 */
    uint32_t (*hp_percent)(void *user, uint32_t pokemon);                               /* 108713a0; 4096 = 1% */
    uint32_t (*weather)(void *user, uint32_t context);                                  /* 1084be58 */
    uint32_t (*move_power)(void *user, uint32_t move);                                  /* 10801634 -> 0039e044 */
    uint32_t (*turn)(void *user, uint32_t context);                                     /* 1084d364 */
    uint32_t (*turn_last_move)(void *user, uint32_t history, uint32_t turn);            /* 108c9b28 */
    void (*set_effect_flag)(void *user, uint32_t context, uint32_t value);              /* 1084bef8 */
    uint32_t (*faint_record)(void *user, uint32_t context);                             /* 1081cc04 */
    uint32_t (*faint_count)(void *user, uint32_t record, uint32_t kind);                /* 108c99fc */
    uint32_t (*faint_id)(void *user, uint32_t record, uint32_t kind, uint32_t index);   /* 108c9a30 */
    uint32_t (*is_ally)(void *user, uint32_t id, uint32_t other);                       /* 1084c5a8 */
    uint32_t (*item_locked)(void *user, uint32_t context, uint32_t id, uint32_t target);/* 1084bb5c */
};

#define PX_BATTLE_MOVES_2_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_2_api *api)
/* Event 0x3c. */
PX_BATTLE_MOVES_2_HANDLER(px_battle_eruption);         /* 108adae8, moves 284 and 323 (Water Spout) */
/* Event 0x3d. */
PX_BATTLE_MOVES_2_HANDLER(px_battle_brine);            /* 108afc9c, move 362 */
PX_BATTLE_MOVES_2_HANDLER(px_battle_charge_power);     /* 108b2200, move 268 (Charge) */
PX_BATTLE_MOVES_2_HANDLER(px_battle_retaliate);        /* 108b2248, move 514 */
PX_BATTLE_MOVES_2_HANDLER(px_battle_venoshock);        /* 108b3548, move 474 */
PX_BATTLE_MOVES_2_HANDLER(px_battle_fusion);           /* 108b682c, moves 558 and 559 (Fusion Flare, Fusion Bolt) */
PX_BATTLE_MOVES_2_HANDLER(px_battle_me_first_power);   /* 108b7850, move 382 (Me First) */
PX_BATTLE_MOVES_2_HANDLER(px_battle_solar_beam_power); /* 108b8048, move 76 (Solar Beam) */
PX_BATTLE_MOVES_2_HANDLER(px_battle_helping_hand_power);/* 108b86e4, move 270 (Helping Hand) */
PX_BATTLE_MOVES_2_HANDLER(px_battle_facade_power);     /* 108b8d7c, move 263 (Facade) */
PX_BATTLE_MOVES_2_HANDLER(px_battle_knock_off_power);  /* 108ba1dc, move 282 (Knock Off) */
/* Event 0x3e. */
PX_BATTLE_MOVES_2_HANDLER(px_battle_foul_play);        /* 108af710, move 492 */
/* Event 0x3f. */
PX_BATTLE_MOVES_2_HANDLER(px_battle_psyshock);         /* 108b420c, moves 473, 540 (Psystrike), 548 (Secret Sword) */
PX_BATTLE_MOVES_2_HANDLER(px_battle_chip_away);        /* 108b9b44, moves 498 and 533 (Sacred Sword) */
/* Event 0x40. */
PX_BATTLE_MOVES_2_HANDLER(px_battle_facade_burn);      /* 108b8340, move 263 (Facade) */

#endif
