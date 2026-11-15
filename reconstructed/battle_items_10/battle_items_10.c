#include "battle_items_10.h"

enum { HANDLER_ITEM = 0x38, STAGES = 0x104 };

static const struct px_battle_ability_api *base(const struct px_battle_items_10_api *api)
{
    return api->items->base;
}

static void *user(const struct px_battle_items_10_api *api)
{
    return base(api)->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_items_10_api *api)
{
    return base(api)->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_items_10_api *api)
{
    return px_battle_event_get(key, base(api)->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_items_10_api *api)
{
    return px_battle_pokemon_by_id(context, id, base(api));
}

uint32_t px_battle_stat_lowered(uint32_t p, const struct px_pokemon_api *m)
{
    for (uint32_t i = 0; i < 7; ++i)
        if ((int8_t)m->load8(m->user, p + STAGES + i) < 6) return 1;
    return 0;
}

static void request_use(uint32_t handler, uint32_t context, uint32_t id, const struct px_battle_items_10_api *api)
{
    px_battle_handler_mark_reentry(handler, base(api)->events);
    api->items->ability_popup(user(api), context, 0, id);
}

static void if_lowered(uint32_t handler, uint32_t context, uint32_t id, const struct px_battle_items_10_api *api)
{
    if (px_battle_stat_lowered(pokemon(context, id, api), memory(api)) == 0) return;
    request_use(handler, context, id, api);
}

/* Event 0x02: unless key 0x0c is 6; no battler test. */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_white_herb_02)
{
    (void)work;
    if (get(0x0c, api) == 6) return;
    if_lowered(handler, context, id, api);
}

/* Events 0x60 and 0x61: no key test at all. */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_white_herb_60)
{
    (void)work;
    if_lowered(handler, context, id, api);
}

PX_BATTLE_ITEMS_10_HANDLER(px_battle_white_herb_80)
{
    (void)work;
    if (get(0x02, api) != id) return;
    if_lowered(handler, context, id, api);
}

/* Event 0x9c: the frame's use flags (key 0x2f) must allow it (bit 3). */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_white_herb_9c)
{
    (void)work;
    if (!(get(0x2f, api) & 8)) return;
    if (get(0x02, api) != id) return;
    if_lowered(handler, context, id, api);
}

/* Kind 0x11 (+4 the battler) restores the stages; then kind 3 with message
 * 0x3f8 naming the holder and the herb. */
static void restore_stages(uint32_t handler, uint32_t context, uint32_t id, const struct px_battle_items_10_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const struct px_battle_items_api *items = api->items;
    uint32_t at = items->action_record(user(api), context, 0x11, id);
    m->store8(m->user, at + 4, (uint8_t)id);
    items->commit_actions(user(api), context, at);
    at = items->action_record(user(api), context, 3, id);
    items->message(user(api), at + 4, 2, 0x3f8);
    items->message_arg(user(api), at + 4, id);
    items->message_arg(user(api), at + 4, m->load16(m->user, handler + HANDLER_ITEM));
    items->commit_actions(user(api), context, at);
}

PX_BATTLE_ITEMS_10_HANDLER(px_battle_white_herb_use)
{
    (void)work;
    if (get(0x02, api) == id) restore_stages(handler, context, id, api);
}

/* Event 0x7c: only with work word 6 set (read before anything else). */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_white_herb_use_7c)
{
    const struct px_pokemon_api *m = memory(api);
    if (m->load32(m->user, work + UINT32_C(0x18)) == 0) return;
    if (get(0x02, api) == id) restore_stages(handler, context, id, api);
}

/* Mental Herb: any condition of the first batch's six-entry list. */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_mental_herb_check)
{
    (void)work;
    if (get(0x02, api) != id) return;
    if (px_battle_listed_condition(pokemon(context, id, api), api->items) == 0) return;
    request_use(handler, context, id, api);
}

PX_BATTLE_ITEMS_10_HANDLER(px_battle_mental_herb_use)
{
    (void)work;
    px_battle_cure_berry_use(handler, context, id, 0x28, api->items);
}

PX_BATTLE_ITEMS_10_HANDLER(px_battle_mental_herb_use_7c)
{
    const struct px_pokemon_api *m = memory(api);
    if (m->load32(m->user, work + UINT32_C(0x18)) == 0) return;
    px_battle_cure_berry_use(handler, context, id, 0x28, api->items);
}

/* Event 0x7b: kind 3 with message 0x404 (literal 108a9264). */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_power_herb_use)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    const struct px_battle_items_api *items = api->items;
    if (get(0x02, api) != id) return;
    const uint32_t at = items->action_record(user(api), context, 3, id);
    items->message(user(api), at + 4, 2, 0x404);
    items->message_arg(user(api), at + 4, id);
    items->message_arg(user(api), at + 4, m->load16(m->user, handler + HANDLER_ITEM));
    items->commit_actions(user(api), context, at);
}

/* Event 0x9f: the attacker's charging move skips its turn (key 0x53 = 1),
 * except Sky Drop (507); work word 0 = 1 when the rewrite took. */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_power_herb_charge)
{
    (void)handler, (void)context;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x03, api) != id) return;
    if (get(0x12, api) == 0x1fb) return;
    if (px_battle_event_rewrite(0x53, 1, base(api)->events) != 0) m->store32(m->user, work, 1);
}

/* Event 0xa2: once it skipped the turn, ask for the herb. */
PX_BATTLE_ITEMS_10_HANDLER(px_battle_power_herb_check)
{
    const struct px_pokemon_api *m = memory(api);
    if (get(0x03, api) != id) return;
    if (m->load32(m->user, work) == 0) return;
    request_use(handler, context, id, api);
}
