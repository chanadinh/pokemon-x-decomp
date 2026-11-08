#include "battle_moves_3.h"

enum { ATTACKER = 0x03, DEFENDER = 0x04, DAMAGE = 0x38 };

static void *user(const struct px_battle_moves_3_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_3_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_3_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t attacking(uint32_t id, const struct px_battle_moves_3_api *api)
{
    return get(ATTACKER, api) == id;
}

static uint32_t battler(uint32_t context, uint32_t id, const struct px_battle_moves_3_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static uint32_t value(uint32_t pokemon, uint32_t key, const struct px_battle_moves_3_api *api)
{
    return api->base->pokemon_value(user(api), pokemon, key);
}

static void set(uint32_t key, uint32_t v, const struct px_battle_moves_3_api *api)
{
    px_battle_event_rewrite(key, v, api->base->events);
}

void px_battle_counter_damage(uint32_t context, uint32_t id, uint32_t work, uint32_t kind, uint32_t factor,
                              const struct px_battle_moves_3_api *api)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    const uint32_t out = api->record_scratch;
    if (!attacking(id, api)) return;
    const uint32_t attacker = battler(context, id, api);
    const uint32_t own = m->load8(m->user, attacker + UINT32_C(0x19));
    for (uint32_t next = 0;;) {
        const uint32_t index = next;
        next = (next + 1) & 0xff;
        if (api->damage_record(user(api), attacker, 0, index, out) == 0) return;
        if (api->is_ally(user(api), own, m->load8(m->user, out + 5)) != 0) continue;
        if (kind != 0 && api->base->move_category(user(api), m->load16(m->user, out)) != kind) continue;
        set(DAMAGE, px_battle_modify(m->load16(m->user, out + 2), factor), api);
        return;
    }
}

void px_battle_set_handler_word(uint32_t handler, uint32_t v, const struct px_pokemon_api *m)
{
    m->store32(m->user, handler + UINT32_C(0xc), v);
}

void px_battle_clear_handler_word(uint32_t handler, const struct px_pokemon_api *m)
{
    m->store32(m->user, handler + UINT32_C(0xc), 0);
}

PX_BATTLE_MOVES_3_HANDLER(px_battle_sonic_boom)
{
    (void)handler, (void)context, (void)work;
    if (attacking(id, api)) set(DAMAGE, 20, api);
}

PX_BATTLE_MOVES_3_HANDLER(px_battle_dragon_rage)
{
    (void)handler, (void)context, (void)work;
    if (attacking(id, api)) set(DAMAGE, 40, api);
}

/* The attacker's level (low byte). */
PX_BATTLE_MOVES_3_HANDLER(px_battle_seismic_toss)
{
    (void)handler, (void)work;
    if (attacking(id, api)) set(DAMAGE, value(battler(context, id, api), 0xf, api) & 0xff, api);
}

/* level * (random(101) + 50) / 100 (literal 0x51eb851f, shift 5), 16 bits, at least 1. */
PX_BATTLE_MOVES_3_HANDLER(px_battle_psywave)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t level = value(battler(context, id, api), 0xf, api) & 0xff;
    const uint32_t scale = (api->random(user(api), 0x65) + 50) & 0xffff;
    const uint32_t damage = level * scale / 100 & 0xffff;
    set(DAMAGE, damage != 0 ? damage : 1, api);
}

/* Half the defender's HP (signed, toward zero), 16 bits, at least 1. */
PX_BATTLE_MOVES_3_HANDLER(px_battle_super_fang)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t hp = value(battler(context, get(DEFENDER, api) & 0xff, api), 0xd, api);
    const uint32_t damage = (hp + (hp >> 31)) >> 1 & 0xffff;
    set(DAMAGE, damage != 0 ? damage : 1, api);
}

