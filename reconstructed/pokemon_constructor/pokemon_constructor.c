#include "pokemon_constructor.h"

static void write16(uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
}

static void write32(uint8_t *p, uint32_t value)
{
    for (unsigned i = 0; i < 4; ++i) p[i] = (uint8_t)(value >> (8 * i));
}

static void zero_words(uint32_t address, uint32_t size,
                       const struct px_pokemon_api *api)
{
    /* The call sites here always clear aligned, whole-word buffers. Physical
     * ARM store grouping inside 0x00301a9c is outside this host contract. */
    for (uint32_t offset = 0; offset < size; offset += UINT32_C(4))
        api->store32(api->user, address + offset, 0);
}

void px_default_pokemon_parameters(uint8_t *p)
{
    /* 0x0039a8a4's local initialization, before the complete parameter copy. */
    write32(p + 0x00, UINT32_MAX);
    write32(p + 0x04, UINT32_MAX);
    write32(p + 0x08, UINT32_MAX);
    write32(p + 0x0c, UINT32_C(3));
    write32(p + 0x10, UINT32_MAX);
    write32(p + 0x14, UINT32_MAX);
    write16(p + 0x20, UINT16_MAX);
    write16(p + 0x1c, UINT16_C(1));
    p[0x22] = UINT8_MAX;
    write16(p + 0x18, UINT16_C(0x2d2));
    p[0x23] = UINT8_C(1);
    p[0x1a] = UINT8_C(0);
    write16(p + 0x1e, UINT16_C(0xff));
    write32(p + 0x24, UINT32_C(0));
    write32(p + 0x28, UINT32_C(0));
    write32(p + 0x2c, UINT32_C(0));
    p[0x34] = UINT8_C(0);
    write32(p + 0x30, UINT32_C(0xffff));
    for (unsigned i = 0; i < 6; ++i)
        write16(p + 0x24 + 2 * i, UINT16_MAX);
}

uint32_t px_create_pokemon_data(uint32_t heap, const uint8_t *parameters,
                                uint8_t *normalized, const struct px_pokemon_api *api)
{
    px_default_pokemon_parameters(normalized);
    px_normalize_pokemon_parameters(normalized, parameters, api->parameters);
    const uint32_t data = api->allocate(api->user, PX_POKEMON_DATA_SIZE, heap);
    /* The restored ARM tail clears a successful allocation twice. It also
     * passes zero to the unconditional clear and population path on failure. */
    if (data != 0) zero_words(data, PX_POKEMON_DATA_SIZE, api);
    zero_words(data, PX_POKEMON_DATA_SIZE, api);
    api->populate_data(api->user, data, normalized);
    return data;
}

uint32_t px_initialize_pokemon_manager(uint32_t manager, const struct px_pokemon_api *api)
{
    /* 0x0014ff4c; the two final byte stores are present in the original. */
    api->store32(api->user, manager + UINT32_C(4), 0);
    api->store32(api->user, manager, UINT32_C(0x00598a78));
    api->store32(api->user, manager + UINT32_C(8), 0);
    api->store32(api->user, manager + UINT32_C(0x0c), 0);
    api->store8(api->user, manager + UINT32_C(0x0c), 0);
    api->store8(api->user, manager + UINT32_C(0x0d), 0);
    return manager;
}

void px_attach_pokemon_buffers(uint32_t manager, uint32_t data, uint32_t extra,
                               const struct px_pokemon_api *api)
{
    /* 0x0013a7a0 preserves the manager's vtable and bytes +0xe/+0xf. */
    api->store8(api->user, manager + UINT32_C(0x0c), 1);
    api->store32(api->user, manager + UINT32_C(8), data);
    api->store8(api->user, manager + UINT32_C(0x0d), 0);
    api->store32(api->user, manager + UINT32_C(4), extra);
}

uint32_t px_initialize_pokemon_base(uint32_t object, uint32_t heap,
                                    const uint8_t *parameters, uint8_t *normalized,
                                    const struct px_pokemon_api *api)
{
    api->store32(api->user, object + UINT32_C(4), 0);
    api->store32(api->user, object, UINT32_C(0x00598a88));
    api->store32(api->user, object + UINT32_C(8), 0);
    api->store32(api->user, object + UINT32_C(0x0c), 0);
    const uint32_t data = px_create_pokemon_data(heap, parameters, normalized, api);
    api->store32(api->user, object + UINT32_C(4), data);
    uint32_t manager = api->allocate(api->user, PX_POKEMON_MANAGER_SIZE, heap);
    if (manager != 0) manager = px_initialize_pokemon_manager(manager, api);
    api->store32(api->user, object + UINT32_C(0x0c), manager);
    const uint32_t current_data = api->load32(api->user, object + UINT32_C(4));
    px_attach_pokemon_buffers(manager, current_data, 0, api);
    api->initialize_base_state(api->user, object);
    return object;
}

