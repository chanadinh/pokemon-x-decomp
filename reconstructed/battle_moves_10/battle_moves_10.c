#include "battle_moves_10.h"
#include "../battle_modifier/battle_modifier.h"

enum { BATTLER = 0x02, MOVE = 0x12, RESULT = 0x23, FAILS = 0x1b };

static void *user(const struct px_battle_moves_10_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_10_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_10_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t moving(uint32_t id, const struct px_battle_moves_10_api *api)
{
    return get(BATTLER, api) == id;
}

static uint32_t battler(uint32_t context, uint32_t id, const struct px_battle_moves_10_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static uint32_t result(uint32_t code, const struct px_battle_moves_10_api *api)
{
    return px_battle_event_rewrite(RESULT, code, api->base->events);
}

static uint32_t item_of(uint32_t pokemon, const struct px_battle_moves_10_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load16(m->user, pokemon + UINT32_C(0x12));
}

void px_battle_counter_check(uint32_t context, uint32_t id, uint32_t work, uint32_t kind,
                             const struct px_battle_moves_10_api *api)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    const uint32_t out = api->record_scratch;
    if (!moving(id, api)) return;
    const uint32_t pokemon = battler(context, id, api);
    const uint32_t own = m->load8(m->user, pokemon + UINT32_C(0x19));
    for (uint32_t next = 0;;) {
        const uint32_t index = next;
        next = (next + 1) & 0xff;
        if (api->damage_record(user(api), pokemon, 0, index, out) == 0) {
            result(FAILS, api);
            return;
        }
        if (api->is_ally(user(api), own, m->load8(m->user, out + 5)) != 0) continue;
        if (kind != 0 && api->base->move_category(user(api), m->load16(m->user, out)) != kind) continue;
        return;
    }
}

PX_BATTLE_MOVES_10_HANDLER(px_battle_counter_ready) { (void)handler; px_battle_counter_check(context, id, work, 1, api); }
PX_BATTLE_MOVES_10_HANDLER(px_battle_mirror_coat_ready) { (void)handler; px_battle_counter_check(context, id, work, 2, api); }
PX_BATTLE_MOVES_10_HANDLER(px_battle_metal_burst_ready) { (void)handler; px_battle_counter_check(context, id, work, 0, api); }

/* Last Resort (literal 108b11a0, 387) must be known, the battler must know at
 * least two moves, and every other move must have been used. */
PX_BATTLE_MOVES_10_HANDLER(px_battle_last_resort_ready)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!moving(id, api)) return;
    const uint32_t pokemon = battler(context, id, api);
    const uint32_t count = m->load8(m->user, pokemon + UINT32_C(0x152));
    uint32_t used = 0, known = 0;
    for (uint32_t slot = 0; slot < count; slot = (slot + 1) & 0xff) {
        if (api->move_at(user(api), pokemon, slot) == 387) {
            known = 1;
            continue;
        }
        if (api->move_used(user(api), pokemon, slot) != 0) used = (used + 1) & 0xff;
    }
    if (known != 0 && count >= 2 && (int32_t)used >= (int32_t)(count - 1)) return;
    result(FAILS, api);
}

/* From the second turn (word 0, signed): fails with no stored damage (word 1)
 * or no attacker (word 2 = 0x1f); word 0 becomes 3. */
PX_BATTLE_MOVES_10_HANDLER(px_battle_bide_ready)
{
    (void)handler, (void)context;
    const struct px_pokemon_api *m = memory(api);
    if (!moving(id, api)) return;
    if ((int32_t)m->load32(m->user, work) < 2) return;
    if (m->load32(m->user, work + 4) == 0 || m->load32(m->user, work + 8) == 0x1f) result(FAILS, api);
    m->store32(m->user, work, 3);
}

/* Table 108d4b38 (literal 108b7db4): random ranges by protect streak 0..6. */
static const uint16_t protect_odds[7] = {1, 3, 9, 27, 81, 243, 729};

