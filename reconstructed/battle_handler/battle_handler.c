#include "battle_handler.h"

enum {
    FIRST = 0x60, IN_USE = 0x64, DEPTH = 0x68,
    PREVIOUS = 0, NEXT = 4, ENTRIES = 8, WORD_C = 0xc, OWNER = 0x10, KEY = 0x14, FLAGS = 0x18, COUNT = 0x1a,
    WORK = 0x1c, ID = 0x38, VALUE = 0x3a, BYTE_3B = 0x3b
};
#define RUNNING UINT32_C(0x1000000)
#define REMOVE UINT32_C(0x8000000)
#define REGISTERED UINT32_C(0x20000000)

uint32_t px_battle_handler_register(uint32_t owner, uint32_t id, uint32_t priority, uint32_t sub_priority,
                                    uint32_t value, uint32_t entries, uint32_t count,
                                    const struct px_battle_handler_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    if (priority >= 0xff) api->abort(api->user);
    if (sub_priority >= UINT32_C(0xffffff)) api->abort(api->user);
    const uint32_t used = m->load32(m->user, api->battle + IN_USE);
    if (used == PX_BATTLE_HANDLER_POOL) {
        api->abort(api->user);
        return 0;
    }
    const uint32_t handler = m->load32(m->user, api->pool + used * UINT32_C(4));
    m->store32(m->user, api->battle + IN_USE, used + 1);
    if (handler == 0) {
        api->abort(api->user);
        return 0;
    }
    const uint32_t key = sub_priority | (UINT32_C(7) - priority) << 24;
    m->store32(m->user, handler + KEY, key);
    m->store8(m->user, handler + OWNER, (uint8_t)owner);
    m->store32(m->user, handler + PREVIOUS, 0);
    m->store32(m->user, handler + NEXT, 0);
    m->store32(m->user, handler + ENTRIES, entries);
    m->store8(m->user, handler + COUNT, (uint8_t)count);
    m->store16(m->user, handler + ID, (uint16_t)id);
    const uint32_t flags = m->load32(m->user, handler + FLAGS);
    m->store32(m->user, handler + WORD_C, 0);
    m->store8(m->user, handler + VALUE, (uint8_t)value);
    /* Keep the entry count and registration bit; record the dispatch depth. */
    m->store32(m->user, handler + FLAGS, flags & ~UINT32_C(0xdf000000));
    m->store16(m->user, handler + FLAGS, (uint16_t)m->load32(m->user, api->battle + DEPTH));
    m->store32(m->user, handler + FLAGS, m->load32(m->user, handler + FLAGS) | REGISTERED);
    m->store8(m->user, handler + BYTE_3B, owner == 0 || owner == 4 || owner == 5 ? (uint8_t)value : 0x1f);
    for (uint32_t offset = WORK; offset <= 0x34; offset += 4) m->store32(m->user, handler + offset, 0);
    /* Insert before the first handler with a smaller or equal key... */
    uint32_t at = m->load32(m->user, api->battle + FIRST);
    if (at == 0) {
        m->store32(m->user, api->battle + FIRST, handler);
        return handler;
    }
    if (key > m->load32(m->user, at + KEY)) {
        m->store32(m->user, at + PREVIOUS, handler);
        m->store32(m->user, handler + NEXT, at);
        m->store32(m->user, handler + PREVIOUS, 0);
        m->store32(m->user, api->battle + FIRST, handler);
        return handler;
    }
    /* ...that is, after every handler whose key is at least as large. */
    for (uint32_t next = m->load32(m->user, at + NEXT); next != 0; at = next, next = m->load32(m->user, next + NEXT)) {
        if (key > m->load32(m->user, next + KEY)) {
            m->store32(m->user, handler + NEXT, next);
            m->store32(m->user, handler + PREVIOUS, m->load32(m->user, next + PREVIOUS));
            const uint32_t previous = m->load32(m->user, next + PREVIOUS);
            if (previous != 0) m->store32(m->user, previous + NEXT, handler);
            m->store32(m->user, next + PREVIOUS, handler);
            return handler;
        }
    }
    m->store32(m->user, at + NEXT, handler);
    m->store32(m->user, handler + PREVIOUS, at);
    return handler;
}

