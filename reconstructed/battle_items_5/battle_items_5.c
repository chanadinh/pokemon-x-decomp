#include "battle_items_5.h"

enum { SPECIES = 0xc, FORM = 0x153 };

static void *user(const struct px_battle_items_5_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_items_5_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_items_5_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_items_5_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static uint32_t species(uint32_t p, const struct px_battle_items_5_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load16(m->user, p + SPECIES);
}

static void multiply(uint32_t factor, const struct px_battle_items_5_api *api)
{
    px_battle_event_multiply(0x36, factor, api->base->events);
}

/* The category of the move (low half of key 0x12) from its data. */
static uint32_t move_category(const struct px_battle_items_5_api *api)
{
    return api->base->move_category(user(api), get(0x12, api) & UINT32_C(0xffff));
}

/* Event 0x13: the holder's speed. */
static void speed(uint32_t id, uint32_t factor, const struct px_battle_items_5_api *api)
{
    if (get(0x02, api) == id) multiply(factor, api);
}

PX_BATTLE_ITEMS_5_HANDLER(px_battle_macho_brace) { (void)handler, (void)context, (void)work; speed(id, 0x800, api); }
PX_BATTLE_ITEMS_5_HANDLER(px_battle_iron_ball_speed) { (void)handler, (void)context, (void)work; speed(id, 0x800, api); }
PX_BATTLE_ITEMS_5_HANDLER(px_battle_choice_scarf) { (void)handler, (void)context, (void)work; speed(id, 0x1800, api); }
PX_BATTLE_ITEMS_5_HANDLER(px_battle_power_item) { (void)handler, (void)context, (void)work; speed(id, 0x800, api); }

PX_BATTLE_ITEMS_5_HANDLER(px_battle_quick_powder)
{
    (void)handler, (void)work;
    if (get(0x02, api) != id) return;
    const uint32_t p = pokemon(context, id, api);
    if (species(p, api) != 0x84) return;
    if (api->transformed(user(api), p) != 0) return;
    multiply(0x2000, api);
}

/* Event 0x3b: the critical-hit stage, as a byte. */
static void raise_critical(uint32_t stages, const struct px_battle_items_5_api *api)
{
    px_battle_event_rewrite(0x2d, (get(0x2d, api) + stages) & UINT32_C(0xff), api->base->events);
}

PX_BATTLE_ITEMS_5_HANDLER(px_battle_scope_lens)
{
    (void)handler, (void)context, (void)work;
    if (get(0x03, api) == id) raise_critical(1, api);
}

static void species_critical(uint32_t context, uint32_t id, uint32_t kind, const struct px_battle_items_5_api *api)
{
    if (get(0x03, api) != id) return;
    if (species(pokemon(context, id, api), api) != kind) return;
    raise_critical(2, api);
}

PX_BATTLE_ITEMS_5_HANDLER(px_battle_lucky_punch) { (void)handler, (void)work; species_critical(context, id, 0x71, api); }
PX_BATTLE_ITEMS_5_HANDLER(px_battle_stick) { (void)handler, (void)work; species_critical(context, id, 0x53, api); }

/* Event 0x40: the attacker's stat. */
static void category_attack(uint32_t id, uint32_t category, const struct px_battle_items_5_api *api)
{
    if (get(0x03, api) != id) return;
    if (move_category(api) == category) multiply(0x1800, api);
}

PX_BATTLE_ITEMS_5_HANDLER(px_battle_choice_band) { (void)handler, (void)context, (void)work; category_attack(id, 1, api); }
PX_BATTLE_ITEMS_5_HANDLER(px_battle_choice_specs) { (void)handler, (void)context, (void)work; category_attack(id, 2, api); }

static int latias_or_latios(uint32_t kind)
{
    return kind == 0x17c || kind == 0x17d;
}

PX_BATTLE_ITEMS_5_HANDLER(px_battle_soul_dew_attack)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    if (!latias_or_latios(species(pokemon(context, id, api), api))) return;
    if (get(0x1b, api) == 2) multiply(0x1800, api);
}

PX_BATTLE_ITEMS_5_HANDLER(px_battle_deep_sea_tooth)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    if (species(pokemon(context, id, api), api) != 0x16e) return;
    if (move_category(api) == 2) multiply(0x2000, api);
}

