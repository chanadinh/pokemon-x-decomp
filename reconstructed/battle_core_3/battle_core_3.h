#ifndef PX_BATTLE_CORE_3_H
#define PX_BATTLE_CORE_3_H

#include "../battle_core_2/battle_core_2.h"

/* DllBattle core helpers, third batch: 19 small functions in 10840000-10880000
 * and 108a6000 and up (static analysis addresses; runtime addresses are
 * module-relative). The names are analyst interpretations.
 *
 * Packed words. Several helpers build or read a 32-bit word whose low 3
 * bits are a kind (1 to 4): kinds 1-3 keep a 16-bit field in bits 9-24 and
 * kind 4 in bits 15-30 (battle_core_1 and battle_core_2 rewrite the same
 * words). Records: a halfword id at +0, a count nibble in the high half of
 * byte +3 (up to 8 entries of one word each from +4). */

/* Packed words and records. */
void px_battle_pack_5(uint32_t address, uint32_t first, uint32_t second, uint32_t flag, const struct px_pokemon_api *memory); /* 10873610 */
void px_battle_pack_6(uint32_t address, uint32_t first, uint32_t second, const struct px_pokemon_api *memory);               /* 108704ac */
uint32_t px_battle_unpack_field(uint32_t word);                                                                          /* 10871408 */
void px_battle_push_record(uint32_t address, uint32_t value, const struct px_pokemon_api *memory);                         /* 10873fd8 */
void px_battle_init_record(uint32_t address, uint32_t nibble, uint32_t id, const struct px_pokemon_api *memory);           /* 10874010 */
void px_battle_copy_8(uint32_t destination, uint32_t source, const struct px_pokemon_api *memory);                         /* 108747fc */
void px_battle_copy_6(uint32_t destination, uint32_t source, const struct px_pokemon_api *memory);                         /* 1085fa8c */

/* Queries. */
uint32_t px_battle_pair_search(uint32_t context, uint32_t value, const struct px_pokemon_api *memory);                     /* 10870368 */
uint32_t px_battle_slot_group(uint32_t slot, const struct px_battle_handler_api *api);                                      /* 1087425c */
uint32_t px_battle_scaled_amount(uint32_t context, const struct px_pokemon_api *memory);                                    /* 108717dc */
uint32_t px_battle_free_of_conditions_2_3(uint32_t pokemon, const struct px_battle_handler_api *api);                       /* 1087198c */
uint32_t px_battle_usable_item(uint32_t pokemon, uint32_t context, const struct px_battle_handler_api *api);                /* 10873524 */
uint32_t px_battle_party_has_hp(uint32_t context, uint32_t out, const struct px_battle_handler_api *api);            /* 10870434 */
uint32_t px_battle_byte_16d(uint32_t context, uint32_t index, const struct px_battle_handler_api *api);                     /* 1084bb2c */
uint32_t px_battle_entry_274(uint32_t context, uint32_t index, const struct px_battle_handler_api *api);                    /* 1084c374 */
uint32_t px_battle_byte_11e(uint32_t context, uint32_t index, const struct px_battle_handler_api *api);                     /* 108520e4 */
uint32_t px_battle_half_11c(uint32_t context, uint32_t index, const struct px_battle_handler_api *api);                     /* 10855c68 */
uint32_t px_battle_is_side_0(uint32_t side, const struct px_battle_handler_api *api);                                       /* 1084af00 */

/* 108ace94: removes the handler a side record holds, clears the record and its kind bits; 1 when it held one. */
uint32_t px_battle_side_record_clear(uint32_t side, uint32_t id, const struct px_battle_tables_api *api);

#endif
