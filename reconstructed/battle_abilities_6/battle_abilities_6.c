#include "battle_abilities_6.h"

static void *user(const struct px_battle_abilities_6_api *api)
{
    return api->base->events->user;
}

static uint32_t get(uint32_t key, const struct px_battle_abilities_6_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static void rewrite(uint32_t key, uint32_t value, const struct px_battle_abilities_6_api *api)
{
    px_battle_event_rewrite(key, value, api->base->events);
}

void px_battle_cure_status(uint32_t context, uint32_t id, uint32_t status, const struct px_battle_abilities_6_api *api)
{
    const struct px_pokemon_api *m = api->base->events->memory;
    if (api->has_status(user(api), px_battle_pokemon_by_id(context, id, api->base), status) == 0) return;
    api->ability_popup(user(api), context, 1, id);
    /* Kind 0xa: battler +5 is cured of status +4 (+0x11 1); not flagged. */
    const uint32_t record = api->action_record(user(api), context, 0xa, id);
    m->store8(m->user, record + 5, (uint8_t)id);
    m->store8(m->user, record + 4, (uint8_t)status);
    m->store8(m->user, record + 0x11, 1);
    api->commit_actions(user(api), context, record);
    api->ability_popup(user(api), context, 2, id);
}

uint32_t px_battle_secondary_effect(uint32_t move, const struct px_battle_abilities_6_api *api)
{
    const struct px_pokemon_api *m = api->base->events->memory;
    const uint32_t kind = api->move_kind(user(api), move);
    /* An effect chance (field 0xa) always counts. */
    if (api->move_value(user(api), move, 0xa) != 0) return 1;
    if (kind == 4) return (api->move_value(user(api), move, 0xb) & UINT32_C(0xffff)) != 8;
    if (kind == 6) return 1;
    if (kind == 7) {
        /* Stat changes count unless one of them is negative. */
        const uint32_t count = api->stat_change_count(user(api), move);
        for (uint32_t i = 0; i < count; ++i) {
            api->stat_change(user(api), move, i, api->stat_scratch);
            if ((int32_t)m->load32(m->user, api->stat_scratch) < 0) return 0;
        }
        return 1;
    }
    /* Secret Power (290). */
    return move == 0x122;
}

#define CURE(name, status) \
    PX_BATTLE_ABILITIES_6_HANDLER(name) \
    { \
        (void)handler, (void)work; \
        px_battle_cure_status(context, id, status, api); \
    }
CURE(px_battle_insomnia_cure, 2)      /* falls through into 1084aba4 */
CURE(px_battle_limber_cure, 1)
CURE(px_battle_immunity_cure, 5)
CURE(px_battle_own_tempo_cure, 6)
CURE(px_battle_water_veil_cure, 4)
CURE(px_battle_magma_armor_cure, 3)

PX_BATTLE_ABILITIES_6_HANDLER(px_battle_oblivious_cure)
{
    (void)handler, (void)work;
    px_battle_cure_status(context, id, 7, api);
    px_battle_cure_status(context, id, 0xb, api);
}

PX_BATTLE_ABILITIES_6_HANDLER(px_battle_truant_sleep)
{
    (void)handler, (void)context;
    const struct px_pokemon_api *m = api->base->events->memory;
    /* Key 0x0c = 7: the loafing turn is reset. */
    if (get(0x02, api) == id && get(0x0c, api) == 7) m->store32(m->user, work, 0);
}

/* The target is this battler and the attacker another one. */
static uint32_t shielded(uint32_t id, const struct px_battle_abilities_6_api *api)
{
    return get(0x04, api) == id && get(0x03, api) != id;
}

PX_BATTLE_ABILITIES_6_HANDLER(px_battle_shield_dust_effect)
{
    (void)handler, (void)context, (void)work;
    if (shielded(id, api)) rewrite(0x42, 1, api);
}

PX_BATTLE_ABILITIES_6_HANDLER(px_battle_shield_dust_status)
{
    (void)handler, (void)context, (void)work;
    /* Not for key 0x1e = 8. */
    if (shielded(id, api) && get(0x1e, api) != 8) rewrite(0x42, 1, api);
}

PX_BATTLE_ABILITIES_6_HANDLER(px_battle_shield_dust_poison)
{
    (void)handler, (void)context, (void)work;
    if (get(0x04, api) == id) rewrite(0x48, 1, api);
}

PX_BATTLE_ABILITIES_6_HANDLER(px_battle_shield_dust_flinch)
{
    (void)handler, (void)context, (void)work;
    if (get(0x04, api) == id) rewrite(0x42, 1, api);
}

PX_BATTLE_ABILITIES_6_HANDLER(px_battle_shield_dust_targets)
{
    (void)handler, (void)context, (void)work;
    /* Only when this battler is the single target. */
    const uint32_t count = get(0x05, api);
    for (uint32_t i = 0; i < count; ++i) {
        if (get((i + 6) & UINT32_C(0xff), api) != id) continue;
        if (get(0x05, api) == 1) rewrite(0x48, 1, api);
        return;
    }
}

PX_BATTLE_ABILITIES_6_HANDLER(px_battle_sheer_force_power)
{
    (void)handler, (void)context, (void)work;
    if (get(0x03, api) == id && px_battle_secondary_effect(get(0x12, api) & UINT32_C(0xffff), api) != 0) rewrite(0x49, 1, api);
}

PX_BATTLE_ABILITIES_6_HANDLER(px_battle_sheer_force_effect)
{
    (void)handler, (void)context, (void)work;
    if (get(0x03, api) == id && px_battle_secondary_effect(get(0x12, api) & UINT32_C(0xffff), api) != 0) rewrite(0x42, 1, api);
}

PX_BATTLE_ABILITIES_6_HANDLER(px_battle_sheer_force_flinch)
{
    (void)handler, (void)context, (void)work;
    if (get(0x03, api) == id) rewrite(0x42, 1, api);
}

PX_BATTLE_ABILITIES_6_HANDLER(px_battle_stench)
{
    (void)handler, (void)context, (void)work;
    /* A 10% flinch chance (key 0x27) unless key 0x26 already has one. */
    if (get(0x03, api) == id && (get(0x26, api) & UINT32_C(0xff)) == 0) rewrite(0x27, 0xa, api);
}

uint32_t px_battle_full_hp(uint32_t pokemon, const struct px_battle_abilities_6_api *api)
{
    const uint32_t hp = api->base->pokemon_value(user(api), pokemon, 0xd);
    return hp == api->base->pokemon_value(user(api), pokemon, 0xe);
}

int32_t px_battle_max(int32_t value, int32_t minimum)
{
    return value < minimum ? minimum : value;
}

/* battle_abilities_1's helpers need only the battler lookup's base. */
static const struct px_battle_abilities_1_api *shared(const struct px_battle_abilities_6_api *api,
                                                      struct px_battle_abilities_1_api *storage)
{
    *storage = (struct px_battle_abilities_1_api){.base = api->base};
    return storage;
}

PX_BATTLE_ABILITIES_6_HANDLER(px_battle_natural_cure)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = api->base->events->memory;
    if (get(0x02, api) != id) return;
    /* Kind 0xa: every status (+4 0x26) of battler +5, quietly (+0x12 1). */
    const uint32_t record = api->action_record(user(api), context, 0xa, id);
    m->store8(m->user, record + 4, 0x26);
    m->store8(m->user, record + 0x11, 1);
    m->store8(m->user, record + 0x12, 1);
    m->store8(m->user, record + 5, (uint8_t)id);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_6_HANDLER(px_battle_regenerator)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = api->base->events->memory;
    struct px_battle_abilities_1_api storage;
    if (get(0x02, api) != id) return;
    const uint32_t self = px_battle_pokemon_by_id(context, id, api->base);
    if (px_battle_fainted(self, shared(api, &storage)) || px_battle_full_hp(self, api)) return;
    /* A third of max HP, at most what is missing. */
    uint32_t heal = px_battle_hp_share(self, 3, shared(api, &storage));
    const uint32_t max = api->base->pokemon_value(user(api), self, 0xe);
    const uint32_t missing = max - api->base->pokemon_value(user(api), self, 0xd);
    if (heal > missing) heal = missing;
    /* Kind 7: +4 1, +5 1, +7 the battler, +0x14 the HP. */
    const uint32_t record = api->action_record(user(api), context, 7, id);
    m->store8(m->user, record + 7, (uint8_t)id);
    m->store8(m->user, record + 4, 1);
    m->store32(m->user, record + 0x14, heal);
    m->store8(m->user, record + 5, 1);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_6_HANDLER(px_battle_poison_heal)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = api->base->events->memory;
    struct px_battle_abilities_1_api storage;
    /* Poison damage (status key 0x1e = 5) is cancelled and 1/8 healed instead. */
    if (get(0x02, api) != id || (get(0x1e, api) & UINT32_C(0xffff)) != 5) return;
    const uint32_t self = px_battle_pokemon_by_id(context, id, api->base);
    rewrite(0x33, 0, api);
    const uint32_t record = api->action_record(user(api), context, 4, id);
    const uint32_t hp = px_battle_hp_share(self, 8, shared(api, &storage));
    const uint32_t flags = m->load32(m->user, record);
    m->store8(m->user, record + 6, (uint8_t)id);
    m->store16(m->user, record + 4, (uint16_t)hp);
    m->store32(m->user, record, flags | UINT32_C(0x800000));
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_6_HANDLER(px_battle_heatproof_burn)
{
    (void)handler, (void)context, (void)work;
    /* Burn damage (status key 0x1e = 4) halved toward zero, at least 1. */
    if (get(0x02, api) != id || (get(0x1e, api) & UINT32_C(0xffff)) != 4) return;
    const int32_t damage = (int32_t)get(0x33, api);
    rewrite(0x33, (uint32_t)px_battle_max(damage / 2, 1), api);
}
