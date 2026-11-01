#include "battle_restores.h"

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

static int popup_flag(uint32_t record, const struct px_battle_process_api *api)
{
    return (r32(record, api) & 0x800000) != 0;
}

/* 108007cc(data, 2): set for battlers that take no effect (checked first). */
static uint32_t unaffected(uint32_t pokemon, const struct px_battle_process_api *api)
{
    return call(api, 0x108007cc, ARGS(r32(pokemon, api), 2));
}

uint32_t px_battle_kind_restore(uint32_t context, uint32_t record, uint32_t low, const struct px_battle_process_api *api)
{
    const uint32_t source = battler(context, header_id(record, api), api);
    const uint32_t target = battler(context, r8(record + 6, api), api);
    if (unaffected(target, api) != 0) return 0;
    if (value(target, 0xd, api) == 0) return 0;
    if (value(target, 0xd, api) == value(target, 0xe, api)) return 0;
    if (popup_flag(record, api)) command(context, ARGS(0x66, r8(source + 0x19, api)), api);
    uint32_t result = 1;
    if (r8(record + 7, api) == 0) {
        /* Heal block (bits of +0x58): message 0x37a and context +0x1093 bit 5. */
        uint32_t blocked = 0;
        if ((r32(target + 0x58, api) & 7) != 0) {
            call(api, PX_BATTLE_MESSAGE_COMMAND, ARGS(r32(context + 0xc, api), 0x6a, 0x37a /* literal 10821b78 */,
                                                      r8(target + 0x19, api), UINT32_C(0xffff0000) /* literal 10821b70 */));
            w8(context + 0x1093, r8(context + 0x1093, api) | 0x20, api);
            blocked = 1;
        }
        result = blocked ^ 1;
    }
    if (result != 0) {
        const uint32_t amount = r16(record + 4, api);
        const uint32_t slot = call(api, PX_BATTLE_POSITION, ARGS(context + 0x120c, r8(target + 0x19, api)));
        if (slot != 0xa) command(context, ARGS(0x57, slot, 0x29e /* literal 10821b74 */), api);
        call(api, 0x10865788, ARGS(context, target, amount, 1));
        if ((r16(record + 0xa, api) & 0x7f) != 0)
            show(context, record + 8, api);
        else if (low != 0)
            call(api, PX_BATTLE_MESSAGE_COMMAND, ARGS(r32(context + 0xc, api), 0x6a, 0x392 /* literal 10821b7c */, r8(record + 6, api), low,
                                                      UINT32_C(0xffff0000)));
    }
    if (popup_flag(record, api)) command(context, ARGS(0x67, r8(source + 0x19, api)), api);
    return result;
}

uint32_t px_battle_kind_pp(uint32_t context, uint32_t record, uint32_t low, const struct px_battle_process_api *api)
{
    (void)low;
    battler(context, header_id(record, api), api);
    const uint32_t target = battler(context, r8(record + 5, api), api);
    if ((unaffected(target, api) != 0 || value(target, 0xd, api) == 0) && (r8(record + 7, api) & 2) == 0) return 0;
    const uint32_t slot = r8(record + 6, api), first = (r8(record + 7, api) & 1) == 0;
    /* Full already: the pair checked starts at +0x116 (first) or +0x11c. */
    const uint32_t check = target + slot * 0xe + (first ? 0x116 : 0x11c);
    if (r16(check, api) != 0 && r8(check + 2, api) == r8(check + 3, api)) return 0;
    const uint32_t amount = r8(record + 4, api), id = r8(target + 0x19, api);
    if (slot >= 4) fault(api);
    const uint32_t entry = target + slot * 0xe + 0x116;
    const uint32_t pp = first ? 2 : 8, other = first ? 8 : 2;
    const uint32_t restored = (r8(entry + pp, api) + amount) & 0xff;
    w8(entry + pp, restored, api);
    if (restored > r8(entry + pp + 1, api)) w8(entry + pp, r8(entry + pp + 1, api), api);
    if (r8(entry + 0xc, api) != 0) w8(entry + other, r8(entry + pp, api), api);
    command(context, ARGS(first ? 9 : 8, id, slot, amount), api);
    show(context, record + 8, api);
    return 1;
}

