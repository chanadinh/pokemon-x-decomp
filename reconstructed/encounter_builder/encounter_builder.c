#include "encounter_builder.h"

#include <stddef.h>
#include <string.h>

static uint32_t get16(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8);
}

static uint32_t get32(const uint8_t *p)
{
    return get16(p) | (get16(p + 2) << 16);
}

static void put16(uint8_t *p, uint32_t value)
{
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
}

static void put32(uint8_t *p, uint32_t value)
{
    put16(p, value);
    put16(p + 2, value >> 16);
}

static void init_parameters(uint8_t *p)
{
    put32(p, UINT32_MAX);
    put32(p + 4, UINT32_MAX);
    put32(p + 8, UINT32_MAX);
    put32(p + 0x0c, UINT32_C(3));
    put32(p + 0x10, UINT32_MAX);
    put32(p + 0x14, UINT32_MAX);
    put16(p + 0x18, UINT32_C(0x2d2));
    p[0x1a] = 0;
    put16(p + 0x1c, UINT32_C(1));
    put16(p + 0x1e, UINT32_C(0xff));
    put16(p + 0x20, UINT32_C(0xffff));
    p[0x22] = UINT8_C(0xff);
    p[0x23] = UINT8_C(1);
    memset(p + 0x24, 0xff, 12);
    put32(p + 0x30, UINT32_C(0xffff));
    p[0x34] = 0;
    /* Original byte +0x1b and bytes +0x35..0x37 are untouched. */
}

static uint32_t choose_value(const uint8_t *request, const struct px_encounter_api *api)
{
    uint32_t values[3];
    for (uint32_t i = 0; i < 3; ++i) {
        values[i] = api->personal_value(api->user, UINT32_C(0x11) + i) & UINT32_C(0xffff);
    }
    if (values[0] == values[1]) {
        return values[0];
    }
    const uint32_t draw = api->random_below(api->user, UINT32_C(100)) & UINT32_C(0xff);
    /* Read the flag and writable weights AFTER the random callback. */
    const uint32_t row = (get32(request + 0xa4) >> 9) & UINT32_C(1);
    const uint32_t table = api->weight_table + row * UINT32_C(3);
    uint32_t threshold = api->load8(api->user, table);
    if (draw < threshold) {
        return values[0];
    }
    threshold = (threshold + api->load8(api->user, table + UINT32_C(1))) & UINT32_C(0xff);
    if (draw < threshold) {
        return values[1];
    }
    threshold = (threshold + api->load8(api->user, table + UINT32_C(2))) & UINT32_C(0xff);
    return draw < threshold ? values[2] : UINT32_C(0);
}

void px_build_encounter(const uint8_t *request, uint32_t heap,
                         const uint8_t *slot, uint32_t destination,
                         uint8_t *parameters, const struct px_encounter_api *api)
{
    init_parameters(parameters);
    const uint32_t species = get16(slot);
    put16(parameters + 0x18, species);
    put16(parameters + 0x1c, slot[4]);
    const uint32_t form = slot[5];
    parameters[0x1a] = (uint8_t)form;
    put32(parameters + 0x10, get32(request + 0x9c));
    put32(parameters + 0x14, UINT32_C(0));
    /* Both registers are consumed by main 0x00167e2c. */
    api->select_species(api->user, species, form);

    if (slot[0x10] == 1) {
        put16(parameters + 0x1e, UINT32_C(0));
    } else if (slot[0x10] == 2) {
        put16(parameters + 0x1e, UINT32_C(1));
    }
    if (slot[6] == 1 || slot[6] == 2) {
        put32(parameters + 8, UINT32_MAX);
        put32(parameters + 0x0c, slot[6] == 1 ? UINT32_C(2) : UINT32_C(1));
    } else {
        uint32_t value = UINT32_C(1) + ((get32(request + 0xa4) >> 3) & UINT32_C(2));
        parameters[0x23] = (uint8_t)value;
        if (request[0x85] == 2) {
            value += UINT32_C(4);
            parameters[0x23] = (uint8_t)value;
        }
        if ((get32(request + 0xa4) & UINT32_C(2)) != 0) {
            value = parameters[0x23] + (get16(request + 0xa2) << 1);
            parameters[0x23] = (uint8_t)value;
        }
    }

    const uint32_t attribute_18 = api->personal_value(api->user, UINT32_C(0x18));
    if (attribute_18 == UINT32_C(15) ||
        api->personal_value(api->user, UINT32_C(0x19)) == UINT32_C(15)) {
        parameters[0x34] = UINT8_C(3);
    } else {
        parameters[0x34] = request[0x85] == 2 ? UINT8_C(2) : slot[0x11];
    }
    if (slot[7] >= 1 && slot[7] <= 3) {
        parameters[0x22] = (uint8_t)(slot[7] - 1);
    }
    if (request[0x85] == 2) {
        const uint32_t context = get32(request + 0x78);
        const uint32_t index = api->context_index(api->user, context);
        const uint32_t result = api->context_probe(api->user, context, index);
        parameters[0x22] = result == 0 ? UINT8_C(0xff) : UINT8_C(0xfe);
    }
    if ((get32(request + 0xa4) & UINT32_C(0x400)) != 0) {
        put16(parameters + 0x20, request[0x97]);
    }

    const uint32_t storage = api->allocate(api->user, UINT32_C(16), heap);
    const uint32_t object = storage == 0 ? UINT32_C(0) :
        api->construct(api->user, storage, heap, parameters);
    /* The slot value is sampled after construction, as in the ARM code. */
    uint32_t selected = get16(slot + 2);
    if (selected <= UINT32_C(0x2cc)) {
        if (selected == 0) {
            selected = choose_value(request, api);
        }
        api->set_value(api->user, object, selected);
    }
    api->append(api->user, destination, object);
    if (object != 0) {
        /* Read the virtual target after append, rather than caching it early. */
        const uint32_t vtable = api->load32(api->user, object);
        const uint32_t function = api->load32(api->user, vtable + UINT32_C(8));
        api->destroy_virtual(api->user, function, object);
    }
}

uint32_t px_populate_encounters(const uint8_t *request, uint32_t heap,
                                uint32_t destination, uint8_t *parameters,
                                const struct px_encounter_api *api)
{
    uint32_t count = get32(request + 0x6c);
    if (count == 0) {
        /* The original diagnostic resolves to a returning no-op. */
        return UINT32_C(0);
    }
    if (count < UINT32_C(0x80000000)) {
        uint32_t i = 0;
        do {
            px_build_encounter(request, heap, request + (size_t)i * PX_ENCOUNTER_SLOT_SIZE,
                                 destination, parameters, api);
            count = get32(request + 0x6c);
            ++i;
        } while (count < UINT32_C(0x80000000) && count > i);
    }
    return UINT32_C(1);
}

uint32_t px_select_encounter_config(const uint8_t *request,
                                     const struct px_encounter_api *api)
{
    if (request[0x8e] != UINT8_C(0x40)) {
        return request[0x8e];
    }
    const uint32_t object = get32(request + 0x88);
    if (object == 0) {
        return UINT32_C(0);
    }
    return api->load8(api->user, object + (request[0x84] == 10 ? UINT32_C(5) : UINT32_C(4)));
}
