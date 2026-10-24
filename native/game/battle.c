#include "battle.h"

#include <stdio.h>
#include <string.h>

#include "../../reconstructed/pokemon_stats/stats_math.h"

uint32_t px_rng_next(struct px_rng *rng)
{
    uint32_t x = rng->state ? rng->state : UINT32_C(0x9e3779b9);
    x ^= x << 13; x ^= x >> 17; x ^= x << 5;
    rng->state = x;
    return x;
}

uint32_t px_rng_below(struct px_rng *rng, uint32_t n) { return n ? px_rng_next(rng) % n : 0; }

/* The reconstructed nature adjustment reads the table through logical
 * addresses; this adapter serves the 125 bytes at 0x00570f6c. */
static uint8_t nature_load8(void *user, uint32_t address)
{
    const uint8_t *table = user;
    const uint32_t at = address - UINT32_C(0x00570f6c);
    return table && at < 125 ? table[at] : 0;
}

int px_battler_make(const struct px_battle_data *d, uint16_t species, uint8_t level, const uint8_t iv[6],
                    uint8_t nature, const uint16_t moves[4], struct px_battler *out)
{
    memset(out, 0, sizeof *out);
    if (!d || species == 0 || species >= d->personal_count || level < 1 || level > 100 || nature >= 25) return 0;
    const struct px_personal *p = &d->personal[species];
    struct px_pokemon_api memory;
    memset(&memory, 0, sizeof memory);
    memory.user = (void *)d->natures;
    memory.load8 = nature_load8;
    out->species = species; out->level = level; out->nature = nature;
    out->types[0] = p->types[0]; out->types[1] = p->types[1];
    memcpy(out->iv, iv, 6);
    out->stats[0] = (uint16_t)px_pokemon_hp_value(species, level, p->base[0], iv[0], 0);
    static const enum px_stat stat[6] = {PX_STAT_HP, PX_STAT_ATTACK, PX_STAT_DEFENSE, PX_STAT_SPEED,
                                         PX_STAT_SPECIAL_ATTACK, PX_STAT_SPECIAL_DEFENSE};
    for (unsigned k = 1; k < 6; ++k)
        out->stats[k] = (uint16_t)px_pokemon_stat_value(level, p->base[k], iv[k], 0, nature, stat[k], &memory);
    out->hp = out->stats[0];
    for (unsigned k = 0; k < 4; ++k) {
        if (moves[k] >= d->move_count) { memset(out, 0, sizeof *out); return 0; }
        out->moves[k] = moves[k];
        out->pp[k] = moves[k] ? d->moves[moves[k]].pp : 0;
    }
    return 1;
}

static const unsigned stage_of[6] = {0, 1, 2, 5, 3, 4}; /* personal index -> stage index */

uint32_t px_battle_stat(const struct px_battler *b, unsigned index, int ignore)
{
    if (index < 1 || index > 5) return 0;
    int s = b->stages[stage_of[index]];
    if ((ignore == 1 && s < 0) || (ignore == 2 && s > 0)) s = 0;
    const uint32_t v = b->stats[index];
    return s >= 0 ? v * (uint32_t)(2 + s) / 2 : v * 2 / (uint32_t)(2 - s);
}

uint32_t px_battle_damage(const struct px_battle_data *d, const struct px_battler *a, const struct px_battler *t,
                          uint16_t move, int critical, uint32_t roll, uint32_t *effectiveness)
{
    const struct px_move *m = &d->moves[move];
    const int physical = m->category == 1;
    const uint32_t attack = px_battle_stat(a, physical ? 1 : 4, critical ? 1 : 0);
    const uint32_t defense = px_battle_stat(t, physical ? 2 : 5, critical ? 2 : 0);
    uint32_t e = 16;
    e = e * d->chart[m->type][t->types[0]] / 4;
    if (t->types[1] != t->types[0]) e = e * d->chart[m->type][t->types[1]] / 4;
    if (effectiveness) *effectiveness = e;
    if (!e) return 0;
    uint32_t damage = ((2 * a->level / 5 + 2) * m->power * attack / (defense ? defense : 1)) / 50 + 2;
    if (critical) damage = damage * 3 / 2;
    damage = damage * roll / 100;
    if (m->type == a->types[0] || m->type == a->types[1]) damage = damage * 3 / 2;
    damage = damage * e / 16;
    return damage ? damage : 1;
}

