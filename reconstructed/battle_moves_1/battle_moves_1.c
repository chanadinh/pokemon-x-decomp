#include "battle_moves_1.h"
#include "../battle_modifier/battle_modifier.h"

enum { ATTACKER = 0x03, DEFENDER = 0x04, POWER = 0x31 };

static void *user(const struct px_battle_moves_1_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_1_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_1_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t attacking(uint32_t id, const struct px_battle_moves_1_api *api)
{
    return get(ATTACKER, api) == id;
}

static uint32_t battler(uint32_t context, uint32_t id, const struct px_battle_moves_1_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static uint32_t defender(uint32_t context, const struct px_battle_moves_1_api *api)
{
    return battler(context, get(DEFENDER, api) & 0xff, api);
}

static uint32_t value(uint32_t pokemon, uint32_t key, const struct px_battle_moves_1_api *api)
{
    return api->base->pokemon_value(user(api), pokemon, key);
}

static void set_power(uint32_t power, const struct px_battle_moves_1_api *api)
{
    px_battle_event_rewrite(POWER, power, api->base->events);
}

static void double_power(const struct px_battle_moves_1_api *api)
{
    set_power(get(POWER, api) << 1, api);
}

/* The divide veneers 1080041c (unsigned) and 10800a34 (signed) are represented
 * by C division; a zero divisor (and INT32_MIN / -1) is outside the host's domain. */
static int32_t sdiv(uint32_t a, uint32_t b)
{
    return (int32_t)a / (int32_t)b;
}

/* Doubled power against a defender in some condition. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_smelling_salts)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if (api->has_status(user(api), defender(context, api), 1) != 0) double_power(api);
}

PX_BATTLE_MOVES_1_HANDLER(px_battle_wake_up_slap)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if (api->has_status(user(api), defender(context, api), 2) != 0) double_power(api);
}

PX_BATTLE_MOVES_1_HANDLER(px_battle_hex)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if (api->base->pokemon_status(user(api), defender(context, api)) != 0) double_power(api);
}

PX_BATTLE_MOVES_1_HANDLER(px_battle_assurance)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if (api->turn_flag(user(api), defender(context, api), 2) != 0) double_power(api);
}

PX_BATTLE_MOVES_1_HANDLER(px_battle_payback)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if (api->turn_flag(user(api), defender(context, api), 1) != 0) double_power(api);
}

PX_BATTLE_MOVES_1_HANDLER(px_battle_gust)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if (px_battle_pokemon_state(defender(context, api), 3, memory(api)) != 0) double_power(api);
}

PX_BATTLE_MOVES_1_HANDLER(px_battle_acrobatics)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    if (m->load16(m->user, battler(context, id, api) + UINT32_C(0x12)) == 0) double_power(api);
}

PX_BATTLE_MOVES_1_HANDLER(px_battle_pursuit)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if (api->target_switching(user(api), context) != 0) double_power(api);
}

PX_BATTLE_MOVES_1_HANDLER(px_battle_weather_ball)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if (api->weather(user(api), context) != 0) double_power(api);
}

/* Doubled when a damage record of the attacker names the defender with damage. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_revenge)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    const uint32_t out = api->record_scratch;
    if (!attacking(id, api)) return;
    const uint32_t attacker = battler(context, id, api);
    const uint32_t target = get(DEFENDER, api) & 0xff;
    uint32_t index = 0, next = 1;
    while (api->damage_record(user(api), attacker, 0, index, out) != 0) {
        if (m->load8(m->user, out + 5) == target && m->load16(m->user, out + 2) != 0) {
            double_power(api);
            return;
        }
        index = next;
        next = (next + 1) & 0xff;
    }
}

/* Doubled when more than one battler used the move this turn. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_round)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    const uint32_t history = context + UINT32_C(0x20);
    const uint32_t move = m->load16(m->user, handler + UINT32_C(0x38));
    if (api->move_users(user(api), history, move, api->turn(user(api), context)) > 1) double_power(api);
}

/* Friendship * 10 / 25 (literal 0x51eb851f, shift 3), at least 1. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_return)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    const uint32_t friendship = api->friendship(user(api), m->load32(m->user, battler(context, id, api)));
    const uint32_t power = friendship * 10 / 25;
    set_power(power != 0 ? power : 1, api);
}

PX_BATTLE_MOVES_1_HANDLER(px_battle_frustration)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    const uint32_t friendship = api->friendship(user(api), m->load32(m->user, battler(context, id, api)));
    const uint32_t power = (0xff - friendship) * 10 / 25;
    set_power(power != 0 ? power : 1, api);
}

/* Stat stages 1..7 in the orders of the tables 108d2c3f and 108d2c4d. */
static const uint8_t punishment_stats[7] = {1, 2, 3, 4, 5, 6, 7};
static const uint8_t stored_power_stats[7] = {1, 2, 5, 3, 4, 6, 7};

/* 60 + 20 per raised stage of the defender, at most 200. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_punishment)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t target = defender(context, api);
    uint32_t raised = 0;
    for (uint32_t i = 0; i < 7; ++i) {
        const uint32_t stage = value(target, punishment_stats[i], api);
        if ((int32_t)stage > 6) raised = stage + raised - 6;
    }
    const uint32_t power = 60 + raised * 20;
    set_power(power > 200 ? 200 : power, api);
}

/* Power + 20 per raised stage of the attacker; unchanged with none raised. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_stored_power)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t attacker = battler(context, id, api);
    uint32_t raised = 0;
    for (uint32_t i = 0; i < 7; ++i) {
        const uint32_t above = value(attacker, stored_power_stats[i], api) - 6;
        if ((int32_t)above > 0) raised += above;
    }
    if (raised == 0) return;
    set_power(get(POWER, api) + raised * 20, api);
}

/* Table 108d494c: (48 * HP / max HP limit, power) pairs. */
static const uint16_t flail_table[6][2] = {{1, 200}, {4, 150}, {9, 100}, {16, 80}, {32, 40}, {48, 20}};

PX_BATTLE_MOVES_1_HANDLER(px_battle_flail)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t attacker = battler(context, id, api);
    const uint32_t max_hp = value(attacker, 0xe, api);
    uint32_t ratio = (uint32_t)sdiv(value(attacker, 0xd, api) * 48, max_hp);
    if (ratio == 0) ratio = 1;
    uint32_t i = 0;
    while (flail_table[i][0] < ratio && ++i < 5) {
    }
    set_power(flail_table[i][1], api);
}

/* 120 times the defender's HP percentage (fixed point), / 100, at least 1. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_wring_out)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t percent = api->hp_percent(user(api), defender(context, api));
    const uint32_t power = api->fixed_multiply(user(api), 120, percent) / 100;
    set_power(power != 0 ? power : 1, api);
}

/* 25 * defender speed / attacker speed + 1, at most 150. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_gyro_ball)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t attacker = battler(context, id, api);
    const uint32_t target = defender(context, api);
    const uint32_t own = api->speed(user(api), context, attacker, 0);
    const uint32_t other = api->speed(user(api), context, target, 0);
    const uint32_t power = other * 25 / own + 1;
    set_power(power > 150 ? 150 : power, api);
}

/* Attacker speed / defender speed: 4 or more 150, 3 120, 2 80, 1 60, else 40. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_electro_ball)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t attacker = battler(context, id, api);
    const uint32_t target = defender(context, api);
    const uint32_t own = api->speed(user(api), context, attacker, 0);
    const int32_t ratio = sdiv(own, api->speed(user(api), context, target, 0));
    set_power(ratio >= 4 ? 150 : ratio >= 3 ? 120 : ratio >= 2 ? 80 : ratio >= 1 ? 60 : 40, api);
}

/* The defender's weight (0.1 kg units). */
PX_BATTLE_MOVES_1_HANDLER(px_battle_low_kick)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    const uint32_t w = api->weight(user(api), context, m->load8(m->user, defender(context, api) + UINT32_C(0x19)));
    set_power(w >= 2000 ? 120 : w >= 1000 ? 100 : w >= 500 ? 80 : w >= 250 ? 60 : w >= 100 ? 40 : 20, api);
}

