#ifndef PX_BATTLE_ITEMS_1_H
#define PX_BATTLE_ITEMS_1_H

#include "../battle_ability/battle_ability.h"

/* DllBattle item handlers, first batch: the status-curing berries and the
 * HP-restoring berries (static analysis addresses; runtime addresses are
 * module-relative). Analyst names. Item ids are the original ids.
 *
 * Each berry has a check handler (events 0x5e, 0x9c and 0x02 for the cures;
 * 0x9c, 0x5e and 0x61 for the HP berries) that asks for the berry to be used
 * by marking its handler for re-entry and committing a header-only record of
 * kind 0, and a use handler (events 0x7b and 0x7c) that commits the effect.
 * Key 0x02 is the battler concerned, key 0x2f the use flags. Battlers:
 * conditions as words at +0x1c + 4 index (active when the low 3 bits are
 * set). Handlers: flags +0x18, item u16 +0x38.
 *
 * Cures: 1 paralysis, 2 sleep, 3 freeze, 4 burn, 5 poison, 6 confusion,
 * 0x26 the current status, 0x27 it or confusion, 0x28 any listed condition. */
struct px_battle_items_api {
    /* Event stack, battler lookup, pokemon_value (10874420; 0xd HP, 0xe max
     * HP, 0x11 ability) and pokemon_status (1086d8ac). */
    const struct px_battle_ability_api *base;
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);              /* 10856a88 */
    uint32_t (*item_param)(void *user, uint32_t item, uint32_t param);                 /* 1084cf74; 3 the amount */
    void (*ability_popup)(void *user, uint32_t context, uint32_t phase, uint32_t id);  /* 1084c948; phase 0 here */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);         /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                 /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);             /* 10867b48 */
    /* Logical address of the six u16 conditions (the original list at
     * 108d146a, literal 1084c8b4). */
    uint32_t cures;
    /* A logical word standing in for 1084cc48's frame slot. */
    uint32_t scratch;
};

/* 1087bfa0: the current frame's value of key into out; 0 when the frame has
 * no such key. Keys 0 and 1 call the abort helper first. */
uint32_t px_battle_event_find(uint32_t key, uint32_t out, const struct px_battle_event_api *api);
/* 1084c96c: a running handler is marked for re-entry (flag 0x10000000). */
void px_battle_handler_mark_reentry(uint32_t handler, const struct px_battle_event_api *api);
/* 1084c844: the first active condition of the six-entry list; 0 if none
 * (a zero entry ends the list). */
uint32_t px_battle_listed_condition(uint32_t pokemon, const struct px_battle_items_api *api);
/* 1084c730: HP at most max HP / divisor (halved above 2 for Gluttony, 82);
 * never with a max HP of 1 or less. */
uint32_t px_battle_item_pinch(uint32_t context, uint32_t id, uint32_t divisor, const struct px_battle_items_api *api);
/* 1084c198: the HP berries' check; flag also requires key 0x2f bit 0. */
void px_battle_hp_berry_check(uint32_t handler, uint32_t context, uint32_t id, uint32_t divisor, uint32_t flag,
                              const struct px_battle_items_api *api);
/* 1084cc48 and 1084c4c0: the cure berries' check and use for one cure. */
void px_battle_cure_berry_check(uint32_t handler, uint32_t context, uint32_t id, uint32_t cure,
                                const struct px_battle_items_api *api);
void px_battle_cure_berry_use(uint32_t handler, uint32_t context, uint32_t id, uint32_t cure,
                              const struct px_battle_items_api *api);

/* Handlers: (handler, context, battler id, work area), as called by the
 * event walk. */
#define PX_BATTLE_ITEMS_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_items_api *api)
PX_BATTLE_ITEMS_HANDLER(px_battle_cheri_check);   /* 108a6b88, item 149 */
PX_BATTLE_ITEMS_HANDLER(px_battle_cheri_use);     /* 108a86a8 */
PX_BATTLE_ITEMS_HANDLER(px_battle_chesto_check);  /* 108a624c, item 150 */
PX_BATTLE_ITEMS_HANDLER(px_battle_chesto_use);    /* 108a77dc */
PX_BATTLE_ITEMS_HANDLER(px_battle_pecha_check);   /* 108a678c, item 151 */
PX_BATTLE_ITEMS_HANDLER(px_battle_pecha_use);     /* 108a80a0 */
PX_BATTLE_ITEMS_HANDLER(px_battle_rawst_check);   /* 108a64b4, item 152 */
PX_BATTLE_ITEMS_HANDLER(px_battle_rawst_use);     /* 108a7d48 */
PX_BATTLE_ITEMS_HANDLER(px_battle_aspear_check);  /* 108a6be4, item 153 */
PX_BATTLE_ITEMS_HANDLER(px_battle_aspear_use);    /* 108a88d4 */
PX_BATTLE_ITEMS_HANDLER(px_battle_persim_check);  /* 1084cc40, item 156 */
PX_BATTLE_ITEMS_HANDLER(px_battle_persim_use);    /* 1084c4b8 */
PX_BATTLE_ITEMS_HANDLER(px_battle_lum_check);     /* 108a5e70, item 157 */
PX_BATTLE_ITEMS_HANDLER(px_battle_lum_use);       /* 108a733c */
PX_BATTLE_ITEMS_HANDLER(px_battle_hp_berry_9c);   /* 108aa1a8, items 43, 155 and 158 */
PX_BATTLE_ITEMS_HANDLER(px_battle_hp_berry_5e);   /* 108aa15c */
PX_BATTLE_ITEMS_HANDLER(px_battle_hp_berry_61);   /* 108aac30 */
PX_BATTLE_ITEMS_HANDLER(px_battle_oran_use);      /* 108a7a08, items 43 and 155 */
PX_BATTLE_ITEMS_HANDLER(px_battle_sitrus_use);    /* 108a790c, item 158 */

#endif
