#include "battle_abilities_8.h"

static void *user(const struct px_battle_abilities_8_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_abilities_8_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_abilities_8_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t rewrite(uint32_t key, uint32_t value, const struct px_battle_abilities_8_api *api)
{
    return px_battle_event_rewrite(key, value, api->base->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_abilities_8_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static uint32_t load16(uint32_t address, const struct px_battle_abilities_8_api *api)
{
    return memory(api)->load16(memory(api)->user, address);
}

static void mark(uint32_t record, const struct px_battle_abilities_8_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store32(m->user, record, m->load32(m->user, record) | UINT32_C(0x800000));
}

/* battle_abilities_1's helpers need only the battler lookup's base. */
static uint32_t fainted(uint32_t target, const struct px_battle_abilities_8_api *api)
{
    const struct px_battle_abilities_1_api shared = {.base = api->base};
    return px_battle_fainted(target, &shared);
}

/* This battler is among the targets (count key 0x05, targets from key 0x06). */
static uint32_t targeted(uint32_t id, const struct px_battle_abilities_8_api *api)
{
    const uint32_t count = get(0x05, api);
    for (uint32_t i = 0; i < count; ++i)
        if (get((i + 6) & UINT32_C(0xff), api) == id) return 1;
    return 0;
}

PX_BATTLE_ABILITIES_8_HANDLER(px_battle_sticky_hold_check)
{
    (void)handler, (void)context;
    const struct px_pokemon_api *m = memory(api);
    /* An item taken (key 0x2e clear) is kept; the work word holds the rewrite. */
    if (get(0x02, api) != id || get(0x2e, api) != 0) return;
    m->store32(m->user, work, rewrite(0x42, 1, api));
}

PX_BATTLE_ABILITIES_8_HANDLER(px_battle_sticky_hold_message)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id || m->load32(m->user, work) == 0) return;
    const uint32_t record = api->action_record(user(api), context, 3, id);
    mark(record, api);
    api->message(user(api), record + 4, 2, 0x1ed);  /* literal 108c3c38 */
    api->message_arg(user(api), record + 4, id);
    api->commit_actions(user(api), context, record);
    m->store32(m->user, work, 0);
}

/* Kind 0x25: the item of battler +4 moves to the record's battler. */
static uint32_t item_record(uint32_t context, uint32_t id, uint32_t from, uint32_t message, const struct px_battle_abilities_8_api *api)
{
    const uint32_t record = api->action_record(user(api), context, 0x25, id);
    memory(api)->store8(memory(api)->user, record + 4, (uint8_t)from);
    mark(record, api);
    api->message(user(api), record + 8, 2, message);
    return record;
}

PX_BATTLE_ABILITIES_8_HANDLER(px_battle_pickpocket)
{
    (void)handler, (void)work;
    if (!targeted(id, api)) return;
    /* A contact move steals into an empty hand, unless stealing is blocked. */
    const uint32_t thief = get(0x03, api) & UINT32_C(0xff);
    if (api->steal_blocked(user(api), context, id, thief) != 0) return;
    if (api->move_flag(user(api), get(0x12, api) & UINT32_C(0xffff), 0) == 0) return;
    const uint32_t self = pokemon(context, id, api), attacker = pokemon(context, thief, api);
    if (load16(self + 0x12, api) != 0 || load16(attacker + 0x12, api) == 0) return;
    const uint32_t record = item_record(context, id, thief, 0x1cc, api);
    api->message_arg(user(api), record + 8, thief);
    api->message_arg(user(api), record + 8, load16(attacker + 0x12, api));
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_8_HANDLER(px_battle_magician_held)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    /* The work word notes whether the attacker holds an item before its move. */
    if (get(0x03, api) == id) m->store32(m->user, work, load16(pokemon(context, id, api) + 0x12, api) != 0);
}

PX_BATTLE_ABILITIES_8_HANDLER(px_battle_symbiosis)
{
    (void)handler, (void)work;
    const uint32_t holder = get(0x02, api) & UINT32_C(0xff);
    if (api->is_ally(user(api), id, holder) == 0) return;
    /* Passes its item to an ally that has just used up its own. */
    const uint32_t self = pokemon(context, id, api), ally = pokemon(context, holder, api);
    if (load16(self + 0x12, api) == 0 || load16(ally + 0x12, api) != 0) return;
    if (api->turn_flag(user(api), self, 0xf) != 0 || api->turn_flag(user(api), self, 0x11) != 0) return;
    const uint32_t item = load16(self + 0x12, api);
    if (api->item_locked(user(api), context, load16(ally + 0xc, api), item) != 0) return;
    if (api->item_locked(user(api), context, load16(self + 0xc, api), item) != 0) return;
    const uint32_t record = item_record(context, id, holder, 0x4c3, api);  /* literal 108be0c4 */
    api->message_arg(user(api), record + 8, id);
    api->message_arg(user(api), record + 8, item);
    api->message_arg(user(api), record + 8, holder);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_8_HANDLER(px_battle_cheek_pouch)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    /* Eating a berry (key 0x52 set, item key 0x2e) also heals a third. */
    if (get(0x02, api) != id || get(0x52, api) == 0) return;
    if (api->is_berry(user(api), get(0x2e, api) & UINT32_C(0xffff)) == 0) return;
    const uint32_t self = pokemon(context, id, api);
    const struct px_battle_abilities_1_api shared = {.base = api->base};
    const uint32_t record = api->action_record(user(api), context, 4, id);
    m->store8(m->user, record + 6, (uint8_t)id);
    mark(record, api);
    m->store16(m->user, record + 4, (uint16_t)px_battle_hp_share(self, 3, &shared));
    api->message(user(api), record + 8, 2, 0x183);  /* literal 108bf2fc */
    api->message_arg(user(api), record + 8, id);
    api->message_arg(user(api), record + 8, load16(handler + 0x38, api));  /* the handler's ability */
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_8_HANDLER(px_battle_moxie)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (api->battle_decided(user(api), context) != 0 || get(0x03, api) != id) return;
    /* Attack + one stage for each target that fainted. */
    uint32_t knocked = 0;
    const uint32_t count = get(0x05, api);
    for (uint32_t i = 0; i < count; ++i)
        if (fainted(pokemon(context, get((i + 6) & UINT32_C(0xff), api) & UINT32_C(0xff), api), api)) ++knocked;
    if (knocked == 0) return;
    const uint32_t record = api->action_record(user(api), context, 0xd, id);
    m->store8(m->user, record + 0xc, (uint8_t)id);
    m->store8(m->user, record + 0x16, 1);
    mark(record, api);
    m->store8(m->user, record + 4, 1);
    m->store8(m->user, record + 0x17, (uint8_t)knocked);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_8_HANDLER(px_battle_color_change)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!targeted(id, api)) return;
    if (api->battle_decided(user(api), context) != 0) return;
    const uint32_t self = pokemon(context, id, api);
    if (fainted(self, api)) return;
    /* Becomes the move's type (not typeless 18, nor a type it already has). */
    const uint32_t type = get(0x16, api) & UINT32_C(0xff);
    if (type == 0x12 || api->has_type(user(api), self, type) != 0) return;
    api->ability_popup(user(api), context, 1, id);
    /* Kind 0x13: battler +6 takes the packed types +4 (both types, flags 0x4800). */
    const uint32_t record = api->action_record(user(api), context, 0x13, id);
    m->store8(m->user, record + 6, (uint8_t)id);
    m->store16(m->user, record + 4, (uint16_t)((type & 0x1f) | (type & 0x1f) << 5 | 0x4800));
    api->commit_actions(user(api), context, record);
    api->ability_popup(user(api), context, 2, id);
}

