#include "battle_core_4.h"

static const struct px_battle_event_api *events(const struct px_battle_core_4_api *api)
{
    return api->core->items->base->events;
}

static const struct px_pokemon_api *memory(const struct px_battle_core_4_api *api)
{
    return api->handler->memory;
}

/* The battler of an id: a bad id or a missing battler calls the abort routine and the code goes on. */
static uint32_t battler_of(uint32_t context, uint32_t id, const struct px_battle_core_4_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t table = m->load32(m->user, context + 8);
    if (id >= 0x18) api->handler->abort(api->handler->user);
    const uint32_t battler = m->load32(m->user, table + id * 4 + 0x84);
    if (battler == 0) api->handler->abort(api->handler->user);
    return m->load32(m->user, table + id * 4 + 0x84);
}

uint32_t px_battle_grounded_check(uint32_t context, uint32_t battler, uint32_t flag, const struct px_battle_core_4_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const struct px_battle_event_api *e = events(api);
    uint32_t flying = 0;
    if (flag != 0) flying = px_battle_has_type(battler, 2, m);
    px_battle_event_push(e);
    px_battle_event_set_id(2, m->load8(m->user, battler + 0x19), e);
    px_battle_event_set_value(0x53, flying, e);
    px_battle_event_set_value(0x42, 0, e);
    px_battle_event_dispatch(context, 0x12, e);
    if (m->load32(m->user, battler + 0x70) & 7) px_battle_event_rewrite(0x42, 1, e);
    if (m->load32(m->user, battler + 0x98) & 7) px_battle_event_rewrite(0x42, 1, e);
    if ((m->load32(m->user, battler + 0x9c) & 7) || (m->load32(m->user, battler + 0x94) & 7))
        px_battle_event_rewrite(0x53, 1, e);
    const uint32_t first = px_battle_event_get(0x53, e) & 0xff;
    const uint32_t second = px_battle_event_get(0x42, e) & 0xff;
    px_battle_event_pop(e);
    return second == 0 && first != 0;
}

uint32_t px_battle_is_airborne(uint32_t context, uint32_t id, const struct px_battle_core_4_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t battler = battler_of(context, id, api);
    if (m->load32(m->user, m->load32(m->user, context + 0x1c) + 0x27c) != 0) return 0;
    return px_battle_grounded_check(context, battler, 1, api) != 0;
}

uint32_t px_battle_is_exempt(uint32_t context, uint32_t id, const struct px_battle_core_4_api *api)
{
    const uint32_t battler = battler_of(context, id, api);
    return px_battle_first_set_id(battler, api->id_table, memory(api)) != PX_BATTLE_NO_TYPE;
}

uint32_t px_battle_item_usable(uint32_t context, uint32_t id, const struct px_battle_core_4_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t battler = battler_of(context, id, api);
    if (api->handler->pokemon_value(api->handler->user, battler, 0x11) == 0x67) return 0;
    if (m->load32(m->user, battler + 0x68) & 7) return 0;
    return m->load32(m->user, m->load32(m->user, context + 0x1c) + 0x290) == 0;
}

uint32_t px_battle_has_flagged_condition(uint32_t context, uint32_t id, const struct px_battle_core_4_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t battler = battler_of(context, id, api);
    const uint32_t value = m->load8(m->user, battler + 0x171);
    if (value != 0 && ((value - 1) & 0xff) < 0x18) return 1;
    const uint32_t again = battler_of(context, id, api);
    return (m->load32(m->user, again + 0xa0) & 7) != 0;
}

uint32_t px_battle_group_table(uint32_t context, uint32_t slot, const struct px_battle_core_4_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t group = px_battle_slot_group(slot, api->handler);
    const uint32_t table = m->load32(m->user, context + 8);
    if (group >= 4) api->handler->abort(api->handler->user);
    return table + group * 28 + 4;
}

void px_battle_mark_group_flag(uint32_t object, uint32_t slot, uint32_t index, const struct px_battle_core_4_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t group = px_battle_slot_group(slot, api->handler);
    const uint32_t inner = m->load32(m->user, object + 4);
    const uint32_t mode = m->load8(m->user, m->load32(m->user, inner + 0x10));
    if (!((mode == 0 || mode == 1) && m->load8(m->user, inner + 0x815) == group)) return;
    const uint32_t outer = m->load32(m->user, object);
    if (index >= 2) {
        api->handler->abort(api->handler->user);
        return;
    }
    m->store8(m->user, m->load32(m->user, outer + 0x10) + index + 0xa50, 1);
}

