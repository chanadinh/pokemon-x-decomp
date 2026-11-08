#include "battle_abilities_2.h"

static void *user(const struct px_battle_abilities_2_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_abilities_2_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_abilities_2_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t weather(uint32_t context, const struct px_battle_abilities_2_api *api)
{
    return api->weather(user(api), context);
}

/* 1084d2f4 (battle_abilities_1) needs only the battler lookup's base. */
static uint32_t share(uint32_t pokemon, uint32_t divisor, const struct px_battle_abilities_2_api *api)
{
    const struct px_battle_abilities_1_api shared = {.base = api->base};
    return px_battle_hp_share(pokemon, divisor, &shared);
}

static void cancel(const struct px_battle_abilities_2_api *api)
{
    px_battle_event_rewrite(0x42, 1, api->base->events);
}

static void multiply(uint32_t factor, const struct px_battle_abilities_2_api *api)
{
    px_battle_event_multiply(0x36, factor, api->base->events);
}

/* Kind 4 heals battler +6 by +4 HP, kind 6 damages it. */
static void hp_record(uint32_t context, uint32_t id, uint32_t kind, uint32_t pokemon, uint32_t divisor, uint32_t flagged,
                      const struct px_battle_abilities_2_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t record = api->action_record(user(api), context, kind, id);
    m->store8(m->user, record + 6, (uint8_t)id);
    if (flagged) m->store32(m->user, record, m->load32(m->user, record) | UINT32_C(0x800000));
    m->store16(m->user, record + 4, (uint16_t)share(pokemon, divisor, api));
    api->commit_actions(user(api), context, record);
}

/* Heal 1/16 in the event's weather, then cancel the weather's own effect. */
static void weather_heal(uint32_t context, uint32_t id, uint32_t wanted, const struct px_battle_abilities_2_api *api)
{
    if (get(0x3a, api) != wanted || get(0x02, api) != id) return;
    hp_record(context, id, 4, px_battle_pokemon_by_id(context, id, api->base), 0x10, 1, api);
    cancel(api);
}

PX_BATTLE_ABILITIES_2_HANDLER(px_battle_ice_body) { (void)handler, (void)work; weather_heal(context, id, 3, api); }
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_rain_dish) { (void)handler, (void)work; weather_heal(context, id, 2, api); }

PX_BATTLE_ABILITIES_2_HANDLER(px_battle_dry_skin_weather)
{
    (void)handler, (void)work;
    if (get(0x02, api) != id) return;
    const uint32_t pokemon = px_battle_pokemon_by_id(context, id, api->base);
    const uint32_t sky = get(0x3a, api) & UINT32_C(0xff);
    if (sky == 1) {
        /* Sun: 1/8 damage inside the popup, unflagged. */
        api->ability_popup(user(api), context, 1, id);
        hp_record(context, id, 6, pokemon, 8, 0, api);
        api->ability_popup(user(api), context, 2, id);
    } else if (sky == 2) {
        hp_record(context, id, 4, pokemon, 8, 1, api);
    }
}

PX_BATTLE_ABILITIES_2_HANDLER(px_battle_solar_power_weather)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id || get(0x3a, api) != 1) return;
    /* The share is taken before the popup. */
    const uint32_t hp = share(px_battle_pokemon_by_id(context, id, api->base), 8, api);
    api->ability_popup(user(api), context, 1, id);
    const uint32_t record = api->action_record(user(api), context, 6, id);
    m->store8(m->user, record + 6, (uint8_t)id);
    m->store16(m->user, record + 4, (uint16_t)hp);
    api->commit_actions(user(api), context, record);
    api->ability_popup(user(api), context, 2, id);
}

/* Weather damage (key 0x33, signed) cancelled, optionally only in one weather. */
static void weather_immune(uint32_t id, uint32_t wanted, const struct px_battle_abilities_2_api *api)
{
    if (get(0x02, api) != id || (int32_t)get(0x33, api) <= 0) return;
    if (wanted != 0 && get(0x3a, api) != wanted) return;
    cancel(api);
}

