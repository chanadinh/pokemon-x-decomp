#include "battle_kinds.h"

#define ARGS(...) (const uint32_t[]){__VA_ARGS__}, sizeof((const uint32_t[]){__VA_ARGS__}) / sizeof(uint32_t)

static uint32_t call(const struct px_battle_process_api *api, uint32_t address, const uint32_t *args, unsigned count)
{
    return api->call(api->events->user, address, args, count);
}

static uint32_t r8(uint32_t address, const struct px_battle_process_api *api)
{
    return api->events->memory->load8(api->events->memory->user, address);
}

static uint32_t r16(uint32_t address, const struct px_battle_process_api *api)
{
    return api->events->memory->load16(api->events->memory->user, address);
}

static uint32_t r32(uint32_t address, const struct px_battle_process_api *api)
{
    return api->events->memory->load32(api->events->memory->user, address);
}

static void w8(uint32_t address, uint32_t value, const struct px_battle_process_api *api)
{
    api->events->memory->store8(api->events->memory->user, address, (uint8_t)value);
}

static void w16(uint32_t address, uint32_t value, const struct px_battle_process_api *api)
{
    api->events->memory->store16(api->events->memory->user, address, (uint16_t)value);
}

static void w32(uint32_t address, uint32_t value, const struct px_battle_process_api *api)
{
    api->events->memory->store32(api->events->memory->user, address, value);
}

/* The inline battler lookup: ids from 0x18 and empty slots fault. */
static uint32_t battler(uint32_t context, uint32_t id, const struct px_battle_process_api *api)
{
    const uint32_t table = r32(context + 8, api);
    if (id >= 0x18) api->events->abort(api->events->user);
    const uint32_t slot = table + 0x84 + 4 * id;
    if (r32(slot, api) == 0) api->events->abort(api->events->user);
    return r32(slot, api);
}

static uint32_t header_id(uint32_t record, const struct px_battle_process_api *api)
{
    return r32(record, api) >> 8 & 0x1f;
}

static uint32_t value(uint32_t pokemon, uint32_t key, const struct px_battle_process_api *api)
{
    return call(api, PX_BATTLE_POKEMON_VALUE, ARGS(pokemon, key));
}

static void command(uint32_t context, const uint32_t *args, unsigned count, const struct px_battle_process_api *api)
{
    uint32_t all[8] = {r32(context + 0xc, api)};
    for (unsigned i = 0; i < count; ++i) all[1 + i] = args[i];
    call(api, PX_BATTLE_COMMAND, all, count + 1);
}

static void show(uint32_t context, uint32_t message, const struct px_battle_process_api *api)
{
    call(api, PX_BATTLE_SHOW_MESSAGE, ARGS(context, message));
}

static int popup_flag(uint32_t record, const struct px_battle_process_api *api)
{
    return (r32(record, api) & 0x800000) != 0;
}

/* The stack word is kept across a nested event (1086b284). */
static uint32_t save_word(uint32_t context, const struct px_battle_process_api *api)
{
    return call(api, 0x1086b284, ARGS(context + 0x437c));
}

uint32_t px_battle_kind_stages(uint32_t context, uint32_t record, uint32_t low, const struct px_battle_process_api *api)
{
    const struct px_battle_event_api *events = api->events;
    uint32_t source = 0;
    if (header_id(record, api) != 0x1f) source = battler(context, header_id(record, api), api);
    /* The popup shows once, for the first target the change applies to. */
    int shown = 0;
    for (uint32_t i = 0; i < r8(record + 0x16, api); ++i) {
        if (call(api, PX_BATTLE_POSITION, ARGS(context + 0x120c, r8(record + 0xc + i, api))) == 0xa) continue;
        const uint32_t p = battler(context, r8(record + 0xc + i, api), api);
        if (value(p, 0xd, api) == 0) continue;
        if (call(api, 0x108722e8, ARGS(p, r8(record + 4, api), (uint32_t)(int32_t)(int8_t)r8(record + 0x17, api))) == 0) continue;
        shown = 1;
        if (popup_flag(record, api) && source != 0) command(context, ARGS(0x66, r8(source + 0x19, api)), api);
        break;
    }
    uint32_t result = 0;
    for (uint32_t i = 0; i < r8(record + 0x16, api); ++i) {
        if (call(api, PX_BATTLE_POSITION, ARGS(context + 0x120c, r8(record + 0xc + i, api))) == 0xa) continue;
        const uint32_t p = battler(context, r8(record + 0xc + i, api), api);
        if (value(p, 0xd, api) == 0) continue;
        const uint32_t flags = r8(record + 0x18, api);
        const uint32_t message = (flags & 8) != 0 ? record + 0x1c : 0;
        /* Event 5 (keys 3, 4, 0x12, 0x53) may block the change, unless bit 4
         * skips it (blocked) or there is no source (allowed). */
        uint32_t blocked;
        if ((flags & 0x10) != 0) {
            blocked = 1;
        } else if (header_id(record, api) == 0x1f) {
            blocked = 0;
        } else {
            const uint32_t from = battler(context, header_id(record, api), api);
            const uint32_t initial = call(api, 0x1080049c, ARGS(0, 0xd));
            px_battle_event_push(events);
            px_battle_event_set_id(0x03, r8(from + 0x19, api), events);
            px_battle_event_set_id(0x04, r8(p + 0x19, api), events);
            px_battle_event_set_id(0x12, 0, events);
            px_battle_event_set_value(0x53, initial, events);
            px_battle_event_dispatch(context, 5, events);
            blocked = px_battle_event_get(0x53, events) != 0;
            px_battle_event_pop(events);
        }
        const uint32_t later = r8(record + 0x18, api);
        const uint32_t done =
            call(api, 0x10860e74,
                 ARGS(context, header_id(record, api), p, r8(record + 4, api), (uint32_t)(int32_t)(int8_t)r8(record + 0x17, api),
                      0x1f, low, r32(record + 8, api), (later & 2) >> 1, 1 & ~later, message, blocked));
        if (done == 0) continue;
        if ((r8(record + 0x18, api) & 8) == 0) show(context, record + 0x1c, api);
        result = 1;
    }
    if (shown && popup_flag(record, api) && source != 0) command(context, ARGS(0x67, r8(source + 0x19, api)), api);
    return result;
}

