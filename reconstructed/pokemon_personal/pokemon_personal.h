#ifndef PX_POKEMON_PERSONAL_H
#define PX_POKEMON_PERSONAL_H

#include "../pokemon_constructor/pokemon_constructor.h"

enum { PX_PERSONAL_RECORD_SIZE = 12, PX_PERSONAL_PAYLOAD_SIZE = 64,
       PX_PERSONAL_SELECTOR_COUNT = 46 };

/* Analyst names; pointers are logical ARM32 addresses. Descriptor fields:
 * species u16 +4, form u8 +6, payload pointer u32 +8. The flat personal table
 * pointer is at 005d177c. Valid ordinary memory and initialized buffers are
 * required, including table entry zero for invalid species. */
void px_load_personal_record(uint32_t record, uint32_t species, uint32_t form,
                              const struct px_pokemon_api *memory); /* 00174c0c */
uint32_t px_personal_resource_field(uint32_t record, uint32_t selector,
                                    const struct px_pokemon_api *memory); /* 00175c78 */

/* 00167e2c / 00167e18: the current descriptor cell is 005d17a4. These wrappers
 * reload it on each call. Resource initialization and lifetime are external. */
void px_select_personal_resource(uint32_t species, uint32_t form,
                                 const struct px_pokemon_api *memory);
uint32_t px_current_personal_value(uint32_t selector, const struct px_pokemon_api *memory);

#endif
