#include "battle_abilities_12.h"

static void *user(const struct px_battle_abilities_12_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_abilities_12_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_abilities_12_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t rewrite(uint32_t key, uint32_t value, const struct px_battle_abilities_12_api *api)
{
    return px_battle_event_rewrite(key, value, api->base->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_abilities_12_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static uint32_t load32(uint32_t address, const struct px_battle_abilities_12_api *api)
{
    return memory(api)->load32(memory(api)->user, address);
}

static void store32(uint32_t address, uint32_t value, const struct px_battle_abilities_12_api *api)
{
    memory(api)->store32(memory(api)->user, address, value);
}

static void mark(uint32_t record, const struct px_battle_abilities_12_api *api)
{
    store32(record, load32(record, api) | UINT32_C(0x800000), api);
}

static uint32_t fainted(uint32_t target, const struct px_battle_abilities_12_api *api)
{
    const struct px_battle_abilities_1_api shared = {.base = api->base};
    return px_battle_fainted(target, &shared);
}

PX_BATTLE_ABILITIES_12_HANDLER(px_battle_imposter)
{
    (void)handler, (void)work;
    if (get(0x02, api) != id) return;
    /* Transforms into the battler facing its position (kind 0x34, +4 the battler, message 0x284). */
    const uint32_t position = api->base->side_of(user(api), context, id);
    const uint32_t facing = api->facing_position(user(api), api->battle_mode(user(api), context), position);
    const uint32_t target = api->battler_at(user(api), context, facing);
    const uint32_t model = pokemon(context, target, api);
    if (fainted(pokemon(context, id, api), api) || fainted(model, api)) return;
    const uint32_t record = api->action_record(user(api), context, 0x34, id);
    memory(api)->store8(memory(api)->user, record + 4, (uint8_t)target);
    mark(record, api);
    api->message(user(api), record + 8, 2, 0x284);
    api->message_arg(user(api), record + 8, id);
    api->message_arg(user(api), record + 8, memory(api)->load8(memory(api)->user, record + 4));
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_12_HANDLER(px_battle_truant_loaf)
{
    (void)handler, (void)context;
    if (get(0x02, api) != id) return;
    /* Every other turn the move fails (reason 0x13); work +4 holds the rewrite. */
    if (load32(work, api) == 0) {
        store32(work, 1, api);
        return;
    }
    store32(work + 4, rewrite(0x23, 0x13, api), api);
    store32(work, 0, api);
}

PX_BATTLE_ABILITIES_12_HANDLER(px_battle_truant_message)
{
    (void)handler;
    if (get(0x02, api) != id || load32(work + 4, api) == 0) return;
    const uint32_t record = api->action_record(user(api), context, 3, id);
    mark(record, api);
    api->message(user(api), record + 4, 2, 0x1bd);  /* literal 108c198c */
    api->message_arg(user(api), record + 4, id);
    api->commit_actions(user(api), context, record);
    store32(work + 4, 0, api);
}

PX_BATTLE_ABILITIES_12_HANDLER(px_battle_synchronize)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x04, api) != id) return;
    /* Poison (5), paralysis (1) or burn (4) is passed back to its source (key 0x03). */
    const uint32_t status = get(0x1e, api) & 0xff;
    if (status != 5 && status != 1 && status != 4) return;
    const uint32_t source = get(0x03, api) & 0xff;
    if (source == 0x1f || source == id) return;
    const uint32_t self = pokemon(context, id, api);
    const uint32_t condition = get(0x1f, api);
    api->ability_popup(user(api), context, 1, id);
    /* Kind 0xb: +4 status, +8 condition (bad poison is kept), +0xc 1, +0xf the source. */
    const uint32_t record = api->action_record(user(api), context, 0xb, id);
    m->store8(m->user, record + 0xf, (uint8_t)source);
    m->store16(m->user, record + 4, (uint16_t)status);
    if (status == 5 && api->badly_poisoned(user(api), condition) != 0) store32(record + 8, condition, api);
    else api->condition_for(user(api), status, self, record + 8);
    m->store8(m->user, record + 0xc, 1);
    api->commit_actions(user(api), context, record);
    api->ability_popup(user(api), context, 2, id);
}

PX_BATTLE_ABILITIES_12_HANDLER(px_battle_aura_break_reverse)
{
    (void)handler, (void)context, (void)id, (void)work;
    /* The auras are reversed (key 0x57), for every battler. */
    rewrite(0x57, 1, api);
}

PX_BATTLE_ABILITIES_12_HANDLER(px_battle_protean)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x03, api) != id) return;
    /* Becomes the move's type (below 18), kind 0x13: +4 t | t << 5 | 0x4800, +6 the battler. */
    const uint32_t type = get(0x16, api) & 0xff;
    if (type >= 0x12) return;
    const uint32_t record = api->action_record(user(api), context, 0x13, id);
    m->store8(m->user, record + 6, (uint8_t)id);
    mark(record, api);
    m->store16(m->user, record + 4, (uint16_t)((type & 0x1f) | (type & 0x1f) << 5 | 0x4800));
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_12_HANDLER(px_battle_stance_change)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x03, api) != id) return;
    /* Aegislash (681): King's Shield (588) the shield form 0, damaging moves the blade form 1. */
    const uint32_t self = pokemon(context, id, api);
    if (m->load16(m->user, self + 0xc) != 0x2a9) return;
    const uint32_t move = get(0x12, api) & 0xffff;
    const uint32_t now = m->load8(m->user, self + 0x153);
    uint32_t next = now;
    if (move == 0x24c) next = 0;
    else if (api->damaging_move(user(api), move) != 0) next = 1;
    if (next == now) return;
    const uint32_t record = api->action_record(user(api), context, 0x3a, id);
    m->store8(m->user, record + 4, (uint8_t)id);
    m->store8(m->user, record + 5, (uint8_t)next);
    mark(record, api);
    api->message(user(api), record + 8, 1, next == 0 ? 0xfd : 0xfc);
    api->commit_actions(user(api), context, record);
}

