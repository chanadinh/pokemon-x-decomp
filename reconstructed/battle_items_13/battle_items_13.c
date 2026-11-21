#include "battle_items_13.h"

enum { HANDLER_ITEM = 0x38 };

static const struct px_battle_ability_api *base(const struct px_battle_items_13_api *api)
{
    return api->items->base;
}

static void *user(const struct px_battle_items_13_api *api)
{
    return base(api)->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_items_13_api *api)
{
    return base(api)->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_items_13_api *api)
{
    return px_battle_event_get(key, base(api)->events);
}

static uint32_t rewrite(uint32_t key, uint32_t value, const struct px_battle_items_13_api *api)
{
    return px_battle_event_rewrite(key, value, base(api)->events);
}

/* Amulet Coin and Luck Incense: 1084c3cc(context, id, 0), the routine Happy
 * Hour also calls. */
PX_BATTLE_ITEMS_13_HANDLER(px_battle_amulet_coin_5e)
{
    (void)handler, (void)work;
    if (get(0x02, api) != id) return;
    api->prize_money(user(api), context, id, 0);
}

/* 1084c3bc moves its arguments into place and falls through into 1084c3cc;
 * there is no battler test. */
PX_BATTLE_ITEMS_13_HANDLER(px_battle_amulet_coin_61)
{
    (void)handler, (void)work;
    api->prize_money(user(api), context, id, 0);
}

/* Smoke Ball: when key 0x02's low byte and the holder are allies, key 0x53
 * (escape) = 1. */
static uint32_t ally_escape(uint32_t id, const struct px_battle_items_13_api *api)
{
    if (api->is_ally(user(api), get(0x02, api) & UINT32_C(0xff), id) == 0) return 0;
    return rewrite(0x53, 1, api);
}

PX_BATTLE_ITEMS_13_HANDLER(px_battle_smoke_ball_0b)
{
    (void)handler, (void)context, (void)work;
    (void)ally_escape(id, api);
}

/* Event 0x0d: once the rewrite took, kind 3 with message 0x3f2 (literal
 * 108a86a0) naming the holder and the item, and the extra value 0xd0024
 * (literal 108a86a4). */
PX_BATTLE_ITEMS_13_HANDLER(px_battle_smoke_ball_0d)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    const struct px_battle_items_api *items = api->items;
    if (ally_escape(id, api) == 0) return;
    const uint32_t at = items->action_record(user(api), context, 3, id);
    items->message(user(api), at + 4, 2, 0x3f2);
    items->message_arg(user(api), at + 4, id);
    items->message_arg(user(api), at + 4, m->load16(m->user, handler + HANDLER_ITEM));
    api->message_extra(user(api), at + 4, 0xd0024);
    items->commit_actions(user(api), context, at);
}

/* Destiny Knot: when the condition given to the holder (key 0x1e) is 7
 * (infatuation), the attacker gets it too, unless key 0x03's low byte is
 * 0x1f. Kind 0xb for the holder:
 * +4 halfword 7, +8 10876e20(holder), +0xc 1, +0xd 1, +0xf the attacker;
 * message 0x14a (literal 108a5d5c) at +0x14 naming the attacker and the
 * item. */
PX_BATTLE_ITEMS_13_HANDLER(px_battle_destiny_knot)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    const struct px_battle_items_api *items = api->items;
    if (get(0x04, api) != id) return;
    if (get(0x1e, api) != 7) return;
    const uint32_t attacker = get(0x03, api) & UINT32_C(0xff);
    if (attacker == 0x1f) return;
    const uint32_t at = items->action_record(user(api), context, 0xb, id);
    m->store8(m->user, at + 0xf, (uint8_t)attacker);
    m->store16(m->user, at + 4, 7);
    m->store32(m->user, at + 8, api->infatuation_condition(user(api), id));
    m->store8(m->user, at + 0xc, 1);
    m->store8(m->user, at + 0xd, 1);
    items->message(user(api), at + 0x14, 2, 0x14a);
    items->message_arg(user(api), at + 0x14, attacker);
    items->message_arg(user(api), at + 0x14, m->load16(m->user, handler + HANDLER_ITEM));
    items->commit_actions(user(api), context, at);
}

/* Float Stone: the holder's weight (key 0x36 multiplier) x0.5. */
PX_BATTLE_ITEMS_13_HANDLER(px_battle_float_stone)
{
    (void)handler, (void)context, (void)work;
    if (get(0x02, api) == id) px_battle_event_multiply(0x36, 0x800, base(api)->events);
}

/* Ring Target: unless the pass does not consume items, the holder hit loses
 * its immunity (key 0x4c = 1). The pass is tested first. */
PX_BATTLE_ITEMS_13_HANDLER(px_battle_ring_target)
{
    (void)handler, (void)work;
    if (api->no_consume(user(api), context) != 0) return;
    if (get(0x04, api) == id) rewrite(0x4c, 1, api);
}

/* Safety Goggles: a positive key 0x33 (signed) sets key 0x42 = 1. */
PX_BATTLE_ITEMS_13_HANDLER(px_battle_safety_goggles)
{
    (void)handler, (void)context, (void)work;
    if (get(0x02, api) != id) return;
    if ((int32_t)get(0x33, api) <= 0) return;
    rewrite(0x42, 1, api);
}