/* Attacker weight / defender weight: 5 or more 120, 4 100, 3 80, 2 60, else 40. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_heavy_slam)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    (void)battler(context, id, api);   /* looked up, result unused */
    const uint32_t target = defender(context, api);
    const uint32_t own = api->weight(user(api), context, id);
    const int32_t ratio = sdiv(own, api->weight(user(api), context, m->load8(m->user, target + UINT32_C(0x19))));
    set_power(ratio >= 5 ? 120 : ratio == 4 ? 100 : ratio == 3 ? 80 : ratio == 2 ? 60 : 40, api);
}

/* Table 108d49ac: power by remaining PP 0..4. */
static const uint16_t trump_card_table[5] = {200, 80, 60, 50, 40};

PX_BATTLE_MOVES_1_HANDLER(px_battle_trump_card)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    const uint32_t attacker = battler(context, id, api);
    const uint32_t slot = api->move_slot(user(api), attacker, m->load16(m->user, handler + UINT32_C(0x38)));
    if (slot == 4) {
        set_power(40, api);
        return;
    }
    uint32_t pp = api->move_pp(user(api), attacker, slot) & 0xff;
    if (pp >= 5) pp = 4;
    set_power(trump_card_table[pp], api);
}

/* 100 per stockpile (a signed 16-bit product). */
PX_BATTLE_MOVES_1_HANDLER(px_battle_spit_up)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t count = api->stockpile(user(api), battler(context, id, api), 0);
    set_power((uint32_t)((int32_t)(int16_t)count * 100), api);
}

/* Parameters 7 and 6 of the held item. */
static void item_power(uint32_t context, uint32_t id, uint32_t param, const struct px_battle_moves_1_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    const uint32_t item = m->load16(m->user, battler(context, id, api) + UINT32_C(0x12));
    set_power(api->item_param(user(api), item, param), api);
}

