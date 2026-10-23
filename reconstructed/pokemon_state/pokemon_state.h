#ifndef PX_POKEMON_STATE_H
#define PX_POKEMON_STATE_H

#include "state_fields.h"

enum { PX_POKEMON_NAME_SCRATCH_SIZE = 0x20 };

/* Address names avoid claiming recovered names or complete stat semantics. */
enum px_state_stat {
    PX_STATE_STAT_0014FB00,
    PX_STATE_STAT_0014FBD4,
    PX_STATE_STAT_0014FDA4,
    PX_STATE_STAT_0014FE78,
    PX_STATE_STAT_0014FA2C
};

struct px_state_api {
    const struct px_pokemon_api *memory;
    void *user;
    uint32_t (*level_from_growth)(void *user, uint32_t species,
                                  uint32_t form, uint32_t growth); /* 00168b3c */
    void (*select_learnset)(void *user, uint32_t species,
                            uint32_t form);                       /* 0014f2b8 */
    uint32_t (*learnset_count)(void *user);                        /* 0014f294 */
    uint32_t (*learnset_level)(void *user, uint32_t index);         /* 0014f2a4 */
    uint32_t (*learnset_move)(void *user, uint32_t index);          /* 0014f310 */
    void (*learn_move)(void *user, uint32_t object, uint32_t move);  /* 00150064 */
    void (*build_name)(void *user, uint8_t *scratch,
                        uint32_t species);                        /* 0014f224 */
    void (*write_name)(void *user, uint32_t manager,
                        const uint8_t *scratch);                  /* 001685c0 */
    uint32_t (*maximum_pp)(void *user, uint32_t move,
                           uint32_t pp_ups);                       /* 00150220 */
    void (*update_mode)(void *user, uint32_t object,
                         uint32_t mode);                          /* 0013aa84 */
    void (*update_stat)(void *user, uint32_t object,
                         enum px_state_stat stat);                /* enum address */
};

/* Names are analyst choices. Required callbacks and selected logical memory
 * must remain valid. Services may mutate logical memory; subsequent reads
 * preserve the ARM capture/reload order. Memory adapters are ordinary accessors.
 * Scratch is a caller-owned 0x20-byte host array, valid throughout the call and
 * disjoint from mutable logical-memory backing and API tables.
 * Its initial contents are unspecified and the name services determine which
 * bytes they initialize/use; this routine adds no blanket initialization. */
void px_initialize_pokemon_base_state(uint32_t object, uint8_t *name_scratch,
                                      const struct px_state_api *api);
void px_initialize_pokemon_extra_state(uint32_t manager,
                                       const struct px_state_api *api);
void px_update_pokemon_derived_state(uint32_t object, uint32_t mode,
                                     const struct px_state_api *api);
void px_refresh_pokemon_state(uint32_t object, const struct px_state_api *api);

#endif
