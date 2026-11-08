#include "battle_moves_7.h"
#include "../battle_modifier/battle_modifier.h"

static void *user(const struct px_battle_moves_7_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_7_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_7_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t attacking(uint32_t id, const struct px_battle_moves_7_api *api)
{
    return get(0x03, api) == id;
}

static uint32_t battler(uint32_t context, uint32_t id, const struct px_battle_moves_7_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_moves_7_api *api)
{
    return api->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t rec, const struct px_battle_moves_7_api *api)
{
    api->commit_actions(user(api), context, rec);
}

static void store8(uint32_t at, uint32_t value, const struct px_battle_moves_7_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store8(m->user, at, (uint8_t)value);
}

static void store16(uint32_t at, uint32_t value, const struct px_battle_moves_7_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store16(m->user, at, (uint16_t)value);
}

static uint32_t item_of(uint32_t pokemon, const struct px_battle_moves_7_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load16(m->user, pokemon + UINT32_C(0x12));
}

/* Target i of the hit (key 0x06 + i, low byte). */
static uint32_t target(uint32_t i, const struct px_battle_moves_7_api *api)
{
    return get((i + 6) & 0xff, api) & 0xff;
}

/* Cure record (kind 0xa) for the status of a battler. */
static void cure_flag(uint32_t context, uint32_t id, uint32_t who, uint32_t status, const struct px_battle_moves_7_api *api)
{
    const uint32_t rec = record(context, 0xa, id, api);
    store8(rec + 4, status, api);
    store8(rec + 5, who, api);
    store8(rec + 0x11, 1, api);
    commit(context, rec, api);
}

/* Thaws every hit target that is still standing and frozen. */
PX_BATTLE_MOVES_7_HANDLER(px_battle_scald_thaw)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t count = get(0x05, api);
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t t = target(i, api);
        const uint32_t pokemon = battler(context, t, api);
        if (api->fainted(user(api), pokemon) != 0) continue;
        if (api->has_status(user(api), pokemon, 3) == 0) continue;
        cure_flag(context, id, t, 3, api);
    }
}

/* With no item of its own (and work word 0 clear), the attacker takes the
 * first target's item unless it is locked. */
PX_BATTLE_MOVES_7_HANDLER(px_battle_thief_steal)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    if (item_of(battler(context, id, api), api) != 0 || m->load32(m->user, work) != 0) return;
    const uint32_t t = target(0, api);
    if (t == 0x1f) return;
    const uint32_t victim = battler(context, t, api);
    if (item_of(victim, api) == 0) return;
    if (api->item_locked(user(api), context, id, t) != 0) return;
    const uint32_t rec = record(context, 0x25, id, api);
    store8(rec + 4, t, api);
    api->message(user(api), rec + 8, 2, 0x427);   /* literal 108af68c */
    api->message_arg(user(api), rec + 8, id);
    api->message_arg(user(api), rec + 8, t);
    api->message_arg(user(api), rec + 8, item_of(victim, api));
    commit(context, rec, api);
    const uint32_t mark = record(context, 0x3b, id, api);
    const uint32_t flags = m->load32(m->user, mark);
    store8(mark + 4, 1, api);
    m->store32(m->user, mark, flags | UINT32_C(0x1000000));
    commit(context, mark, api);
}

/* Eats the first target's berry: removal (kind 0x21), then its use by the attacker (kind 0x23). */
PX_BATTLE_MOVES_7_HANDLER(px_battle_pluck_eat)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    const uint32_t t = target(0, api);
    const uint32_t item = item_of(battler(context, t, api), api);
    if (api->item_is_berry(user(api), item) == 0) return;
    const uint32_t rec = record(context, 0x21, id, api);
    store8(rec + 6, t, api);
    store16(rec + 4, 0, api);
    api->message(user(api), rec + 0xc, 2, 0x308);
    api->message_arg(user(api), rec + 0xc, id);
    api->message_arg(user(api), rec + 0xc, item);
    commit(context, rec, api);
    const uint32_t use = record(context, 0x23, id, api);
    const uint32_t flags = m->load32(m->user, use);
    store16(use + 6, item, api);
    store8(use + 4, id, api);
    store8(use + 5, 1, api);
    m->store32(m->user, use, flags | UINT32_C(0x1000000));
    commit(context, use, api);
}

