#include "battle_abilities_13.h"

static void *user(const struct px_battle_abilities_13_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_abilities_13_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_abilities_13_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_abilities_13_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

void px_battle_clear_work(uint32_t handler, const struct px_battle_abilities_13_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    for (uint32_t offset = 0x1c; offset <= 0x34; offset += 4) m->store32(m->user, handler + offset, 0);
}

PX_BATTLE_ABILITIES_13_HANDLER(px_battle_magician)
{
    const struct px_pokemon_api *m = memory(api);
    if (get(0x03, api) != id) return;
    /* Empty-handed now and before the move (work +0, set by the event-0x8a handler). */
    if (m->load16(m->user, pokemon(context, id, api) + 0x12) != 0 || m->load32(m->user, work) != 0) return;
    if (get(0x4e, api) != 0) return;
    /* The targets go into the work bytes (at most five). */
    const uint32_t count = get(0x05, api);
    for (uint32_t i = 0; i < count; ++i) m->store8(m->user, work + i, (uint8_t)get((i + 6) & 0xff, api));
    if (count >= 6) {
        api->base->events->abort(user(api));
    } else {
        /* Each target's speed (allies +5) into the halfword table, then sorted slowest first. */
        const uint32_t table = api->speed_table;
        for (uint32_t i = 0; i < count; ++i) {
            const uint32_t target = m->load8(m->user, work + i);
            m->store16(m->user, table + 2 * i, (uint16_t)api->speed(user(api), context, pokemon(context, target, api), 1));
            if (api->is_ally(user(api), id, m->load8(m->user, work + i)) != 0)
                m->store16(m->user, table + 2 * i, (uint16_t)(m->load16(m->user, table + 2 * i) + 5));
        }
        for (uint32_t i = 0; i < count; ++i)
            for (uint32_t j = i + 1; j < count; ++j) {
                const uint32_t a = m->load16(m->user, table + 2 * i), b = m->load16(m->user, table + 2 * j);
                if (b >= a) continue;
                m->store16(m->user, table + 2 * i, (uint16_t)b);
                m->store16(m->user, table + 2 * j, (uint16_t)a);
                const uint32_t x = m->load8(m->user, work + j), y = m->load8(m->user, work + i);
                m->store8(m->user, work + i, (uint8_t)x);
                m->store8(m->user, work + j, (uint8_t)y);
            }
    }
    /* The first target holding an item it may take is robbed (kind 0x25, message 0x427). */
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t victim = pokemon(context, m->load8(m->user, work + i), api);
        if (m->load16(m->user, victim + 0x12) == 0) continue;
        if (api->steal_blocked(user(api), context, id, m->load8(m->user, work + i)) != 0) continue;
        const uint32_t record = api->action_record(user(api), context, 0x25, id);
        m->store32(m->user, record, m->load32(m->user, record) | UINT32_C(0x800000));
        m->store8(m->user, record + 4, m->load8(m->user, work + i));
        api->message(user(api), record + 8, 2, 0x427);  /* literal 108bef0c */
        api->message_arg(user(api), record + 8, id);
        api->message_arg(user(api), record + 8, m->load8(m->user, work + i));
        api->message_arg(user(api), record + 8, m->load16(m->user, victim + 0x12));
        api->commit_actions(user(api), context, record);
        break;
    }
    px_battle_clear_work(handler, api);
}

PX_BATTLE_ABILITIES_13_HANDLER(px_battle_scrappy_unhook)
{
    (void)context, (void)id;
    /* Work +0x10 (set on event 0xb7): the handler removes itself. */
    if (memory(api)->load32(memory(api)->user, work + 0x10) != 0) api->remove_handler(user(api), handler);
}

PX_BATTLE_ABILITIES_13_HANDLER(px_battle_ate_reset)
{
    (void)handler, (void)context, (void)id;
    memory(api)->store32(memory(api)->user, work, 0);
}
