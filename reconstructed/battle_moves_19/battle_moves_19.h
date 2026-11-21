#ifndef PX_BATTLE_MOVES_19_H
#define PX_BATTLE_MOVES_19_H

#include "../battle_ability/battle_ability.h"

/* DllBattle move handlers of event 0xac (the effect of the move itself), the
 * lower half of the last twenty-one (static analysis addresses; runtime
 * addresses are module-relative). Analyst names. Keys: 0x03 attacker (an
 * id), 0x06 target (an id), 0x53 (a value, here only Rest writes it).
 * Records (10869f7c/10867b48): kind 3 message at +4; kind 4 damage (+4
 * halfword, +6 battler); kind 7 (+7 battler, +4 flag, word +0x14); kind 9 PP
 * (+4 amount, +5 battler, +6 slot, message at +8); kind 0xb condition (+4
 * halfword condition, +8 word turns, +0xc..+0x10 flags, +0xf battler,
 * message at +0x14); kind 0xd stat change (+4 stat, +0xc battler, +0x16,
 * +0x17, +0x18); kind 0xe stat copy (+4 battler, +5..+0xb seven stages,
 * +0xc flag byte); kind 0x13 (+4 halfword types, +6 battler); kind 0x15
 * (+4, +5); kind 0x17 and 0x18 (+4 = 9, +5 battler); kind 0x21 item
 * (+4 halfword, +6 battler, message at +0xc); kind 0x22 (+4, +5); kind
 * 0x25 (+4 target, messages at +8, +0x2c, +0x50); kind 0x26 (+4, +5 slot,
 * +6, +7, halfword +8); kind 0x27 counter reset (+4 battler, +5 counter,
 * +6 value). Record words at +0 carry flags (bit 24). Scratch buffer: 28
 * bytes (seven words) the stat copy uses. */
struct px_battle_moves_19_api {
    /* Event stack, battler lookup, pokemon_value (10874420: 0xd HP, 0xe max HP, 1-7 stat stages). */
    const struct px_battle_ability_api *base;
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id);    /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);                  /* 10867b48 */
    void (*message)(void *user, uint32_t at, uint32_t kind, uint32_t message);              /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t at, uint32_t value);                           /* 1084d29c */
    uint32_t (*counter)(void *user, uint32_t pokemon, uint32_t index);                      /* 1084bb2c */
    uint32_t (*has_type)(void *user, uint32_t pokemon, uint32_t type);                      /* 1086f960 */
    uint32_t (*hp_fraction)(void *user, uint32_t pokemon, uint32_t divisor);                /* 1084d2f4 */
    void (*set_effect_flag)(void *user, uint32_t context, uint32_t value);                  /* 1084bef8 */
    uint32_t (*full_hp)(void *user, uint32_t pokemon);                                      /* 1084d318 */
    uint32_t (*turn_word)(void *user, uint32_t turns);                                      /* 1084be3c */
    uint32_t (*trade_forbidden)(void *user, uint32_t context);                              /* 108544ec */
    uint32_t (*owner_of)(void *user, uint32_t id);                                          /* 1087425c */
    uint32_t (*item_locked)(void *user, uint32_t item);                                     /* 1084becc */
    uint32_t (*item_blocked)(void *user, uint32_t context, uint32_t form, uint32_t item);   /* 1085441c */
    uint32_t (*last_move)(void *user, uint32_t pokemon);                                    /* 1084c7fc */
    uint32_t (*last_used)(void *user, uint32_t pokemon);                                    /* 1084c278 */
    uint32_t (*move_slot)(void *user, uint32_t pokemon, uint32_t move);                     /* 1084c224; 4 none */
    uint32_t (*move_pp)(void *user, uint32_t pokemon, uint32_t slot);                       /* 108520e4 */
    uint32_t (*mimic_banned)(void *user, uint32_t pokemon, uint32_t move);                  /* 108c8548 */
    uint32_t (*mimic_blocked)(void *user, uint32_t pokemon);                                /* 10851c28 */
    uint32_t (*stage_copy_flag)(void *user, uint32_t pokemon, uint32_t kind);               /* 1084ca4c */
    uint32_t (*temp_buffer)(void *user, uint32_t context, uint32_t size);                   /* 1084c7d0 */
    uint32_t (*terrain)(void *user, uint32_t context);                                      /* 1084b81c */
    uint32_t (*environment)(void *user, uint32_t context);                                  /* 1084b8d4 */
    uint32_t (*type_word)(void *user, uint32_t pokemon);                                    /* 10870994 */
};

#define PX_BATTLE_MOVES_19_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_19_api *api)
PX_BATTLE_MOVES_19_HANDLER(px_battle_protect_streak_up);   /* 1084bd3c, Protect (182) and Detect (197) */
PX_BATTLE_MOVES_19_HANDLER(px_battle_curse_effect);        /* 108add30, Curse (174) */
PX_BATTLE_MOVES_19_HANDLER(px_battle_trick_swap);          /* 108adfb4, Trick (271) and Switcheroo (415) */
PX_BATTLE_MOVES_19_HANDLER(px_battle_spite_drain);         /* 108ae1a0, Spite (180) */
PX_BATTLE_MOVES_19_HANDLER(px_battle_rest_heal);           /* 108aeb70, Rest (156) */
PX_BATTLE_MOVES_19_HANDLER(px_battle_mimic_copy);          /* 108aeff8, Mimic (166) */
PX_BATTLE_MOVES_19_HANDLER(px_battle_bestow_give);         /* 108b0248, Bestow (516) */
PX_BATTLE_MOVES_19_HANDLER(px_battle_psych_up_copy);       /* 108b0468, Psych Up (244) */
PX_BATTLE_MOVES_19_HANDLER(px_battle_guard_swap);          /* 108b1898, Guard Swap (385) */
PX_BATTLE_MOVES_19_HANDLER(px_battle_heart_swap);          /* 108b1c7c, Heart Swap (391) */
PX_BATTLE_MOVES_19_HANDLER(px_battle_camouflage_type);     /* 108b1e4c, Camouflage (293) */

#endif
