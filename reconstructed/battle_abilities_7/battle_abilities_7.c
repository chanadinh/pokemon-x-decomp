#include "battle_abilities_7.h"

static void *user(const struct px_battle_abilities_7_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_abilities_7_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_abilities_7_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t rewrite(uint32_t key, uint32_t value, const struct px_battle_abilities_7_api *api)
{
    return px_battle_event_rewrite(key, value, api->base->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_abilities_7_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static void mark(uint32_t record, const struct px_battle_abilities_7_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store32(m->user, record, m->load32(m->user, record) | UINT32_C(0x800000));
}

/* The battler switching (key 0x02) is not an ally: list this battler's
 * opponents into the stack area; returns the count. */
static uint32_t opponents(uint32_t context, uint32_t id, const struct px_battle_abilities_7_api *api)
{
    return api->base->side_battlers(user(api), context, api->opposing_selector(user(api), context, id) | 0x100, api->list_scratch);
}

static uint32_t listed(uint32_t i, const struct px_battle_abilities_7_api *api)
{
    return memory(api)->load8(memory(api)->user, api->list_scratch + i);
}

static uint32_t opponent_switching(uint32_t id, const struct px_battle_abilities_7_api *api)
{
    return api->is_ally(user(api), get(0x02, api) & UINT32_C(0xff), id) == 0;
}

PX_BATTLE_ABILITIES_7_HANDLER(px_battle_shadow_tag)
{
    (void)handler, (void)work;
    if (!opponent_switching(id, api)) return;
    /* Trapped unless one of the listed battlers has Shadow Tag (23) itself. */
    const uint32_t count = opponents(context, id, api);
    for (uint32_t i = 0; i < count; i = (i + 1) & 0xff)
        if (api->base->pokemon_value(user(api), pokemon(context, listed(i, api), api), 0x11) == 0x17) return;
    rewrite(0x42, 1, api);
}

PX_BATTLE_ABILITIES_7_HANDLER(px_battle_arena_trap)
{
    (void)handler, (void)work;
    if (!opponent_switching(id, api)) return;
    /* The first listed battler on the ground is trapped. */
    const uint32_t count = opponents(context, id, api);
    for (uint32_t i = 0; i < count; i = (i + 1) & 0xff) {
        if (api->airborne(user(api), context, listed(i, api)) != 0) continue;
        rewrite(0x42, 1, api);
        return;
    }
}

PX_BATTLE_ABILITIES_7_HANDLER(px_battle_magnet_pull)
{
    (void)handler, (void)work;
    if (!opponent_switching(id, api)) return;
    /* Each listed Steel type (8) rewrites key 0x42. */
    const uint32_t count = opponents(context, id, api);
    for (uint32_t i = 0; i < count; i = (i + 1) & 0xff)
        if (api->has_type(user(api), pokemon(context, listed(i, api), api), 8) != 0) rewrite(0x42, 1, api);
}

PX_BATTLE_ABILITIES_7_HANDLER(px_battle_run_away)
{
    (void)handler, (void)context, (void)work;
    if (api->is_ally(user(api), get(0x02, api) & UINT32_C(0xff), id) != 0) rewrite(0x53, 1, api);
}

PX_BATTLE_ABILITIES_7_HANDLER(px_battle_run_away_message)
{
    (void)handler, (void)work;
    if (api->is_ally(user(api), get(0x02, api) & UINT32_C(0xff), id) == 0 || rewrite(0x53, 1, api) == 0) return;
    /* Message 0x4b (kind 1) with the extra word 0xd0024 (literal 108c08b4). */
    const uint32_t record = api->action_record(user(api), context, 3, id);
    mark(record, api);
    api->message(user(api), record + 4, 1, 0x4b);
    api->message_extra(user(api), record + 4, 0xd0024);
    api->commit_actions(user(api), context, record);
}

/* A flagged message naming this battler. */
static void announce(uint32_t context, uint32_t id, uint32_t message, const struct px_battle_abilities_7_api *api)
{
    const uint32_t record = api->action_record(user(api), context, 3, id);
    mark(record, api);
    api->message(user(api), record + 4, 2, message);
    api->message_arg(user(api), record + 4, id);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_7_HANDLER(px_battle_suction_cups)
{
    (void)handler, (void)work;
    if (get(0x04, api) == id && rewrite(0x42, 1, api) != 0) announce(context, id, 0x1c6, api);  /* literal 108bdaac */
}

PX_BATTLE_ABILITIES_7_HANDLER(px_battle_levitate_check)
{
    (void)handler, (void)context;
    const struct px_pokemon_api *m = memory(api);
    /* Ground moves miss: key 0x53 set, remembered in the work word. */
    if (get(0x02, api) != id || get(0x53, api) != 0) return;
    m->store32(m->user, work, rewrite(0x53, 1, api));
}

PX_BATTLE_ABILITIES_7_HANDLER(px_battle_levitate_message)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id || m->load32(m->user, work) == 0) return;
    announce(context, id, 0xd2, api);
    m->store32(m->user, work, 0);
}

PX_BATTLE_ABILITIES_7_HANDLER(px_battle_pressure_pp)
{
    (void)handler, (void)context, (void)work;
    if (api->is_ally(user(api), get(0x03, api) & UINT32_C(0xff), id) != 0) return;
    /* One more PP when it is a target, or for field moves (kind 10) and the
     * moves listed by 108a48d4. */
    const uint32_t count = get(0x05, api);
    uint32_t i = 0;
    for (; i < count; ++i)
        if (get((i + 6) & UINT32_C(0xff), api) == id) break;
    if (i == count) {
        const uint32_t move = get(0x12, api) & UINT32_C(0xffff);
        if (api->move_kind(user(api), move) != 0xa && api->pressure_move(user(api), move) == 0) return;
    }
    rewrite(0x21, (get(0x21, api) + 1) & UINT32_C(0xff), api);
}

PX_BATTLE_ABILITIES_7_HANDLER(px_battle_prankster)
{
    (void)handler, (void)context, (void)work;
    /* Status moves (category 0): priority + 1. */
    if (get(0x03, api) != id || api->base->move_category(user(api), get(0x12, api) & UINT32_C(0xffff)) != 0) return;
    rewrite(0x19, get(0x19, api) + 1, api);
}

PX_BATTLE_ABILITIES_7_HANDLER(px_battle_gale_wings)
{
    (void)handler, (void)context, (void)work;
    /* Flying moves (type 2): priority + 1. */
    if (get(0x03, api) == id && get(0x16, api) == 2) rewrite(0x19, get(0x19, api) + 1, api);
}

PX_BATTLE_ABILITIES_7_HANDLER(px_battle_sturdy_ohko)
{
    (void)handler, (void)work;
    if (get(0x04, api) != id || rewrite(0x42, 1, api) == 0) return;
    /* "It doesn't affect" (0xd2) inside the popup, unflagged. */
    api->ability_popup(user(api), context, 1, id);
    const uint32_t record = api->action_record(user(api), context, 3, id);
    api->message(user(api), record + 4, 2, 0xd2);
    api->message_arg(user(api), record + 4, id);
    api->commit_actions(user(api), context, record);
    api->ability_popup(user(api), context, 2, id);
}

PX_BATTLE_ABILITIES_7_HANDLER(px_battle_sturdy_endure)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    const struct px_battle_abilities_6_api shared = {.base = api->base};
    if (get(0x04, api) != id) return;
    /* At full HP it survives (reason 4); the work word holds the rewrite's result. */
    uint32_t endured = 0;
    if (px_battle_full_hp(pokemon(context, id, api), &shared) != 0) endured = rewrite(0x3b, 4, api);
    m->store32(m->user, work, endured);
}

PX_BATTLE_ABILITIES_7_HANDLER(px_battle_sturdy_message)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id || m->load32(m->user, work) == 0) return;
    api->ability_popup(user(api), context, 1, id);
    const uint32_t record = api->action_record(user(api), context, 3, id);
    api->message(user(api), record + 4, 2, 0x202);  /* literal 108c2978 */
    api->message_arg(user(api), record + 4, id);
    api->commit_actions(user(api), context, record);
    api->ability_popup(user(api), context, 2, id);
    m->store32(m->user, work, 0);
}