uint32_t px_battle_smack_down_target(uint32_t context, uint32_t id, uint32_t t, const struct px_battle_moves_7_api *api)
{
    const uint32_t pokemon = battler(context, t, api);
    uint32_t grounded = 0;
    if (api->charging(user(api), context, t) != 0) return 0;
    if (api->airborne(user(api), context, t) != 0 && api->has_status(user(api), pokemon, 0x21) == 0) {
        const uint32_t rec = record(context, 0xb, id, api);
        store8(rec + 0xf, t, api);
        store16(rec + 4, 0x1f, api);
        memory(api)->store32(memory(api)->user, rec + 8, 1);
        api->message(user(api), rec + 0x14, 2, 0x46e);   /* literal 108b67ac */
        api->message_arg(user(api), rec + 0x14, t);
        commit(context, rec, api);
        grounded = 1;
        /* Magnet Rise (0x1e) and Telekinesis (0x20) end. */
        for (uint32_t status = 0x1e; status <= 0x20; status += 2) {
            if (api->has_status(user(api), pokemon, status) == 0) continue;
            const uint32_t cure = record(context, 0xa, id, api);
            store8(cure + 0x11, 1, api);
            store8(cure + 4, status, api);
            store8(cure + 0x12, 1, api);
            store8(cure + 5, t, api);
            commit(context, cure, api);
        }
    }
    /* A target in the air (state bit 3) comes down; the message only when not grounded above. */
    if (px_battle_pokemon_state(pokemon, 3, memory(api)) != 0) {
        const uint32_t rec = record(context, 0x37, id, api);
        store8(rec + 5, 3, api);
        store8(rec + 4, t, api);
        if (grounded == 0) {
            api->message(user(api), rec + 8, 2, 0x46e);
            api->message_arg(user(api), rec + 8, t);
        }
        commit(context, rec, api);
    }
    return grounded;
}

PX_BATTLE_MOVES_7_HANDLER(px_battle_smack_down_hit)
{
    (void)handler, (void)work;
    if (attacking(id, api)) (void)px_battle_smack_down_target(context, id, target(0, api), api);
}

PX_BATTLE_MOVES_7_HANDLER(px_battle_thousand_arrows_hit)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t count = get(0x05, api);
    for (uint32_t i = 0; i < count; ++i) (void)px_battle_smack_down_target(context, id, target(i, api), api);
}

/* Traps every target not already trapped (condition 0x16). */
PX_BATTLE_MOVES_7_HANDLER(px_battle_thousand_waves_hit)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    const uint32_t count = get(0x05, api);
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t t = target(i, api);
        if (api->has_status(user(api), battler(context, t, api), 0x16) != 0) continue;
        const uint32_t rec = record(context, 0xb, id, api);
        store8(rec + 0xf, t, api);
        store16(rec + 4, 0x16, api);
        m->store32(m->user, rec + 8, api->trap_value(user(api), id));
        api->message(user(api), rec + 0x14, 2, 0x36b);   /* literal 108b65f4 */
        api->message_arg(user(api), rec + 0x14, t);
        commit(context, rec, api);
    }
}

/* Forces the first target out; Circle Throw (509) and Dragon Tail have their own messages. */
PX_BATTLE_MOVES_7_HANDLER(px_battle_circle_throw_hit)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    const uint32_t rec = record(context, 0x2f, id, api);
    store8(rec + 6, get(0x06, api), api);
    const uint32_t move = m->load16(m->user, handler + UINT32_C(0x38));
    store16(rec + 4, move == 509 ? 0x2c7 : 0x2c8, api);   /* literal 108b30f8 or immediate */
    commit(context, rec, api);
}

/* Attack +2 when any target fainted. */
PX_BATTLE_MOVES_7_HANDLER(px_battle_fell_stinger_hit)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!attacking(id, api)) return;
    const uint32_t count = get(0x05, api);
    uint32_t down = 0;
    for (uint32_t i = 0; i < count; ++i)
        if (api->fainted(user(api), battler(context, target(i, api), api)) != 0) ++down;
    if (down == 0) return;
    const uint32_t rec = record(context, 0xd, id, api);
    const uint32_t flags = m->load8(m->user, rec + 0x18);
    store8(rec + 0xc, id, api);
    store8(rec + 0x16, 1, api);
    store8(rec + 4, 1, api);
    store8(rec + 0x17, 2, api);
    store8(rec + 0x18, flags | 4, api);
    commit(context, rec, api);
}

/* Knocks off the first target's item unless it is locked. */
PX_BATTLE_MOVES_7_HANDLER(px_battle_knock_off_hit)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t t = target(0, api);
    if (api->item_locked(user(api), context, id, t) != 0) return;
    const uint32_t item = item_of(battler(context, t, api), api);
    if (item == 0) return;
    const uint32_t rec = record(context, 0x21, id, api);
    store8(rec + 6, t, api);
    store16(rec + 4, 0, api);
    api->message(user(api), rec + 0xc, 2, 0x420);
    api->message_arg(user(api), rec + 0xc, id);
    api->message_arg(user(api), rec + 0xc, t);
    api->message_arg(user(api), rec + 0xc, item);
    commit(context, rec, api);
}

/* Cures the first target's paralysis. */
PX_BATTLE_MOVES_7_HANDLER(px_battle_smelling_salts_cure)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t t = target(0, api);
    if (t == 0x1f) return;
    if (api->has_status(user(api), battler(context, t, api), 1) == 0) return;
    const uint32_t rec = record(context, 0xa, id, api);
    store8(rec + 5, t, api);
    store8(rec + 0x11, 1, api);
    store8(rec + 4, 1, api);
    commit(context, rec, api);
}

/* Wakes the first target (no 0x1f test). */
PX_BATTLE_MOVES_7_HANDLER(px_battle_wake_up_slap_cure)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t t = target(0, api);
    if (api->has_status(user(api), battler(context, t, api), 2) == 0) return;
    const uint32_t rec = record(context, 0xa, id, api);
    store8(rec + 0x11, 1, api);
    store8(rec + 5, t, api);
    store8(rec + 4, 2, api);
    commit(context, rec, api);
}
