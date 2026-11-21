#include "battle_moves_b4.h"

static void *user(const struct px_battle_moves_b4_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_moves_b4_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_moves_b4_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_moves_b4_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static uint32_t value(uint32_t pokemon_address, uint32_t key, const struct px_battle_moves_b4_api *api)
{
    return api->base->pokemon_value(user(api), pokemon_address, key);
}

static void store8(uint32_t address, uint32_t v, const struct px_battle_moves_b4_api *api)
{
    memory(api)->store8(memory(api)->user, address, (uint8_t)v);
}

static void store16(uint32_t address, uint32_t v, const struct px_battle_moves_b4_api *api)
{
    memory(api)->store16(memory(api)->user, address, (uint16_t)v);
}

static void store32(uint32_t address, uint32_t v, const struct px_battle_moves_b4_api *api)
{
    memory(api)->store32(memory(api)->user, address, v);
}

static uint32_t load8(uint32_t address, const struct px_battle_moves_b4_api *api)
{
    return memory(api)->load8(memory(api)->user, address);
}

static uint32_t record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_moves_b4_api *api)
{
    return api->action_record(user(api), context, kind, id);
}

static void commit(uint32_t context, uint32_t rec, const struct px_battle_moves_b4_api *api)
{
    api->commit_actions(user(api), context, rec);
}

/* Kind 0x22 naming a battler (flag byte 1). */
static void kind_22(uint32_t context, uint32_t id, uint32_t battler, const struct px_battle_moves_b4_api *api)
{
    const uint32_t rec = record(context, 0x22, id, api);
    store8(rec + 4, battler, api);
    store8(rec + 5, 1, api);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B4_HANDLER(px_battle_encore_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* The target's last move is locked in (condition 0x17) for 3 turns, 4 when it has moved. */
    const uint32_t target = get(0x06, api) & 0xff;
    const uint32_t other = pokemon(context, target, api);
    const uint32_t move = api->last_move(user(api), other);
    if (api->has_status(user(api), other, 0x17) != 0) return;
    if (api->encore_blocked(user(api), move) != 0) return;
    if (api->has_move(user(api), other, move) == 0) return;
    uint32_t turns = 3;
    if (api->turn_flag(user(api), other, 1) != 0) turns = 4;
    const uint32_t rec = record(context, 0xb, id, api);
    store16(rec + 4, 0x17, api);
    store32(rec + 8, api->condition_word(user(api), turns, move), api);
    store8(rec + 0xf, target, api);
    store8(rec + 0xc, 0, api);
    api->message(user(api), rec + 0x14, 2, 0x22f);  /* literal 108ae710 */
    api->message_arg(user(api), rec + 0x14, target);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B4_HANDLER(px_battle_mimic_effect)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x03, api) != id) return;
    const uint32_t self = pokemon(context, id, api);
    if (api->transformed(user(api), self) != 0) return;
    const uint32_t other = pokemon(context, get(0x06, api) & 0xff, api);
    uint32_t move = api->copy_source(user(api), other);
    const uint32_t last = api->last_move(user(api), other);
    if (last != move) move = last;
    if (move == 0) return;
    if (api->mimic_blocked(user(api), move) != 0) return;
    if (api->move_slot(user(api), self, move) != 4) return;
    const uint32_t own = m->load16(m->user, handler + 0x38);
    const uint32_t slot = api->move_slot(user(api), self, own);
    if (slot == 4) return;
    uint32_t rec = record(context, 0x26, id, api);
    store8(rec + 5, slot, api);
    store8(rec + 6, 0, api);
    store8(rec + 4, id, api);
    store8(rec + 7, 0, api);
    store16(rec + 8, move, api);
    commit(context, rec, api);
    rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 2, 0x2b0);
    api->message_arg(user(api), rec + 4, id);
    api->message_arg(user(api), rec + 4, move);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B4_HANDLER(px_battle_conversion_2_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* The user takes a type that resists the type of the target's last move (+0x157). */
    const uint32_t type = load8(pokemon(context, get(0x06, api) & 0xff, api) + 0x157, api);
    if (type == 0x12) return;
    const uint32_t list = api->temp_buffer(user(api), context, 0x12);
    uint32_t count = api->resist_types(user(api), type, list);
    if (count == 0) return;
    const uint32_t self = pokemon(context, id, api);
    /* Types the user already has are removed from the list. */
    uint32_t i = 0;
    while (i < count) {
        if (api->has_type(user(api), self, load8(list + i, api)) == 0) {
            ++i;
            continue;
        }
        const int32_t rest = (int32_t)(count - i) - 1;
        if (rest > 0) api->move_bytes(user(api), list + i, list + i + 1, (uint32_t)rest);
        --count;
    }
    if (count == 0) return;
    const uint32_t pick = api->random(user(api), count);
    const uint32_t chosen = load8(list + pick, api);
    const uint32_t rec = record(context, 0x13, id, api);
    store8(rec + 6, id, api);
    store16(rec + 4, (chosen & 0x1f) | (chosen & 0x1f) << 5 | 0x4800, api);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B4_HANDLER(px_battle_belly_drum_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Half the maximum HP buys +6 Attack: needs more HP than that and room to rise. */
    const uint32_t self = pokemon(context, id, api);
    const uint32_t cost = api->hp_share(user(api), self, 2);
    const uint32_t room = api->stage_room(user(api), self, 1);
    if (value(self, 0xd, api) <= cost) return;
    if (room == 0) return;
    uint32_t rec = record(context, 7, id, api);
    store8(rec + 4, 1, api);
    store32(rec + 0x14, 0u - cost, api);
    store8(rec + 6, 1, api);
    store8(rec + 7, id, api);
    commit(context, rec, api);
    rec = record(context, 0xd, id, api);
    store8(rec + 4, 1, api);
    store8(rec + 0xc, id, api);
    store8(rec + 0x17, room, api);
    store8(rec + 0x16, 1, api);
    store8(rec + 0x18, load8(rec + 0x18, api) | 7, api);
    commit(context, rec, api);
    rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 2, 0x265);
    api->message_arg(user(api), rec + 4, id);
    commit(context, rec, api);
    kind_22(context, id, id, api);
}

