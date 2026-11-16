#include "battle_moves_b1.h"

static void *user(const struct px_battle_moves_b1_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_b1_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_b1_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t rewrite(uint32_t key, uint32_t value, const struct px_battle_moves_b1_api *api)
{
    return px_battle_event_rewrite(key, value, api->base->events);
}

static uint32_t contact(const struct px_battle_moves_b1_api *api)
{
    return api->move_flag(user(api), get(0x12, api) & 0xffff, 0) != 0;
}

PX_BATTLE_MOVES_B1_HANDLER(px_battle_final_gambit_faint)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    /* The user faints (kind 0x12, +4 the battler) once key 0x53 is rewritten. */
    if (get(0x03, api) != id || rewrite(0x53, 1, api) == 0) return;
    const uint32_t record = api->action_record(user(api), context, 0x12, id);
    m->store8(m->user, record + 4, (uint8_t)id);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_MOVES_B1_HANDLER(px_battle_roar_message)
{
    (void)context, (void)work;
    /* The forced-switch message: 0x2c9 for Roar (46), otherwise 0x2ca (literals 108add28, 108add2c). */
    if (get(0x03, api) != id) return;
    const uint32_t move = memory(api)->load16(memory(api)->user, handler + 0x38);
    rewrite(0x59, move == 0x2e ? 0x2c9 : 0x2ca, api);
}

PX_BATTLE_MOVES_B1_HANDLER(px_battle_spiky_shield_hit)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x04, api) != id || !contact(api)) return;
    /* The attacker loses 1/8 of its max HP (kind 6, +7 bit 0 cleared, message 0x4c0). */
    const uint32_t attacker = get(0x03, api) & 0xff;
    const uint32_t target = px_battle_pokemon_by_id(context, attacker, api->base);
    const struct px_battle_abilities_1_api shared = {.base = api->base};
    const uint32_t record = api->action_record(user(api), context, 6, id);
    m->store8(m->user, record + 6, (uint8_t)attacker);
    m->store16(m->user, record + 4, (uint16_t)px_battle_hp_share(target, 8, &shared));
    m->store8(m->user, record + 7, m->load8(m->user, record + 7) & ~1u);
    api->message(user(api), record + 0xc, 2, 0x4c0);
    api->message_arg(user(api), record + 0xc, attacker);
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_MOVES_B1_HANDLER(px_battle_kings_shield_hit)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x04, api) != id || !contact(api)) return;
    /* The attacker's Attack -2 (kind 0xd, flags +0x18 | 0x16). */
    const uint32_t record = api->action_record(user(api), context, 0xd, id);
    m->store8(m->user, record + 4, 1);
    m->store8(m->user, record + 0x17, 0xfe);
    m->store8(m->user, record + 0x16, 1);
    m->store8(m->user, record + 0x18, m->load8(m->user, record + 0x18) | 0x16);
    m->store8(m->user, record + 0xc, (uint8_t)get(0x03, api));
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_MOVES_B1_HANDLER(px_battle_pledge_combine)
{
    const struct px_pokemon_api *m = memory(api);
    if (get(0x03, api) != id) return;
    const uint32_t frame = api->pledge_scratch;
    if (api->pledge_partner(user(api), px_battle_pokemon_by_id(context, id, api->base), frame + 4, frame) == 0) return;
    /* Water (518, literal 108b94bc), Fire (519 = 518 | 518 >> 9) and Grass (520) Pledge pair up:
     * 1 water + fire, 2 grass + fire, 3 water + grass. */
    const uint32_t mine = m->load16(m->user, handler + 0x38), theirs = m->load16(m->user, frame);
    const uint32_t water = 0x206, fire = 0x206 | 0x206 >> 9, grass = 0x208;
    uint32_t combo;
    if ((mine == water && theirs == fire) || (theirs == water && mine == fire)) combo = 1;
    else if ((mine == grass && theirs == fire) || (theirs == grass && mine == fire)) combo = 2;
    else if ((mine == water && theirs == grass) || (theirs == water && mine == grass)) combo = 3;
    else combo = 0;
    uint32_t type = get(0x16, api) & 0xff;
    if (combo == 0) return;
    /* Work: +0 combined, +4 the combination, +8 the partner. The move takes type 10, 9 or 11. */
    m->store32(m->user, work + 4, combo);
    m->store32(m->user, work, 1);
    m->store32(m->user, work + 8, m->load8(m->user, frame + 4));
    if (combo == 1) type = 0xa;
    else if (combo == 2) type = 9;
    else if (combo == 3) type = 0xb;
    rewrite(0x16, type, api);
}
