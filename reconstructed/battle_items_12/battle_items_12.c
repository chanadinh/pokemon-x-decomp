#include "battle_items_12.h"

enum { HANDLER_ITEM = 0x38, MOVES = 0x11c, MOVE_STRIDE = 0xe, MOVE_COUNT = 0x152, LAST_MOVE = 0x162, NO_SLOT = 4 };

static const struct px_battle_ability_api *base(const struct px_battle_items_12_api *api)
{
    return api->items->base;
}

static void *user(const struct px_battle_items_12_api *api)
{
    return base(api)->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_items_12_api *api)
{
    return base(api)->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_items_12_api *api)
{
    return px_battle_event_get(key, base(api)->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_items_12_api *api)
{
    return px_battle_pokemon_by_id(context, id, base(api));
}

static uint32_t item(uint32_t handler, const struct px_battle_items_12_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load16(m->user, handler + HANDLER_ITEM);
}

uint32_t px_battle_last_move(uint32_t p, const struct px_pokemon_api *m)
{
    return m->load16(m->user, p + LAST_MOVE);
}

uint32_t px_battle_move_slot(uint32_t p, uint32_t move, const struct px_pokemon_api *m)
{
    if (move == 0) return NO_SLOT;
    for (uint32_t slot = 0; slot < 4; ++slot)
        if (m->load16(m->user, p + MOVES + slot * MOVE_STRIDE) == move) return slot;
    return NO_SLOT;
}

static void request_use(uint32_t handler, uint32_t context, uint32_t id, const struct px_battle_items_12_api *api)
{
    px_battle_handler_mark_reentry(handler, base(api)->events);
    api->items->ability_popup(user(api), context, 0, id);
}

/* The first slot below the move count with no PP; NO_SLOT when none (or when
 * that slot is 4). */
static uint32_t empty_slot(uint32_t p, const struct px_battle_items_12_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t count = m->load8(m->user, p + MOVE_COUNT);
    for (uint32_t i = 0; i < count; ++i)
        if (api->move_pp(user(api), p, i & UINT32_C(0xff)) == 0) return i & UINT32_C(0xff);
    return NO_SLOT;
}

/* The last move's slot when it has no PP left; NO_SLOT otherwise. */
static uint32_t last_move_empty(uint32_t p, const struct px_battle_items_12_api *api)
{
    const uint32_t slot = px_battle_move_slot(p, px_battle_last_move(p, memory(api)), memory(api));
    if (slot == NO_SLOT) return NO_SLOT;
    return api->move_pp(user(api), p, slot) == 0 ? slot : NO_SLOT;
}

uint32_t px_battle_leppa_use(uint32_t handler, uint32_t context, uint32_t id, uint32_t any,
                             const struct px_battle_items_12_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const struct px_battle_items_api *items = api->items;
    const uint32_t holder = pokemon(context, id, api);
    uint32_t slot = last_move_empty(pokemon(context, id, api), api);
    if (slot == NO_SLOT) slot = empty_slot(pokemon(context, id, api), api);
    if (slot == NO_SLOT) {
        if (!any) return 0;
        /* As when flung: the first slot 1084d184 accepts. */
        const uint32_t count = m->load8(m->user, holder + MOVE_COUNT);
        uint32_t i = 0;
        while (i < count && api->pp_restorable(user(api), holder, i & UINT32_C(0xff)) == 0) ++i;
        if (i >= count) return 0;
        slot = i & UINT32_C(0xff);
        if (slot == NO_SLOT) return 0;
    }
    /* Kind 8: +4 0xa (PP), +5 the holder, +6 the slot, +7 bit 0; message
     * 0x395 (literal 108a9fbc) naming the holder, the berry and the move. */
    const uint32_t at = items->action_record(user(api), context, 8, id);
    const uint8_t flags = m->load8(m->user, at + 7);
    m->store8(m->user, at + 5, (uint8_t)id);
    m->store8(m->user, at + 6, (uint8_t)slot);
    m->store8(m->user, at + 4, 0xa);
    m->store8(m->user, at + 7, (uint8_t)(flags | 1));
    items->message(user(api), at + 8, 2, 0x395);
    items->message_arg(user(api), at + 8, id);
    items->message_arg(user(api), at + 8, item(handler, api));
    items->message_arg(user(api), at + 8, api->slot_move(user(api), holder, slot));
    items->commit_actions(user(api), context, at);
    return 1;
}

/* Events 0x04 and 0x57: the last move has run out of PP. */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_leppa_check_04)
{
    (void)work;
    if (get(0x02, api) != id) return;
    if (last_move_empty(pokemon(context, id, api), api) == NO_SLOT) return;
    request_use(handler, context, id, api);
}

/* ... and on event 0x57 sets key 0x53. */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_leppa_check_57)
{
    (void)handler, (void)work;
    if (get(0x02, api) != id) return;
    if (last_move_empty(pokemon(context, id, api), api) == NO_SLOT) return;
    px_battle_event_rewrite(0x53, 1, base(api)->events);
}