static const char *species_name(const struct px_battle *b, unsigned side, const char *const *names, uint32_t count,
                                char *buffer, size_t size)
{
    const uint16_t s = b->side[side].species;
    char name[48];
    if (names && s < count) snprintf(name, sizeof name, "%s", names[s]);
    else snprintf(name, sizeof name, "#%u", s);
    snprintf(buffer, size, "%s%s", side ? "The wild " : "", name);
    return buffer;
}

static void say(struct px_battle *b, const char *text)
{
    if (b->log_count < sizeof b->log / sizeof b->log[0])
        snprintf(b->log[b->log_count++], sizeof b->log[0], "%s", text);
}

static void change_stats(struct px_battle *b, unsigned target, const struct px_move *m, unsigned k,
                         const char *target_name)
{
    static const char *const stat_names[8] = {"", "Attack", "Defense", "Sp. Atk", "Sp. Def", "Speed",
                                              "accuracy", "evasiveness"};
    const unsigned stat = m->stat[k];
    if (stat < 1 || stat > 7 || !m->stage[k]) return;
    int8_t *stage = &b->side[target].stages[stat];
    char line[96];
    const int next = *stage + m->stage[k];
    if ((m->stage[k] > 0 && *stage == 6) || (m->stage[k] < 0 && *stage == -6)) {
        snprintf(line, sizeof line, "%s's %s won't go any %s!", target_name, stat_names[stat],
                 m->stage[k] > 0 ? "higher" : "lower");
    } else {
        *stage = (int8_t)(next > 6 ? 6 : next < -6 ? -6 : next);
        const int amount = m->stage[k] < 0 ? -m->stage[k] : m->stage[k];
        snprintf(line, sizeof line, "%s's %s %s%s!", target_name, stat_names[stat],
                 m->stage[k] > 0 ? "rose" : "fell", amount >= 2 ? " sharply" : "");
    }
    say(b, line);
}

static int self_target(uint8_t target) { return target == 1 || target == 7 || target == 14; }

/* One move of `side` against the other; 1 when the defender fainted. */
static int use_move(struct px_battle *b, unsigned side, unsigned slot, const char *const *species_names,
                    uint32_t species_count, const char *const *move_names, uint32_t move_count)
{
    struct px_battler *a = &b->side[side], *t = &b->side[!side];
    const uint16_t id = a->moves[slot];
    const struct px_move *m = &b->data->moves[id];
    char user_name[64], target_name[64], line[96];
    species_name(b, side, species_names, species_count, user_name, sizeof user_name);
    species_name(b, !side, species_names, species_count, target_name, sizeof target_name);
    if (a->pp[slot]) --a->pp[slot];
    if (move_names && id < move_count) snprintf(line, sizeof line, "%s used %s!", user_name, move_names[id]);
    else snprintf(line, sizeof line, "%s used move %u!", user_name, id);
    say(b, line);
    if (m->accuracy != 101) {
        const int s = a->stages[6] - t->stages[7] > 6 ? 6 : a->stages[6] - t->stages[7] < -6 ? -6 :
                      a->stages[6] - t->stages[7];
        const uint32_t chance = s >= 0 ? m->accuracy * (uint32_t)(3 + s) / 3 : m->accuracy * 3u / (uint32_t)(3 - s);
        if (px_rng_below(&b->rng, 100) >= chance) {
            snprintf(line, sizeof line, "%s's attack missed!", user_name);
            say(b, line);
            return 0;
        }
    }
    if (m->category == 0) {
        const unsigned target = self_target(m->target) ? side : !side;
        const char *name = target == side ? user_name : target_name;
        for (unsigned k = 0; k < 3; ++k)
            if (!m->stat_percent[k] || m->stat_percent[k] >= 100 || px_rng_below(&b->rng, 100) < m->stat_percent[k])
                change_stats(b, target, m, k, name);
        if (!m->stat[0]) say(b, "But nothing happened!"); /* effects beyond stat changes are not executed */
        return 0;
    }
    static const uint32_t crit_odds[4] = {16, 8, 2, 1};
    const int critical = px_rng_below(&b->rng, crit_odds[m->crit_stage > 3 ? 3 : m->crit_stage]) == 0;
    const uint32_t roll = 85 + px_rng_below(&b->rng, 16);
    uint32_t effectiveness;
    const uint32_t damage = px_battle_damage(b->data, a, t, id, critical, roll, &effectiveness);
    if (!effectiveness) {
        snprintf(line, sizeof line, "It doesn't affect %s...", target_name);
        say(b, line);
        return 0;
    }
    t->hp = damage >= t->hp ? 0 : (uint16_t)(t->hp - damage);
    if (critical) say(b, "A critical hit!");
    if (effectiveness > 16) say(b, "It's super effective!");
    else if (effectiveness < 16) say(b, "It's not very effective...");
    if (t->hp == 0) {
        snprintf(line, sizeof line, "%s fainted!", target_name);
        say(b, line);
        return 1;
    }
    for (unsigned k = 0; k < 3; ++k) /* secondary stat changes on the target */
        if (m->stat[k] && m->stat_percent[k] && px_rng_below(&b->rng, 100) < m->stat_percent[k])
            change_stats(b, self_target(m->target) ? side : !side, m, k,
                         self_target(m->target) ? user_name : target_name);
    return 0;
}

