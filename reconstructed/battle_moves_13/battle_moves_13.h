#ifndef PX_BATTLE_MOVES_13_H
#define PX_BATTLE_MOVES_13_H

#include "../battle_ability/battle_ability.h"

/* DllBattle handlers of the protection-breaking moves and the rest of event
 * 0x4d (static analysis addresses; runtime addresses are module-relative).
 * Analyst names. Events: 0x4d after a move's damage, 0x8d on the second turn
 * of Shadow Force and Phantom Force, 0x33, 0x32 and 0x2c around their
 * execution. Keys: 0x03 attacker, 0x04 defender, 0x06 target (ids), 0x53
 * (value). Work word 6 (+0x18): Uproar has started.
 * Side conditions: a global of 0x110 bytes per side, one 16-byte entry per
 * condition (word +0 set while active; 0 Reflect and 1 Light Screen are the
 * ones Brick Break tests). Records (10869f7c/10867b48): kind 3 message at +4;
 * kind 0xa cure (+4 status, +5 battler, +0x11, message at +0x14); kind 0xb
 * condition (+4 halfword condition, +8 word count, +0xc, +0xf battler);
 * kind 0x12 (+4 battler); kinds 0x1a and 0x1b side conditions (+8 side,
 * +9 flag of 0x1b, +4 mask size 4 then mask bytes +5..+7, bit i of byte
 * 1 + i / 8 for condition i); kind 0x3b (+4). Status 2 is sleep, 0x19 the
 * Uproar lock. */
struct px_battle_moves_13_api {
    /* Event stack, battler lookup, pokemon_value (10874420; 0xf level) and side_battlers (1084bc20). */
    const struct px_battle_ability_api *base;
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id);    /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);                  /* 10867b48 */
    void (*message)(void *user, uint32_t at, uint32_t kind, uint32_t message);              /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t at, uint32_t value);                           /* 1084d29c */
    uint32_t (*side_of)(void *user, uint32_t id);                                           /* 1084bfc0 */
    uint32_t (*opposing_side)(void *user, uint32_t side);                                   /* 1084af00 */
    uint32_t (*side_condition)(void *user, uint32_t context, uint32_t side, uint32_t index); /* 108acedc; entry word != 0 */
    uint32_t (*protection_condition)(void *user, uint32_t index);                           /* 1084b3e8 (table test via 1086fe6c) */
    uint32_t (*turn_flag)(void *user, uint32_t pokemon, uint32_t flag);                     /* 1084c818; 7 protected */
    /* Ends the target's protection, with the message given in r3. */
    void (*break_protection)(void *user, uint32_t context, uint32_t id, uint32_t pokemon,
                             uint32_t message);                                             /* 1084b0ec */
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);                  /* 10856a88 */
    uint32_t (*lock_count)(void *user, uint32_t turns, uint32_t move);                      /* 1084beac */
    uint32_t (*opposing_selector)(void *user, uint32_t context, uint32_t id);               /* 1084cb78 */
    uint32_t (*temp_buffer)(void *user, uint32_t context, uint32_t size);                   /* 1084c7d0 */
    /* Adds coins to the prize; 0 when the battle keeps none. */
    uint32_t (*scatter_money)(void *user, uint32_t context, uint32_t amount, uint32_t id);  /* 1081ca98 */
    /* The side-condition global (literal 108bbb48, relocated). */
    uint32_t side_conditions;
};

#define PX_BATTLE_MOVES_13_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_13_api *api)
/* Feint (364), Shadow Force (467), Phantom Force (566), Hyperspace Fury (593). */
PX_BATTLE_MOVES_13_HANDLER(px_battle_feint_break);           /* 108b8a70, event 0x4d, Feint */
PX_BATTLE_MOVES_13_HANDLER(px_battle_hyperspace_fury_break); /* 108bb43c, event 0x4d, Hyperspace Fury */
PX_BATTLE_MOVES_13_HANDLER(px_battle_shadow_force_break);    /* 108bb688, event 0x8d, Shadow Force, Phantom Force */
PX_BATTLE_MOVES_13_HANDLER(px_battle_feint_bypass);          /* 108b8b58, event 0x33, all four */
PX_BATTLE_MOVES_13_HANDLER(px_battle_feint_unprotect);       /* 108ba084, event 0x32, all four */
PX_BATTLE_MOVES_13_HANDLER(px_battle_feint_unprotect_early); /* 108bb138, event 0x2c, all four */
/* Event 0x4d. */
PX_BATTLE_MOVES_13_HANDLER(px_battle_brick_break);           /* 108bba4c, move 280 */
PX_BATTLE_MOVES_13_HANDLER(px_battle_uproar_start);          /* 108aee4c, move 253 */
PX_BATTLE_MOVES_13_HANDLER(px_battle_pay_day);               /* 108b50e8, move 6 */
PX_BATTLE_MOVES_13_HANDLER(px_battle_explosion_faint);       /* 108bbd34, moves 120 and 153 */

#endif
