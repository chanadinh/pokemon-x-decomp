#include "battle_event.h"

enum {
    KEYS = 4, VALUES = 0xc4, MAXIMA = 0x244, MINIMA = 0x3c4, KINDS = 0x544,
    FIRST = 0x60, FREE_COUNT = 0x64, DEPTH = 0x68,
    PREVIOUS = 0, NEXT = 4, ENTRIES = 8, OWNER = 0x10, FLAGS = 0x18, WORK = 0x1c, VALUE = 0x3a
};
#define RUNNING UINT32_C(0x1000000)
#define DISABLED UINT32_C(0x2000000)
#define ACCEPTS_7C UINT32_C(0x4000000)
#define REMOVE UINT32_C(0x8000000)
#define REENTRY UINT32_C(0x10000000)
#define REGISTERED UINT32_C(0x20000000)
#define SUSPENDED UINT32_C(0x40000000)
#define VISIT UINT32_C(0x80000000)

static uint32_t key_at(uint32_t position, const struct px_battle_event_api *api)
{
    return api->memory->load16(api->memory->user, api->stack + KEYS + position * UINT32_C(2));
}

static void set_key(uint32_t position, uint32_t key, const struct px_battle_event_api *api)
{
    api->memory->store16(api->memory->user, api->stack + KEYS + position * UINT32_C(2), (uint16_t)key);
}

void px_battle_event_push(const struct px_battle_event_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    uint32_t position = m->load32(m->user, api->stack);
    /* Skip the entries of the current frame. */
    if (position < PX_BATTLE_EVENT_CAPACITY) {
        while (key_at(position, api) != 0) {
            ++position;
            m->store32(m->user, api->stack, position);
            if (position >= PX_BATTLE_EVENT_CAPACITY) break;
        }
    }
    if (position >= PX_BATTLE_EVENT_CAPACITY - 1) {
        api->overflow(api->user);
        return;
    }
    m->store32(m->user, api->stack, position + 1);
    set_key(position, 1, api);
}

void px_battle_event_pop(const struct px_battle_event_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    uint32_t position = m->load32(m->user, api->stack);
    if (position == 0) {
        api->abort(api->user);
        return;
    }
    /* Clear the frame's entries, then its marker. */
    for (;;) {
        position &= UINT32_C(0xffff);
        if (position >= PX_BATTLE_EVENT_CAPACITY || key_at(position, api) == 0) break;
        set_key(position, 0, api);
        ++position;
    }
    position = m->load32(m->user, api->stack) - 1;
    m->store32(m->user, api->stack, position);
    set_key(position, 0, api);
    /* Return to the entry after the previous marker, or to zero. */
    position = m->load32(m->user, api->stack);
    if (position == 0) return;
    for (;;) {
        --position;
        m->store32(m->user, api->stack, position);
        if (key_at(position, api) == 1) {
            m->store32(m->user, api->stack, position + 1);
            return;
        }
        if (position == 0) return;
    }
}

/* Shared body of the four setters: the first free slot of the frame. A
 * duplicate key or a full stack aborts (which returns); a duplicate is
 * overwritten and a full stack writes slot 0. */
static uint32_t free_slot(uint32_t key, const struct px_battle_event_api *api)
{
    if (key == 0 || key == 1) api->abort(api->user);
    uint32_t position = api->memory->load32(api->memory->user, api->stack);
    for (; position < PX_BATTLE_EVENT_CAPACITY; ++position) {
        const uint32_t existing = key_at(position, api);
        if (existing == 0) break;
        if (existing == key) {
            api->abort(api->user);
            break;
        }
    }
    if (position >= PX_BATTLE_EVENT_CAPACITY) {
        api->abort(api->user);
        position = 0;
    }
    return position;
}

static void set_entry(uint32_t key, uint32_t value, uint32_t kind, const struct px_battle_event_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const uint32_t position = free_slot(key, api);
    set_key(position, key, api);
    m->store32(m->user, api->stack + VALUES + position * UINT32_C(4), value);
    m->store32(m->user, api->stack + MINIMA + position * UINT32_C(4), 0);
    m->store32(m->user, api->stack + MAXIMA + position * UINT32_C(4), 0);
    m->store8(m->user, api->stack + KINDS + position, (uint8_t)kind);
}

void px_battle_event_set_id(uint32_t key, uint32_t value, const struct px_battle_event_api *api)
{
    set_entry(key, value, 4, api);
}

void px_battle_event_set_value(uint32_t key, uint32_t value, const struct px_battle_event_api *api)
{
    set_entry(key, value, 1, api);
}

void px_battle_event_set_scaled(uint32_t key, uint32_t value, const struct px_battle_event_api *api)
{
    set_entry(key, value, 0, api);
}

void px_battle_event_set_multiplier(uint32_t key, uint32_t value, uint32_t minimum, uint32_t maximum,
                                    const struct px_battle_event_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const uint32_t position = free_slot(key, api);
    set_key(position, key, api);
    m->store32(m->user, api->stack + VALUES + position * UINT32_C(4), value);
    m->store32(m->user, api->stack + MAXIMA + position * UINT32_C(4), maximum);
    m->store32(m->user, api->stack + MINIMA + position * UINT32_C(4), minimum);
    m->store8(m->user, api->stack + KINDS + position, 3);
}

uint32_t px_battle_event_get(uint32_t key, const struct px_battle_event_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    if (key == 0 || key == 1) api->abort(api->user);
    for (uint32_t position = m->load32(m->user, api->stack); position < PX_BATTLE_EVENT_CAPACITY; ++position) {
        const uint32_t existing = key_at(position, api);
        if (existing == key) return m->load32(m->user, api->stack + VALUES + position * UINT32_C(4));
        if (existing == 0) break;
    }
    api->abort(api->user);
    return 0;
}

