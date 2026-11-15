#ifndef PX_BATTLE_MOVES_12_H
#define PX_BATTLE_MOVES_12_H

#include "../battle_ability/battle_ability.h"
#include "../battle_handler/battle_handler.h"

/* DllBattle handlers of Sky Drop and of the lock releases around a move
 * (static analysis addresses; runtime addresses are module-relative).
 * Analyst names. Events: 0xa0 charging turn and 0xa3 release turn (key 0x03
 * attacker, 0x06 target, 0x40 the action data, 0x42 failure, 0x50, 0x53
 * announced), 0x21 after a move ends and 0x26 after it misses (key 0x02),
 * 0x23 on its success, 0x91/0xb6 when Sky Drop is interrupted (key 0x03).
 * Work words: +0, +4, +0x18 (word 6, the lock). Records (10869f7c/10867b48):
 * kind 3 message at +4; kind 6 damage to self (halfword +4, +6 battler,
 * message at +0xc); kind 0xa cure (+4 condition, +5 battler, +0x11, +0x12);
 * kind 0x27 counter reset (+4 battler, +5 counter, +6 value). Condition 0x19
 * is a move lock; counter 3 the protect streak, counter 4 Sky Drop's
 * carried battler + 1. */
struct px_battle_moves_12_api {
    /* Event stack and battler lookup. */
    const struct px_battle_ability_api *base;
    /* Registered handlers (Protect and Rollout remove their own). */
    const struct px_battle_handler_api *handlers;
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id);    /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);                  /* 10867b48 */
    void (*message)(void *user, uint32_t at, uint32_t kind, uint32_t message);              /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t at, uint32_t value);                           /* 1084d29c */
    uint32_t (*counter)(void *user, uint32_t pokemon, uint32_t index);                      /* 1084bb2c */
    uint32_t (*opposing_selector)(void *user, uint32_t context, uint32_t id);               /* 1084cb78; 0xa none */
    void (*sky_drop_land)(void *user, uint32_t context, uint32_t id, uint32_t zero, uint32_t zero2); /* 108683e4 */
    void (*sky_drop_release)(void *user, uint32_t context, uint32_t target);                /* 1084af8c */
    /* Lifts the target; 0 when it cannot (and may set the byte at `blocked`). Fifth argument on the stack. */
    uint32_t (*sky_drop_lift)(void *user, uint32_t context, uint32_t id, uint32_t target, uint32_t state,
                              uint32_t blocked);                                            /* 1081cb44 */
    uint32_t (*hp_fraction)(void *user, uint32_t pokemon, uint32_t divisor);                /* 1084d2f4 */
    /* The 20-byte global Sky Drop copies the action data to (literal 108b96a4, relocated; static 10b11be0). */
    uint32_t sky_drop_state;
    /* Logical byte standing in for the stack flag 1081cb44 may set. */
    uint32_t blocked_scratch;
};

#define PX_BATTLE_MOVES_12_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_12_api *api)
/* Sky Drop (507). */
PX_BATTLE_MOVES_12_HANDLER(px_battle_sky_drop_lift);      /* 108b95a0, event 0xa0 */
PX_BATTLE_MOVES_12_HANDLER(px_battle_sky_drop_drop);      /* 108ba8b4, event 0xa3 */
PX_BATTLE_MOVES_12_HANDLER(px_battle_sky_drop_missed);    /* 108b9558, event 0x26 */
PX_BATTLE_MOVES_12_HANDLER(px_battle_sky_drop_cancelled); /* 108ba140, events 0x91 and 0xb6 */
/* Event 0x21 (and 0x26 for Rollout). */
PX_BATTLE_MOVES_12_HANDLER(px_battle_bide_end);           /* 108b3618, move 117 */
PX_BATTLE_MOVES_12_HANDLER(px_battle_protect_end);        /* 108b7610, moves 182, 197, 203, 561, 588, 596 */
PX_BATTLE_MOVES_12_HANDLER(px_battle_rollout_end);        /* 108b8df8, moves 205 and 301 (events 0x21 and 0x26) */
/* Event 0x26. */
PX_BATTLE_MOVES_12_HANDLER(px_battle_jump_kick_crash);    /* 108b9138, moves 26 and 136 (Jump Kick, High Jump Kick) */
/* Event 0x23. */
PX_BATTLE_MOVES_12_HANDLER(px_battle_heal_bell_done);     /* 108b7adc, move 215 */
PX_BATTLE_MOVES_12_HANDLER(px_battle_aromatherapy_done);  /* 108b80d8, move 312 */
PX_BATTLE_MOVES_12_HANDLER(px_battle_pledge_done);        /* 108b8110, moves 518, 519, 520 */

#endif
