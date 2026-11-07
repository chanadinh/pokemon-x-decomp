#include "battle_abilities_1.h"

static void *user(const struct px_battle_abilities_1_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_abilities_1_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_abilities_1_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t value(uint32_t pokemon, uint32_t key, const struct px_battle_abilities_1_api *api)
{
    return api->base->pokemon_value(user(api), pokemon, key);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_abilities_1_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

/* The move of the event makes contact (move flag 0). */
static uint32_t contact(const struct px_battle_abilities_1_api *api)
{
    return api->move_flag(user(api), get(0x12, api) & UINT32_C(0xffff), 0) != 0;
}

/* This battler was hit and key 0x47 does not suppress the after-effects. */
static uint32_t hit(uint32_t id, const struct px_battle_abilities_1_api *api)
{
    return get(0x04, api) == id && get(0x47, api) == 0;
}

static void mark(uint32_t record, const struct px_battle_abilities_1_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store32(m->user, record, m->load32(m->user, record) | UINT32_C(0x800000));
}

uint32_t px_battle_fainted(uint32_t pokemon, const struct px_battle_abilities_1_api *api)
{
    return value(pokemon, 0xd, api) == 0;
}

uint32_t px_battle_hp_share(uint32_t pokemon, uint32_t divisor, const struct px_battle_abilities_1_api *api)
{
    /* The unsigned divide veneer 1080041c is represented by C division. */
    const uint32_t share = value(pokemon, 0xe, api) / divisor;
    return share == 0 ? 1 : share;
}

void px_battle_contact_status(uint32_t context, uint32_t id, uint32_t status, uint32_t condition, uint32_t percent,
                              const struct px_battle_abilities_1_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    if (!hit(id, api) || !contact(api)) return;
    if (api->chance(user(api), percent) == 0 && api->forced(user(api), context, 1) == 0) return;
    /* Kind 0xb: +4 status, +8 condition, +0xf the battler receiving it. */
    const uint32_t record = api->action_record(user(api), context, 0xb, id);
    m->store16(m->user, record + 4, (uint16_t)status);
    m->store32(m->user, record + 8, condition);
    m->store8(m->user, record + 0xc, 0);
    mark(record, api);
    m->store8(m->user, record + 0xf, (uint8_t)get(0x03, api));
    api->commit_actions(user(api), context, record);
}

#define CONTACT_STATUS(name, status) \
    PX_BATTLE_ABILITIES_1_HANDLER(name) \
    { \
        (void)handler, (void)work; \
        px_battle_contact_status(context, id, status, api->condition(user(api), status), 0x1e, api); \
    }
CONTACT_STATUS(px_battle_static, 1)
CONTACT_STATUS(px_battle_poison_point, 5)
CONTACT_STATUS(px_battle_flame_body, 4)

PX_BATTLE_ABILITIES_1_HANDLER(px_battle_effect_spore)
{
    (void)handler, (void)work;
    if (!hit(id, api)) return;
    const uint32_t attacker = get(0x03, api) & UINT32_C(0xff);
    const uint32_t target = pokemon(context, attacker, api);
    /* Grass types, Overcoat (142) and Safety Goggles (650) are immune. */
    if (api->has_type(user(api), target, 0xb) != 0) return;
    if (value(target, 0x11, api) == 0x8e) return;
    if (api->held_item(user(api), context, attacker) == 0x28a) return;
    const uint32_t roll = api->random(user(api), 0x1e);
    const uint32_t status = roll > 20 ? 5 : roll > 10 ? 1 : 2;
    px_battle_contact_status(context, id, status, api->condition(user(api), status), 0x1e, api);
}

PX_BATTLE_ABILITIES_1_HANDLER(px_battle_cute_charm)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!hit(id, api) || api->battle_decided(user(api), context) != 0) return;
    const uint32_t self = pokemon(context, id, api);
    const uint32_t attacker = pokemon(context, get(0x03, api) & UINT32_C(0xff), api);
    const uint32_t mine = value(self, 0x12, api) & UINT32_C(0xff);
    const uint32_t theirs = value(attacker, 0x12, api) & UINT32_C(0xff);
    /* Opposite genders only; 2 is genderless. */
    if (mine == 2 || theirs == 2 || mine == theirs) return;
    api->condition_for(user(api), 7, self, api->condition_scratch);
    px_battle_contact_status(context, id, 7, m->load32(m->user, api->condition_scratch), 0x1e, api);
}