PX_BATTLE_MOVES_B4_HANDLER(px_battle_pain_split_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Both battlers end up with the average HP (kind 7), then two kind-0x22 records. */
    const uint32_t target = get(0x06, api) & 0xff;
    const uint32_t mine_pokemon = pokemon(context, id, api);
    const uint32_t theirs_pokemon = pokemon(context, target, api);
    const uint32_t mine = value(mine_pokemon, 0xd, api);
    const uint32_t theirs = value(theirs_pokemon, 0xd, api);
    const uint32_t average = (mine + theirs) >> 1;
    uint32_t rec = record(context, 7, id, api);
    store8(rec + 4, 2, api);
    store32(rec + 0x14, average - mine, api);
    store32(rec + 0x18, average - theirs, api);
    store8(rec + 6, 1, api);
    store8(rec + 7, id, api);
    store8(rec + 8, target, api);
    commit(context, rec, api);
    rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 1, 0x75);
    commit(context, rec, api);
    kind_22(context, id, id, api);
    kind_22(context, id, target, api);
}

PX_BATTLE_MOVES_B4_HANDLER(px_battle_memento_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Two stat records (1 and 3, -2 each) for the target, then kind 0x12 for the user. */
    const uint32_t first = get(0x06, api);
    const uint32_t word = api->context_word(user(api), context);
    for (uint32_t stat = 1; stat <= 3; stat += 2) {
        const uint32_t rec = record(context, 0xd, id, api);
        store8(rec + 0xc, first, api);
        store8(rec + 0x16, 1, api);
        store8(rec + 4, stat, api);
        store8(rec + 0x17, 0xfe, api);
        store8(rec + 0x18, load8(rec + 0x18, api) | 0x16, api);
        store32(rec + 8, word, api);
        commit(context, rec, api);
    }
    const uint32_t rec = record(context, 0x12, id, api);
    store8(rec + 4, id, api);
    commit(context, rec, api);
}

