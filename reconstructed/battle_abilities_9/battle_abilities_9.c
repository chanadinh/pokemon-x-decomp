#include "battle_abilities_9.h"

static void *user(const struct px_battle_abilities_9_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_abilities_9_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_abilities_9_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t rewrite(uint32_t key, uint32_t value, const struct px_battle_abilities_9_api *api)
{
    return px_battle_event_rewrite(key, value, api->base->events);
}

static uint32_t load32(uint32_t address, const struct px_battle_abilities_9_api *api)
{
    return memory(api)->load32(memory(api)->user, address);
}

static void store32(uint32_t address, uint32_t value, const struct px_battle_abilities_9_api *api)
{
    memory(api)->store32(memory(api)->user, address, value);
}

/* 1084aeec, 1084aef8 (battle_moves_3) and 1084c700 (battle_items_4). */
static void px_battle_clear_hook(uint32_t handler, const struct px_battle_abilities_9_api *api)
{
    px_battle_clear_handler_word(handler, memory(api));
}

static void px_battle_set_hook(uint32_t handler, uint32_t hook, const struct px_battle_abilities_9_api *api)
{
    px_battle_set_handler_word(handler, hook, memory(api));
}

static void px_battle_set_owner_six(uint32_t handler, const struct px_battle_abilities_9_api *api)
{
    px_battle_item_mark_consumed(handler, memory(api));
}

/* Damp: hooked for every move (no battler check), cleared at its end. */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_damp_hook) { (void)context, (void)id, (void)work; px_battle_set_hook(handler, api->damp_hook, api); }
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_damp_unhook) { (void)context, (void)id, (void)work; px_battle_clear_hook(handler, api); }

/* Key 0x02 is this battler: the hook is cleared. */
static void unhook_own(uint32_t handler, uint32_t id, const struct px_battle_abilities_9_api *api)
{
    if (get(0x02, api) == id) px_battle_clear_hook(handler, api);
}

PX_BATTLE_ABILITIES_9_HANDLER(px_battle_damp_unhook_entry) { (void)context, (void)work; unhook_own(handler, id, api); }

PX_BATTLE_ABILITIES_9_HANDLER(px_battle_infiltrator_hook)
{
    (void)context, (void)work;
    if (get(0x03, api) == id) px_battle_set_hook(handler, api->infiltrator_hook, api);
}

PX_BATTLE_ABILITIES_9_HANDLER(px_battle_infiltrator_unhook) { (void)context, (void)work; unhook_own(handler, id, api); }

PX_BATTLE_ABILITIES_9_HANDLER(px_battle_infiltrator_pass)
{
    (void)handler, (void)context, (void)work;
    if (get(0x03, api) == id) rewrite(0x53, 1, api);
}

/* Mold Breaker's hook is installed once per move (work word 1) and cleared by its battler. */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_mold_breaker_hook)
{
    (void)context;
    if (get(0x03, api) != id || load32(work, api) != 0) return;
    px_battle_set_hook(handler, api->mold_breaker_hook, api);
    store32(work, 1, api);
}

static void mold_breaker_unhook(uint32_t handler, uint32_t id, uint32_t work, const struct px_battle_abilities_9_api *api)
{
    if (get(0x02, api) != id || load32(work, api) != 1) return;
    px_battle_clear_hook(handler, api);
    store32(work, 0, api);
}

PX_BATTLE_ABILITIES_9_HANDLER(px_battle_mold_breaker_unhook) { (void)context; mold_breaker_unhook(handler, id, work, api); }
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_mold_breaker_leave) { (void)context; mold_breaker_unhook(handler, id, work, api); }

PX_BATTLE_ABILITIES_9_HANDLER(px_battle_scrappy_self)
{
    (void)handler, (void)context;
    /* Its own move at itself: work word +0x10 = 1. */
    if (get(0x03, api) == id && get(0x04, api) == id) store32(work + 0x10, 1, api);
}

PX_BATTLE_ABILITIES_9_HANDLER(px_battle_liquid_ooze_owner)
{
    (void)context, (void)work;
    if (get(0x02, api) == id) px_battle_set_owner_six(handler, api);
}

