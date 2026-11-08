#ifndef PX_BATTLE_ITEMS_2_H
#define PX_BATTLE_ITEMS_2_H

#include "../battle_items_1/battle_items_1.h"
#include "../battle_abilities_1/battle_abilities_1.h"

/* DllBattle item handlers, second batch: the pinch berries (static analysis
 * addresses; runtime addresses are module-relative). Analyst names. Item ids
 * are the original ids.
 *
 * Each berry has three check handlers (events 0x9c, 0x5e and 0x61) that ask
 * for the berry with the first batch's HP-berry check, pinch test and
 * re-entry mark, and a use handler (events 0x7b and 0x7c). The Figy family
 * restores a share of max HP and confuses a battler that dislikes the flavor;
 * the stat berries raise a stat (kind-0xd record), Lansat sets the
 * critical-hit state 9 and Starf raises a random stat by two.
 *
 * Kind 0xd: +4 stat, +0xc battler, +0x16 count 1, +0x17 signed stages.
 * Stats: 1 Attack, 2 Defense, 3 Sp. Atk, 4 Sp. Def, 5 Speed. Flavors: 0
 * spicy, 1 dry, 2 sweet, 3 bitter, 4 sour. */
struct px_battle_items_2_api {
    /* The first batch's services (event stack, battler lookup, pokemon_value,
     * has_status, item_param, 1084c948, records, messages and commit). */
    const struct px_battle_items_api *items;
    /* For 1084d2f4 (px_battle_hp_share), which reads pokemon_value through
     * its base; the same base as items. */
    const struct px_battle_abilities_1_api *abilities;
    uint32_t (*flavor_taste)(void *user, uint32_t data, uint32_t flavor);              /* 108014cc; 2 disliked */
    void (*condition_for)(void *user, uint32_t status, uint32_t pokemon, uint32_t out); /* 1084c438; word at out */
    uint32_t (*temp_buffer)(void *user, uint32_t context, uint32_t size);              /* 1084c7d0 */
    uint32_t (*stage_can_change)(void *user, uint32_t pokemon, uint32_t stat, int32_t delta); /* 108722e8 */
    uint32_t (*random)(void *user, uint32_t range);                                    /* 1084c7bc; below range */
    /* Logical address of Starf's five stats (the original bytes at 108d2c3a,
     * literal 108a68e0). */
    uint32_t starf_stats;
};

/* 1084c5e0: the Figy family's use. Work word 0 records that the berry has
 * been used; flavor is the fifth (stack) argument. */
void px_battle_figy_berry_use(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, uint32_t flavor,
                              const struct px_battle_items_2_api *api);

#define PX_BATTLE_ITEMS_2_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_items_2_api *api)
PX_BATTLE_ITEMS_2_HANDLER(px_battle_figy_check_9c);   /* 108abd68, items 159-163 */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_figy_check_5e);   /* 108abd10 */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_figy_check_61);   /* 108abdc0 */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_figy_use);        /* 108a7584, item 159, event 0x7b */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_figy_use_7c);     /* 108a8e3c, event 0x7c */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_wiki_use);        /* 108a6d3c, item 160 */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_wiki_use_7c);     /* 108a8298 */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_mago_use);        /* 108a782c, item 161 */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_mago_use_7c);     /* 108a91cc */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_aguav_use);       /* 108a7d34, item 162 */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_aguav_use_7c);    /* 108a95a0 */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_iapapa_use);      /* 108a6a50, item 163 */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_iapapa_use_7c);   /* 108a7e88 */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_stat_check_9c);   /* 108a9948, items 201-207 */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_stat_check_5e);   /* 108aa67c */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_stat_check_61);   /* 108ab100 */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_liechi_use);      /* 108a6924, item 201 */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_ganlon_use);      /* 108a67b4, item 202 */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_salac_use);       /* 108a6a64, item 203 */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_petaya_use);      /* 108a6d98, item 204 */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_apicot_use);      /* 108a60c8, item 205 */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_lansat_use);      /* 108a5fc0, item 206 */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_starf_use);       /* 108a680c, item 207 */

#endif
