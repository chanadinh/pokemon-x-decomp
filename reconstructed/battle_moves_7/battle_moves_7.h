#ifndef PX_BATTLE_MOVES_7_H
#define PX_BATTLE_MOVES_7_H

#include "../battle_ability/battle_ability.h"

/* DllBattle move handlers of event 0x8c, which runs after a move has hit
 * (static analysis addresses; runtime addresses are module-relative).
 * Analyst names. Keys: 0x03 attacker, 0x05 number of targets hit, 0x06 + i
 * their ids (0x1f none). Each handler requires key 0x03 to be its battler
 * and commits action records (10869f7c / 10867b48):
 *   kind 0xa cure: +4 status, +5 battler, +0x11 flag, +0x12 flag;
 *   kind 0xb add condition: halfword +4 condition, word +8 value, +0xf
 *        battler, message at +0x14;
 *   kind 0xd stat change: +4 stat, +0xc battler, +0x16 count, +0x17 stages,
 *        +0x18 flags;
 *   kind 0x21 item removed: halfword +4, +6 battler, message at +0xc;
 *   kind 0x23 item used: +4 battler, +5 flag, halfword +6 item, flag 0x1000000;
 *   kind 0x25 item taken: +4 target, message at +8;
 *   kind 0x2f forced switch: halfword +4 message, +6 target;
 *   kind 0x37 state cleared: +4 battler, +5 state, message at +8;
 *   kind 0x3b: +4 value, flag 0x1000000.
 * Statuses (10856a88): 1 paralysis, 2 sleep, 3 freeze, 0x16 trapped by
 * Thousand Waves, 0x1e Magnet Rise, 0x20 Telekinesis, 0x21 grounded.
 * Battlers: held item +0x12. Secret Power's entry of this event (108b741c)
 * is not part of this milestone. */
struct px_battle_moves_7_api {
    /* Event stack and battler lookup. */
    const struct px_battle_ability_api *base;
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id);    /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);                  /* 10867b48 */
    void (*message)(void *user, uint32_t at, uint32_t kind, uint32_t message);              /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t at, uint32_t value);                           /* 1084d29c */
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);                  /* 10856a88 */
    uint32_t (*fainted)(void *user, uint32_t pokemon);                                      /* 1085f844 */
    uint32_t (*item_is_berry)(void *user, uint32_t item);                                   /* 108007a4 -> 0039f0bc */
    uint32_t (*item_locked)(void *user, uint32_t context, uint32_t id, uint32_t target);    /* 1084bb5c */
    uint32_t (*charging)(void *user, uint32_t context, uint32_t id);                        /* 1084c030 */
    uint32_t (*airborne)(void *user, uint32_t context, uint32_t id);                        /* 1084bf24 */
    uint32_t (*trap_value)(void *user, uint32_t id);                                        /* 1089e638 */
};

/* 108b65f8: grounds one target (Smack Down, Thousand Arrows); 1 when it was
 * airborne and is now grounded. */
uint32_t px_battle_smack_down_target(uint32_t context, uint32_t id, uint32_t target, const struct px_battle_moves_7_api *api);

#define PX_BATTLE_MOVES_7_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_7_api *api)
PX_BATTLE_MOVES_7_HANDLER(px_battle_scald_thaw);       /* 108aecac, moves 503 and 592 (Steam Eruption) */
PX_BATTLE_MOVES_7_HANDLER(px_battle_thief_steal);      /* 108af56c, moves 168 and 343 (Covet) */
PX_BATTLE_MOVES_7_HANDLER(px_battle_pluck_eat);        /* 108afd74, moves 365 and 450 (Bug Bite) */
PX_BATTLE_MOVES_7_HANDLER(px_battle_smack_down_hit);   /* 108b11a4, move 479 */
PX_BATTLE_MOVES_7_HANDLER(px_battle_thousand_arrows_hit); /* 108b6438, move 614 */
PX_BATTLE_MOVES_7_HANDLER(px_battle_thousand_waves_hit);  /* 108b6528, move 615 */
PX_BATTLE_MOVES_7_HANDLER(px_battle_circle_throw_hit); /* 108b3088, moves 509 and 525 (Dragon Tail) */
PX_BATTLE_MOVES_7_HANDLER(px_battle_fell_stinger_hit); /* 108b45bc, move 565 */
PX_BATTLE_MOVES_7_HANDLER(px_battle_knock_off_hit);    /* 108b9700, move 282 */
PX_BATTLE_MOVES_7_HANDLER(px_battle_smelling_salts_cure); /* 108b97fc, move 265 */
PX_BATTLE_MOVES_7_HANDLER(px_battle_wake_up_slap_cure);   /* 108bbd78, move 358 */

#endif
