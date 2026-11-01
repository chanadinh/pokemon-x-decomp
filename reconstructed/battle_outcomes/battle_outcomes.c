#include "battle_outcomes.h"

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

static uint32_t value(uint32_t pokemon, uint32_t key, const struct px_battle_process_api *api)
{
    return call(api, PX_BATTLE_POKEMON_VALUE, ARGS(pokemon, key));
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

static void show(uint32_t context, uint32_t message, const struct px_battle_process_api *api)
{
    call(api, PX_BATTLE_SHOW_MESSAGE, ARGS(context, message));
}

static uint32_t header_id(uint32_t record, const struct px_battle_process_api *api)
{
    return r32(record, api) >> 8 & 0x1f;
}

uint32_t px_battle_kind_set_values(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    if (position(context, r8(record + 0xe, api), api) == 0xa) return 0;
    const uint32_t p = battler(context, r8(record + 0xe, api), api);
    /* Keys 8..0xc, one mask bit each, with command 0x14. */
    for (uint32_t k = 0; k < 5; ++k) {
        if ((r8(record + 0xf, api) & 1u << k) == 0) continue;
        call(api, 0x10871250, ARGS(p, 8 + k, r16(record + 4 + 2 * k, api)));
        command(context, ARGS(0x14, r8(record + 0xe, api), 8 + k, r16(record + 4 + 2 * k, api) & 0xff), api);
    }
    show(context, record + 0x10, api);
    return 1;
}

uint32_t px_battle_kind_faint(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const struct px_battle_event_api *events = api->events;
    if (position(context, r8(record + 4, api), api) == 0xa) return 0;
    const uint32_t p = battler(context, r8(record + 4, api), api);
    if (value(p, 0xd, api) == 0 && r8(record + 5, api) == 0) return 0;
    uint32_t source = 0;
    if (header_id(record, api) != 0x1f && r16(record + 6, api) != 0) source = battler(context, header_id(record, api), api);
    w32(api->scratch, source, api);
    w16(api->scratch + 4, r16(record + 6, api), api);
    show(context, record + 8, api);
    const uint32_t frame = source != 0 ? api->scratch : 0;
    /* Event 0xb7 (keys 4 the battler, 3 the source id). */
    const uint32_t id = r8(p + 0x19, api), from = header_id(record, api);
    px_battle_event_push(events);
    px_battle_event_set_id(0x04, id, events);
    px_battle_event_set_id(0x03, from, events);
    px_battle_event_dispatch(context, 0xb7, events);
    px_battle_event_pop(events);
    value(p, 0xd, api);
    const uint32_t own = r8(p + 0x19, api);
    w16(p + 0x10, 0, api);
    call(api, 0x108003fc, ARGS(r32(p, api)));
    command(context, ARGS(4, own), api);
    command(context, ARGS(0x4b, own, 0), api);
    call(api, 0x108646c4, ARGS(context, p, 0, frame));
    return 1;
}

uint32_t px_battle_kind_leave(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const uint32_t p = battler(context, r8(record + 0x4c, api), api);
    if (call(api, 0x10865c4c, ARGS(context)) != 0) return 0;
    const uint32_t mark = r8(battler(context, r8(record + 0x4c, api), api) + 0x171, api);
    if (mark != 0 && ((mark - 1) & 0xff) < 0x18) return 0;
    if ((r32(battler(context, r8(record + 0x4c, api), api) + 0xa0, api) & 7) != 0 || r8(context + 0x18, api) != 0) return 0;
    show(context, record + 4, api);
    if (call(api, 0x10866c68, ARGS(context, p, r8(record + 0x4d, api) != 0)) == 0) return 0;
    const uint32_t slot = call(api, 0x10873164, ARGS(r32(context + 4, api), r32(context + 8, api), r8(record + 0x4c, api)));
    call(api, 0x10810cc0, ARGS(r32(context, api), slot));
    show(context, record + 0x28, api);
    w8(context + 0x18, 1, api);
    return 1;
}

uint32_t px_battle_kind_heal(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const uint32_t p = battler(context, r8(record + 4, api), api);
    const uint32_t hp = (r16(record + 6, api) + r16(p + 0x10, api)) & 0xffff;
    w16(p + 0x10, hp, api);
    if (hp > r16(p + 0xe, api)) w16(p + 0x10, r16(p + 0xe, api), api);
    command(context, ARGS(3, r8(record + 4, api), r16(record + 6, api)), api);
    /* A pending battler (context +0xd68 + id) joins the list at +0xd98
     * (24 bytes, count +0xdb0) once. */
    const uint32_t id = r8(record + 4, api), list = context + 0xca4;
    if (id >= 0x18) {
        fault(api);
    } else if (r8(list + 0xc4 + id, api) != 0) {
        w8(list + 0xc4 + id, 0, api);
        int present = 0;
        for (uint32_t j = 0; j < 0x18 && !present; j += 2)
            present = r8(list + 0xf4 + j, api) == id || r8(list + 0xf5 + j, api) == id;
        if (!present) {
            const uint32_t count = r8(list + 0x10c, api);
            if (count >= 0x18) {
                fault(api);
            } else {
                w8(list + 0x10c, count + 1, api);
                w8(list + 0xf4 + count, id, api);
            }
        }
    }
    show(context, record + 8, api);
    const uint32_t slot = call(api, 0x10873164, ARGS(r32(context + 4, api), r32(context + 8, api), r8(record + 4, api)));
    if (slot == 0xa) return 1;
    const uint32_t side = call(api, 0x1087425c, ARGS(r8(record + 4, api)));
    const uint32_t entry = call(api, 0x10871b10, ARGS(r32(context + 4, api), slot));
    call(api, 0x1086c720, ARGS(context, side, entry, entry));
    command(context, ARGS(0x44, side, entry, entry, 1), api);
    call(api, 0x1086c500, ARGS(context));
    return 1;
}

uint32_t px_battle_kind_swap(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    if (r8(record + 4, api) == r8(record + 5, api)) return 0;
    const uint32_t side = call(api, 0x1087425c, ARGS(r8(record + 4, api)));
    if (call(api, 0x1087425c, ARGS(r8(record + 5, api))) != side) return 0;
    const uint32_t p = battler(context, r8(record + 4, api), api), q = battler(context, r8(record + 5, api), api);
    if (value(p, 0xd, api) == 0 || value(q, 0xd, api) == 0) return 0;
    /* The side's party (28 bytes each from table +4, at most 4 sides). */
    const uint32_t table = r32(context + 8, api);
    if (side >= 4) fault(api);
    const uint32_t party = table + side * 0x1c + 4;
    const int16_t a = (int16_t)call(api, 0x1086fc8c, ARGS(party, p));
    const int16_t b = (int16_t)call(api, 0x1086fc8c, ARGS(party, q));
    if (a < 0 || b < 0) return 0;
    call(api, 0x10867e74, ARGS(context, side, (uint32_t)a & 0xff, (uint32_t)b & 0xff, 0));
    show(context, record + 8, api);
    call(api, 0x10867d64, ARGS(context, side, (uint32_t)a & 0xff, (uint32_t)b & 0xff));
    return 1;
}
