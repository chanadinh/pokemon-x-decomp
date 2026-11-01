#include "battle_ability.h"

enum { KEYS = 4, VALUES = 0xc4, MAXIMA = 0x244, MINIMA = 0x3c4, KINDS = 0x544 };

/* The current frame's slot for a key, or -1 (after the abort) when absent.
 * Keys 0 and 1 abort and the search continues. */
static int32_t find(uint32_t key, const struct px_battle_event_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    if (key == 0 || key == 1) api->abort(api->user);
    for (uint32_t position = m->load32(m->user, api->stack); position < PX_BATTLE_EVENT_CAPACITY; ++position) {
        const uint32_t existing = m->load16(m->user, api->stack + KEYS + position * UINT32_C(2));
        if (existing == 0) break;
        if (existing == key) return (int32_t)position;
    }
    api->abort(api->user);
    return -1;
}

void px_battle_event_multiply(uint32_t key, uint32_t factor, const struct px_battle_event_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const int32_t slot = find(key, api);
    if (slot < 0) return;
    const uint32_t position = (uint32_t)slot;
    if (m->load8(m->user, api->stack + KINDS + position) != 3) {
        api->abort(api->user);
        return;
    }
    const uint32_t at = api->stack + position * UINT32_C(4);
    /* Signed 64-bit product, rounded at 0x800 and shifted by 12. */
    const int64_t product = (int64_t)(int32_t)m->load32(m->user, at + VALUES) * (int32_t)factor + 0x800;
    uint32_t value = (uint32_t)((uint64_t)product >> 12);
    const uint32_t maximum = m->load32(m->user, at + MAXIMA);
    if (maximum != 0 || m->load32(m->user, at + MINIMA) != 0) {
        const uint32_t minimum = m->load32(m->user, at + MINIMA);
        if ((int32_t)minimum > (int32_t)value) value = minimum;
        if ((int32_t)maximum < (int32_t)value) value = maximum;
    }
    m->store32(m->user, at + VALUES, value);
}

uint32_t px_battle_event_rewrite(uint32_t key, uint32_t value, const struct px_battle_event_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const int32_t slot = find(key, api);
    if (slot < 0) return 0;
    const uint32_t position = (uint32_t)slot;
    const uint32_t kind = m->load8(m->user, api->stack + KINDS + position);
    if (kind == 0 || kind == 1) {
        m->store16(m->user, api->stack + KEYS + position * UINT32_C(2), (uint16_t)key);
        m->store32(m->user, api->stack + VALUES + position * UINT32_C(4), value);
        /* A value (kind 1) can be rewritten once; a scaled value (kind 0) again. */
        if (m->load8(m->user, api->stack + KINDS + position) == 1) m->store8(m->user, api->stack + KINDS + position, 2);
        return 1;
    }
    if (kind == 4 || kind == 3) api->abort(api->user);
    return 0;
}

uint32_t px_battle_pokemon_by_id(uint32_t context, uint32_t id, const struct px_battle_ability_api *api)
{
    const struct px_pokemon_api *m = api->events->memory;
    const uint32_t table = m->load32(m->user, context + UINT32_C(8));
    if (id >= 0x18) api->events->abort(api->events->user);
    const uint32_t slot = table + id * UINT32_C(4) + UINT32_C(0x84);
    if (m->load32(m->user, slot) == 0) api->events->abort(api->events->user);
    return m->load32(m->user, slot);
}

uint32_t px_battle_hp_fraction(uint32_t pokemon, uint32_t divisor, const struct px_battle_ability_api *api)
{
    /* The unsigned divide veneer 1080041c is represented by C division. */
    return api->pokemon_value(api->events->user, pokemon, 0xe) / divisor;
}

uint32_t px_battle_modify(uint32_t value, uint32_t factor)
{
    const uint32_t product = value * factor;
    return (product >> 12) + ((product & UINT32_C(0xfff)) > UINT32_C(0x800));
}

static uint32_t get(uint32_t key, const struct px_battle_ability_api *api)
{
    return px_battle_event_get(key, api->events);
}

static void multiply(uint32_t factor, const struct px_battle_ability_api *api)
{
    px_battle_event_multiply(0x36, factor, api->events);
}

static uint32_t category(const struct px_battle_ability_api *api)
{
    return api->move_category(api->events->user, get(0x12, api) & UINT32_C(0xffff));
}

PX_BATTLE_ABILITY_HANDLER(px_battle_thick_fat)
{
    (void)handler, (void)context, (void)work;
    if (get(0x04, api) != id) return;
    const uint32_t type = get(0x16, api) & UINT32_C(0xff);
    if (type == 14 || type == 9) multiply(0x800, api);
}

