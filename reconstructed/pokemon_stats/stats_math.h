#ifndef PX_STATS_MATH_H
#define PX_STATS_MATH_H

#include "stats_fields.h"

uint32_t px_pokemon_hp_value(uint32_t species, uint32_t level, uint32_t base,
                             uint32_t iv, uint32_t ev);
/* stat_index is the original nature column argument (1..5); other full-width
 * values return value without reading the table. Nature is not range-clamped. */
uint32_t px_apply_pokemon_nature(uint32_t value, uint32_t nature, uint32_t stat_index,
                                 const struct px_pokemon_api *memory);
uint32_t px_pokemon_stat_value(uint32_t level, uint32_t base, uint32_t iv,
                               uint32_t ev, uint32_t nature, enum px_stat stat,
                               const struct px_pokemon_api *memory);
uint32_t px_pokemon_level_from_growth(uint32_t species, uint32_t form, uint32_t growth,
                                      const struct px_data_api *api);

#endif
