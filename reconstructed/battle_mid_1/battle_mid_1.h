#ifndef PX_BATTLE_MID_1_H
#define PX_BATTLE_MID_1_H

#include "../battle_ability/battle_ability.h"

/* DllBattle helpers of the static band 10880000-108a6000 that committed
 * milestones call but never reconstructed (static analysis addresses; runtime
 * addresses are module-relative). Analyst names. Logical ARM32 addresses.
 * Eleven functions test a value against a halfword list (1086fe6c), one
 * returns a list, and six do small computations or build records. The lists
 * are read-only data reached through relocated literals; their addresses are
 * inputs here, like the callbacks. */
struct px_battle_mid_1_api {
    /* Event stack and memory (keys 0x15, 0x16, 0x4c and 0x4d are used). */
    const struct px_battle_event_api *events;
    /* Is `value` one of the `count` halfwords at `table`? (nonzero when it is) */
    uint32_t (*in_list)(void *user, uint32_t value, uint32_t table, uint32_t count);        /* 1086fe6c */
    uint32_t (*slot_status)(void *user, uint32_t context, uint32_t slot);                   /* 10860c80 */
    uint32_t (*word_owner)(void *user, uint32_t word);                                      /* 10871408 */
    uint32_t (*modulo)(void *user, uint32_t value, uint32_t divisor);                       /* 108003dc */
    /* 17 entries of 6 bytes: id, then two signed halfwords (literal 108822fc; static 108d25fe). */
    uint32_t effect_table;
    /* The global of two words (+0x54 a random value, +0x58 a halfword buffer) (literal 1089e7e8; static 108fca00). */
    uint32_t state;
    /* Halfword lists (static addresses of the relocated literals). */
    uint32_t list_25c6, list_25b8, list_24fa, list_256e, list_146a, list_252e, list_25ac, list_2378, list_25f0, list_2384,
        list_25a0, list_10f0, list_23b6, list_1476;
};

/* 10882188: appends to an action-argument record (halfword id at +0, halfword header at +2, words from +4; header bits
 * 0-6 kind, 7-13 count of words, at most 8) the values of table entry `kind`: returns 0 when there is no entry or no value. */
uint32_t px_battle_mid_effect_record(uint32_t kind, uint32_t argument, uint32_t pokemon, uint32_t value, uint32_t out,
                                     const struct px_battle_mid_1_api *api);
/* 10888a88: three condition words of an object (+0x60, +0x70, +0x98) set keys 0x4c and 0x4d. */
void px_battle_mid_pending_flags(uint32_t object, const struct px_battle_mid_1_api *api);
/* 1089e5ac: the slots 0-17 whose status is 0 or 6, stored as bytes; returns how many. */
uint32_t px_battle_mid_free_slots(uint32_t context, uint32_t out, const struct px_battle_mid_1_api *api);
/* 1089e5f8: a * b / 4096, rounded half down, at least 1. */
uint32_t px_battle_mid_scale_12(uint32_t a, uint32_t b);
/* 1089e724: a random value of 1-0x269 that is not among the `count` halfwords at `list`; 0 when there is none. */
uint32_t px_battle_mid_random_unused(uint32_t list, uint32_t count, const struct px_battle_mid_1_api *api);
/* List tests (the value is looked up in the lists named; counts are the lists' sizes). */
uint32_t px_battle_listed_46c4(uint32_t value, const struct px_battle_mid_1_api *api);   /* 108a46c4: list_25c6, 21 */
uint32_t px_battle_listed_4714(uint32_t value, const struct px_battle_mid_1_api *api);   /* 108a4714: list_25b8, 7 */
uint32_t px_battle_listed_4724(uint32_t value, const struct px_battle_mid_1_api *api);   /* 108a4724: list_24fa 14, else list_256e 25 */
uint32_t px_battle_listed_4764(uint32_t value, const struct px_battle_mid_1_api *api);   /* 108a4764: list_146a, 6 (read directly) */
uint32_t px_battle_listed_47b4(uint32_t value, const struct px_battle_mid_1_api *api);   /* 108a47b4: list_24fa 14, else list_252e 32 */
uint32_t px_battle_listed_47f4(uint32_t value, const struct px_battle_mid_1_api *api);   /* 108a47f4: list_25ac, 6 */
uint32_t px_battle_listed_4804(uint32_t value, const struct px_battle_mid_1_api *api);   /* 108a4804: list_2378, 6 */
uint32_t px_battle_listed_4814(uint32_t value, const struct px_battle_mid_1_api *api);   /* 108a4814: list_25f0, 7 */
uint32_t px_battle_listed_4824(uint32_t value, const struct px_battle_mid_1_api *api);   /* 108a4824: list_2384, 11 */
uint32_t px_battle_listed_4844(uint32_t value, const struct px_battle_mid_1_api *api);   /* 108a4844: list_25a0, 6 */
uint32_t px_battle_listed_4854(uint32_t value, const struct px_battle_mid_1_api *api);   /* 108a4854: 0x19 is never; list_10f0 2, else list_23b6 10 */
uint32_t px_battle_listed_48e4(uint32_t value, const struct px_battle_mid_1_api *api);   /* 108a48e4: list_10f0 2, else list_23b6 10 */
/* 108a48a0: stores 0x36 through `count` and returns list_1476 (a list of 0x36 entries). */
uint32_t px_battle_mid_list_1476(uint32_t count, const struct px_battle_mid_1_api *api);

#endif
