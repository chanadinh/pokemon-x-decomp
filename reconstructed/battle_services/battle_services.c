#include "battle_services.h"

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

static void command(uint32_t context, const uint32_t *args, unsigned count, const struct px_battle_process_api *api)
{
    uint32_t all[8] = {r32(context + 0xc, api)};
    for (unsigned i = 0; i < count; ++i) all[1 + i] = args[i];
    call(api, PX_BATTLE_COMMAND, all, count + 1);
}

static uint32_t position(uint32_t context, uint32_t id, const struct px_battle_process_api *api)
{
    return call(api, PX_BATTLE_POSITION, ARGS(context + 0x120c, id));
}

/* The stage byte of stats 1-7 (inner jump tables 10872304 and 10871508). */
static int stage_at(uint32_t pokemon, uint32_t stat, uint32_t *at)
{
    if (stat < 1 || stat > 7) return 0;
    *at = pokemon + 0x103 + stat;
    return 1;
}

uint32_t px_battle_stage_can_change(uint32_t pokemon, uint32_t stat, int32_t delta, const struct px_battle_process_api *api)
{
    uint32_t at;
    int32_t stage = 0;
    if (stage_at(pokemon, stat, &at)) stage = (int8_t)r8(at, api);
    else fault(api);
    return delta > 0 ? stage < 12 : stage > 0;
}

void px_battle_set_stage(uint32_t pokemon, uint32_t stat, uint32_t value, const struct px_battle_process_api *api)
{
    uint32_t at;
    if (stage_at(pokemon, stat, &at) && value <= 12) w8(at, value, api);
    else fault(api);
}

uint32_t px_battle_value_key(uint32_t pokemon, uint32_t key, const struct px_battle_process_api *api)
{
    const uint32_t field = r32(pokemon + 0x210, api);
    if (field == 0) return key;
    if (key == 9) return r32(field + 0x28c, api) != 0 ? 0xb : key;
    if (key == 0xb) return r32(field + 0x28c, api) != 0 ? 9 : key;
    return key;
}

void px_battle_set_value(uint32_t pokemon, uint32_t key, uint32_t value, const struct px_battle_process_api *api)
{
    /* Keys 8-0xc (inner jump table 10871270). */
    const uint32_t index = px_battle_value_key(pokemon, key, api) - 8;
    if (index < 5) w16(pokemon + 0xf6 + 2 * index, value, api);
    else fault(api);
}

void px_battle_lose_hp(uint32_t context, uint32_t pokemon, uint32_t amount, uint32_t flag, const struct px_battle_process_api *api)
{
    /* Nothing is lost while 108007e4 reports bit 4 of the battle setup (+4, +0x10). */
    if (call(api, 0x108007e4, ARGS(r32(r32(context + 4, api) + 0x10, api), 4)) != 0) return;
    const uint32_t hp = r16(pokemon + 0x10, api), loss = amount & 0xffff;
    if (hp > loss) {
        w16(pokemon + 0x10, hp - loss, api);
    } else {
        w16(pokemon + 0x10, 0, api);
        call(api, 0x108003fc, ARGS(r32(pokemon, api)));
    }
    command(context, ARGS(flag != 0 ? 2 : 1, r8(pokemon + 0x19, api), loss), api);
}

void px_battle_change_hp(uint32_t context, uint32_t pokemon, int32_t amount, uint32_t report, const struct px_battle_process_api *api)
{
    const uint32_t id = r8(pokemon + 0x19, api);
    if (amount > 0) {
        const uint32_t hp = (r16(pokemon + 0x10, api) + ((uint32_t)amount & 0xffff)) & 0xffff;
        w16(pokemon + 0x10, hp, api);
        if (hp > r16(pokemon + 0xe, api)) w16(pokemon + 0x10, r16(pokemon + 0xe, api), api);
        command(context, ARGS(3, id, (uint32_t)amount & 0xffff), api);
    } else if (amount < 0) {
        px_battle_lose_hp(context, pokemon, (uint32_t)-amount, 0, api);
    }
    if (report != 0 && position(context, id, api) != 0xa) command(context, ARGS(0x48, id), api);
}

void px_battle_cure_condition(uint32_t context, uint32_t pokemon, uint32_t condition, uint32_t out, const struct px_battle_process_api *api)
{
    if (condition == 0) return;
    const uint32_t id = r8(pokemon + 0x19, api);
    if (out != 0) {
        if (condition >= 0x26) fault(api);
        w32(out, r32(pokemon + 0x1c + 4 * condition, api), api);
    }
    if (condition < 6) {
        /* A major status clears all five, with condition 9's word and turns. */
        for (uint32_t i = 1; i < 6; ++i) {
            w32(pokemon + 0x1c + 4 * i, 0, api);
            w8(pokemon + 0xb4 + i, 0, api);
            if (i == 2) {
                w32(pokemon + 0x40, 0, api);
                w8(pokemon + 0xbd, 0, api);
            }
        }
        command(context, ARGS(0x11, id), api);
        if (position(context, id, api) != 0xa) command(context, ARGS(0x3c, id, 0), api);
    } else {
        call(api, 0x1086fd8c, ARGS(pokemon, condition));
        command(context, ARGS(0x12, id, condition), api);
    }
}

