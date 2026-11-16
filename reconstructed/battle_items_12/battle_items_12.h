#ifndef PX_BATTLE_ITEMS_12_H
#define PX_BATTLE_ITEMS_12_H

#include "../battle_items_1/battle_items_1.h"

/* DllBattle item handlers, twelfth batch: Leppa Berry (154) and Micle Berry
 * (209) (static analysis addresses; runtime addresses are module-relative).
 * Analyst names. Item ids are the original ids.
 *
 * Battlers keep four move slots of 0xe bytes from +0x11c (the move id a
 * halfword at the start of each), the number of moves at +0x152 and the last
 * move used at +0x162. Leppa Berry restores PP to a slot with none left (the
 * last move's first); used on event 0x7c (as when flung) it may also pick any
 * slot 1084d184 accepts. Micle Berry asks to be used at a pinch (parameter 3
 * as the divisor) and gives the holder condition 0x22. */
struct px_battle_items_12_api {
    /* Event stack, battler lookup, pokemon_value, item_param, 1084c948,
     * records, messages and commit. */
    const struct px_battle_items_api *items;
    uint32_t (*move_pp)(void *user, uint32_t pokemon, uint32_t slot);        /* 108520e4 */
    uint32_t (*pp_restorable)(void *user, uint32_t pokemon, uint32_t slot);  /* 1084d184 */
    uint32_t (*slot_move)(void *user, uint32_t pokemon, uint32_t slot);      /* 10855c68 */
    uint32_t (*condition_word)(void *user, uint32_t value);                  /* 1084c3b0 */
};

/* 1084c278: the last move used (+0x162). */
uint32_t px_battle_last_move(uint32_t pokemon, const struct px_pokemon_api *memory);
/* 1084c224: the slot holding `move` (0-3), or 4; move 0 is in no slot. */
uint32_t px_battle_move_slot(uint32_t pokemon, uint32_t move, const struct px_pokemon_api *memory);
/* 108a9e1c: Leppa Berry's use; `any` (the stack argument) also allows any
 * restorable slot. Returns whether it restored PP. */
uint32_t px_battle_leppa_use(uint32_t handler, uint32_t context, uint32_t id, uint32_t any,
                             const struct px_battle_items_12_api *api);

#define PX_BATTLE_ITEMS_12_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_items_12_api *api)
PX_BATTLE_ITEMS_12_HANDLER(px_battle_leppa_check_04);  /* 108aa494, item 154, event 0x04: the last move */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_leppa_check_57);  /* 108a9710, event 0x57 */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_leppa_check_5e);  /* 1084c8b8, events 0x5e and 0xa9: any slot */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_leppa_check_61);  /* 108a977c, event 0x61 */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_leppa_check_9c);  /* 108aa970, event 0x9c */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_leppa_eat);       /* 108a8588, event 0x7b */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_leppa_eat_7c);    /* 108a9dfc, event 0x7c */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_micle_check_02);  /* 108ab518, item 209, event 0x02 */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_micle_check_5e);  /* 108aab1c, event 0x5e */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_micle_check_61);  /* 108ab5a0, event 0x61 */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_micle_check_9c);  /* 108aab94, event 0x9c */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_micle_use);       /* 108a871c, event 0x7b */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_micle_use_7c);    /* 108aa074, event 0x7c */

#endif
