#include "battle_items_8.h"

enum { HANDLER_ITEM = 0x38 };

static const struct px_battle_ability_api *base(const struct px_battle_items_8_api *api)
{
    return api->items->base;
}

static void *user(const struct px_battle_items_8_api *api)
{
    return base(api)->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_items_8_api *api)
{
    return base(api)->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_items_8_api *api)
{
    return px_battle_event_get(key, base(api)->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_items_8_api *api)
{
    return px_battle_pokemon_by_id(context, id, base(api));
}

static uint32_t item(uint32_t handler, const struct px_battle_items_8_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load16(m->user, handler + HANDLER_ITEM);
}

/* The item's parameter 3, low byte: a max-HP divisor here. */
static uint32_t divisor(uint32_t handler, const struct px_battle_items_8_api *api)
{
    return api->items->item_param(user(api), item(handler, api), 3) & UINT32_C(0xff);
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_items_8_api *api)
{
    return api->items->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t at, const struct px_battle_items_8_api *api)
{
    api->items->commit_actions(user(api), context, at);
}

/* Key 0x04 is the holder and key 0x47 is clear. */
static int hit_on(uint32_t id, const struct px_battle_items_8_api *api)
{
    return get(0x04, api) == id && get(0x47, api) == 0;
}

static void request_use(uint32_t handler, uint32_t context, uint32_t id, const struct px_battle_items_8_api *api)
{
    px_battle_handler_mark_reentry(handler, base(api)->events);
    api->items->ability_popup(user(api), context, 0, id);
}

/* Weakness Policy, Jaboca and Rowap: mark, keep the attacker + 1 in work
 * word 0, and commit a kind-0 record with +4 bit 1. */
static void remember_attacker(uint32_t handler, uint32_t context, uint32_t id, uint32_t work,
                              const struct px_battle_items_8_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    px_battle_handler_mark_reentry(handler, base(api)->events);
    m->store32(m->user, work, get(0x03, api) + 1);
    const uint32_t at = record(context, 0, id, api);
    m->store32(m->user, at + 4, m->load32(m->user, at + 4) | 2);
    commit(context, at, api);
}

/* Kind 0xd: the holder's stat by `stages`. */
static void raise(uint32_t context, uint32_t id, uint32_t stat, uint32_t stages, const struct px_battle_items_8_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t at = record(context, 0xd, id, api);
    m->store8(m->user, at + 0x16, 1);
    m->store8(m->user, at + 4, (uint8_t)stat);
    m->store8(m->user, at + 0x17, (uint8_t)stages);
    m->store8(m->user, at + 0xc, (uint8_t)id);
    commit(context, at, api);
}

/* A contact hit on the holder: the attacker loses max HP / parameter 3.
 * Kind 6 for the holder with +6 the attacker, message 0x1a8 at +0xc. */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_rocky_helmet)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!hit_on(id, api)) return;
    if (api->move_flag(user(api), get(0x12, api) & UINT32_C(0xffff), 0) == 0) return;
    const uint32_t at = record(context, 6, id, api);
    const uint32_t attacker = get(0x03, api) & UINT32_C(0xff);
    m->store8(m->user, at + 6, (uint8_t)attacker);
    const uint32_t p = pokemon(context, attacker, api);
    m->store16(m->user, at + 4, (uint16_t)px_battle_hp_share(p, divisor(handler, api), api->abilities));
    api->items->message(user(api), at + 0xc, 2, 0x1a8);
    api->items->message_arg(user(api), at + 0xc, m->load8(m->user, at + 6));
    commit(context, at, api);
}

/* Event 0x5e: unless gravity (field condition 2), a kind-3 message 0x198. */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_air_balloon_announce)
{
    (void)handler, (void)work;
    if (get(0x02, api) != id) return;
    if (api->field_active(user(api), context, 2) != 0) return;
    const uint32_t at = record(context, 3, id, api);
    api->items->message(user(api), at + 4, 2, 0x198);
    api->items->message_arg(user(api), at + 4, id);
    commit(context, at, api);
}

/* Event 0x12: the holder floats (key 0x53 = 1). */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_air_balloon_float)
{
    (void)handler, (void)context, (void)work;
    if (get(0x02, api) == id) px_battle_event_rewrite(0x53, 1, base(api)->events);
}

/* Event 0x53: any hit pops it. Kind 0x21: +4 halfword 0, +6 the holder,
 * +0xa 1, message 0x19b (literal 108ab2c8) at +0xc. */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_air_balloon_pop)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x04, api) != id) return;
    const uint32_t at = record(context, 0x21, id, api);
    m->store16(m->user, at + 4, 0);
    m->store8(m->user, at + 6, (uint8_t)id);
    m->store8(m->user, at + 0xa, 1);
    api->items->message(user(api), at + 0xc, 2, 0x19b);
    api->items->message_arg(user(api), at + 0xc, id);
    commit(context, at, api);
}

/* A hit of `type` on the holder whose `stat` can still rise asks for it. */
static void stat_item_check(uint32_t handler, uint32_t context, uint32_t id, uint32_t type, uint32_t stat,
                            const struct px_battle_items_8_api *api)
{
    if (!hit_on(id, api)) return;
    if (get(0x16, api) != type) return;
    if (api->stage_can_change(user(api), pokemon(context, id, api), stat, 1) == 0) return;
    request_use(handler, context, id, api);
}

