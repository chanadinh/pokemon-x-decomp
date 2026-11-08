#ifndef PX_BATTLE_MOVES_9_H
#define PX_BATTLE_MOVES_9_H

#include "../battle_ability/battle_ability.h"
#include "../battle_handler/battle_handler.h"

/* DllBattle move handlers of event 0x24, which runs after a battler has used
 * its move (static analysis addresses; runtime addresses are module-relative),
 * Secret Power's handler of event 0x8c and its environment routine 108bb8ac.
 * Analyst names. Event 0x24 keys: 0x02 the battler, 0x12 the move used,
 * 0x13 a follow-up move, 0x3f the move's lock state. Handler +0x38 holds the
 * move id; work word 0 +0, word 1 +4, word 6 +0x18. Records (10869f7c /
 * 10867b48): kind 3 message at +4; kind 0xa cure (+4 condition, +5 battler,
 * +0x11, +0x12 flags); kind 0xb add condition (halfword +4, word +8 value,
 * +0xc, +0xf battler); kind 0xd stat change; kind 0x17 state (+4, +5
 * battler); kind 0x2c flinch (+4 battler, +5 chance); kind 0x31 move used
 * (halfword +4). Condition 0x19 locks the battler into its move. */
struct px_battle_moves_9_api {
    /* Event stack, battler lookup and side_of (10873158). */
    const struct px_battle_ability_api *base;
    /* Registered handlers (Rage removes its own). */
    const struct px_battle_handler_api *handlers;
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id);    /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);                  /* 10867b48 */
    void (*message)(void *user, uint32_t at, uint32_t kind, uint32_t message);              /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t at, uint32_t value);                           /* 1084d29c */
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);                  /* 10856a88; 2 sleep */
    uint32_t (*has_type)(void *user, uint32_t pokemon, uint32_t type);                      /* 1086f960; 3 Poison */
    uint32_t (*random)(void *user, uint32_t range);                                         /* 1084c7bc */
    uint32_t (*turn)(void *user, uint32_t context);                                         /* 1084d364 */
    uint32_t (*move_users)(void *user, uint32_t history, uint32_t move, uint32_t turn);     /* 108c9ac8 */
    uint32_t (*lock_count)(void *user, uint32_t turns, uint32_t move);                      /* 1084beac */
    uint32_t (*move_count)(void *user, uint32_t move);                                      /* 1084c3b0 */
    void (*set_effect_flag)(void *user, uint32_t context, uint32_t value);                  /* 1084bef8 */
    uint32_t (*environment)(void *user, uint32_t context);                                  /* 1084b81c */
    uint32_t (*terrain)(void *user, uint32_t context);                                      /* 1084b8d4 */
    uint32_t (*effect_roll)(void *user, uint32_t context, uint32_t id, uint32_t target, uint32_t chance); /* 108261e8 */
    void (*status_value)(void *user, uint32_t status, uint32_t pokemon, uint32_t out);      /* 1084c438 */
    void (*explode)(void *user, uint32_t context, uint32_t side, uint32_t other, uint32_t move,
                    uint32_t zero, uint32_t zero2);                                         /* 1081cf14 (two stack arguments) */
    /* Logical 8 bytes standing in for Secret Power's two stack result bytes (+0, +4). */
    uint32_t effect_scratch;
};

/* 108bb8ac: Secret Power's effect for the environment and terrain. Stores
 * the effect class at `out_class` and its parameter at `out_value` (when
 * nonzero) and returns the animation. */
uint32_t px_battle_secret_power_effect(uint32_t context, uint32_t out_class, uint32_t out_value,
                                       const struct px_battle_moves_9_api *api);

#define PX_BATTLE_MOVES_9_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_9_api *api)
/* Event 0x24. */
PX_BATTLE_MOVES_9_HANDLER(px_battle_bide_use);          /* 108adbe8, move 117 */
PX_BATTLE_MOVES_9_HANDLER(px_battle_thrash_use);        /* 108af3e0, moves 37, 80, 200 (Thrash, Petal Dance, Outrage) */
PX_BATTLE_MOVES_9_HANDLER(px_battle_rollout_use);       /* 108b7b94, moves 205 and 301 (Ice Ball) */
PX_BATTLE_MOVES_9_HANDLER(px_battle_rage_use);          /* 108b1f70, move 99 */
PX_BATTLE_MOVES_9_HANDLER(px_battle_charge_use);        /* 108b2168, move 268 */
PX_BATTLE_MOVES_9_HANDLER(px_battle_round_use);         /* 108afbcc, move 496 */
PX_BATTLE_MOVES_9_HANDLER(px_battle_toxic_use);         /* 108b0194, move 92 */
PX_BATTLE_MOVES_9_HANDLER(px_battle_defense_curl_use);  /* 108b39e8, move 111 */
PX_BATTLE_MOVES_9_HANDLER(px_battle_minimize_use);      /* 108b55d4, move 107 */
PX_BATTLE_MOVES_9_HANDLER(px_battle_magnitude_use);     /* 108b836c, move 222 */
PX_BATTLE_MOVES_9_HANDLER(px_battle_pledge_use);        /* 108b9f9c, moves 518, 519, 520 */
PX_BATTLE_MOVES_9_HANDLER(px_battle_secret_power_use);  /* 108bab20, move 290 */
PX_BATTLE_MOVES_9_HANDLER(px_battle_explosion_use);     /* 108bb0c8, moves 120 and 153 (Selfdestruct, Explosion) */
/* Event 0x8c. */
PX_BATTLE_MOVES_9_HANDLER(px_battle_secret_power_hit);  /* 108b741c, move 290 */

#endif
