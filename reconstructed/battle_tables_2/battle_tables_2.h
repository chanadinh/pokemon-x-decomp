#ifndef PX_BATTLE_TABLES_2_H
#define PX_BATTLE_TABLES_2_H

#include "../battle_tables_1/battle_tables_1.h"

/* DllBattle move-table functions (static analysis addresses; runtime addresses
 * are module-relative). The move table (108d3ddc) has 296 rows of 8 bytes: a
 * 16-bit move id and a table function. The rows name 248 distinct table
 * functions, all regular four-instruction stubs of the form reconstructed in
 * battle_tables_1: store a count word (priority << 16 | count) through the
 * first argument and return the entries table. The move registrar
 * (10869dd4, px_battle_register_move) is in the battle_handler milestone, and
 * the move handlers are in the move-batch milestones. */
uint32_t px_battle_move_table_function(uint32_t count_out, uint32_t count_word, uint32_t entries,
                                       const struct px_pokemon_api *memory);

#endif
