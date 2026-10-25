#include "pokemon_move_resource.h"

static uint32_t signed_byte(uint8_t value)
{
    return value < 128 ? value : UINT32_C(0xffffff00) | value;
}

void px_copy_move_resource(uint32_t source, uint32_t destination, const struct px_pokemon_api *m)
{
    const uint32_t from = m->load32(m->user, source + UINT32_C(8));
    const uint32_t to = m->load32(m->user, destination + UINT32_C(8));
    for (uint32_t i = 0; i < PX_MOVE_RESOURCE_PAYLOAD_SIZE; ++i)
        m->store8(m->user, to + i, m->load8(m->user, from + i));
    m->store16(m->user, destination + UINT32_C(4), m->load16(m->user, source + UINT32_C(4)));
}

uint32_t px_find_cached_move_resource(uint32_t cache, uint32_t move,
                                      uint32_t destination, const struct px_pokemon_api *m)
{
    if (m->load8(m->user, cache + UINT32_C(8)) == 0) return 0;
    uint32_t index = 0;
    do {
        uint32_t table = m->load32(m->user, cache + UINT32_C(4));
        uint32_t record = m->load32(m->user, table + index * UINT32_C(4));
        if (m->load16(m->user, record + UINT32_C(4)) == move) {
            table = m->load32(m->user, cache + UINT32_C(4));
            record = m->load32(m->user, table + index * UINT32_C(4));
            px_copy_move_resource(record, destination, m);
            return 1;
        }
        ++index;
    } while (m->load8(m->user, cache + UINT32_C(8)) > index);
    return 0;
}

void px_store_cached_move_resource(uint32_t cache, uint32_t source, const struct px_pokemon_api *m)
{
    const uint32_t move = m->load16(m->user, source + UINT32_C(4));
    uint32_t index = 0;
    if (m->load8(m->user, cache + UINT32_C(8)) != 0) {
        do {
            const uint32_t table = m->load32(m->user, cache + UINT32_C(4));
            const uint32_t record = m->load32(m->user, table + index * UINT32_C(4));
            if (m->load16(m->user, record + UINT32_C(4)) == move) return;
            ++index;
        } while (m->load8(m->user, cache + UINT32_C(8)) > index);
    }
    const uint32_t table = m->load32(m->user, cache + UINT32_C(4));
    const uint32_t cursor = m->load8(m->user, cache + UINT32_C(9));
    px_copy_move_resource(source, m->load32(m->user, table + cursor * UINT32_C(4)), m);
    const uint32_t next = (uint32_t)m->load8(m->user, cache + UINT32_C(9)) + UINT32_C(1);
    const uint32_t capacity = m->load8(m->user, cache + UINT32_C(8));
    m->store8(m->user, cache + UINT32_C(9), (uint8_t)(next % capacity));
}

void px_load_move_resource(uint32_t record, uint32_t move, const struct px_move_resource_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    if (move >= PX_MOVE_RESOURCE_COUNT) move = 0;
    if (m->load16(m->user, record + UINT32_C(4)) == move) return;
    const uint32_t destination = m->load32(m->user, record + UINT32_C(8));
    const uint32_t archive = m->load32(m->user, UINT32_C(0x005e5338));
    api->read_member(api->user, archive, move, destination);
    m->store16(m->user, record + UINT32_C(4), (uint16_t)move);
}

uint32_t px_get_move_resource_value(uint32_t move, uint32_t selector, const struct px_move_resource_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const uint32_t selected = m->load32(m->user, UINT32_C(0x005d188c));
    const uint32_t cache = m->load32(m->user, UINT32_C(0x005d1890));
    if (!px_find_cached_move_resource(cache, move, selected, m)) {
        px_load_move_resource(selected, move, api);
        px_store_cached_move_resource(m->load32(m->user, UINT32_C(0x005d1890)), selected, m);
    }
    return px_move_resource_field(m->load32(m->user, UINT32_C(0x005d188c)), selector, m);
}

uint32_t px_move_resource_field(uint32_t record, uint32_t selector,
                                const struct px_pokemon_api *m)
{
    /* 00176328. Invalid selectors reach zero without loading the payload. */
    if (selector >= UINT32_C(0x21) || selector == UINT32_C(0x1d)) return 0;
    const uint32_t buffer = m->load32(m->user, record + UINT32_C(8));
    switch (selector) {
    case 0: case 1: case 2: case 3: case 5:
        return m->load8(m->user, buffer + selector);
    case 4: {
        const uint32_t value = m->load8(m->user, buffer + UINT32_C(4));
        return value == 101 ? 100 : value;
    }
    case 6: return signed_byte(m->load8(m->user, buffer + UINT32_C(6)));
    case 7: return m->load8(m->user, buffer + UINT32_C(0x0e));
    case 8: return m->load8(m->user, buffer + UINT32_C(7)) >> 4;
    case 9: return m->load8(m->user, buffer + UINT32_C(7)) & UINT32_C(15);
    case 0x0a: return m->load8(m->user, buffer + UINT32_C(0x0f));
    case 0x0b: return m->load16(m->user, buffer + UINT32_C(8));
    case 0x0c: case 0x0d: case 0x0e: case 0x0f:
        return m->load8(m->user, buffer + selector - UINT32_C(2));
    case 0x10: case 0x11: case 0x12: case 0x16: case 0x17: case 0x18:
        return m->load8(m->user, buffer + selector + UINT32_C(5));
    case 0x13: case 0x14: case 0x15:
        return signed_byte(m->load8(m->user, buffer + selector + UINT32_C(5)));
    case 0x19: case 0x1a: {
        const uint32_t value = m->load8(m->user, buffer + selector - UINT32_C(7));
        return value < 128 ? value : 0;
    }
    case 0x1b: return m->load8(m->user, buffer + UINT32_C(0x14));
    case 0x1c: return m->load16(m->user, buffer + UINT32_C(0x10));
    case 0x1e: case 0x1f: {
        const uint32_t value = m->load8(m->user, buffer + selector - UINT32_C(0x0c));
        return value >= 128 ? UINT32_C(256) - value : 0;
    }
    case 0x20: return m->load8(m->user, buffer + UINT32_C(4)) == 101;
    default: return 0;
    }
}