/* Fails when the streak's roll misses or the battler moves last. */
PX_BATTLE_MOVES_10_HANDLER(px_battle_protect_ready)
{
    (void)handler, (void)work;
    if (!moving(id, api)) return;
    const uint32_t streak = api->counter(user(api), battler(context, id, api), 3);
    if (streak != 0 && api->random(user(api), protect_odds[streak >= 7 ? 6 : streak]) != 0) {
        result(FAILS, api);
        return;
    }
    if (api->moves_last(user(api), context, id) != 0) result(FAILS, api);
}

/* Rest: unless another failure is set, asleep 0x1b, heal-blocked 0xd, full HP
 * 0xe, Insomnia (15) or Vital Spirit (72) 0xf. */
PX_BATTLE_MOVES_10_HANDLER(px_battle_rest_ready)
{
    (void)handler, (void)work;
    if (!moving(id, api)) return;
    if ((get(RESULT, api) & 0xff) != 0) return;
    const uint32_t pokemon = battler(context, id, api);
    if (api->has_status(user(api), pokemon, 2) != 0) {
        result(FAILS, api);
        return;
    }
    if (api->has_status(user(api), pokemon, 0xf) != 0) {
        result(0xd, api);
        return;
    }
    if (api->full_hp(user(api), pokemon) != 0) {
        result(0xe, api);
        return;
    }
    const uint32_t ability = api->base->pokemon_value(user(api), pokemon, 0x11) & 0xffff;
    if (ability == 15 || ability == 72) result(0xf, api);
}

/* Spit Up and Swallow fail with nothing stockpiled; Stockpile fails at three. */
PX_BATTLE_MOVES_10_HANDLER(px_battle_spit_up_ready)
{
    (void)handler, (void)work;
    if (!moving(id, api)) return;
    if (api->counter(user(api), battler(context, id, api), 0) == 0) result(FAILS, api);
}

PX_BATTLE_MOVES_10_HANDLER(px_battle_stockpile_ready)
{
    (void)handler, (void)work;
    if (!moving(id, api)) return;
    if (api->counter(user(api), battler(context, id, api), 0) >= 3) result(FAILS, api);
}

/* Snore and Sleep Talk clear the sleep failure (2) and fail when awake. */
PX_BATTLE_MOVES_10_HANDLER(px_battle_snore_ready)
{
    (void)handler, (void)work;
    if (!moving(id, api)) return;
    const uint32_t pokemon = battler(context, id, api);
    if ((get(RESULT, api) & 0xff) == 2) {
        result(0, api);
        return;
    }
    if (api->has_status(user(api), pokemon, 2) == 0) result(FAILS, api);
}

/* Teleport (no key test at first): fails where escape is forbidden or in a
 * battle format other than 0; then, unless escape is allowed outright, fails
 * when the battler is trapped or Fairy Lock is up. */
PX_BATTLE_MOVES_10_HANDLER(px_battle_teleport_ready)
{
    (void)handler, (void)work;
    if (api->escape_forbidden(user(api), context) != 0 || api->battle_format(user(api), context) != 0) result(FAILS, api);
    if (api->escape_allowed(user(api), context, id) != 0) return;
    const uint32_t pokemon = battler(context, id, api);
    if (api->has_status(user(api), pokemon, 8) != 0 || api->has_status(user(api), pokemon, 0x16) != 0 ||
        api->field_active(user(api), context, 9) != 0)
        result(FAILS, api);
}

/* Sky Drop fails when its carried battler (counter 4, minus 1) has left. */
PX_BATTLE_MOVES_10_HANDLER(px_battle_sky_drop_ready)
{
    (void)handler, (void)work;
    if (!moving(id, api)) return;
    const uint32_t carried = api->counter(user(api), battler(context, id, api), 4);
    if (carried == 0) return;
    const uint32_t target = (carried - 1) & 0xff;
    if (target >= 0x18) return;
    if (api->slot_present(user(api), context, target) == 0) result(FAILS, api);
}

/* Magic Coat: its own use fails when moving last; another battler's
 * reflectable status move aimed at this one is bounced (code 0x19). */
