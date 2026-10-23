#ifndef PX_WILD_BATTLE_SETUP_H
#define PX_WILD_BATTLE_SETUP_H

#include <stdint.h>

enum {
    PX_WILD_RECORD_SIZE = 12,
    /* Available space after the helper's 8-byte outgoing-argument area. */
    PX_WILD_WORKSPACE_SIZE = 0xac
};

/*
 * Game addresses are logical 32-bit addresses, never native host pointers.
 * Memory callbacks read/write ordinary little-endian game memory. get_record
 * supplies a stable, nonoverlapping 12-byte row for the FULL 32-bit index.
 * setup and submit remain unreconstructed game dependencies.
 */
struct px_wild_api {
    void *user;
    uint32_t (*load32)(void *user, uint32_t address);
    uint8_t (*load8)(void *user, uint32_t address);
    void (*store32)(void *user, uint32_t address, uint32_t value);
    const uint8_t *(*get_record)(void *user, uint32_t index);
    void (*setup)(void *user, uint8_t *workspace, uint32_t field,
                  uint32_t one, uint32_t optional, uint32_t ff,
                  uint32_t context_byte);
    uint32_t (*submit)(void *user, uint32_t manager, uint32_t field,
                       const uint8_t *workspace, uint32_t mode);
};

/* All pointers/callbacks and logical accesses must be valid. No error API is inferred. */
uint8_t *px_init_wild_workspace(uint8_t *workspace);
void px_patch_wild_record(uint8_t *workspace, const uint8_t *record);
uint32_t px_prepare_wild_battle(uint32_t field, uint32_t index, uint32_t mode,
                               uint8_t *workspace, const struct px_wild_api *api);
uint32_t px_finish_wild_script(uint32_t context, const struct px_wild_api *api);

#endif
