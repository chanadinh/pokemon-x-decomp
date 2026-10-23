#ifndef PX_POKEMON_DATA_H
#define PX_POKEMON_DATA_H

#include "../pokemon_constructor/pokemon_constructor.h"

/* Names describe observed roles; they are not recovered original symbols. */
enum px_pokemon_field {
    PX_DATA_KEY, PX_DATA_WORD_10, PX_DATA_WORD_08, PX_DATA_SPECIES,
    PX_DATA_FORM, PX_DATA_EXTRA_LEVEL, PX_DATA_NATURE, PX_DATA_GENDER,
    PX_DATA_LEVEL, PX_DATA_ZERO_BYTE, PX_DATA_PARAM_30,
    PX_DATA_SETTING_68, PX_DATA_SETTING_69, PX_DATA_SETTING_6A, PX_DATA_SETTING_6B,
    PX_DATA_STAT_24, PX_DATA_STAT_26, PX_DATA_STAT_28,
    PX_DATA_STAT_2A, PX_DATA_STAT_2C, PX_DATA_STAT_2E,
    PX_DATA_ABILITY, PX_DATA_ABILITY_FLAG_0, PX_DATA_ABILITY_FLAG_1,
    PX_DATA_ABILITY_FLAG_2, PX_DATA_GROWTH
};

struct px_data_api {
    const struct px_pokemon_api *memory;
    void *user;
    /* 00167db0: select/load the real growth resource; remains external. */
    void (*select_growth)(void *user, uint32_t species, uint32_t form);
};

uint16_t px_pokemon_checksum(uint32_t address, uint32_t size,
                             const struct px_pokemon_api *memory);
void px_encode_pokemon_manager(uint32_t manager, const struct px_pokemon_api *memory);
void px_decode_pokemon_manager(uint32_t manager, const struct px_pokemon_api *memory);
void px_bind_pokemon_data(uint32_t manager, uint32_t data, uint32_t extra,
                          const struct px_pokemon_api *memory);
/* block is 0..3; write_mode is tested for zero versus nonzero. */
uint32_t px_pokemon_block(uint32_t manager, uint32_t block, uint32_t write_mode,
                          const struct px_pokemon_api *memory);
void px_set_pokemon_field(uint32_t manager, enum px_pokemon_field field,
                          uint32_t value, const struct px_pokemon_api *memory);
uint32_t px_resolve_pokemon_ability(uint32_t species, uint32_t form, uint32_t slot,
                                   const struct px_data_api *api);
uint32_t px_pokemon_growth_value(uint32_t level, const struct px_data_api *api);
/* Scratch manager is a caller-owned logical 0x10-byte region, disjoint from
 * parameters and data. This void API omits the ignored expired-stack return
 * token of 0014f6b8. Required memory/resources and callbacks must be valid. */
void px_populate_pokemon_data(uint32_t data, const uint8_t *parameters,
                              uint32_t scratch_manager, const struct px_data_api *api);

#endif