/* Unlinks a handler and returns it to the pool; a running handler only
 * records the request. next is the link read before the handler is changed. */
static void release(uint32_t handler, uint32_t next, const struct px_battle_handler_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const uint32_t flags = m->load32(m->user, handler + FLAGS);
    if (!(flags & REGISTERED)) return;
    if (flags & RUNNING) {
        m->store32(m->user, handler + FLAGS, flags | REMOVE);
        return;
    }
    if (m->load32(m->user, api->battle + FIRST) == handler) m->store32(m->user, api->battle + FIRST, next);
    const uint32_t previous = m->load32(m->user, handler + PREVIOUS);
    if (previous != 0) m->store32(m->user, previous + NEXT, next);
    const uint32_t following = m->load32(m->user, handler + NEXT);
    if (following != 0) m->store32(m->user, following + PREVIOUS, m->load32(m->user, handler + PREVIOUS));
    const uint32_t used = m->load32(m->user, api->battle + IN_USE);
    if (used == 0) {
        api->abort(api->user);
        return;
    }
    for (uint32_t offset = 0; offset <= 0x38; offset += 4) m->store32(m->user, handler + offset, 0);
    m->store32(m->user, api->battle + IN_USE, used - 1);
    m->store32(m->user, api->pool + (used - 1) * UINT32_C(4), handler);
}

void px_battle_handler_remove(uint32_t handler, const struct px_battle_handler_api *api)
{
    if (handler == 0) return;
    release(handler, api->memory->load32(api->memory->user, handler + NEXT), api);
}

void px_battle_handler_remove_owner6(const struct px_battle_handler_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    for (uint32_t handler = m->load32(m->user, api->battle + FIRST); handler != 0;) {
        const uint32_t next = m->load32(m->user, handler + NEXT);
        if (m->load8(m->user, handler + OWNER) == 6) release(handler, next, api);
        handler = next;
    }
}

uint32_t px_battle_handler_find(uint32_t owner, uint32_t value, const struct px_battle_handler_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    for (uint32_t handler = m->load32(m->user, api->battle + FIRST); handler != 0;
         handler = m->load32(m->user, handler + NEXT)) {
        const uint32_t flags = m->load32(m->user, handler + FLAGS);
        if ((flags & REGISTERED) && m->load8(m->user, handler + OWNER) == owner &&
            m->load8(m->user, handler + VALUE) == value && !(flags & REMOVE))
            return handler;
    }
    return 0;
}

uint32_t px_battle_handler_find_next(uint32_t handler, const struct px_battle_handler_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    if (handler == 0) return 0;
    const uint32_t owner = m->load8(m->user, handler + OWNER), value = m->load8(m->user, handler + VALUE);
    /* Unlike find, the registration bit is not tested. */
    for (uint32_t next = m->load32(m->user, handler + NEXT); next != 0; next = m->load32(m->user, next + NEXT))
        if (m->load8(m->user, next + OWNER) == owner && m->load8(m->user, next + VALUE) == value &&
            !(m->load32(m->user, next + FLAGS) & REMOVE))
            return next;
    return 0;
}

void px_battle_handler_set_work(uint32_t handler, uint32_t index, uint32_t value,
                                const struct px_battle_handler_api *api)
{
    if (index >= 7) api->abort(api->user);
    api->memory->store32(api->memory->user, handler + WORK + index * UINT32_C(4), value);
}