/* Any slot without PP. */
static void if_any_empty(uint32_t handler, uint32_t context, uint32_t id, const struct px_battle_items_12_api *api)
{
    if (empty_slot(pokemon(context, id, api), api) == NO_SLOT) return;
    request_use(handler, context, id, api);
}

/* 1084c8b8 falls through into 1084c948 (the request's record). */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_leppa_check_5e)
{
    (void)work;
    if (get(0x02, api) != id) return;
    if_any_empty(handler, context, id, api);
}

PX_BATTLE_ITEMS_12_HANDLER(px_battle_leppa_check_61)
{
    (void)work;
    if_any_empty(handler, context, id, api);
}

PX_BATTLE_ITEMS_12_HANDLER(px_battle_leppa_check_9c)
{
    (void)work;
    if (get(0x02, api) != id) return;
    if (!(get(0x2f, api) & 2)) return;
    if_any_empty(handler, context, id, api);
}

PX_BATTLE_ITEMS_12_HANDLER(px_battle_leppa_eat)
{
    (void)work;
    if (get(0x02, api) != id) return;
    (void)px_battle_leppa_use(handler, context, id, 0, api);
}

/* Event 0x7c: bxeq lr when work word 6 is zero, then the use with `any`. */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_leppa_eat_7c)
{
    const struct px_pokemon_api *m = memory(api);
    if (m->load32(m->user, work + UINT32_C(0x18)) == 0) return;
    (void)px_battle_leppa_use(handler, context, id, 1, api);
}

/* Micle Berry: the pinch test with parameter 3's low byte as the divisor.
 * The checks look the holder up first and discard it. */
static int micle_pinch(uint32_t handler, uint32_t context, uint32_t id, const struct px_battle_items_12_api *api)
{
    const uint32_t divisor = api->items->item_param(user(api), item(handler, api), 3) & UINT32_C(0xff);
    return px_battle_item_pinch(context, id, divisor, api->items) != 0;
}

/* Event 0x02: unless key 0x0c is 6. */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_micle_check_02)
{
    (void)work;
    if (get(0x0c, api) == 6) return;
    if (get(0x02, api) != id) return;
    (void)pokemon(context, id, api);
    if (!micle_pinch(handler, context, id, api)) return;
    request_use(handler, context, id, api);
}

PX_BATTLE_ITEMS_12_HANDLER(px_battle_micle_check_5e)
{
    (void)work;
    if (get(0x02, api) != id) return;
    (void)pokemon(context, id, api);
    if (!micle_pinch(handler, context, id, api)) return;
    request_use(handler, context, id, api);
}

/* Event 0x61: no battler test. */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_micle_check_61)
{
    (void)work;
    (void)pokemon(context, id, api);
    if (!micle_pinch(handler, context, id, api)) return;
    request_use(handler, context, id, api);
}

/* Event 0x9c: the divisor is read before key 0x02 is tested again and the
 * use flags (key 0x2f bit 0) are checked. */
PX_BATTLE_ITEMS_12_HANDLER(px_battle_micle_check_9c)
{
    (void)work;
    if (get(0x02, api) != id) return;
    (void)pokemon(context, id, api);
    const uint32_t divisor = api->items->item_param(user(api), item(handler, api), 3) & UINT32_C(0xff);
    if (get(0x02, api) != id) return;
    if (!(get(0x2f, api) & 1)) return;
    if (px_battle_item_pinch(context, id, divisor, api->items) == 0) return;
    request_use(handler, context, id, api);
}

/* Kind 0xb: +4 halfword 0x22, +8 1084c3b0(0x78), +0xf the holder; message
 * 0x40a (literals 108a87b8, 108aa11c) at +0x14 naming the holder and the
 * berry. */
static void micle_effect(uint32_t handler, uint32_t context, uint32_t id, const struct px_battle_items_12_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const struct px_battle_items_api *items = api->items;
    if (get(0x02, api) != id) return;
    (void)pokemon(context, id, api);
    const uint32_t at = items->action_record(user(api), context, 0xb, id);
    m->store8(m->user, at + 0xf, (uint8_t)id);
    m->store16(m->user, at + 4, 0x22);
    m->store32(m->user, at + 8, api->condition_word(user(api), 0x78));
    items->message(user(api), at + 0x14, 2, 0x40a);
    items->message_arg(user(api), at + 0x14, id);
    items->message_arg(user(api), at + 0x14, item(handler, api));
    items->commit_actions(user(api), context, at);
}

PX_BATTLE_ITEMS_12_HANDLER(px_battle_micle_use) { (void)work; micle_effect(handler, context, id, api); }

PX_BATTLE_ITEMS_12_HANDLER(px_battle_micle_use_7c)
{
    const struct px_pokemon_api *m = memory(api);
    if (m->load32(m->user, work + UINT32_C(0x18)) == 0) return;
    micle_effect(handler, context, id, api);
}
