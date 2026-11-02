#include "battle_cures.h"

#define ARGS(...) (const uint32_t[]){__VA_ARGS__}, sizeof((const uint32_t[]){__VA_ARGS__}) / sizeof(uint32_t)

static const struct px_battle_process_api *base(const struct px_battle_cures_api *api)
{
    return api->base;
}

static uint32_t call(const struct px_battle_cures_api *api, uint32_t address, const uint32_t *args, unsigned count)
{
    return base(api)->call(base(api)->events->user, address, args, count);
}

static const struct px_pokemon_api *memory(const struct px_battle_cures_api *api)
{
    return base(api)->events->memory;
}

static uint32_t r8(uint32_t address, const struct px_battle_cures_api *api)
{
    return memory(api)->load8(memory(api)->user, address);
}

static uint32_t r16(uint32_t address, const struct px_battle_cures_api *api)
{
    return memory(api)->load16(memory(api)->user, address);
}

static uint32_t r32(uint32_t address, const struct px_battle_cures_api *api)
{
    return memory(api)->load32(memory(api)->user, address);
}

static void w8(uint32_t address, uint32_t value, const struct px_battle_cures_api *api)
{
    memory(api)->store8(memory(api)->user, address, (uint8_t)value);
}

static void w16(uint32_t address, uint32_t value, const struct px_battle_cures_api *api)
{
    memory(api)->store16(memory(api)->user, address, (uint16_t)value);
}

static void w32(uint32_t address, uint32_t value, const struct px_battle_cures_api *api)
{
    memory(api)->store32(memory(api)->user, address, value);
}

static void fault(const struct px_battle_cures_api *api)
{
    base(api)->events->abort(base(api)->events->user);
}

/* The inline battler lookup: ids from 0x18 and empty slots fault. */
static uint32_t battler(uint32_t context, uint32_t id, const struct px_battle_cures_api *api)
{
    const uint32_t table = r32(context + 8, api);
    if (id >= 0x18) fault(api);
    const uint32_t slot = table + 0x84 + 4 * id;
    if (r32(slot, api) == 0) fault(api);
    return r32(slot, api);
}

static uint32_t value(uint32_t pokemon, uint32_t key, const struct px_battle_cures_api *api)
{
    return call(api, PX_BATTLE_POKEMON_VALUE, ARGS(pokemon, key));
}

static uint32_t position(uint32_t context, uint32_t id, const struct px_battle_cures_api *api)
{
    return call(api, PX_BATTLE_POSITION, ARGS(context + 0x120c, id));
}

static void command(uint32_t context, const uint32_t *args, unsigned count, const struct px_battle_cures_api *api)
{
    uint32_t all[8] = {r32(context + 0xc, api)};
    for (unsigned i = 0; i < count; ++i) all[1 + i] = args[i];
    call(api, PX_BATTLE_COMMAND, all, count + 1);
}

static void show(uint32_t context, uint32_t message, const struct px_battle_cures_api *api)
{
    call(api, PX_BATTLE_SHOW_MESSAGE, ARGS(context, message));
}

static uint32_t header_id(uint32_t record, const struct px_battle_cures_api *api)
{
    return r32(record, api) >> 8 & 0x1f;
}

static int popup_flag(uint32_t record, const struct px_battle_cures_api *api)
{
    return (r32(record, api) & 0x800000) != 0;
}

/* A nested event keeping the action-stack word (1086b284). */
static void item_event(uint32_t context, uint32_t pokemon, uint32_t reason, const struct px_battle_cures_api *api)
{
    const struct px_battle_event_api *events = base(api)->events;
    const uint32_t word = context + 0x437c, saved = call(api, 0x1086b284, ARGS(word));
    px_battle_event_push(events);
    px_battle_event_set_id(0x02, r8(pokemon + 0x19, api), events);
    px_battle_event_set_id(0x2f, reason, events);
    px_battle_event_dispatch(context, 0x9c, events);
    px_battle_event_pop(events);
    w32(word, saved, api);
}

/* Event 0xa6 (keys 2, 0x2e the item) may set key 0x42 to block a change of
 * held item; then event 0xa7 runs and the record fails. */
