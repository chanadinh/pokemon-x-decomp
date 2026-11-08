#include "battle_items_7.h"

enum { HANDLER_ITEM = 0x38, HELD_ITEM = 0x12 };

static const struct px_battle_ability_api *base(const struct px_battle_items_7_api *api)
{
    return api->items->base;
}

static void *user(const struct px_battle_items_7_api *api)
{
    return base(api)->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_items_7_api *api)
{
    return base(api)->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_items_7_api *api)
{
    return px_battle_event_get(key, base(api)->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_items_7_api *api)
{
    return px_battle_pokemon_by_id(context, id, base(api));
}

static uint32_t item(uint32_t handler, const struct px_battle_items_7_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load16(m->user, handler + HANDLER_ITEM);
}

static uint32_t param3(uint32_t handler, const struct px_battle_items_7_api *api)
{
    return api->items->item_param(user(api), item(handler, api), 3);
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_items_7_api *api)
{
    return api->items->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t at, const struct px_battle_items_7_api *api)
{
    api->items->commit_actions(user(api), context, at);
}

/* A message at `at` naming the battler and, when asked, the item. */
static void message(uint32_t at, uint32_t text, uint32_t id, uint32_t handler, int with_item,
                    const struct px_battle_items_7_api *api)
{
    api->items->message(user(api), at, 2, text);
    api->items->message_arg(user(api), at, id);
    if (with_item) api->items->message_arg(user(api), at, item(handler, api));
}

/* Kind 0xb: status for `target`, +0xf the holder; the condition word given. */
static uint32_t status_record(uint32_t context, uint32_t id, uint32_t target, uint32_t status,
                              const struct px_battle_items_7_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t at = record(context, 0xb, target, api);
    m->store8(m->user, at + 0xf, (uint8_t)id);
    m->store16(m->user, at + 4, (uint16_t)status);
    return at;
}

/* Event 0x80: the holder becomes badly poisoned (condition word 0x79),
 * message 0xf0 at +0x14. */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_toxic_orb)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    const uint32_t at = record(context, 0xb, id, api);
    m->store16(m->user, at + 4, 5);
    m->store32(m->user, at + 8, 0x79);
    m->store8(m->user, at + 0xf, (uint8_t)id);
    m->store8(m->user, at + 0xc, 0);
    message(at + 0x14, 0xf0, id, handler, 1, api);
    commit(context, at, api);
}

PX_BATTLE_ITEMS_7_HANDLER(px_battle_toxic_orb_use)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    const uint32_t at = status_record(context, id, get(0x03, api) & UINT32_C(0xff), 5, api);
    m->store32(m->user, at + 8, 0x79);
    commit(context, at, api);
}

/* Event 0x80: the holder is burned; +0x1c the item, message 0x102 (literal
 * 108a6248) at +0x14. */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_flame_orb)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    const uint32_t at = record(context, 0xb, id, api);
    m->store16(m->user, at + 4, 4);
    m->store32(m->user, at + 8, api->condition(user(api), 4));
    m->store8(m->user, at + 0xc, 0);
    m->store8(m->user, at + 0xf, (uint8_t)id);
    m->store32(m->user, at + 0x1c, item(handler, api));
    message(at + 0x14, 0x102, id, handler, 1, api);
    commit(context, at, api);
}

PX_BATTLE_ITEMS_7_HANDLER(px_battle_flame_orb_use)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    const uint32_t at = status_record(context, id, get(0x03, api) & UINT32_C(0xff), 4, api);
    m->store32(m->user, at + 8, api->condition(user(api), 4));
    commit(context, at, api);
}

/* Kind 6: the holder loses `amount`; +8 0x26f, +7 bit 1, +0xa its side,
 * +0xb 0xa, message 0x414 (literal) at +0xc naming it and the item. */
static void hp_loss(uint32_t handler, uint32_t context, uint32_t id, uint32_t p, uint32_t divisor_from_item,
                    uint32_t divisor, const struct px_battle_items_7_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t at = record(context, 6, id, api);
    m->store8(m->user, at + 6, (uint8_t)id);
    if (divisor_from_item) divisor = param3(handler, api);
    m->store16(m->user, at + 4, (uint16_t)px_battle_hp_share(p, divisor, api->abilities));
    m->store16(m->user, at + 8, 0x26f);
    m->store8(m->user, at + 7, m->load8(m->user, at + 7) | 2);
    m->store8(m->user, at + 0xa, (uint8_t)base(api)->side_of(user(api), context, id));
    m->store8(m->user, at + 0xb, 0xa);
    message(at + 0xc, 0x414, id, handler, 1, api);
    commit(context, at, api);
}

