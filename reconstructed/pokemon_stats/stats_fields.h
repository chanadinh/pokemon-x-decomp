#ifndef PX_STATS_FIELDS_H
#define PX_STATS_FIELDS_H

#include "../pokemon_state/pokemon_state.h"

enum px_stat {
    PX_STAT_HP, PX_STAT_ATTACK, PX_STAT_DEFENSE, PX_STAT_SPEED,
    PX_STAT_SPECIAL_ATTACK, PX_STAT_SPECIAL_DEFENSE
};

uint32_t px_read_pokemon_iv(uint32_t manager, enum px_stat stat,
                            const struct px_pokemon_api *memory);
uint32_t px_read_pokemon_ev(uint32_t manager, enum px_stat stat,
                            const struct px_pokemon_api *memory);
uint32_t px_read_pokemon_nature(uint32_t manager, const struct px_pokemon_api *memory);
uint32_t px_pokemon_has_extra(uint32_t manager, const struct px_pokemon_api *memory);
uint32_t px_read_pokemon_current_hp(uint32_t manager, const struct px_pokemon_api *memory);
uint32_t px_read_pokemon_extra_level(uint32_t manager, const struct px_pokemon_api *memory);
void px_write_pokemon_stat(uint32_t manager, enum px_stat stat, uint32_t value,
                            const struct px_pokemon_api *memory);

#endif
