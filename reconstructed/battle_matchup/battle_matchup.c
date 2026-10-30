#include "battle_matchup.h"

uint32_t px_battle_packed_types(uint32_t pokemon, const struct px_battle_matchup_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const int roost = (m->load32(m->user, pokemon + UINT32_C(0x7c)) & 7) != 0;
    uint32_t first = m->load8(m->user, pokemon + UINT32_C(0x100));
    if (first == 2 && roost) first = PX_BATTLE_TYPELESS;
    const uint32_t raw_second = m->load8(m->user, pokemon + UINT32_C(0x101));
    uint32_t second = raw_second == 2 && roost ? PX_BATTLE_TYPELESS : raw_second;
    if (first != PX_BATTLE_TYPELESS) {
        if (second == PX_BATTLE_TYPELESS) second = first;
    } else {
        /* A typeless first type takes the second; both typeless become Normal. */
        if (second == PX_BATTLE_TYPELESS) second = 0;
        first = second;
    }
    const uint32_t third = m->load8(m->user, pokemon + UINT32_C(0x102));
    return (third & 0x1f) << 10 | (second & 0x1f) << 5 | (first & 0x1f);
}

uint32_t px_battle_single_matchup(uint32_t context, uint32_t attacker, uint32_t defender,
                                  uint32_t attack_type, uint32_t defend_type,
                                  const struct px_battle_matchup_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    void *u = api->user;
    api->event_push(u);
    api->event_set_id(u, 0x03, m->load8(m->user, attacker + UINT32_C(0x19)));
    api->event_set_id(u, 0x04, m->load8(m->user, defender + UINT32_C(0x19)));
    api->event_set_id(u, 0x15, defend_type);
    api->event_set_id(u, 0x16, attack_type);
    api->event_set_value(u, 0x4c, 0);
    api->event_set_value(u, 0x4d, 0);
    api->event_set_value(u, 0x39, 0xe);
    api->event_set_value(u, 0x17, PX_BATTLE_TYPELESS);
    api->type_event_state(u, defender);
    api->event_dispatch(u, context, 0x44);
    const int no_immunity = api->event_get(u, 0x4c) != 0;
    const int neutral = api->event_get(u, 0x4d) != 0;
    const uint32_t extra_type = api->event_get(u, 0x17) & UINT32_C(0xff);
    uint32_t code = api->event_get(u, 0x39) & UINT32_C(0xff);
    api->event_pop(u);
    if (neutral) return PX_BATTLE_NEUTRAL;
    /* Key 0x39 left at 0xe means no handler supplied a code. */
    if (code == 0xe) code = px_battle_type_effectiveness(attack_type, defend_type, api->types);
    /* Key 0x17 adds a second attacking type. */
    if (extra_type != PX_BATTLE_TYPELESS)
        code = px_battle_combine_effectiveness(code, px_battle_type_effectiveness(extra_type, defend_type, api->types),
                                               api->types);
    if (code == 0 && no_immunity) return PX_BATTLE_NEUTRAL;
    return code;
}

/* Event 0x45 may replace an immunity with a code (key 0x39). */
static uint32_t immunity_override(uint32_t context, uint32_t attacker, uint32_t defender, uint32_t move,
                                  const struct px_battle_matchup_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    void *u = api->user;
    api->event_push(u);
    api->event_set_id(u, 0x03, m->load8(m->user, attacker + UINT32_C(0x19)));
    api->event_set_id(u, 0x04, m->load8(m->user, defender + UINT32_C(0x19)));
    api->event_set_id(u, 0x16, m->load8(m->user, move + UINT32_C(6)));
    api->event_set_value(u, 0x39, 0);
    api->event_dispatch(u, context, 0x45);
    const uint32_t code = api->event_get(u, 0x39) & UINT32_C(0xff);
    api->event_pop(u);
    return code;
}

static uint32_t field_word(uint32_t context, const struct px_pokemon_api *m)
{
    return m->load32(m->user, m->load32(m->user, context + UINT32_C(0x1c)) + UINT32_C(0x27c));
}

