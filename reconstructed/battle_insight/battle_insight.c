#include "battle_insight.h"

static void *user(const struct px_battle_insight_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_insight_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_insight_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_insight_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static uint32_t ability_of(uint32_t context, uint32_t id, const struct px_battle_insight_api *api)
{
    return api->base->pokemon_value(user(api), pokemon(context, id, api), 0x10) & UINT32_C(0xff);
}

/* Trace's record: copy an ability (kind 0x20), with message 0x17d naming the
 * source and the ability. Abilities 0 and an entering id 0x1f are skipped. */
static void copy_ability(uint32_t context, uint32_t id, uint32_t ability, uint32_t source,
                         const struct px_battle_insight_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    if (ability == 0 || id == 0x1f) return;
    const uint32_t record = api->action_record(user(api), context, 0x20, id);
    const uint32_t flags = m->load32(m->user, record);
    m->store8(m->user, record + 6, (uint8_t)id);
    m->store16(m->user, record + 4, (uint16_t)ability);
    m->store8(m->user, record + 8, 1);
    m->store32(m->user, record, flags | UINT32_C(0x800000));
    api->message(user(api), record + 0xc, 2, 0x17d);  /* literal 108bcd24 */
    api->message_arg(user(api), record + 0xc, source);
    api->message_arg(user(api), record + 0xc, m->load16(m->user, record + 4));
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_INSIGHT_HANDLER(px_battle_trace)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    const uint32_t entering = get(0x02, api) & UINT32_C(0xff);
    if (entering == id) {
        /* Its own entry: a random opponent whose ability can be traced. */
        const uint32_t list = api->list_scratch, candidates = api->candidate_scratch;
        const uint32_t count = api->side_battlers(user(api), context, api->opposing_selector(user(api), context, id) | 0x100, list);
        uint32_t found = 0;
        for (uint32_t i = 0; i < count; i = (i + 1) & UINT32_C(0xff)) {
            if (api->untraceable(user(api), ability_of(context, m->load8(m->user, list + i), api)) != 0) continue;
            m->store8(m->user, candidates + found, m->load8(m->user, list + i));
            found = (found + 1) & UINT32_C(0xff);
        }
        if (found == 0) {
            /* Nothing to trace now: wait for an opponent to enter. */
            m->store32(m->user, work, 1);
            return;
        }
        const uint32_t pick = (found == 1 ? 0 : api->random(user(api), found)) & UINT32_C(0xff);
        const uint32_t source = pokemon(context, m->load8(m->user, candidates + pick), api);
        const uint32_t ability = api->base->pokemon_value(user(api), source, 0x10) & UINT32_C(0xff);
        copy_ability(context, id, ability, m->load8(m->user, source + UINT32_C(0x19)), api);
        return;
    }
    /* Pending: an opponent entering in a position this battler faces. */
    if (api->is_ally(user(api), entering, id) != 0 || m->load32(m->user, work) != 1) return;
    const uint32_t position = api->position(user(api), context, entering);
    const uint32_t selector = api->opposing_selector(user(api), context, id);
    if (position == 0xa) return;
    if (api->battle_mode(user(api), context) == 2 && api->position_in(user(api), selector, position) == 0) return;
    const uint32_t ability = ability_of(context, entering, api);
    if (api->untraceable(user(api), ability) != 0) return;
    copy_ability(context, id, ability, entering, api);
}

PX_BATTLE_INSIGHT_HANDLER(px_battle_download)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    const uint32_t list = api->list_scratch;
    const uint32_t count = api->opponents(user(api), context, id, list);
    if (count == 0) return;
    /* Defense against special defense, 16-bit sums over the opponents. */
    uint32_t defense = 0, special = 0;
    for (uint32_t i = 0; i < count; i = (i + 1) & UINT32_C(0xff)) {
        const uint32_t p = pokemon(context, m->load8(m->user, list + i), api);
        defense = (defense + api->base->pokemon_value(user(api), p, 9)) & UINT32_C(0xffff);
        special = (special + api->base->pokemon_value(user(api), p, 0xb)) & UINT32_C(0xffff);
    }
    const uint32_t stat = defense < special ? 1 : 3;
    const uint32_t record = api->action_record(user(api), context, 0xd, id);
    const uint32_t flags = m->load32(m->user, record);
    m->store8(m->user, record + 0xc, (uint8_t)id);
    m->store8(m->user, record + 4, (uint8_t)stat);
    m->store8(m->user, record + 0x16, 1);
    m->store32(m->user, record, flags | UINT32_C(0x800000));
    m->store8(m->user, record + 0x17, 1);
    api->commit_actions(user(api), context, record);
}

uint32_t px_battle_forewarn_power(uint32_t move, const struct px_battle_insight_api *api)
{
    if (api->move_category(user(api), move) == 0) return 1;
    const uint32_t power = api->move_power(user(api), move) & UINT32_C(0xff);
    if (power != 1) return power;
    /* Variable power: one-hit KO 150; Counter (68), Mirror Coat (243) and
     * Metal Burst (368) 120; others 80. */
    if (api->move_kind(user(api), move) == 9) return 0x96;
    if (move == 0x44 || move == 0xf3 || move == 0x170) return 0x78;
    return 0x50;
}

