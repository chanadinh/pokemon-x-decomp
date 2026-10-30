#include "battle_damage.h"

uint32_t px_battle_apply_modifier(uint32_t value, uint32_t modifier)
{
    const uint32_t product = value * modifier;
    return (product >> 12) + ((product & UINT32_C(0xfff)) > UINT32_C(0x800));
}

uint32_t px_battle_calculate_damage(const struct px_battle_damage_args *a, const struct px_battle_damage_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    void *u = api->user;
    const uint32_t move_id = m->load16(m->user, a->move);
    const uint32_t category = api->move_category(u, move_id);
    uint32_t fixed = 0;
    uint32_t damage;

    /* Event 0x4e may supply fixed damage, which ends the calculation. */
    api->event_push(u);
    api->event_set_id(u, 0x39, a->effectiveness);
    api->event_set_id(u, 0x03, m->load8(m->user, a->attacker + UINT32_C(0x19)));
    api->event_set_id(u, 0x04, m->load8(m->user, a->defender + UINT32_C(0x19)));
    api->event_set_id(u, 0x46, a->critical);
    api->event_set_id(u, 0x16, m->load8(m->user, a->move + UINT32_C(6)));
    api->event_set_id(u, 0x12, m->load16(m->user, a->move));
    api->event_set_id(u, 0x1b, category);
    api->event_set_id(u, 0x47, a->extra);
    api->event_set_scaled(u, 0x38, 0);
    api->event_dispatch(u, a->context, 0x4e);
    damage = api->event_get(u, 0x38);
    if (damage != 0) {
        fixed = 1;
        goto finish;
    }

    /* Base power through events 0x3c/0x3d, then the 4096-based power modifier. */
    {
        const uint32_t base_power = api->move_power(u, m->load16(m->user, a->move));
        api->event_push(u);
        api->event_set_id(u, 0x03, m->load8(m->user, a->attacker + UINT32_C(0x19)));
        api->event_set_id(u, 0x04, m->load8(m->user, a->defender + UINT32_C(0x19)));
        api->event_set_id(u, 0x12, m->load16(m->user, a->move));
        api->event_set_id(u, 0x16, m->load8(m->user, a->move + UINT32_C(6)));
        api->event_set_id(u, 0x1b, m->load8(m->user, a->move + UINT32_C(7)));
        api->event_set_value(u, 0x57, 0);
        api->event_set_value(u, 0x58, 0);
        api->event_set_value(u, 0x31, base_power);
        api->event_dispatch(u, a->context, 0x3c);
        api->event_set_multiplier(u, 0x32, 0x1000, 0x29, 0x200000);
        api->event_dispatch(u, a->context, 0x3d);
        const uint32_t power_value = api->event_get(u, 0x31) & UINT32_C(0xffff);
        const uint32_t power_modifier = api->event_get(u, 0x32);
        api->event_pop(u);
        const uint32_t power = px_battle_apply_modifier(power_value, power_modifier) & UINT32_C(0xffff);

        const uint32_t attack = api->attack(u, a->context, a->attacker, a->defender, a->move, a->critical,
                                            api->flag_scratch);
        const uint32_t defense = api->defense(u, a->context, a->attacker, a->defender, a->move, a->critical);
        const uint32_t level = api->pokemon_value(u, a->attacker, 0x0f) & UINT32_C(0xff);
        const uint32_t numerator = ((level * UINT32_C(2)) / UINT32_C(5) + UINT32_C(2)) * (power * attack);
        damage = (numerator / defense) / UINT32_C(50) + UINT32_C(2);
    }
    if (a->spread != UINT32_C(0x1000)) damage = px_battle_apply_modifier(damage, a->spread);

    /* Weather from event 0x83 (suppressed when key 0x42 is set): sun (1) and
     * rain (2) scale fire (type 9) and water (type 10) moves by 1.5 or 0.5. */
    {
        uint32_t weather;
        api->event_push(u);
        api->event_set_value(u, 0x42, 0);
        api->event_dispatch(u, a->context, 0x83);
        if (api->event_get(u, 0x42) != 0) {
            api->event_pop(u);
            weather = 0;
        } else {
            api->event_pop(u);
            weather = m->load8(m->user, m->load32(m->user, a->context + UINT32_C(0x1c)) + UINT32_C(1));
        }
        const uint32_t type = m->load8(m->user, a->move + UINT32_C(6));
        uint32_t modifier = 0;
        if (weather == 1) modifier = type == 9 ? 0x1800 : type == 10 ? 0x800 : 0;
        else if (weather == 2) modifier = type == 9 ? 0x800 : type == 10 ? 0x1800 : 0;
        if (modifier) damage = px_battle_apply_modifier(damage, modifier);
    }
    if (a->critical != 0) damage = (damage * UINT32_C(15)) / UINT32_C(10);

    /* Random roll 85..100 unless rule flag 7 disables it. */
    if (api->rule_flag(u, m->load32(m->user, m->load32(m->user, a->context + UINT32_C(4)) + UINT32_C(0x10)), 7) == 0) {
        uint32_t roll;
        if (a->fixed_roll != 0) {
            roll = 0x55;
        } else {
            const uint32_t generator = m->load32(m->user, api->random_owner + UINT32_C(0x54));
            roll = (UINT32_C(100) - api->random(u, generator, 0x10)) & UINT32_C(0xffff);
        }
        damage = (damage * roll) / UINT32_C(100);
    }

    /* Same-type bonus through events 0x48/0x49, except for type 0x12: key 0x53
     * starts as whether the attacker has the move's type. */
    const uint32_t move_type = m->load8(m->user, a->move + UINT32_C(6));
    if (move_type != 0x12) {
        const uint32_t types = api->has_type(u, a->attacker, move_type);
        api->event_push(u);
        api->event_set_id(u, 0x02, m->load8(m->user, a->attacker + UINT32_C(0x19)));
        api->event_set_value(u, 0x53, types);
        api->event_dispatch(u, a->context, 0x48);
        const uint32_t same_type = api->event_get(u, 0x53) != 0;
        api->event_set_id(u, 0x45, same_type);
        api->event_set_scaled(u, 0x36, same_type ? 0x1800 : 0x1000);
        api->event_dispatch(u, a->context, 0x49);
        const uint32_t bonus = api->event_get(u, 0x36);
        api->event_pop(u);
        damage = px_battle_apply_modifier(damage, bonus);
    }
    damage = api->apply_effectiveness(u, damage, a->effectiveness);

    /* Burn halves physical damage unless attack() reported an exception. */
    if (category == 1 && api->pokemon_status(u, a->attacker) == 4 && m->load8(m->user, api->flag_scratch) == 0)
        damage = (damage * UINT32_C(50)) / UINT32_C(100);

    /* Final modifiers through event 0x4f; damage is at least one. */
    api->event_set_multiplier(u, 0x36, 0x1000, 0x29, 0x20000);
    api->event_set_scaled(u, 0x33, damage);
    api->event_dispatch(u, a->context, 0x4f);
    {
        const uint32_t modifier = api->event_get(u, 0x36);
        const uint32_t value = api->event_get(u, 0x33);
        damage = px_battle_apply_modifier(value, modifier);
    }
    if (damage == 0) damage = 1;

finish:
    api->event_dispatch(u, a->context, 0x50);
    api->event_pop(u);
    m->store16(m->user, a->output, (uint16_t)damage);
    return fixed;
}
