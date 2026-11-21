#ifndef PX_BATTLE_MOVES_18_H
#define PX_BATTLE_MOVES_18_H

#include "../battle_ability/battle_ability.h"
#include "../battle_handler/battle_handler.h"

/* DllBattle move handlers of events 0x7d-0x9f (static analysis addresses;
 * runtime addresses are module-relative). Analyst names. Keys: 0x02 battler,
 * 0x03 attacker, 0x04 defender, 0x06 target (ids); 0x36 (the move's power
 * modifier), 0x3b, 0x42, 0x53 (values). Work words: +0, +4 (the pledge's
 * form), +0x18 (word 6: the move's flag).
 * Records (10869f7c/10867b48): kind 3 message at +4; kind 0xa cure (+4
 * condition, +5 battler, +0x11); kind 0x15 (+4, +5); kind 0x19 condition
 * (+4 kind byte, +0xc side byte, +8 turn word, message at +0x10); kind 0x2a
 * (+0x4c id byte, message at +4, a flag word at +0); kind 0x38 (+4 halfword,
 * +6 side, +7 = 0xa); kind 0x3a (+4 battler, +5 flag, message at +8). */
struct px_battle_moves_18_api {
    /* Event stack, battler lookup, side_of (10873158) and the ability api. */
    const struct px_battle_ability_api *base;
    /* Registered handlers (a handler removes the move's handlers). */
    const struct px_battle_handler_api *handlers;
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id);    /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);                  /* 10867b48 */
    void (*message)(void *user, uint32_t at, uint32_t kind, uint32_t message);              /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t at, uint32_t value);                           /* 1084d29c */
    uint32_t (*no_hp)(void *user, uint32_t pokemon);                                        /* 1085f844 */
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);                  /* 10856a88 */
    uint32_t (*is_ally)(void *user, uint32_t id, uint32_t other);                           /* 1084c5a8 */
    uint32_t (*weather)(void *user, uint32_t context);                                      /* 1084be58 */
    uint32_t (*counter)(void *user, uint32_t pokemon, uint32_t index);                      /* 1084bb2c */
    uint32_t (*can_switch)(void *user, uint32_t context, uint32_t id);                      /* 1084ca78 */
    uint32_t (*claim_switch)(void *user, uint32_t context);                                 /* 1084c000 */
    uint32_t (*owner_of)(void *user, uint32_t id);                                          /* 1087425c */
    uint32_t (*side_id)(void *user, uint32_t id);                                           /* 1084bfc0 */
    uint32_t (*position)(void *user, uint32_t context, uint32_t id);                        /* 1084b590; 0xa none */
    uint32_t (*turn_word)(void *user, uint32_t turns);                                      /* 1084be3c */
};

#define PX_BATTLE_MOVES_18_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_18_api *api)
PX_BATTLE_MOVES_18_HANDLER(px_battle_support_boost);        /* 108af958, moves 206 and 610, event 0x7d */
PX_BATTLE_MOVES_18_HANDLER(px_battle_flagged_boost);        /* 108b6e80, move 203, event 0x7d */
PX_BATTLE_MOVES_18_HANDLER(px_battle_uproar_end);           /* 108b8590, move 253, event 0x80 */
PX_BATTLE_MOVES_18_HANDLER(px_battle_sleep_wake);           /* 108b898c, moves 37, 80, 200, event 0x80 */
PX_BATTLE_MOVES_18_HANDLER(px_battle_item_flag);            /* 108b67ec, moves 168 and 343, event 0x8a */
PX_BATTLE_MOVES_18_HANDLER(px_battle_fling_start);          /* 108bbbb4, move 374, event 0x8a */
PX_BATTLE_MOVES_18_HANDLER(px_battle_form_check);           /* 108b4e64, move 547, event 0x8e */
PX_BATTLE_MOVES_18_HANDLER(px_battle_pledge_effect);        /* 108b9244, moves 518-520, event 0x8e */
PX_BATTLE_MOVES_18_HANDLER(px_battle_switch_out);           /* 108b4678, moves 369, 521 (0x8f) and 575 (0x92) */
PX_BATTLE_MOVES_18_HANDLER(px_battle_weather_power);        /* 108b4bb8, moves 234-236, event 0x99 */
PX_BATTLE_MOVES_18_HANDLER(px_battle_stockpile_power);      /* 108b7670, move 256, event 0x99 */
PX_BATTLE_MOVES_18_HANDLER(px_battle_ally_target);          /* 108b8b84, move 507, event 0x9e */
PX_BATTLE_MOVES_18_HANDLER(px_battle_sun_ready);            /* 108b9c70, move 76, event 0x9f */

#endif
