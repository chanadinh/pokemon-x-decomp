#include "battle_low_4.h"

static const struct px_pokemon_api *mem(const struct px_battle_low_4_api *api)
{
    return api->base->memory;
}

static uint32_t load8(uint32_t a, const struct px_battle_low_4_api *api)
{
    return mem(api)->load8(mem(api)->user, a);
}

static uint32_t load32(uint32_t a, const struct px_battle_low_4_api *api)
{
    return mem(api)->load32(mem(api)->user, a);
}

static void store8(uint32_t a, uint32_t v, const struct px_battle_low_4_api *api)
{
    mem(api)->store8(mem(api)->user, a, (uint8_t)v);
}

static void store32(uint32_t a, uint32_t v, const struct px_battle_low_4_api *api)
{
    mem(api)->store32(mem(api)->user, a, v);
}

static void track(uint32_t id, const struct px_battle_low_4_api *api)
{
    if (load32(api->base->state_object + 0x6c, api) != id) store32(api->base->state_object + 0x6c, id, api);
}

static uint32_t call(const struct px_battle_low_4_api *api, uint32_t target, uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
    return api->base->call(api->base->user, target, a, b, c, d);
}

static uint32_t vcall(const struct px_battle_low_4_api *api, uint32_t object, uint32_t offset, uint32_t a)
{
    return api->base->vcall(api->base->user, object, offset, a);
}

void px_battle_release_objects_10836230(uint32_t page, const struct px_battle_low_4_api *api)
{
    /* The helper objects at +0x70, +0x6c, ... +0x50 (word offsets, highest first) are released and cleared. */
    for (uint32_t offset = 0x70;; offset -= 4) {
        const uint32_t object = load32(page + offset, api);
        if (object != 0) {
            vcall(api, object, 4, 0);
            store32(page + offset, 0, api);
        }
        if (offset == 0x50) break;
    }
}

void px_battle_state_1083b268(uint32_t page, uint32_t a, const struct px_battle_low_4_api *api)
{
    track(0xbf7, api);
    store32(page + 0x6b4, 0, api);
    store32(page + 0x6b8, 0, api);
    call(api, 0x1086fd18, page + 0x6ca, 0xff, 0, 0);
    store8(page + 0x6c8, 0xff, api);
    store8(page + 0x6c9, 0xff, api);
    store8(page + 0x25d, 0, api);
    store8(page + 0x6c8, vcall(api, page, 0x10c, a), api);
    store8(page + 0x6ca, vcall(api, page, 0x10c, a), api);
    if (vcall(api, page, 0xbc, 0) != 0) store32(page + 0x6b8, 0xff, api);
}

uint32_t px_battle_state_1083ef8c(uint32_t page, const struct px_battle_low_4_api *api)
{
    track(0x197c, api);
    const uint32_t step = load32(page + 0x6b8, api);
    if (step == 0) {
        track(0xfc4, api);
        if (call(api, 0x10858cb8, page, 0, 0, 0) == 0) return 0;
        call(api, 0x1085db0c, page, 0, 0, 0);
        call(api, 0x10859cc4, load32(page + 0x190, api) + 0x280, 0, 0, 0);
        return 1;
    }
    if (step == 0x64) {
        const uint32_t index = load32(page + 0x1b0, api);
        if (index != 0xffffffffu &&
            call(api, 0x1080103c, load32(page + 0xcc, api), page + 4 * index + 0x1b8, 0, 0) == 0) return 0;
        call(api, 0x1083ac28, page, 0, 0, 0);
        return 1;
    }
    if (step == 0xc8) {
        call(api, 0x108574e8, page, 0, 0, 0);
        return 1;
    }
    return 0;
}

uint32_t px_battle_state_10839a0c(uint32_t page, uint32_t a, const struct px_battle_low_4_api *api)
{
    track(0x154a, api);
    const uint32_t sub = load32(page + 0x190, api);
    if (load8(page + 0x494, api) != 0) {
        if (load8(sub + 0x84d, api) != 0 && load8(sub + 0x84c, api) != 0) return 0;
        store8(page + 0x494, 0, api);
        const uint32_t waited = mem(api)->load16(mem(api)->user, sub + 0x880);
        store8(sub + 0x84c, 0, api);
        mem(api)->store16(mem(api)->user, sub + 0x880, 0);
        store8(sub + 0x84d, 0, api);
        if (waited > 0x5a) {
            store32(page + 0x6b8, 0, api);
            return 0;
        }
        return 1;
    }
    const uint32_t step = load32(page + 0x6b8, api);
    if (step == 0) {
        call(api, 0x1086fd18, page + 0x6ca, 0xff, 0, 0);
        store8(page + 0x25d, 0, api);
        store8(page + 0x6c8, a, api);
        store8(page + 0x6c9, a, api);
        store8(page + 0x6ca, a, api);
        const uint32_t object = load32(load32(page + 0x190, api) + 4 * a + 0x798, api);
        store8(object + 0x35, 1, api);
        if (load8(object + 0x10, api) != 0) {
            store8(object + 0x10, 0, api);
            store8(object + 0x44, 1, api);
        }
        call(api, 0x10859464, page, 0x280, 0, 0);
        store32(page + 0x6b8, load32(page + 0x6b8, api) + 1, api);
        return 0;
    }
    if (step == 1) {
        if (call(api, 0x10858cb8, page, 0, 0, 0) == 0) return 0;
        call(api, 0x10859464, page, 0x275, 0, 0);   /* literal 10839ba0 */
        call(api, 0x10859464, page, 0x278, 0, 0);
        store32(page + 0x6b8, load32(page + 0x6b8, api) + 1, api);
        return 0;
    }
    if (step == 2) {
        if (call(api, 0x10858cb8, page, 0, 0, 0) == 0) return 0;
        call(api, 0x108582a0, load32(page + 0x190, api), a, 0, 0);
        return 1;
    }
    return step == 0x64 ? 1 : 0;
}