PX_BATTLE_ABILITIES_1_HANDLER(px_battle_poison_touch)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x03, api) != id || get(0x47, api) != 0 || get(0x48, api) != 0 || !contact(api)) return;
    if (api->chance(user(api), 0x1e) == 0 && api->forced(user(api), context, 1) == 0) return;
    const uint32_t record = api->action_record(user(api), context, 0xb, id);
    mark(record, api);
    m->store8(m->user, record + 0xf, (uint8_t)get(0x04, api));
    m->store16(m->user, record + 4, 5);
    m->store32(m->user, record + 8, api->condition(user(api), 5));
    api->message(user(api), record + 0x14, 2, 0x1d8);
    api->message_arg(user(api), record + 0x14, m->load8(m->user, record + 0xf));
    api->commit_actions(user(api), context, record);
}

/* Kind 6 damages +6 by +4 HP; +7 bit 0 when the battler need not be alive. */
static void recoil(uint32_t context, uint32_t id, uint32_t attacker, uint32_t target, uint32_t divisor,
                   uint32_t message, uint32_t dead, const struct px_battle_abilities_1_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t record = api->action_record(user(api), context, 6, id);
    m->store8(m->user, record + 6, (uint8_t)attacker);
    mark(record, api);
    m->store16(m->user, record + 4, (uint16_t)px_battle_hp_share(target, divisor, api));
    if (dead) m->store8(m->user, record + 7, m->load8(m->user, record + 7) | 1);
    api->message(user(api), record + 0xc, 2, message);
    api->message_arg(user(api), record + 0xc, attacker);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_1_HANDLER(px_battle_rough_skin)
{
    (void)handler, (void)work;
    if (!hit(id, api) || !contact(api)) return;
    const uint32_t attacker = get(0x03, api) & UINT32_C(0xff);
    const uint32_t target = pokemon(context, attacker, api);
    if (px_battle_fainted(target, api)) return;
    recoil(context, id, attacker, target, 8, 0x1ae, 0, api);  /* literal 108be9cc */
}

PX_BATTLE_ABILITIES_1_HANDLER(px_battle_aftermath)
{
    (void)handler, (void)work;
    if (get(0x04, api) != id || !px_battle_fainted(pokemon(context, id, api), api) || !contact(api)) return;
    const uint32_t attacker = get(0x03, api) & UINT32_C(0xff);
    recoil(context, id, attacker, pokemon(context, attacker, api), 4, 0x192, 1, api);  /* literal 108bdf74 */
}

PX_BATTLE_ABILITIES_1_HANDLER(px_battle_mummy)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!hit(id, api) || api->battle_decided(user(api), context) != 0 || !contact(api)) return;
    const uint32_t attacker = get(0x03, api) & UINT32_C(0xff);
    if (value(pokemon(context, attacker, api), 0x10, api) == 0x98) return;
    /* Kind 0x20: battler +6 takes ability +4 (Mummy, 152). */
    const uint32_t record = api->action_record(user(api), context, 0x20, id);
    m->store8(m->user, record + 6, (uint8_t)attacker);
    m->store16(m->user, record + 4, 0x98);
    api->message(user(api), record + 0xc, 2, 0x1cf);  /* literal 108bca98 */
    api->message_arg(user(api), record + 0xc, m->load8(m->user, record + 6));
    if (api->is_ally(user(api), id, attacker) == 0) mark(record, api);
    api->commit_actions(user(api), context, record);
}

