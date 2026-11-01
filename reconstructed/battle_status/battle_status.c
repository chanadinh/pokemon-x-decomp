#include "battle_status.h"

static void *user(const struct px_battle_status_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_status_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_status_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t block(const struct px_battle_status_api *api)
{
    return px_battle_event_rewrite(0x42, 1, api->base->events);
}

static uint32_t status(const struct px_battle_status_api *api)
{
    return get(0x1e, api) & UINT32_C(0xffff);
}

/* The target (key 0x04, compared in full) receiving the status: the rewrite
 * result, or 0. */
static uint32_t try_block(uint32_t target, uint32_t code, const struct px_battle_status_api *api)
{
    if (get(0x04, api) == target && status(api) == code) return block(api);
    return 0;
}

static void set_work(uint32_t work, uint32_t value, const struct px_battle_status_api *api)
{
    memory(api)->store32(memory(api)->user, work, value);
}

#define ONE_STATUS(name, code) \
    PX_BATTLE_STATUS_HANDLER(name) { (void)handler, (void)context; set_work(work, try_block(id, code, api), api); }
ONE_STATUS(px_battle_limber, 1)
ONE_STATUS(px_battle_magma_armor, 3)
ONE_STATUS(px_battle_water_veil, 4)
ONE_STATUS(px_battle_immunity_status, 5)
ONE_STATUS(px_battle_own_tempo, 6)

/* Two statuses: the second is tried only when the first was not blocked. */
static void two_statuses(uint32_t target, uint32_t work, uint32_t first, uint32_t second,
                         const struct px_battle_status_api *api)
{
    const uint32_t result = try_block(target, first, api);
    set_work(work, result, api);
    if (result != 0) return;
    set_work(work, try_block(target, second, api), api);
}

PX_BATTLE_STATUS_HANDLER(px_battle_insomnia) { (void)handler, (void)context; two_statuses(id, work, 2, 0xe, api); }
PX_BATTLE_STATUS_HANDLER(px_battle_oblivious_status) { (void)handler, (void)context; two_statuses(id, work, 7, 0xb, api); }

PX_BATTLE_STATUS_HANDLER(px_battle_sweet_veil)
{
    (void)handler, (void)context;
    /* Sleep and drowsiness on this battler's side. */
    const uint32_t target = get(0x04, api) & UINT32_C(0xff);
    if (api->is_ally(user(api), id, target) == 0) return;
    two_statuses(target, work, 2, 0xe, api);
}

PX_BATTLE_STATUS_HANDLER(px_battle_leaf_guard)
{
    (void)handler;
    if (get(0x04, api) != id || api->weather(user(api), context) != 1) return;
    const uint32_t code = status(api);
    if (api->major_status(user(api), code) != 0 || code == 0xe) set_work(work, block(api), api);
}

PX_BATTLE_STATUS_HANDLER(px_battle_flower_veil_status)
{
    (void)handler;
    /* Major statuses (codes below 6) and drowsiness on Grass-type allies. */
    const uint32_t target = get(0x04, api) & UINT32_C(0xff);
    if (api->is_ally(user(api), id, target) == 0) return;
    if (api->has_type(user(api), px_battle_pokemon_by_id(context, target, api->base), 11) == 0) return;
    if ((get(0x03, api) & UINT32_C(0xff)) == target) return;
    const uint32_t code = status(api);
    if (code < 6 || code == 0xe) set_work(work, block(api), api);
}

PX_BATTLE_STATUS_HANDLER(px_battle_aroma_veil)
{
    (void)handler, (void)context;
    const uint32_t target = get(0x04, api) & UINT32_C(0xff);
    const uint32_t code = status(api);
    if (api->is_ally(user(api), id, target) == 0 || api->mental_status(user(api), code) == 0) return;
    const uint32_t result = block(api);
    set_work(work, result, api);
    if (result != 0) px_battle_event_rewrite(0x44, 1, api->base->events);
}

PX_BATTLE_STATUS_HANDLER(px_battle_uproar)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    /* Work word 6 holds whether the uproar is going on; key 0x1e in full. */
    if (m->load32(m->user, work + UINT32_C(0x18)) == 0 || get(0x1e, api) != 2 || block(api) == 0) return;
    const uint32_t record = api->action_record(user(api), context, 3, id);
    const uint32_t target = get(0x04, api) & UINT32_C(0xff);
    api->message(user(api), record + 4, 2, target == id ? 0x2c8 : 0x2c5);  /* literal 108baee0 */
    api->message_arg(user(api), record + 4, target);
    api->commit_actions(user(api), context, record);
}

/* The message of event 0x70 inside the ability popup; the work word is cleared. */
static void popup_message(uint32_t context, uint32_t id, uint32_t work, uint32_t message, uint32_t named,
                          const struct px_battle_status_api *api)
{
    api->ability_popup(user(api), context, 1, id);
    const uint32_t record = api->action_record(user(api), context, 3, id);
    api->message(user(api), record + 4, 2, message);
    api->message_arg(user(api), record + 4, named);
    api->commit_actions(user(api), context, record);
    api->ability_popup(user(api), context, 2, id);
    set_work(work, 0, api);
}

static void own_message(uint32_t context, uint32_t id, uint32_t work, uint32_t message,
                        const struct px_battle_status_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    if (get(0x04, api) == id && m->load32(m->user, work) == 1) popup_message(context, id, work, message, id, api);
}

static void ally_message(uint32_t context, uint32_t id, uint32_t work, uint32_t message,
                         const struct px_battle_status_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    const uint32_t target = get(0x04, api) & UINT32_C(0xff);
    if (api->is_ally(user(api), id, target) != 0 && m->load32(m->user, work) != 0)
        popup_message(context, id, work, message, target, api);
}

PX_BATTLE_STATUS_HANDLER(px_battle_status_blocked_message) { (void)handler; own_message(context, id, work, 0xd2, api); }
PX_BATTLE_STATUS_HANDLER(px_battle_own_tempo_message) { (void)handler; own_message(context, id, work, 0x165, api); }  /* literal 108c5038 */
PX_BATTLE_STATUS_HANDLER(px_battle_aroma_veil_message) { (void)handler; ally_message(context, id, work, 0x49f, api); }  /* literal 108c3ba4 */
PX_BATTLE_STATUS_HANDLER(px_battle_flower_veil_status_message) { (void)handler; ally_message(context, id, work, 0x49c, api); }  /* literal 108c526c */
PX_BATTLE_STATUS_HANDLER(px_battle_sweet_veil_message) { (void)handler; ally_message(context, id, work, 0x4a2, api); }  /* literal 108c583c */
