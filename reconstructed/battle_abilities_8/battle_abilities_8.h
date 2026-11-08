#ifndef PX_BATTLE_ABILITIES_8_H
#define PX_BATTLE_ABILITIES_8_H

#include "../battle_abilities_1/battle_abilities_1.h"

/* DllBattle item and after-move abilities (static analysis addresses;
 * runtime addresses are module-relative). Analyst names. Keys: 0x02 the
 * battler concerned, 0x03 attacker, 0x04 target, 0x05 target count with the
 * targets from key 0x06, 0x12 move, 0x14 the attacker's last move, 0x16 type,
 * 0x2e item, 0x47 suppressed, 0x52 item consumed, 0x42 prevented.
 * Battlers: species +0xc, held item +0x12. Fainted (1085f844) and the max-HP
 * share (1084d2f4) come from battle_abilities_1. */
struct px_battle_abilities_8_api {
    /* Event stack, battler lookup and pokemon_value (10874420). */
    const struct px_battle_ability_api *base;
    uint32_t (*is_ally)(void *user, uint32_t id, uint32_t other);                     /* 1084c5a8 */
    uint32_t (*steal_blocked)(void *user, uint32_t context, uint32_t thief, uint32_t victim); /* 1084bb5c */
    uint32_t (*move_flag)(void *user, uint32_t move, uint32_t flag);                  /* 1080049c; 0 contact */
    uint32_t (*turn_flag)(void *user, uint32_t pokemon, uint32_t flag);               /* 1084c818 */
    uint32_t (*item_locked)(void *user, uint32_t context, uint32_t species, uint32_t item); /* 1085441c */
    uint32_t (*is_berry)(void *user, uint32_t item);                                  /* 108007a4 */
    uint32_t (*battle_decided)(void *user, uint32_t context);                         /* 10865c48 */
    uint32_t (*has_type)(void *user, uint32_t pokemon, uint32_t type);                /* 1086f960 */
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);            /* 10856a88; 0xd disabled */
    uint32_t (*disable_blocked)(void *user, uint32_t move);                           /* 1084c808 */
    uint32_t (*disable_condition)(void *user, uint32_t turns, uint32_t move);         /* 1084beac */
    uint32_t (*chance)(void *user, uint32_t percent);                                 /* 1084c0f8 */
    uint32_t (*forced)(void *user, uint32_t context, uint32_t flag);                  /* 1084c0ec */
    void (*ability_popup)(void *user, uint32_t context, uint32_t phase, uint32_t id); /* 1084c948; 1 show, 2 hide */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);        /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);            /* 10867b48 */
};

#define PX_BATTLE_ABILITIES_8_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_abilities_8_api *api)
PX_BATTLE_ABILITIES_8_HANDLER(px_battle_sticky_hold_check);   /* 108be5b8, ability 60, event 0xa6 */
PX_BATTLE_ABILITIES_8_HANDLER(px_battle_sticky_hold_message); /* 108c3bb8, ability 60, event 0xa7 */
PX_BATTLE_ABILITIES_8_HANDLER(px_battle_pickpocket);          /* 108c0aec, ability 124, event 0x90 */
PX_BATTLE_ABILITIES_8_HANDLER(px_battle_magician_held);       /* 108c2444, ability 170, event 0x8a */
PX_BATTLE_ABILITIES_8_HANDLER(px_battle_symbiosis);           /* 108bdf78, ability 180, event 0x9b */
PX_BATTLE_ABILITIES_8_HANDLER(px_battle_cheek_pouch);         /* 108bf22c, ability 167, event 0x9b */
PX_BATTLE_ABILITIES_8_HANDLER(px_battle_moxie);               /* 108bfe10, ability 153, event 0x8c */
PX_BATTLE_ABILITIES_8_HANDLER(px_battle_color_change);        /* 108be340, ability 16, event 0x8c */
PX_BATTLE_ABILITIES_8_HANDLER(px_battle_cursed_body);         /* 108c10a0, ability 130, event 0x53 */

#endif
