#ifndef PX_BATTLE_ITEMS_10_H
#define PX_BATTLE_ITEMS_10_H

#include "../battle_items_1/battle_items_1.h"

/* DllBattle item handlers, tenth batch: the herbs (static analysis addresses;
 * runtime addresses are module-relative). Analyst names. Item ids are the
 * original ids.
 *
 * White Herb (214) asks to be used on several events once a stat stage is
 * lowered, and its use (kind 0x11) restores them. Mental Herb (219) uses the
 * first batch's listed-condition scan and cure body with cure 0x28. Power
 * Herb (271) skips a two-turn move's charging turn: on event 0x9f it sets
 * key 0x53 (except for Sky Drop) and remembers whether that took, and on
 * event 0xa2 asks to be used. Battlers keep seven stat stages as signed
 * bytes at +0x104..+0x10a, 6 neutral. */
struct px_battle_items_10_api {
    /* Event stack, battler lookup, item_param, 1084c948, records, messages,
     * commit and the condition list (1084c844). */
    const struct px_battle_items_api *items;
};

/* 1084c140: whether any of the seven stages is below 6. */
uint32_t px_battle_stat_lowered(uint32_t pokemon, const struct px_pokemon_api *memory);

#define PX_BATTLE_ITEMS_10_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_items_10_api *api)
PX_BATTLE_ITEMS_10_HANDLER(px_battle_white_herb_02);    /* 108a9ad0, item 214, event 0x02 */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_white_herb_60);    /* 108ab69c, events 0x60 and 0x61 */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_white_herb_80);    /* 108aace0, event 0x80 */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_white_herb_9c);    /* 108aa6dc, event 0x9c */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_white_herb_use);   /* 108a81b8, event 0x7b */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_white_herb_use_7c); /* 108a9b28, event 0x7c */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_mental_herb_check); /* 108a98a0, item 219, events 0x5e and 0x9c */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_mental_herb_use);  /* 108a8714, event 0x7b */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_mental_herb_use_7c); /* 108aa05c, event 0x7c */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_power_herb_use);   /* 108a91ec, item 271, event 0x7b */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_power_herb_charge); /* 108abb48, event 0x9f */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_power_herb_check); /* 108ab910, event 0xa2 */

#endif