/* The defender's HP minus the attacker's; 1 when negative. */
PX_BATTLE_MOVES_3_HANDLER(px_battle_endeavor)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t attacker = battler(context, id, api);
    const uint32_t target = battler(context, get(DEFENDER, api) & 0xff, api);
    const uint32_t remaining = value(target, 0xd, api);
    const uint32_t difference = remaining - value(attacker, 0xd, api);
    set(DAMAGE, (int32_t)difference < 0 ? 1 : difference, api);
}

/* The attacker's HP. */
PX_BATTLE_MOVES_3_HANDLER(px_battle_final_gambit)
{
    (void)handler, (void)work;
    if (attacking(id, api)) set(DAMAGE, value(battler(context, id, api), 0xd, api), api);
}

/* From the second turn (work word 0, signed): twice the stored damage (word 1);
 * then word 0 becomes 3. */
PX_BATTLE_MOVES_3_HANDLER(px_battle_bide_damage)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    if ((int32_t)m->load32(m->user, work) < 2) return;
    (void)battler(context, id, api);   /* looked up, result unused */
    const uint32_t stored = m->load32(m->user, work + 4);
    set(DAMAGE, stored != 0 ? stored << 1 : stored, api);
    m->store32(m->user, work, 3);
}

PX_BATTLE_MOVES_3_HANDLER(px_battle_counter) { (void)handler; px_battle_counter_damage(context, id, work, 1, 0x2000, api); }
PX_BATTLE_MOVES_3_HANDLER(px_battle_mirror_coat) { (void)handler; px_battle_counter_damage(context, id, work, 2, 0x2000, api); }
PX_BATTLE_MOVES_3_HANDLER(px_battle_metal_burst) { (void)handler; px_battle_counter_damage(context, id, work, 0, 0x1800, api); }

/* Brick Break stores its hook in handler word +0xc and clears it after the damage. */
PX_BATTLE_MOVES_3_HANDLER(px_battle_brick_break_mark)
{
    (void)context, (void)work;
    if (attacking(id, api)) px_battle_set_handler_word(handler, api->brick_break_hook, memory(api));
}

PX_BATTLE_MOVES_3_HANDLER(px_battle_brick_break_clear)
{
    (void)context, (void)work;
    if (attacking(id, api)) px_battle_clear_handler_word(handler, memory(api));
}

/* Event 0x43: key 0x53 = 1. */
PX_BATTLE_MOVES_3_HANDLER(px_battle_thunder_wave_check)
{
    (void)handler, (void)context, (void)work;
    if (attacking(id, api)) set(0x53, 1, api);
}

PX_BATTLE_MOVES_3_HANDLER(px_battle_thousand_arrows_check)
{
    (void)handler, (void)context, (void)work;
    if (attacking(id, api)) set(0x53, 1, api);
}

/* Against a Water defending type the code becomes 8 (super effective). */
PX_BATTLE_MOVES_3_HANDLER(px_battle_freeze_dry)
{
    (void)handler, (void)context, (void)work;
    if (!attacking(id, api)) return;
    if (get(0x15, api) == 10) set(0x39, 8, api);
}

/* Flying as the second attacking type. */
PX_BATTLE_MOVES_3_HANDLER(px_battle_flying_press)
{
    (void)handler, (void)context, (void)work;
    if (attacking(id, api)) set(0x17, 2, api);
}

/* A Ground move's immunity becomes neutral (7). */
PX_BATTLE_MOVES_3_HANDLER(px_battle_thousand_arrows_immunity)
{
    (void)handler, (void)context, (void)work;
    if (!attacking(id, api)) return;
    if (get(0x16, api) == 4) set(0x39, 7, api);
}

/* An airborne defender keeps the code (key 0x42 = 1). */
PX_BATTLE_MOVES_3_HANDLER(px_battle_thousand_arrows_airborne)
{
    (void)handler, (void)context, (void)work;
    if (attacking(id, api)) set(0x42, 1, api);
}

/* Event 0x38: key 0x4c = 1. */
PX_BATTLE_MOVES_3_HANDLER(px_battle_chip_away_matchup)
{
    (void)handler, (void)context, (void)work;
    if (attacking(id, api)) set(0x4c, 1, api);
}
