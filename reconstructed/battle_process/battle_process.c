#include "battle_process.h"

#define ARGS(...) (const uint32_t[]){__VA_ARGS__}, sizeof((const uint32_t[]){__VA_ARGS__}) / sizeof(uint32_t)

static const struct px_pokemon_api *memory(const struct px_battle_process_api *api)
{
    return api->events->memory;
}

static uint32_t call(const struct px_battle_process_api *api, uint32_t address, const uint32_t *args, unsigned count)
{
    return api->call(api->events->user, address, args, count);
}

static void fault(const struct px_battle_process_api *api)
{
    api->events->abort(api->events->user);
}

static uint32_t r8(uint32_t address, const struct px_battle_process_api *api)
{
    return memory(api)->load8(memory(api)->user, address);
}

static uint32_t r16(uint32_t address, const struct px_battle_process_api *api)
{
    return memory(api)->load16(memory(api)->user, address);
}

static uint32_t r32(uint32_t address, const struct px_battle_process_api *api)
{
    return memory(api)->load32(memory(api)->user, address);
}

static void w8(uint32_t address, uint32_t value, const struct px_battle_process_api *api)
{
    memory(api)->store8(memory(api)->user, address, (uint8_t)value);
}

static void w16(uint32_t address, uint32_t value, const struct px_battle_process_api *api)
{
    memory(api)->store16(memory(api)->user, address, (uint16_t)value);
}