uint32_t px_battle_kind_weather(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const struct px_battle_event_api *events = api->events;
    const uint32_t p = battler(context, header_id(record, api), api), word = context + 0x437c;
    const uint32_t weather = r8(record + 4, api);
    if (weather == 0) {
        /* Clearing: event 0x82. */
        if (r8(record + 6, api) == 0) return 0;
        if (popup_flag(record, api)) command(context, ARGS(0x66, r8(p + 0x19, api)), api);
        show(context, record + 8, api);
        const uint32_t saved = save_word(context, api);
        px_battle_event_push(events);
        px_battle_event_dispatch(context, 0x82, events);
        px_battle_event_pop(events);
        w32(word, saved, api);
    } else {
        const uint32_t turns = r8(record + 5, api);
        if (weather >= 5) return 0;
        const uint32_t field = r32(context + 0x1c, api), current = r8(field + 1, api);
        /* The same weather again only extends a limited one to permanent. */
        if (current == weather) {
            if (turns != 0xff) return 0;
            if (current != 0 && r32(field + 4, api) == 0xff) return 0;
        }
        if (popup_flag(record, api)) command(context, ARGS(0x66, r8(p + 0x19, api)), api);
        const uint32_t set = r8(record + 4, api), now = r32(context + 0x1c, api), length = r8(record + 5, api);
        w8(now + 1, set, api);
        w32(now + 4, length, api);
        command(context, ARGS(0x45, set, length), api);
        /* Event 0x86 (key 0x3a); unless key 0x42 is set, event 0x87. */
        const uint32_t saved = save_word(context, api);
        px_battle_event_push(events);
        px_battle_event_set_id(0x3a, set, events);
        px_battle_event_set_value(0x42, 0, events);
        px_battle_event_dispatch(context, 0x86, events);
        if (px_battle_event_get(0x42, events) == 0) px_battle_event_dispatch(context, 0x87, events);
        px_battle_event_pop(events);
        w32(word, saved, api);
        show(context, record + 8, api);
    }
    if (popup_flag(record, api)) command(context, ARGS(0x67, r8(p + 0x19, api)), api);
    return 1;
}

uint32_t px_battle_kind_ability(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const struct px_battle_event_api *events = api->events;
    const uint32_t p = battler(context, r8(record + 6, api), api);
    const uint32_t old = value(p, 0x10, api) & 0xff;
    if (call(api, 0x1086fe6c, ARGS(old, PX_BATTLE_FIXED_ABILITIES, 2)) != 0) return 0;
    if (value(p, 0xd, api) == 0) return 0;
    /* The same ability again needs byte +7 (force). */
    if (r8(record + 7, api) == 0 && r16(record + 4, api) == old) return 0;
    if (popup_flag(record, api)) command(context, ARGS(0x66, header_id(record, api)), api);
    command(context, ARGS(0x53, r8(record + 6, api), r16(record + 4, api)), api);
    show(context, record + 0xc, api);
    /* Event 0x93 (keys 2, 0xf old, 0x10 new) before the change. */
    uint32_t saved = save_word(context, api);
    const uint32_t id = r8(record + 6, api), ability = r16(record + 4, api);
    px_battle_event_push(events);
    px_battle_event_set_id(0x02, id, events);
    px_battle_event_set_id(0x0f, old, events);
    px_battle_event_set_id(0x10, ability, events);
    px_battle_event_dispatch(context, 0x93, events);
    px_battle_event_pop(events);
    w32(context + 0x437c, saved, api);
    call(api, 0x108679ac, ARGS(p));
    w16(p + 0x14e /* literal 10824b9c */, r16(record + 4, api) & 0xff, api);
    command(context, ARGS(0x20, r8(record + 6, api), r16(record + 4, api)), api);
    call(api, 0x108678f8, ARGS(p));
    command(context, ARGS(0x67, r8(record + 6, api)), api);
    /* A new ability takes effect at once (event 0x94, as on entry). */
    if (r16(record + 4, api) != old) {
        saved = save_word(context, api);
        px_battle_event_push(events);
        px_battle_event_set_id(0x02, r8(record + 6, api), events);
        px_battle_event_dispatch(context, 0x94, events);
        px_battle_event_pop(events);
        w32(context + 0x437c, saved, api);
    }
    if (popup_flag(record, api)) command(context, ARGS(0x67, header_id(record, api)), api);
    call(api, 0x108677e4, ARGS(context, p, old, r16(record + 4, api) & 0xff));
    return 1;
}
