#include "battle_moves_11.h"

enum { BATTLER = 0x02, TARGET = 0x0d, ORIGINAL = 0x0e, MOVE = 0x12, MESSAGE = 0x40, FAILED = 0x42, ANNOUNCED = 0x53 };

static void *user(const struct px_battle_moves_11_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_11_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_11_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static void set(uint32_t key, uint32_t value, const struct px_battle_moves_11_api *api)
{
    px_battle_event_rewrite(key, value, api->base->events);
}

static uint32_t moving(uint32_t id, const struct px_battle_moves_11_api *api)
{
    return get(BATTLER, api) == id;
}

static uint32_t battler(uint32_t context, uint32_t id, const struct px_battle_moves_11_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

/* The called move and the target chosen for it. */
static void call_move(uint32_t context, uint32_t id, uint32_t move, const struct px_battle_moves_11_api *api)
{
    const uint32_t target = api->choose_target(user(api), context, id, move);
    set(MOVE, move, api);
    set(TARGET, target, api);
}

/* Sleep Talk: while asleep, one of the battler's own moves that Sleep Talk
 * may call and that does not charge (flag 1), listed in the work area. */
PX_BATTLE_MOVES_11_HANDLER(px_battle_sleep_talk_call)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    if (!moving(id, api)) return;
    const uint32_t pokemon = battler(context, id, api);
    if (api->has_status(user(api), pokemon, 2) == 0) return;
    const uint32_t count = m->load8(m->user, pokemon + UINT32_C(0x152));
    uint32_t listed = 0;
    for (uint32_t slot = 0; slot < count; slot = (slot + 1) & 0xff) {
        const uint32_t move = api->move_at(user(api), pokemon, slot);
        if (api->sleep_talk_banned(user(api), move) != 0) continue;
        if (api->move_flag(user(api), move, 1) != 0) continue;
        m->store32(m->user, work + listed * 4, move);
        listed = (listed + 1) & 0xff;
    }
    if (listed == 0) {
        set(FAILED, 1, api);
        return;
    }
    const uint32_t pick = api->random(user(api), listed) & 0xff;
    call_move(context, id, m->load32(m->user, work + pick * 4) & 0xffff, api);
}

/* Assist: a move of another party member (fainted ones included; members
 * that neither fainted nor can battle are skipped) that Assist may call,
 * gathered in a 0x30-byte buffer. Nothing happens when there is none. */
PX_BATTLE_MOVES_11_HANDLER(px_battle_assist_call)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!moving(id, api)) return;
    const uint32_t party = api->party(user(api), context, id);
    const uint32_t buffer = api->temp_buffer(user(api), context, 0x30);
    const uint32_t members = m->load8(m->user, party + UINT32_C(0x18));
    uint32_t listed = 0;
    for (uint32_t i = 0; i < members; i = (i + 1) & 0xff) {
        const uint32_t member = api->party_member(user(api), party, i);
        if (m->load8(m->user, member + UINT32_C(0x19)) == id) continue;
        if (api->fainted(user(api), member) == 0 && api->can_battle(user(api), member) == 0) continue;
        const uint32_t count = m->load8(m->user, member + UINT32_C(0x152));
        for (uint32_t slot = 0; slot < count; slot = (slot + 1) & 0xff) {
            const uint32_t move = api->move_at(user(api), member, slot);
            if (api->assist_banned(user(api), move) != 0) continue;
            m->store16(m->user, buffer + listed * 2, (uint16_t)move);
            listed = (listed + 1) & 0xff;
        }
    }
    if (listed == 0) return;
    const uint32_t pick = api->random(user(api), listed) & 0xff;
    call_move(context, id, m->load16(m->user, buffer + pick * 2), api);
}

/* Metronome: a move picked from its pool (the pool routine also stores a count). */
PX_BATTLE_MOVES_11_HANDLER(px_battle_metronome_call)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (!moving(id, api)) return;
    const uint32_t pool = api->metronome_pool(user(api), api->pool_scratch);
    const uint32_t move = api->metronome_pick(user(api), pool, m->load32(m->user, api->pool_scratch) & 0xffff);
    call_move(context, id, move, api);
}

/* Nature Power: by terrain (jump table 108b7150: none 0 and 6+ fall to the
 * environment), then by environment (jump table 108b7194). The target is
 * left as it was; with none (0xa) the chooser runs but its result is unused. */
