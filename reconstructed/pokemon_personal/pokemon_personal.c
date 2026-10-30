#include "pokemon_personal.h"

void px_load_personal_record(uint32_t record, uint32_t species, uint32_t form,
                              const struct px_pokemon_api *m)
{
    const uint32_t table = m->load32(m->user, UINT32_C(0x005d177c));
    uint32_t index = 0;
    if (species <= UINT32_C(721)) {
        index = species;
        if (form != 0) {
            const uint32_t base = table + species * UINT32_C(64);
            const uint32_t first = m->load32(m->user, base + UINT32_C(0x1c)) & UINT32_C(0xffff);
            const uint32_t count = m->load32(m->user, base + UINT32_C(0x20)) & UINT32_C(0xff);
            if (first != 0 && form < count) index = first + form - UINT32_C(1);
        }
    }
    const uint32_t source = table + index * UINT32_C(64);
    const uint32_t destination = m->load32(m->user, record + UINT32_C(8));
    /* Preserve the two 32-byte VFP load/store groups rather than treating
     * this as a bytewise or overlap-safe 64-byte library copy. */
    for (uint32_t block = 0; block < 2; ++block) {
        uint32_t words[8];
        for (uint32_t i = 0; i < 8; ++i)
            words[i] = m->load32(m->user, source + block * UINT32_C(32) + i * UINT32_C(4));
        for (uint32_t i = 0; i < 8; ++i)
            m->store32(m->user, destination + block * UINT32_C(32) + i * UINT32_C(4), words[i]);
    }
    m->store16(m->user, record + UINT32_C(4), (uint16_t)species);
    m->store8(m->user, record + UINT32_C(6), (uint8_t)form);
}

uint32_t px_personal_resource_field(uint32_t record, uint32_t selector,
                                    const struct px_pokemon_api *m)
{
    if (selector >= PX_PERSONAL_SELECTOR_COUNT) return 0;
    const uint32_t buffer = m->load32(m->user, record + UINT32_C(8));
    if (selector <= 8) return m->load8(m->user, buffer + selector);
    if (selector >= UINT32_C(0x0a) && selector <= UINT32_C(0x10)) {
        const uint32_t value = m->load16(m->user, buffer + UINT32_C(10));
        return selector == UINT32_C(0x10) ? (value >> 12) & UINT32_C(1)
                                        : (value >> ((selector - UINT32_C(0x0a)) * UINT32_C(2))) & UINT32_C(3);
    }
    if (selector >= UINT32_C(0x11) && selector <= UINT32_C(0x13))
        return m->load16(m->user, buffer + UINT32_C(12) + (selector - UINT32_C(0x11)) * UINT32_C(2));
    if (selector >= UINT32_C(0x14) && selector <= UINT32_C(0x1d))
        return m->load8(m->user, buffer + selector - UINT32_C(2));
    switch (selector) {
    case 9: return m->load16(m->user, buffer + UINT32_C(0x22));
    case 0x1e: return m->load16(m->user, buffer + UINT32_C(0x1c));
    case 0x1f: return m->load16(m->user, buffer + UINT32_C(0x1e));
    case 0x20: return m->load8(m->user, buffer + UINT32_C(0x20));
    case 0x21: return m->load8(m->user, buffer + UINT32_C(0x21)) & UINT32_C(0x3f);
    case 0x22: return (m->load8(m->user, buffer + UINT32_C(0x21)) >> 6) & UINT32_C(1);
    case 0x23: return m->load8(m->user, buffer + UINT32_C(0x21)) >> 7;
    case 0x24: return m->load8(m->user, buffer + UINT32_C(9));
    case 0x25: case 0x26:
        return m->load16(m->user, buffer + UINT32_C(0x24) + (selector - UINT32_C(0x25)) * UINT32_C(2));
    case 0x27: case 0x28: case 0x29: case 0x2a: case 0x2b:
        return m->load32(m->user, buffer + UINT32_C(0x28) + (selector - UINT32_C(0x27)) * UINT32_C(4));
    case 0x2c: return m->load16(m->user, buffer + UINT32_C(0x3c));
    case 0x2d: return m->load8(m->user, buffer + UINT32_C(0x3e));
    default: return 0;
    }
}

void px_select_personal_resource(uint32_t species, uint32_t form, const struct px_pokemon_api *m)
{
    px_load_personal_record(m->load32(m->user, UINT32_C(0x005d17a4)), species, form, m);
}

uint32_t px_current_personal_value(uint32_t selector, const struct px_pokemon_api *m)
{
    return px_personal_resource_field(m->load32(m->user, UINT32_C(0x005d17a4)), selector, m);
}
