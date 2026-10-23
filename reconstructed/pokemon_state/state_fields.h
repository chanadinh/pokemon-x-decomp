#ifndef PX_STATE_FIELDS_H
#define PX_STATE_FIELDS_H

#include "../pokemon_data/pokemon_data.h"

/* Analyst names; original addresses are documented with each implementation. */
enum px_state_value {
    PX_STATE_SPECIES, PX_STATE_FORM, PX_STATE_GROWTH, PX_STATE_MAX_HP,
    PX_STATE_CURRENT_HP, PX_STATE_STATUS, PX_STATE_NAME_FLAG
};
enum px_state_slot { PX_STATE_MOVE, PX_STATE_PP_UPS, PX_STATE_PP };

uint32_t px_read_pokemon_state_value(uint32_t manager, enum px_state_value field,
                                     const struct px_pokemon_api *memory);
void px_write_pokemon_state_value(uint32_t manager, enum px_state_value field,
                                   uint32_t value, const struct px_pokemon_api *memory);
uint32_t px_read_pokemon_state_slot(uint32_t manager, uint32_t slot,
                                    enum px_state_slot field,
                                    const struct px_pokemon_api *memory);
void px_write_pokemon_state_slot(uint32_t manager, uint32_t slot,
                                  enum px_state_slot field, uint32_t value,
                                  const struct px_pokemon_api *memory);

#endif
