#ifndef PX_BATTLE_CORE_4_H
#define PX_BATTLE_CORE_4_H

#include "../battle_core_3/battle_core_3.h"
#include "../battle_fields_1/battle_fields_1.h"

/* DllBattle core helpers, fourth batch: 15 functions in 10840000-10880000
 * and 108a6000 and up, mostly the battler-condition queries around 1084a9d4
 * to 1084d184 (static analysis addresses; runtime addresses are
 * module-relative). The names are analyst interpretations.
 *
 * The battle context holds the battler table pointer at +8: battlers
 * are the pointers at +0x84 + 4 id (id below 0x18; a bad id or a missing
 * battler calls the abort routine and the code goes on). Battler fields used
 * here: condition words +0x70, +0x94, +0x98, +0x9c and +0xa0 (active when the low 3 bits
 * are set), +0x68, byte +0x171 and the type bytes of battle_core_1. The
 * context's +0x1c object holds the flags at +0x27c and +0x290. */
struct px_battle_core_4_api {
    /* The event stack, records, messages, commit and ability popup. */
    const struct px_battle_core_api *core;
    /* Memory, abort routine and pokemon_value (10874420). */
    const struct px_battle_handler_api *handler;
    /* The four-byte id table 10874348 reads (literal 108743a4). */
    uint32_t id_table;
};

/* Event-based checks. */
uint32_t px_battle_grounded_check(uint32_t context, uint32_t battler, uint32_t flag, const struct px_battle_core_4_api *api); /* 108651d8 */
uint32_t px_battle_is_airborne(uint32_t context, uint32_t id, const struct px_battle_core_4_api *api);                         /* 1084bf24 */
uint32_t px_battle_is_exempt(uint32_t context, uint32_t id, const struct px_battle_core_4_api *api);                           /* 1084af30 */
uint32_t px_battle_item_usable(uint32_t context, uint32_t id, const struct px_battle_core_4_api *api);                         /* 1084b5b8 */
uint32_t px_battle_has_flagged_condition(uint32_t context, uint32_t id, const struct px_battle_core_4_api *api);               /* 1084c030 */

/* Context and slot queries. */
uint32_t px_battle_group_table(uint32_t context, uint32_t slot, const struct px_battle_core_4_api *api);                       /* 1084b950: table + 28 group + 4 */
void px_battle_mark_group_flag(uint32_t object, uint32_t slot, uint32_t index, const struct px_battle_core_4_api *api);        /* 1084c3cc */
uint32_t px_battle_ring_copy(uint32_t context, uint32_t back, uint32_t skip, uint32_t destination, const struct px_pokemon_api *memory); /* 1084bac4 */
uint32_t px_battle_has_condition(uint32_t battler, uint32_t index, const struct px_battle_handler_api *api);                   /* 10856a88 */
uint32_t px_battle_range_width(uint32_t context, uint32_t index, const struct px_battle_handler_api *api);                     /* 1084d184 */
uint32_t px_battle_array_word_b(uint32_t array, uint32_t index, const struct px_pokemon_api *memory);                           /* 10876384 */
void px_battle_fill_6(uint32_t address, uint32_t value, const struct px_pokemon_api *memory);                                  /* 1086fd18 */
uint32_t px_battle_pack_7(uint32_t first, uint32_t second);                                                                    /* 1084beac */

/* Record builders. */
void px_battle_report_slot(uint32_t context, uint32_t id, const struct px_battle_core_4_api *api);                              /* 1084bd48 */
void px_battle_popup_message(uint32_t context, uint32_t id, uint32_t popup, uint32_t state, uint32_t message,
                             const struct px_battle_core_4_api *api);                                                          /* 1084a9d4 */

#endif