PX_BATTLE_MOVES_10_HANDLER(px_battle_magic_coat_ready)
{
    (void)handler, (void)work;
    const uint32_t mover = get(BATTLER, api) & 0xff;
    if (mover == id) {
        if (api->moves_last(user(api), context, id) != 0) result(FAILS, api);
        return;
    }
    if (api->is_ally(user(api), id, mover) != 0) return;
    if (api->typed(user(api), context, id) != 0) return;
    const uint32_t other = get(BATTLER, api) & 0xff;
    if (api->is_ally(user(api), other, id) != 0) return;
    const uint32_t move = get(MOVE, api) & 0xffff;
    if (api->move_target(user(api), move) != 0xb) return;
    if (api->move_flag(user(api), move, 4) == 0) return;
    if (result(0x19, api) != 0) api->bounce(user(api), context, id, other);
}

/* Wide Guard, Quick Guard, Crafty Shield (no key test): moving last fails and
 * resets the protect streak (kind 0x27: battler, counter 3, value 0). */
PX_BATTLE_MOVES_10_HANDLER(px_battle_guard_ready)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (api->moves_last(user(api), context, id) == 0) return;
    if (result(FAILS, api) == 0) return;
    const uint32_t rec = api->action_record(user(api), context, 0x27, id);
    m->store8(m->user, rec + 5, 3);
    m->store8(m->user, rec + 4, (uint8_t)id);
    m->store8(m->user, rec + 6, 0);
    api->commit_actions(user(api), context, rec);
}

/* Fling fails without a flingable, usable item that the holder may lose. */
PX_BATTLE_MOVES_10_HANDLER(px_battle_fling_ready)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!moving(id, api)) return;
    const uint32_t item = item_of(battler(context, id, api), api);
    if (item != 0 && api->item_param(user(api), item, 6) != 0 && api->item_usable(user(api), context, id) != 0) {
        const uint32_t pokemon = battler(context, id, api);
        const uint32_t species = m->load16(m->user, pokemon + UINT32_C(0xc));
        if (api->item_locked(user(api), context, species, item_of(pokemon, api)) == 0) return;
    }
    result(FAILS, api);
}

PX_BATTLE_MOVES_10_HANDLER(px_battle_magnet_rise_ready)
{
    (void)handler, (void)work;
    if (!moving(id, api)) return;
    const uint32_t pokemon = battler(context, id, api);
    if (api->has_status(user(api), pokemon, 0x15) != 0 || api->has_status(user(api), pokemon, 0x1f) != 0) result(FAILS, api);
}

/* Mat Block and Fake Out: only on the battler's first turn (state bit 0);
 * Mat Block also fails when moving last. */
PX_BATTLE_MOVES_10_HANDLER(px_battle_mat_block_ready)
{
    (void)handler, (void)work;
    if (!moving(id, api)) return;
    if (px_battle_pokemon_state(battler(context, id, api), 0, memory(api)) != 0) result(FAILS, api);
    if (api->moves_last(user(api), context, id) != 0) result(FAILS, api);
}

PX_BATTLE_MOVES_10_HANDLER(px_battle_fake_out_ready)
{
    (void)handler, (void)work;
    if (!moving(id, api)) return;
    if (px_battle_pokemon_state(battler(context, id, api), 0, memory(api)) != 0) result(FAILS, api);
}

/* Natural Gift fails unless the held item may be used and has a gift power. */
PX_BATTLE_MOVES_10_HANDLER(px_battle_natural_gift_ready)
{
    (void)handler, (void)work;
    if (!moving(id, api)) return;
    if (api->item_usable(user(api), context, id) != 0) {
        const uint32_t item = item_of(battler(context, id, api), api);
        const uint32_t power = api->item_param(user(api), item, 7);
        if (item != 0 && power != 0) return;
    }
    result(FAILS, api);
}

/* Follow Me and Rage Powder fail in format 0 or 3 when nothing else failed. */
PX_BATTLE_MOVES_10_HANDLER(px_battle_follow_me_ready)
{
    (void)handler, (void)work;
    if (api->battle_format(user(api), context) != 0 && api->battle_format(user(api), context) != 3) return;
    if (!moving(id, api)) return;
    if (get(RESULT, api) != 0) return;
    result(FAILS, api);
}