uint32_t px_battle_status_blocked(uint32_t context, uint32_t target, uint32_t source, uint32_t status, uint32_t word, uint32_t byte,
                                  int8_t report, const struct px_battle_process_api *api)
{
    const struct px_battle_event_api *events = api->events;
    const uint32_t reason = call(api, 0x1082008c, ARGS(context, target, status, word, byte));
    if (reason != 0) {
        if ((uint8_t)report != 0) call(api, 0x1081cd58, ARGS(context, target, reason, status));
        return 1;
    }
    /* Event 0x6e (keys 3 source, 4 target, 0x1e status, 0x42, 0x44). */
    const uint32_t stack = context + 0x437c, saved = call(api, 0x1086b284, ARGS(stack));
    px_battle_event_push(events);
    px_battle_event_set_id(0x03, source != 0 ? r8(source + 0x19, api) : 0x1f, events);
    px_battle_event_set_id(0x04, r8(target + 0x19, api), events);
    px_battle_event_set_id(0x1e, status, events);
    px_battle_event_set_value(0x42, 0, events);
    px_battle_event_set_value(0x44, (uint32_t)(int32_t)report, events);
    px_battle_event_dispatch(context, 0x6e, events);
    const uint32_t blocked = px_battle_event_get(0x42, events) != 0;
    const uint32_t shown = px_battle_event_get(0x44, events) != 0;
    px_battle_event_pop(events);
    if (blocked && shown) {
        /* Event 0x70 announces the block (the source id read as given). */
        px_battle_event_push(events);
        px_battle_event_set_scaled(0x04, r8(target + 0x19, api), events);
        px_battle_event_set_scaled(0x03, r8(source + 0x19, api), events);
        px_battle_event_set_scaled(0x1e, status, events);
        px_battle_event_dispatch(context, 0x70, events);
        px_battle_event_pop(events);
        w8(context + 0x1093, r8(context + 0x1093, api) | 0x10, api);
    }
    w32(stack, saved, api);
    return blocked;
}

uint32_t px_battle_apply_stage(uint32_t context, uint32_t source, uint32_t pokemon, uint32_t stat, const struct px_battle_stage_change *change,
                               const struct px_battle_process_api *api)
{
    const struct px_battle_event_api *events = api->events;
    if (call(api, 0x10865c4c, ARGS(context)) != 0) return 0;
    /* Event 0x63 may adjust the stages (key 0x21). */
    px_battle_event_push(events);
    px_battle_event_set_id(0x02, r8(pokemon + 0x19, api), events);
    px_battle_event_set_id(0x03, change->source_key, events);
    px_battle_event_set_id(0x20, stat, events);
    px_battle_event_set_value(0x21, (uint32_t)change->delta, events);
    px_battle_event_dispatch(context, 0x63, events);
    const int32_t delta = (int32_t)px_battle_event_get(0x21, events);
    px_battle_event_pop(events);
    const uint32_t end = UINT32_C(0xffff0000); /* literal 10861174 */
    if (!px_battle_stage_can_change(pokemon, stat, delta, api)) {
        if (change->report == 0) return 0;
        call(api, PX_BATTLE_MESSAGE_COMMAND,
             ARGS(r32(context + 0xc, api), 0x6a, delta > 0 ? 0x99 : 0xae, r8(pokemon + 0x19, api), stat, end));
        w8(context + 0x1093, r8(context + 0x1093, api) | 0x10, api);
        return 0;
    }
    /* A protected battler (+0x208) refuses others' changes unless forced. */
    if (r16(pokemon + 0x208, api) != 0 && change->forced == 0 && source != r8(pokemon + 0x19, api)) {
        if (change->report != 0)
            call(api, PX_BATTLE_MESSAGE_COMMAND, ARGS(r32(context + 0xc, api), 0x6a, 0xd2, r8(pokemon + 0x19, api), end));
        return 0;
    }
    /* Event 0x64 (keys 2, 3, 0x20, 0x21, 0x1a) may block it (key 0x42). */
    px_battle_event_push(events);
    px_battle_event_set_id(0x02, r8(pokemon + 0x19, api), events);
    px_battle_event_set_id(0x03, source, events);
    px_battle_event_set_id(0x20, stat, events);
    px_battle_event_set_id(0x21, (uint32_t)delta, events);
    px_battle_event_set_id(0x1a, change->word, events);
    px_battle_event_set_value(0x42, 0, events);
    px_battle_event_dispatch(context, 0x64, events);
    const int blocked = px_battle_event_get(0x42, events) != 0;
    px_battle_event_pop(events);
    const uint32_t stack = context + 0x437c;
    if (blocked) {
        if (change->report != 0) {
            w8(context + 0x1093, r8(context + 0x1093, api) | 0x10, api);
            const uint32_t saved = call(api, 0x1086b284, ARGS(stack));
            px_battle_event_push(events);
            px_battle_event_set_id(0x02, r8(pokemon + 0x19, api), events);
            px_battle_event_set_id(0x1a, change->word, events);
            px_battle_event_dispatch(context, 0x65, events);
            px_battle_event_pop(events);
            w32(stack, saved, api);
        }
        return 0;
    }
    if (change->message != 0) call(api, PX_BATTLE_SHOW_MESSAGE, ARGS(context, change->message));
    call(api, 0x1081c0a0, ARGS(context, pokemon, stat, (uint32_t)delta, change->low, change->quiet));
    /* Event 0x66 after the change. */
    const uint32_t saved = call(api, 0x1086b284, ARGS(stack));
    px_battle_event_push(events);
    px_battle_event_set_id(0x03, source, events);
    px_battle_event_set_id(0x02, r8(pokemon + 0x19, api), events);
    px_battle_event_set_id(0x20, stat, events);
    px_battle_event_set_id(0x21, (uint32_t)delta, events);
    px_battle_event_dispatch(context, 0x66, events);
    px_battle_event_pop(events);
    w32(stack, saved, api);
    return 1;
}
