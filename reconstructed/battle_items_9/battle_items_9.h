#ifndef PX_BATTLE_ITEMS_9_H
#define PX_BATTLE_ITEMS_9_H

#include "../battle_items_1/battle_items_1.h"

/* DllBattle item handlers, ninth batch: turn-order items (event 0xf), the
 * survival items Focus Band and Focus Sash (events 0x7d, 0x7e and 0x7b) and
 * the flinch items (event 0x75, with King's Rock's event-0x7c use) (static
 * analysis addresses; runtime addresses are module-relative). Analyst names.
 * Item ids are the original ids.
 *
 * Event 0xf sets the holder's priority bracket key 0x11 (2 first, 0 last)
 * through the rewrite 108658d4, which reports whether it took. Event 0x7d
 * (a hit that would knock the holder out) sets key 0x3b to 3 (hold on with
 * 1 HP) and keeps the rewrite's answer in work word 0; event 0x7e then shows
 * the item. Event 0x75 sets the flinch chance key 0x27 unless key 0x26 is
 * set. Chances roll through 1084c0f8 (percent) or are forced by 1084c0ec. */
struct px_battle_items_9_api {
    /* Event stack, battler lookup, pokemon_value (for the pinch test),
     * item_param, 1084c948, records, messages and commit. */
    const struct px_battle_items_api *items;
    uint32_t (*chance)(void *user, uint32_t percent);                   /* 1084c0f8 */
    uint32_t (*forced)(void *user, uint32_t context, uint32_t flag);    /* 1084c0ec */
    uint32_t (*full_hp)(void *user, uint32_t pokemon);                  /* 1084d318 */
};

#define PX_BATTLE_ITEMS_9_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_items_9_api *api)
PX_BATTLE_ITEMS_9_HANDLER(px_battle_custap_check);         /* 108aba18, item 210, event 0xf: at a quarter HP */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_custap_use);           /* 108a761c, event 0x7b */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_quick_claw_check);     /* 108abc80, item 217, event 0xf: parameter-3 percent */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_quick_claw_use);       /* 108a9a4c, event 0x7b */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_lagging_tail);         /* 108a8000, items 279 and 316 (Full Incense), event 0xf */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_kings_rock);           /* 108a7aa0, items 221 and 327 (Razor Fang), event 0x75 */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_kings_rock_use);       /* 108aac94, items 221 and 327, event 0x7c: flinch */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_focus_band_use);       /* 108ab81c, item 230, event 0x7b */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_focus_band_endure);    /* 108ab354, event 0x7d: parameter-3 percent */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_focus_band_show);      /* 108aaa48, event 0x7e */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_focus_sash_endure);    /* 108aa578, item 275, event 0x7d: at full HP */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_focus_sash_show);      /* 108a9810, event 0x7e */

#endif