static int item_blocked(uint32_t context, uint32_t pokemon, uint32_t item, const struct px_battle_cures_api *api)
{
    const struct px_battle_event_api *events = base(api)->events;
    const uint32_t word = context + 0x437c;
    uint32_t saved = call(api, 0x1086b284, ARGS(word));
    px_battle_event_push(events);
    px_battle_event_set_id(0x02, r8(pokemon + 0x19, api), events);
    px_battle_event_set_id(0x2e, item, events);
    px_battle_event_set_value(0x42, 0, events);
    px_battle_event_dispatch(context, 0xa6, events);
    const int blocked = px_battle_event_get(0x42, events) != 0;
    px_battle_event_pop(events);
    w32(word, saved, api);
    if (!blocked) return 0;
    saved = call(api, 0x1086b284, ARGS(word));
    px_battle_event_push(events);
    px_battle_event_set_id(0x02, r8(pokemon + 0x19, api), events);
    px_battle_event_dispatch(context, 0xa7, events);
    px_battle_event_pop(events);
    w32(word, saved, api);
    return 1;
}

uint32_t px_battle_kind_cure(uint32_t context, uint32_t record, uint32_t low, const struct px_battle_cures_api *api)
{
    const uint32_t source = battler(context, header_id(record, api), api);
    uint32_t result = 0;
    if (popup_flag(record, api)) command(context, ARGS(0x66, r8(source + 0x19, api)), api);
    for (uint32_t k = 0; k < r8(record + 0x11, api); ++k) {
        const uint32_t t = battler(context, r8(record + 5 + k, api), api);
        if (call(api, 0x108007cc, ARGS(r32(t, api), 2)) != 0) continue;
        if (value(t, 0xd, api) == 0) continue;
        uint32_t mode = r8(record + 4, api);
        for (;;) {
            const uint32_t current = call(api, 0x1086d8ac, ARGS(t));
            const uint32_t select = mode & 0xff;
            uint32_t condition = 0;
            if (select == 0) break;
            if (select == 0x26) {
                /* The current status. */
                if (current == 0) break;
                condition = current;
                mode = 0;
            } else if (select == 0x27) {
                /* The current status, then condition 6. */
                if (current != 0) {
                    condition = current;
                    mode = 6;
                } else {
                    if ((r32(t + 0x34, api) & 7) != 0) condition = 6;
                    mode = 0;
                }
            } else if (select == 0x28) {
                /* The first active condition of the list; repeated until none. */
                for (uint32_t i = 0;; ++i) {
                    if (i >= 6) break;
                    const uint32_t c = r16(api->cures + 2 * i, api);
                    if (c == 0) {
                        mode = 0;
                        break;
                    }
                    if (c >= 0x26) fault(api);
                    if ((r32(t + 0x1c + 4 * c, api) & 7) != 0) {
                        condition = c;
                        break;
                    }
                }
            } else {
                if (select >= 0x26) fault(api);
                if ((r32(t + 0x1c + 4 * select, api) & 7) != 0) condition = select;
                mode = 0;
            }
            if (condition == 0) break;
            const uint32_t out = api->frames;
            call(api, 0x108695c0, ARGS(context, t, condition, out));
            if (r8(record + 0x12, api) != 0) {
                show(context, record + 0x14, api);
            } else {
                const uint32_t text = context + 0x433c;
                if (call(api, 0x10882188, ARGS(condition, r32(out, api), t, low, text)) != 0) {
                    show(context, text, api);
                    /* The built message is cleared (0x24 bytes, kind bits last). */
                    for (uint32_t w = 0; w < 3; ++w) w32(text + 4 * w, 0, api);
                    const uint32_t flags = r16(text + 2, api);
                    for (uint32_t w = 3; w < 9; ++w) w32(text + 4 * w, 0, api);
                    w16(text + 2, flags & ~0x7fu, api);
                }
            }
            if (condition == 0x13 && r16(t + 0x12, api) != 0) item_event(context, t, 7, api);
            result = 1;
        }
    }
    if (popup_flag(record, api)) command(context, ARGS(0x67, r8(source + 0x19, api)), api);
    return result;
}