/* The illusion breaks: kind 0x35 (+4 the battler), message 0x1de naming it, unflagged. */
static void illusion_break(uint32_t context, uint32_t id, const struct px_battle_abilities_12_api *api)
{
    if (api->illusion_active(user(api), pokemon(context, id, api)) == 0) return;
    const uint32_t record = api->action_record(user(api), context, 0x35, id);
    memory(api)->store8(memory(api)->user, record + 4, (uint8_t)id);
    api->message(user(api), record + 8, 2, 0x1de);  /* literals 108c1724, 108c2a54, 108c4240 */
    api->message_arg(user(api), record + 8, id);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_12_HANDLER(px_battle_illusion_leave)
{
    (void)handler, (void)work;
    if (get(0x02, api) == id) illusion_break(context, id, api);
}

PX_BATTLE_ABILITIES_12_HANDLER(px_battle_illusion_hit)
{
    (void)handler, (void)work;
    if (get(0x04, api) == id && get(0x47, api) == 0) illusion_break(context, id, api);
}

PX_BATTLE_ABILITIES_12_HANDLER(px_battle_illusion_lost)
{
    (void)work;
    /* Losing the ability (key 0x10 differs from the handler's +0x38). */
    if (get(0x10, api) == memory(api)->load16(memory(api)->user, handler + 0x38)) return;
    if (get(0x02, api) == id) illusion_break(context, id, api);
}

PX_BATTLE_ABILITIES_12_HANDLER(px_battle_steadfast)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    /* Flinching (reason 6): Speed +1 (kind 0xd, flags +0x18 without bit 1). */
    if (get(0x23, api) != 6 || get(0x02, api) != id) return;
    const uint32_t record = api->action_record(user(api), context, 0xd, id);
    m->store8(m->user, record + 4, 5);
    mark(record, api);
    m->store8(m->user, record + 0x17, 1);
    m->store8(m->user, record + 0x18, m->load8(m->user, record + 0x18) & ~2u);
    m->store8(m->user, record + 0x16, 1);
    m->store8(m->user, record + 0xc, (uint8_t)id);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_12_HANDLER(px_battle_mega_launcher)
{
    (void)handler, (void)context, (void)work;
    /* Heal Pulse (505): its multiplier key 0x36 is rewritten to 0xc00. */
    if (get(0x03, api) == id && (get(0x12, api) & 0xffff) == 0x1f9) rewrite(0x36, 0xc00, api);
}