uint32_t px_battle_matchup(uint32_t context, uint32_t attacker, uint32_t defender, uint32_t move,
                           uint32_t show_message, const struct px_battle_matchup_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    void *u = api->user;
    if (m->load8(m->user, move + UINT32_C(6)) == PX_BATTLE_TYPELESS) return PX_BATTLE_NEUTRAL;
    const uint32_t types = px_battle_packed_types(defender, api);
    const uint32_t first = types & 0x1f, second = types >> 5 & 0x1f, third = types >> 10 & 0x1f;
    uint32_t code = px_battle_single_matchup(context, attacker, defender, m->load8(m->user, move + UINT32_C(6)),
                                             first, api);
    /* A single type with no third type skips the rest. */
    if (first != second || third != PX_BATTLE_TYPELESS) {
        if (second != PX_BATTLE_TYPELESS && second != first)
            code = px_battle_combine_effectiveness(
                code, px_battle_single_matchup(context, attacker, defender, m->load8(m->user, move + UINT32_C(6)),
                                               second, api), api->types);
        if (third != PX_BATTLE_TYPELESS && third != first && third != second)
            code = px_battle_combine_effectiveness(
                code, px_battle_single_matchup(context, attacker, defender, m->load8(m->user, move + UINT32_C(6)),
                                               third, api), api->types);
    }
    const int ground = m->load8(m->user, move + UINT32_C(6)) == 4;
    int floating = 0;
    if (code != 0) {
        if (!ground) return code;
        if (field_word(context, m) != 0) return code;
        if (api->airborne(u, context, defender, 0) == 0) return code;
        /* A floating defender is immune unless event 0x46 grounds it (key 0x42). */
        floating = 1;
        api->event_push(u);
        api->event_set_id(u, 0x03, m->load8(m->user, attacker + UINT32_C(0x19)));
        api->event_set_id(u, 0x04, m->load8(m->user, defender + UINT32_C(0x19)));
        api->event_set_value(u, 0x42, 0);
        api->event_dispatch(u, context, 0x46);
        const int grounded = api->event_get(u, 0x42) != 0;
        api->event_pop(u);
        if (grounded) return code;
    } else if (ground) {
        /* Ground against a Flying type: neutral when the field word is set or
         * the defender is not airborne. */
        if (field_word(context, m) != 0) return PX_BATTLE_NEUTRAL;
        if (api->airborne(u, context, defender, 1) == 0) return PX_BATTLE_NEUTRAL;
    }
    code = immunity_override(context, attacker, defender, move, api);
    if (code != 0 || show_message == 0) return code;
    const uint32_t extra = UINT32_C(0xffff0000);  /* literal 10826570 */
    if (!floating) {
        api->message(u, m->load32(m->user, context + UINT32_C(0xc)), 0x6a, 0xd2,
                     m->load8(m->user, defender + UINT32_C(0x19)), extra);
        return code;
    }
    const uint32_t state = context + UINT32_C(0x437c);  /* literal 10826574 */
    const uint32_t saved = api->message_state(u, state);
    api->event_push(u);
    api->event_set_id(u, 0x02, m->load8(m->user, defender + UINT32_C(0x19)));
    api->event_dispatch(u, context, 0x1b);
    api->event_pop(u);
    const uint32_t flags = m->load32(m->user, state);
    if (!(flags & UINT32_C(0x40000000)) || !(flags & UINT32_C(0x20000000)))
        api->message(u, m->load32(m->user, context + UINT32_C(0xc)), 0x6a, 0xd2,
                     m->load8(m->user, defender + UINT32_C(0x19)), extra);
    m->store32(m->user, state, saved);
    return code;
}

static uint32_t battler(uint32_t context, uint32_t index, const struct px_battle_matchup_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const uint32_t table = m->load32(m->user, context + UINT32_C(8));
    if (index >= 0x18) api->types->abort(api->user);
    const uint32_t slot = table + index * UINT32_C(4) + UINT32_C(0x84);
    if (m->load32(m->user, slot) == 0) api->types->abort(api->user);
    return m->load32(m->user, slot);
}

uint32_t px_battle_matchup_by_index(uint32_t context, uint32_t attacker_index,
                                    uint32_t defender_index, uint32_t move,
                                    const struct px_battle_matchup_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    void *u = api->user;
    const uint32_t attacker = battler(context, attacker_index, api);
    uint32_t defender = battler(context, defender_index, api);
    const uint32_t depth = context + UINT32_C(0x1080);
    m->store32(m->user, depth, m->load32(m->user, depth) + 1);
    if (m->load8(m->user, defender + UINT32_C(0x1b)) & 0x40)
        defender = api->replacement(u, m->load32(m->user, context + UINT32_C(4)),
                                    m->load32(m->user, context + UINT32_C(8)), defender);
    api->load_move(u, context, move, attacker, api->move_scratch);
    const uint32_t code = px_battle_matchup(context, attacker, defender, api->move_scratch, 0, api);
    m->store32(m->user, depth, m->load32(m->user, depth) - 1);
    return code;
}
