#include "battle_stats.h"

static uint32_t modifier(uint32_t value, uint32_t multiplier)
{
    const uint32_t product = value * multiplier;
    return (product >> 12) + ((product & UINT32_C(0xfff)) > UINT32_C(0x800));
}

static int8_t stage(uint32_t pokemon, uint32_t offset, const struct px_pokemon_api *m)
{
    const uint8_t raw = m->load8(m->user, pokemon + offset);
    return raw < 0x80 ? (int8_t)raw : (int8_t)(raw - 0x100);
}

/* Shared stage choice: a critical hit ignores lowered offensive stages
 * (keys 8/10 below neutral) and raised defensive stages (keys 9/11 above). */
static uint32_t staged_or_raw(uint32_t pokemon, uint32_t key, uint32_t ignore_stages, uint32_t critical,
                              const struct px_battle_stats_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    int raw = ignore_stages != 0;
    if (!raw && critical != 0) {
        switch (api->stage_key(api->user, pokemon, key)) {
        case 8: raw = stage(pokemon, 0x104, m) < 6; break;
        case 9: raw = stage(pokemon, 0x105, m) > 6; break;
        case 10: raw = stage(pokemon, 0x106, m) < 6; break;
        case 11: raw = stage(pokemon, 0x107, m) > 6; break;
        default: break;
        }
    }
    return (raw ? api->raw_stat(api->user, pokemon, key) : api->stat(api->user, pokemon, key)) & UINT32_C(0xffff);
}

uint32_t px_battle_attack_value(uint32_t context, uint32_t attacker, uint32_t defender, uint32_t move,
                                uint32_t critical, uint32_t flag, const struct px_battle_stats_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    void *u = api->user;
    const uint32_t key = api->move_category(u, m->load16(m->user, move)) == 2 ? 10 : 8;
    api->event_push(u);
    api->event_set_id(u, 0x03, m->load8(m->user, attacker + UINT32_C(0x19)));
    api->event_set_id(u, 0x04, m->load8(m->user, defender + UINT32_C(0x19)));
    api->event_set_scaled(u, 0x3c, 0x1f);
    api->event_set_scaled(u, 0x53, 0);
    api->event_dispatch(u, context, 0x3e);
    /* Event 0x3e may name another battler (index 0..23) whose stat is used. */
    const uint32_t index = api->event_get(u, 0x3c) & UINT32_C(0xff);
    uint32_t source = attacker;
    if (index != 0x1f) {
        const uint32_t table = m->load32(m->user, context + UINT32_C(8));
        if (index >= 0x18) api->abort(u);
        const uint32_t slot = table + index * UINT32_C(4) + UINT32_C(0x84);
        if (m->load32(m->user, slot) == 0) api->abort(u);
        source = m->load32(m->user, slot);
    }
    const uint32_t value = staged_or_raw(source, key, api->event_get(u, 0x53), critical, api);
    api->event_set_id(u, 0x12, m->load16(m->user, move));
    api->event_set_id(u, 0x16, m->load8(m->user, move + UINT32_C(6)));
    api->event_set_id(u, 0x1b, m->load8(m->user, move + UINT32_C(7)));
    api->event_set_scaled(u, 0x34, value);
    api->event_set_multiplier(u, 0x36, 0x1000, 0x19a, 0x20000);
    api->event_set_value(u, 0x54, 0);
    api->event_dispatch(u, context, 0x40);
    const uint32_t scaled = api->event_get(u, 0x34) & UINT32_C(0xffff);
    const uint32_t result = modifier(scaled, api->event_get(u, 0x36)) & UINT32_C(0xffff);
    m->store8(m->user, flag, api->event_get(u, 0x54) != 0);
    api->event_pop(u);
    return result;
}

uint32_t px_battle_defense_value(uint32_t context, uint32_t attacker, uint32_t defender, uint32_t move,
                                 uint32_t critical, const struct px_battle_stats_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    void *u = api->user;
    const uint32_t special = api->move_category(u, m->load16(m->user, move)) == 2;
    uint32_t category = m->load8(m->user, move + UINT32_C(7));
    uint32_t key = special ? 11 : 9;
    api->event_push(u);
    api->event_set_id(u, 0x03, m->load8(m->user, attacker + UINT32_C(0x19)));
    api->event_set_id(u, 0x04, m->load8(m->user, defender + UINT32_C(0x19)));
    api->event_set_id(u, 0x3d, key);
    api->event_set_scaled(u, 0x3e, 0);
    api->event_set_value(u, 0x53, 0);
    api->event_dispatch(u, context, 0x3f);
    /* An odd key 0x3e swaps defense and special defense, with the value of key 0x1b. */
    if (api->event_get(u, 0x3e) & 1) {
        if (key == 9) { key = 11; category = 2; } else { key = 9; category = 1; }
    }
    const uint32_t ignore = api->event_get(u, 0x53) & UINT32_C(0xff);
    api->event_pop(u);
    uint32_t value = staged_or_raw(defender, key, ignore, critical, api);

    /* Sandstorm (weather 4, not suppressed) raises a rock-type defender's special defense. */
    api->event_push(u);
    api->event_set_value(u, 0x42, 0);
    api->event_dispatch(u, context, 0x83);
    if (api->event_get(u, 0x42) != 0) {
        api->event_pop(u);
    } else {
        api->event_pop(u);
        if (m->load8(m->user, m->load32(m->user, context + UINT32_C(0x1c)) + UINT32_C(1)) == 4 &&
            api->has_type(u, defender, 5) != 0 && key == 11)
            value = modifier(value, 0x1800) & UINT32_C(0xffff);
    }

    api->event_push(u);
    api->event_set_id(u, 0x03, m->load8(m->user, attacker + UINT32_C(0x19)));
    api->event_set_id(u, 0x04, m->load8(m->user, defender + UINT32_C(0x19)));
    api->event_set_id(u, 0x12, m->load16(m->user, move));
    api->event_set_id(u, 0x16, m->load8(m->user, move + UINT32_C(6)));
    api->event_set_id(u, 0x1b, category);
    api->event_set_scaled(u, 0x35, value);
    api->event_set_multiplier(u, 0x36, 0x1000, 0x19a, 0x20000);
    api->event_dispatch(u, context, 0x41);
    const uint32_t scaled = api->event_get(u, 0x35) & UINT32_C(0xffff);
    const uint32_t multiplier = api->event_get(u, 0x36);
    api->event_pop(u);
    return modifier(scaled, multiplier) & UINT32_C(0xffff);
}
