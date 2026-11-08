#include "battle_items_4.h"

enum { HANDLER_STATE = 0x10, HANDLER_ITEM = 0x38, CONSUMED = 6 };

static void *user(const struct px_battle_items_4_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_items_4_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_items_4_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

void px_battle_item_mark_consumed(uint32_t handler, const struct px_pokemon_api *m)
{
    m->store8(m->user, handler + HANDLER_STATE, CONSUMED);
}

uint32_t px_battle_item_consumed(uint32_t handler, const struct px_pokemon_api *m)
{
    return m->load8(m->user, handler + HANDLER_STATE) == CONSUMED;
}

void px_battle_gem_use(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, uint32_t type,
                       const struct px_battle_items_4_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    if (get(0x03, api) != id) return;
    if (get(0x16, api) != type) return;
    const uint32_t move = get(0x12, api) & UINT32_C(0xffff);
    if (api->damaging_move(user(api), move) == 0) return;
    if (api->fixed_target(user(api), move) != 0) return;
    if (px_battle_item_consumed(handler, m)) return;
    if (api->no_consume(user(api), context) != 0) return;
    px_battle_item_mark_consumed(handler, m);
    /* Kind 0x24: message 0xbc naming the gem and the move. */
    const uint32_t record = api->action_record(user(api), context, 0x24, id);
    api->message(user(api), record + 8, 1, 0xbc);
    api->message_arg(user(api), record + 8, m->load16(m->user, handler + HANDLER_ITEM));
    api->message_arg(user(api), record + 8, get(0x12, api));
    api->commit_actions(user(api), context, record);
    m->store32(m->user, work, 1);
}

PX_BATTLE_ITEMS_4_HANDLER(px_battle_gem_spent)
{
    (void)handler, (void)context;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x03, api) != id) return;
    if (m->load32(m->user, work) == 1) m->store32(m->user, work, 2);
}

/* Event 0x3d: x1.3 (literal 0x14cd after each handler) for the gem's type,
 * once used or in a pass that does not consume items. */
static void gem_power(uint32_t context, uint32_t id, uint32_t work, uint32_t type, const struct px_battle_items_4_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    if (get(0x03, api) != id) return;
    if (get(0x16, api) != type) return;
    if (m->load32(m->user, work) != 1 && api->no_consume(user(api), context) == 0) return;
    px_battle_event_multiply(0x32, 0x14cd, api->base->events);
}

#define GEM(power, use, type) \
    PX_BATTLE_ITEMS_4_HANDLER(power) { (void)handler; gem_power(context, id, work, type, api); } \
    PX_BATTLE_ITEMS_4_HANDLER(use) { px_battle_gem_use(handler, context, id, work, type, api); }
GEM(px_battle_fire_gem_power, px_battle_fire_gem_use, 9)
GEM(px_battle_water_gem_power, px_battle_water_gem_use, 0xa)
GEM(px_battle_electric_gem_power, px_battle_electric_gem_use, 0xc)
GEM(px_battle_grass_gem_power, px_battle_grass_gem_use, 0xb)
GEM(px_battle_ice_gem_power, px_battle_ice_gem_use, 0xe)
GEM(px_battle_fighting_gem_power, px_battle_fighting_gem_use, 1)
GEM(px_battle_poison_gem_power, px_battle_poison_gem_use, 3)
GEM(px_battle_ground_gem_power, px_battle_ground_gem_use, 4)
GEM(px_battle_flying_gem_power, px_battle_flying_gem_use, 2)
GEM(px_battle_psychic_gem_power, px_battle_psychic_gem_use, 0xd)
GEM(px_battle_bug_gem_power, px_battle_bug_gem_use, 6)
GEM(px_battle_rock_gem_power, px_battle_rock_gem_use, 5)
GEM(px_battle_ghost_gem_power, px_battle_ghost_gem_use, 7)
GEM(px_battle_dragon_gem_power, px_battle_dragon_gem_use, 0xf)
GEM(px_battle_dark_gem_power, px_battle_dark_gem_use, 0x10)
GEM(px_battle_steel_gem_power, px_battle_steel_gem_use, 8)
GEM(px_battle_normal_gem_power, px_battle_normal_gem_use, 0)
GEM(px_battle_fairy_gem_power, px_battle_fairy_gem_use, 0x11)
