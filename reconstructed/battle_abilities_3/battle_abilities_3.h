#ifndef PX_BATTLE_ABILITIES_3_H
#define PX_BATTLE_ABILITIES_3_H

#include "../battle_abilities_1/battle_abilities_1.h"

/* DllBattle end-of-turn abilities: handlers of events 0x80 (end of turn)
 * and 0x7f (end-of-turn status) (static analysis addresses; runtime
 * addresses are module-relative). Analyst names. Key 0x02 is the battler
 * concerned. Battlers: species +0xc, held item +0x12, consumed item +0x14,
 * id byte +0x19. Stats 1-7: Attack, Defense, Sp. Atk, Sp. Def, Speed,
 * accuracy, evasion. */
struct px_battle_abilities_3_api {
    /* Event stack, battler lookup, pokemon_value (10874420), pokemon_status
     * (1086d8ac), side_of (10873158) and side_battlers (1084bc20). */
    const struct px_battle_ability_api *base;
    uint32_t (*battle_flag)(void *user, uint32_t pokemon, uint32_t flag);             /* 1084ca4c; bit of +0x169 */
    uint32_t (*turn_flag)(void *user, uint32_t pokemon, uint32_t flag);               /* 1084c818; bit of +0x166 */
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);            /* 10856a88; 2 sleep */
    uint32_t (*chance)(void *user, uint32_t percent);                                 /* 1084c0f8 */
    uint32_t (*forced)(void *user, uint32_t context, uint32_t flag);                  /* 1084c0ec */
    uint32_t (*random)(void *user, uint32_t range);                                   /* 1084c7bc */
    uint32_t (*stage_can_change)(void *user, uint32_t pokemon, uint32_t stat, int32_t delta); /* 108722e8 */
    uint32_t (*temp_buffer)(void *user, uint32_t context, uint32_t size);             /* 1084c7d0 */
    uint32_t (*opposing_selector)(void *user, uint32_t context, uint32_t id);         /* 1084cb78 */
    uint32_t (*weather)(void *user, uint32_t context);                                /* 1084be58; 1 sun */
    uint32_t (*is_berry)(void *user, uint32_t item);                                  /* 108007a4 */
    uint32_t (*item_locked)(void *user, uint32_t context, uint32_t species, uint32_t item); /* 1085441c */
    uint32_t (*queued_action)(void *user, uint32_t context, uint32_t id, uint32_t out); /* 1084b784; word at out */
    void (*ability_popup)(void *user, uint32_t context, uint32_t phase, uint32_t id); /* 1084c948; 1 show, 2 hide */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);        /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);            /* 10867b48 */
    /* Logical stack areas standing in for the original frames: a battler
     * list (Healer 8 bytes, Bad Dreams 12) and Slow Start's action word. */
    uint32_t list_scratch, action_scratch;
};

#define PX_BATTLE_ABILITIES_3_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_abilities_3_api *api)
/* Event 0x80. */
PX_BATTLE_ABILITIES_3_HANDLER(px_battle_speed_boost);     /* 108bcff0, ability 3 */
PX_BATTLE_ABILITIES_3_HANDLER(px_battle_moody);           /* 108bdb4c, ability 141 */
PX_BATTLE_ABILITIES_3_HANDLER(px_battle_harvest);         /* 108beb00, ability 139 */
PX_BATTLE_ABILITIES_3_HANDLER(px_battle_pickup);          /* 108bf590, ability 53 */
PX_BATTLE_ABILITIES_3_HANDLER(px_battle_bad_dreams);      /* 108bf750, ability 123 */
PX_BATTLE_ABILITIES_3_HANDLER(px_battle_slow_start_turn); /* 108c4bac, ability 112 */
/* Event 0x7f. */
PX_BATTLE_ABILITIES_3_HANDLER(px_battle_shed_skin);       /* 108bc800, ability 61 */
PX_BATTLE_ABILITIES_3_HANDLER(px_battle_healer);          /* 108bff30, ability 131 */
PX_BATTLE_ABILITIES_3_HANDLER(px_battle_levitate_turn);   /* 108c282c, ability 26 */

#endif
