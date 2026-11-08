#include "battle_items_2.h"
#include "../battle_modifier/battle_modifier.h"

enum { HANDLER_ITEM = 0x38 };

static void *user(const struct px_battle_items_2_api *api)
{
    return api->items->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_items_2_api *api)
{
    return api->items->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_items_2_api *api)
{
    return px_battle_event_get(key, api->items->base->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_items_2_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->items->base);
}

static uint32_t item(uint32_t handler, const struct px_battle_items_2_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load16(m->user, handler + HANDLER_ITEM);
}

/* The item's parameter 3: the Figy family's max-HP divisor, the stat
 * berries' pinch divisor. */
static uint32_t param3(uint32_t handler, const struct px_battle_items_2_api *api)
{
    return api->items->item_param(user(api), item(handler, api), 3);
}

/* Asks for the berry: marks the handler and commits a kind-0 record. */
static void request_use(uint32_t handler, uint32_t context, uint32_t id, const struct px_battle_items_2_api *api)
{
    px_battle_handler_mark_reentry(handler, api->items->base->events);
    api->items->ability_popup(user(api), context, 0, id);
}

/* The Figy family's checks also give up once work word 0 is set and for a
 * battler with condition 0xf. */
static int figy_blocked(uint32_t context, uint32_t id, uint32_t work, const struct px_battle_items_2_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    if (m->load32(m->user, work) != 0) return 1;
    return api->items->has_status(user(api), pokemon(context, id, api), 0xf) != 0;
}

PX_BATTLE_ITEMS_2_HANDLER(px_battle_figy_check_9c)
{
    if (figy_blocked(context, id, work, api)) return;
    px_battle_hp_berry_check(handler, context, id, 2, 1, api->items);
}

PX_BATTLE_ITEMS_2_HANDLER(px_battle_figy_check_5e)
{
    if (figy_blocked(context, id, work, api)) return;
    px_battle_hp_berry_check(handler, context, id, 2, 0, api->items);
}

PX_BATTLE_ITEMS_2_HANDLER(px_battle_figy_check_61)
{
    /* No key 0x02 test here. */
    if (figy_blocked(context, id, work, api)) return;
    if (px_battle_item_pinch(context, id, 2, api->items) == 0) return;
    request_use(handler, context, id, api);
}

void px_battle_figy_berry_use(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, uint32_t flavor,
                              const struct px_battle_items_2_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const struct px_battle_items_api *items = api->items;
    if (m->load32(m->user, work) != 0) return;
    if (get(0x02, api) != id) return;
    const uint32_t p = pokemon(context, id, api);
    /* Kind 4: restore max HP / parameter 3 with message 0x392 (literal
     * 1084c6fc) naming the battler and the item. */
    uint32_t record = items->action_record(user(api), context, 4, id);
    m->store8(m->user, record + 6, (uint8_t)id);
    m->store16(m->user, record + 4, (uint16_t)px_battle_hp_share(p, param3(handler, api), api->abilities));
    items->message(user(api), record + 8, 2, 0x392);
    items->message_arg(user(api), record + 8, id);
    items->message_arg(user(api), record + 8, item(handler, api));
    items->commit_actions(user(api), context, record);
    /* A battler that dislikes the flavor becomes confused: kind 0xb, +4
     * status 6, +8 its condition word, +0xf the battler. */
    if (api->flavor_taste(user(api), m->load32(m->user, p), flavor) == 2) {
        record = items->action_record(user(api), context, 0xb, id);
        m->store8(m->user, record + 0xf, (uint8_t)id);
        m->store16(m->user, record + 4, 6);
        api->condition_for(user(api), 6, p, record + 8);
        items->commit_actions(user(api), context, record);
    }
    m->store32(m->user, work, 1);
}

/* Event 0x7b calls the body directly; event 0x7c first requires work word
 * 6 (bxeq lr), then falls into the same call. */
#define FIGY_BERRY(use, use_7c, flavor) \
    PX_BATTLE_ITEMS_2_HANDLER(use) { px_battle_figy_berry_use(handler, context, id, work, flavor, api); } \
    PX_BATTLE_ITEMS_2_HANDLER(use_7c) \
    { \
        const struct px_pokemon_api *m = memory(api); \
        if (m->load32(m->user, work + UINT32_C(0x18)) == 0) return; \
        px_battle_figy_berry_use(handler, context, id, work, flavor, api); \
    }
