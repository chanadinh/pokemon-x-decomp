#ifndef PX_BATTLE_ITEMS_13_H
#define PX_BATTLE_ITEMS_13_H

#include "../battle_items_1/battle_items_1.h"

/* DllBattle item handlers, thirteenth and last batch: the remaining entries
 * of Amulet Coin and Luck Incense, Smoke Ball, Destiny Knot, Float Stone,
 * Ring Target and Safety Goggles (static analysis addresses; runtime
 * addresses are module-relative). Analyst names. Item ids are the original
 * ids. Key 0x02 the holder (or the battler concerned), key 0x03 the attacker,
 * key 0x04 the battler hit. */
struct px_battle_items_13_api {
    /* Event stack, battler lookup, records, messages and commit. */
    const struct px_battle_items_api *items;
    void (*prize_money)(void *user, uint32_t context, uint32_t id, uint32_t value);   /* 1084c3cc (Happy Hour's routine) */
    uint32_t (*is_ally)(void *user, uint32_t id, uint32_t other);                      /* 1084c5a8 */
    void (*message_extra)(void *user, uint32_t message, uint32_t value);              /* 10876c28 */
    uint32_t (*infatuation_condition)(void *user, uint32_t id);                       /* 10876e20 */
    uint32_t (*no_consume)(void *user, uint32_t context);                             /* 1084c70c */
};

#define PX_BATTLE_ITEMS_13_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_items_13_api *api)
PX_BATTLE_ITEMS_13_HANDLER(px_battle_amulet_coin_5e);   /* 108a79d4, items 223 and 319, event 0x5e */
PX_BATTLE_ITEMS_13_HANDLER(px_battle_amulet_coin_61);   /* 1084c3bc, event 0x61 */
PX_BATTLE_ITEMS_13_HANDLER(px_battle_smoke_ball_0b);    /* 108a6adc, item 228, event 0x0b */
PX_BATTLE_ITEMS_13_HANDLER(px_battle_smoke_ball_0d);    /* 108a85fc, event 0x0d */
PX_BATTLE_ITEMS_13_HANDLER(px_battle_destiny_knot);     /* 108a5c9c, item 280, event 0x72 */
PX_BATTLE_ITEMS_13_HANDLER(px_battle_float_stone);      /* 108a5db4, item 539, event 0x84 */
PX_BATTLE_ITEMS_13_HANDLER(px_battle_ring_target);      /* 108a71f4, item 543, event 0x44 */
PX_BATTLE_ITEMS_13_HANDLER(px_battle_safety_goggles);   /* 108ab6f8, item 650, event 0x88 */

#endif