PX_BATTLE_ABILITY_HANDLER(px_battle_huge_power)
{
    (void)handler, (void)context, (void)work;
    if (get(0x03, api) == id && category(api) == 1) multiply(0x2000, api);
}

/* Shared rule of the four pinch abilities: at or below a third of the
 * maximum HP, a move of the ability's type (full word compared). */
static void pinch(uint32_t context, uint32_t id, uint32_t type, const struct px_battle_ability_api *api)
{
    if (get(0x03, api) != id) return;
    const uint32_t pokemon = px_battle_pokemon_by_id(context, id, api);
    const uint32_t third = px_battle_hp_fraction(pokemon, 3, api);
    if (api->pokemon_value(api->events->user, pokemon, 0xd) > third) return;
    if (get(0x16, api) == type) multiply(0x1800, api);
}

PX_BATTLE_ABILITY_HANDLER(px_battle_blaze) { (void)handler, (void)work; pinch(context, id, 9, api); }
PX_BATTLE_ABILITY_HANDLER(px_battle_torrent) { (void)handler, (void)work; pinch(context, id, 10, api); }
PX_BATTLE_ABILITY_HANDLER(px_battle_overgrow) { (void)handler, (void)work; pinch(context, id, 11, api); }
PX_BATTLE_ABILITY_HANDLER(px_battle_swarm) { (void)handler, (void)work; pinch(context, id, 6, api); }

PX_BATTLE_ABILITY_HANDLER(px_battle_guts)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    if (api->pokemon_status(api->events->user, px_battle_pokemon_by_id(context, id, api)) == 0) return;
    if (category(api) != 1) return;
    multiply(0x1800, api);
    /* Tell the damage calculation to skip the burn halving. */
    px_battle_event_rewrite(0x54, 1, api->events);
}

PX_BATTLE_ABILITY_HANDLER(px_battle_marvel_scale)
{
    (void)handler, (void)work;
    if (get(0x04, api) != id) return;
    if (api->pokemon_status(api->events->user, px_battle_pokemon_by_id(context, id, api)) == 0) return;
    if (get(0x1b, api) == 1) multiply(0x1800, api);
}

PX_BATTLE_ABILITY_HANDLER(px_battle_hustle)
{
    (void)handler, (void)context, (void)work;
    if (get(0x03, api) != id || category(api) != 1) return;
    px_battle_event_rewrite(0x34, px_battle_modify(get(0x34, api), 0x1800), api->events);
}

/* Whether a battler on the same side (other than this one) has the ability.
 * The side list is written as id bytes into the handler's work area. */
static int ally_has(uint32_t context, uint32_t id, uint32_t work, uint32_t ability,
                    const struct px_battle_ability_api *api)
{
    const struct px_pokemon_api *m = api->events->memory;
    void *u = api->events->user;
    const uint32_t side = api->side_of(u, context, id);
    if (side == 0xa) return 0;
    const uint32_t count = api->side_battlers(u, context, side | 0x700, work);
    for (uint32_t i = 0; i < count; i = (i + 1) & UINT32_C(0xff)) {
        const uint32_t other = m->load8(m->user, work + i);
        if (other != id && api->pokemon_value(u, px_battle_pokemon_by_id(context, other, api), 0x11) == ability) return 1;
    }
    return 0;
}

PX_BATTLE_ABILITY_HANDLER(px_battle_plus_minus)
{
    (void)handler;
    if (get(0x03, api) != id) return;
    /* Plus (57) first, then Minus (58). */
    if (!ally_has(context, id, work, 0x39, api) && !ally_has(context, id, work, 0x3a, api)) return;
    if (category(api) == 2) multiply(0x1800, api);
}

PX_BATTLE_ABILITY_HANDLER(px_battle_defeatist)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    const uint32_t pokemon = px_battle_pokemon_by_id(context, id, api);
    const uint32_t hp = api->pokemon_value(api->events->user, pokemon, 0xd);
    if (hp <= px_battle_hp_fraction(pokemon, 2, api)) multiply(0x800, api);
}

PX_BATTLE_ABILITY_HANDLER(px_battle_fur_coat)
{
    (void)handler, (void)context, (void)work;
    if (get(0x04, api) == id && get(0x1b, api) == 1) multiply(0x2000, api);
}

PX_BATTLE_ABILITY_HANDLER(px_battle_grass_pelt)
{
    (void)handler, (void)work;
    if (get(0x04, api) != id || get(0x1b, api) != 1) return;
    if (api->field_check(api->events->user, context) == 1) multiply(0x1800, api);
}
