#ifndef PX_ENCOUNTER_BUILDER_H
#define PX_ENCOUNTER_BUILDER_H

#include <stdint.h>

enum { PX_ENCOUNTER_SLOT_SIZE = 18, PX_ENCOUNTER_PARAMS_SIZE = 0x38 };

struct px_encounter_api {
    void *user;
    /* Logical base of the writable two-row, three-byte probability table. */
    uint32_t weight_table;
    uint8_t (*load8)(void *user, uint32_t address);
    uint32_t (*load32)(void *user, uint32_t address);
    void (*select_species)(void *user, uint32_t species, uint32_t form);
    uint32_t (*personal_value)(void *user, uint32_t selector);
    uint32_t (*context_index)(void *user, uint32_t context);
    uint32_t (*context_probe)(void *user, uint32_t context, uint32_t index);
    uint32_t (*allocate)(void *user, uint32_t size, uint32_t heap);
    uint32_t (*construct)(void *user, uint32_t storage, uint32_t heap,
                          const uint8_t *parameters);
    uint32_t (*random_below)(void *user, uint32_t bound);
    void (*set_value)(void *user, uint32_t object, uint32_t value);
    void (*append)(void *user, uint32_t destination, uint32_t object);
    void (*destroy_virtual)(void *user, uint32_t function, uint32_t object);
};

/* Request/slot/parameter storage must be valid and parameters must not overlap
 * inputs. Parameter holes are preserved. See README for callback contracts. */
void px_build_encounter(const uint8_t *request, uint32_t heap,
                         const uint8_t *slot, uint32_t destination,
                         uint8_t *parameters, const struct px_encounter_api *api);
uint32_t px_populate_encounters(const uint8_t *request, uint32_t heap,
                                uint32_t destination, uint8_t *parameters,
                                const struct px_encounter_api *api);
uint32_t px_select_encounter_config(const uint8_t *request,
                                     const struct px_encounter_api *api);

#endif
