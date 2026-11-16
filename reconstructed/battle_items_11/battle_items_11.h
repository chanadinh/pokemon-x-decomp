#ifndef PX_BATTLE_ITEMS_11_H
#define PX_BATTLE_ITEMS_11_H

#include "../battle_items_1/battle_items_1.h"

/* DllBattle item handlers, eleventh batch: the items of event 0x8c (after a
 * move, for each battler it hit) and their uses (static analysis addresses;
 * runtime addresses are module-relative). Analyst names. Item ids are the
 * original ids.
 *
 * Event 0x8c lists the battlers hit: key 0x05 their number, keys 0x06,
 * 0x07, ... the battlers. Each handler acts when the holder is in the list.
 * Key 0x03 is the attacker, key 0x4e set skips the after-effects, key 0x1b
 * is the move's category (1 physical, 2 special). The context byte +0x1093
 * holds per-turn switching flags: bit 0 a switch already pending, bit 3 an
 * Eject Button already claimed. */
struct px_battle_items_11_api {
    /* Event stack, battler lookup, 1084c948, records, messages and commit. */
    const struct px_battle_items_api *items;
    uint32_t (*opposing_selector)(void *user, uint32_t context, uint32_t id);  /* 1084cb78; 0xa none */
    uint32_t (*can_switch)(void *user, uint32_t context, uint32_t id);         /* 1084ca78: a replacement exists */
    uint32_t (*battle_decided)(void *user, uint32_t context);                  /* 10865c48 */
    uint32_t (*charging)(void *user, uint32_t context, uint32_t id);           /* 1084c030 */
    uint32_t (*stage_can_change)(void *user, uint32_t pokemon, uint32_t stat, int32_t delta); /* 108722e8 */
};

/* 1084c000: claim this turn's Eject Button (set bit 3 of context +0x1093);
 * 0 when it was already claimed. */
uint32_t px_battle_claim_eject(uint32_t context, const struct px_pokemon_api *memory);
/* 1084c020: bit 0 of context +0x1093 (a switch is pending). */
uint32_t px_battle_switch_pending(uint32_t context, const struct px_pokemon_api *memory);

#define PX_BATTLE_ITEMS_11_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_items_11_api *api)
PX_BATTLE_ITEMS_11_HANDLER(px_battle_red_card);          /* 108a5e78, item 542, event 0x8c */
PX_BATTLE_ITEMS_11_HANDLER(px_battle_eject_button_check); /* 108ab734, item 547, event 0x8c */
PX_BATTLE_ITEMS_11_HANDLER(px_battle_eject_button_use);  /* 108aa3d4, event 0x7b */
PX_BATTLE_ITEMS_11_HANDLER(px_battle_kee_check);         /* 108aaec4, item 687, event 0x8c: physical, Defense */
PX_BATTLE_ITEMS_11_HANDLER(px_battle_kee_use);           /* 108a7478, events 0x7b and 0x7c */
PX_BATTLE_ITEMS_11_HANDLER(px_battle_maranga_check);     /* 108aadac, item 688, event 0x8c: special, Sp. Def */
PX_BATTLE_ITEMS_11_HANDLER(px_battle_maranga_use);       /* 108a8a30, events 0x7b and 0x7c */

#endif
