#include "pokemon_state.h"

static uint32_t current_manager(uint32_t object,
                                 const struct px_pokemon_api *memory)
{
    return memory->load32(memory->user, object + UINT32_C(0x0c));
}

void px_refresh_pokemon_state(uint32_t object, const struct px_state_api *api)
{
    /* 0x0013a7bc. Each repeated getter is a separate original call. */
    const struct px_pokemon_api *memory = api->memory;
    uint32_t manager = current_manager(object, memory);
    const uint32_t maximum_hp = px_read_pokemon_state_value(manager, PX_STATE_MAX_HP, memory);
    manager = current_manager(object, memory);
    px_write_pokemon_state_value(manager, PX_STATE_CURRENT_HP, (uint16_t)maximum_hp, memory);
    manager = current_manager(object, memory);
    px_write_pokemon_state_value(manager, PX_STATE_STATUS, 0, memory);

    for (uint32_t slot = 0; slot < UINT32_C(4); ++slot) {
        manager = current_manager(object, memory);
        uint32_t move = px_read_pokemon_state_slot(manager, slot, PX_STATE_MOVE, memory);
        manager = current_manager(object, memory);
        uint32_t pp_ups = px_read_pokemon_state_slot(manager, slot, PX_STATE_PP_UPS, memory);
        const uint32_t initial_maximum = api->maximum_pp(api->user, move, (uint8_t)pp_ups);

        manager = current_manager(object, memory);
        move = px_read_pokemon_state_slot(manager, slot, PX_STATE_MOVE, memory);
        if (move == 0) continue;

        manager = current_manager(object, memory);
        move = px_read_pokemon_state_slot(manager, slot, PX_STATE_MOVE, memory);
        manager = current_manager(object, memory);
        pp_ups = px_read_pokemon_state_slot(manager, slot, PX_STATE_PP_UPS, memory);
        const uint32_t second_maximum = api->maximum_pp(api->user, move, (uint8_t)pp_ups);
        manager = current_manager(object, memory);
        const uint32_t current_pp = px_read_pokemon_state_slot(manager, slot, PX_STATE_PP, memory);
        const uint32_t sum = current_pp + initial_maximum;
        const uint32_t capped = sum <= second_maximum ? sum : second_maximum;
        /* The low-byte truncation precedes the third maximum-PP call. */
        const uint32_t captured_byte = (uint8_t)capped;

        manager = current_manager(object, memory);
        move = px_read_pokemon_state_slot(manager, slot, PX_STATE_MOVE, memory);
        manager = current_manager(object, memory);
        pp_ups = px_read_pokemon_state_slot(manager, slot, PX_STATE_PP_UPS, memory);
        const uint32_t third_maximum = api->maximum_pp(api->user, move, (uint8_t)pp_ups);
        manager = current_manager(object, memory);
        const uint32_t final_pp = third_maximum >= captured_byte ? captured_byte : third_maximum;
        px_write_pokemon_state_slot(manager, slot, PX_STATE_PP, (uint8_t)final_pp, memory);
    }
}

void px_initialize_pokemon_base_state(uint32_t object, uint8_t *name_scratch,
                                      const struct px_state_api *api)
{
    /* 0x0013a8d8. Keep the first species/form pair across growth lookup. */
    const struct px_pokemon_api *memory = api->memory;
    uint32_t manager;

    for (uint32_t slot = 0; slot < UINT32_C(4); ++slot) {
        manager = current_manager(object, memory);
        px_write_pokemon_state_slot(manager, slot, PX_STATE_MOVE, 0, memory);
        manager = current_manager(object, memory);
        px_write_pokemon_state_slot(manager, slot, PX_STATE_PP_UPS, 0, memory);
        manager = current_manager(object, memory);
        px_write_pokemon_state_slot(manager, slot, PX_STATE_PP, 0, memory);
    }

    manager = current_manager(object, memory);
    const uint32_t learnset_species = px_read_pokemon_state_value(manager, PX_STATE_SPECIES, memory);
    manager = current_manager(object, memory);
    const uint32_t learnset_form = px_read_pokemon_state_value(manager, PX_STATE_FORM, memory);
    manager = current_manager(object, memory);
    const uint32_t growth_species = px_read_pokemon_state_value(manager, PX_STATE_SPECIES, memory);
    manager = current_manager(object, memory);
    const uint32_t growth_form = px_read_pokemon_state_value(manager, PX_STATE_FORM, memory);
    manager = current_manager(object, memory);
    const uint32_t growth = px_read_pokemon_state_value(manager, PX_STATE_GROWTH, memory);
    const uint32_t level = api->level_from_growth(api->user, growth_species, growth_form, growth);
    api->select_learnset(api->user, learnset_species, learnset_form);

    uint32_t index = 0;
    if (api->learnset_count(api->user) != 0) {
        do {
            const uint32_t required_level = api->learnset_level(api->user, (uint16_t)index);
            if (required_level > level) break;
            const uint32_t move = api->learnset_move(api->user, (uint16_t)index);
            api->learn_move(api->user, object, move);
            ++index;
        } while (api->learnset_count(api->user) > index);
    }

    manager = current_manager(object, memory);
    const uint32_t name_species = px_read_pokemon_state_value(manager, PX_STATE_SPECIES, memory);
    api->build_name(api->user, name_scratch, name_species);
    manager = current_manager(object, memory);
    api->write_name(api->user, manager, name_scratch);
    manager = current_manager(object, memory);
    px_write_pokemon_state_value(manager, PX_STATE_NAME_FLAG, 0, memory);
    px_refresh_pokemon_state(object, api);

    static const uint32_t offsets[] = { UINT32_C(0x69), UINT32_C(0x6a),
                                        UINT32_C(0x68), UINT32_C(0x6b) };
    static const enum px_pokemon_field fields[] = { PX_DATA_SETTING_69,
        PX_DATA_SETTING_6A, PX_DATA_SETTING_68, PX_DATA_SETTING_6B };
    for (unsigned i = 0; i < 4; ++i) {
        const uint32_t settings = memory->load32(memory->user, UINT32_C(0x005d1870));
        const uint32_t value = memory->load8(memory->user, settings + offsets[i]);
        manager = current_manager(object, memory);
        px_set_pokemon_field(manager, fields[i], value, memory);
    }
}