void px_battle_start(struct px_battle *b, const struct px_battle_data *data, const struct px_battler *player,
                     const struct px_battler *wild, uint32_t seed)
{
    memset(b, 0, sizeof *b);
    b->data = data;
    b->side[0] = *player; b->side[1] = *wild;
    b->rng.state = seed ? seed : 1;
    b->state = PX_BATTLE_CHOOSING;
}

static int wild_choice(struct px_battle *b)
{
    unsigned usable[4], n = 0;
    for (unsigned k = 0; k < 4; ++k) if (b->side[1].moves[k] && b->side[1].pp[k]) usable[n++] = k;
    return n ? (int)usable[px_rng_below(&b->rng, n)] : -1;
}

int px_battle_turn(struct px_battle *b, int player_move, const char *const *species_names, uint32_t species_count,
                   const char *const *move_names, uint32_t move_count)
{
    if (b->state != PX_BATTLE_CHOOSING) return 0;
    if (player_move >= 4 || (player_move >= 0 && (!b->side[0].moves[player_move] || !b->side[0].pp[player_move])))
        return 0;
    b->log_count = 0;
    ++b->turn;
    if (player_move < 0) {
        const uint32_t mine = px_battle_stat(&b->side[0], 3, 0), theirs = px_battle_stat(&b->side[1], 3, 0);
        ++b->escape_attempts;
        const uint32_t odds = theirs ? mine * 128 / theirs + 30 * (b->escape_attempts - 1) : 256;
        if (mine >= theirs || odds > 255 || px_rng_below(&b->rng, 256) < odds) {
            say(b, "Got away safely!");
            b->state = PX_BATTLE_ESCAPED;
            return 1;
        }
        say(b, "You can't escape!");
        const int w = wild_choice(b);
        if (w >= 0 && use_move(b, 1, (unsigned)w, species_names, species_count, move_names, move_count))
            b->state = PX_BATTLE_LOST;
        return 1;
    }
    const int w = wild_choice(b);
    const struct px_move *mine = &b->data->moves[b->side[0].moves[player_move]];
    int player_first = 1;
    if (w >= 0) {
        const struct px_move *theirs = &b->data->moves[b->side[1].moves[w]];
        const int pa = (int8_t)mine->priority, pb = (int8_t)theirs->priority;
        const uint32_t sa = px_battle_stat(&b->side[0], 3, 0), sb = px_battle_stat(&b->side[1], 3, 0);
        player_first = pa != pb ? pa > pb : sa != sb ? sa > sb : (int)px_rng_below(&b->rng, 2) == 0;
    }
    for (unsigned order = 0; order < 2; ++order) {
        const unsigned side = (order == 0) == player_first ? 0 : 1;
        if (side == 1 && w < 0) continue;
        const unsigned slot = side ? (unsigned)w : (unsigned)player_move;
        if (use_move(b, side, slot, species_names, species_count, move_names, move_count)) {
            b->state = side == 0 ? PX_BATTLE_WON : PX_BATTLE_LOST;
            break;
        }
    }
    return 1;
}
