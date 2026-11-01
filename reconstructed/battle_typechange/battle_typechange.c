#include "battle_typechange.h"

static void *user(const struct px_battle_typechange_api *api)
{
    return api->base->events->user;
}

static uint32_t get(uint32_t key, const struct px_battle_typechange_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t rewrite(uint32_t key, uint32_t value, const struct px_battle_typechange_api *api)
{
    return px_battle_event_rewrite(key, value, api->base->events);
}

static uint32_t held_item(uint32_t context, uint32_t id, const struct px_battle_typechange_api *api)
{
    const struct px_pokemon_api *m = api->base->events->memory;
    return m->load16(m->user, px_battle_pokemon_by_id(context, id, api->base) + UINT32_C(0x12));
}

uint32_t px_battle_curse_form(uint32_t pokemon, const struct px_battle_typechange_api *api)
{
    return api->has_type(user(api), pokemon, 7) != 0 ? 0 : 7;
}

uint32_t px_battle_plate_type(uint32_t item)
{
    /* Flame..Earth Plate (298-305) through the jump table at 108b7778. */
    static const uint8_t low[8] = {9, 10, 12, 11, 14, 1, 3, 4};
    /* Sky..Iron Plate (306-313) by comparison; Pixie Plate is 644. */
    static const uint8_t high[8] = {2, 13, 6, 5, 7, 15, 16, 8};
    if (item - UINT32_C(298) < 8) return low[item - 298];
    if (item - UINT32_C(306) < 8) return high[item - 306];
    return item == 644 ? 17 : 0;
}

PX_BATTLE_TYPECHANGE_HANDLER(px_battle_normalize)
{
    (void)handler, (void)context, (void)work;
    if (get(0x02, api) == id) rewrite(0x16, 0, api);
}

/* Aerilate, Pixilate and Refrigerate: a Normal move takes the type, and the
 * work word records whether the rewrite was accepted (read by event 0x3d). */
static void convert_normal(uint32_t id, uint32_t work, uint32_t type, const struct px_battle_typechange_api *api)
{
    const struct px_pokemon_api *m = api->base->events->memory;
    m->store32(m->user, work, 0);
    if (get(0x02, api) != id || get(0x16, api) != 0) return;
    m->store32(m->user, work, rewrite(0x16, type, api));
}

PX_BATTLE_TYPECHANGE_HANDLER(px_battle_aerilate_type) { (void)handler, (void)context; convert_normal(id, work, 2, api); }
PX_BATTLE_TYPECHANGE_HANDLER(px_battle_pixilate_type) { (void)handler, (void)context; convert_normal(id, work, 17, api); }
PX_BATTLE_TYPECHANGE_HANDLER(px_battle_refrigerate_type) { (void)handler, (void)context; convert_normal(id, work, 14, api); }

PX_BATTLE_TYPECHANGE_HANDLER(px_battle_weather_ball)
{
    (void)handler, (void)work;
    if (get(0x02, api) != id) return;
    const uint32_t weather = api->weather(user(api), context);
    uint32_t type = get(0x16, api) & UINT32_C(0xff), code = 0;
    switch (weather) {
    case 1: type = 9, code = 1; break;   /* sun: Fire */
    case 2: type = 10, code = 4; break;  /* rain: Water */
    case 3: type = 14, code = 2; break;  /* hail: Ice */
    case 4: type = 5, code = 3; break;   /* sandstorm: Rock */
    default: break;
    }
    rewrite(0x16, type, api);
    api->type_changed(user(api), context, code);
}

PX_BATTLE_TYPECHANGE_HANDLER(px_battle_curse)
{
    (void)handler, (void)work;
    if (get(0x02, api) == id) rewrite(0x1c, px_battle_curse_form(px_battle_pokemon_by_id(context, id, api->base), api), api);
}

PX_BATTLE_TYPECHANGE_HANDLER(px_battle_struggle)
{
    (void)handler, (void)context, (void)work;
    if (get(0x02, api) == id) rewrite(0x4c, 1, api);
}

PX_BATTLE_TYPECHANGE_HANDLER(px_battle_hidden_power)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = api->base->events->memory;
    if (get(0x02, api) != id) return;
    const uint32_t pokemon = px_battle_pokemon_by_id(context, id, api->base);
    uint32_t type = 0;
    if (pokemon != 0)
        type = api->hidden_power_type(user(api), m->load32(m->user, pokemon));
    else
        api->base->events->abort(user(api));
    rewrite(0x16, type, api);
}

PX_BATTLE_TYPECHANGE_HANDLER(px_battle_judgment)
{
    (void)handler, (void)work;
    if (get(0x02, api) != id || api->item_usable(user(api), context, id) == 0) return;
    rewrite(0x16, px_battle_plate_type(held_item(context, id, api)), api);
}

PX_BATTLE_TYPECHANGE_HANDLER(px_battle_natural_gift)
{
    (void)handler, (void)work;
    if (get(0x02, api) != id) return;
    rewrite(0x16, api->item_param(user(api), held_item(context, id, api), 8), api);
}

PX_BATTLE_TYPECHANGE_HANDLER(px_battle_techno_blast)
{
    (void)handler, (void)work;
    if (get(0x02, api) != id || api->item_usable(user(api), context, id) == 0) return;
    /* Douse, Shock, Burn and Chill Drive (items 116-119). */
    uint32_t type = 0, code = 0;
    switch (held_item(context, id, api)) {
    case 116: type = 10, code = 1; break;
    case 117: type = 12, code = 2; break;
    case 118: type = 9, code = 3; break;
    case 119: type = 14, code = 4; break;
    default: break;
    }
    rewrite(0x16, type, api);
    api->type_changed(user(api), context, code);
}