PX_BATTLE_ABILITIES_2_HANDLER(px_battle_overcoat) { (void)handler, (void)context, (void)work; weather_immune(id, 0, api); }
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_sand_immunity) { (void)handler, (void)context, (void)work; weather_immune(id, 4, api); }
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_snow_cloak_weather) { (void)handler, (void)context, (void)work; weather_immune(id, 3, api); }

/* Multiplier 0x36 for battler key `key` in weather `wanted`. */
static void weather_multiply(uint32_t context, uint32_t id, uint32_t key, uint32_t wanted, uint32_t factor,
                             const struct px_battle_abilities_2_api *api)
{
    if (get(key, api) != id || weather(context, api) != wanted) return;
    multiply(factor, api);
}

PX_BATTLE_ABILITIES_2_HANDLER(px_battle_swift_swim) { (void)handler, (void)work; weather_multiply(context, id, 0x02, 2, 0x2000, api); }
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_chlorophyll) { (void)handler, (void)work; weather_multiply(context, id, 0x02, 1, 0x2000, api); }
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_sand_rush) { (void)handler, (void)work; weather_multiply(context, id, 0x02, 4, 0x2000, api); }
/* Evasion: the defender's accuracy multiplier x0.8 (literals 108c0218, 108c0334). */
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_sand_veil) { (void)handler, (void)work; weather_multiply(context, id, 0x04, 4, 0xccd, api); }
PX_BATTLE_ABILITIES_2_HANDLER(px_battle_snow_cloak) { (void)handler, (void)work; weather_multiply(context, id, 0x04, 3, 0xccd, api); }

PX_BATTLE_ABILITIES_2_HANDLER(px_battle_solar_power)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id || weather(context, api) != 1) return;
    /* Special moves (category 2) x1.5 in sun. */
    if (api->base->move_category(user(api), get(0x12, api) & UINT32_C(0xffff)) != 2) return;
    multiply(0x1800, api);
}

PX_BATTLE_ABILITIES_2_HANDLER(px_battle_hydration)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id || weather(context, api) != 2) return;
    if (api->base->pokemon_status(user(api), px_battle_pokemon_by_id(context, id, api->base)) == 0) return;
    api->ability_popup(user(api), context, 1, id);
    /* Kind 0xa: cure (+4 0x26) of battler +5; flag 0x2000000. */
    const uint32_t record = api->action_record(user(api), context, 0xa, id);
    const uint32_t flags = m->load32(m->user, record);
    m->store8(m->user, record + 4, 0x26);
    m->store8(m->user, record + 5, (uint8_t)id);
    m->store8(m->user, record + 0x11, 1);
    m->store32(m->user, record, flags | UINT32_C(0x2000000));
    api->commit_actions(user(api), context, record);
    api->ability_popup(user(api), context, 2, id);
}

PX_BATTLE_ABILITIES_2_HANDLER(px_battle_leaf_guard)
{
    (void)handler, (void)work;
    if (get(0x02, api) != id || weather(context, api) != 1) return;
    cancel(api);
}

PX_BATTLE_ABILITIES_2_HANDLER(px_battle_air_lock)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    /* Kind 0x1e with weather 0 and +6 set: the weather's effects stop (message 0x61). */
    const uint32_t record = api->action_record(user(api), context, 0x1e, id);
    const uint32_t flags = m->load32(m->user, record);
    m->store8(m->user, record + 4, 0);
    m->store8(m->user, record + 6, 1);
    m->store32(m->user, record, flags | UINT32_C(0x800000));
    api->message(user(api), record + 8, 1, 0x61);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_2_HANDLER(px_battle_air_lock_suppress)
{
    (void)handler, (void)context, (void)id, (void)work;
    cancel(api);
}