uint32_t px_battle_kind_item(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const struct px_battle_event_api *events = api->events;
    const uint32_t p = battler(context, r8(record + 4, api), api);
    if (value(p, 0xd, api) == 0) return 0;
    const uint32_t holder = call(api, 0x108abf28, ARGS(p, r16(record + 6, api)));
    if (holder == 0) return 0;
    int used = 0;
    const uint32_t queued = call(api, 0x10863e4c, ARGS(r32(context + 0xc, api), 0x49, 0x6e, 0));
    const uint32_t word = context + 0x437c, saved = call(api, 0x1086b284, ARGS(word));
    /* Event 0x7c (scaled keys 2 and 3), dispatched inline without the blocked check. */
    const uint32_t from = header_id(record, api);
    px_battle_event_push(events);
    px_battle_event_set_scaled(0x02, r8(p + 0x19, api), events);
    px_battle_event_set_scaled(0x03, from, events);
    const uint32_t depth = events->battle /* literal 1081dd14 */ + 0x68;
    w32(depth, r32(depth, api) + 1, api);
    px_battle_event_run_handlers(context, 0x7c, PX_BATTLE_ANY_OWNER, 0, events);
    const uint32_t remaining = r32(depth, api) - 1;
    w32(depth, remaining, api);
    if (remaining == 0)
        for (uint32_t h = r32(events->battle + 0x60, api); h != 0; h = r32(h + 4, api)) w16(h + 0x18, 0, api);
    px_battle_event_pop(events);
    /* Records the event ran, one of them successful: the item counts as used. */
    const uint32_t now = r32(context + 0x437c /* literal 1081dd18 */, api);
    if ((now & 0x40000000) != 0 && (now & 0x20000000) != 0) {
        used = 1;
        const uint32_t v = call(api, 0x108007a4, ARGS(r16(record + 6, api)));
        call(api, 0x108659b8, ARGS(r32(context + 0xc, api), queued, 0x49, r8(record + 4, api), v));
    }
    w32(word, saved, api);
    call(api, 0x108a5bd4, ARGS(holder));
    if (r8(record + 5, api) != 0 && call(api, 0x108007a4, ARGS(r16(record + 6, api))) != 0) {
        w8(p + 0x16c, r8(p + 0x16c, api) | 1, api);
        command(context, ARGS(0x1f, r8(p + 0x19, api), 0), api);
    }
    if (used) call(api, 0x1086b098, ARGS(context, p, r16(record + 6, api), 1));
    return 1;
}

uint32_t px_battle_kind_transform(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const struct px_battle_event_api *events = api->events;
    const uint32_t p = battler(context, header_id(record, api), api);
    const uint32_t q = battler(context, r8(record + 4, api), api);
    if ((r8(p + 0x1b, api) & 0x40) != 0 || (r8(q + 0x1b, api) & 0x40) != 0) return 0;
    if (call(api, 0x10874348, ARGS(p)) != 0x12) return 0;
    const uint32_t before = value(p, 0x10, api) & 0xff;
    if (call(api, 0x1082a720, ARGS(p, q)) == 0) return 0;
    const uint32_t pid = r8(p + 0x19, api), qid = r8(q + 0x19, api);
    if (popup_flag(record, api)) command(context, ARGS(0x66, pid), api);
    call(api, 0x108ae5e0, ARGS(p));
    call(api, 0x108679ac, ARGS(p));
    call(api, 0x108678f8, ARGS(p));
    command(context, ARGS(0x5f, pid, qid), api);
    show(context, record + 8, api);
    if (popup_flag(record, api)) command(context, ARGS(0x67, r8(p + 0x19, api)), api);
    /* A copied ability takes effect at once (event 0x94). */
    const uint32_t after = value(p, 0x11, api) & 0xff;
    if (before != after) {
        const uint32_t id = r8(p + 0x19, api), word = context + 0x437c, saved = call(api, 0x1086b284, ARGS(word));
        px_battle_event_push(events);
        px_battle_event_set_id(0x02, id, events);
        px_battle_event_dispatch(context, 0x94, events);
        px_battle_event_pop(events);
        w32(word, saved, api);
        call(api, 0x108677e4, ARGS(context, p, before, after));
    }
    return 1;
}