static void w32(uint32_t address, uint32_t value, const struct px_battle_process_api *api)
{
    memory(api)->store32(memory(api)->user, address, value);
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

static uint32_t header_id(uint32_t record, const struct px_battle_process_api *api)
{
    return r32(record, api) >> 8 & 0x1f;
}

static uint32_t value(uint32_t pokemon, uint32_t key, const struct px_battle_process_api *api)
{
    return call(api, PX_BATTLE_POKEMON_VALUE, ARGS(pokemon, key));
}

static uint32_t state(uint32_t context, const struct px_battle_process_api *api)
{
    return r32(context + 0xc, api);
}

static void show(uint32_t context, uint32_t message, const struct px_battle_process_api *api)
{
    call(api, PX_BATTLE_SHOW_MESSAGE, ARGS(context, message));
}

static uint32_t position(uint32_t context, uint32_t id, const struct px_battle_process_api *api)
{
    return call(api, PX_BATTLE_POSITION, ARGS(context + 0x120c, id));
}

/* Kinds that hand over to a routine: address, and 1 if it also takes the
 * stack word's low 10 bits. */
static const struct { uint32_t address, low; } routines[PX_BATTLE_PROCESS_KINDS] = {
    [0x04] = {0x10821948, 1}, [0x05] = {0x1081d558, 0}, [0x06] = {0x1081dd1c, 0}, [0x07] = {0x1081f7a0, 0},
    [0x08] = {0x10821b80, 1}, [0x0a] = {0x10820904, 1}, [0x0b] = {0x1081f1a0, 0}, [0x0d] = {0x10823014, 1},
    [0x0e] = {0x1081f648, 0}, [0x10] = {0x10821d98, 0}, [0x11] = {0x10823704, 0}, [0x12] = {0x1081cf54, 0},
    [0x13] = {0x10822e28, 0}, [0x1a] = {0x108266b8, 0}, [0x1c] = {0x10824fb0, 0}, [0x1e] = {0x10824774, 0},
    [0x20] = {0x10824978, 0}, [0x21] = {0x1081f36c, 0}, [0x23] = {0x1081db1c, 0}, [0x25] = {0x10820c9c, 0},
    [0x28] = {0x10825eb8, 0}, [0x2a] = {0x10824288, 0}, [0x2b] = {0x10822cbc, 0}, [0x2d] = {0x1081e0f8, 0},
    [0x33] = {0x10820f3c, 0}, [0x34] = {0x1081defc, 0}, [0x36] = {0x10824414, 0},
};

uint32_t px_battle_process_routine(uint32_t kind)
{
    return kind < PX_BATTLE_PROCESS_KINDS ? routines[kind].address : 0;
}

enum { FAIL, SUCCEED };

/* Commands 0x66 and 0x67 show and hide the ability popup. */
static void popup(uint32_t context, uint32_t command, uint32_t pokemon, const struct px_battle_process_api *api)
{
    call(api, PX_BATTLE_COMMAND, ARGS(state(context, api), command, r8(pokemon + 0x19, api)));
}

/* Kind 0: the battler faints (1086c06c) if alive, or flagged by word +4 bit 1;
 * bit 0 requires HP below its maximum (value 0xe). */
static int faint(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const uint32_t p = battler(context, header_id(record, api), api);
    if (value(p, 0xd, api) == 0 && (r32(record + 4, api) & 2) == 0) return FAIL;
    if ((r32(record + 4, api) & 1) != 0 && value(p, 0xd, api) == value(p, 0xe, api)) return FAIL;
    return call(api, 0x1086c06c, ARGS(context, p)) != 0;
}

/* Kind 3: a message, with the popup around it when bit 23 is set. */
static int message(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    uint32_t p = 0;
    if (header_id(record, api) != 0x1f) p = battler(context, header_id(record, api), api);
    if ((r32(record, api) & 0x800000) != 0 && p != 0) popup(context, 0x66, p, api);
    show(context, record + 4, api);
    if ((r32(record, api) & 0x800000) != 0 && p != 0) popup(context, 0x67, p, api);
    if ((r16(record + 6, api) & 0x8000) != 0) w8(context + 0x1093, r8(context + 0x1093, api) | 0x10, api);
    return SUCCEED;
}

/* Kind 9: 1086bf44, then its message and event 0x57 (key 0x53); a nonzero
 * key 0x53 faints the battler. */
static int event_0x57(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const struct px_battle_event_api *events = api->events;
    const uint32_t p = battler(context, r8(record + 5, api), api);
    if (value(p, 0xd, api) == 0 && (r8(record + 7, api) & 2) == 0) return FAIL;
    if (call(api, 0x1086bf44, ARGS(context, p, r8(record + 6, api), r8(record + 4, api))) == 0) return SUCCEED;
    show(context, record + 8, api);
    const uint32_t v = r8(record + 6, api);
    px_battle_event_push(events);
    px_battle_event_set_id(0x02, r8(p + 0x19, api), events);
    px_battle_event_set_id(0x18, v, events);
    px_battle_event_set_value(0x53, 0, events);
    px_battle_event_dispatch(context, 0x57, events);
    if (px_battle_event_get(0x53, events) == 0) {
        px_battle_event_pop(events);
    } else {
        px_battle_event_pop(events);
        call(api, 0x1086c06c, ARGS(context, p));
    }
    return SUCCEED;
}

/* Kind 0xf: reset every listed live battler's seven stat stages to 6. */
static int reset_stages(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    int result = FAIL;
    for (uint32_t i = 0; i < r8(record + 4, api); ++i) {
        const uint32_t p = battler(context, r8(record + 5 + i, api), api);
        if (value(p, 0xd, api) == 0) continue;
        call(api, PX_BATTLE_COMMAND, ARGS(state(context, api), 0xe, r8(record + 5 + i, api)));
        for (uint32_t s = 0; s < 7; ++s) w8(p + 0x104 + s, 6, api);
        result = SUCCEED;
    }
    return result;
}

/* Kind 0x14: a type change (byte +0x102 unless it is already one of the two
 * types) with command 0x18 and message 0x386. */
static int change_type(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    if (position(context, r8(record + 5, api), api) == 0xa) return FAIL;
    const uint32_t p = battler(context, r8(record + 5, api), api);
    if (value(p, 0xd, api) == 0) return FAIL;
    if (call(api, 0x1086f960, ARGS(p, r8(record + 4, api))) != 0) return FAIL;
    call(api, PX_BATTLE_COMMAND, ARGS(state(context, api), 0x18, r8(record + 5, api), r8(record + 4, api)));
    const uint32_t type = r8(record + 4, api);
    if (r8(p + 0x100, api) != type && r8(p + 0x101, api) != type) w8(p + 0x102, type, api);
    call(api, PX_BATTLE_MESSAGE_COMMAND,
         ARGS(state(context, api), 0x6a, 0x386 /* literal 1081ac68 */, r8(record + 5, api), r8(record + 4, api),
              UINT32_C(0xffff0000) /* literal 1081ac64 */));
    return SUCCEED;
}

/* Kinds 0x15-0x18: set or clear a bit of the flag bytes +0x166 or +0x169;
 * the latter with command 0x1b or 0x1c. */
static int flag(uint32_t context, uint32_t record, uint32_t base, int set, uint32_t command,
                const struct px_battle_process_api *api)
{
    const uint32_t p = battler(context, r8(record + 5, api), api);
    if (value(p, 0xd, api) == 0) return FAIL;
    const uint32_t bit = r8(record + 4, api), at = p + base + (bit >> 3), mask = 1u << (bit & 7);
    w8(at, set ? r8(at, api) | mask : r8(at, api) & ~mask, api);
    if (command != 0) call(api, PX_BATTLE_COMMAND, ARGS(state(context, api), command, r8(p + 0x19, api)));
    return SUCCEED;
}

/* Kind 0x19: event 0xab over keys 0x55, 0x56 and 0x1f, then 108acd0c. */
static int event_0xab(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const struct px_battle_event_api *events = api->events;
    const uint32_t id = header_id(record, api), scaled = r32(record + 8, api);
    const uint32_t first = r8(record + 4, api), second = r8(record + 0xc, api);
    px_battle_event_push(events);
    px_battle_event_set_id(0x02, id, events);
    px_battle_event_set_id(0x55, second, events);
    px_battle_event_set_id(0x56, first, events);
    px_battle_event_set_scaled(0x1f, scaled, events);
    px_battle_event_dispatch(context, 0xab, events);
    const uint32_t result = px_battle_event_get(0x1f, events);
    px_battle_event_pop(events);
    if (call(api, 0x108acd0c, ARGS(r8(record + 0xc, api), r8(record + 4, api), result)) == 0) return FAIL;
    show(context, record + 0x10, api);
    return SUCCEED;
}

/* Kind 0x1b: up to 17 flags from the bit bytes after the length +4. */
static int field_flags(uint32_t record, const struct px_battle_process_api *api)
{
    int result = FAIL;
    for (uint32_t i = 0; i < 0x11; ++i) {
        const uint32_t index = (1 + (i >> 3)) & 0xff;
        if (r8(record + 4, api) <= index || (r8(record + 4 + index, api) & 1u << (i & 7)) == 0) continue;
        const uint32_t address = r8(record + 9, api) != 0 ? 0x108ace4c : 0x108ace70;
        if (call(api, address, ARGS(r8(record + 8, api), i & 0xff)) != 0) result = SUCCEED;
    }
    return result;
}

/* Kind 0x1d: release the side table entry (side +0x274 + 4 id, below 0xb). */
static int release(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const uint32_t id = r8(record + 4, api), side = r32(context + 0x1c, api);
    if (id >= 0xb) fault(api);
    if (id >= 0xb) {
        fault(api);
        return FAIL;
    }
    const uint32_t entry = side + 4 * id;
    if (r32(entry + 0x274, api) == 0) return FAIL;
    const uint32_t held = r32(entry + 0xc, api);
    if (held != 0) {
        call(api, 0x1084d1c8, ARGS(held, id));
        w32(entry + 0xc, 0, api);
    }
    call(api, 0x1086fd30, ARGS(side, id));
    call(api, 0x1086b2b4, ARGS(context, r8(record + 4, api)));
    return SUCCEED;
}

/* Kind 0x22: event 0x9c for a battler holding an item, keeping the stack word
 * that 1086b284 returns. */
static int event_0x9c(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const struct px_battle_event_api *events = api->events;
    const uint32_t p = battler(context, r8(record + 4, api), api), item = r8(record + 5, api);
    if (r16(p + 0x12, api) == 0) return SUCCEED;
    const uint32_t word = context + 0x437c, saved = call(api, 0x1086b284, ARGS(word));
    px_battle_event_push(events);
    px_battle_event_set_id(0x02, r8(p + 0x19, api), events);
    px_battle_event_set_id(0x2f, item, events);
    px_battle_event_dispatch(context, 0x9c, events);
    px_battle_event_pop(events);
    w32(word, saved, api);
    return SUCCEED;
}

/* Kind 0x24: consume the held item (command 0x49 with 108007a4's value when
 * byte +4 is 0), its message, then 1086b140 and 1086b098. */
static int use_item(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const uint32_t p = battler(context, header_id(record, api), api), item = r16(p + 0x12, api);
    if (r8(record + 4, api) == 0) {
        const uint32_t v = call(api, 0x108007a4, ARGS(item));
        call(api, PX_BATTLE_COMMAND, ARGS(state(context, api), 0x49, r8(p + 0x19, api), v));
    }
    show(context, record + 8, api);
    call(api, 0x1086b140, ARGS(context, p, 0, 1));
    call(api, 0x1086b098, ARGS(context, p, item, r8(record + 5, api) == 0));
    return SUCCEED;
}

/* Kind 0x26: a move slot (14 bytes from +0x116) through 1082b064. */
static int move_slot(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const uint32_t p = battler(context, r8(record + 4, api), api);
    call(api, PX_BATTLE_COMMAND,
         ARGS(state(context, api), 0x22, r8(record + 4, api), r8(record + 5, api), r8(record + 6, api), r8(record + 7, api),
              r16(record + 8, api)));
    call(api, 0x1082b064,
         ARGS(p, p + r8(record + 5, api) * 0xe + 0x116, r16(record + 8, api), r8(record + 6, api), r8(record + 7, api) != 0));
    return SUCCEED;
}

/* Kind 0x27: one of five bytes from +0x16d, with command 0x29. */
static int set_byte(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const uint32_t p = battler(context, r8(record + 4, api), api);
    const uint32_t index = r8(record + 5, api), v = r8(record + 6, api);
    call(api, PX_BATTLE_COMMAND, ARGS(state(context, api), 0x29, r8(p + 0x19, api), index, v));
    if (index >= 5) fault(api);
    w8(p + index + 0x16d, v, api);
    return SUCCEED;
}

/* Kind 0x2e: the u16 at +0x150 (0 stored as 1), command 0x15, message. */
static int set_counter(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const uint32_t p = battler(context, r8(record + 4, api), api);
    const uint32_t v = r16(record + 6, api);
    w16(p + 0x150, v == 0 ? 1 : v, api);
    call(api, PX_BATTLE_COMMAND, ARGS(state(context, api), 0x15, r8(record + 4, api), r16(record + 6, api)));
    show(context, record + 8, api);
    return SUCCEED;
}

/* Kind 0x2f: 1086a8f8 for the header battler against byte +6's. */
static int interaction(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const uint32_t q = battler(context, r8(record + 6, api), api);
    const uint32_t p = battler(context, header_id(record, api), api);
    const uint32_t flags = r8(record + 7, api);
    return call(api, 0x1086a8f8,
                ARGS(context, p, q, (flags & 0xf) != 0, api->scratch, r16(record + 4, api), (flags & 0xf0) != 0, record + 8)) != 0;
}

/* Kind 0x30: the first entry (16 bytes from +0x10bc) of the battler at +4. */
static int entry(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const uint32_t id = r8(record + 4, api), count = r8(context + 0x108c /* literal 1081ac6c */, api);
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t e = context + 0x10bc + 0x10 * i;
        if ((r32(e + 4, api) & 0xf) == 8) continue;
        if (r8(r32(e, api) + 0x19, api) != id) continue;
        if (e == 0 || r8(e + 0xd, api) != 0) return FAIL;
        if ((int32_t)call(api, 0x1086a7fc, ARGS(context, e, 0)) < 0) return FAIL;
        show(context, record + 8, api);
        return SUCCEED;
    }
    return FAIL;
}