PX_BATTLE_MOVES_11_HANDLER(px_battle_nature_power_call)
{
    (void)handler, (void)work;
    if (!moving(id, api)) return;
    uint32_t move = 1;
    const uint32_t terrain = api->terrain(user(api), context);
    switch (terrain) {
    case 1: move = 0x19c; break;   /* Energy Ball */
    case 2: move = 0x249; break;   /* Moonblast (literal 108b7234) */
    case 3: move = 0x55; break;    /* Thunderbolt */
    case 4: move = 0x38; break;    /* Hydro Pump */
    case 5: move = 0x3a; break;    /* Ice Beam */
    default:
        if (move == 1) {
            switch (api->environment(user(api), context)) {
            case 1: move = 0x19e; break;   /* literal 108b7238 */
            case 2: move = 0x198; break;
            case 3: move = 0x19c; break;
            case 4: move = 0x38; break;
            case 5: move = 0x1aa; break;   /* literal 108b723c */
            case 6: move = 0x20c; break;
            case 7: move = 0x3a; break;
            default: move = 0xa1; break;
            }
        }
        break;
    }
    const uint32_t target = get(ORIGINAL, api) & 0xff;
    if (target == 0xa) (void)api->choose_target(user(api), context, id, move);
    set(MOVE, move, api);
    set(TARGET, target, api);
}

/* Copycat: the last move used in the battle, unless Copycat may not call it. */
PX_BATTLE_MOVES_11_HANDLER(px_battle_copycat_call)
{
    (void)handler, (void)work;
    if (!moving(id, api)) return;
    const uint32_t move = api->last_move_used(user(api), context);
    if (move == 0) return;
    if (api->copycat_banned(user(api), move) != 0) return;
    call_move(context, id, move, api);
}

/* With no target (0xa), the target selector of the opposing side in this format. */
static uint32_t target_of(uint32_t context, uint32_t id, const struct px_battle_moves_11_api *api)
{
    uint32_t target = get(ORIGINAL, api) & 0xff;
    if (target == 0xa) {
        const uint32_t selector = api->opposing_selector(user(api), context, id);
        target = api->target_selector(user(api), api->battle_format(user(api), context), selector, 0);
    }
    return target;
}

/* Me First: the target's chosen damaging move, if it has not moved yet. */
PX_BATTLE_MOVES_11_HANDLER(px_battle_me_first_call)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    const uint32_t action = api->action_scratch;
    if (!moving(id, api)) return;
    const uint32_t target = target_of(context, id, api);
    if (api->base->side_battlers(user(api), context, target, work) != 0) {
        const uint32_t pokemon = battler(context, m->load8(m->user, work), api);
        const uint32_t other = m->load8(m->user, work);
        if (api->turn_flag(user(api), pokemon, 3) == 0 && api->chosen_action(user(api), context, other, action) != 0 &&
            api->slot_present(user(api), context, other) != 0 && api->charging(user(api), context, other) == 0) {
            const uint32_t move = api->action_move(user(api), action);
            if (move != 0 && api->move_damages(user(api), move) != 0 && api->me_first_banned(user(api), move) == 0) {
                set(MOVE, move, api);
                set(TARGET, target, api);
                return;
            }
        }
    }
    set(FAILED, 1, api);
}

/* Mirror Move: the target's last move when it may be mirrored (flag 6). */
PX_BATTLE_MOVES_11_HANDLER(px_battle_mirror_move_call)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    if (!moving(id, api)) return;
    const uint32_t target = target_of(context, id, api);
    if (api->base->side_battlers(user(api), context, target, work) == 0) return;
    const uint32_t move = api->last_move(user(api), battler(context, m->load8(m->user, work), api));
    if (move == 0 || api->move_flag(user(api), move, 6) == 0) {
        set(FAILED, 1, api);
        return;
    }
    set(MOVE, move, api);
    set(TARGET, target, api);
}

/* Event 0x19: Bide's messages by turn (1: literal 108b69b4, 0x2e9; 2: 0x2ec). */
PX_BATTLE_MOVES_11_HANDLER(px_battle_bide_announce)
{
    (void)handler, (void)context;
    const struct px_pokemon_api *m = memory(api);
    if (!moving(id, api)) return;
    const uint32_t buffer = get(MESSAGE, api);
    const uint32_t turn = m->load32(m->user, work);
    if (turn != 1 && turn != 2) return;
    api->message(user(api), buffer, 2, turn == 1 ? 0x2e9 : 0x2ec);
    api->message_arg(user(api), buffer, id);
    set(ANNOUNCED, 1, api);
}

static void announce_move(uint32_t id, uint32_t message, const struct px_battle_moves_11_api *api)
{
    if (!moving(id, api)) return;
    const uint32_t buffer = get(MESSAGE, api);
    const uint32_t move = get(MOVE, api) & 0xffff;
    api->message(user(api), buffer, 1, message);
    api->message_arg(user(api), buffer, move);
    set(ANNOUNCED, 1, api);
}

PX_BATTLE_MOVES_11_HANDLER(px_battle_metronome_announce) { (void)handler, (void)context, (void)work; announce_move(id, 0x7e, api); }
PX_BATTLE_MOVES_11_HANDLER(px_battle_nature_power_announce) { (void)handler, (void)context, (void)work; announce_move(id, 0x7f, api); }
