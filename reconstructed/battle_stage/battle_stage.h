#ifndef PX_BATTLE_STAGE_H
#define PX_BATTLE_STAGE_H

#include "../battle_ability/battle_ability.h"

/* DllBattle ability handlers of the stat-stage events (static analysis
 * addresses; runtime addresses are module-relative). Analyst names. Every
 * entry of events 0x63 (change size), 0x64 (block a drop), 0x65 (blocked-drop
 * message) and 0x66 (after a drop) is covered. Keys: 0x02 the battler whose
 * stats change, 0x03 the battler causing it, 0x20 stat (1 Attack, 2 Defense,
 * 6 accuracy), 0x21 stages (signed), 0x42 blocked. Type 11 is Grass. */
struct px_battle_stage_api {
    /* Event stack and battler lookup. */
    const struct px_battle_ability_api *base;
    uint32_t (*is_ally)(void *user, uint32_t id, uint32_t other);           /* 1084c5a8 */
    uint32_t (*has_type)(void *user, uint32_t pokemon, uint32_t type);      /* 1086f960 */
    void (*blocked_message)(void *user, uint32_t context, uint32_t holder, uint32_t id,
                            uint32_t work, uint32_t message);             /* 1084a9d4 */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);  /* 10867b48 */
};

#define PX_BATTLE_STAGE_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_stage_api *api)
/* Event 0x63. */
PX_BATTLE_STAGE_HANDLER(px_battle_simple);            /* 108bd42c, ability 86 */
PX_BATTLE_STAGE_HANDLER(px_battle_contrary);          /* 108bef24, ability 126 */
/* Event 0x64. */
PX_BATTLE_STAGE_HANDLER(px_battle_clear_body);        /* 108c2740, abilities 29 and 73 */
PX_BATTLE_STAGE_HANDLER(px_battle_hyper_cutter);      /* 108c489c, ability 52 */
PX_BATTLE_STAGE_HANDLER(px_battle_keen_eye);          /* 108c2bdc, ability 51 */
PX_BATTLE_STAGE_HANDLER(px_battle_big_pecks);         /* 108c1e40, ability 145 */
PX_BATTLE_STAGE_HANDLER(px_battle_flower_veil);       /* 108c30a4, ability 166 */
/* Event 0x65. */
PX_BATTLE_STAGE_HANDLER(px_battle_clear_body_message);   /* 108c2790, message 0xc6 */
PX_BATTLE_STAGE_HANDLER(px_battle_hyper_cutter_message); /* 108c48fc, message 0xc9 */
PX_BATTLE_STAGE_HANDLER(px_battle_keen_eye_message);     /* 108c2c3c, message 0xcf */
PX_BATTLE_STAGE_HANDLER(px_battle_big_pecks_message);    /* 108c1ea0, message 0xcc */
PX_BATTLE_STAGE_HANDLER(px_battle_flower_veil_message);  /* 108c3134, message 0x49c */
/* Event 0x66. */
PX_BATTLE_STAGE_HANDLER(px_battle_defiant);           /* 108bdab0, ability 128 */
PX_BATTLE_STAGE_HANDLER(px_battle_competitive);       /* 108bd070, ability 172 */

#endif