uint32_t px_battle_ring_copy(uint32_t context, uint32_t back, uint32_t skip, uint32_t destination, const struct px_pokemon_api *m)
{
    if (back >= 3) return 0;
    int32_t position = (int32_t)m->load8(m->user, context + 0x205) - (int32_t)back;
    if (position < 0) position += 3;
    const uint32_t length = m->load8(m->user, context + (uint32_t)position + 0x202);
    if (length <= skip) return 0;
    const uint32_t entry = (length - skip - 1) & 0xff;
    px_battle_copy_8(destination, context + (uint32_t)position * 48 + entry * 8 + 0x172, m);
    return 1;
}

uint32_t px_battle_has_condition(uint32_t battler, uint32_t index, const struct px_battle_handler_api *api)
{
    if (index >= 0x26) api->abort(api->user);
    return (api->memory->load32(api->memory->user, battler + index * 4 + 0x1c) & 7) != 0;
}

uint32_t px_battle_range_width(uint32_t context, uint32_t index, const struct px_battle_handler_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    if (m->load8(m->user, context + 0x152) <= index) api->abort(api->user);
    return (m->load8(m->user, context + index * 14 + 0x11f) - m->load8(m->user, context + index * 14 + 0x11e)) & 0xff;
}

uint32_t px_battle_array_word_b(uint32_t array, uint32_t index, const struct px_pokemon_api *m)
{
    return px_battle_array_word(array, index, m);
}

void px_battle_fill_6(uint32_t address, uint32_t value, const struct px_pokemon_api *m)
{
    const uint32_t byte = (value & 0xff) | (value & 0xff) << 8;
    const uint32_t word = byte | byte << 16;
    m->store32(m->user, address, word);
    m->store16(m->user, address + 4, (uint16_t)word);
}

uint32_t px_battle_pack_7(uint32_t first, uint32_t second)
{
    return (((first << 3 & UINT32_C(0x1f8)) | 2) & ~UINT32_C(0x1fffe00)) | second << 9;
}

void px_battle_report_slot(uint32_t context, uint32_t id, const struct px_battle_core_4_api *api)
{
    const struct px_battle_items_api *items = api->core->items;
    const struct px_pokemon_api *m = memory(api);
    void *user = api->handler->user;
    if (px_battle_event_get(3, events(api)) != id) return;
    (void)px_battle_pokemon_by_id(context, id, items->base);
    uint32_t record = items->action_record(user, context, 0x15, id);
    m->store8(m->user, record + 5, (uint8_t)id);
    m->store8(m->user, record + 4, 7);
    items->commit_actions(user, context, record);
    const uint32_t battler = px_battle_pokemon_by_id(context, id, items->base);
    record = items->action_record(user, context, 0x27, id);
    const uint32_t header = m->load32(m->user, record);
    const uint32_t count = px_battle_byte_16d(battler, 3, api->handler);
    m->store8(m->user, record + 4, (uint8_t)id);
    m->store8(m->user, record + 5, 3);
    m->store8(m->user, record + 6, (uint8_t)(count + 1));
    m->store32(m->user, record, header & ~UINT32_C(0x1000000));
    items->commit_actions(user, context, record);
    record = items->action_record(user, context, 3, id);
    items->message(user, record + 4, 2, 0x205);
    items->message_arg(user, record + 4, id);
    items->commit_actions(user, context, record);
}

void px_battle_popup_message(uint32_t context, uint32_t id, uint32_t popup, uint32_t state, uint32_t message,
                             const struct px_battle_core_4_api *api)
{
    const struct px_battle_items_api *items = api->core->items;
    const struct px_pokemon_api *m = memory(api);
    void *user = api->handler->user;
    if (px_battle_event_get(2, events(api)) != id) return;
    if (m->load32(m->user, state) == 0) return;
    const uint32_t previous = px_battle_event_get(0x1a, events(api));
    if (previous == 0 || m->load32(m->user, state + 4) != previous) {
        items->ability_popup(user, context, 1, popup);
        const uint32_t record = items->action_record(user, context, 3, popup);
        items->message(user, record + 4, 2, message);
        items->message_arg(user, record + 4, id);
        px_battle_message_flag(record + 4, m);
        items->commit_actions(user, context, record);
        items->ability_popup(user, context, 2, popup);
        m->store32(m->user, state + 4, previous);
    }
    m->store32(m->user, state, 0);
}
