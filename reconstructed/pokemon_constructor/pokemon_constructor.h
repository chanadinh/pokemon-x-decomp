#ifndef PX_POKEMON_CONSTRUCTOR_H
#define PX_POKEMON_CONSTRUCTOR_H

#include <stdint.h>

enum {
    PX_POKEMON_OBJECT_SIZE = 0x10,
    PX_POKEMON_DATA_SIZE = 0xe8,
    PX_POKEMON_MANAGER_SIZE = 0x10,
    PX_POKEMON_EXTRA_SIZE = 0x1c,
    PX_POKEMON_PARAMETERS_SIZE = 0x38
};

/* Service names describe observed roles, not recovered original symbols. */
struct px_parameter_api {
    void *user;
    uint32_t (*random_word)(void *user);                         /* 00167c0c */
    uint32_t (*random_below)(void *user, uint32_t bound);        /* 00167bdc */
    uint32_t (*mode1_value)(void *user, uint32_t key, uint32_t value); /* 001689a4 */
    uint32_t (*mode2_value)(void *user, uint32_t key, uint32_t value); /* 00168944 */
    uint32_t (*candidate_matches)(void *user, uint32_t key, uint32_t value); /* 001688a4 */
    void (*select_species_base)(void *user, uint32_t species);   /* 00167ee4 */
    uint32_t (*form_probe)(void *user);                        /* 00167f44 */
    void (*select_species)(void *user, uint32_t species, uint32_t form); /* 00167e2c */
    uint32_t (*personal_value)(void *user, uint32_t selector);   /* 00167e18 */
    uint32_t (*resolve_gender)(void *user, uint32_t species,
                               uint32_t form, uint32_t value); /* 00168aa0 */
};

struct px_pokemon_api {
    void *user;
    const struct px_parameter_api *parameters;
    /* Logical ARM32 addresses. Memory adapters perform ordinary reads/writes. */
    uint8_t (*load8)(void *user, uint32_t address);
    uint16_t (*load16)(void *user, uint32_t address);
    uint32_t (*load32)(void *user, uint32_t address);
    void (*store8)(void *user, uint32_t address, uint8_t value);
    void (*store16)(void *user, uint32_t address, uint16_t value);
    void (*store32)(void *user, uint32_t address, uint32_t value);
    uint32_t (*allocate)(void *user, uint32_t size, uint32_t heap); /* 00122920 */
    void (*populate_data)(void *user, uint32_t data,
                           const uint8_t *parameters);          /* 0014f6b8 */
    void (*initialize_base_state)(void *user, uint32_t object); /* 0013a8d8 */
    void (*decode_manager)(void *user, uint32_t manager);       /* 00168660 */
    void (*encode_manager)(void *user, uint32_t manager);       /* 001685f0 */
    void (*initialize_extra_state)(void *user, uint32_t manager); /* 0012b744 */
    void (*update_derived_state)(void *user, uint32_t object, uint32_t mode); /* 0012b808 */
    void (*refresh_state)(void *user, uint32_t object);         /* 0013a7bc */
};

/* Valid nonoverlapping 0x38-byte arrays are required. Services may mutate the
 * destination through their user context; the source is copied before calls.
 * Bounded-RNG callbacks must provide valid indices, and retry loops must make
 * progress. See README for the precise service and mutation contract. */
void px_normalize_pokemon_parameters(uint8_t *destination, const uint8_t *source,
                                      const struct px_parameter_api *api);
void px_default_pokemon_parameters(uint8_t *parameters);
uint32_t px_create_pokemon_data(uint32_t heap, const uint8_t *parameters,
                                uint8_t *normalized, const struct px_pokemon_api *api);
uint32_t px_initialize_pokemon_base(uint32_t object, uint32_t heap,
                                    const uint8_t *parameters, uint8_t *normalized,
                                    const struct px_pokemon_api *api);
uint32_t px_construct_pokemon(uint32_t object, uint32_t heap,
                              const uint8_t *parameters, uint8_t *normalized,
                              const struct px_pokemon_api *api);
uint32_t px_initialize_pokemon_manager(uint32_t manager, const struct px_pokemon_api *api);
void px_attach_pokemon_buffers(uint32_t manager, uint32_t data, uint32_t extra,
                               const struct px_pokemon_api *api);
uint32_t px_create_pokemon_extra(uint32_t heap, uint32_t key, const struct px_pokemon_api *api);
void px_xor_pokemon_data(uint32_t address, uint32_t size, uint32_t key,
                         const struct px_pokemon_api *api);
uint32_t px_acquire_pokemon_state(uint32_t object, const struct px_pokemon_api *api);
uint32_t px_restore_pokemon_state(uint32_t object, uint32_t mask,
                                  const struct px_pokemon_api *api);

#endif