PX_BATTLE_ITEMS_8_HANDLER(px_battle_absorb_bulb_check) { (void)work; stat_item_check(handler, context, id, 0xa, 3, api); }
PX_BATTLE_ITEMS_8_HANDLER(px_battle_cell_battery_check) { (void)work; stat_item_check(handler, context, id, 0xc, 1, api); }
PX_BATTLE_ITEMS_8_HANDLER(px_battle_snowball_check) { (void)work; stat_item_check(handler, context, id, 0xe, 1, api); }
PX_BATTLE_ITEMS_8_HANDLER(px_battle_luminous_moss_check) { (void)work; stat_item_check(handler, context, id, 0xa, 4, api); }

#define STAT_USE(name, stat) \
    PX_BATTLE_ITEMS_8_HANDLER(name) \
    { \
        (void)handler, (void)work; \
        if (get(0x02, api) == id) raise(context, id, stat, 1, api); \
    }
STAT_USE(px_battle_absorb_bulb_use, 3)
STAT_USE(px_battle_attack_raise_use, 1)
STAT_USE(px_battle_luminous_moss_use, 4)

/* A super-effective hit (key 0x39 above 7, signed). */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_weakness_policy_check)
{
    if (!hit_on(id, api)) return;
    if ((int32_t)get(0x39, api) <= 7) return;
    remember_attacker(handler, context, id, work, api);
}

PX_BATTLE_ITEMS_8_HANDLER(px_battle_weakness_policy_use)
{
    (void)handler, (void)work;
    if (get(0x02, api) != id) return;
    raise(context, id, 1, 2, api);
    raise(context, id, 3, 2, api);
}

/* The resist berries' event 0x53: once work word 0 is positive (the berry
 * weakened a hit, battle_modifier's 1084c980), it is cleared, the eater is
 * noted, and a kind-0x24 record shows message 0xdb naming the holder and the
 * berry. */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_resist_berry_eaten)
{
    const struct px_pokemon_api *m = memory(api);
    if ((int32_t)m->load32(m->user, work) <= 0) return;
    if (get(0x04, api) != id) return;
    m->store32(m->user, work, 0);
    api->note_berry(user(api), context, id);
    const uint32_t at = record(context, 0x24, id, api);
    m->store8(m->user, at + 4, 1);
    api->items->message(user(api), at + 8, 2, 0xdb);
    api->items->message_arg(user(api), at + 8, id);
    api->items->message_arg(user(api), at + 8, item(handler, api));
    commit(context, at, api);
}

/* Enigma Berry: a super-effective hit, unless condition 0xf. */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_enigma_check)
{
    (void)work;
    if (!hit_on(id, api)) return;
    if ((int32_t)get(0x39, api) <= 7) return;
    if (api->items->has_status(user(api), pokemon(context, id, api), 0xf) != 0) return;
    request_use(handler, context, id, api);
}

/* Kind 4: max HP / parameter 3, with no message. */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_enigma_use)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    const uint32_t p = pokemon(context, id, api);
    const uint32_t d = divisor(handler, api);
    const uint32_t at = record(context, 4, id, api);
    m->store8(m->user, at + 6, (uint8_t)id);
    m->store16(m->user, at + 4, (uint16_t)px_battle_hp_share(p, d, api->abilities));
    commit(context, at, api);
}

/* Jaboca and Rowap: a hit of their category by a move 1084c808 does not
 * exclude. */
static void category_berry_check(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, uint32_t category,
                                 const struct px_battle_items_8_api *api)
{
    if (!hit_on(id, api)) return;
    if (api->move_excluded(user(api), get(0x12, api) & UINT32_C(0xffff)) != 0) return;
    if (get(0x1b, api) != category) return;
    remember_attacker(handler, context, id, work, api);
}

PX_BATTLE_ITEMS_8_HANDLER(px_battle_jaboca_check) { category_berry_check(handler, context, id, work, 1, api); }
PX_BATTLE_ITEMS_8_HANDLER(px_battle_rowap_check) { category_berry_check(handler, context, id, work, 2, api); }

/* The remembered attacker, unless fainted, loses max HP / parameter 3: kind
 * 6 for the holder with +6 the attacker, message 0x40d (literal 108a7784) at
 * +0xc naming the attacker, the holder and the berry. */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_jaboca_rowap_use)
{
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    const uint32_t remembered = m->load32(m->user, work);
    if (remembered == 0) return;
    const uint32_t attacker = (remembered - 1) & UINT32_C(0xff);
    const uint32_t p = pokemon(context, attacker, api);
    if (px_battle_fainted(p, api->abilities) != 0) return;
    const uint32_t d = divisor(handler, api);
    const uint32_t at = record(context, 6, id, api);
    m->store8(m->user, at + 6, (uint8_t)attacker);
    m->store16(m->user, at + 4, (uint16_t)px_battle_hp_share(p, d, api->abilities));
    api->items->message(user(api), at + 0xc, 2, 0x40d);
    api->items->message_arg(user(api), at + 0xc, m->load8(m->user, at + 6));
    api->items->message_arg(user(api), at + 0xc, id);
    api->items->message_arg(user(api), at + 0xc, item(handler, api));
    commit(context, at, api);
}
