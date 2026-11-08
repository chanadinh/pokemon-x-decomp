#include "battle_moves_8.h"

enum { ATTACKER = 0x03, DEFENDER = 0x04, MESSAGE = 0x40, FAIL = 0x41 };

static void *user(const struct px_battle_moves_8_api *api)
{
    return api->base->events->user;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_8_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t attacking(uint32_t id, const struct px_battle_moves_8_api *api)
{
    return get(ATTACKER, api) == id;
}

static uint32_t battler(uint32_t context, uint32_t id, const struct px_battle_moves_8_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static uint32_t defender(uint32_t context, const struct px_battle_moves_8_api *api)
{
    return battler(context, get(DEFENDER, api) & 0xff, api);
}

static uint32_t value(uint32_t pokemon, uint32_t key, const struct px_battle_moves_8_api *api)
{
    return api->base->pokemon_value(user(api), pokemon, key);
}

static uint32_t fail(const struct px_battle_moves_8_api *api)
{
    return px_battle_event_rewrite(FAIL, 1, api->base->events);
}

/* Fails when either battler is genderless or both share a gender. */
static void genders(uint32_t first, uint32_t second, const struct px_battle_moves_8_api *api, int attacker_first)
{
    const uint32_t a = value(first, 0x12, api) & 0xff;
    const uint32_t b = value(second, 0x12, api) & 0xff;
    const uint32_t own = attacker_first ? a : b, other = attacker_first ? b : a;
    if (own == 2 || other == 2 || own == other) fail(api);
}

/* Attract looks the attacker up first and reads its gender first. */
PX_BATTLE_MOVES_8_HANDLER(px_battle_attract_check)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t attacker = battler(context, id, api);
    const uint32_t target = defender(context, api);
    genders(attacker, target, api, 1);
}

/* Captivate looks the defender up first but also reads the attacker's gender first. */
PX_BATTLE_MOVES_8_HANDLER(px_battle_captivate_check)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t target = defender(context, api);
    const uint32_t attacker = battler(context, id, api);
    genders(attacker, target, api, 1);
}

static void needs_sleep(uint32_t context, uint32_t id, const struct px_battle_moves_8_api *api)
{
    if (!attacking(id, api)) return;
    if (api->has_status(user(api), defender(context, api), 2) == 0) fail(api);
}

PX_BATTLE_MOVES_8_HANDLER(px_battle_dream_eater_check) { (void)handler, (void)work; needs_sleep(context, id, api); }
PX_BATTLE_MOVES_8_HANDLER(px_battle_nightmare_check) { (void)handler, (void)work; needs_sleep(context, id, api); }

/* Fails unless one of the attacker's types (bits 0-4, 5-9, 10-14; 0x12 none)
 * is one of the defender's. */
PX_BATTLE_MOVES_8_HANDLER(px_battle_synchronoise_check)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t target = defender(context, api);
    const uint32_t attacker = battler(context, id, api);
    const uint32_t own = api->types(user(api), attacker);
    const uint32_t other = api->types(user(api), target);
    const uint32_t mine[3] = {own & 0x1f, own >> 5 & 0x1f, own >> 10 & 0x1f};
    const uint32_t theirs[3] = {other & 0x1f, other >> 5 & 0x1f, other >> 10 & 0x1f};
    for (uint32_t i = 0; i < 3; ++i) {
        if (mine[i] == 0x12) continue;
        if (mine[i] == theirs[0] || mine[i] == theirs[1] || mine[i] == theirs[2]) return;
    }
    fail(api);
}

/* Succeeds only when the defender has not moved yet and chose a damaging move.
 * The action buffer is examined a second time on every other path; when the
 * defender has already moved it was never filled. On failure the message
 * 0x4a goes to the buffer named by key 0x40. */
PX_BATTLE_MOVES_8_HANDLER(px_battle_sucker_punch_check)
{
    (void)handler, (void)work;
    const uint32_t action = api->action_scratch;
    if (!attacking(id, api)) return;
    const uint32_t t = get(DEFENDER, api) & 0xff;
    if (api->turn_flag(user(api), battler(context, t, api), 3) == 0 &&
        api->chosen_action(user(api), context, t, action) != 0) {
        const uint32_t move = api->action_move(user(api), action);
        if (move != 0 && api->move_damages(user(api), move) != 0) return;
    }
    const uint32_t move = api->action_move(user(api), action);
    if (move != 0 && api->move_damages(user(api), move) != 0) return;
    if (fail(api) != 0) api->message(user(api), get(MESSAGE, api), 1, 0x4a);
}

PX_BATTLE_MOVES_8_HANDLER(px_battle_rototiller_check)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if (api->airborne(user(api), context, get(DEFENDER, api) & 0xff) != 0) fail(api);
}

PX_BATTLE_MOVES_8_HANDLER(px_battle_sky_drop_check)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if (api->has_type(user(api), defender(context, api), 2) != 0) fail(api);
}

/* Truant (54) and Insomnia (15); the ability is read again for the second test. */
PX_BATTLE_MOVES_8_HANDLER(px_battle_worry_seed_check)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t target = defender(context, api);
    if (value(target, 0x11, api) == 54 || value(target, 0x11, api) == 15) fail(api);
}

PX_BATTLE_MOVES_8_HANDLER(px_battle_mean_look_check)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    if (api->has_type(user(api), defender(context, api), 7) != 0) fail(api);
}

/* Fails unless the defender's HP exceeds the attacker's (signed difference). */
PX_BATTLE_MOVES_8_HANDLER(px_battle_endeavor_check)
{
    (void)handler, (void)work;
    if (!attacking(id, api)) return;
    const uint32_t target = defender(context, api);
    const uint32_t attacker = battler(context, id, api);
    const uint32_t remaining = value(target, 0xd, api);
    if ((int32_t)(remaining - value(attacker, 0xd, api)) <= 0) fail(api);
}
