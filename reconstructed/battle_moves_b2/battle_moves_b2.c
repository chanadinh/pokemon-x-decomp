#include "battle_moves_b2.h"

static void *user(const struct px_battle_moves_b2_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_b2_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_b2_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_moves_b2_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static void store8(uint32_t address, uint32_t value, const struct px_battle_moves_b2_api *api)
{
    memory(api)->store8(memory(api)->user, address, (uint8_t)value);
}

static void store16(uint32_t address, uint32_t value, const struct px_battle_moves_b2_api *api)
{
    memory(api)->store16(memory(api)->user, address, (uint16_t)value);
}

static void store32(uint32_t address, uint32_t value, const struct px_battle_moves_b2_api *api)
{
    memory(api)->store32(memory(api)->user, address, value);
}

static uint32_t load8(uint32_t address, const struct px_battle_moves_b2_api *api)
{
    return memory(api)->load8(memory(api)->user, address);
}

/* A kind-3 message naming the user. */
static void announce(uint32_t context, uint32_t id, uint32_t message, const struct px_battle_moves_b2_api *api)
{
    const uint32_t record = api->action_record(user(api), context, 3, id);
    api->message(user(api), record + 4, 2, message);
    api->message_arg(user(api), record + 4, id);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_MOVES_B2_HANDLER(px_battle_teleport_effect)
{
    (void)work;
    if (get(0x03, api) != id) return;
    /* The handler runs again (re-entry mark), then the escape record (kind 0x29). */
    px_battle_handler_mark_reentry(handler, api->base->events);
    api->header_record(user(api), context, 0x29, id);
}

PX_BATTLE_MOVES_B2_HANDLER(px_battle_hold_hands_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Only in double and triple battles (modes 1 and 2). */
    const uint32_t mode = api->battle_mode(user(api), context);
    if (mode != 1 && mode != 2) return;
    px_battle_event_rewrite(0x53, 0, api->base->events);
    api->header_record(user(api), context, 0x3c, id);
}

PX_BATTLE_MOVES_B2_HANDLER(px_battle_wish_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Kind 0x1f registers the wish at the user's position (field 0). */
    const uint32_t record = api->action_record(user(api), context, 0x1f, id);
    store8(record + 4, 0, api);
    store8(record + 5, api->base->side_of(user(api), context, id), api);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_MOVES_B2_HANDLER(px_battle_kings_shield_effect)
{
    (void)handler;
    if (get(0x03, api) != id) return;
    api->protect_start(user(api), context, id);
    const uint32_t record = api->action_record(user(api), context, 0x15, id);
    store8(record + 5, id, api);
    store8(record + 4, 0x10, api);
    api->commit_actions(user(api), context, record);
    store32(work + 0x18, 1, api);
}

PX_BATTLE_MOVES_B2_HANDLER(px_battle_grudge_effect)
{
    (void)handler;
    if (get(0x03, api) != id) return;
    announce(context, id, 0x278, api);
    store32(work + 0x18, 1, api);
}

PX_BATTLE_MOVES_B2_HANDLER(px_battle_follow_me_effect)
{
    (void)handler;
    if (get(0x03, api) != id) return;
    announce(context, id, 0x29e, api);  /* literal 108b98e4 */
    store32(work + 0x18, 1, api);
}

PX_BATTLE_MOVES_B2_HANDLER(px_battle_quash_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Kind 0x32: the target (+4) moves last; message 0x471 (literal 108b2e30) names it. */
    const uint32_t record = api->action_record(user(api), context, 0x32, id);
    store8(record + 4, get(0x06, api), api);
    api->message(user(api), record + 8, 2, 0x471);
    api->message_arg(user(api), record + 8, load8(record + 4, api));
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_MOVES_B2_HANDLER(px_battle_magic_coat_effect)
{
    (void)handler;
    if (get(0x03, api) != id || memory(api)->load32(memory(api)->user, work + 0x18) != 0) return;
    announce(context, id, 0x2f9, api);  /* literal 108b2734 */
    store32(work + 0x18, 1, api);
}

PX_BATTLE_MOVES_B2_HANDLER(px_battle_refresh_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Poison (5), paralysis (1) or burn (4) is cured. */
    const uint32_t status = api->base->pokemon_status(user(api), pokemon(context, id, api));
    if (status != 5 && status != 1 && status != 4) return;
    const uint32_t record = api->action_record(user(api), context, 0xa, id);
    store8(record + 4, status, api);
    store8(record + 0x11, 1, api);
    store8(record + 5, id, api);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_MOVES_B2_HANDLER(px_battle_forests_curse_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* The target gains the Grass type (11) unless it already has it (kind 0x14). */
    const uint32_t target = get(0x06, api) & 0xff;
    if (api->has_type(user(api), pokemon(context, target, api), 0xb) != 0) return;
    const uint32_t record = api->action_record(user(api), context, 0x14, id);
    store8(record + 5, target, api);
    store8(record + 4, 0xb, api);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_MOVES_B2_HANDLER(px_battle_transform_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Kind 0x34: transform into the target (+4); message 0x284 names both. */
    const uint32_t record = api->action_record(user(api), context, 0x34, id);
    store8(record + 4, get(0x06, api), api);
    api->message(user(api), record + 8, 2, 0x284);
    api->message_arg(user(api), record + 8, id);
    api->message_arg(user(api), record + 8, load8(record + 4, api));
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_MOVES_B2_HANDLER(px_battle_magnet_rise_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Condition 0x1e for five turns; message 0x292 (literal 108b3614). */
    const uint32_t record = api->action_record(user(api), context, 0xb, id);
    store8(record + 0xf, id, api);
    store16(record + 4, 0x1e, api);
    store32(record + 8, px_battle_turn_count(5), api);
    api->message(user(api), record + 0x14, 2, 0x292);
    api->message_arg(user(api), record + 0x14, id);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_MOVES_B2_HANDLER(px_battle_snatch_effect)
{
    (void)handler;
    if (get(0x03, api) != id || api->moves_last(user(api), context, id) != 0) return;
    announce(context, id, 0x2ef, api);  /* literal 108b125c */
    store32(work + 0x18, 1, api);
}

PX_BATTLE_MOVES_B2_HANDLER(px_battle_imprison_effect)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x03, api) != id) return;
    /* Kind 0x1c: +4 3, +8 the condition, +0xc message 0x24a (literal 108adbe4),
     * +0xe the top two bits kept with 0x82, +0x10 the user, +0x30 1. */
    const uint32_t record = api->action_record(user(api), context, 0x1c, id);
    store8(record + 4, 3, api);
    const uint32_t condition = api->imprison_condition(user(api), id);
    const uint32_t flags = m->load16(m->user, record + 0xe);
    store32(record + 8, condition, api);
    store8(record + 0x30, 1, api);
    store16(record + 0xc, 0x24a, api);
    store32(record + 0x10, id, api);
    store16(record + 0xe, (flags >> 14) << 14 | 0x82, api);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_MOVES_B2_HANDLER(px_battle_torment_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* The target is tormented (condition 0xc) unless it already is. */
    const uint32_t target = get(0x06, api) & 0xff;
    if (api->has_status(user(api), pokemon(context, target, api), 0xc) != 0) return;
    const uint32_t record = api->action_record(user(api), context, 0xb, id);
    store16(record + 4, 0xc, api);
    store32(record + 8, 1, api);
    store8(record + 0xf, target, api);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_MOVES_B2_HANDLER(px_battle_soak_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Every target becomes pure Water (packed 0x494a, literal 108b3c68). */
    const uint32_t count = get(0x05, api);
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t record = api->action_record(user(api), context, 0x13, id);
        store16(record + 4, 0x494a, api);
        store8(record + 6, get((i + 6) & 0xff, api), api);
        store8(record + 8, 1, api);
        api->commit_actions(user(api), context, record);
    }
}

PX_BATTLE_MOVES_B2_HANDLER(px_battle_conversion_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* The user takes its first move's type unless it already has it. */
    const uint32_t self = pokemon(context, id, api);
    const uint32_t type = api->move_type(user(api), api->move_at(user(api), self, 0));
    if (api->has_type(user(api), self, type) != 0) return;
    const uint32_t record = api->action_record(user(api), context, 0x13, id);
    store8(record + 6, id, api);
    store16(record + 4, (type & 0x1f) | (type & 0x1f) << 5 | 0x4800, api);
    api->commit_actions(user(api), context, record);
}
