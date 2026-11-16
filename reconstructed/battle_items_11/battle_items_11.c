#include "battle_items_11.h"

enum { SWITCH_FLAGS = 0x1093, EJECT_CLAIMED = 8, SWITCH_PENDING = 1 };

static const struct px_battle_ability_api *base(const struct px_battle_items_11_api *api)
{
    return api->items->base;
}

static void *user(const struct px_battle_items_11_api *api)
{
    return base(api)->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_items_11_api *api)
{
    return base(api)->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_items_11_api *api)
{
    return px_battle_event_get(key, base(api)->events);
}

uint32_t px_battle_claim_eject(uint32_t context, const struct px_pokemon_api *m)
{
    const uint8_t flags = m->load8(m->user, context + SWITCH_FLAGS);
    if (flags & EJECT_CLAIMED) return 0;
    m->store8(m->user, context + SWITCH_FLAGS, (uint8_t)(flags | EJECT_CLAIMED));
    return 1;
}

uint32_t px_battle_switch_pending(uint32_t context, const struct px_pokemon_api *m)
{
    return m->load8(m->user, context + SWITCH_FLAGS) & SWITCH_PENDING;
}

/* Whether the holder is among the battlers hit (keys 0x06 + i, i below key
 * 0x05). The original runs the item's checks at the first match and returns
 * whatever they decide, so stopping at the first match is the same. */
static int was_hit(uint32_t id, const struct px_battle_items_11_api *api)
{
    const uint32_t count = get(0x05, api);
    for (uint32_t i = 0; i < count; ++i)
        if (get((i + 6) & UINT32_C(0xff), api) == id) return 1;
    return 0;
}

static void request_use(uint32_t handler, uint32_t context, uint32_t id, const struct px_battle_items_11_api *api)
{
    px_battle_handler_mark_reentry(handler, base(api)->events);
    api->items->ability_popup(user(api), context, 0, id);
}

/* Red Card: the attacker is sent back. Kind 0x24 shows message 0x1a1
 * (literal 108a5fb8) naming the holder and the attacker; kind 0x2f forces
 * the switch: +4 halfword 0x2a7 (literal 108a5fbc), +6 key 0x03, +7's high
 * nibble 1. */
PX_BATTLE_ITEMS_11_HANDLER(px_battle_red_card)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    const struct px_battle_items_api *items = api->items;
    if (!was_hit(id, api)) return;
    const uint32_t attacker = get(0x03, api) & UINT32_C(0xff);
    if (api->opposing_selector(user(api), context, attacker) == 0xa) return;
    if (get(0x4e, api) != 0) return;
    if (api->can_switch(user(api), context, attacker) == 0) return;
    if (api->battle_decided(user(api), context) != 0) return;
    uint32_t at = items->action_record(user(api), context, 0x24, id);
    items->message(user(api), at + 8, 2, 0x1a1);
    items->message_arg(user(api), at + 8, id);
    items->message_arg(user(api), at + 8, attacker);
    items->commit_actions(user(api), context, at);
    at = items->action_record(user(api), context, 0x2f, id);
    const uint32_t target = get(0x03, api);
    const uint8_t flags = m->load8(m->user, at + 7);
    m->store8(m->user, at + 6, (uint8_t)target);
    m->store16(m->user, at + 4, 0x2a7);
    m->store8(m->user, at + 7, (uint8_t)((flags & ~0xf0) | 0x10));
    items->commit_actions(user(api), context, at);
}

PX_BATTLE_ITEMS_11_HANDLER(px_battle_eject_button_check)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!was_hit(id, api)) return;
    if (get(0x4e, api) != 0) return;
    if (api->charging(user(api), context, id) != 0) return;
    if (api->battle_decided(user(api), context) != 0) return;
    if (px_battle_switch_pending(context, m) != 0) return;
    if (api->can_switch(user(api), context, id) == 0) return;
    if (px_battle_claim_eject(context, m) == 0) return;
    request_use(handler, context, id, api);
}

/* Event 0x7b: kind 3 with message 0x19e (literal 108aa468) naming the holder,
 * then kind 0x2a with +0x4c the holder (the switch). */
PX_BATTLE_ITEMS_11_HANDLER(px_battle_eject_button_use)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    const struct px_battle_items_api *items = api->items;
    if (get(0x02, api) != id) return;
    if (api->can_switch(user(api), context, id) == 0) return;
    uint32_t at = items->action_record(user(api), context, 3, id);
    items->message(user(api), at + 4, 2, 0x19e);
    items->message_arg(user(api), at + 4, id);
    items->commit_actions(user(api), context, at);
    at = items->action_record(user(api), context, 0x2a, id);
    m->store8(m->user, at + 0x4c, (uint8_t)id);
    items->commit_actions(user(api), context, at);
}

static int can_rise(uint32_t context, uint32_t id, uint32_t stat, const struct px_battle_items_11_api *api)
{
    return api->stage_can_change(user(api), px_battle_pokemon_by_id(context, id, base(api)), stat, 1) != 0;
}

/* Kee and Maranga: a hit of their category when the stat can still rise. */
static void category_berry_check(uint32_t handler, uint32_t context, uint32_t id, uint32_t category, uint32_t stat,
                                 const struct px_battle_items_11_api *api)
{
    if (!was_hit(id, api)) return;
    if (get(0x1b, api) != category) return;
    if (!can_rise(context, id, stat, api)) return;
    request_use(handler, context, id, api);
}

/* Kind 0xd: the stat +1 (+0x16 1, +4 the stat, +0x17 1, +0xc the holder). */
static void stat_berry_use(uint32_t context, uint32_t id, uint32_t stat, const struct px_battle_items_11_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    if (!can_rise(context, id, stat, api)) return;
    const uint32_t at = api->items->action_record(user(api), context, 0xd, id);
    m->store8(m->user, at + 0x16, 1);
    m->store8(m->user, at + 4, (uint8_t)stat);
    m->store8(m->user, at + 0x17, 1);
    m->store8(m->user, at + 0xc, (uint8_t)id);
    api->items->commit_actions(user(api), context, at);
}

PX_BATTLE_ITEMS_11_HANDLER(px_battle_kee_check) { (void)work; category_berry_check(handler, context, id, 1, 2, api); }
PX_BATTLE_ITEMS_11_HANDLER(px_battle_maranga_check) { (void)work; category_berry_check(handler, context, id, 2, 4, api); }
PX_BATTLE_ITEMS_11_HANDLER(px_battle_kee_use) { (void)handler, (void)work; stat_berry_use(context, id, 2, api); }
PX_BATTLE_ITEMS_11_HANDLER(px_battle_maranga_use) { (void)handler, (void)work; stat_berry_use(context, id, 4, api); }