PX_BATTLE_ABILITIES_9_HANDLER(px_battle_klutz_fling)
{
    (void)handler, (void)context;
    if (get(0x02, api) != id) return;
    /* Fling (363) fails with reason 0x13; work word +8 holds the rewrite (0 otherwise). */
    const uint32_t move = get(0x12, api) & UINT32_C(0xffff);
    store32(work + 8, 0, api);
    if (move != 0x16b || get(0x02, api) != id) return;
    store32(work + 8, rewrite(0x23, 0x13, api), api);
}

PX_BATTLE_ABILITIES_9_HANDLER(px_battle_klutz_fling_message)
{
    (void)handler;
    if (get(0x02, api) != id || load32(work + 8, api) == 0) return;
    (void)get(0x02, api);   /* read again; the value is unused */
    const uint32_t record = api->action_record(user(api), context, 3, id);
    store32(record, load32(record, api) | UINT32_C(0x800000), api);
    api->message(user(api), record + 4, 2, 0x38f);  /* literal 108c273c */
    api->message_arg(user(api), record + 4, id);
    api->message_arg(user(api), record + 4, get(0x12, api));
    api->commit_actions(user(api), context, record);
    store32(work + 8, 0, api);
}

PX_BATTLE_ABILITIES_9_HANDLER(px_battle_klutz_leave)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    /* Holding an item: kind 0x22 (+4 the battler), unflagged. */
    if (m->load16(m->user, px_battle_pokemon_by_id(context, id, api->base) + 0x12) == 0) return;
    const uint32_t record = api->action_record(user(api), context, 0x22, id);
    m->store8(m->user, record + 4, (uint8_t)id);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_9_HANDLER(px_battle_klutz_unhook) { (void)context, (void)work; unhook_own(handler, id, api); }

/* Hooked again once (work word), with no battler check. */
static void rehook(uint32_t handler, uint32_t work, uint32_t hook, const struct px_battle_abilities_9_api *api)
{
    px_battle_set_hook(handler, hook, api);
    store32(work, 1, api);
}

PX_BATTLE_ABILITIES_9_HANDLER(px_battle_klutz_rehook)
{
    (void)context, (void)id;
    if (load32(work, api) == 0) rehook(handler, work, api->klutz_hook, api);
}

/* "The opposing side is too nervous" (message 0xb6, kind 1) inside the popup. */
static void unnerve(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_abilities_9_api *api)
{
    const uint32_t side = api->opposing_side(user(api), api->side_of(user(api), id));
    api->ability_popup(user(api), context, 1, id);
    const uint32_t record = api->action_record(user(api), context, 3, id);
    api->message(user(api), record + 4, 1, 0xb6);
    api->message_arg(user(api), record + 4, side);
    api->commit_actions(user(api), context, record);
    api->ability_popup(user(api), context, 2, id);
    rehook(handler, work, api->unnerve_hook, api);
}

PX_BATTLE_ABILITIES_9_HANDLER(px_battle_unnerve_effect)
{
    if (get(0x02, api) == id && load32(work, api) == 0) unnerve(handler, context, id, work, api);
}

PX_BATTLE_ABILITIES_9_HANDLER(px_battle_unnerve_rehook)
{
    if (load32(work, api) == 0) unnerve(handler, context, id, work, api);
}

PX_BATTLE_ABILITIES_9_HANDLER(px_battle_slow_start_restart)
{
    (void)handler;
    if (load32(work, api) != 0) return;
    /* Work: +0 started, +4 turns 0, +8 finished 0, +0xc active 1. */
    store32(work, 1, api);
    store32(work + 4, 0, api);
    store32(work + 8, 0, api);
    store32(work + 0xc, 1, api);
    const uint32_t record = api->action_record(user(api), context, 3, id);
    store32(record, load32(record, api) | UINT32_C(0x800000), api);
    api->message(user(api), record + 4, 2, 0x1f0);
    api->message_arg(user(api), record + 4, id);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_9_HANDLER(px_battle_slow_start_attack)
{
    (void)handler, (void)context;
    /* While active: physical moves (category 1) x0.5. */
    if (get(0x03, api) != id || load32(work + 0xc, api) == 0) return;
    if (api->base->move_category(user(api), get(0x12, api) & UINT32_C(0xffff)) == 1)
        px_battle_event_multiply(0x36, 0x800, api->base->events);
}