PX_BATTLE_INSIGHT_HANDLER(px_battle_forewarn)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    const uint32_t list = api->list_scratch;
    const uint32_t count = api->opponents(user(api), context, id, list);
    const uint32_t found = api->temp_buffer(user(api), context, 0x14);
    /* The strongest moves: entries of a battler id byte and a 16-bit move. */
    uint32_t best = 0, entries = 0;
    for (uint32_t i = 0; i < count; i = (i + 1) & UINT32_C(0xff)) {
        const uint32_t p = pokemon(context, m->load8(m->user, list + i), api);
        const uint32_t moves = m->load8(m->user, p + UINT32_C(0x152));
        for (uint32_t slot = 0; slot < moves; slot = (slot + 1) & UINT32_C(0xff)) {
            const uint32_t move = api->move_at(user(api), p, slot);
            const uint32_t power = px_battle_forewarn_power(move, api);
            if (power < best) continue;
            if (power > best) {
                best = power;
                entries = 0;
            }
            m->store8(m->user, found + entries * 4, m->load8(m->user, p + UINT32_C(0x19)));
            m->store16(m->user, found + entries * 4 + 2, (uint16_t)move);
            entries = (entries + 1) & UINT32_C(0xff);
        }
    }
    if (entries == 0) return;
    const uint32_t pick = api->random(user(api), entries) & UINT32_C(0xff);
    api->ability_popup(user(api), context, 1, id);
    const uint32_t record = api->action_record(user(api), context, 3, id);
    api->message(user(api), record + 4, 2, 0x1b1);  /* literal 108bd614 */
    api->message_arg(user(api), record + 4, m->load8(m->user, found + pick * 4));
    api->message_arg(user(api), record + 4, m->load16(m->user, found + pick * 4 + 2));
    api->commit_actions(user(api), context, record);
    api->ability_popup(user(api), context, 2, id);
}

PX_BATTLE_INSIGHT_HANDLER(px_battle_anticipation)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    const uint32_t list = api->list_scratch;
    const uint32_t count = api->opponents(user(api), context, id, list);
    const uint32_t self = pokemon(context, id, api);
    /* A one-hit KO move, or a damaging move super effective against this battler. */
    uint32_t i = 0;
    for (; i < count; i = (i + 1) & UINT32_C(0xff)) {
        const uint32_t p = pokemon(context, m->load8(m->user, list + i), api);
        const uint32_t types = api->packed_types(user(api), self);
        const uint32_t moves = m->load8(m->user, p + UINT32_C(0x152));
        uint32_t slot = 0;
        for (; slot < moves; slot = (slot + 1) & UINT32_C(0xff)) {
            const uint32_t move = api->move_at(user(api), p, slot);
            if (api->move_kind(user(api), move) == 9) break;
            if (api->damaging_move(user(api), move) == 0) continue;
            uint32_t type;
            if (move == 0xed) {
                /* Hidden Power (237): the holder's own type. */
                const uint32_t holder = pokemon(context, m->load8(m->user, p + UINT32_C(0x19)), api);
                if (holder != 0) {
                    type = api->hidden_power_type(user(api), m->load32(m->user, holder));
                } else {
                    api->base->events->abort(user(api));
                    type = 0;
                }
            } else {
                type = api->move_type(user(api), move);
            }
            if (api->effectiveness(user(api), type, types) > 7) break;
        }
        if (slot < moves) break;
    }
    if (i == count) return;
    api->ability_popup(user(api), context, 1, id);
    const uint32_t record = api->action_record(user(api), context, 3, id);
    api->message(user(api), record + 4, 2, 0x1b4);
    api->message_arg(user(api), record + 4, id);
    api->commit_actions(user(api), context, record);
    api->ability_popup(user(api), context, 2, id);
}

PX_BATTLE_INSIGHT_HANDLER(px_battle_frisk)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    /* Buffer: items (u16) at +0, opponent ids at +0xa, indices with an item at +0xf. */
    const uint32_t buffer = api->temp_buffer(user(api), context, 0x14);
    const uint32_t count = api->opponents(user(api), context, id, buffer + 0xa);
    uint32_t held = 0;
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t item = m->load16(m->user, pokemon(context, m->load8(m->user, buffer + 0xa + i), api) + UINT32_C(0x12));
        m->store16(m->user, buffer + 2 * i, (uint16_t)item);
        if (item != 0) {
            m->store8(m->user, buffer + 0xf + held, (uint8_t)i);
            held = (held + 1) & UINT32_C(0xff);
        }
    }
    if (held == 0) return;
    api->ability_popup(user(api), context, 1, id);
    for (uint32_t k = 0; k < held; ++k) {
        const uint32_t record = api->action_record(user(api), context, 3, id);
        const uint32_t j = m->load8(m->user, buffer + 0xf + k);
        if (k == 0) {
            /* The first report names this battler too (literal 108be844). */
            api->message(user(api), record + 4, 2, 0x1b7);
            api->message_arg(user(api), record + 4, id);
        } else {
            api->message(user(api), record + 4, 1, 0xfe);
        }
        api->message_arg(user(api), record + 4, m->load8(m->user, buffer + 0xa + j));
        api->message_arg(user(api), record + 4, m->load16(m->user, buffer + 2 * j));
        api->commit_actions(user(api), context, record);
    }
    api->ability_popup(user(api), context, 2, id);
}
