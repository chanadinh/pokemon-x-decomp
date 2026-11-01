#include "battle_changes.h"

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
    uint32_t all[8] = {r32(context + 0xc, api)};
    for (unsigned i = 0; i < count; ++i) all[1 + i] = args[i];
    call(api, PX_BATTLE_COMMAND, all, count + 1);
}

static uint32_t header_id(uint32_t record, const struct px_battle_process_api *api)
{
    return r32(record, api) >> 8 & 0x1f;
}

static uint32_t source_of(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    return header_id(record, api) == 0x1f ? 0 : battler(context, header_id(record, api), api);
}

/* The popup commands 0x66/0x67 when bit 23 is set. */
static void popup(uint32_t context, uint32_t record, uint32_t command_id, uint32_t pokemon,
                  const struct px_battle_process_api *api)
{
    if ((r32(record, api) & 0x800000) != 0) command(context, ARGS(command_id, r8(pokemon + 0x19, api)), api);
}

uint32_t px_battle_kind_condition(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const struct px_battle_event_api *events = api->events;
    if (position(context, r8(record + 6, api), api) == 0xa) return 0;
    const uint32_t target = battler(context, r8(record + 6, api), api);
    const uint32_t source = source_of(context, record, api);
    if (alive(target, api) == 0) return 0;
    if ((r8(record + 7, api) & 1) != 0 && call(api, 0x10874348, ARGS(target)) != 0x12) return 0;
    /* Event 0x89 (key 4) may clear key 0x53 to refuse. */
    px_battle_event_push(events);
    px_battle_event_set_id(0x04, r8(target + 0x19, api), events);
    px_battle_event_set_value(0x53, 1, events);
    px_battle_event_dispatch(context, 0x89, events);
    if (px_battle_event_get(0x53, events) == 0) {
        px_battle_event_pop(events);
        return 0;
    }
    px_battle_event_pop(events);
    popup(context, record, 0x66, source, api);
    if ((r8(record + 7, api) & 2) != 0)
        call(api, 0x1086a6e0, ARGS(context, r16(record + 8, api), r8(record + 0xa, api), r8(record + 0xb, api), 0, 0));
    call(api, 0x108671c0, ARGS(context, target, r16(record + 4, api), record + 0xc));
    popup(context, record, 0x67, source, api);
    return 1;
}

uint32_t px_battle_kind_hp(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const struct px_battle_event_api *events = api->events;
    uint32_t result = 0;
    for (uint32_t i = 0; i < r8(record + 4, api); ++i) {
        if (position(context, r8(record + 7 + i, api), api) == 0xa) continue;
        const uint32_t p = battler(context, r8(record + 7 + i, api), api);
        if (alive(p, api) == 0) continue;
        const int report = r8(record + 5, api) == 0;
        const int32_t change = (int32_t)r32(record + 0x14 + 4 * i, api);
        const uint32_t id = r8(p + 0x19, api);
        if (change > 0) {
            /* Restore (16-bit), capped at the maximum. */
            const uint32_t hp = (r16(p + 0x10, api) + ((uint32_t)change & 0xffff)) & 0xffff;
            w16(p + 0x10, hp, api);
            if (hp > r16(p + 0xe, api)) w16(p + 0x10, r16(p + 0xe, api), api);
            command(context, ARGS(3, id, (uint32_t)change & 0xffff), api);
        } else if (change < 0) {
            call(api, 0x10867a04, ARGS(context, p, (uint32_t)-change, 0));
        }
        if (report && position(context, id, api) != 0xa) command(context, ARGS(0x48, id), api);
        /* A held item reacts (event 0x9c, key 0x2f = 1), keeping the stack word. */
        if (r8(record + 6, api) == 0 && r16(p + 0x12, api) != 0) {
            const uint32_t word = context + 0x437c, saved = call(api, 0x1086b284, ARGS(word));
            px_battle_event_push(events);
            px_battle_event_set_id(0x02, r8(p + 0x19, api), events);
            px_battle_event_set_id(0x2f, 1, events);
            px_battle_event_dispatch(context, 0x9c, events);
            px_battle_event_pop(events);
            w32(word, saved, api);
        }
        result = 1;
    }
    return result;
}

uint32_t px_battle_kind_status(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const uint32_t source = source_of(context, record, api);
    const uint32_t quiet = (r16(record + 0x16, api) & 0x7f) == 0 && r8(record + 0xd, api) == 0;
    if (position(context, r8(record + 0xf, api), api) == 0xa) return 0;
    const uint32_t target = battler(context, r8(record + 0xf, api), api);
    if (alive(target, api) == 0) return 0;
    /* 10862858 reports a reason the status cannot be applied. */
    if (call(api, 0x10862858, ARGS(context, target, source, r16(record + 4, api), r32(record + 8, api), r8(record + 0x10, api),
                                   r8(record + 0xc, api) != 0)) != 0)
        return 0;
    popup(context, record, 0x66, source, api);
    call(api, 0x108629b8, ARGS(context, target, source, r16(record + 4, api), r32(record + 8, api), quiet, r8(record + 0xe, api) != 0,
                               record + 0x14));
    popup(context, record, 0x67, source, api);
    return 1;
}

uint32_t px_battle_kind_types(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    if (position(context, r8(record + 6, api), api) == 0xa) return 0;
    const uint32_t p = battler(context, r8(record + 6, api), api);
    if (alive(p, api) == 0) return 0;
    if (r16(p + 0xc, api) == PX_BATTLE_ARCEUS) return 0;
    command(context, ARGS(0x17, r8(record + 6, api), r16(record + 4, api)), api);
    const uint32_t before = call(api, 0x10870994, ARGS(p));
    const uint32_t packed = r16(record + 4, api);
    const uint32_t first = packed & 0x1f, second = packed >> 5 & 0x1f, third = packed >> 10 & 0x1f;
    const uint32_t old1 = r8(p + 0x100, api), old2 = r8(p + 0x101, api), old3 = r8(p + 0x102, api);
    w8(p + 0x100, first, api);
    w8(p + 0x101, second, api);
    w8(p + 0x102, third, api);
    /* Unchanged when the pair matches in either order and the third type is the same. */
    const int same_pair = (first == old1 && second == old2) || (second == old1 && first == old2);
    const int changed = !(same_pair && third == old3);
    if (changed) popup(context, record, 0x66, p, api);
    if (r8(record + 7, api) == 0) {
        const uint32_t v = r16(record + 4, api);
        /* A single type with no third (0x12) that differs: message 0x383 naming it. */
        if ((v & 0x1f) == (v >> 5 & 0x1f) && (v & 0x7c00) == 0x4800 && v != before)
            call(api, PX_BATTLE_MESSAGE_COMMAND,
                 ARGS(r32(context + 0xc, api), 0x6a, 0x383 /* literal 10823010 */, r8(record + 6, api), v & 0x1f,
                      UINT32_C(0xffff0000) /* literal 1082300c */));
        else if (r8(record + 8, api) != 0)
            /* The generic message; the original sets no stack arguments here. */
            call(api, PX_BATTLE_MESSAGE_COMMAND, ARGS(r32(context + 0xc, api), 0x69, 0x4a, UINT32_C(0xffff0000)));
    }
    if (changed) popup(context, record, 0x67, p, api);
    return 1;
}
