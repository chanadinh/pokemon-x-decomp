#ifndef PX_BATTLE_MOVES_1_H
#define PX_BATTLE_MOVES_1_H

#include "../battle_ability/battle_ability.h"
#include "../battle_handler/battle_handler.h"

/* DllBattle move handlers of the base-power event 0x3c (static analysis
 * addresses; runtime addresses are module-relative). Analyst names. This
 * milestone covers 33 of the 34 move entries of event 0x3c (Eruption and
 * Water Spout, 108adae8, remain). Every handler first requires key 0x03
 * (attacker) to be its battler id, then replaces key 0x31 (power) through
 * the event rewrite service. Key 0x04 is the defender; handler +0x38 holds
 * the move id. Battlers: core data pointer +0, held item +0x12, id +0x19,
 * state bits +0x169 (3 in the air, 7 Defense Curl). Values (10874420): 1..7
 * stat stages (6 neutral), 0xd HP, 0xe max HP, 0x14 base Attack. */
struct px_battle_moves_1_api {
    /* Event stack, battler lookup, pokemon_value and pokemon_status (1086d8ac). */
    const struct px_battle_ability_api *base;
    /* Registered handlers (Rollout removes its own). */
    const struct px_battle_handler_api *handlers;
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);              /* 10856a88; 1 paralysis, 2 sleep */
    uint32_t (*turn_flag)(void *user, uint32_t pokemon, uint32_t flag);                 /* 1084c818 */
    uint32_t (*friendship)(void *user, uint32_t core);                                  /* 10801cf4 -> 004a0030 */
    /* Copies damage record `index` of the battler to the 8 bytes at `out`
     * (+2 damage, +5 attacker id); 0 when there is none. */
    uint32_t (*damage_record)(void *user, uint32_t pokemon, uint32_t kind, uint32_t index, uint32_t out); /* 1084bac4 */
    uint32_t (*speed)(void *user, uint32_t context, uint32_t pokemon, uint32_t flag);   /* 108676cc */
    uint32_t (*weight)(void *user, uint32_t context, uint32_t id);                      /* 1084b9f0 */
    uint32_t (*move_slot)(void *user, uint32_t pokemon, uint32_t move);                 /* 1084c224; 4 none */
    uint32_t (*move_pp)(void *user, uint32_t pokemon, uint32_t slot);                   /* 108520e4 */
    uint32_t (*turn)(void *user, uint32_t context);                                     /* 1084d364 */
    uint32_t (*move_used)(void *user, uint32_t history, uint32_t move, uint32_t turn);  /* 108c9a64 */
    uint32_t (*move_users)(void *user, uint32_t history, uint32_t move, uint32_t turn); /* 108c9ac8 */
    uint32_t (*target_switching)(void *user, uint32_t context);                         /* 1084c020 */
    uint32_t (*hp_percent)(void *user, uint32_t pokemon);                               /* 108713a0 */
    uint32_t (*fixed_multiply)(void *user, uint32_t value, uint32_t factor);            /* 1089e5f8 */
    uint32_t (*random)(void *user, uint32_t range);                                     /* 1084c7bc */
    uint32_t (*last_move)(void *user, uint32_t pokemon);                                /* 1084c7fc */
    uint32_t (*use_streak)(void *user, uint32_t pokemon);                               /* 108c86c8 */
    uint32_t (*stockpile)(void *user, uint32_t pokemon, uint32_t index);                /* 1084bb2c */
    uint32_t (*weather)(void *user, uint32_t context);                                  /* 1084be58 */
    uint32_t (*party)(void *user, uint32_t context, uint32_t id);                       /* 1084b950 */
    uint32_t (*party_member)(void *user, uint32_t party, uint32_t index);               /* 1085daf8 */
    uint32_t (*can_battle)(void *user, uint32_t pokemon);                               /* 108551d4 */
    uint32_t (*item_param)(void *user, uint32_t item, uint32_t param);                  /* 1084cf74 */
    /* Logical 8 bytes standing in for Revenge's stack record. */
    uint32_t record_scratch;
};

#define PX_BATTLE_MOVES_1_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_1_api *api)
/* Doubled power. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_smelling_salts); /* 108ae96c, move 265 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_wake_up_slap);   /* 108b6f0c, move 358 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_hex);            /* 108b0ee8, move 506 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_assurance);      /* 108af4e4, move 372 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_payback);        /* 108b44c8, move 371 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_gust);           /* 108b0f40, moves 16 and 239 (Twister) */
PX_BATTLE_MOVES_1_HANDLER(px_battle_acrobatics);     /* 108af494, move 512 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_pursuit);        /* 108b2a04, move 228 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_weather_ball);   /* 108b8090, move 311 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_revenge);        /* 108afb0c, moves 279 and 419 (Avalanche) */
PX_BATTLE_MOVES_1_HANDLER(px_battle_round);          /* 108b53a0, move 496 */
/* Power from the battlers. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_return);         /* 108af994, move 216 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_frustration);    /* 108b32c4, move 218 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_punishment);     /* 108aedc0, move 386 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_stored_power);   /* 108b33d0, move 500 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_flail);          /* 108b05f4, moves 175 and 179 (Reversal) */
PX_BATTLE_MOVES_1_HANDLER(px_battle_wring_out);      /* 108b4250, moves 378 and 462 (Crush Grip) */
PX_BATTLE_MOVES_1_HANDLER(px_battle_gyro_ball);      /* 108b03d4, move 360 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_electro_ball);   /* 108b164c, move 486 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_low_kick);       /* 108b068c, moves 67 and 447 (Grass Knot) */
PX_BATTLE_MOVES_1_HANDLER(px_battle_heavy_slam);     /* 108b4d08, moves 484 and 535 (Heat Crash) */
PX_BATTLE_MOVES_1_HANDLER(px_battle_trump_card);     /* 108b07c0, move 376 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_spit_up);        /* 108b59a8, move 255 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_natural_gift);   /* 108b8f8c, move 363 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_fling);          /* 108bad24, move 374 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_beat_up);        /* 108b8200, move 251 */
/* Power from the turn history and work words. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_echoed_voice);   /* 108b150c, move 497 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_fury_cutter);    /* 108b5304, move 210 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_rollout);        /* 108b5ea8, moves 205 and 301 (Ice Ball) */
PX_BATTLE_MOVES_1_HANDLER(px_battle_triple_kick);    /* 108b47dc, move 167 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_magnitude);      /* 108b6ecc, move 222 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_present);        /* 108b52bc, move 217 */
PX_BATTLE_MOVES_1_HANDLER(px_battle_pledge);         /* 108b67b0, moves 518, 519 and 520 */

#endif
