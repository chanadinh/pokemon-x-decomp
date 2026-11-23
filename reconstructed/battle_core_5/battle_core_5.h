#ifndef PX_BATTLE_CORE_5_H
#define PX_BATTLE_CORE_5_H

#include "../battle_core_4/battle_core_4.h"

/* DllBattle core helpers, fifth batch: 22 functions in 10840000-10880000 and
 * 108a6000 and up (static analysis addresses; runtime addresses are
 * module-relative). The names are analyst interpretations.
 *
 * The battle state object (108fca00) holds the random number object at +0x54,
 * the item data object at +0x5c and the first handler at +0x60. Several
 * helpers are one-instruction wrappers around such an object's import thunk
 * (108003dc a random value below a limit, 108003ac an item data value);
 * those are callbacks here. 108c59b4 is the handler-blocking test the event
 * milestone takes as its `blocked` callback. */
struct px_battle_core_5_api {
    /* The battler, record and event callbacks of battle_core_4 (and with them the
     * memory, the abort routine and pokemon_value). */
    const struct px_battle_core_4_api *core4;
    uint32_t battle;                                                                         /* 108fca00 */
    uint32_t (*random)(void *user, uint32_t object, uint32_t limit);                          /* 108003dc */
    uint32_t (*item_value)(void *user, uint32_t object, uint32_t item, uint32_t param);       /* 108003ac */
    /* A handler's own blocking function (word +0xc of the handler). */
    uint32_t (*checker)(void *user, uint32_t node, uint32_t context, uint32_t owner, uint32_t event,
                        uint32_t id, uint32_t value);
};

/* The random object and the item data object. */
uint32_t px_battle_random(uint32_t limit, const struct px_battle_core_5_api *api);                          /* 1084c7bc */
uint32_t px_battle_item_value(uint32_t item, uint32_t param, const struct px_battle_core_5_api *api);      /* 1084cf74 */
uint32_t px_battle_chance(uint32_t percent, const struct px_battle_core_5_api *api);                        /* 1084c0f8 */

/* Bounds-checked accessors (a bad index calls the abort routine). */
uint32_t px_battle_ring_length(uint32_t context, uint32_t back, const struct px_battle_core_5_api *api);    /* 108c86d4 */
uint32_t px_battle_group_count(uint32_t context, uint32_t group, const struct px_battle_core_5_api *api);   /* 108c99fc */
uint32_t px_battle_entry_flag_121(uint32_t context, uint32_t index, const struct px_battle_core_5_api *api); /* 108c8714 */
uint32_t px_battle_entry_83c(uint32_t object, uint32_t index, const struct px_battle_core_5_api *api);      /* 108c8400 */
uint32_t px_battle_byte_or_6(uint32_t object, uint32_t index, const struct px_battle_core_5_api *api);     /* 108c87a0 (no abort) */
uint32_t px_battle_condition_word(uint32_t battler, uint32_t index, const struct px_battle_core_5_api *api); /* 108c84e0 */
uint32_t px_battle_byte_7ec(uint32_t context, uint32_t index, const struct px_battle_core_5_api *api);      /* 108c78ec */
uint32_t px_battle_status_is_0x78(uint32_t battler, const struct px_battle_core_5_api *api);                 /* 108c8510 */
uint32_t px_battle_slot_half(uint32_t object, uint32_t index, const struct px_battle_core_5_api *api);       /* 108c79b0 */
uint32_t px_battle_next_pair(uint32_t context, const struct px_battle_core_5_api *api);                      /* 108c82f4 */

/* Small struct initializers. */
void px_battle_clear_18(uint32_t address, const struct px_battle_core_5_api *api);                           /* 108c5c18 */
void px_battle_clear_10(uint32_t address, const struct px_battle_core_5_api *api);                           /* 108c9e30 */
void px_battle_fill_10(uint32_t address, uint32_t value, const struct px_battle_core_5_api *api);            /* 108c9e84 */
void px_battle_copy_7(uint32_t destination, uint32_t source, const struct px_battle_core_5_api *api);       /* 108c9e68 */

/* Lists and event builders. */
uint32_t px_battle_find_matching_entry(uint32_t unused, uint32_t table, uint32_t entry, const struct px_battle_core_5_api *api); /* 10872ff8 */
void px_battle_add_unique(uint32_t list, uint32_t value, const struct px_battle_core_5_api *api);            /* 108696b0 */
uint32_t px_battle_add_unique_slot(uint32_t context, uint32_t index, uint32_t value, const struct px_battle_core_5_api *api); /* 1086f878 */
uint32_t px_battle_scaled_hp(uint32_t context, uint32_t id, const struct px_battle_core_5_api *api);         /* 1084b9f0 */
uint32_t px_battle_event_blocked(uint32_t context, uint32_t handler, uint32_t event, const struct px_battle_core_5_api *api); /* 108c59b4 */

#endif
