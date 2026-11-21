#include "battle_moves_b7.h"

static void *user(const struct px_battle_moves_b7_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_b7_api *api)
{
    return api->base->events->memory;
}

static uint32_t load8(uint32_t address, const struct px_battle_moves_b7_api *api)
{
    return memory(api)->load8(memory(api)->user, address);
}

static void store8(uint32_t address, uint32_t v, const struct px_battle_moves_b7_api *api)
{
    memory(api)->store8(memory(api)->user, address, (uint8_t)v);
}

/* The shared body, 108b87e4. */
static void party_cure(uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_b7_api *api)
{
    if (px_battle_event_get(0x03, api->base->events) != id) return;
    uint32_t lists[2];
    lists[0] = api->side_list(user(api), context);
    lists[1] = api->partner_list(user(api), context, id);
    const uint32_t list_count = lists[1] == 0 ? 1 : 2;
    uint32_t cured = 0;
    for (uint32_t l = 0; l < list_count; ++l) {
        const uint32_t size = load8(lists[l] + 0x18, api);
        for (uint32_t m = 0; m < size; ++m) {
            const uint32_t member = api->list_member(user(api), lists[l], m & 0xff);
            const uint32_t battler = load8(member + 0x19, api);
            int wanted = api->member_state(user(api), context, battler) == 0xa;
            if (!wanted) {
                const uint32_t targets = px_battle_event_get(0x05, api->base->events);
                for (uint32_t i = 0; i < targets; ++i) {
                    if (px_battle_event_get((i + 6) & 0xff, api->base->events) == battler) {
                        wanted = 1;
                        break;
                    }
                }
            }
            if (!wanted) continue;
            const uint32_t pokemon = px_battle_pokemon_by_id(context, battler, api->base);
            if (pokemon == 0) continue;
            if (api->base->pokemon_status(user(api), pokemon) == 0) continue;
            store8(work + cured, battler, api);
            ++cured;
        }
    }
    if (cured == 0) {
        px_battle_event_rewrite(0x53, 1, api->base->events);
        return;
    }
    const uint32_t rec = api->action_record(user(api), context, 0xa, id);
    store8(rec + 0x11, cured, api);
    for (uint32_t k = 0; k < cured; ++k) store8(rec + 5 + k, load8(work + k, api), api);
    store8(rec + 4, 0x26, api);
    api->commit_actions(user(api), context, rec);
}

PX_BATTLE_MOVES_B7_HANDLER(px_battle_heal_bell_effect)
{
    (void)handler;
    party_cure(context, id, work, api);
}

PX_BATTLE_MOVES_B7_HANDLER(px_battle_aromatherapy_effect)
{
    (void)handler;
    party_cure(context, id, work, api);
}
