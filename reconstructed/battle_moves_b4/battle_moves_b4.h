#ifndef PX_BATTLE_MOVES_B4_H
#define PX_BATTLE_MOVES_B4_H

#include "../battle_ability/battle_ability.h"

/* DllBattle move handlers of event 0xac, the effect of a status move, third
 * group: 10 REA-listed ones (static analysis addresses; runtime addresses
 * are module-relative). Analyst names. Every handler acts for the user (key
 * 0x03); key 0x06 is the first target. Records (10869f7c/10867b48): kind 3
 * message at +4; kind 7 HP change (+4 mode, +6, +7 battler, +8 target, words
 * +0x14 and +0x18); kind 0xb condition (+4 halfword, +8 word, +0xc, +0xf
 * battler, message at +0x14); kind 0xd stat stage (+4 stat, +0xc battler,
 * +0x16 count, +0x17 stages, +0x18 flags); kind 0xe (+4 battler, +5..+0xb
 * seven bytes, +0xc); kind 0x10 (+4, +6, +8, +0xa halfwords, +0xe battler, +0xf
 * flags, message at +0x10); kind 0x12 (+4 battler); kind 0x13 types (+4
 * packed halfword, +6 battler); kind 0x22 (+4 battler, +5); kind 0x26 (+4
 * battler, +5 slot, +8 move). */
struct px_battle_moves_b4_api {
    /* Event stack, battler lookup and pokemon_value (10874420). */
    const struct px_battle_ability_api *base;
    uint32_t (*last_move)(void *user, uint32_t pokemon);                                 /* 1084c278 */
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);               /* 10856a88; 0x17 Encore */
    uint32_t (*encore_blocked)(void *user, uint32_t move);                               /* 108a4804 */
    uint32_t (*has_move)(void *user, uint32_t pokemon, uint32_t move);                   /* 10869eb8 */
    uint32_t (*turn_flag)(void *user, uint32_t pokemon, uint32_t flag);                  /* 1084c818 */
    uint32_t (*condition_word)(void *user, uint32_t a, uint32_t b);                      /* 1084beac */
    uint32_t (*transformed)(void *user, uint32_t pokemon);                               /* 10851c28 */
    uint32_t (*copy_source)(void *user, uint32_t pokemon);                               /* 1084c7fc */
    uint32_t (*mimic_blocked)(void *user, uint32_t move);                                /* 108a4844 */
    uint32_t (*move_slot)(void *user, uint32_t pokemon, uint32_t move);                  /* 1084c224; 4 none */
    uint32_t (*hp_share)(void *user, uint32_t pokemon, uint32_t divisor);                /* 1084d2f4 */
    uint32_t (*stage_room)(void *user, uint32_t pokemon, uint32_t stat);                 /* 1084b82c */
    uint32_t (*context_word)(void *user, uint32_t context);                              /* 1081d83c */
    uint32_t (*raw_stat)(void *user, uint32_t pokemon, uint32_t key);                    /* 108743a8 */
    uint32_t (*temp_buffer)(void *user, uint32_t context, uint32_t size);                /* 1084c7d0 */
    uint32_t (*resist_types)(void *user, uint32_t type, uint32_t out);                   /* 1089e5ac; count */
    uint32_t (*has_type)(void *user, uint32_t pokemon, uint32_t type);                   /* 1086f960 */
    uint32_t (*random)(void *user, uint32_t count);                                      /* 1084c7bc */
    void (*move_bytes)(void *user, uint32_t destination, uint32_t source, uint32_t count); /* 1080042c */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);           /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                   /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);               /* 10867b48 */
};

#define PX_BATTLE_MOVES_B4_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_b4_api *api)
PX_BATTLE_MOVES_B4_HANDLER(px_battle_encore_effect);         /* 108ae618, Encore (227) */
PX_BATTLE_MOVES_B4_HANDLER(px_battle_mimic_effect);          /* 108b0a10, Mimic (102) */
PX_BATTLE_MOVES_B4_HANDLER(px_battle_conversion_2_effect);   /* 108b0fdc, Conversion 2 (176) */
PX_BATTLE_MOVES_B4_HANDLER(px_battle_belly_drum_effect);     /* 108b1b44, Belly Drum (187) */
PX_BATTLE_MOVES_B4_HANDLER(px_battle_pain_split_effect);     /* 108b2038, Pain Split (220) */
PX_BATTLE_MOVES_B4_HANDLER(px_battle_memento_effect);        /* 108b2a4c, Memento (262) */
PX_BATTLE_MOVES_B4_HANDLER(px_battle_guard_split_effect);    /* 108b3708, Guard Split (470) */
PX_BATTLE_MOVES_B4_HANDLER(px_battle_power_split_effect);    /* 108b3f00, Power Split (471) */
PX_BATTLE_MOVES_B4_HANDLER(px_battle_topsy_turvy_effect);    /* 108b5a48, Topsy-Turvy (576) */
PX_BATTLE_MOVES_B4_HANDLER(px_battle_shell_smash_effect);    /* 108b5cc4, Shell Smash (504) */

#endif