/* Kind 0xd: one battler +0xc, stat +4, stages +0x17, flags +0x18. */
static uint32_t stage_record(uint32_t context, uint32_t id, uint32_t target, uint32_t stat, uint32_t stages,
                             uint32_t flags, const struct px_battle_abilities_1_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t record = api->action_record(user(api), context, 0xd, id);
    m->store8(m->user, record + 0x16, 1);
    m->store8(m->user, record + 4, (uint8_t)stat);
    m->store8(m->user, record + 0x17, (uint8_t)stages);
    if (flags) m->store8(m->user, record + 0x18, m->load8(m->user, record + 0x18) | (uint8_t)flags);
    m->store8(m->user, record + 0xc, (uint8_t)target);
    return record;
}

PX_BATTLE_ABILITIES_1_HANDLER(px_battle_gooey)
{
    (void)handler, (void)work;
    if (!hit(id, api) || !contact(api)) return;
    const uint32_t attacker = get(0x03, api) & UINT32_C(0xff);
    if (px_battle_fainted(pokemon(context, attacker, api), api)) return;
    const uint32_t record = stage_record(context, id, attacker, 5, 0xff, 0x12, api);
    mark(record, api);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_1_HANDLER(px_battle_rattled)
{
    (void)handler, (void)work;
    if (!hit(id, api)) return;
    /* Dark (16), Bug (6) or Ghost (7): Speed +1. */
    const uint32_t type = get(0x16, api) & UINT32_C(0xff);
    if (type != 0x10 && type != 6 && type != 7) return;
    const uint32_t record = stage_record(context, id, id, 5, 1, 0, api);
    mark(record, api);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_1_HANDLER(px_battle_justified)
{
    (void)handler, (void)work;
    /* The whole type word is compared with Dark (16): Attack +1. */
    if (!hit(id, api) || get(0x16, api) != 0x10) return;
    const uint32_t record = stage_record(context, id, id, 1, 1, 0, api);
    mark(record, api);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_1_HANDLER(px_battle_anger_point)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!hit(id, api) || get(0x46, api) == 0) return;
    const uint32_t self = pokemon(context, id, api);
    if (api->stage_room(user(api), self, 1) <= 0) return;
    /* A critical hit maximises Attack: the stages left are asked again after the record. */
    const uint32_t record = api->action_record(user(api), context, 0xd, id);
    m->store8(m->user, record + 4, 1);
    const uint32_t room = (uint32_t)api->stage_room(user(api), self, 1);
    m->store8(m->user, record + 0xc, (uint8_t)id);
    m->store8(m->user, record + 0x17, (uint8_t)room);
    m->store8(m->user, record + 0x18, m->load8(m->user, record + 0x18) | 3);
    m->store8(m->user, record + 0x16, 1);
    mark(record, api);
    api->message(user(api), record + 0x1c, 2, 0x1e1);
    api->message_arg(user(api), record + 0x1c, id);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_1_HANDLER(px_battle_weak_armor)
{
    (void)handler, (void)work;
    if (get(0x04, api) != id || get(0x1b, api) != 1 || get(0x47, api) != 0) return;
    const uint32_t self = pokemon(context, id, api);
    if (api->stage_can_change(user(api), self, 2, -1) == 0 && api->stage_can_change(user(api), self, 5, 1) == 0) return;
    if (px_battle_fainted(self, api)) return;
    /* A physical hit: Defense -1 and Speed +1, two unflagged records inside the popup. */
    api->ability_popup(user(api), context, 1, id);
    api->commit_actions(user(api), context, stage_record(context, id, id, 2, 0xff, 2, api));
    api->commit_actions(user(api), context, stage_record(context, id, id, 5, 1, 2, api));
    api->ability_popup(user(api), context, 2, id);
}
