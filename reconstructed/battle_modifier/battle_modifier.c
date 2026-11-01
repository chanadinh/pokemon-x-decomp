#include "battle_modifier.h"

static void *user(const struct px_battle_modifier_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_modifier_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_modifier_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static void multiply(uint32_t factor, const struct px_battle_modifier_api *api)
{
    px_battle_event_multiply(0x36, factor, api->base->events);
}

uint32_t px_battle_effectiveness_class(uint32_t code)
{
    if (code == 7) return 1;
    if (code > 7) return 2;
    return code == 0 ? 0 : 3;
}

uint32_t px_battle_pokemon_state(uint32_t pokemon, uint32_t flag, const struct px_pokemon_api *m)
{
    const uint8_t bits = m->load8(m->user, pokemon + UINT32_C(0x169) + (flag >> 3));
    return (bits & (uint8_t)(1u << (flag & 7))) != 0;
}

/* (parameter 3 + 100)% in 4096 units, rounded half away from zero in single
 * precision and divided by 100 with truncation. */
uint32_t px_battle_item_factor(uint32_t handler, const struct px_battle_modifier_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t param = api->item_param(user(api), m->load16(m->user, handler + UINT32_C(0x38)), 3);
    const float scaled = (float)(int32_t)((param << 12) + UINT32_C(0x64000));
    const float rounded = (int32_t)(param + 100) > 0 ? scaled + 0.5f : scaled - 0.5f;  /* literal 1084cc38 */
    return (uint32_t)((int32_t)rounded / 100);
}

uint32_t px_battle_resist_berry(uint32_t context, uint32_t id, uint32_t work, uint32_t type,
                                uint32_t any_hit, const struct px_battle_modifier_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    if (get(0x04, api) != id || get(0x16, api) != type || get(0x47, api) != 0) return 0;
    if (any_hit == 0 && (int32_t)get(0x39, api) <= 7) return 0;
    /* The work word marks the berry as used. */
    if (m->load32(m->user, work) != 0) return 0;
    multiply(0x800, api);
    if (api->no_consume(user(api), context) != 0) return 1;
    m->store32(m->user, work, 1);
    const uint32_t record = api->action_record(user(api), context, 0x15, id);
    m->store8(m->user, record + UINT32_C(5), (uint8_t)id);
    m->store8(m->user, record + UINT32_C(4), 0x11);
    api->commit_actions(user(api), context);
    return 1;
}

#define BERRY(name, type, any_hit) \
    PX_BATTLE_MODIFIER_HANDLER(name) { (void)handler; px_battle_resist_berry(context, id, work, type, any_hit, api); }
BERRY(px_battle_occa_berry, 9, 0)
BERRY(px_battle_passho_berry, 10, 0)
BERRY(px_battle_wacan_berry, 12, 0)
BERRY(px_battle_rindo_berry, 11, 0)
BERRY(px_battle_yache_berry, 14, 0)
BERRY(px_battle_chople_berry, 1, 0)
BERRY(px_battle_kebia_berry, 3, 0)
BERRY(px_battle_shuca_berry, 4, 0)
BERRY(px_battle_coba_berry, 2, 0)
BERRY(px_battle_payapa_berry, 13, 0)
BERRY(px_battle_tanga_berry, 6, 0)
BERRY(px_battle_charti_berry, 5, 0)
BERRY(px_battle_kasib_berry, 7, 0)
BERRY(px_battle_haban_berry, 15, 0)
BERRY(px_battle_colbur_berry, 16, 0)
BERRY(px_battle_babiri_berry, 8, 0)
BERRY(px_battle_chilan_berry, 0, 1)
BERRY(px_battle_roseli_berry, 17, 0)

PX_BATTLE_MODIFIER_HANDLER(px_battle_expert_belt)
{
    (void)context, (void)work;
    if (get(0x03, api) == id && (int32_t)get(0x39, api) > 7) multiply(px_battle_item_factor(handler, api), api);
}

PX_BATTLE_MODIFIER_HANDLER(px_battle_life_orb)
{
    (void)context, (void)work;
    if (get(0x03, api) == id) multiply(px_battle_item_factor(handler, api), api);
}

PX_BATTLE_MODIFIER_HANDLER(px_battle_metronome_item)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x03, api) != id || api->move_excluded(user(api), get(0x12, api) & UINT32_C(0xffff)) != 0) return;
    const uint32_t pokemon = px_battle_pokemon_by_id(context, id, api->base);
    const uint32_t streak = api->use_streak(user(api), pokemon) & UINT32_C(0xffff);
    if (streak == 0) return;
    if ((get(0x12, api) & UINT32_C(0xffff)) != api->last_move(user(api), pokemon)) return;
    /* 100% plus the item's parameter per consecutive use, at most 200%. */
    const uint32_t param = api->item_param(user(api), m->load16(m->user, handler + UINT32_C(0x38)), 3);
    uint32_t percent = (param * streak + 100) & UINT32_C(0xffff);
    if (percent > 200) percent = 200;
    const float scaled = percent == 0 ? -0.5f : (float)(percent << 12) + 0.5f;  /* literals 108a6780, 108a6784 */
    multiply((uint32_t)((int32_t)scaled / 100), api);
}

