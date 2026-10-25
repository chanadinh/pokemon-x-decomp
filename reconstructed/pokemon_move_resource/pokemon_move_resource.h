#ifndef PX_POKEMON_MOVE_RESOURCE_H
#define PX_POKEMON_MOVE_RESOURCE_H

#include "../pokemon_constructor/pokemon_constructor.h"

enum { PX_MOVE_RESOURCE_COUNT = 618, PX_MOVE_RESOURCE_PAYLOAD_SIZE = 36 };

struct px_move_resource_api {
    const struct px_pokemon_api *memory;
    void *user;
    void (*read_member)(void *user, uint32_t archive, uint32_t index,
                        uint32_t destination); /* 00138214, external */
};

/* Analyst names. Addresses are logical ARM32 addresses. The descriptor's
 * payload pointer is at +8. Valid mapped ordinary memory is required.
 * Signed fields are returned as their 32-bit ARM r0 bit pattern. */
uint32_t px_move_resource_field(uint32_t record, uint32_t selector,
                                const struct px_pokemon_api *memory);

/* Record: vtable +0, move u16 +4, payload pointer u32 +8. Copies preserve
 * descriptor pointers and padding. Payloads must be disjoint or identical,
 * and disjoint from descriptors/cache metadata; ARM store grouping omitted. */
void px_copy_move_resource(uint32_t source, uint32_t destination,
                            const struct px_pokemon_api *memory);
/* Cache: pointer table u32 +4, capacity u8 +8, cursor u8 +9. Store requires
 * positive capacity, cursor < capacity, valid records and backing buffers. */
uint32_t px_find_cached_move_resource(uint32_t cache, uint32_t move,
                                      uint32_t destination, const struct px_pokemon_api *memory);
void px_store_cached_move_resource(uint32_t cache, uint32_t source,
                                    const struct px_pokemon_api *memory);
/* IDs >=618 load record zero. The archive callback may mutate logical memory;
 * required memory/callbacks must remain valid. Archive cell: 005e5338. */
void px_load_move_resource(uint32_t record, uint32_t move,
                            const struct px_move_resource_api *api);
/* 00168bbc: selected/cache cells 005d188c/005d1890. The cache search compares
 * the full-width requested ID; only the loader normalizes it. */
uint32_t px_get_move_resource_value(uint32_t move, uint32_t selector,
                                    const struct px_move_resource_api *api);

#endif