PX_BATTLE_ABILITIES_8_HANDLER(px_battle_cursed_body)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x04, api) != id || get(0x47, api) != 0 || api->battle_decided(user(api), context) != 0) return;
    const uint32_t attacker = get(0x03, api) & UINT32_C(0xff);
    const uint32_t target = pokemon(context, attacker, api);
    /* Disables the attacker's last move (not already disabled, not Struggle 165), 30%. */
    if (api->has_status(user(api), target, 0xd) != 0) return;
    const uint32_t move = get(0x14, api) & UINT32_C(0xffff);
    if (move == 0 || move == 0xa5 || api->disable_blocked(user(api), move) != 0) return;
    if (api->chance(user(api), 0x1e) == 0 && api->forced(user(api), context, 1) == 0) return;
    /* Four turns, five when turn flag 1 is set. */
    const uint32_t turns = api->turn_flag(user(api), target, 1) != 0 ? 5 : 4;
    const uint32_t record = api->action_record(user(api), context, 0xb, id);
    m->store16(m->user, record + 4, 0xd);
    m->store32(m->user, record + 8, api->disable_condition(user(api), turns, move));
    m->store8(m->user, record + 0xf, (uint8_t)attacker);
    mark(record, api);
    api->commit_actions(user(api), context, record);
}
