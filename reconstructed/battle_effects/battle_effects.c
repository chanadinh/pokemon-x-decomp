#include "battle_effects.h"

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

static void fault(const struct px_battle_process_api *api)
{
    api->events->abort(api->events->user);
}

/* The inline battler lookup: ids from 0x18 and empty slots fault. */
static uint32_t battler(uint32_t context, uint32_t id, const struct px_battle_process_api *api)
{
    const uint32_t table = r32(context + 8, api);
    if (id >= 0x18) fault(api);
    const uint32_t slot = table + 0x84 + 4 * id;
    if (r32(slot, api) == 0) fault(api);
    return r32(slot, api);
}

static uint32_t alive(uint32_t pokemon, const struct px_battle_process_api *api)
{
    return call(api, PX_BATTLE_POKEMON_VALUE, ARGS(pokemon, 0xd));
}

static uint32_t position(uint32_t context, uint32_t id, const struct px_battle_process_api *api)
{
    return call(api, PX_BATTLE_POSITION, ARGS(context + 0x120c, id));
}

static void command(uint32_t context, const uint32_t *args, unsigned count, const struct px_battle_process_api *api)
{
    uint32_t all[12] = {r32(context + 0xc, api)};
    for (unsigned i = 0; i < count; ++i) all[1 + i] = args[i];
    call(api, PX_BATTLE_COMMAND, all, count + 1);
}

static void show(uint32_t context, uint32_t message, const struct px_battle_process_api *api)
{
    call(api, PX_BATTLE_SHOW_MESSAGE, ARGS(context, message));
}

uint32_t px_battle_kind_value(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    uint32_t source = 0;
    if (r8(record + 7, api) != 0x1f) source = battler(context, r8(record + 7, api), api);
    if (position(context, r8(record + 6, api), api) == 0xa) return 0;
    const uint32_t p = battler(context, r8(record + 6, api), api);
    if (alive(p, api) == 0) return 0;
    if (call(api, 0x10863f5c, ARGS(context, p, source, r16(record + 4, api))) == 0) return 0;
    if ((r16(record + 0xa, api) & 0x7f) != 0) show(context, record + 8, api);
    return 1;
}

uint32_t px_battle_kind_set_stages(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    if (position(context, r8(record + 4, api), api) == 0xa) return 0;
    const uint32_t p = battler(context, r8(record + 4, api), api);
    if (alive(p, api) == 0) return 0;
    for (uint32_t stat = 1; stat <= 7; ++stat) call(api, 0x108714fc, ARGS(p, stat, r8(record + 4 + stat, api)));
    if (r8(record + 0xc, api) <= 3) w8(p + 0x155, r8(record + 0xc, api), api);
    command(context,
            ARGS(0xc, r8(record + 4, api), r8(record + 5, api), r8(record + 6, api), r8(record + 7, api), r8(record + 8, api),
                 r8(record + 9, api), r8(record + 0xa, api), r8(record + 0xb, api), r8(record + 0xc, api)),
            api);
    return 1;
}

uint32_t px_battle_kind_clear_drops(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const uint32_t p = battler(context, r8(record + 4, api), api);
    if (alive(p, api) == 0) return 0;
    command(context, ARGS(0xd, r8(record + 4, api)), api);
    /* Every lowered stage (signed, below the neutral 6) returns to 6. */
    uint32_t raised = 0;
    for (uint32_t s = 0; s < 7; ++s) {
        if ((int8_t)r8(p + 0x104 + s, api) >= 6) continue;
        w8(p + 0x104 + s, 6, api);
        raised = 1;
    }
    return raised;
}

int32_t px_battle_field_clear_message(uint32_t flag)
{
    static const int16_t messages[0xf] = {0x84, 0x88, 0x8c, 0x90, 0x94, 0x98, 0x9c, 0xa0, 0xa4, -1, -1, 0xac, 0xb0, 0xb4, 0xd8};
    return flag < 0xf ? messages[flag] : -1;
}

uint32_t px_battle_kind_clear_field(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    uint32_t result = 0;
    for (uint32_t i = 0; i < 0x11; ++i) {
        const uint32_t index = (1 + (i >> 3)) & 0xff;
        if (r8(record + 4, api) <= index || (r8(record + 4 + index, api) & 1u << (i & 7)) == 0) continue;
        if (call(api, 0x108ace94, ARGS(r8(record + 8, api), i & 0xff)) == 0) continue;
        const int32_t message = px_battle_field_clear_message(i);
        if (message >= 0)
            call(api, PX_BATTLE_MESSAGE_COMMAND,
                 ARGS(r32(context + 0xc, api), 0x69, (uint32_t)message, r8(record + 8, api), UINT32_C(0xffff0000) /* literal 108267fc */));
        result = 1;
    }
    return result;
}

uint32_t px_battle_kind_side_entry(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const uint32_t index = r8(record + 4, api), word = r32(record + 8, api), side = r32(context + 0x1c, api);
    if (index == 0xa) {
        const uint32_t other = r8(record + 5, api);
        if (call(api, 0x1086fde0, ARGS(side, other, word)) == 0) return 0;
        command(context, ARGS(0x26, other, word), api);
        command(context, ARGS(0x62, 0), api);
        command(context, ARGS(0x59, r8(record + 5, api)), api);
    } else {
        const int fallback = r8(record + 0x30, api) != 0;
        if (index >= 0xb) fault(api);
        if (index == 0xa) fault(api);
        if (call(api, 0x108712c0, ARGS(side, index, word, 0)) != 0) {
            command(context, ARGS(0x24, index, word), api);
        } else {
            /* Otherwise a 6-bit value from bits 3 (word type 3) or 9 (type 4). */
            if (!fallback) return 0;
            uint32_t value;
            if ((word & 7) == 3) value = word >> 3 & 0x3f;
            else if ((word & 7) == 4) value = word >> 9 & 0x3f;
            else return 0;
            if (value == 0x1f) return 0;
            call(api, 0x1086f878, ARGS(r32(context + 0x1c, api), index, value));
            command(context, ARGS(0x25, index, value), api);
        }
    }
    show(context, record + 0xc, api);
    return 1;
}

uint32_t px_battle_kind_transfer(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const struct px_battle_event_api *events = api->events;
    const uint32_t p = battler(context, r8(record + 4, api), api), q = battler(context, r8(record + 5, api), api);
    if ((r32(p + 0x5c, api) & 7) != 0) {
        /* Event 0x73 (key 2), dispatched inline without the blocked check. */
        px_battle_event_push(events);
        px_battle_event_set_id(0x02, r8(q + 0x19, api), events);
        const uint32_t depth = events->battle + 0x68;
        w32(depth, r32(depth, api) + 1, api);
        px_battle_event_run_handlers(context, 0x73, PX_BATTLE_ANY_OWNER, 0, events);
        const uint32_t remaining = r32(depth, api) - 1;
        w32(depth, remaining, api);
        if (remaining == 0)
            for (uint32_t h = r32(events->battle + 0x60, api); h != 0; h = r32(h + 4, api)) w16(h + 0x18, 0, api);
        px_battle_event_pop(events);
    }
    call(api, 0x1082aea4, ARGS(q, p));
    command(context, ARGS(0x2a, r8(record + 4, api), r8(record + 5, api)), api);
    if (r16(q + 0x208, api) != 0) command(context, ARGS(0x5d, position(context, r8(record + 5, api), api)), api);
    return 1;
}
