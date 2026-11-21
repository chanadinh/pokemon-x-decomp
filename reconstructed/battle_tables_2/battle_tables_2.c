#include "battle_tables_2.h"

uint32_t px_battle_move_table_function(uint32_t count_out, uint32_t count_word, uint32_t entries,
                                       const struct px_pokemon_api *memory)
{
    memory->store32(memory->user, count_out, count_word);
    return entries;
}
