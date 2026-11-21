#ifndef PX_BATTLE_CORE_2_H
#define PX_BATTLE_CORE_2_H

#include "../battle_core_1/battle_core_1.h"
#include "../battle_tables_1/battle_tables_1.h"

/* DllBattle core battle-state helpers, second batch: 21 small functions in
 * 10840000-10880000 and 108a6000 and up that committed code already calls
 * (static analysis addresses; runtime addresses are module-relative). The
 * names are analyst interpretations. Layouts: see battle_core_1.h; in
 * addition the side-effect layer records at 10b11c00 (17 records of 16 bytes
 * per side; the word at +0 is the registered handler or 0), battler words
 * +0x1c + 4 i and bytes +0xb4 + i for i below 6, and context areas at
 * +0x120c (ten 3-byte entries), +0xdc0 (a pointer to a ten-entry log) and
 * +0x1244 (a pointer, a length and an inline default byte). */

/* Pure value helpers. */
uint32_t px_battle_is_kind1_0x78(uint32_t word);                                /* 1087a1c0 */
uint32_t px_battle_pack_4(uint32_t high, uint32_t low);                         /* 108797b0 */
uint32_t px_battle_double_plus(uint32_t flag, uint32_t value);                  /* 1084b664 */

/* Queries. */
uint32_t px_battle_four_halves_match(uint32_t base, uint32_t value, const struct px_pokemon_api *memory);   /* 108c8548 */
uint32_t px_battle_group_slot(uint32_t base, uint32_t group, uint32_t index, const struct px_pokemon_api *memory); /* 108c9a30 */
uint32_t px_battle_default_byte(uint32_t context, uint32_t index, const struct px_pokemon_api *memory);   /* 1084b590 */
uint32_t px_battle_has_entry_120c(uint32_t context, uint32_t value, const struct px_pokemon_api *memory);  /* 1084bfa0 */
uint32_t px_battle_has_set_id(uint32_t context, uint32_t table, const struct px_pokemon_api *memory);      /* 1084b804 */
uint32_t px_battle_half_in_array(uint32_t value, uint32_t array, uint32_t count, const struct px_pokemon_api *memory); /* 1086fe6c */
uint32_t px_battle_log_contains(uint32_t log, uint32_t id, uint32_t value, const struct px_pokemon_api *memory);       /* 108c9a64 */
uint32_t px_battle_find_word_index(uint32_t array, uint32_t value, const struct px_pokemon_api *memory);  /* 1086fc8c: -1 when absent */
uint32_t px_battle_side_record_set(uint32_t unused, uint32_t side, uint32_t id, const struct px_battle_tables_api *api); /* 108acedc */

/* Updates. */
void px_battle_set_field_3(uint32_t address, uint32_t value, const struct px_pokemon_api *memory);   /* 10876cf4 */
void px_battle_set_field_9(uint32_t address, uint32_t value, const struct px_pokemon_api *memory);   /* 1084b990 */
void px_battle_clear_slot_tables(uint32_t base, uint32_t index, const struct px_pokemon_api *memory); /* 1086fd30 */
void px_battle_clear_entries(uint32_t base, uint32_t first, const struct px_pokemon_api *memory);    /* 1086fd8c */
void px_battle_clear_tables_10(uint32_t base, const struct px_pokemon_api *memory);                  /* 10869870 */
void px_battle_push_log(uint32_t context, uint32_t first, uint32_t second, const struct px_pokemon_api *memory); /* 1084b3f8 */
/* 108ace4c and 108ace70 (with the shared tails 10876e04 and 10877490): clear or set flag
 * bit 25 of the handler a side record holds; 1 when the bit changed, 0 otherwise or without a handler. */
uint32_t px_battle_side_flag_clear(uint32_t side, uint32_t id, const struct px_battle_tables_api *api);
uint32_t px_battle_side_flag_set(uint32_t side, uint32_t id, const struct px_battle_tables_api *api);
/* 108ae5e0: removes every owner-0 handler registered for a battler's slot. */
void px_battle_remove_move_handlers(uint32_t pokemon, const struct px_battle_handler_api *api);

#endif