void px_update_pokemon_derived_state(uint32_t object, uint32_t mode,
                                     const struct px_state_api *api)
{
    /* 0x0012b808. The final mask is the saved acquisition result, 0 or 1. */
    const struct px_pokemon_api *memory = api->memory;
    const uint32_t acquired = px_acquire_pokemon_state(object, memory);
    uint32_t manager = current_manager(object, memory);
    const uint32_t species = px_read_pokemon_state_value(manager, PX_STATE_SPECIES, memory);
    manager = current_manager(object, memory);
    const uint32_t form = px_read_pokemon_state_value(manager, PX_STATE_FORM, memory);
    manager = current_manager(object, memory);
    const uint32_t growth = px_read_pokemon_state_value(manager, PX_STATE_GROWTH, memory);
    const uint32_t level = api->level_from_growth(api->user, species, form, growth);
    manager = current_manager(object, memory);
    px_set_pokemon_field(manager, PX_DATA_EXTRA_LEVEL, level, memory);
    api->update_mode(api->user, object, mode);
    static const enum px_state_stat stats[] = { PX_STATE_STAT_0014FB00,
        PX_STATE_STAT_0014FBD4, PX_STATE_STAT_0014FDA4, PX_STATE_STAT_0014FE78,
        PX_STATE_STAT_0014FA2C };
    for (unsigned i = 0; i < 5; ++i)
        api->update_stat(api->user, object, stats[i]);
    (void)px_restore_pokemon_state(object, acquired, memory);
}

void px_initialize_pokemon_extra_state(uint32_t manager,
                                       const struct px_state_api *api)
{
    /* 0x0012b744. Only the initial pointer is tested for zero; the pointer
     * passed to the clear helper is freshly loaded after possible decoding. */
    const struct px_pokemon_api *memory = api->memory;
    if (memory->load32(memory->user, manager + UINT32_C(4)) == 0) return;
    const uint8_t original_state = memory->load8(memory->user, manager + UINT32_C(0x0d));
    if (original_state == 0) {
        memory->decode_manager(memory->user, manager);
        memory->store8(memory->user, manager + UINT32_C(0x0d), 1);
        (void)memory->load8(memory->user, manager + UINT32_C(0x0c));
        /* The conditional diagnostic is the verified returning no-op. */
    }
    uint32_t extra = memory->load32(memory->user, manager + UINT32_C(4));
    uint32_t remaining = UINT32_C(0x1c);
    /* 0x0015e914 aligns an unaligned prefix before entering 0x00301aa0.
     * Retain byte/halfword/word widths and ascending addresses; multiple-word
     * ARM stores become individual ordinary word stores. */
    const uint32_t misalignment = extra & UINT32_C(3);
    if (misalignment != 0) {
        const uint32_t prefix = UINT32_C(4) - misalignment;
        if (prefix != 2) memory->store8(memory->user, extra++, 0);
        if (prefix >= 2) {
            memory->store16(memory->user, extra, 0);
            extra += UINT32_C(2);
        }
        remaining -= prefix;
    }
    while (remaining >= 4) {
        memory->store32(memory->user, extra, 0);
        extra += UINT32_C(4);
        remaining -= UINT32_C(4);
    }
    if (remaining >= 2) {
        memory->store16(memory->user, extra, 0);
        extra += UINT32_C(2);
        remaining -= UINT32_C(2);
    }
    if (remaining != 0) memory->store8(memory->user, extra, 0);
    if (original_state == 0) {
        memory->store8(memory->user, manager + UINT32_C(0x0d), 0);
        memory->encode_manager(memory->user, manager);
        if (memory->load8(memory->user, manager + UINT32_C(0x0d)) == 0)
            (void)memory->load8(memory->user, manager + UINT32_C(0x0c));
        /* Either failed check likewise reaches the returning no-op. */
    }
}
