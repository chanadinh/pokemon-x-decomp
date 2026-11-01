#include "battle_switchin.h"

static void *user(const struct px_battle_switchin_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_switchin_api *api)
{
    return api->base->events->memory;
}

static uint32_t entering(uint32_t id, const struct px_battle_switchin_api *api)
{
    return px_battle_event_get(0x02, api->base->events) == id;
}

static uint32_t flag_record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_switchin_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t record = api->action_record(user(api), context, kind, id);
    m->store32(m->user, record, m->load32(m->user, record) | UINT32_C(0x800000));
    return record;
}

void px_battle_set_entry_weather(uint32_t context, uint32_t id, uint32_t weather, uint32_t rock,
                                 const struct px_battle_switchin_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    if (!entering(id, api)) return;
    uint32_t turns = 5;
    if (rock != 0 && m->load16(m->user, px_battle_pokemon_by_id(context, id, api->base) + UINT32_C(0x12)) == rock &&
        api->item_usable(user(api), context, id) != 0)
        turns = 8;
    const uint32_t record = api->action_record(user(api), context, 0x1e, id);
    m->store8(m->user, record + 4, (uint8_t)weather);
    m->store8(m->user, record + 5, (uint8_t)turns);
    m->store32(m->user, record, m->load32(m->user, record) | UINT32_C(0x800000));
    api->commit_actions(user(api), context, record);
}

/* Heat Rock (284), Damp Rock (285, literal 108bef70), Smooth Rock (283,
 * literal 108bf90c) and Icy Rock (282, literal 108c02f0). */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_drought) { (void)handler, (void)work; px_battle_set_entry_weather(context, id, 1, 0x11c, api); }
PX_BATTLE_SWITCHIN_HANDLER(px_battle_drizzle) { (void)handler, (void)work; px_battle_set_entry_weather(context, id, 2, 0x11d, api); }
PX_BATTLE_SWITCHIN_HANDLER(px_battle_sand_stream) { (void)handler, (void)work; px_battle_set_entry_weather(context, id, 4, 0x11b, api); }
PX_BATTLE_SWITCHIN_HANDLER(px_battle_snow_warning) { (void)handler, (void)work; px_battle_set_entry_weather(context, id, 3, 0x11a, api); }

#define CURE(name, status) \
    PX_BATTLE_SWITCHIN_HANDLER(name) \
    { \
        (void)handler, (void)work; \
        if (entering(id, api)) api->cure_status(user(api), context, id, status); \
    }
CURE(px_battle_limber_entry, 1)
CURE(px_battle_insomnia_entry, 2)
CURE(px_battle_immunity_entry, 5)
CURE(px_battle_own_tempo_entry, 6)
CURE(px_battle_water_veil_entry, 4)
CURE(px_battle_magma_armor_entry, 3)

PX_BATTLE_SWITCHIN_HANDLER(px_battle_oblivious_entry)
{
    (void)handler, (void)work;
    if (entering(id, api)) api->cure_status(user(api), context, id, 7);
    if (entering(id, api)) api->cure_status(user(api), context, id, 0xb);
}

PX_BATTLE_SWITCHIN_HANDLER(px_battle_intimidate)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!entering(id, api)) return;
    const uint32_t selector = api->opposing_selector(user(api), context, id) | 0x100;
    const uint32_t buffer = api->temp_buffer(user(api), context, 0xa);
    const uint32_t count = api->side_battlers(user(api), context, selector, buffer);
    if (count == 0) return;
    api->ability_popup(user(api), context, 1, id);
    /* One record lowers Attack (stat 1) by one stage for every battler listed. */
    const uint32_t record = api->action_record(user(api), context, 0xd, id);
    m->store8(m->user, record + 4, 1);
    m->store8(m->user, record + 0x16, (uint8_t)count);
    m->store8(m->user, record + 0x17, 0xff);
    m->store8(m->user, record + 0x18, m->load8(m->user, record + 0x18) | 2);
    for (uint32_t i = 0; i != count; ++i) m->store8(m->user, record + 0xc + i, m->load8(m->user, buffer + i));
    api->commit_actions(user(api), context, record);
    api->ability_popup(user(api), context, 2, id);
}

/* Announced once (work word); the handler's word +0xc then takes a hook. */
static void announce_once(uint32_t handler, uint32_t work, uint32_t hook, const struct px_battle_switchin_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store32(m->user, handler + UINT32_C(0xc), hook);
    m->store32(m->user, work, 1);
}

PX_BATTLE_SWITCHIN_HANDLER(px_battle_unnerve)
{
    const struct px_pokemon_api *m = memory(api);
    if (m->load32(m->user, work) != 0) return;
    const uint32_t side = api->opposing_side(user(api), api->side_of(user(api), id));
    api->ability_popup(user(api), context, 1, id);
    const uint32_t record = api->action_record(user(api), context, 3, id);
    api->message(user(api), record + 4, 1, 0xb6);
    api->message_arg(user(api), record + 4, side);
    api->commit_actions(user(api), context, record);
    api->ability_popup(user(api), context, 2, id);
    announce_once(handler, work, api->unnerve_hook, api);
}