uint32_t px_battle_kind_set_item(uint32_t context, uint32_t record, const struct px_battle_cures_api *api)
{
    const struct px_battle_event_api *events = base(api)->events;
    const uint32_t t = battler(context, r8(record + 6, api), api), word = context + 0x437c;
    if (r8(record + 6, api) != header_id(record, api) && item_blocked(context, t, r16(record + 4, api), api)) return 0;
    if (popup_flag(record, api)) command(context, ARGS(0x66, header_id(record, api)), api);
    const uint32_t old = r16(t + 0x12, api);
    show(context, record + 0xc, api);
    if (r8(record + 7, api) != 0) {
        w16(t + 0x14, 0, api);
        command(context, ARGS(0x2e, r8(record + 6, api)), api);
    }
    call(api, 0x1086b140, ARGS(context, t, r16(record + 4, api), 0));
    /* Removing an item: event 0x9b (keys 2, 0x2e the old item, 0x52 = 1). */
    if (r16(record + 4, api) == 0 && r8(record + 0xa, api) != 0) {
        const uint32_t saved = call(api, 0x1086b284, ARGS(word));
        px_battle_event_push(events);
        px_battle_event_set_id(0x02, r8(t + 0x19, api), events);
        px_battle_event_set_id(0x2e, old, events);
        px_battle_event_set_id(0x52, 1, events);
        px_battle_event_dispatch(context, 0x9b, events);
        px_battle_event_pop(events);
        w32(word, saved, api);
    }
    if (popup_flag(record, api)) command(context, ARGS(0x67, header_id(record, api)), api);
    if (r8(record + 8, api) != 0) {
        const uint32_t q = battler(context, r8(record + 9, api), api);
        w16(q + 0x14, 0, api);
        command(context, ARGS(0x2e, r8(record + 9, api)), api);
    }
    if (r16(t + 0x12, api) != 0) item_event(context, t, 0xf, api);
    return 1;
}

uint32_t px_battle_kind_swap_items(uint32_t context, uint32_t record, const struct px_battle_cures_api *api)
{
    const uint32_t a = battler(context, r8(record + 4, api), api);
    const uint32_t b = battler(context, header_id(record, api), api);
    const uint32_t item_a = r16(a + 0x12, api), item_b = r16(b + 0x12, api);
    if (item_blocked(context, a, item_b, api)) return 0;
    if (popup_flag(record, api)) command(context, ARGS(0x66, r8(b + 0x19, api)), api);
    show(context, record + 8, api);
    show(context, record + 0x2c, api);
    show(context, record + 0x50, api);
    if (popup_flag(record, api)) command(context, ARGS(0x67, r8(b + 0x19, api)), api);
    call(api, 0x1086b140, ARGS(context, b, item_a, 0));
    call(api, 0x1086b140, ARGS(context, a, item_b, 0));
    if (r16(b + 0x12, api) != 0) item_event(context, b, 7, api);
    if (r16(a + 0x12, api) != 0) item_event(context, a, 7, api);
    return 1;
}

static int no_targets(uint32_t list, const struct px_battle_cures_api *api)
{
    return !(r8(list + 0x6f, api) == 0 && r8(list + 0x71, api) == 0) && r8(list + 0x6e, api) == 0;
}

uint32_t px_battle_kind_use_move(uint32_t context, uint32_t record, const struct px_battle_cures_api *api)
{
    const uint32_t user = battler(context, r8(record + 4, api), api);
    const uint32_t target = battler(context, r8(record + 5, api), api);
    const uint32_t frame = api->frames + 0x20, stage = context + 0x4360;
    call(api, 0x10869c5c, ARGS(context, r16(record + 8, api), user, frame));
    call(api, 0x10869870, ARGS(r32(context + 0x1260, api)));
    call(api, 0x10869870, ARGS(r32(context + 0x1268, api)));
    call(api, 0x108696b0, ARGS(r32(context + 0x1268, api), target));
    w8(r32(context + 0x1268, api) + 0x71, 1, api);
    /* The target list (+0x1268): pointers, count +0x6e, cursor +0x70;
     * fainted or unpositioned targets are removed. */
    const uint32_t list = r32(context + 0x1268, api);
    w8(list + 0x70, 0, api);
    while (r8(list + 0x70, api) < r8(list + 0x6e, api)) {
        const uint32_t i = r8(list + 0x70, api);
        w8(list + 0x70, i + 1, api);
        const uint32_t p = r32(list + 4 * i, api);
        if (p == 0) break;
        if (value(p, 0xd, api) != 0 && position(context, r8(p + 0x19, api), api) != 0xa) continue;
        call(api, 0x10869784, ARGS(list, p));
    }
    if (no_targets(r32(context + 0x1268, api), api)) {
        call(api, PX_BATTLE_MESSAGE_COMMAND, ARGS(r32(context + 0xc, api), 0x69, 0x4a, UINT32_C(0xffff0000) /* literal 108261e4 */));
        return 0;
    }
    call(api, 0x1082399c, ARGS(context, frame, user));
    call(api, 0x10826800, ARGS(context, frame, user, r32(context + 0x1268, api), stage));
    call(api, 0x10863790, ARGS(context, frame, user, r32(context + 0x1268, api), stage));
    call(api, 0x10863604, ARGS(context, frame, user, r32(context + 0x1268, api)));
    if (no_targets(r32(context + 0x1268, api), api)) return 0;
    /* The field struct (context +0xdb4, 12 bytes) is saved, set for this
     * move, and restored afterwards. */
    const uint32_t field = r32(context + 0xdb4, api);
    uint32_t saved[3];
    for (uint32_t w = 0; w < 3; ++w) saved[w] = r32(field + 4 * w, api);
    w16(field, 0, api);
    w8(field + 2, 10, api);
    w8(field + 3, 10, api);
    w8(field + 4, 0, api);
    w8(field + 5, r8(field + 5, api) & ~3u, api);
    w32(field + 8, 0, api);
    const uint32_t targets = r32(context + 0x1268, api), now = r32(context + 0xdb4, api);
    const uint32_t mode = r8(targets + 0x6f, api);
    w8(now + 2, position(context, r8(user + 0x19, api), api), api);
    w8(now + 3, 10, api);
    w8(now + 5, r8(now + 5, api) & ~3u, api);
    w32(now + 8, 0, api);
    if (mode == 1 && r8(targets + 0x6e, api) != 0 && r32(targets, api) != 0)
        w8(now + 3, position(context, r8(r32(targets, api) + 0x19, api), api), api);
    w8(r32(context + 0xdb4, api) + 2, r8(record + 6, api), api);
    w8(r32(context + 0xdb4, api) + 4, 1, api);
    call(api, 0x108225dc, ARGS(context, frame, user, r32(context + 0x1268, api), stage, 1));
    const uint32_t after = r32(context + 0xdb4, api), result = r8(after + 5, api) & 1;
    for (uint32_t w = 0; w < 3; ++w) w32(after + 4 * w, saved[w], api);
    return result;
}

