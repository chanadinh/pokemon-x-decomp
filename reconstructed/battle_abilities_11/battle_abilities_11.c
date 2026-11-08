#include "battle_abilities_11.h"

static void *user(const struct px_battle_abilities_11_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_abilities_11_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_abilities_11_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t load32(uint32_t address, const struct px_battle_abilities_11_api *api)
{
    return memory(api)->load32(memory(api)->user, address);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_abilities_11_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static uint32_t full_hp(uint32_t target, const struct px_battle_abilities_11_api *api)
{
    const struct px_battle_abilities_6_api shared = {.base = api->base};
    return px_battle_full_hp(target, &shared);
}

/* The effect ends: clear its field entry and remove the handler. */
static void finish(uint32_t handler, uint32_t context, uint32_t position, const struct px_battle_abilities_11_api *api)
{
    api->field_clear(user(api), context, memory(api)->load16(memory(api)->user, handler + 0x38) & 0xff, position);
    api->remove_handler(user(api), handler);
}

PX_BATTLE_ABILITIES_11_HANDLER(px_battle_field_wish)
{
    const struct px_pokemon_api *m = memory(api);
    /* Work: +0 started, +4 the turn it was made, +0x18 the battler who made it. */
    if (get(0x02, api) != 0x1f || api->battle_decided(user(api), context) != 0) return;
    if (load32(work, api) == 0) {
        m->store32(m->user, work + 4, api->turn(user(api), context));
        m->store32(m->user, work, 1);
        return;
    }
    if (load32(work + 4, api) >= api->turn(user(api), context)) return;
    /* A turn later the battler at the position, unless empty or at full HP,
     * regains half of the maker's max HP (message 0x2bc naming the maker). */
    const uint32_t target = api->battler_at(user(api), context, position);
    if (target != 0x1f && !full_hp(pokemon(context, target, api), api)) {
        const uint32_t maker = load32(work + 0x18, api) & 0xff;
        const uint32_t source = pokemon(context, maker, api);
        const struct px_battle_abilities_1_api shared = {.base = api->base};
        const uint32_t record = api->action_record(user(api), context, 4, maker);
        m->store8(m->user, record + 6, (uint8_t)target);
        m->store16(m->user, record + 4, (uint16_t)px_battle_hp_share(source, 2, &shared));
        api->message(user(api), record + 8, 2, 0x2bc);
        api->message_arg(user(api), record + 8, maker);
        api->commit_actions(user(api), context, record);
    }
    finish(handler, context, position, api);
}

PX_BATTLE_ABILITIES_11_HANDLER(px_battle_field_entry)
{
    const struct px_pokemon_api *m = memory(api);
    /* The battler entering the position: kind 0x2b (+4 work byte, +5 the battler) for no battler (0x1f). */
    const uint32_t entering = get(0x02, api) & 0xff;
    if (api->battler_at(user(api), context, position) != entering) return;
    const uint32_t record = api->action_record(user(api), context, 0x2b, 0x1f);
    m->store8(m->user, record + 4, (uint8_t)load32(work, api));
    m->store8(m->user, record + 5, (uint8_t)entering);
    api->commit_actions(user(api), context, record);
    finish(handler, context, position, api);
}

PX_BATTLE_ABILITIES_11_HANDLER(px_battle_field_future_sight)
{
    const struct px_pokemon_api *m = memory(api);
    /* Work: +0 the turn it strikes (signed), +4 the move, +8 a byte, +0x18 the user. */
    if (get(0x02, api) != 0x1f || api->battle_decided(user(api), context) != 0) return;
    if ((int32_t)load32(work, api) > (int32_t)(api->turn(user(api), context) & 0xff)) return;
    const uint32_t target = api->battler_at(user(api), context, position);
    const uint32_t attacker = load32(work + 0x18, api) & 0xff;
    if (target != 0x1f && target != attacker) {
        const struct px_battle_abilities_1_api shared = {.base = api->base};
        if (!px_battle_fainted(pokemon(context, target, api), &shared)) {
            /* The announcement (0x43e, literal 108a5710), then kind 0x28: the attack itself. */
            uint32_t record = api->action_record(user(api), context, 3, attacker);
            api->message(user(api), record + 4, 2, 0x43e);
            api->message_arg(user(api), record + 4, target);
            api->message_arg(user(api), record + 4, load32(work + 4, api));
            api->commit_actions(user(api), context, record);
            record = api->action_record(user(api), context, 0x28, attacker);
            m->store8(m->user, record + 4, (uint8_t)attacker);
            m->store8(m->user, record + 5, (uint8_t)target);
            m->store8(m->user, record + 6, (uint8_t)load32(work + 8, api));
            m->store16(m->user, record + 8, (uint16_t)load32(work + 4, api));
            api->commit_actions(user(api), context, record);
        }
    }
    finish(handler, context, position, api);
}

/* Kind 0x38: the wish comes true for the battler entering the position (+4 message base, +6 position, +7 10). */
static uint32_t wish_record(uint32_t context, uint32_t position, uint32_t entering, uint32_t base,
                            const struct px_battle_abilities_11_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t record = api->action_record(user(api), context, 0x38, entering);
    m->store16(m->user, record + 4, (uint16_t)base);
    m->store8(m->user, record + 6, (uint8_t)position);
    m->store8(m->user, record + 7, 0xa);
    return record;
}

static void announce(uint32_t context, uint32_t entering, uint32_t record, uint32_t message, const struct px_battle_abilities_11_api *api)
{
    api->message(user(api), record + 0xc, 2, message);
    api->message_arg(user(api), record + 0xc, entering);
    api->commit_actions(user(api), context, record);
}

/* Full HP restored, then every status cured (kind 0xa, +4 0x26). */
static void restore(uint32_t context, uint32_t entering, uint32_t self, const struct px_battle_abilities_11_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    if (!full_hp(self, api)) {
        const uint32_t record = api->action_record(user(api), context, 4, entering);
        m->store8(m->user, record + 6, (uint8_t)entering);
        const uint32_t max = api->base->pokemon_value(user(api), self, 0xe);
        m->store16(m->user, record + 4, (uint16_t)(max - api->base->pokemon_value(user(api), self, 0xd)));
        api->commit_actions(user(api), context, record);
    }
    if (api->base->pokemon_status(user(api), self) != 0) {
        const uint32_t record = api->action_record(user(api), context, 0xa, entering);
        m->store8(m->user, record + 0x11, 1);
        m->store8(m->user, record + 5, (uint8_t)entering);
        m->store8(m->user, record + 4, 0x26);
        api->commit_actions(user(api), context, record);
    }
}

PX_BATTLE_ABILITIES_11_HANDLER(px_battle_field_healing_wish)
{
    (void)work;
    const uint32_t entering = get(0x02, api) & 0xff;
    if (api->base->side_of(user(api), context, entering) != position) return;
    const uint32_t self = pokemon(context, entering, api);
    const uint32_t hurt = !full_hp(self, api);
    if (api->base->pokemon_status(user(api), self) != 0 || hurt) {
        /* Message 0x2b9: the base 0x2bb (literal 108a588c) without bit 1. */
        const uint32_t record = wish_record(context, position, entering, 0x2bb, api);
        announce(context, entering, record, 0x2bb & ~(0xa >> 2), api);
        restore(context, entering, self, api);
    }
    finish(handler, context, position, api);
}

PX_BATTLE_ABILITIES_11_HANDLER(px_battle_field_lunar_dance)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    const uint32_t entering = get(0x02, api) & 0xff;
    if (api->base->side_of(user(api), context, entering) != position) return;
    const uint32_t self = pokemon(context, entering, api);
    /* Needed when hurt, with a status, or with a move short of PP. */
    uint32_t needed = !full_hp(self, api);
    if (api->base->pokemon_status(user(api), self) != 0) needed = 1;
    const uint32_t moves = m->load8(m->user, self + 0x152);
    for (uint32_t i = 0; i < moves; ++i)
        if (api->pp_missing(user(api), self, i & 0xff) != 0) needed = 1;
    if (needed) {
        const uint32_t record = wish_record(context, position, entering, 0x2bc, api);
        m->store8(m->user, record + 0xb, 1);
        announce(context, entering, record, 0xa + 0x2ac, api);
        restore(context, entering, self, api);
        /* Kind 8: slot +6 of battler +5 regains +4 PP (the move count is read again). */
        const uint32_t slots = m->load8(m->user, self + 0x152);
        for (uint32_t i = 0; i < slots; ++i) {
            const uint32_t missing = api->pp_missing(user(api), self, i & 0xff);
            if (missing == 0) continue;
            const uint32_t record8 = api->action_record(user(api), context, 8, entering);
            m->store8(m->user, record8 + 5, (uint8_t)entering);
            m->store8(m->user, record8 + 6, (uint8_t)i);
            m->store8(m->user, record8 + 4, (uint8_t)missing);
            api->commit_actions(user(api), context, record8);
        }
    }
    finish(handler, context, position, api);
}
