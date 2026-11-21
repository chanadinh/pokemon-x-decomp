#ifndef PX_BATTLE_MOVES_B3_H
#define PX_BATTLE_MOVES_B3_H

#include "../battle_items_4/battle_items_4.h"

/* DllBattle move handlers of event 0xac, the effect of a status move, second
 * group: 12 REA-listed ones (static analysis addresses; runtime addresses are
 * module-relative). Analyst names. Every handler acts for the user (key
 * 0x03); key 0x06 is the first target (id, low byte). Records
 * (10869f7c/10867b48): kind 3 message at +4; kind 0xb condition (+4 halfword
 * condition, +8 word, +0xf battler, message at +0x14); kind 0x10 (Psych Up);
 * kind 0x12 (+4 battler); kind 0x13 types (+4 packed halfword, +6 battler,
 * +7); kind 0x17 (+4, +5 battler); kind 0x1f (+4, +5 position); kind 0x20
 * (+4 ability, +6 battler, message at +0xc); kind 0x21 (+4 item, +6 battler,
 * +7, message at +0xc); kind 0x27 (+4 battler, +5, +6). Work word 6 (+0x18)
 * marks a move as done. The consumed mark (1084c700) comes from
 * battle_items_4. The first word of a record holds flag 0x1000000. */
struct px_battle_moves_b3_api {
    /* Event stack, battler lookup, pokemon_value (10874420) and side_of (10873158). */
    const struct px_battle_ability_api *base;
    uint32_t (*turn_flag)(void *user, uint32_t pokemon, uint32_t flag);                  /* 1084c818 */
    uint32_t (*battle_flag)(void *user, uint32_t pokemon, uint32_t flag);                /* 1084ca4c */
    uint32_t (*can_switch)(void *user, uint32_t context, uint32_t id);                   /* 1084ca78 */
    uint32_t (*condition_word)(void *user, uint32_t a, uint32_t b);                      /* 1084beac */
    uint32_t (*target_condition)(void *user, uint32_t id, uint32_t turns);               /* 108797b0 */
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);               /* 10856a88; 0xd Disable */
    uint32_t (*last_move)(void *user, uint32_t pokemon);                                 /* 1084c278 */
    uint32_t (*move_usable)(void *user, uint32_t pokemon, uint32_t move);                /* 108c8548 */
    uint32_t (*counter)(void *user, uint32_t pokemon, uint32_t index);                   /* 1084bb2c */
    uint32_t (*types)(void *user, uint32_t pokemon);                                     /* 10870994 */
    uint32_t (*raw_stat)(void *user, uint32_t pokemon, uint32_t key);                    /* 108743a8 */
    uint32_t (*ability_blocked)(void *user, uint32_t ability);                           /* 108a48e4 */
    uint32_t (*fainted)(void *user, uint32_t pokemon);                                   /* 1085f844 */
    uint32_t (*owner_of)(void *user, uint32_t id);                                       /* 1087425c */
    uint32_t (*owner_count)(void *user, uint32_t context, uint32_t owner);               /* 10874634 */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);           /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                   /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);               /* 10867b48 */
};

#define PX_BATTLE_MOVES_B3_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_b3_api *api)
PX_BATTLE_MOVES_B3_HANDLER(px_battle_mind_reader_effect);    /* 108ae9c8, Mind Reader (170), Lock-On (199) */
PX_BATTLE_MOVES_B3_HANDLER(px_battle_electrify_effect);      /* 108af13c, Electrify (582) */
PX_BATTLE_MOVES_B3_HANDLER(px_battle_endure_effect);         /* 108af7d8, Endure (203) */
PX_BATTLE_MOVES_B3_HANDLER(px_battle_recycle_effect);        /* 108af9f0, Recycle (278) */
PX_BATTLE_MOVES_B3_HANDLER(px_battle_focus_energy_effect);   /* 108b0718, Focus Energy (116) */
PX_BATTLE_MOVES_B3_HANDLER(px_battle_role_play_effect);      /* 108b0ba0, Role Play (272) */
PX_BATTLE_MOVES_B3_HANDLER(px_battle_disable_effect);        /* 108b38a4, Disable (50) */
PX_BATTLE_MOVES_B3_HANDLER(px_battle_reflect_type_effect);   /* 108b3b18, Reflect Type (513) */
PX_BATTLE_MOVES_B3_HANDLER(px_battle_psych_up_effect);       /* 108b402c, Psych Up (379) */
PX_BATTLE_MOVES_B3_HANDLER(px_battle_healing_wish_effect);   /* 108b5be0, Healing Wish (361) */
PX_BATTLE_MOVES_B3_HANDLER(px_battle_lunar_dance_effect);    /* 108b6f68, Lunar Dance (461) */
PX_BATTLE_MOVES_B3_HANDLER(px_battle_helping_hand_effect);   /* 108b787c, Helping Hand (270) */

#endif