void px_battle_pair_for_code_1083c0b8(uint32_t object, uint32_t code, const struct px_battle_low_4_api *api)
{
    /* Copies a pair of words of the table object (108d688c) to +0x730 of the object with a flag byte at +0x72c. */
    uint32_t source;
    uint32_t flag = 0;
    if (code >= 9) return;
    if (code == 8) {
        const uint32_t kind = load8(object + 0x82c, api);
        if (kind != 0 && kind != 1) return;
        code = 1;
    }
    if (code == 0) {
        source = api->table + 0x9c;
        flag = 1;
    } else {
        static const uint32_t offsets[8] = {0, 0xa4, 0xac, 0xb4, 0xbc, 0xc4, 0xcc, 0xd4};
        source = api->table + offsets[code];
    }
    const uint32_t first = load32(source, api);
    const uint32_t second = load32(source + 4, api);
    store32(object + 0x730, first, api);
    store32(object + 0x734, second, api);
    store8(object + 0x72c, flag, api);
}

void px_battle_write_message_1081cd58(uint32_t context, uint32_t battler, uint32_t kind, uint32_t code,
                                      const struct px_battle_low_4_api *api)
{
    static const uint32_t messages[7] = {0, 0x11a, 0x13b, 0x129, 0x10b, 0xf9, 0};
    const uint32_t id = load8(battler + 0x19, api);
    const uint32_t writer = load32(context + 0xc, api);
    const uint32_t literal = 0xffff0000u;   /* literal 1081cef0 */
    uint32_t message = 0;
    uint32_t first = 0x6a;
    uint32_t c = id;
    uint32_t stack = literal;   /* [sp] is set explicitly, except on the default path where it holds the entry value of r3 (the code) */
    if (kind == 1) {
        if (code == 0x14) return;
        if (code >= 1 && code <= 5) message = messages[code];
        else if (code == 6) message = 0x162;   /* literal 1081cf04 */
        else if (code == 0xf) message = 0x18;
        else {
            first = 0x69;
            message = 0x4a;
            c = literal;
            stack = code;
        }
    } else if (kind == 2) {
        message = 0xd2;
    } else {
        first = 0x69;
        message = 0x4a;
        c = literal;
        stack = code;
    }
    api->base->call_stack(api->base->user, 0x1086b5e0, writer, first, message, c, stack, 0);
    store8(context + 0x1093, load8(context + 0x1093, api) | 0x10, api);
}

void px_battle_event_0xa5_walk_1082399c(uint32_t context, uint32_t unused, uint32_t battler, uint32_t list,
                                        const struct px_battle_low_4_api *api)
{
    (void)unused;
    const struct px_battle_event_api *events = api->events;
    store8(list + 0x70, 0, api);
    for (;;) {
        const uint32_t count = load8(list + 0x6e, api);
        const uint32_t index = load8(list + 0x70, api);
        if (index >= count) return;
        store8(list + 0x70, index + 1, api);
        const uint32_t other = load32(list + 4 * index, api);
        if (other == 0) return;
        if (call(api, 0x10865ae0, context, battler, other, 0) != 0) continue;
        uint32_t first = 0;
        const uint32_t id = call(api, 0x10874348, other, 0, 0, 0);
        if (id == 0x12) continue;
        px_battle_event_push(events);
        px_battle_event_set_id(0x03, load8(battler + 0x19, api), events);
        px_battle_event_set_id(0x04, load8(other + 0x19, api), events);
        px_battle_event_set_id(0x22, id, events);
        px_battle_event_set_value(0x43, 1, events);
        px_battle_event_set_value(0x50, 1, events);
        px_battle_event_dispatch(context, 0xa5, events);
        if (px_battle_event_get(0x43, events) != 0) first = 1;
        const uint32_t second = px_battle_event_get(0x50, events) != 0;
        px_battle_event_pop(events);
        if (first == 0) continue;
        call(api, 0x10869784, list, other, 0, 0);
        if (second == 0) continue;
        api->base->call_stack(api->base->user, 0x1086b5e0, load32(context + 0xc, api), 0x6a, 0xd5,
                              load8(other + 0x19, api), 0xffff0000u, 0);
    }
}