PX_BATTLE_ITEMS_5_HANDLER(px_battle_light_ball)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    if (species(pokemon(context, id, api), api) == 0x19) multiply(0x2000, api);
}

PX_BATTLE_ITEMS_5_HANDLER(px_battle_thick_club)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    const uint32_t kind = species(pokemon(context, id, api), api);
    if (kind != 0x68 && kind != 0x69) return;
    if (move_category(api) == 1) multiply(0x2000, api);
}

/* Event 0x41: the defender's stat. */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_soul_dew_defense)
{
    (void)handler, (void)work;
    if (get(0x04, api) != id) return;
    if (!latias_or_latios(species(pokemon(context, id, api), api))) return;
    if (get(0x1b, api) == 2) multiply(0x1800, api);
}

PX_BATTLE_ITEMS_5_HANDLER(px_battle_deep_sea_scale)
{
    (void)handler, (void)work;
    if (get(0x04, api) != id) return;
    if (species(pokemon(context, id, api), api) != 0x16e) return;
    if (get(0x1b, api) == 2) multiply(0x2000, api);
}

PX_BATTLE_ITEMS_5_HANDLER(px_battle_metal_powder)
{
    (void)handler, (void)work;
    if (get(0x04, api) != id) return;
    const uint32_t p = pokemon(context, id, api);
    if (species(p, api) != 0x84) return;
    if (api->transformed(user(api), p) != 0) return;
    if (get(0x1b, api) == 1) multiply(0x2000, api);
}

PX_BATTLE_ITEMS_5_HANDLER(px_battle_eviolite)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x04, api) != id) return;
    const uint32_t p = pokemon(context, id, api);
    /* Floette (670) in form 5 (the Eternal Flower) is excluded. */
    if (species(p, api) == 0x29e && m->load8(m->user, p + FORM) == 5) return;
    if (api->can_evolve(user(api), context, id) != 0) multiply(0x1800, api);
}

PX_BATTLE_ITEMS_5_HANDLER(px_battle_assault_vest)
{
    (void)handler, (void)context, (void)work;
    if (get(0x04, api) != id) return;
    if (get(0x1b, api) == 2) multiply(0x1800, api);
}

/* Event 0x22: lock the attacker into its move, unless it is Struggle (165).
 * Kind 0xb: +4 status 0x1b (halfword), +8 the move's lock condition, +0xf
 * the battler, +0x10 2. */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_choice_lock)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x03, api) != id) return;
    const uint32_t move = get(0x12, api) & UINT32_C(0xffff);
    if (move == 0xa5) return;
    const uint32_t record = api->action_record(user(api), context, 0xb, id);
    m->store8(m->user, record + 0xf, (uint8_t)id);
    m->store16(m->user, record + 4, 0x1b);
    m->store32(m->user, record + 8, api->lock_condition(user(api), move));
    m->store8(m->user, record + 0x10, 2);
    api->commit_actions(user(api), context, record);
}

/* Event 0xa8: release the lock. Kind 0xa: +4 cure 0x1b, +5 the battler,
 * +0x11 count 1, +0x12 1. */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_choice_release)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    const uint32_t record = api->action_record(user(api), context, 0xa, id);
    m->store8(m->user, record + 4, 0x1b);
    m->store8(m->user, record + 0x11, 1);
    m->store8(m->user, record + 0x12, 1);
    m->store8(m->user, record + 5, (uint8_t)id);
    api->commit_actions(user(api), context, record);
}

/* Event 0x12: the holder is grounded (key 0x42 = 1). */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_iron_ball_ground)
{
    (void)handler, (void)context, (void)work;
    if (get(0x02, api) == id) px_battle_event_rewrite(0x42, 1, api->base->events);
}

/* Event 0x7c: kind 0xb paralysis (status 1) for the low byte of key 0x03,
 * +8 its condition word, +0xf the holder. */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_light_ball_use)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    const uint32_t record = api->action_record(user(api), context, 0xb, get(0x03, api) & UINT32_C(0xff));
    m->store8(m->user, record + 0xf, (uint8_t)id);
    m->store16(m->user, record + 4, 1);
    m->store32(m->user, record + 8, api->condition(user(api), 1));
    api->commit_actions(user(api), context, record);
}
