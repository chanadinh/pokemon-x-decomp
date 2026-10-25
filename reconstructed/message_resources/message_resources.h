#ifndef PX_MESSAGE_RESOURCES_H
#define PX_MESSAGE_RESOURCES_H

#include "../pokemon_constructor/pokemon_constructor.h"

/* Analyst names; all pointers are logical ARM32 addresses. Resource callbacks
 * may mutate logical memory. Memory and all subsequently used objects must
 * remain valid. Archive position, allocation and freeing remain external. */
struct px_message_api {
    const struct px_pokemon_api *memory;
    void *user;
    void (*seek)(void *user, uint32_t archive, uint32_t member, uint32_t offset); /* 001381c4 */
    void (*read)(void *user, uint32_t archive, uint32_t size, uint32_t destination); /* 00127b6c */
    uint32_t (*alignment)(void *user, uint32_t heap); /* heap vtable +1c */
    uint32_t (*allocate)(void *user, uint32_t heap, uint32_t size,
                         uint32_t negative_alignment, uint32_t tail); /* 0012b0c8 */
    void (*release)(void *user, uint32_t buffer); /* 0013a50c */
};

/* 0014e984: count UTF-16 units, skipping 0010 tokens by count+2. The 2048
 * guard is checked after a complete token, so the result may exceed it.
 * All examined units must be mapped; the return value is full-width. */
uint32_t px_game_string_length(uint32_t source, const struct px_pokemon_api *memory);

/* 0013ff50: raw UTF-16 copy or index-keyed XOR/rotate decoding. String fields:
 * payload pointer +4, u16 capacity +8, u16 length +a, active byte +c.
 * Capacity includes the terminator. Overflow clears instead of truncating.
 * Control tokens are preserved as units; they are not interpreted here. */
void px_decode_message(uint32_t resource, uint32_t source, uint32_t units,
                        uint32_t index, uint32_t string, const struct px_pokemon_api *memory);

/* 0013a19c: cached or streamed loading, language selection, decode, and return
 * the entry's u16 metadata. Language cell: 005d113c. Allocation failure leaves
 * the previous string unchanged and still returns the entry metadata. */
uint32_t px_load_message(uint32_t resource, uint32_t index, uint32_t string,
                         const struct px_message_api *api);

#endif