/* Guard Split and Power Split: both battlers' two raw stats go to their averages. */
static void split(uint32_t context, uint32_t id, uint32_t first, uint32_t second, uint32_t offset_first,
                  uint32_t offset_second, uint32_t flags, uint32_t message,
                  const struct px_battle_moves_b4_api *api)
{
    const uint32_t target = get(0x06, api) & 0xff;
    const uint32_t mine = pokemon(context, id, api);
    const uint32_t theirs = pokemon(context, target, api);
    const uint32_t own_first = api->raw_stat(user(api), mine, first);
    const uint32_t other_first = api->raw_stat(user(api), theirs, first);
    const uint32_t x = (uint32_t)((int32_t)(own_first + other_first) / 2);
    const uint32_t own_second = api->raw_stat(user(api), mine, second);
    const uint32_t other_second = api->raw_stat(user(api), theirs, second);
    const uint32_t y = (uint32_t)((int32_t)(own_second + other_second) / 2);
    uint32_t rec = record(context, 0x10, id, api);
    const uint32_t old = load8(rec + 0xf, api);
    store16(rec + offset_first, x, api);
    store8(rec + 0xe, target, api);
    store16(rec + offset_second, y, api);
    store8(rec + 0xf, old | flags, api);
    commit(context, rec, api);
    rec = record(context, 0x10, id, api);
    const uint32_t again = load8(rec + 0xf, api);
    store16(rec + offset_first, x, api);
    store16(rec + offset_second, y, api);
    store8(rec + 0xe, id, api);
    store8(rec + 0xf, again | flags, api);
    api->message(user(api), rec + 0x10, 2, message);
    api->message_arg(user(api), rec + 0x10, id);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B4_HANDLER(px_battle_guard_split_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    split(context, id, 9, 0xb, 6, 0xa, 0xa, 0x451, api);  /* literal 108b3830 */
}

PX_BATTLE_MOVES_B4_HANDLER(px_battle_power_split_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    split(context, id, 8, 0xa, 4, 8, 5, 0x44e, api);  /* literal 108b4028 */
}

PX_BATTLE_MOVES_B4_HANDLER(px_battle_topsy_turvy_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Every stat stage of the target is mirrored around 6 (value 12 - v). */
    const uint32_t target = get(0x06, api) & 0xff;
    const uint32_t other = pokemon(context, target, api);
    uint32_t changed = 0;
    const uint32_t buffer = api->temp_buffer(user(api), context, 0x1c);
    for (uint32_t i = 0; i < 7; ++i) {
        const uint32_t v = value(other, i + 1, api);
        store32(buffer + 4 * i, v, api);
        if (v == 6) continue;
        store32(buffer + 4 * i, 6 - (v - 6), api);
        changed = 1;
    }
    if (changed == 0) return;
    uint32_t rec = record(context, 0xe, id, api);
    store8(rec + 4, target, api);
    for (uint32_t i = 0; i < 7; ++i) store8(rec + 5 + i, memory(api)->load32(memory(api)->user, buffer + 4 * i), api);
    store8(rec + 0xc, load8(other + 0x155, api), api);
    commit(context, rec, api);
    rec = record(context, 3, id, api);
    api->message(user(api), rec + 4, 2, 0x499);  /* literal 108b5b84 */
    api->message_arg(user(api), rec + 4, target);
    commit(context, rec, api);
}

PX_BATTLE_MOVES_B4_HANDLER(px_battle_shell_smash_effect)
{
    (void)handler, (void)work;
    if (get(0x03, api) != id) return;
    /* Defense and Special Defense -1, Attack, Special Attack and Speed +2. */
    static const uint8_t stats[5] = {2, 4, 1, 3, 5};
    static const uint8_t stages[5] = {0xff, 0xff, 2, 2, 2};
    for (unsigned i = 0; i < 5; ++i) {
        const uint32_t rec = record(context, 0xd, id, api);
        store8(rec + 4, stats[i], api);
        store8(rec + 0x17, stages[i], api);
        store8(rec + 0x16, 1, api);
        store8(rec + 0x18, load8(rec + 0x18, api) | 6, api);
        store8(rec + 0xc, id, api);
        commit(context, rec, api);
    }
}
