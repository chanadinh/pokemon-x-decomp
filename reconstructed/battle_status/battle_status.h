#ifndef PX_BATTLE_STATUS_H
#define PX_BATTLE_STATUS_H

#include "../battle_ability/battle_ability.h"

/* DllBattle handlers of the status-prevention events (static analysis
 * addresses; runtime addresses are module-relative). Analyst names. Every
 * entry of events 0x6e (block a status) and 0x70 (the blocked-status
 * message) is covered. Keys: 0x03 the battler causing it, 0x04 the target,
 * 0x1e status (16 bits: 1 paralysis, 2 sleep, 3 freeze, 4 burn, 5 poison,
 * 6 confusion, 7 attraction, 0xb taunt, 0xe drowsiness), 0x42 blocked,
 * 0x44 Aroma Veil. Handler work word 0 carries "blocked" to the message;
 * Uproar reads work word 6. Type 11 is Grass; weather 1 is sun. */
struct px_battle_status_api {
    /* Event stack and battler lookup. */
    const struct px_battle_ability_api *base;
    uint32_t (*is_ally)(void *user, uint32_t id, uint32_t other);           /* 1084c5a8 */
    uint32_t (*has_type)(void *user, uint32_t pokemon, uint32_t type);      /* 1086f960 */
    uint32_t (*weather)(void *user, uint32_t context);                      /* 1084be58 */
    uint32_t (*major_status)(void *user, uint32_t status);                  /* 1084bf04 */
    uint32_t (*mental_status)(void *user, uint32_t status);                 /* 108a4764 */
    void (*ability_popup)(void *user, uint32_t context, uint32_t phase, uint32_t id); /* 1084c948 */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id); /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);      /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);  /* 10867b48 */
};

#define PX_BATTLE_STATUS_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_status_api *api)
/* Event 0x6e. */
PX_BATTLE_STATUS_HANDLER(px_battle_limber);        /* 108c2a58, ability 7 */
PX_BATTLE_STATUS_HANDLER(px_battle_insomnia);      /* 108c1cb8, abilities 15 and 72 */
PX_BATTLE_STATUS_HANDLER(px_battle_magma_armor);   /* 108c556c, ability 40 */
PX_BATTLE_STATUS_HANDLER(px_battle_water_veil);    /* 108c4b58, ability 41 */
PX_BATTLE_STATUS_HANDLER(px_battle_immunity_status); /* 108c2afc, ability 17 */
PX_BATTLE_STATUS_HANDLER(px_battle_own_tempo);     /* 108c2b88, ability 20 */
PX_BATTLE_STATUS_HANDLER(px_battle_oblivious_status); /* 108bce68, ability 12 */
PX_BATTLE_STATUS_HANDLER(px_battle_leaf_guard);    /* 108bf864, ability 102 */
PX_BATTLE_STATUS_HANDLER(px_battle_sweet_veil);    /* 108c4618, ability 175 */
PX_BATTLE_STATUS_HANDLER(px_battle_flower_veil_status); /* 108c4db8, ability 166 */
PX_BATTLE_STATUS_HANDLER(px_battle_aroma_veil);    /* 108c333c, ability 165 */
PX_BATTLE_STATUS_HANDLER(px_battle_uproar);        /* 108bae4c, move 253 */
/* Event 0x70. */
PX_BATTLE_STATUS_HANDLER(px_battle_status_blocked_message);   /* 108c35f8, nine abilities */
PX_BATTLE_STATUS_HANDLER(px_battle_own_tempo_message);        /* 108c4fa4, ability 20 */
PX_BATTLE_STATUS_HANDLER(px_battle_aroma_veil_message);       /* 108c3b00, ability 165 */
PX_BATTLE_STATUS_HANDLER(px_battle_flower_veil_status_message); /* 108c51c8, ability 166 */
PX_BATTLE_STATUS_HANDLER(px_battle_sweet_veil_message);       /* 108c5798, ability 175 */

#endif
