#ifndef PX_BATTLE_MOVES_14_H
#define PX_BATTLE_MOVES_14_H

#include "../battle_ability/battle_ability.h"
#include "../battle_handler/battle_handler.h"

/* DllBattle move handlers of the lowest-numbered events, 0x01-0x0e (static
 * analysis addresses; runtime addresses are module-relative). Analyst names.
 * Keys: 0x02 battler, 0x03 attacker (ids); 0x0d target (a value, rewritten), 0x0c the action
 * kind (8 does not end the effects tested), 0x12 move, 0x42 failure, 0x53
 * (values). Work words: +0 (Charge's state 2, Thrash's turns left, Snatch's
 * flag), +0x18 (word 6: Rage active, Uproar started).
 * Records (10869f7c/10867b48): kind 3 message at +4; kind 0xa cure (+4
 * condition, +5 battler, +0x11); kind 0xb condition (+4 halfword condition,
 * +8 data, +0xd, +0xf battler, message at +0x14); kind 0x1f delayed attack
 * (+4 = 3, +5 target, +8 word turn + 2, +0xc word move, +0x10 word side,
 * +0x18 = 3); kind 0x27 counter reset (+4 battler, +5 counter, +6 value).
 * Condition 0x19 is a move lock, 6 confusion; counter 3 the protect streak. */
struct px_battle_moves_14_api {
    /* Event stack, battler lookup and side_of (10873158). */
    const struct px_battle_ability_api *base;
    /* Registered handlers (find 1084d3c0, next 1084d370, remove 1084d1cc). */
    const struct px_battle_handler_api *handlers;
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id);    /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);                  /* 10867b48 */
    void (*message)(void *user, uint32_t at, uint32_t kind, uint32_t message);              /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t at, uint32_t value);                           /* 1084d29c */
    uint32_t (*last_move)(void *user, uint32_t pokemon);                                    /* 1084c7fc */
    void (*condition_for)(void *user, uint32_t status, uint32_t pokemon, uint32_t out);     /* 1084c438; word at out */
    uint32_t (*battle_mode)(void *user, uint32_t context);                                  /* 1084bf14 */
    uint32_t (*target_selector)(void *user, uint32_t mode, uint32_t side, uint32_t zero);   /* 1086de1c */
    uint32_t (*turn)(void *user, uint32_t context);                                         /* 1084d364 */
    uint32_t (*reflection_blocked)(void *user, uint32_t pokemon);                           /* 1084b804 */
    /* The eight protect-type moves, halfwords (literal 108b7e4c, relocated; static 108d4b28). */
    uint32_t protect_moves;
};

#define PX_BATTLE_MOVES_14_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_14_api *api)
/* Event 0x01. */
PX_BATTLE_MOVES_14_HANDLER(px_battle_rage_end);            /* 108b6d8c, move 99 */
PX_BATTLE_MOVES_14_HANDLER(px_battle_destiny_bond_end);    /* 108b9ab8, moves 194 and 288 (Destiny Bond, Grudge) */
/* Event 0x03. */
PX_BATTLE_MOVES_14_HANDLER(px_battle_charge_check);        /* 108b7b50, move 268 */
PX_BATTLE_MOVES_14_HANDLER(px_battle_protect_streak);      /* 108b7db8, moves 182, 197, 203, 561, 588, 596 */
/* Event 0x04. */
PX_BATTLE_MOVES_14_HANDLER(px_battle_charge_end);          /* 108b6e48, move 268 */
PX_BATTLE_MOVES_14_HANDLER(px_battle_uproar_interrupted);  /* 108b7058, move 253 */
PX_BATTLE_MOVES_14_HANDLER(px_battle_thrash_turn);         /* 108b7240, moves 37, 80, 200 */
PX_BATTLE_MOVES_14_HANDLER(px_battle_rollout_interrupted); /* 108b7c58, moves 205 and 301 */
/* Event 0x06. */
PX_BATTLE_MOVES_14_HANDLER(px_battle_future_sight_set);    /* 108b27d4, move 248 */
PX_BATTLE_MOVES_14_HANDLER(px_battle_doom_desire_set);     /* 108b6ca4, move 353 */
/* Event 0x07. */
PX_BATTLE_MOVES_14_HANDLER(px_battle_future_sight_message); /* 108b8418, move 248 */
PX_BATTLE_MOVES_14_HANDLER(px_battle_doom_desire_message);  /* 108baabc, move 353 */
/* Events 0x08, 0x09, 0x0d, 0x0e. */
PX_BATTLE_MOVES_14_HANDLER(px_battle_snatch_taken);        /* 108b6494, move 289, event 0x08 */
PX_BATTLE_MOVES_14_HANDLER(px_battle_magic_coat_reflect);  /* 108b8ec0, move 277, event 0x09 */
PX_BATTLE_MOVES_14_HANDLER(px_battle_teleport);            /* 108b7958, move 100, event 0x0d */
PX_BATTLE_MOVES_14_HANDLER(px_battle_uproar_fail);         /* 108ba778, move 253, event 0x0e */

#endif
