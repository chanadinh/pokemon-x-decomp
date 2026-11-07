#include "battle_conditions.h"

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

static void w16(uint32_t address, uint32_t value, const struct px_battle_process_api *api)
{
    api->events->memory->store16(api->events->memory->user, address, (uint16_t)value);
}

static void w32(uint32_t address, uint32_t value, const struct px_battle_process_api *api)
{
    api->events->memory->store32(api->events->memory->user, address, value);
}

static void command(uint32_t context, const uint32_t *args, unsigned count, const struct px_battle_process_api *api)
{
    uint32_t all[8] = {r32(context + 0xc, api)};
    for (unsigned i = 0; i < count; ++i) all[1 + i] = args[i];
    call(api, PX_BATTLE_COMMAND, all, count + 1);
}

uint32_t px_battle_status_animation(uint32_t status)
{
    static const uint16_t animations[8] = {0, 0x299, 0x295, 0x298, 0x297, 0x296, 0x29a, 0x29b};
    return status < 8 ? animations[status] : 0;
}

void px_battle_apply_status(uint32_t context, uint32_t target, uint32_t source, uint32_t status, uint32_t word, uint32_t build,
                            uint32_t keep_item, uint32_t message, const struct px_battle_process_api *api)
{
    const struct px_battle_event_api *events = api->events;
    call(api, 0x10865118, ARGS(context, target, status, word));
    const uint32_t animation = px_battle_status_animation(status);
    if (animation != 0) {
        const uint32_t slot = call(api, PX_BATTLE_POSITION, ARGS(context + 0x120c, r8(target + 0x19, api)));
        if (slot != 0xa) command(context, ARGS(0x57, slot, animation), api);
    }
    if (build != 0) {
        /* A default message built into the context's buffer, cleared after. */
        const uint32_t text = context + 0x433c;
        call(api, 0x1087afa0, ARGS(status, word, target, text));
        call(api, PX_BATTLE_SHOW_MESSAGE, ARGS(context, text));
        for (uint32_t w = 0; w < 3; ++w) w32(text + 4 * w, 0, api);
        const uint32_t flags = r16(text + 2, api);
        for (uint32_t w = 3; w < 9; ++w) w32(text + 4 * w, 0, api);
        w16(text + 2, flags & ~0x7fu, api);
    } else if (message != 0) {
        call(api, PX_BATTLE_SHOW_MESSAGE, ARGS(context, message));
    }
    if (status == 3 && r16(target + 0xc, api) == PX_BATTLE_SHAYMIN && r8(target + 0x153, api) == 1) {
        call(api, 0x108679ac, ARGS(target));
        call(api, 0x1086fff0, ARGS(target, 0));
        command(context, ARGS(0x5b, r8(target + 0x19, api), 0), api);
        call(api, 0x108678f8, ARGS(target));
        call(api, PX_BATTLE_MESSAGE_COMMAND,
             ARGS(r32(context + 0xc, api), 0x6a, 0xde, r8(target + 0x19, api), UINT32_C(0xffff0000) /* literal 10862e00 */));
    }
    const uint32_t stack = context + 0x437c, saved = call(api, 0x1086b284, ARGS(stack));
    const uint32_t from = source != 0 ? r8(source + 0x19, api) : 0x1f;
    if (status < 6) {
        /* Event 0x71 (keys 4, 3, 0x1e, 0x1f). */
        px_battle_event_push(events);
        px_battle_event_set_id(0x04, r8(target + 0x19, api), events);
        px_battle_event_set_id(0x03, source != 0 ? r8(source + 0x19, api) : 0x1f, events);
        px_battle_event_set_id(0x1e, status & 0xff, events);
        px_battle_event_set_id(0x1f, word, events);
        px_battle_event_dispatch(context, 0x71, events);
        px_battle_event_pop(events);
    } else if (status != 0x10) {
        /* Event 0x72 (keys 4, 3, 0x1e). */
        px_battle_event_push(events);
        px_battle_event_set_id(0x04, r8(target + 0x19, api), events);
        px_battle_event_set_id(0x03, from, events);
        px_battle_event_set_id(0x1e, status, events);
        px_battle_event_dispatch(context, 0x72, events);
        px_battle_event_pop(events);
    } else {
        /* Status 0x10: event 0x73 (key 2) inline without the blocked check
         * (battle state through literal 10862e04), then 10864dc0 for ability
         * value 0x7f. */
        px_battle_event_push(events);
        px_battle_event_set_id(0x02, r8(target + 0x19, api), events);
        const uint32_t depth = events->battle + 0x68;
        w32(depth, r32(depth, api) + 1, api);
        px_battle_event_run_handlers(context, 0x73, PX_BATTLE_ANY_OWNER, 0, events);
        const uint32_t remaining = r32(depth, api) - 1;
        w32(depth, remaining, api);
        if (remaining == 0)
            for (uint32_t h = r32(events->battle + 0x60, api); h != 0; h = r32(h + 4, api)) w16(h + 0x18, 0, api);
        px_battle_event_pop(events);
        if (call(api, PX_BATTLE_POKEMON_VALUE, ARGS(target, 0x10)) == 0x7f) call(api, 0x10864dc0, ARGS(context, target));
    }
    w32(stack, saved, api);
    /* The held item reacts (event 0x9c, key 0x2f = 4) unless kept. */
    if (keep_item == 0 && r16(target + 0x12, api) != 0) {
        const uint32_t again = call(api, 0x1086b284, ARGS(stack));
        px_battle_event_push(events);
        px_battle_event_set_id(0x02, r8(target + 0x19, api), events);
        px_battle_event_set_id(0x2f, 4, events);
        px_battle_event_dispatch(context, 0x9c, events);
        px_battle_event_pop(events);
        w32(stack, again, api);
    }
}
