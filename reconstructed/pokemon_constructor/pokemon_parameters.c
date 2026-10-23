#include "pokemon_constructor.h"

#include <string.h>

static uint16_t read16(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t read32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static void write16(uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
}

static void write32(uint8_t *p, uint32_t value)
{
    for (unsigned i = 0; i < 4; ++i)
        p[i] = (uint8_t)(value >> (8 * i));
}

static void write_pair(uint8_t *p, uint32_t low)
{
    write32(p, low);
    write32(p + 4, UINT32_C(0));
}

void px_normalize_pokemon_parameters(uint8_t *destination, const uint8_t *source,
                                      const struct px_parameter_api *api)
{
    /* 0x0014f32c..0x0014f350 copies all fourteen words, including padding.
     * Inputs cannot overlap; no service is called during this initial copy. */
    memcpy(destination, source, PX_POKEMON_PARAMETERS_SIZE);

    if (read32(destination) == UINT32_MAX &&
        read32(destination + 4) == UINT32_MAX) {
        const uint32_t value = api->random_word(api->user);
        write_pair(destination, value);
    }
    if (read32(destination + 0x10) == UINT32_MAX &&
        read32(destination + 0x14) == UINT32_MAX) {
        const uint32_t value = api->random_word(api->user);
        write_pair(destination + 0x10, value);
    }

    /* The mode pair is sampled once. The key at +0x10 is sampled after
     * random_word returns, so its side effects are visible to the next call. */
    const uint32_t candidate = read32(destination + 8);
    const uint32_t mode = read32(destination + 0x0c);
    if (candidate == UINT32_MAX && mode == 1) {
        const uint32_t random = api->random_word(api->user);
        const uint32_t key = read32(destination + 0x10);
        const uint32_t value = api->mode1_value(api->user, key, random);
        write_pair(destination + 8, value);
    } else if (candidate == UINT32_MAX && mode == 2) {
        const uint32_t random = api->random_word(api->user);
        const uint32_t key = read32(destination + 0x10);
        const uint32_t value = api->mode2_value(api->user, key, random);
        write_pair(destination + 8, value);
    } else if (candidate == UINT32_MAX && mode == 3 &&
               destination[0x23] != 0) {
        uint32_t attempts = 0;
        do {
            const uint32_t value = api->random_word(api->user);
            write_pair(destination + 8, value);
            const uint32_t key = read32(destination + 0x10);
            if (api->candidate_matches(api->user, key, value) != 0)
                break;
            ++attempts;
            /* Preserve the fresh budget read after the predicate callback. */
        } while (attempts < destination[0x23]);
    }

    if (read16(destination + 0x1c) > UINT16_C(100))
        write16(destination + 0x1c, UINT16_C(100));

    /* Literal 0x0014f6b4 is the full word 0x0000ffff. A nonzero count
     * always resets the six fields, even when an invalid count becomes zero. */
    if (destination[0x34] != 0) {
        if (destination[0x34] > 5)
            destination[0x34] = 0;
        for (unsigned i = 0; i < 6; ++i)
            write16(destination + 0x24 + 2 * i, UINT16_MAX);
        if (destination[0x34] != 0) {
            uint8_t selected = 0;
            do {
                uint8_t index;
                do {
                    index = (uint8_t)api->random_below(api->user, UINT32_C(6));
                } while (read16(destination + 0x24 + 2u * index) != UINT16_MAX);
                write16(destination + 0x24 + 2u * index, UINT16_C(31));
                selected = (uint8_t)(selected + 1u);
            } while (destination[0x34] > selected);
        }
    }
    for (unsigned i = 0; i < 6; ++i) {
        uint8_t *const field = destination + 0x24 + 2 * i;
        if (read16(field) == UINT16_MAX) {
            const uint32_t value = api->random_below(api->user, UINT32_C(32));
            write16(field, (uint16_t)value);
        }
    }

    const uint8_t selector = destination[0x22];
    if (selector == UINT8_C(0xff)) {
        destination[0x22] = (uint8_t)api->random_below(api->user, UINT32_C(2));
    } else if (selector == UINT8_C(0xfe)) {
        destination[0x22] = (uint8_t)api->random_below(api->user, UINT32_C(3));
    }
    if (read16(destination + 0x20) == UINT16_MAX) {
        const uint32_t value = api->random_below(api->user, UINT32_C(25));
        write16(destination + 0x20, (uint16_t)value);
    }

    if (destination[0x1a] == UINT8_C(0xff)) {
        api->select_species_base(api->user, read16(destination + 0x18));
        if (api->form_probe(api->user) != 0) {
            destination[0x1a] = 0;
        } else {
            api->select_species(api->user, read16(destination + 0x18), 0);
            const uint32_t bound =
                (uint8_t)api->personal_value(api->user, UINT32_C(0x20));
            destination[0x1a] = (uint8_t)api->random_below(api->user, bound);
        }
    }

    if (read16(destination + 0x1e) == UINT16_C(0xff)) {
        api->select_species(api->user, read16(destination + 0x18), destination[0x1a]);
        const uint32_t attribute = api->personal_value(api->user, UINT32_C(0x14));
        uint16_t selected = 0;
        if (attribute == UINT32_C(0xfe)) {
            selected = 1;
        } else if (attribute == UINT32_C(0xff)) {
            selected = 2;
        } else if (attribute != 0) {
            const uint32_t draw = api->random_below(api->user, UINT32_C(0xfc));
            /* ARM ADD wraps at 32 bits; comparison is unsigned and strict. */
            if (attribute > (uint32_t)(draw + UINT32_C(1)))
                selected = 1;
        }
        write16(destination + 0x1e, selected);
    }

    const uint32_t requested_gender = (uint8_t)read16(destination + 0x1e);
    const uint32_t form = destination[0x1a];
    const uint32_t species = read16(destination + 0x18);
    const uint32_t resolved = api->resolve_gender(api->user, species, form,
                                                 requested_gender);
    /* This full-width word is reloaded after resolve_gender, before writing
     * that callback's narrowed result to the earlier halfword. */
    const uint32_t trailing_value = read32(destination + 0x30);
    write16(destination + 0x1e, (uint16_t)resolved);
    if (trailing_value == UINT32_C(0xffff)) {
        api->select_species(api->user, read16(destination + 0x18), destination[0x1a]);
        const uint32_t value = api->personal_value(api->user, UINT32_C(0x16));
        write32(destination + 0x30, value);
    }
}
