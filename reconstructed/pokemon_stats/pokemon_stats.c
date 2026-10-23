#include "pokemon_stats.h"

static uint32_t current_manager(uint32_t object,
                                 const struct px_pokemon_api *memory)
{
    return memory->load32(memory->user, object + UINT32_C(0x0c));
}

uint32_t px_pokemon_object_level(uint32_t object, const struct px_data_api *api)
{
    /* 0x00168ae4 includes the fallthrough to 0x00168b3c for absent extras. */
    const struct px_pokemon_api *memory = api->memory;
    uint32_t manager = current_manager(object, memory);
    const uint32_t has_extra = px_pokemon_has_extra(manager, memory);
    manager = current_manager(object, memory);
    if (has_extra != 0) return px_read_pokemon_extra_level(manager, memory);

    const uint32_t species = px_read_pokemon_state_value(manager, PX_STATE_SPECIES, memory);
    manager = current_manager(object, memory);
    const uint32_t form = px_read_pokemon_state_value(manager, PX_STATE_FORM, memory);
    manager = current_manager(object, memory);
    const uint32_t growth = px_read_pokemon_state_value(manager, PX_STATE_GROWTH, memory);
    return px_pokemon_level_from_growth(species, form, growth, api);
}

static uint32_t calculate_hp(uint32_t object, uint32_t species_manager,
                              const struct px_data_api *api)
{
    /* Shared sequence in 0x0015306c and both calculations in 0x0013aa84.
     * Species survives the level helper and both personal-resource callbacks. */
    const struct px_pokemon_api *memory = api->memory;
    const struct px_parameter_api *personal = memory->parameters;
    const uint32_t species = px_read_pokemon_state_value(species_manager, PX_STATE_SPECIES, memory);
    const uint32_t level = px_pokemon_object_level(object, api);
    uint32_t manager = current_manager(object, memory);
    const uint32_t iv = px_read_pokemon_iv(manager, PX_STAT_HP, memory);
    manager = current_manager(object, memory);
    const uint32_t ev = px_read_pokemon_ev(manager, PX_STAT_HP, memory);
    manager = current_manager(object, memory);
    const uint32_t form = px_read_pokemon_state_value(manager, PX_STATE_FORM, memory);
    personal->select_species(personal->user, species, form);
    const uint32_t base = personal->personal_value(personal->user, 0);
    return px_pokemon_hp_value(species, (uint8_t)level, (uint16_t)base,
                               (uint16_t)iv, (uint16_t)ev);
}

uint32_t px_pokemon_object_max_hp(uint32_t object, const struct px_data_api *api)
{
    /* 0x0015306c: the auxiliary presence check does not replace the reload. */
    const struct px_pokemon_api *memory = api->memory;
    uint32_t manager = current_manager(object, memory);
    const uint32_t has_extra = px_pokemon_has_extra(manager, memory);
    manager = current_manager(object, memory);
    if (has_extra != 0)
        return px_read_pokemon_state_value(manager, PX_STATE_MAX_HP, memory);
    return calculate_hp(object, manager, api);
}

void px_update_pokemon_hp(uint32_t object, uint32_t mode,
                          const struct px_data_api *api)
{
    /* 0x0013aa84. Neither the first HP value nor the old maximum is replaced
     * by resource mutations during the second, unconditional calculation. */
    const struct px_pokemon_api *memory = api->memory;
    const uint32_t old_maximum = px_pokemon_object_max_hp(object, api);
    uint32_t manager = current_manager(object, memory);
    const uint32_t has_extra = px_pokemon_has_extra(manager, memory);
    manager = current_manager(object, memory);
    uint32_t current = has_extra != 0 ? px_read_pokemon_current_hp(manager, memory) :
                                      calculate_hp(object, manager, api);

    manager = current_manager(object, memory);
    const uint32_t calculated_maximum = calculate_hp(object, manager, api);
    manager = current_manager(object, memory);
    px_write_pokemon_stat(manager, PX_STAT_HP, calculated_maximum, memory);
    if (mode != 0 && current == 0) return;

    const uint32_t new_maximum = px_pokemon_object_max_hp(object, api);
    if (new_maximum >= old_maximum)
        current += new_maximum - old_maximum;
    else if (current > new_maximum)
        current = new_maximum;
    manager = current_manager(object, memory);
    px_write_pokemon_state_value(manager, PX_STATE_CURRENT_HP, (uint16_t)current, memory);
}

void px_update_pokemon_stat(uint32_t object, enum px_state_stat requested,
                            const struct px_data_api *api)
{
    enum px_stat stat;
    switch (requested) {
    case PX_STATE_STAT_0014FB00: stat = PX_STAT_ATTACK; break;
    case PX_STATE_STAT_0014FBD4: stat = PX_STAT_DEFENSE; break;
    case PX_STATE_STAT_0014FDA4: stat = PX_STAT_SPECIAL_ATTACK; break;
    case PX_STATE_STAT_0014FE78: stat = PX_STAT_SPECIAL_DEFENSE; break;
    case PX_STATE_STAT_0014FA2C: stat = PX_STAT_SPEED; break;
    default: return;
    }

    const struct px_pokemon_api *memory = api->memory;
    const struct px_parameter_api *personal = memory->parameters;
    const uint32_t level = px_pokemon_object_level(object, api);
    uint32_t manager = current_manager(object, memory);
    const uint32_t iv = px_read_pokemon_iv(manager, stat, memory);
    manager = current_manager(object, memory);
    const uint32_t ev = px_read_pokemon_ev(manager, stat, memory);
    manager = current_manager(object, memory);
    const uint32_t nature = (uint8_t)px_read_pokemon_nature(manager, memory);
    manager = current_manager(object, memory);
    const uint32_t species = px_read_pokemon_state_value(manager, PX_STATE_SPECIES, memory);
    manager = current_manager(object, memory);
    const uint32_t form = px_read_pokemon_state_value(manager, PX_STATE_FORM, memory);
    personal->select_species(personal->user, species, form);
    const uint32_t base = personal->personal_value(personal->user, (uint32_t)stat);
    const uint32_t value = px_pokemon_stat_value((uint8_t)level, (uint16_t)base,
                                                (uint16_t)iv, (uint16_t)ev,
                                                nature, stat, memory);
    /* The five ARM routines fall through into their setters after restoring
     * their own frame; this host call retains their complete memory effect. */
    manager = current_manager(object, memory);
    px_write_pokemon_stat(manager, stat, value, memory);
}
