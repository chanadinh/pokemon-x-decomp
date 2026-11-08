#include "battle_abilities_10.h"

static void *user(const struct px_battle_abilities_10_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_abilities_10_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_abilities_10_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t load32(uint32_t address, const struct px_battle_abilities_10_api *api)
{
    return memory(api)->load32(memory(api)->user, address);
}

static uint32_t species_is(uint32_t pokemon, uint32_t species, const struct px_battle_abilities_10_api *api)
{
    return memory(api)->load16(memory(api)->user, pokemon + 0xc) == species;
}

static uint32_t form(uint32_t pokemon, const struct px_battle_abilities_10_api *api)
{
    return memory(api)->load8(memory(api)->user, pokemon + 0x153);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_abilities_10_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

/* Kind 0x3a: battler +4 takes form +5; message `message` at +8. */
static uint32_t form_record(uint32_t context, uint32_t id, uint32_t to, uint32_t flagged, const struct px_battle_abilities_10_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t record = api->action_record(user(api), context, 0x3a, id);
    m->store8(m->user, record + 4, (uint8_t)id);
    m->store8(m->user, record + 5, (uint8_t)to);
    if (flagged) m->store32(m->user, record, m->load32(m->user, record) | UINT32_C(0x800000));
    return record;
}

/* Castform transformed: back to its normal form (message 0xde naming it, unflagged). */
static void forecast_reset(uint32_t context, uint32_t id, const struct px_battle_abilities_10_api *api)
{
    const uint32_t self = pokemon(context, id, api);
    if (!species_is(self, 0x15f, api) || form(self, api) == 0) return;
    const uint32_t record = form_record(context, id, 0, 0, api);
    api->message(user(api), record + 8, 2, 0xde);
    api->message_arg(user(api), record + 8, id);
    api->commit_actions(user(api), context, record);
}

/* Castform takes the form of the weather (1 sun, 2 rain, 3 hail, else 0), flagged. */
static void forecast_follow(uint32_t context, uint32_t id, const struct px_battle_abilities_10_api *api)
{
    uint32_t wanted = api->weather(user(api), context);
    const uint32_t self = pokemon(context, id, api);
    if (!species_is(self, 0x15f, api)) return;
    const uint32_t now = form(self, api);
    if (wanted != 1 && wanted != 2 && wanted != 3) wanted = 0;
    if (wanted == now) return;
    const uint32_t record = form_record(context, id, wanted, 1, api);
    api->message(user(api), record + 8, 2, 0xde);
    api->message_arg(user(api), record + 8, id);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_10_HANDLER(px_battle_forecast_leave)
{
    (void)handler, (void)work;
    if (get(0x02, api) == id) forecast_reset(context, id, api);
}

PX_BATTLE_ABILITIES_10_HANDLER(px_battle_forecast_suppressed)
{
    (void)handler;
    if (load32(work, api) != 0) forecast_reset(context, id, api);
}

PX_BATTLE_ABILITIES_10_HANDLER(px_battle_forecast_weather)
{
    (void)handler;
    if (load32(work, api) != 0) forecast_follow(context, id, api);
}

/* The ability changes to another one (key 0x10 differs from the handler's +0x38). */
static uint32_t ability_lost(uint32_t handler, uint32_t id, const struct px_battle_abilities_10_api *api)
{
    if (get(0x02, api) != id) return 0;
    return get(0x10, api) != memory(api)->load16(memory(api)->user, handler + 0x38);
}

PX_BATTLE_ABILITIES_10_HANDLER(px_battle_forecast_lost)
{
    (void)work;
    if (ability_lost(handler, id, api)) forecast_reset(context, id, api);
}

PX_BATTLE_ABILITIES_10_HANDLER(px_battle_forecast_start)
{
    (void)handler;
    forecast_follow(context, id, api);
    memory(api)->store32(memory(api)->user, work, 1);
}

static uint32_t cherrim(uint32_t context, uint32_t id, const struct px_battle_abilities_10_api *api)
{
    return species_is(pokemon(context, id, api), 0x1a5, api);
}

/* In sun, Cherrim's allies' (and its own) special defense or physical attack x1.5. */
PX_BATTLE_ABILITIES_10_HANDLER(px_battle_flower_gift_defense)
{
    (void)handler, (void)work;
    if (!cherrim(context, id, api) || api->weather(user(api), context) != 1) return;
    if (api->is_ally(user(api), id, get(0x04, api) & UINT32_C(0xff)) == 0 || get(0x1b, api) != 2) return;
    px_battle_event_multiply(0x36, 0x1800, api->base->events);
}

PX_BATTLE_ABILITIES_10_HANDLER(px_battle_flower_gift_attack)
{
    (void)handler, (void)work;
    if (!cherrim(context, id, api) || api->weather(user(api), context) != 1) return;
    if (api->is_ally(user(api), id, get(0x03, api) & UINT32_C(0xff)) == 0) return;
    if (api->base->move_category(user(api), get(0x12, api) & UINT32_C(0xffff)) != 1) return;
    px_battle_event_multiply(0x36, 0x1800, api->base->events);
}

PX_BATTLE_ABILITIES_10_HANDLER(px_battle_flower_gift_leave)
{
    if (load32(work, api) == 0 || get(0x02, api) != id || !cherrim(context, id, api)) return;
    api->flower_gift(user(api), handler, context, id, 0, 0);
}

PX_BATTLE_ABILITIES_10_HANDLER(px_battle_flower_gift_suppressed)
{
    if (load32(work, api) == 0 || !cherrim(context, id, api)) return;
    api->flower_gift(user(api), handler, context, id, 0, 0);
}

PX_BATTLE_ABILITIES_10_HANDLER(px_battle_flower_gift_weather)
{
    if (load32(work, api) == 0 || !cherrim(context, id, api)) return;
    api->flower_gift(user(api), handler, context, id, api->weather(user(api), context) == 1, 1);
}

PX_BATTLE_ABILITIES_10_HANDLER(px_battle_flower_gift_lost)
{
    if (load32(work, api) == 0 || !cherrim(context, id, api)) return;
    if (ability_lost(handler, id, api)) api->flower_gift(user(api), handler, context, id, 0, 0);
}

PX_BATTLE_ABILITIES_10_HANDLER(px_battle_flower_gift_start)
{
    if (!cherrim(context, id, api)) return;
    api->flower_gift(user(api), handler, context, id, api->weather(user(api), context) == 1, 1);
    memory(api)->store32(memory(api)->user, work, 1);
}

PX_BATTLE_ABILITIES_10_HANDLER(px_battle_zen_mode)
{
    (void)handler, (void)work;
    const uint32_t self = pokemon(context, id, api);
    if (!species_is(self, 0x22b, api)) return;
    /* Zen form (1) at half HP or less (1084c124), otherwise 0. */
    const uint32_t hp = api->base->pokemon_value(user(api), self, 0xd);
    const uint32_t zen = hp <= px_battle_hp_fraction(self, 2, api->base);
    if (form(self, api) == zen) return;
    const uint32_t record = form_record(context, id, zen, 1, api);
    api->message(user(api), record + 8, 1, zen ? 0xbf : 0xc0);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_10_HANDLER(px_battle_zen_mode_leave)
{
    (void)handler, (void)work;
    if (get(0x02, api) != id || !species_is(pokemon(context, id, api), 0x22b, api)) return;
    /* Back to the standard form (message 0xc0 naming it, unflagged), whatever the form. */
    const uint32_t record = form_record(context, id, 0, 0, api);
    api->message(user(api), record + 8, 1, 0xc0);
    api->message_arg(user(api), record + 8, id);
    api->commit_actions(user(api), context, record);
}