PX_BATTLE_MOVES_1_HANDLER(px_battle_natural_gift) { (void)handler, (void)work; item_power(context, id, 7, api); }
PX_BATTLE_MOVES_1_HANDLER(px_battle_fling) { (void)handler, (void)work; item_power(context, id, 6, api); }

/* The next eligible party member (the user, or any that can battle without a
 * status) after skipping work word 0 of them; base Attack / 10 + 5. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_beat_up)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    uint32_t skip = m->load32(m->user, work) & 0xff;
    const uint32_t party = api->party(user(api), context, id);
    const uint32_t count = m->load8(m->user, party + UINT32_C(0x18));
    uint32_t member = 0;
    if (count > skip && count != 0) {
        for (uint32_t i = 0; i < count; ++i) {
            const uint32_t candidate = api->party_member(user(api), party, i & 0xff);
            if (m->load8(m->user, candidate + UINT32_C(0x19)) != id) {
                if (api->can_battle(user(api), candidate) == 0) continue;
                if (api->base->pokemon_status(user(api), candidate) != 0) continue;
            }
            if (skip != 0) {
                skip = (skip - 1) & 0xff;
                continue;
            }
            member = candidate;
            break;
        }
    }
    if (member == 0) member = battler(context, id, api);
    const uint32_t next = m->load32(m->user, work) + 1;
    m->store32(m->user, work, next >= 6 ? 0 : next);
    set_power(value(member, 0x14, api) / 10 + 5, api);
}

/* Consecutive earlier turns in which the move was used: 0 40, 1 80, 2 120, 3 160, more 200. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_echoed_voice)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    const uint32_t history = context + UINT32_C(0x20);
    const uint32_t move = m->load16(m->user, handler + UINT32_C(0x38));
    int32_t turn = (int32_t)(api->turn(user(api), context) - 1);
    uint32_t streak = 0;
    while (turn >= 0) {
        if (api->move_used(user(api), history, move, (uint32_t)turn--) == 0) break;
        ++streak;
    }
    set_power(streak == 0 ? 40 : streak == 1 ? 80 : streak == 2 ? 120 : streak == 3 ? 160 : 200, api);
}

/* Doubled for each consecutive use, at most 160; only when the last move was this one. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_fury_cutter)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    const uint32_t attacker = battler(context, id, api);
    if (api->last_move(user(api), attacker) != m->load16(m->user, handler + UINT32_C(0x38))) return;
    uint32_t power = get(POWER, api);
    const uint32_t uses = api->use_streak(user(api), attacker);
    for (uint32_t i = 0; i < uses; ++i) {
        power <<= 1;
        if (power > 160) {
            power = 160;
            break;
        }
    }
    set_power(power, api);
}

/* Power * 2^(work word 0), doubled after Defense Curl; the move's handlers are
 * removed once work word 0 reaches work word 1. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_rollout)
{
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    const uint32_t attacker = battler(context, id, api);
    uint32_t hits = m->load32(m->user, work), factor = 1;
    if (hits != 0) {
        if (hits & 1) factor = 2;
        for (hits >>= 1; hits != 0; --hits) factor <<= 2;
    }
    if (px_battle_pokemon_state(attacker, 7, m) != 0) factor <<= 1;
    set_power(get(POWER, api) * factor, api);
    const uint32_t done = m->load32(m->user, work) + 1, limit = m->load32(m->user, work + 4);
    m->store32(m->user, work, done);
    if ((int32_t)limit > (int32_t)done) return;
    const uint32_t move = m->load16(m->user, handler + UINT32_C(0x38));
    for (uint32_t h = px_battle_handler_find(0, id, api->handlers); h != 0;) {
        if (m->load16(m->user, h + UINT32_C(0x38)) == move) {
            px_battle_handler_remove(h, api->handlers);
            h = px_battle_handler_find(0, id, api->handlers);
        } else {
            h = px_battle_handler_find_next(h, api->handlers);
        }
    }
}

/* Work word 0 counts the hits: 10 per hit. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_triple_kick)
{
    (void)handler, (void)context;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    const uint32_t hits = m->load32(m->user, work) + 1;
    m->store32(m->user, work, hits);
    set_power(hits * 10, api);
}

/* Work word 0 holds the power chosen earlier; 10 when unset. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_magnitude)
{
    (void)handler, (void)context;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    uint32_t power = m->load32(m->user, work);
    if (power == 0) {
        power = 10;
        m->store32(m->user, work, power);
    }
    set_power(power, api);
}

/* random(80): below 40 power 40, below 70 power 80, else 120. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_present)
{
    (void)handler, (void)context, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t r = api->random(user(api), 80);
    set_power(r < 40 ? 40 : r < 70 ? 80 : 120, api);
}

/* 150 when work word 0 marks a combined pledge. */
PX_BATTLE_MOVES_1_HANDLER(px_battle_pledge)
{
    (void)handler, (void)context;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    if (m->load32(m->user, work) != 0) set_power(150, api);
}
