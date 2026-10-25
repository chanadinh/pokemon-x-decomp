#include "pokemon_names.h"

uint32_t px_copy_game_string(uint32_t destination, uint32_t source,
                             const struct px_pokemon_api *m)
{
    uint32_t count = 0;
    for (;;) {
        const uint16_t unit = m->load16(m->user, source);
        m->store16(m->user, destination, unit);
        if (unit == UINT16_C(0x10)) {
            const uint32_t span = (uint32_t)m->load16(m->user, source + UINT32_C(2)) + UINT32_C(2);
            /* The ARM odd prefix and paired halfword loop copy the rest
             * of the token AND its following unit, in ascending order. */
            for (uint32_t i = 1; i <= span; ++i)
                m->store16(m->user, destination + i * UINT32_C(2),
                            m->load16(m->user, source + i * UINT32_C(2)));
            source += span * UINT32_C(2);
            destination += span * UINT32_C(2);
            count += span;
        }
        if (m->load16(m->user, destination) == 0) return count;
        ++count;
        destination += UINT32_C(2);
        source += UINT32_C(2);
        if (count >= UINT32_C(0x800)) {
            m->store16(m->user, destination, 0);
            return count - UINT32_C(1);
        }
    }
}

uint32_t px_pokemon_form_index(uint32_t species, uint32_t form, const struct px_pokemon_api *m)
{
    if (species == UINT32_C(0x2d2) || species == UINT32_C(0x2d3) || species > UINT32_C(0x2d1)) return 0;
    if (form == 0) return species;
    const uint32_t table = m->load32(m->user, UINT32_C(0x005d177c));
    const uint32_t entry = table + species * UINT32_C(0x40);
    /* Original word reads, including when the first-index halfword is zero. */
    const uint32_t first = m->load32(m->user, entry + UINT32_C(0x1c)) & UINT32_C(0xffff);
    const uint32_t word = m->load32(m->user, entry + UINT32_C(0x20));
    if (first != 0 && (word & UINT32_C(0xff)) > form) return first + form - UINT32_C(1);
    return species;
}

uint32_t px_build_pokemon_name(uint32_t destination, uint32_t species, const struct px_name_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const uint32_t string = m->load32(m->user, UINT32_C(0x005d1790));
    uint32_t result = 0;
    if (species < UINT32_C(0x2d4)) {
        const uint32_t index = px_pokemon_form_index(species, 0, m);
        const uint32_t resource = m->load32(m->user, UINT32_C(0x005d1794));
        result = api->load_text(api->user, resource, index, string);
    }
    const uint32_t current = m->load32(m->user, UINT32_C(0x005d1790));
    const uint32_t source = m->load32(m->user, current + UINT32_C(4));
    (void)px_copy_game_string(destination, source, m);
    return result;
}

void px_write_pokemon_name(uint32_t manager, uint32_t source, const struct px_pokemon_api *m)
{
    px_decode_pokemon_manager(manager, m);
    const uint32_t block = px_pokemon_block(manager, 1, 1, m);
    (void)px_copy_game_string(block, source, m);
    px_encode_pokemon_manager(manager, m);
}