FIGY_BERRY(px_battle_figy_use, px_battle_figy_use_7c, 0)
FIGY_BERRY(px_battle_wiki_use, px_battle_wiki_use_7c, 1)
FIGY_BERRY(px_battle_mago_use, px_battle_mago_use_7c, 2)
FIGY_BERRY(px_battle_aguav_use, px_battle_aguav_use_7c, 3)
FIGY_BERRY(px_battle_iapapa_use, px_battle_iapapa_use_7c, 4)

/* The stat berries' checks pass parameter 3 as the divisor. Each looks the
 * battler up first and discards it. */
PX_BATTLE_ITEMS_2_HANDLER(px_battle_stat_check_9c)
{
    (void)work;
    if (get(0x02, api) != id) return;
    (void)pokemon(context, id, api);
    px_battle_hp_berry_check(handler, context, id, param3(handler, api), 1, api->items);
}

PX_BATTLE_ITEMS_2_HANDLER(px_battle_stat_check_5e)
{
    (void)work;
    if (get(0x02, api) != id) return;
    (void)pokemon(context, id, api);
    px_battle_hp_berry_check(handler, context, id, param3(handler, api), 0, api->items);
}

PX_BATTLE_ITEMS_2_HANDLER(px_battle_stat_check_61)
{
    (void)work;
    /* No key 0x02 test here. */
    (void)pokemon(context, id, api);
    if (px_battle_item_pinch(context, id, param3(handler, api), api->items) == 0) return;
    request_use(handler, context, id, api);
}

/* Kind 0xd for the battler: stat +stages. */
static void raise_stat(uint32_t context, uint32_t id, uint32_t stat, uint32_t stages,
                       const struct px_battle_items_2_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t record = api->items->action_record(user(api), context, 0xd, id);
    m->store8(m->user, record + 0x17, (uint8_t)stages);
    m->store8(m->user, record + 4, (uint8_t)stat);
    m->store8(m->user, record + 0x16, 1);
    m->store8(m->user, record + 0xc, (uint8_t)id);
    api->items->commit_actions(user(api), context, record);
}

#define STAT_BERRY(use, stat) \
    PX_BATTLE_ITEMS_2_HANDLER(use) \
    { \
        (void)handler, (void)work; \
        if (get(0x02, api) != id) return; \
        raise_stat(context, id, stat, 1, api); \
    }
STAT_BERRY(px_battle_liechi_use, 1)
STAT_BERRY(px_battle_ganlon_use, 2)
STAT_BERRY(px_battle_salac_use, 5)
STAT_BERRY(px_battle_petaya_use, 3)
STAT_BERRY(px_battle_apicot_use, 4)

PX_BATTLE_ITEMS_2_HANDLER(px_battle_lansat_use)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    const struct px_battle_items_api *items = api->items;
    if (get(0x02, api) != id) return;
    /* Nothing when the battler already has state 9 (getting pumped). */
    if (px_battle_pokemon_state(pokemon(context, id, api), 9, m) != 0) return;
    /* Kind 0x17: +4 the state, +5 the battler. */
    uint32_t record = items->action_record(user(api), context, 0x17, id);
    m->store8(m->user, record + 5, (uint8_t)id);
    m->store8(m->user, record + 4, 9);
    items->commit_actions(user(api), context, record);
    /* Kind 3: message 0x3ef (literal 108a607c) naming the battler and the
     * item. */
    record = items->action_record(user(api), context, 3, id);
    items->message(user(api), record + 4, 2, 0x3ef);
    items->message_arg(user(api), record + 4, id);
    items->message_arg(user(api), record + 4, item(handler, api));
    items->commit_actions(user(api), context, record);
}

PX_BATTLE_ITEMS_2_HANDLER(px_battle_starf_use)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    const uint32_t p = pokemon(context, id, api);
    /* The stats that can still rise, in table order, in a temporary buffer. */
    const uint32_t buffer = api->temp_buffer(user(api), context, 5);
    uint32_t count = 0;
    for (uint32_t i = 0; i < 5; ++i) {
        if (api->stage_can_change(user(api), p, m->load8(m->user, api->starf_stats + i), 1) == 0) continue;
        m->store8(m->user, buffer + count, m->load8(m->user, api->starf_stats + i));
        ++count;
    }
    if (count == 0) return;
    /* The roll comes before the battler test. */
    const uint32_t stat = m->load8(m->user, buffer + api->random(user(api), count));
    if (get(0x02, api) != id) return;
    raise_stat(context, id, stat, 2, api);
}
