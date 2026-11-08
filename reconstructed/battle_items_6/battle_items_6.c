#include "battle_items_6.h"

enum { HANDLER_ITEM = 0x38 };

static const struct px_battle_ability_api *base(const struct px_battle_items_6_api *api)
{
    return api->modifier->base;
}

static void *user(const struct px_battle_items_6_api *api)
{
    return base(api)->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_items_6_api *api)
{
    return base(api)->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_items_6_api *api)
{
    return px_battle_event_get(key, base(api)->events);
}

static uint32_t param3(uint32_t handler, const struct px_battle_items_6_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return api->modifier->item_param(user(api), m->load16(m->user, handler + HANDLER_ITEM), 3);
}

static void multiply(uint32_t factor, const struct px_battle_items_6_api *api)
{
    px_battle_event_multiply(0x36, factor, base(api)->events);
}

/* vcvt.s32.f32: toward zero, saturating. */
static int32_t to_int(float value)
{
    if (value >= 2147483648.0f) return INT32_MAX;
    if (value < -2147483648.0f) return INT32_MIN;
    return (int32_t)value;
}

uint32_t px_battle_accuracy_drop(uint32_t reduction)
{
    /* Zero gives -0.5 (literal 108a7614), otherwise the scaled value plus
     * 0.5 (literal 108a7610), converted in single precision. */
    const float scaled = reduction == 0 ? -0.5f : (float)(uint32_t)(reduction << 12) + 0.5f;
    /* smull with 0x51eb851f (literal 108a7618), asr 5, minus the sign: a
     * signed division by 100. */
    return (uint32_t)(to_int(scaled) / 100);
}

PX_BATTLE_ITEMS_6_HANDLER(px_battle_bright_powder)
{
    (void)context, (void)work;
    if (get(0x04, api) != id) return;
    multiply(px_battle_accuracy_drop(UINT32_C(100) - param3(handler, api)), api);
}

PX_BATTLE_ITEMS_6_HANDLER(px_battle_lax_incense)
{
    (void)context, (void)work;
    if (get(0x04, api) != id) return;
    /* The reduction is narrowed to 16 bits first (literals 108a72b0-108a72b8). */
    multiply(px_battle_accuracy_drop((UINT32_C(100) - param3(handler, api)) & UINT32_C(0xffff)), api);
}

PX_BATTLE_ITEMS_6_HANDLER(px_battle_wide_lens)
{
    (void)context, (void)work;
    if (get(0x03, api) != id) return;
    multiply(px_battle_item_factor(handler, api->modifier), api);
}

PX_BATTLE_ITEMS_6_HANDLER(px_battle_zoom_lens)
{
    (void)work;
    if (get(0x03, api) != id) return;
    const uint32_t defender = px_battle_pokemon_by_id(context, get(0x04, api) & UINT32_C(0xff), base(api));
    if (api->turn_flag(user(api), defender, 1) == 0) return;
    multiply(px_battle_item_factor(handler, api->modifier), api);
}

/* Event 0x85: the holder's weather lasts parameter 3 turns. */
static void weather_rock(uint32_t handler, uint32_t id, uint32_t weather, const struct px_battle_items_6_api *api)
{
    if (get(0x03, api) != id) return;
    if (get(0x3a, api) != weather) return;
    px_battle_event_rewrite(0x25, param3(handler, api), base(api)->events);
}

PX_BATTLE_ITEMS_6_HANDLER(px_battle_heat_rock) { (void)context, (void)work; weather_rock(handler, id, 1, api); }
PX_BATTLE_ITEMS_6_HANDLER(px_battle_damp_rock) { (void)context, (void)work; weather_rock(handler, id, 2, api); }
PX_BATTLE_ITEMS_6_HANDLER(px_battle_icy_rock) { (void)context, (void)work; weather_rock(handler, id, 3, api); }
PX_BATTLE_ITEMS_6_HANDLER(px_battle_smooth_rock) { (void)context, (void)work; weather_rock(handler, id, 4, api); }

/* Copy key 0x1f to the scratch word (the original's sp + 0) for a helper. */
static uint32_t load_condition(const struct px_battle_items_6_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store32(m->user, api->scratch, get(0x1f, api));
    return api->scratch;
}

static void store_condition(const struct px_battle_items_6_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    px_battle_event_rewrite(0x1f, m->load32(m->user, api->scratch), base(api)->events);
}

PX_BATTLE_ITEMS_6_HANDLER(px_battle_light_clay)
{
    (void)context, (void)work;
    if (get(0x02, api) != id) return;
    /* Key 0x56's low byte must be 0 or 1 (Reflect or Light Screen). */
    const uint32_t screen = get(0x56, api) & UINT32_C(0xff);
    if (screen != 1 && screen != 0) return;
    const uint32_t word = load_condition(api);
    api->set_turns(user(api), word, param3(handler, api) & UINT32_C(0xff));
    store_condition(api);
}

PX_BATTLE_ITEMS_6_HANDLER(px_battle_grip_claw)
{
    (void)handler, (void)context, (void)work;
    if (get(0x03, api) != id) return;
    if (get(0x1e, api) != 8) return;
    api->set_trap_turns(user(api), load_condition(api), 8);
    store_condition(api);
}

PX_BATTLE_ITEMS_6_HANDLER(px_battle_binding_band)
{
    (void)handler, (void)context, (void)work;
    if (get(0x03, api) != id) return;
    api->set_trap_flag(user(api), load_condition(api), 1);
    store_condition(api);
}
