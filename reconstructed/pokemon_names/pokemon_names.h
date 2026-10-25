#ifndef PX_POKEMON_NAMES_H
#define PX_POKEMON_NAMES_H

#include "../pokemon_data/pokemon_data.h"

struct px_name_api {
    const struct px_pokemon_api *memory;
    void *user;
    /* 0013a19c: load a message into the captured string descriptor.
     * The resource result is returned unchanged by the name builder.
     * Resource services may mutate logical memory, including globals. */
    uint32_t (*load_text)(void *user, uint32_t resource, uint32_t index,
                           uint32_t string);
};

/* 001679f4: shared UTF-16/control-token string copy, not a bounded Pokémon
 * field writer. Both logical regions must accommodate all copied units.
 * A 0010 token copies its count+2 units plus the following unit before the
 * 0800-unit guard is tested. A guard exit appends zero and returns the
 * previous counter. See README for exact limits and overlap contract. */
uint32_t px_copy_game_string(uint32_t destination, uint32_t source,
                             const struct px_pokemon_api *memory);

/* 00167c38: species/form index using the personal table at 005d1778+4.
 * Form zero and invalid species do not access memory (memory may be NULL). */
uint32_t px_pokemon_form_index(uint32_t species, uint32_t form,
                               const struct px_pokemon_api *memory);

/* 0014f224: text/string globals at 005d1790+0/+4. Species >=724 skips
 * loading, returns zero and still copies the currently selected string.
 * The destination is logical memory; callers provide sufficient storage. */
uint32_t px_build_pokemon_name(uint32_t destination, uint32_t species,
                                const struct px_name_api *api);

/* 001685c0: concrete decode, writable block B, shared copy, encode tail.
 * This routine does not set or clear the nickname flag. A valid Pokémon
 * name must fit before the move slots; the shared copy adds no field bound. */
void px_write_pokemon_name(uint32_t manager, uint32_t source,
                            const struct px_pokemon_api *memory);

#endif