/* Kind 0x35: clear a battler's bit 6 of +0x1b and word +4, command 0x55. */
static int clear_flag(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    if (position(context, r8(record + 4, api), api) == 0xa) return FAIL;
    const uint32_t p = battler(context, r8(record + 4, api), api);
    const uint32_t flags = r8(p + 0x1b, api);
    if ((flags & 0x40) == 0) return FAIL;
    w8(p + 0x1b, flags & ~0x40u, api);
    w32(p + 4, 0, api);
    call(api, PX_BATTLE_COMMAND, ARGS(state(context, api), 0x55, r8(record + 4, api)));
    show(context, record + 8, api);
    return SUCCEED;
}

/* Kind 0x3a: a form change (form byte +0x153) unless bit 5 of +0x1b is set. */
static int change_form(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const uint32_t p = battler(context, r8(record + 4, api), api);
    if (value(p, 0xd, api) == 0 || (r8(p + 0x1b, api) & 0x20) != 0) return FAIL;
    if (r8(record + 5, api) == r8(p + 0x153, api)) return FAIL;
    if ((r32(record, api) & 0x800000) != 0) popup(context, 0x66, p, api);
    call(api, 0x1086fff0, ARGS(p, r8(record + 5, api)));
    call(api, PX_BATTLE_COMMAND, ARGS(state(context, api), 0x5b, r8(record + 4, api), r8(record + 5, api)));
    show(context, record + 8, api);
    if ((r32(record, api) & 0x800000) != 0) popup(context, 0x67, p, api);
    return SUCCEED;
}