PX_BATTLE_SWITCHIN_HANDLER(px_battle_klutz)
{
    const struct px_pokemon_api *m = memory(api);
    (void)context;
    if (entering(id, api) && m->load32(m->user, work) == 0) announce_once(handler, work, api->klutz_hook, api);
}

PX_BATTLE_SWITCHIN_HANDLER(px_battle_forecast)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    if (!entering(id, api)) return;
    const uint32_t weather = api->weather(user(api), context);
    const uint32_t pokemon = px_battle_pokemon_by_id(context, id, api->base);
    /* Castform (351): form 1 sun, 2 rain, 3 hail, otherwise 0. */
    if (m->load16(m->user, pokemon + UINT32_C(0xc)) == 0x15f) {
        const uint32_t form = m->load8(m->user, pokemon + UINT32_C(0x153));
        const uint32_t wanted = weather >= 1 && weather <= 3 ? weather : 0;
        if (wanted != form) {
            const uint32_t record = api->action_record(user(api), context, 0x3a, id);
            const uint32_t flags = m->load32(m->user, record);
            m->store8(m->user, record + 5, (uint8_t)wanted);
            m->store8(m->user, record + 4, (uint8_t)id);
            m->store32(m->user, record, flags | UINT32_C(0x800000));
            api->message(user(api), record + 8, 2, 0xde);
            api->message_arg(user(api), record + 8, id);
            api->commit_actions(user(api), context, record);
        }
    }
    m->store32(m->user, work, 1);
}

PX_BATTLE_SWITCHIN_HANDLER(px_battle_flower_gift)
{
    const struct px_pokemon_api *m = memory(api);
    if (!entering(id, api)) return;
    /* Cherrim (421) only. */
    if (m->load16(m->user, px_battle_pokemon_by_id(context, id, api->base) + UINT32_C(0xc)) != 0x1a5) return;
    api->flower_gift(user(api), handler, context, id, api->weather(user(api), context) == 1, 1);
    m->store32(m->user, work, 1);
}

PX_BATTLE_SWITCHIN_HANDLER(px_battle_slow_start)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    if (!entering(id, api)) return;
    m->store32(m->user, work, 1);
    m->store32(m->user, work + 4, 0);
    m->store32(m->user, work + 8, 0);
    m->store32(m->user, work + 0xc, 1);
    const uint32_t record = flag_record(context, 3, id, api);
    api->message(user(api), record + 4, 2, 0x1f0);
    api->message_arg(user(api), record + 4, id);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_SWITCHIN_HANDLER(px_battle_truant)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    if (!entering(id, api)) return;
    if (api->truant_check(user(api), px_battle_pokemon_by_id(context, id, api->base), 0) != 0) m->store32(m->user, work, 1);
}

/* The ability popup around a message naming the battler. */
static void announce_body(uint32_t context, uint32_t id, uint32_t message, const struct px_battle_switchin_api *api)
{
    api->ability_popup(user(api), context, 1, id);
    const uint32_t record = api->action_record(user(api), context, 3, id);
    api->message(user(api), record + 4, 2, message);
    api->message_arg(user(api), record + 4, id);
    api->commit_actions(user(api), context, record);
    api->ability_popup(user(api), context, 2, id);
}

static void announce(uint32_t context, uint32_t id, uint32_t message, const struct px_battle_switchin_api *api)
{
    if (entering(id, api)) announce_body(context, id, message, api);
}

PX_BATTLE_SWITCHIN_HANDLER(px_battle_mold_breaker)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!entering(id, api)) return;
    /* The handler id is the ability: Turboblaze (163), Teravolt (164), Mold Breaker. */
    const uint32_t ability = m->load16(m->user, handler + UINT32_C(0x38));
    announce_body(context, id, ability == 0xa3 ? 0x1f9 : ability == 0xa4 ? 0x1f6 : 0x1ba, api);  /* literals 108c49bc-c4 */
}

PX_BATTLE_SWITCHIN_HANDLER(px_battle_pressure) { (void)handler, (void)work; announce(context, id, 0x1e7, api); }  /* literal 108c3cc0 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_dark_aura_entry) { (void)handler, (void)work; announce(context, id, 0x4b1, api); }  /* literal 108c3884 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_fairy_aura_entry) { (void)handler, (void)work; announce(context, id, 0x4b4, api); }  /* literal 108c40ac */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_aura_break) { (void)handler, (void)work; announce(context, id, 0x4cf, api); }  /* literal 108c3fdc */
