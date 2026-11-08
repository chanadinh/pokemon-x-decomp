#include "battle_moves_2.h"

enum { ATTACKER = 0x03, DEFENDER = 0x04, TYPE = 0x16, POWER = 0x31, MULTIPLIER = 0x32 };

static void *user(const struct px_battle_moves_2_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_2_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_2_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t attacking(uint32_t id, const struct px_battle_moves_2_api *api)
{
    return get(ATTACKER, api) == id;
}

static uint32_t battler(uint32_t context, uint32_t id, const struct px_battle_moves_2_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static uint32_t defender(uint32_t context, const struct px_battle_moves_2_api *api)
{
    return battler(context, get(DEFENDER, api) & 0xff, api);
}

static void multiply(uint32_t factor, const struct px_battle_moves_2_api *api)
{
    px_battle_event_multiply(MULTIPLIER, factor, api->base->events);
}

/* Base power * HP / max HP (unsigned divide veneer 1080041c), at least 1. */
PX_BATTLE_MOVES_2_HANDLER(px_battle_eruption)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    const uint32_t attacker = battler(context, id, api);
    const uint32_t hp = api->base->pokemon_value(user(api), attacker, 0xd);
    const uint32_t max_hp = api->base->pokemon_value(user(api), attacker, 0xe);
    const uint32_t power = api->move_power(user(api), m->load16(m->user, handler + UINT32_C(0x38))) * hp / max_hp;
    px_battle_event_rewrite(POWER, power != 0 ? power : 1, api->base->events);
}

/* x2 when the defender has at most 50% HP (signed compare with 0x32000). */
PX_BATTLE_MOVES_2_HANDLER(px_battle_brine)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if ((int32_t)api->hp_percent(user(api), defender(context, api)) > 0x32000) return;
    multiply(0x2000, api);
}

/* x2 for an Electric move while work word 0 is 2 (tested first). */
PX_BATTLE_MOVES_2_HANDLER(px_battle_charge_power)
{
    (void)handler, (void)context;
    const struct px_pokemon_api *m = memory(api);
    if (m->load32(m->user, work) != 2) return;
    if (!attacking(id, api)) return;
    if (get(TYPE, api) == 12) multiply(0x2000, api);
}

/* x2 when a battler listed in faint record kind 1 is an ally. */
PX_BATTLE_MOVES_2_HANDLER(px_battle_retaliate)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t record = api->faint_record(user(api), context);
    const uint32_t count = api->faint_count(user(api), record, 1);
    for (uint32_t i = 0; i < count; i = (i + 1) & 0xff) {
        if (api->is_ally(user(api), id, api->faint_id(user(api), record, 1, i)) != 0) {
            multiply(0x2000, api);
            return;
        }
    }
}

/* x2 against a poisoned defender. */
PX_BATTLE_MOVES_2_HANDLER(px_battle_venoshock)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if (api->has_status(user(api), defender(context, api), 5) != 0) multiply(0x2000, api);
}

/* Fusion Flare (558) after Fusion Bolt (559) this turn, or the reverse: x2 and
 * effect flag 1. The partner ids are literals 108b68bc and 108b68c0. */
PX_BATTLE_MOVES_2_HANDLER(px_battle_fusion)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    const uint32_t history = context + UINT32_C(0x20);
    const uint32_t move = m->load16(m->user, handler + UINT32_C(0x38));
    uint32_t partner;
    if (move == 558) partner = 559;
    else if (move == 559) partner = 558;
    else return;
    if (api->turn_last_move(user(api), history, api->turn(user(api), context)) != partner) return;
    multiply(0x2000, api);
    api->set_effect_flag(user(api), context, 1);
}

PX_BATTLE_MOVES_2_HANDLER(px_battle_me_first_power)
{
    (void)handler, (void)context, (void)work;
    if (attacking(id, api)) multiply(0x1800, api);
}

/* x0.5 in rain, hail or sandstorm. */
PX_BATTLE_MOVES_2_HANDLER(px_battle_solar_beam_power)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t weather = api->weather(user(api), context);
    if (weather == 2 || weather == 3 || weather == 4) multiply(0x800, api);
}

/* x1.5 when the attacker is the battler in work word 0 (the handler's own id is not used). */
PX_BATTLE_MOVES_2_HANDLER(px_battle_helping_hand_power)
{
    (void)handler, (void)context, (void)id;
    const struct px_pokemon_api *m = memory(api);
    if (get(ATTACKER, api) == m->load32(m->user, work)) multiply(0x1800, api);
}

/* x2 while poisoned, paralysed or burned. */
PX_BATTLE_MOVES_2_HANDLER(px_battle_facade_power)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t status = api->base->pokemon_status(user(api), battler(context, id, api));
    if (status == 5 || status == 1 || status == 4) multiply(0x2000, api);
}

/* x1.5 when the defender holds an item that can be knocked off. */
PX_BATTLE_MOVES_2_HANDLER(px_battle_knock_off_power)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    const uint32_t target = get(DEFENDER, api) & 0xff;
    if (m->load16(m->user, battler(context, target, api) + UINT32_C(0x12)) == 0) return;
    if (api->item_locked(user(api), context, id, target) == 0) multiply(0x1800, api);
}

/* The defender's attack is used: key 0x3c becomes the defender id. */
PX_BATTLE_MOVES_2_HANDLER(px_battle_foul_play)
{
    (void)handler, (void)context, (void)work;
    if (!attacking(id, api)) return;
    px_battle_event_rewrite(0x3c, get(DEFENDER, api) & 0xff, api->base->events);
}

/* Key 0x3d 11 (special defense): key 0x3e advances by one; an odd key 0x3e makes
 * the stat routine use defense instead. */
PX_BATTLE_MOVES_2_HANDLER(px_battle_psyshock)
{
    (void)handler, (void)context, (void)work;
    if (!attacking(id, api)) return;
    if (get(0x3d, api) != 0xb) return;
    px_battle_event_rewrite(0x3e, get(0x3e, api) + 1, api->base->events);
}

/* Key 0x53: the defender's stat stages are ignored. */
PX_BATTLE_MOVES_2_HANDLER(px_battle_chip_away)
{
    (void)handler, (void)context, (void)work;
    if (attacking(id, api)) px_battle_event_rewrite(0x53, 1, api->base->events);
}

/* Key 0x54: a burn does not halve Facade's attack. */
PX_BATTLE_MOVES_2_HANDLER(px_battle_facade_burn)
{
    (void)handler, (void)context, (void)work;
    if (attacking(id, api)) px_battle_event_rewrite(0x54, 1, api->base->events);
}