void px_battle_event_dispatch(uint32_t context, uint32_t event, const struct px_battle_event_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const uint32_t depth = api->battle + DEPTH;
    m->store32(m->user, depth, m->load32(m->user, depth) + 1);
    px_battle_event_run_handlers(context, event, PX_BATTLE_ANY_OWNER, 1, api);
    const uint32_t remaining = m->load32(m->user, depth) - 1;
    m->store32(m->user, depth, remaining);
    /* Leaving the outermost dispatch clears every registration depth. */
    if (remaining == 0)
        for (uint32_t handler = m->load32(m->user, api->battle + FIRST); handler != 0;
             handler = m->load32(m->user, handler + NEXT))
            m->store16(m->user, handler + FLAGS, 0);
}

static uint32_t flags_of(uint32_t handler, const struct px_pokemon_api *m)
{
    return m->load32(m->user, handler + FLAGS);
}

static void set_flags(uint32_t handler, uint32_t flags, const struct px_pokemon_api *m)
{
    m->store32(m->user, handler + FLAGS, flags);
}

static int eligible(uint32_t handler, uint32_t flags, uint32_t event, uint32_t owner,
                    const struct px_battle_event_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    if ((flags & RUNNING) && !(flags & REENTRY)) return 0;
    if (flags & (DISABLED | SUSPENDED)) return 0;
    if (owner != PX_BATTLE_ANY_OWNER && m->load8(m->user, handler + OWNER) != owner) return 0;
    /* A handler registered at depth d runs only in dispatches deeper than d. */
    const uint32_t registered = flags & UINT32_C(0xffff);
    if (registered != 0 && m->load32(m->user, api->battle + DEPTH) <= registered) return 0;
    if (!(flags & REGISTERED)) return 0;
    if (event == 0x7c && !(flags & ACCEPTS_7C)) return 0;
    return 1;
}

/* Unlinks a handler whose removal was requested and returns it to the free
 * array; a handler still running only records the request. */
static void release(uint32_t handler, const struct px_battle_event_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const uint32_t flags = flags_of(handler, m);
    if (!(flags & REGISTERED)) return;
    if (flags & RUNNING) {
        set_flags(handler, flags | REMOVE, m);
        return;
    }
    if (m->load32(m->user, api->battle + FIRST) == handler)
        m->store32(m->user, api->battle + FIRST, m->load32(m->user, handler + NEXT));
    const uint32_t previous = m->load32(m->user, handler + PREVIOUS);
    if (previous != 0) m->store32(m->user, previous + NEXT, m->load32(m->user, handler + NEXT));
    const uint32_t next = m->load32(m->user, handler + NEXT);
    if (next != 0) m->store32(m->user, next + PREVIOUS, m->load32(m->user, handler + PREVIOUS));
    const uint32_t count = m->load32(m->user, api->battle + FREE_COUNT);
    if (count == 0) {
        api->abort(api->user);
        return;
    }
    for (uint32_t offset = 0; offset <= 0x38; offset += 4) m->store32(m->user, handler + offset, 0);
    m->store32(m->user, api->battle + FREE_COUNT, count - 1);
    m->store32(m->user, api->free_handlers + (count - 1) * UINT32_C(4), handler);
}

void px_battle_event_run_handlers(uint32_t context, uint32_t event, uint32_t owner, uint32_t check,
                                  const struct px_battle_event_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    uint32_t handler = m->load32(m->user, api->battle + FIRST);
    if (handler == 0) return;
    for (;;) {
        uint32_t next = m->load32(m->user, handler + NEXT);
        if (next != 0) set_flags(next, flags_of(next, m) | VISIT, m);
        const uint32_t flags = flags_of(handler, m);
        if (eligible(handler, flags, event, owner, api)) {
            const uint32_t entries = m->load32(m->user, handler + ENTRIES);
            const uint32_t count = flags >> 16 & UINT32_C(0xff);
            for (uint32_t i = 0; i < count; ++i) {
                if (m->load8(m->user, entries + i * UINT32_C(8)) != event) continue;
                if (check != 0 && api->blocked(api->user, context, handler, event) != 0) break;
                set_flags(handler, flags_of(handler, m) | RUNNING, m);
                const uint32_t value = m->load8(m->user, handler + VALUE);
                const uint32_t function = m->load32(m->user, entries + i * UINT32_C(8) + UINT32_C(4));
                api->call(api->user, function, handler, context, value, handler + WORK);
                uint32_t after = flags_of(handler, m);
                after &= (after & REENTRY) ? ~REENTRY : ~RUNNING;
                set_flags(handler, after, m);
                if (after & REMOVE) {
                    if (next != 0) {
                        if (!(flags_of(next, m) & REGISTERED)) {
                            /* Search from the current link. The original never
                             * advances here, so an unregistered successor loops forever. */
                            next = m->load32(m->user, handler + NEXT);
                            while (next != 0 && !(flags_of(next, m) & REGISTERED)) {
                            }
                        }
                        if (next != 0) set_flags(next, flags_of(next, m) | VISIT, m);
                    }
                    release(handler, api);
                }
                break;
            }
        }
        if (next == 0) return;
        const uint32_t next_flags = flags_of(next, m);
        if ((next_flags & REGISTERED) && (next_flags & VISIT)) {
            /* Continue with the marked successor. */
            set_flags(next, next_flags & ~VISIT, m);
            handler = next;
            continue;
        }
        handler = m->load32(m->user, handler + NEXT);
        if (handler == 0) return;
    }
}