void px_xor_pokemon_data(uint32_t address, uint32_t size, uint32_t key,
                         const struct px_pokemon_api *api)
{
    /* 0x00167fb4: odd trailing bytes are untouched; all arithmetic wraps32. */
    const uint32_t count = size >> 1;
    for (uint32_t i = 0; i < count; ++i) {
        key = key * UINT32_C(0x41c64e6d) + UINT32_C(0x6073);
        const uint32_t slot = address + i * UINT32_C(2);
        const uint16_t value = api->load16(api->user, slot);
        api->store16(api->user, slot, (uint16_t)(value ^ (uint16_t)(key >> 16)));
    }
}

uint32_t px_create_pokemon_extra(uint32_t heap, uint32_t key, const struct px_pokemon_api *api)
{
    const uint32_t extra = api->allocate(api->user, PX_POKEMON_EXTRA_SIZE, heap);
    if (extra != 0) zero_words(extra, PX_POKEMON_EXTRA_SIZE, api);
    zero_words(extra, PX_POKEMON_EXTRA_SIZE, api);
    px_xor_pokemon_data(extra, PX_POKEMON_EXTRA_SIZE, key, api);
    return extra;
}

static void acquire_manager(uint32_t manager, const struct px_pokemon_api *api)
{
    /* 0x0013a768 keeps this manager across the transform callback. */
    api->decode_manager(api->user, manager);
    api->store8(api->user, manager + UINT32_C(0x0d), 1);
    (void)api->load8(api->user, manager + UINT32_C(0x0c));
    /* Its conditional diagnostic targets the verified no-op 0x0014f220. */
}

static void restore_manager(uint32_t manager, const struct px_pokemon_api *api)
{
    api->store8(api->user, manager + UINT32_C(0x0d), 0);
    api->encode_manager(api->user, manager);
    if (api->load8(api->user, manager + UINT32_C(0x0d)) == 0)
        (void)api->load8(api->user, manager + UINT32_C(0x0c));
    /* Either failing check reaches the same returning no-op diagnostic. */
}

uint32_t px_acquire_pokemon_state(uint32_t object, const struct px_pokemon_api *api)
{
    uint32_t manager = api->load32(api->user, object + UINT32_C(0x0c));
    if (api->load8(api->user, manager + UINT32_C(0x0d)) != 0) return 0;
    manager = api->load32(api->user, object + UINT32_C(0x0c));
    acquire_manager(manager, api);
    return 1;
}

uint32_t px_restore_pokemon_state(uint32_t object, uint32_t mask,
                                  const struct px_pokemon_api *api)
{
    uint32_t manager = api->load32(api->user, object + UINT32_C(0x0c));
    const uint8_t byte = api->load8(api->user, manager + UINT32_C(0x0d));
    const uint32_t signed_word = (byte & UINT8_C(0x80)) != 0 ?
        UINT32_C(0xffffff00) | byte : (uint32_t)byte;
    if ((signed_word & mask) == 0) return 0;
    manager = api->load32(api->user, object + UINT32_C(0x0c));
    restore_manager(manager, api);
    return 1;
}

uint32_t px_construct_pokemon(uint32_t object, uint32_t heap,
                              const uint8_t *parameters, uint8_t *normalized,
                              const struct px_pokemon_api *api)
{
    object = px_initialize_pokemon_base(object, heap, parameters, normalized, api);
    api->store32(api->user, object, UINT32_C(0x00598a50));
    uint32_t manager = api->load32(api->user, object + UINT32_C(0x0c));
    const uint32_t data = api->load32(api->user, manager + UINT32_C(8));
    const uint32_t key = api->load32(api->user, data);
    const uint32_t extra = px_create_pokemon_extra(heap, key, api);
    api->store32(api->user, object + UINT32_C(8), extra);
    manager = api->load32(api->user, object + UINT32_C(0x0c));
    const uint32_t current_data = api->load32(api->user, object + UINT32_C(4));
    px_attach_pokemon_buffers(manager, current_data, extra, api);
    const uint32_t acquired = px_acquire_pokemon_state(object, api);
    manager = api->load32(api->user, object + UINT32_C(0x0c));
    api->initialize_extra_state(api->user, manager);
    api->update_derived_state(api->user, object, UINT32_C(1));
    api->refresh_state(api->user, object);
    (void)px_restore_pokemon_state(object, acquired, api);
    return object;
}