uint32_t px_battle_register_ability(uint32_t pokemon, const struct px_battle_handler_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    void *u = api->user;
    const uint32_t ability = api->pokemon_value(u, pokemon, 0x10) & UINT32_C(0xffff);
    for (uint32_t row = 0; row < 0xb6; ++row) {
        const uint32_t at = api->ability_table + row * UINT32_C(8);
        if (m->load8(m->user, at) != ability) continue;
        const uint32_t speed = api->raw_value(u, pokemon, 0xc) & UINT32_C(0xffff);
        const uint32_t id = m->load8(m->user, pokemon + UINT32_C(0x19));
        const uint32_t entries = api->entries(u, m->load32(m->user, at + 4), api->count_scratch);
        const uint32_t word = m->load32(m->user, api->count_scratch);
        /* A zero priority half means the default priority 5. */
        const uint32_t priority = (word >> 16 ? word >> 16 : 5) & UINT32_C(0xff);
        return px_battle_handler_register(4, ability, priority, speed, id, entries, word & UINT32_C(0xffff), api);
    }
    return 0;
}

uint32_t px_battle_register_item(uint32_t pokemon, const struct px_battle_handler_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    void *u = api->user;
    const uint32_t item = m->load16(m->user, pokemon + UINT32_C(0x12));
    if (item == 0) return 0;
    for (uint32_t row = 0; row < 0xb6; ++row) {
        const uint32_t at = api->item_table + row * UINT32_C(8);
        if (m->load32(m->user, at) != item) continue;
        const uint32_t speed = api->raw_value(u, pokemon, 0xc) & UINT32_C(0xffff);
        const uint32_t id = m->load8(m->user, pokemon + UINT32_C(0x19));
        const uint32_t entries = api->entries(u, m->load32(m->user, at + 4), api->count_scratch);
        const uint32_t count = m->load32(m->user, api->count_scratch) & UINT32_C(0xffff);
        return px_battle_handler_register(5, item, 6, speed, id, entries, count, api);
    }
    return 0;
}

uint32_t px_battle_register_move(uint32_t pokemon, uint32_t move, uint32_t sub_priority,
                                 const struct px_battle_handler_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    void *u = api->user;
    for (uint32_t row = 0; row < 0x128; ++row) {
        const uint32_t at = api->move_table + row * UINT32_C(8);
        if (m->load16(m->user, at) != move) continue;
        const uint32_t id = m->load8(m->user, pokemon + UINT32_C(0x19));
        /* Not again for the same move, and at most nine move handlers per battler. */
        unsigned others = 0;
        for (uint32_t handler = px_battle_handler_find(0, id, api); handler != 0;
             handler = px_battle_handler_find_next(handler, api)) {
            if (m->load16(m->user, handler + ID) == move) return 0;
            if (++others > 8) return 0;
        }
        const uint32_t entries = api->entries(u, m->load32(m->user, at + 4), api->count_scratch);
        if (entries == 0) return 0;
        px_battle_handler_register(0, move, 0, sub_priority, id, entries,
                                   m->load32(m->user, api->count_scratch) & UINT32_C(0xffff), api);
        return 1;
    }
    return 0;
}

uint32_t px_battle_register_field(uint32_t kind, uint32_t value, uint32_t work6, uint32_t words,
                                  uint32_t word_count, const struct px_battle_handler_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    void *u = api->user;
    if (kind >= 5) api->abort(u);
    for (uint32_t row = 0; row < 5; ++row) {
        const uint32_t at = api->field_table + row * UINT32_C(8);
        if (m->load8(m->user, at) != kind) continue;
        for (uint32_t handler = px_battle_handler_find(1, value, api); handler != 0;
             handler = px_battle_handler_find_next(handler, api))
            if (m->load16(m->user, handler + ID) == kind) return 0;
        const uint32_t entries = api->entries(u, m->load32(m->user, at + 4), api->count_scratch);
        const uint32_t count = m->load32(m->user, api->count_scratch) & UINT32_C(0xffff);
        const uint32_t handler = px_battle_handler_register(1, kind, 1, 0, value, entries, count, api);
        if (handler != 0) {
            for (uint32_t i = 0; i < word_count; ++i)
                px_battle_handler_set_work(handler, i & UINT32_C(0xff), m->load32(m->user, words + i * UINT32_C(4)), api);
            px_battle_handler_set_work(handler, 6, work6, api);
        }
        return handler;
    }
    return 0;
}
