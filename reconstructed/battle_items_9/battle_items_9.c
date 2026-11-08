#include "battle_items_9.h"

enum { HANDLER_ITEM = 0x38 };

static const struct px_battle_ability_api *base(const struct px_battle_items_9_api *api)
{
    return api->items->base;
}

static void *user(const struct px_battle_items_9_api *api)
{
    return base(api)->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_items_9_api *api)
{
    return base(api)->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_items_9_api *api)
{
    return px_battle_event_get(key, base(api)->events);
}

static uint32_t rewrite(uint32_t key, uint32_t value, const struct px_battle_items_9_api *api)
{
    return px_battle_event_rewrite(key, value, base(api)->events);
}

static uint32_t item(uint32_t handler, const struct px_battle_items_9_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load16(m->user, handler + HANDLER_ITEM);
}

static uint32_t param3_low(uint32_t handler, const struct px_battle_items_9_api *api)
{
    return api->items->item_param(user(api), item(handler, api), 3) & UINT32_C(0xff);
}

/* The item's parameter-3 percent, or forced (flag 2). */
static int rolls(uint32_t handler, uint32_t context, const struct px_battle_items_9_api *api)
{
    if (api->chance(user(api), param3_low(handler, api)) != 0) return 1;
    return api->forced(user(api), context, 2) != 0;
}

static void request_use(uint32_t handler, uint32_t context, uint32_t id, const struct px_battle_items_9_api *api)
{
    px_battle_handler_mark_reentry(handler, base(api)->events);
    api->items->ability_popup(user(api), context, 0, id);
}

/* Moving first: key 0x11 = 2; when the rewrite takes, ask for the item. */
static void move_first(uint32_t handler, uint32_t context, uint32_t id, const struct px_battle_items_9_api *api)
{
    if (rewrite(0x11, 2, api) == 0) return;
    request_use(handler, context, id, api);
}

/* Kind 3 at +4 or kind 0x24 at +8: `text` naming the holder and the item. */
static void show(uint32_t handler, uint32_t context, uint32_t id, uint32_t kind, uint32_t at_offset, uint32_t text,
                 const struct px_battle_items_9_api *api)
{
    const uint32_t at = api->items->action_record(user(api), context, kind, id);
    api->items->message(user(api), at + at_offset, 2, text);
    api->items->message_arg(user(api), at + at_offset, id);
    api->items->message_arg(user(api), at + at_offset, item(handler, api));
    api->items->commit_actions(user(api), context, at);
}

/* Custap Berry: at a quarter of max HP or less (Gluttony halves it). */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_custap_check)
{
    (void)work;
    if (get(0x02, api) != id) return;
    if (px_battle_item_pinch(context, id, 4, api->items) == 0) return;
    move_first(handler, context, id, api);
}

PX_BATTLE_ITEMS_9_HANDLER(px_battle_quick_claw_check)
{
    (void)work;
    if (get(0x02, api) != id) return;
    if (!rolls(handler, context, api)) return;
    move_first(handler, context, id, api);
}

/* Event 0x7b: kind 3 with message 0x407 (literals 108a769c, 108a9acc), then
 * work word 0 is cleared. */
static void moved_first(uint32_t handler, uint32_t context, uint32_t id, uint32_t work,
                        const struct px_battle_items_9_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    show(handler, context, id, 3, 4, 0x407, api);
    m->store32(m->user, work, 0);
}

PX_BATTLE_ITEMS_9_HANDLER(px_battle_custap_use) { moved_first(handler, context, id, work, api); }
PX_BATTLE_ITEMS_9_HANDLER(px_battle_quick_claw_use) { moved_first(handler, context, id, work, api); }

/* Lagging Tail and Full Incense: key 0x11 = 0, whatever the rewrite says. */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_lagging_tail)
{
    (void)handler, (void)context, (void)work;
    if (get(0x02, api) == id) rewrite(0x11, 0, api);
}

/* King's Rock and Razor Fang: the attacker's flinch chance (key 0x27) is
 * parameter 3's low byte, unless key 0x26 is set. */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_kings_rock)
{
    (void)context, (void)work;
    if (get(0x03, api) != id) return;
    if (get(0x26, api) != 0) return;
    rewrite(0x27, param3_low(handler, api), api);
}

/* Event 0x7c: kind 0x2c, +4 the holder, +5 100. */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_kings_rock_use)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    const uint32_t at = api->items->action_record(user(api), context, 0x2c, id);
    m->store8(m->user, at + 4, (uint8_t)id);
    m->store8(m->user, at + 5, 100);
    api->items->commit_actions(user(api), context, at);
}

/* Event 0x7b: kind 3 with message 0x3f5 (literal 108ab894). */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_focus_band_use)
{
    (void)work;
    if (get(0x02, api) != id) return;
    show(handler, context, id, 3, 4, 0x3f5, api);
}

/* Event 0x7d: key 0x3b = 3 when the roll succeeds; work word 0 keeps
 * whether the rewrite took. */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_focus_band_endure)
{
    const struct px_pokemon_api *m = memory(api);
    if (get(0x04, api) != id) return;
    if (!rolls(handler, context, api)) return;
    m->store32(m->user, work, rewrite(0x3b, 3, api));
}

/* Event 0x7e: once it held on, ask for the item and clear the work word. */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_focus_band_show)
{
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    if (m->load32(m->user, work) == 0) return;
    request_use(handler, context, id, api);
    m->store32(m->user, work, 0);
}

PX_BATTLE_ITEMS_9_HANDLER(px_battle_focus_sash_endure)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x04, api) != id) return;
    if (api->full_hp(user(api), px_battle_pokemon_by_id(context, id, base(api))) == 0) return;
    m->store32(m->user, work, rewrite(0x3b, 3, api));
}

/* Event 0x7e: kind 0x24 with message 0x3f5 (literal 108a989c), then the
 * work word is cleared. */
PX_BATTLE_ITEMS_9_HANDLER(px_battle_focus_sash_show)
{
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    if (m->load32(m->user, work) == 0) return;
    show(handler, context, id, 0x24, 8, 0x3f5, api);
    m->store32(m->user, work, 0);
}