PX_BATTLE_MODIFIER_HANDLER(px_battle_sniper)
{
    (void)handler, (void)context, (void)work;
    if (get(0x03, api) == id && get(0x46, api) != 0) multiply(0x1800, api);
}

PX_BATTLE_MODIFIER_HANDLER(px_battle_solid_rock)
{
    (void)handler, (void)context, (void)work;
    if (get(0x04, api) == id && px_battle_effectiveness_class(get(0x39, api) & UINT32_C(0xff)) == 2) multiply(0xc00, api);
}

PX_BATTLE_MODIFIER_HANDLER(px_battle_tinted_lens)
{
    (void)handler, (void)context, (void)work;
    if (get(0x03, api) == id && px_battle_effectiveness_class(get(0x39, api) & UINT32_C(0xff)) == 3) multiply(0x2000, api);
}

PX_BATTLE_MODIFIER_HANDLER(px_battle_multiscale)
{
    (void)handler, (void)work;
    if (get(0x04, api) != id) return;
    if (api->full_hp(user(api), px_battle_pokemon_by_id(context, id, api->base)) != 0) multiply(0x800, api);
}

PX_BATTLE_MODIFIER_HANDLER(px_battle_friend_guard)
{
    (void)handler, (void)context, (void)work;
    const uint32_t defender = get(0x04, api) & UINT32_C(0xff);
    if (defender != id && api->is_ally(user(api), id, defender) != 0) multiply(0xc00, api);
}

/* The move handlers: the attacker's move doubles against a defender state. */
static void against_state(uint32_t context, uint32_t id, uint32_t flag, const struct px_battle_modifier_api *api)
{
    if (get(0x03, api) != id) return;
    const uint32_t defender = px_battle_pokemon_by_id(context, get(0x04, api) & UINT32_C(0xff), api->base);
    if (px_battle_pokemon_state(defender, flag, memory(api)) != 0) multiply(0x2000, api);
}

PX_BATTLE_MODIFIER_HANDLER(px_battle_hits_underwater_surf) { (void)handler, (void)work; against_state(context, id, 4, api); }
PX_BATTLE_MODIFIER_HANDLER(px_battle_hits_underwater_whirlpool) { (void)handler, (void)work; against_state(context, id, 4, api); }
PX_BATTLE_MODIFIER_HANDLER(px_battle_hits_underground) { (void)handler, (void)work; against_state(context, id, 5, api); }
PX_BATTLE_MODIFIER_HANDLER(px_battle_hits_minimized) { (void)handler, (void)work; against_state(context, id, 8, api); }
