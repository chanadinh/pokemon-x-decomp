#include "battle_abilities_3.h"

static void *user(const struct px_battle_abilities_3_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_abilities_3_api *api)
{
    return api->base->events->memory;
}

static uint32_t entering(uint32_t id, const struct px_battle_abilities_3_api *api)
{
    return px_battle_event_get(0x02, api->base->events) == id;
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_abilities_3_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static uint32_t load8(uint32_t address, const struct px_battle_abilities_3_api *api)
{
    return memory(api)->load8(memory(api)->user, address);
}

static uint32_t load16(uint32_t address, const struct px_battle_abilities_3_api *api)
{
    return memory(api)->load16(memory(api)->user, address);
}

static void store8(uint32_t address, uint32_t value, const struct px_battle_abilities_3_api *api)
{
    memory(api)->store8(memory(api)->user, address, (uint8_t)value);
}

static void mark(uint32_t record, uint32_t flags, const struct px_battle_abilities_3_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store32(m->user, record, m->load32(m->user, record) | flags);
}

/* 1084c0f8 with the 1084c0ec fallback. */
static uint32_t lucky(uint32_t context, uint32_t percent, const struct px_battle_abilities_3_api *api)
{
    return api->chance(user(api), percent) != 0 || api->forced(user(api), context, 1) != 0;
}

/* 1084d2f4 (battle_abilities_1) needs only the battler lookup's base. */
static uint32_t share(uint32_t target, uint32_t divisor, const struct px_battle_abilities_3_api *api)
{
    const struct px_battle_abilities_1_api shared = {.base = api->base};
    return px_battle_hp_share(target, divisor, &shared);
}

/* Kind 0xd: battler +0xc, stat +4, one battler, stages +0x17. */
static uint32_t stage_record(uint32_t context, uint32_t id, uint32_t stat, uint32_t stages,
                             const struct px_battle_abilities_3_api *api)
{
    const uint32_t record = api->action_record(user(api), context, 0xd, id);
    store8(record + 0x16, 1, api);
    store8(record + 0xc, id, api);
    store8(record + 0x17, stages, api);
    store8(record + 4, stat, api);
    return record;
}

/* Kind 0xa cures battler +5 of every status (+4 0x26, +0x11 1). */
static void cure_record(uint32_t context, uint32_t id, uint32_t target, const struct px_battle_abilities_3_api *api)
{
    const uint32_t record = api->action_record(user(api), context, 0xa, id);
    store8(record + 0x11, 1, api);
    mark(record, UINT32_C(0x2800000), api);
    store8(record + 4, 0x26, api);
    store8(record + 5, target, api);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_3_HANDLER(px_battle_speed_boost)
{
    (void)handler, (void)work;
    if (!entering(id, api) || api->battle_flag(user(api), pokemon(context, id, api), 0) == 0) return;
    const uint32_t record = stage_record(context, id, 5, 1, api);
    mark(record, UINT32_C(0x800000), api);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_3_HANDLER(px_battle_moody)
{
    (void)handler, (void)work;
    if (!entering(id, api)) return;
    /* Buffer: stat pairs (raise << 8 | lower) from +0, raisable stats +0xa8, lowerable +0xaf. */
    const uint32_t buffer = api->temp_buffer(user(api), context, 0xb6);
    const uint32_t self = pokemon(context, id, api);
    uint32_t ups = 0, downs = 0;
    for (uint32_t stat = 1; stat <= 7; ++stat) {
        if (api->stage_can_change(user(api), self, stat, 1) != 0) store8(buffer + 0xa8 + ups, stat, api), ups = (ups + 1) & 0xff;
        if (api->stage_can_change(user(api), self, stat, -1) != 0) store8(buffer + 0xaf + downs, stat, api), downs = (downs + 1) & 0xff;
    }
    uint32_t up = 0, down = 0;
    if (ups == 0) {
        if (downs != 0) down = load8(buffer + 0xaf + api->random(user(api), downs), api);
    } else if (downs == 0) {
        up = load8(buffer + 0xa8 + api->random(user(api), ups), api);
    } else {
        /* Every pair of different stats; the table holds at most 0x54. */
        uint32_t pairs = 0;
        for (uint32_t i = 0; i < ups; ++i)
            for (uint32_t j = 0; j < downs; ++j) {
                const uint32_t raise = load8(buffer + 0xa8 + i, api), lower = load8(buffer + 0xaf + j, api);
                if (raise == lower) continue;
                memory(api)->store16(memory(api)->user, buffer + 2 * pairs, (uint16_t)(lower | raise << 8));
                pairs = (pairs + 1) & 0xff;
                if (pairs >= 0x54) break;
            }
        const uint32_t pair = load16(buffer + 2 * api->random(user(api), pairs), api);
        down = pair & 0xff;
        up = pair >> 8;
    }
    /* +2 to one stat and -1 to another, inside the popup; the records are not flagged. */
    api->ability_popup(user(api), context, 1, id);
    if (up != 0) api->commit_actions(user(api), context, stage_record(context, id, up, 2, api));
    if (down != 0) api->commit_actions(user(api), context, stage_record(context, id, down, 0xff, api));
    api->ability_popup(user(api), context, 2, id);
}

PX_BATTLE_ABILITIES_3_HANDLER(px_battle_harvest)
{
    (void)handler, (void)work;
    if (!entering(id, api)) return;
    const uint32_t self = pokemon(context, id, api);
    const uint32_t berry = load16(self + 0x14, api);
    if (berry == 0 || api->is_berry(user(api), berry) == 0 || load16(self + 0x12, api) != 0) return;
    /* Always in sun, otherwise 50%. */
    if (api->weather(user(api), context) != 1 && !lucky(context, 0x32, api)) return;
    /* Kind 0x21: battler +6 regains item +4 (+7 1). */
    const uint32_t record = api->action_record(user(api), context, 0x21, id);
    memory(api)->store16(memory(api)->user, record + 4, (uint16_t)berry);
    store8(record + 6, id, api);
    store8(record + 7, 1, api);
    mark(record, UINT32_C(0x800000), api);
    api->message(user(api), record + 0xc, 2, 0x1db);  /* literal 108bebf4 */
    api->message_arg(user(api), record + 0xc, id);
    api->message_arg(user(api), record + 0xc, berry);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_3_HANDLER(px_battle_pickup)
{
    (void)handler, (void)work;
    if (!entering(id, api)) return;
    const uint32_t self = pokemon(context, id, api);
    if (load16(self + 0x12, api) != 0) return;
    /* Buffer: battlers +0, count +0xc, +0xd cleared, candidates from +6. */
    const uint32_t selector = api->opposing_selector(user(api), context, id) | 0x200;
    const uint32_t buffer = api->temp_buffer(user(api), context, 0xe);
    const uint32_t count = api->base->side_battlers(user(api), context, selector, buffer);
    store8(buffer + 0xc, count, api);
    store8(buffer + 0xd, 0, api);
    if (count == 0) return;   /* the whole word; the loop bound is the stored byte */
    uint32_t candidates = 0;
    for (uint32_t i = 0; i < load8(buffer + 0xc, api); i = (i + 1) & 0xff) {
        const uint32_t other = pokemon(context, load8(buffer + i, api), api);
        /* An item consumed this turn (turn flag 8) that is still recorded. */
        if (api->turn_flag(user(api), other, 8) == 0 || load16(other + 0x14, api) == 0) continue;
        store8(buffer + 6 + candidates, load8(other + 0x19, api), api);
        candidates = (candidates + 1) & 0xff;
    }
    if (candidates == 0) return;
    const uint32_t from = load8(buffer + 6 + (api->random(user(api), candidates) & 0xff), api);
    const uint32_t item = load16(pokemon(context, from, api) + 0x14, api);
    if (item == 0) return;
    if (api->item_locked(user(api), context, load16(self + 0xc, api), item) != 0) return;
    /* Kind 0x21: battler +6 takes item +4 from battler +9 (+8 1). */
    const uint32_t record = api->action_record(user(api), context, 0x21, id);
    store8(record + 9, from, api);
    store8(record + 6, id, api);
    store8(record + 8, 1, api);
    memory(api)->store16(memory(api)->user, record + 4, (uint16_t)item);
    mark(record, UINT32_C(0x800000), api);
    api->message(user(api), record + 0xc, 2, 0x1ea);  /* literal 108bf74c */
    api->message_arg(user(api), record + 0xc, id);
    api->message_arg(user(api), record + 0xc, item);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_3_HANDLER(px_battle_bad_dreams)
{
    (void)handler, (void)work;
    if (!entering(id, api)) return;
    const uint32_t list = api->list_scratch;
    uint32_t shown = 0;
    const uint32_t count = api->base->side_battlers(user(api), context, api->opposing_selector(user(api), context, id) | 0x100, list);
    for (uint32_t i = 0; i < count; i = (i + 1) & 0xff) {
        const uint32_t target = pokemon(context, load8(list + i, api), api);
        if (api->has_status(user(api), target, 2) == 0) continue;
        /* Every sleeping opponent loses 1/8 inside one popup. */
        if (!shown) shown = 1, api->ability_popup(user(api), context, 1, id);
        const uint32_t record = api->action_record(user(api), context, 6, id);
        store8(record + 6, load8(list + i, api), api);
        memory(api)->store16(memory(api)->user, record + 4, (uint16_t)share(target, 8, api));
        api->message(user(api), record + 0xc, 2, 0x1e4);
        api->message_arg(user(api), record + 0xc, load8(list + i, api));
        api->commit_actions(user(api), context, record);
    }
    if (shown) api->ability_popup(user(api), context, 2, id);
}

PX_BATTLE_ABILITIES_3_HANDLER(px_battle_slow_start_turn)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    /* Work: +4 turns counted, +8 finished, +0xc cleared when finished. */
    if (!entering(id, api) || m->load32(m->user, work + 8) != 0) return;
    if (m->load32(m->user, work + 4) < 5) {
        if (api->queued_action(user(api), context, id, api->action_scratch) == 0) return;
        if ((m->load32(m->user, api->action_scratch) & 0xf) == 3) return;
        const uint32_t turns = m->load32(m->user, work + 4) + 1;
        m->store32(m->user, work + 4, turns);
        if (turns < 5) return;
    }
    const uint32_t record = api->action_record(user(api), context, 3, id);
    api->message(user(api), record + 4, 2, 0x1f3);  /* literal 108c4c68 */
    api->message_arg(user(api), record + 4, id);
    api->commit_actions(user(api), context, record);
    m->store32(m->user, work + 8, 1);
    m->store32(m->user, work + 0xc, 0);
}

PX_BATTLE_ABILITIES_3_HANDLER(px_battle_shed_skin)
{
    (void)handler, (void)work;
    if (!entering(id, api) || api->base->pokemon_status(user(api), pokemon(context, id, api)) == 0) return;
    if (!lucky(context, 0x21, api)) return;
    cure_record(context, id, id, api);
}

PX_BATTLE_ABILITIES_3_HANDLER(px_battle_healer)
{
    (void)handler, (void)work;
    if (!entering(id, api)) return;
    const uint32_t list = api->list_scratch;
    const uint32_t count = api->base->side_battlers(user(api), context, api->base->side_of(user(api), context, id) | 0x400, list);
    for (uint32_t i = 0; i < count; i = (i + 1) & 0xff) {
        const uint32_t ally = load8(list + i, api);
        if (ally == id || api->base->pokemon_status(user(api), pokemon(context, ally, api)) == 0) continue;
        if (!lucky(context, 0x1e, api)) continue;
        cure_record(context, id, load8(list + i, api), api);
    }
}

PX_BATTLE_ABILITIES_3_HANDLER(px_battle_levitate_turn)
{
    (void)handler, (void)context;
    if (entering(id, api)) memory(api)->store32(memory(api)->user, work, 0);
}
