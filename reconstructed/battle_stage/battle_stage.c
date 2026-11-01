#include "battle_stage.h"

static void *user(const struct px_battle_stage_api *api)
{
    return api->base->events->user;
}

static uint32_t get(uint32_t key, const struct px_battle_stage_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t rewrite(uint32_t key, uint32_t value, const struct px_battle_stage_api *api)
{
    return px_battle_event_rewrite(key, value, api->base->events);
}

PX_BATTLE_STAGE_HANDLER(px_battle_simple)
{
    (void)handler, (void)context, (void)work;
    if (get(0x02, api) == id) rewrite(0x21, get(0x21, api) << 1, api);
}

PX_BATTLE_STAGE_HANDLER(px_battle_contrary)
{
    (void)handler, (void)context;
    const struct px_pokemon_api *m = api->base->events->memory;
    if (get(0x02, api) == id) m->store32(m->user, work, rewrite(0x21, 0u - get(0x21, api), api));
}

/* A drop (negative stages) caused by another battler, optionally of one stat:
 * blocked through key 0x42, with the result kept in the work word. */
static void block_drop(uint32_t id, uint32_t work, int stat, const struct px_battle_stage_api *api)
{
    const struct px_pokemon_api *m = api->base->events->memory;
    if (get(0x02, api) != id || get(0x03, api) == id) return;
    if (stat >= 0 && get(0x20, api) != (uint32_t)stat) return;
    if ((int32_t)get(0x21, api) >= 0) return;
    m->store32(m->user, work, rewrite(0x42, 1, api));
}

PX_BATTLE_STAGE_HANDLER(px_battle_clear_body) { (void)handler, (void)context; block_drop(id, work, -1, api); }
PX_BATTLE_STAGE_HANDLER(px_battle_hyper_cutter) { (void)handler, (void)context; block_drop(id, work, 1, api); }
PX_BATTLE_STAGE_HANDLER(px_battle_keen_eye) { (void)handler, (void)context; block_drop(id, work, 6, api); }
PX_BATTLE_STAGE_HANDLER(px_battle_big_pecks) { (void)handler, (void)context; block_drop(id, work, 2, api); }

PX_BATTLE_STAGE_HANDLER(px_battle_flower_veil)
{
    (void)handler;
    const struct px_pokemon_api *m = api->base->events->memory;
    /* For an ally (or itself) of the Grass type. */
    const uint32_t target = get(0x02, api) & UINT32_C(0xff);
    if (api->is_ally(user(api), id, target) == 0) return;
    if (api->has_type(user(api), px_battle_pokemon_by_id(context, target, api->base), 11) == 0) return;
    if (get(0x02, api) != target || get(0x03, api) == target) return;
    if ((int32_t)get(0x21, api) >= 0) return;
    m->store32(m->user, work, rewrite(0x42, 1, api));
}

#define MESSAGE(name, message) \
    PX_BATTLE_STAGE_HANDLER(name) { (void)handler; api->blocked_message(user(api), context, id, id, work, message); }
MESSAGE(px_battle_clear_body_message, 0xc6)
MESSAGE(px_battle_hyper_cutter_message, 0xc9)
MESSAGE(px_battle_keen_eye_message, 0xcf)
MESSAGE(px_battle_big_pecks_message, 0xcc)

PX_BATTLE_STAGE_HANDLER(px_battle_flower_veil_message)
{
    (void)handler;
    /* The protected battler is key 0x02; the message names it (literal 108c316c). */
    api->blocked_message(user(api), context, get(0x02, api) & UINT32_C(0xff), id, work, 0x49c);
}

/* Defiant and Competitive: after a drop by an opponent, +2 to one stat. */
static void raise_after_drop(uint32_t context, uint32_t id, uint32_t stat, const struct px_battle_stage_api *api)
{
    const struct px_pokemon_api *m = api->base->events->memory;
    if (get(0x02, api) != id || api->is_ally(user(api), id, get(0x03, api) & UINT32_C(0xff)) != 0) return;
    if ((int32_t)get(0x21, api) >= 0) return;
    const uint32_t record = api->action_record(user(api), context, 0xd, id);
    m->store8(m->user, record + 4, (uint8_t)stat);
    m->store32(m->user, record, m->load32(m->user, record) | UINT32_C(0x800000));
    m->store8(m->user, record + 0x17, 2);
    m->store8(m->user, record + 0x18, m->load8(m->user, record + 0x18) | 2);
    m->store8(m->user, record + 0x16, 1);
    m->store8(m->user, record + 0xc, (uint8_t)id);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_STAGE_HANDLER(px_battle_defiant) { (void)handler, (void)work; raise_after_drop(context, id, 1, api); }
PX_BATTLE_STAGE_HANDLER(px_battle_competitive) { (void)handler, (void)work; raise_after_drop(context, id, 3, api); }
