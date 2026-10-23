#include "pokemon_data.h"

uint16_t px_pokemon_checksum(uint32_t address, uint32_t size,
                             const struct px_pokemon_api *memory)
{
    /* 0x00174e0c sums complete little-endian halfwords modulo 2^16.
     * Its paired accumulators have the same result and ascending load order;
     * an odd final byte is never read. */
    const uint32_t count = size >> 1;
    uint16_t sum = 0;
    for (uint32_t i = 0; i < count; ++i) {
        const uint16_t value =
            memory->load16(memory->user, address + i * UINT32_C(2));
        sum = (uint16_t)((uint32_t)sum + value);
    }
    return sum;
}

void px_encode_pokemon_manager(uint32_t manager,
                                const struct px_pokemon_api *memory)
{
    /* 0x001685f0 does not even read +0xd when +0xc is nonzero. */
    if (memory->load8(memory->user, manager + UINT32_C(0x0c)) != 0)
        return;
    if (memory->load8(memory->user, manager + UINT32_C(0x0d)) != 0)
        return;

    uint32_t data = memory->load32(memory->user, manager + UINT32_C(8));
    const uint16_t checksum =
        px_pokemon_checksum(data + UINT32_C(8), UINT32_C(0xe0), memory);
    data = memory->load32(memory->user, manager + UINT32_C(8));
    memory->store16(memory->user, data + UINT32_C(6), checksum);

    /* The payload pointer is reloaded again after the checksum store. */
    data = memory->load32(memory->user, manager + UINT32_C(8));
    uint32_t key = memory->load32(memory->user, data);
    px_xor_pokemon_data(data + UINT32_C(8), UINT32_C(0xe0), key, memory);

    const uint32_t extra = memory->load32(memory->user, manager + UINT32_C(4));
    if (extra != 0) {
        data = memory->load32(memory->user, manager + UINT32_C(8));
        key = memory->load32(memory->user, data);
        px_xor_pokemon_data(extra, UINT32_C(0x1c), key, memory);
    }
    memory->store8(memory->user, manager + UINT32_C(0x0c), UINT8_C(1));
}

void px_decode_pokemon_manager(uint32_t manager,
                                const struct px_pokemon_api *memory)
{
    /* 0x00168660 checks only +0xc; the acquisition flag at +0xd is irrelevant. */
    if (memory->load8(memory->user, manager + UINT32_C(0x0c)) == 0)
        return;

    uint32_t data = memory->load32(memory->user, manager + UINT32_C(8));
    uint32_t key = memory->load32(memory->user, data);
    /* Reviewed 0x00174e6c uses the same key recurrence and XOR primitive. */
    px_xor_pokemon_data(data + UINT32_C(8), UINT32_C(0xe0), key, memory);

    const uint32_t extra = memory->load32(memory->user, manager + UINT32_C(4));
    if (extra != 0) {
        data = memory->load32(memory->user, manager + UINT32_C(8));
        key = memory->load32(memory->user, data);
        px_xor_pokemon_data(extra, UINT32_C(0x1c), key, memory);
    }
    /* The manager is marked decoded before its checksum is checked. */
    memory->store8(memory->user, manager + UINT32_C(0x0c), UINT8_C(0));
    data = memory->load32(memory->user, manager + UINT32_C(8));
    const uint16_t stored_checksum = memory->load16(memory->user, data + UINT32_C(6));
    const uint16_t actual_checksum =
        px_pokemon_checksum(data + UINT32_C(8), UINT32_C(0xe0), memory);
    if (stored_checksum != actual_checksum) {
        /* Corruption is marked on the current data pointer. A matching
         * checksum never clears a previously set corruption bit. */
        data = memory->load32(memory->user, manager + UINT32_C(8));
        const uint16_t flags = memory->load16(memory->user, data + UINT32_C(4));
        memory->store16(memory->user, data + UINT32_C(4),
                          (uint16_t)(flags | UINT16_C(4)));
    }
}

void px_bind_pokemon_data(uint32_t manager, uint32_t data, uint32_t extra,
                          const struct px_pokemon_api *memory)
{
    /* 0x00168548 tail-calls encode after these stores in this order. */
    memory->store32(memory->user, manager + UINT32_C(8), data);
    memory->store8(memory->user, manager + UINT32_C(0x0c), UINT8_C(0));
    memory->store32(memory->user, manager + UINT32_C(4), extra);
    memory->store8(memory->user, manager + UINT32_C(0x0d), UINT8_C(0));
    px_encode_pokemon_manager(manager, memory);
}

uint32_t px_pokemon_block(uint32_t manager, uint32_t block, uint32_t write_mode,
                          const struct px_pokemon_api *memory)
{
    /* 0x0016b358/0x0016b3d4/0x00176168/0x001761e8 correspond to
     * logical blocks 0..3. The caller must supply one of those four values. */
    uint32_t data = memory->load32(memory->user, manager + UINT32_C(8));
    const uint16_t flags = memory->load16(memory->user, data + UINT32_C(4));
    if ((flags & UINT16_C(4)) != 0) {
        /* Verified literal addresses, not embedded fallback block contents. */
        const uint32_t fallback = write_mode != 0 ?
            UINT32_C(0x0064839c) : UINT32_C(0x006482a0);
        return fallback + block * UINT32_C(0x38);
    }

    const uint32_t key = memory->load32(memory->user, data);
    const uint32_t permutation = (key & UINT32_C(0x3e000)) >> 13;
    /* The ARM diagnostic for permutation >=32 is unreachable after the mask.
     * Reload the data pointer before reading the table, as the original does. */
    data = memory->load32(memory->user, manager + UINT32_C(8));
    const uint32_t physical_block = memory->load8(memory->user,
        UINT32_C(0x00570eec) + permutation * UINT32_C(4) + block);
    return data + UINT32_C(8) + physical_block * UINT32_C(0x38);
}
