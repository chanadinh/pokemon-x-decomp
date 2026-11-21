#include "battle_core_3.h"

void px_battle_pack_5(uint32_t address, uint32_t first, uint32_t second, uint32_t flag, const struct px_pokemon_api *m)
{
    uint32_t word = ((second << 4) & UINT32_C(0xf0)) | 1;
    word = word | first << 8;
    m->store32(m->user, address, word);
    if (flag != 0) word |= UINT32_C(0x10000000);
    else word &= ~UINT32_C(0x10000000);
    m->store32(m->user, address, word);
}

void px_battle_pack_6(uint32_t address, uint32_t first, uint32_t second, const struct px_pokemon_api *m)
{
    m->store32(m->user, address, ((first << 4) & UINT32_C(0x70)) | 3 | ((second << 7) & UINT32_C(0x380)));
}

uint32_t px_battle_unpack_field(uint32_t word)
{
    const uint32_t kind = word & 7;
    if (kind == 1 || kind == 2 || kind == 3) return (word << 7) >> 16;
    if (kind == 4) return (word << 1) >> 16;
    return 0;
}

void px_battle_push_record(uint32_t address, uint32_t value, const struct px_pokemon_api *m)
{
    const uint32_t flags = m->load8(m->user, address + 3);
    const uint32_t count = flags >> 4;
    if (count >= 8) return;
    m->store8(m->user, address + 3, (uint8_t)((flags & ~UINT32_C(0xf0)) | (((count << 4) + 0x10) & 0xf0)));
    m->store32(m->user, address + 4 + 4 * count, value);
}

void px_battle_init_record(uint32_t address, uint32_t nibble, uint32_t id, const struct px_pokemon_api *m)
{
    m->store16(m->user, address, (uint16_t)id);
    for (uint32_t offset = 4; offset <= 0x20; offset += 4) m->store32(m->user, address + offset, 0);
    m->store8(m->user, address + 3, (uint8_t)(nibble & 0xf));
    m->store8(m->user, address + 2, 0);
}

void px_battle_copy_8(uint32_t destination, uint32_t source, const struct px_pokemon_api *m)
{
    const uint32_t first = m->load32(m->user, source), second = m->load32(m->user, source + 4);
    m->store32(m->user, destination, first);
    m->store32(m->user, destination + 4, second);
}

void px_battle_copy_6(uint32_t destination, uint32_t source, const struct px_pokemon_api *m)
{
    m->store32(m->user, destination, m->load32(m->user, source));
    m->store16(m->user, destination + 4, m->load16(m->user, source + 4));
}

uint32_t px_battle_pair_search(uint32_t context, uint32_t value, const struct px_pokemon_api *m)
{
    if (value >= 4) return 0;
    for (uint32_t i = 0; i < 10; i += 2)
        if (m->load8(m->user, context + i + 0x7ec) == value || m->load8(m->user, context + i + 0x7ed) == value) return 1;
    return 0;
}

uint32_t px_battle_slot_group(uint32_t slot, const struct px_battle_handler_api *api)
{
    if (slot <= 5) return 0;
    if (slot >= 12 && slot <= 0x11) return 1;
    if (slot >= 6 && slot <= 11) return 2;
    if (slot >= 0x12 && slot <= 0x17) return 3;
    api->abort(api->user);
    return 0;
}

uint32_t px_battle_scaled_amount(uint32_t context, const struct px_pokemon_api *m)
{
    if (m->load8(m->user, m->load32(m->user, context + 0x10) + 0xb3) != 0) return 0;
    uint32_t amount = m->load32(m->user, context + 0x804);
    if (m->load8(m->user, context + 0x81c) & 8) return amount;
    uint32_t factor = 1;
    if (m->load8(m->user, context + 0xa50) != 0) factor = 2;
    if (m->load8(m->user, context + 0xa51) != 0) factor <<= 1;
    if (factor > 1) {
        amount *= factor;
        if (amount > UINT32_C(0x1869f)) amount = UINT32_C(0x1869f);
    }
    return amount;
}

uint32_t px_battle_free_of_conditions_2_3(uint32_t pokemon, const struct px_battle_handler_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    if (pokemon == 0) return 0;
    if (api->pokemon_value(api->user, pokemon, 0xd) == 0) return 0;
    if (m->load32(m->user, pokemon + 0x24) & 7) return 0;
    return (m->load32(m->user, pokemon + 0x28) & 7) == 0;
}

uint32_t px_battle_usable_item(uint32_t pokemon, uint32_t context, const struct px_battle_handler_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    if (m->load32(m->user, context + 0x290) != 0) return 0;
    if (m->load32(m->user, pokemon + 0x68) & 7) return 0;
    if (api->pokemon_value(api->user, pokemon, 0x11) == 0x67) return 0;
    return m->load16(m->user, pokemon + 0x12);
}

uint32_t px_battle_party_has_hp(uint32_t context, uint32_t out, const struct px_battle_handler_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    uint32_t party = m->load32(m->user, context + 0xb8);
    const uint32_t first = m->load8(m->user, party + 0x18) > 1 ? m->load32(m->user, party + 4) : 0;
    if (api->pokemon_value(api->user, first, 0xd) != 0) {
        m->store32(m->user, out, 0x16);
        return 4;
    }
    party = m->load32(m->user, context + 0xb8);
    const uint32_t second = m->load8(m->user, party + 0x18) > 2 ? m->load32(m->user, party + 8) : 0;
    if (api->pokemon_value(api->user, second, 0xd) != 0) {
        m->store32(m->user, out, 0x26);
        return 4;
    }
    return 0;
}

uint32_t px_battle_byte_16d(uint32_t context, uint32_t index, const struct px_battle_handler_api *api)
{
    if (index >= 5) api->abort(api->user);
    return api->memory->load8(api->memory->user, context + index + 0x16d);
}

uint32_t px_battle_entry_274(uint32_t context, uint32_t index, const struct px_battle_handler_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const uint32_t table = m->load32(m->user, context + 0x1c);
    if (index >= 11) {
        api->abort(api->user);
        return 0;
    }
    return m->load32(m->user, table + index * 4 + 0x274) != 0;
}

uint32_t px_battle_byte_11e(uint32_t context, uint32_t index, const struct px_battle_handler_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    if (m->load8(m->user, context + 0x152) <= index) {
        api->abort(api->user);
        return 0;
    }
    return m->load8(m->user, context + index * 14 + 0x11e);
}

uint32_t px_battle_half_11c(uint32_t context, uint32_t index, const struct px_battle_handler_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    if (m->load8(m->user, context + 0x152) <= index) api->abort(api->user);
    return m->load16(m->user, context + index * 14 + 0x11c);
}

uint32_t px_battle_is_side_0(uint32_t side, const struct px_battle_handler_api *api)
{
    if (side >= 2) api->abort(api->user);
    return side == 0;
}

uint32_t px_battle_side_record_clear(uint32_t side, uint32_t id, const struct px_battle_tables_api *api)
{
    const struct px_pokemon_api *m = api->handler->memory;
    const uint32_t record = api->layers + (side * UINT32_C(17) + id) * UINT32_C(16);
    const uint32_t handler = m->load32(m->user, record);
    if (handler == 0) return 0;
    px_battle_handler_remove(handler, api->handler);
    const uint32_t kind = m->load32(m->user, record + 4);
    m->store32(m->user, record, 0);
    m->store32(m->user, record + 4, kind & ~UINT32_C(7));
    m->store32(m->user, record + 0xc, 0);
    return 1;
}
