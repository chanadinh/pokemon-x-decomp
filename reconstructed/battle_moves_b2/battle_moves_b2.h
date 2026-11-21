#ifndef PX_BATTLE_MOVES_B2_H
#define PX_BATTLE_MOVES_B2_H

#include "../battle_items_1/battle_items_1.h"
#include "../battle_moves_5/battle_moves_5.h"

/* DllBattle move handlers of event 0xac, the effect of a status move, first
 * group: the 17 shortest REA-listed ones (static analysis addresses;
 * runtime addresses are module-relative). Analyst names. Every handler acts
 * for the user (key 0x03); key 0x05 is the target count and key 0x06 the
 * first target (ids). Records (10869f7c/10867b48): kind 3 message at +4;
 * kind 0xa cure (+4 status, +5 battler, +0x11 1); kind 0xb condition (+4
 * halfword condition, +8 word, +0xf battler, message at +0x14); kind 0x13
 * types (+4 packed t | t << 5 | 0x4800, +6 battler, +8); kind 0x14 added type
 * (+4 type, +5 battler); kind 0x15 (+4, +5 battler); kind 0x1c (Imprison);
 * kind 0x1f (+4, +5 position); kind 0x32 (+4 battler, message at +8); kind
 * 0x34 transform (+4 target, message at +8). 1084c948(context, kind, id)
 * commits a header-only record. Work word 6 (+0x18) marks a move as done.
 * The re-entry mark (1084c96c) comes from battle_items_1, the turn-count word
 * (1084be3c) from battle_moves_5. */
struct px_battle_moves_b2_api {
    /* Event stack, battler lookup, pokemon_status (1086d8ac) and side_of (10873158, the position). */
    const struct px_battle_ability_api *base;
    uint32_t (*battle_mode)(void *user, uint32_t context);                            /* 1084bf14 */
    void (*header_record)(void *user, uint32_t context, uint32_t kind, uint32_t id);  /* 1084c948 */
    void (*protect_start)(void *user, uint32_t context, uint32_t id);                 /* 1084bd48 */
    uint32_t (*moves_last)(void *user, uint32_t context, uint32_t id);                /* 1084b8e0 */
    uint32_t (*imprison_condition)(void *user, uint32_t id);                          /* 10876e20 */
    uint32_t (*has_type)(void *user, uint32_t pokemon, uint32_t type);                /* 1086f960 */
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);            /* 10856a88; 0xc Torment */
    uint32_t (*move_at)(void *user, uint32_t pokemon, uint32_t slot);                 /* 10855c68 */
    uint32_t (*move_type)(void *user, uint32_t move);                                 /* 1080043c */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);        /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);            /* 10867b48 */
};

#define PX_BATTLE_MOVES_B2_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_b2_api *api)
PX_BATTLE_MOVES_B2_HANDLER(px_battle_teleport_effect);       /* 108b0f9c, Teleport (100) */
PX_BATTLE_MOVES_B2_HANDLER(px_battle_hold_hands_effect);     /* 108b4524, Hold Hands (607) */
PX_BATTLE_MOVES_B2_HANDLER(px_battle_wish_effect);           /* 108b29a8, Wish (273) */
PX_BATTLE_MOVES_B2_HANDLER(px_battle_kings_shield_effect);   /* 108b5948, King's Shield (588) */
PX_BATTLE_MOVES_B2_HANDLER(px_battle_grudge_effect);         /* 108b5168, Grudge (288) */
PX_BATTLE_MOVES_B2_HANDLER(px_battle_follow_me_effect);      /* 108b987c, Follow Me (266), Rage Powder (476) */
PX_BATTLE_MOVES_B2_HANDLER(px_battle_quash_effect);          /* 108b2dc4, Quash (511) */
PX_BATTLE_MOVES_B2_HANDLER(px_battle_magic_coat_effect);     /* 108b26c4, Magic Coat (277) */
PX_BATTLE_MOVES_B2_HANDLER(px_battle_refresh_effect);        /* 108afa98, Refresh (287) */
PX_BATTLE_MOVES_B2_HANDLER(px_battle_forests_curse_effect);  /* 108b3974, Forest's Curse (571) */
PX_BATTLE_MOVES_B2_HANDLER(px_battle_transform_effect);      /* 108ae854, Transform (144) */
PX_BATTLE_MOVES_B2_HANDLER(px_battle_magnet_rise_effect);    /* 108b359c, Magnet Rise (393) */
PX_BATTLE_MOVES_B2_HANDLER(px_battle_snatch_effect);         /* 108b11e0, Snatch (289) */
PX_BATTLE_MOVES_B2_HANDLER(px_battle_imprison_effect);       /* 108adb64, Imprison (286) */
PX_BATTLE_MOVES_B2_HANDLER(px_battle_torment_effect);        /* 108af690, Torment (259) */
PX_BATTLE_MOVES_B2_HANDLER(px_battle_soak_effect);           /* 108b3be8, Soak (487) */
PX_BATTLE_MOVES_B2_HANDLER(px_battle_conversion_effect);     /* 108afcec, Conversion (160) */

#endif