/* Kind 4: the holder restores `amount`, message 0x398 at +8. */
static void hp_restore(uint32_t handler, uint32_t context, uint32_t id, uint32_t p, uint32_t divisor_from_item,
                       uint32_t divisor, const struct px_battle_items_7_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t at = record(context, 4, id, api);
    m->store8(m->user, at + 6, (uint8_t)id);
    if (divisor_from_item) divisor = param3(handler, api);
    m->store16(m->user, at + 4, (uint16_t)px_battle_hp_share(p, divisor, api->abilities));
    message(at + 8, 0x398, id, handler, 1, api);
    commit(context, at, api);
}

PX_BATTLE_ITEMS_7_HANDLER(px_battle_sticky_barb)
{
    (void)work;
    if (get(0x02, api) != id) return;
    hp_loss(handler, context, id, pokemon(context, id, api), 1, 0, api);
}

/* Event 0x53: a contact hit on the holder by an attacker holding nothing.
 * Kind 0x25: +4 the attacker. */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_sticky_barb_move)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x04, api) != id) return;
    if (get(0x47, api) != 0) return;
    if (api->move_flag(user(api), get(0x12, api) & UINT32_C(0xffff), 0) == 0) return;
    const uint32_t attacker = get(0x03, api) & UINT32_C(0xff);
    if (m->load16(m->user, pokemon(context, attacker, api) + HELD_ITEM) != 0) return;
    const uint32_t at = record(context, 0x25, id, api);
    m->store8(m->user, at + 4, (uint8_t)attacker);
    commit(context, at, api);
}

/* Event 0x7f: below full HP, ask for the item (kind 0, +4 bit 0) and mark
 * the handler for re-entry. */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_leftovers_check)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    if (api->full_hp(user(api), pokemon(context, id, api)) != 0) return;
    const uint32_t at = record(context, 0, id, api);
    m->store32(m->user, at + 4, m->load32(m->user, at + 4) | 1);
    px_battle_handler_mark_reentry(handler, base(api)->events);
    commit(context, at, api);
}

PX_BATTLE_ITEMS_7_HANDLER(px_battle_leftovers)
{
    (void)work;
    if (get(0x02, api) != id) return;
    const uint32_t p = pokemon(context, id, api);
    if (api->full_hp(user(api), p) != 0) return;
    hp_restore(handler, context, id, p, 0, 0x10, api);
}

/* Event 0x7f: a Poison type (any of the three 5-bit types is 3) restores
 * max HP / parameter 3 (even at full HP); any other loses 1/8. */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_black_sludge)
{
    (void)work;
    if (get(0x02, api) != id) return;
    if (api->battle_decided(user(api), context) != 0) return;
    const uint32_t p = pokemon(context, id, api);
    const uint32_t types = api->packed_types(user(api), p);
    const int poison = (types & 0x1f) == 3 || (types >> 5 & 0x1f) == 3 || (types >> 10 & 0x1f) == 3;
    if (poison)
        hp_restore(handler, context, id, p, 1, 0, api);
    else
        hp_loss(handler, context, id, p, 0, 8, api);
}

/* Event 0x8e: the attacker restores its damage dealt (key 0x33) / parameter
 * 3, at least 1, unless key 0x4e is set. */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_shell_bell)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x03, api) != id) return;
    if (get(0x4e, api) != 0) return;
    const uint32_t damage = get(0x33, api);
    if (damage == 0) return;
    /* The unsigned divide veneer 1080041c is represented by C division. */
    uint32_t amount = damage / param3(handler, api);
    if (amount == 0) amount = 1;
    const uint32_t at = record(context, 4, id, api);
    m->store8(m->user, at + 6, (uint8_t)id);
    m->store16(m->user, at + 4, (uint16_t)amount);
    message(at + 8, 0x398, id, handler, 1, api);
    commit(context, at, api);
}

/* Event 0x8e: the attacker loses 1/10; kind 6 with only +4, +6 and message
 * 0x3fb (literal 108ab350) at +0xc naming the battler. */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_life_orb_recoil)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x03, api) != id) return;
    const uint32_t p = pokemon(context, id, api);
    const uint32_t at = record(context, 6, id, api);
    m->store8(m->user, at + 6, (uint8_t)id);
    m->store16(m->user, at + 4, (uint16_t)px_battle_hp_share(p, 0xa, api->abilities));
    message(at + 0xc, 0x3fb, id, handler, 0, api);
    commit(context, at, api);
}

/* Event 0x96: the holder's drain is multiplied (key 0x36) by the item factor. */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_big_root)
{
    (void)context, (void)work;
    if (get(0x03, api) != id) return;
    px_battle_event_multiply(0x36, px_battle_item_factor(handler, api->modifier), base(api)->events);
}
