#ifndef PX_POKEMON_STATS_H
#define PX_POKEMON_STATS_H

#include "stats_math.h"

/* 00168ae4 and 0015306c. With an auxiliary buffer these read its cached
 * values; otherwise they calculate from freshly selected main-data inputs. */
uint32_t px_pokemon_object_level(uint32_t object, const struct px_data_api *api);
uint32_t px_pokemon_object_max_hp(uint32_t object, const struct px_data_api *api);

/* Reconstructs 0x0013aa84. The full mode word is tested for zero/nonzero.
 * Personal and growth resources use the existing data/parameter APIs. */
void px_update_pokemon_hp(uint32_t object, uint32_t mode,
                          const struct px_data_api *api);

/* Reconstructs the five address-identified stat updates in px_state_stat,
 * including their fallthrough auxiliary-field setter tails. Invalid host enum
 * values return without memory or resource access. */
void px_update_pokemon_stat(uint32_t object, enum px_state_stat stat,
                            const struct px_data_api *api);

#endif