void px_battle_action_execute(uint32_t context, uint32_t record, const struct px_battle_process_api *api)
{
    const uint32_t word = context + 0x437c, previous_word = r32(word, api);
    /* Before any record runs, the previous one counts as a success. */
    uint32_t result = (previous_word & 0x40000000) == 0 ? 1 : previous_word >> 28 & 1;
    const uint32_t low = previous_word & 0x3ff;
    const uint32_t header = r32(record, api);
    if ((header & 0x1000000) != 0 && result == 0) return;
    if ((header & 0x2000000) != 0) {
        const uint32_t p = battler(context, header >> 8 & 0x1f, api);
        if (value(p, 0xd, api) == 0) return;
    }
    const uint32_t kind = r32(record, api) & 0xff;
    if (kind < PX_BATTLE_PROCESS_KINDS && routines[kind].address != 0) {
        result = routines[kind].low ? call(api, routines[kind].address, ARGS(context, record, low))
                                    : call(api, routines[kind].address, ARGS(context, record));
    } else {
        switch (kind) {
        case 0x00: result = faint(context, record, api); break;
        case 0x01:
            if (position(context, header_id(record, api), api) == 0xa) {
                result = FAIL;
                break;
            }
            popup(context, 0x66, battler(context, header_id(record, api), api), api);
            result = SUCCEED;
            break;
        case 0x02:
            popup(context, 0x67, battler(context, header_id(record, api), api), api);
            result = SUCCEED;
            break;
        case 0x03: result = message(context, record, api); break;
        case 0x09: result = event_0x57(context, record, api); break;
        case 0x0f: result = reset_stages(context, record, api); break;
        case 0x14: result = change_type(context, record, api); break;
        case 0x15: result = flag(context, record, 0x166, 1, 0, api); break;
        case 0x16: result = flag(context, record, 0x166, 0, 0, api); break;
        case 0x17: result = flag(context, record, 0x169, 1, 0x1b, api); break;
        case 0x18: result = flag(context, record, 0x169, 0, 0x1c, api); break;
        case 0x19: result = event_0xab(context, record, api); break;
        case 0x1b: result = field_flags(record, api); break;
        case 0x1d: result = release(context, record, api); break;
        case 0x1f:
            /* 108a5aac, then command 0x34. */
            result = call(api, 0x108a5aac, ARGS(r8(record + 4, api), r8(record + 5, api), header_id(record, api), record + 8,
                                                r8(record + 0x18, api))) != 0;
            if (result) call(api, PX_BATTLE_COMMAND, ARGS(state(context, api), 0x34, r8(record + 4, api), r8(record + 5, api)));
            break;
        case 0x22: result = event_0x9c(context, record, api); break;
        case 0x24: result = use_item(context, record, api); break;
        case 0x26: result = move_slot(context, record, api); break;
        case 0x27: result = set_byte(context, record, api); break;
        case 0x29:
            /* 1086af2c; on success byte +0x18 becomes 5. */
            result = call(api, 0x1086af2c, ARGS(context, battler(context, header_id(record, api), api), 1)) != 0;
            if (result) w8(context + 0x18, 5, api);
            break;
        case 0x2c: result = call(api, 0x1086add4, ARGS(context, battler(context, r8(record + 4, api), api), r8(record + 5, api))) != 0; break;
        case 0x2e: result = set_counter(context, record, api); break;
        case 0x2f: result = interaction(context, record, api); break;
        case 0x30: result = entry(context, record, api); break;
        case 0x31: result = call(api, 0x108237fc, ARGS(context, r16(record + 4, api))) != 0; break;
        case 0x32:
            result = call(api, 0x1081c950, ARGS(context, r8(record + 4, api))) != 0;
            if (result) show(context, record + 8, api);
            break;
        case 0x35: result = clear_flag(context, record, api); break;
        case 0x37: {
            const uint32_t p = battler(context, r8(record + 4, api), api);
            result = value(p, 0xd, api) != 0 && call(api, 0x1081fc20, ARGS(context, p, r8(record + 5, api), 0)) != 0;
            if (result) show(context, record + 8, api);
            break;
        }
        case 0x38:
            /* 1086a6e0 (command 0x62 first if byte +0xb is set), message +0xc. */
            if (r8(record + 0xb, api) != 0) call(api, PX_BATTLE_COMMAND, ARGS(state(context, api), 0x62, 0));
            call(api, 0x1086a6e0, ARGS(context, r16(record + 4, api), r8(record + 6, api), r8(record + 7, api),
                                       r8(record + 0xa, api) != 0, r16(record + 8, api)));
            show(context, record + 0xc, api);
            result = SUCCEED;
            break;
        case 0x39:
            call(api, PX_BATTLE_COMMAND, ARGS(state(context, api), 0x62, 0));
            result = SUCCEED;
            break;
        case 0x3a: result = change_form(context, record, api); break;
        case 0x3b:
            w8(r32(context + 0xdb4, api) + 4, r8(record + 4, api), api);
            result = SUCCEED;
            break;
        case 0x3c: {
            const uint32_t field = r32(context + 0xdb4, api);
            w8(field + 5, r8(field + 5, api) | 1, api);
            result = SUCCEED;
            break;
        }
        case 0x3e:
            /* A message of at most two arguments at +8, as command 100. */
            if ((r16(record + 0xa, api) >> 7 & 0x7f) > 2) fault(api);
            call(api, PX_BATTLE_COMMAND,
                 ARGS(state(context, api), 100, r8(record + 4, api), r8(record + 5, api), r16(record + 0xa, api) & 0x7f,
                      r16(record + 8, api), r32(record + 0xc, api) & 0xffff, r32(record + 0x10, api) & 0xffff));
            result = SUCCEED;
            break;
        default:
            /* Kinds 0xc, 0x3d and from 0x3f fault; the result stays the previous one. */
            fault(api);
            break;
        }
    }
    const uint32_t now = r32(word, api);
    w32(word, (result ? now | UINT32_C(0x30000000) : now & ~UINT32_C(0x10000000)) | UINT32_C(0x40000000), api);
}
