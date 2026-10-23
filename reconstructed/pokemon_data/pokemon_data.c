#include "pokemon_data.h"

static uint16_t read16(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | (uint16_t)((uint16_t)p[1] << 8));
}

static uint32_t read32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 |
           (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

uint32_t px_resolve_pokemon_ability(uint32_t species, uint32_t form, uint32_t slot,
                                   const struct px_data_api *api)
{
    /* 001688c8: choose the attribute before the resource-selection service.
     * Invalid full-width slots use the first attribute; return narrows to u8. */
    const uint32_t attribute = slot == 1 ? UINT32_C(0x1b) :
                               slot == 2 ? UINT32_C(0x1c) : UINT32_C(0x1a);
    const struct px_parameter_api *personal = api->memory->parameters;
    personal->select_species(personal->user, species, form);
    return (uint8_t)personal->personal_value(personal->user, attribute);
}

uint32_t px_pokemon_growth_value(uint32_t level, const struct px_data_api *api)
{
    /* 00167f98 reads the currently selected growth table, after an unsigned cap. */
    const struct px_pokemon_api *m = api->memory;
    if (level > UINT32_C(100)) level = UINT32_C(100);
    const uint32_t table = m->load32(m->user, UINT32_C(0x005d17ac));
    return m->load32(m->user, table + UINT32_C(8) + level * UINT32_C(4));
}

void px_populate_pokemon_data(uint32_t data, const uint8_t *p,
                              uint32_t manager, const struct px_data_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    (void)px_initialize_pokemon_manager(manager, m);
    px_bind_pokemon_data(manager, data, 0, m);
    if (read16(p + 0x18) == 0) return;

    px_set_pokemon_field(manager, PX_DATA_KEY, read32(p), m);
    px_set_pokemon_field(manager, PX_DATA_WORD_10, read32(p + 0x10), m);
    px_set_pokemon_field(manager, PX_DATA_WORD_08, read32(p + 0x08), m);
    px_set_pokemon_field(manager, PX_DATA_SPECIES, read16(p + 0x18), m);
    px_set_pokemon_field(manager, PX_DATA_FORM, p[0x1a], m);
    px_set_pokemon_field(manager, PX_DATA_EXTRA_LEVEL, read16(p + 0x1c) & UINT32_C(0xff), m);
    px_set_pokemon_field(manager, PX_DATA_NATURE, read16(p + 0x20), m);
    px_set_pokemon_field(manager, PX_DATA_GENDER, read16(p + 0x1e), m);
    px_set_pokemon_field(manager, PX_DATA_LEVEL, read16(p + 0x1c) & UINT32_C(0xff), m);
    px_set_pokemon_field(manager, PX_DATA_ZERO_BYTE, 0, m);
    px_set_pokemon_field(manager, PX_DATA_PARAM_30, read32(p + 0x30) & UINT32_C(0xff), m);

    /* 0014f6b8 reloads the settings pointer before each of these four reads. */
    uint32_t settings = m->load32(m->user, UINT32_C(0x005d1870));
    px_set_pokemon_field(manager, PX_DATA_SETTING_68,
                         m->load8(m->user, settings + UINT32_C(0x68)), m);
    settings = m->load32(m->user, UINT32_C(0x005d1870));
    px_set_pokemon_field(manager, PX_DATA_SETTING_69,
                         m->load8(m->user, settings + UINT32_C(0x69)), m);
    settings = m->load32(m->user, UINT32_C(0x005d1870));
    px_set_pokemon_field(manager, PX_DATA_SETTING_6A,
                         m->load8(m->user, settings + UINT32_C(0x6a)), m);
    settings = m->load32(m->user, UINT32_C(0x005d1870));
    px_set_pokemon_field(manager, PX_DATA_SETTING_6B,
                         m->load8(m->user, settings + UINT32_C(0x6b)), m);

    px_set_pokemon_field(manager, PX_DATA_STAT_24, read16(p + 0x24) & UINT32_C(0xff), m);
    px_set_pokemon_field(manager, PX_DATA_STAT_26, read16(p + 0x26) & UINT32_C(0xff), m);
    px_set_pokemon_field(manager, PX_DATA_STAT_28, read16(p + 0x28) & UINT32_C(0xff), m);
    px_set_pokemon_field(manager, PX_DATA_STAT_2A, read16(p + 0x2a) & UINT32_C(0xff), m);
    px_set_pokemon_field(manager, PX_DATA_STAT_2C, read16(p + 0x2c) & UINT32_C(0xff), m);
    px_set_pokemon_field(manager, PX_DATA_STAT_2E, read16(p + 0x2e) & UINT32_C(0xff), m);

    const uint32_t ability = px_resolve_pokemon_ability(read16(p + 0x18), p[0x1a], p[0x22], api);
    px_set_pokemon_field(manager, PX_DATA_ABILITY, ability, m);
    px_set_pokemon_field(manager, PX_DATA_ABILITY_FLAG_0, p[0x22] == 0, m);
    px_set_pokemon_field(manager, PX_DATA_ABILITY_FLAG_1, p[0x22] == 1, m);
    px_set_pokemon_field(manager, PX_DATA_ABILITY_FLAG_2, p[0x22] == 2, m);
    api->select_growth(api->user, read16(p + 0x18), p[0x1a]);
    const uint32_t growth = px_pokemon_growth_value(read16(p + 0x1c) & UINT32_C(0xff), api);
    px_set_pokemon_field(manager, PX_DATA_GROWTH, growth, m);
}