uint32_t px_battle_kind_cure_side(uint32_t context, uint32_t record, const struct px_battle_cures_api *api)
{
    const uint32_t slot = call(api, 0x10873164, ARGS(r32(context + 4, api), r32(context + 8, api), header_id(record, api)));
    const uint32_t ids = api->frames + 0x10, list = api->frames + 0x18;
    const uint32_t count = call(api, 0x108672c0, ARGS(r32(context + 4, api), slot | 0x800, list));
    /* The side's live battlers. */
    uint32_t live = 0;
    for (uint32_t i = 0; i < count; i = (i + 1) & 0xff) {
        const uint32_t p = call(api, 0x108707d4, ARGS(r32(context + 8, api), r8(list + i, api)));
        if (p == 0 || value(p, 0xd, api) == 0) continue;
        w8(ids + live, r8(p + 0x19, api), api);
        live = (live + 1) & 0xff;
    }
    for (uint32_t k = 0; k < live; k = (k + 1) & 0xff) {
        const uint32_t p = battler(context, r8(ids + k, api), api);
        int cured = 0;
        const uint32_t flags = r8(p + 0x169, api);
        if ((flags & 8) != 0) {
            const uint32_t locked = (r32(p + 0xa0, api) & 7) != 0, own = r8(p + 0x19, api);
            w8(p + 0x169, flags & ~8u, api);
            command(context, ARGS(0x1c, r8(p + 0x19, api), 3), api);
            if ((r32(p + 0x84, api) & 7) != 0) call(api, 0x108695c0, ARGS(context, p, 0x1a, 0));
            command(context, ARGS(0x38, own, 0), api);
            if (!locked) {
                /* Mark its entry (16 bytes from +0x10bc) as done. */
                for (uint32_t i = 0; i < r8(context + 0x108c, api); ++i) {
                    const uint32_t e = context + 0x10bc + 0x10 * i;
                    if ((r32(e + 4, api) & 0xf) == 8) continue;
                    if (r8(r32(e, api) + 0x19, api) != own) continue;
                    if (e != 0) w8(e + 0xd, 1, api);
                    break;
                }
            }
            const uint32_t mark = r8(p + 0x171, api);
            if (mark != 0 && ((mark - 1) & 0xff) < 0x18) call(api, 0x1086844c, ARGS(context, p, 0, 1));
            cured = 1;
        }
        if (call(api, 0x108651d8, ARGS(context, p, 1)) != 0) cured = 1;
        if ((r32(p + 0x94, api) & 7) != 0) {
            call(api, 0x108695c0, ARGS(context, p, 0x1e, 0));
            cured = 1;
        }
        if ((r32(p + 0x9c, api) & 7) != 0) {
            call(api, 0x108695c0, ARGS(context, p, 0x20, 0));
            cured = 1;
        }
        if (cured)
            call(api, PX_BATTLE_MESSAGE_COMMAND, ARGS(r32(context + 0xc, api), 0x6a, 0x441 /* literal 108246f0 */, r8(ids + k, api),
                                                      UINT32_C(0xffff0000) /* literal 108246ec */));
    }
    return 1;
}
