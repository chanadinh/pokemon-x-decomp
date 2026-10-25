#include "battle.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../../reconstructed/pokemon_stats/stats_math.h"
#include "../../reconstructed/pokemon_moves/pokemon_moves.h"
#include "../platform/guest.h"

uint32_t px_rng_next(struct px_rng *rng)
{
    uint32_t x = rng->state ? rng->state : UINT32_C(0x9e3779b9);
    x ^= x << 13; x ^= x >> 17; x ^= x << 5;
    rng->state = x;
    return x;
}

uint32_t px_rng_below(struct px_rng *rng, uint32_t n) { return n ? px_rng_next(rng) % n : 0; }

/* The reconstructed nature adjustment reads the table at its game address,
 * 0x00570f6c (rodata); the guest maps the 125 bytes there (zeros, which are
 * neutral, without a table). */
enum { NATURE_TABLE = 0x00570f6c };
static const uint8_t neutral_natures[125];

/* Stats of the battler's species, level, IVs and nature through the
 * reconstructed formulas (EVs 0). */
static void compute_stats(const struct px_battle_data *d, struct px_battler *out)
{
    const struct px_personal *p = &d->personal[out->species];
    struct px_guest guest;
    px_guest_init(&guest);
    px_guest_map_view(&guest, NATURE_TABLE, 125, PX_GUEST_READ, (void *)(d->natures ? d->natures : neutral_natures),
                      "natures");
    const struct px_pokemon_api memory = px_guest_pokemon_api(&guest);
    out->stats[0] = (uint16_t)px_pokemon_hp_value(out->species, out->level, p->base[0], out->iv[0], 0);
    static const enum px_stat stat[6] = {PX_STAT_HP, PX_STAT_ATTACK, PX_STAT_DEFENSE, PX_STAT_SPEED,
                                         PX_STAT_SPECIAL_ATTACK, PX_STAT_SPECIAL_DEFENSE};
    for (unsigned k = 1; k < 6; ++k)
        out->stats[k] = (uint16_t)px_pokemon_stat_value(out->level, p->base[k], out->iv[k], 0, out->nature, stat[k],
                                                        &memory);
}

/* The reconstructed level-from-growth routine reads the selected growth
 * table through the pointer at 0x005d17ac (in the game's data segment) and
 * then table + 8 + 4 x level. The guest holds that pointer cell and, at the
 * start of the heap, the selected a/2/1/7 member behind an 8-byte header;
 * the selection callback fills it (zeros for a species out of range). */
enum { GROWTH_POINTER = 0x005d17ac, GROWTH_TABLE = PX_GUEST_HEAP };
struct growth_view { const struct px_battle_data *data; uint8_t cell[4], table[8 + 4 * 101]; };
static void growth_select(void *user, uint32_t species, uint32_t form)
{
    (void)form;
    struct growth_view *g = user;
    const uint32_t group = species < g->data->personal_count ? g->data->personal[species].growth : UINT32_MAX;
    memset(g->table, 0, sizeof g->table);
    for (unsigned l = 0; group < g->data->growth_count && l <= 100; ++l)
        for (unsigned k = 0; k < 4; ++k) g->table[8 + 4 * l + k] = (uint8_t)(g->data->growth[group][l] >> (8 * k));
}

uint8_t px_battle_level_for(const struct px_battle_data *d, uint16_t species, uint32_t exp)
{
    if (!d->growth || species >= d->personal_count) return 1;
    struct growth_view view;
    memset(&view, 0, sizeof view);
    view.data = d;
    for (unsigned k = 0; k < 4; ++k) view.cell[k] = (uint8_t)(GROWTH_TABLE >> (8 * k));
    struct px_guest guest;
    px_guest_init(&guest);
    px_guest_map_view(&guest, GROWTH_POINTER, 4, PX_GUEST_READ | PX_GUEST_WRITE, view.cell, "growth pointer");
    px_guest_map_view(&guest, GROWTH_TABLE, sizeof view.table, PX_GUEST_READ, view.table, "growth table");
    const struct px_pokemon_api memory = px_guest_pokemon_api(&guest);
    struct px_data_api api = {&memory, &view, growth_select};
    return (uint8_t)px_pokemon_level_from_growth(species, 0, exp, &api);
}

uint32_t px_battle_exp_gain(const struct px_battle_data *d, const struct px_battler *fainted)
{
    return (uint32_t)d->personal[fainted->species].base_exp * fainted->level / 7;
}

uint32_t px_battle_catch_threshold(const struct px_battle_data *d, const struct px_battler *w, uint32_t ball10)
{
    const uint32_t max = w->stats[0], rate = d->personal[w->species].catch_rate;
    const uint32_t a = max ? (3 * max - 2 * w->hp) * rate * ball10 / (30 * max) : 0;
    if (a >= 255) return 65536;
    if (!a) return 0;
    return (uint32_t)floor(65536.0 / pow(255.0 / a, 0.1875));
}

/* Adapter for 00168bbc's selector 5. Resource caching/loading stays in the
 * native data layer; the PP arithmetic runs through reconstructed 00150220. */
static uint32_t move_pp_value(void *user, uint32_t move, uint32_t selector)
{
    const struct px_battle_data *d = user;
    return selector == 5 && move < d->move_count ? d->moves[move].pp : 0;
}

int px_battler_make(const struct px_battle_data *d, uint16_t species, uint8_t level, const uint8_t iv[6],
                    uint8_t nature, const uint16_t moves[4], struct px_battler *out)
{
    memset(out, 0, sizeof *out);
    if (!d || species == 0 || species >= d->personal_count || level < 1 || level > 100 || nature >= 25) return 0;
    const struct px_personal *p = &d->personal[species];
    out->species = species; out->level = level; out->nature = nature;
    out->types[0] = p->types[0]; out->types[1] = p->types[1];
    memcpy(out->iv, iv, 6);
    compute_stats(d, out);
    out->hp = out->stats[0];
    if (d->growth && p->growth < d->growth_count) out->exp = d->growth[p->growth][level];
    const struct px_move_api move_api = {.user = (void *)d, .move_value = move_pp_value};
    for (unsigned k = 0; k < 4; ++k) {
        if (moves[k] >= d->move_count) { memset(out, 0, sizeof *out); return 0; }
        out->moves[k] = moves[k];
        out->pp[k] = moves[k] ? (uint8_t)px_pokemon_maximum_pp(moves[k], 0, &move_api) : 0;
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
    if (b->log_count < sizeof b->log / sizeof b->log[0]) {
        b->log_event[b->log_count] = PX_LOG_NONE;
        snprintf(b->log[b->log_count++], sizeof b->log[0], "%s", text);
    }
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
    const uint32_t used = b->log_count;
    say(b, line);
    uint8_t unlogged = 0, *event = b->log_count > used ? &b->log_event[used] : &unlogged;
    *event = (uint8_t)((m->category == 1 ? PX_LOG_PHYSICAL : m->category == 2 ? PX_LOG_SPECIAL : PX_LOG_STATUS) |
                       (side ? PX_LOG_SIDE1 : 0));
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
    if (damage) *event |= PX_LOG_HIT;
    if (critical) say(b, "A critical hit!");
    if (effectiveness > 16) say(b, "It's super effective!");
    else if (effectiveness < 16) say(b, "It's not very effective...");
    if (t->hp == 0) {
        snprintf(line, sizeof line, "%s fainted!", target_name);
        const uint32_t at = b->log_count;
        say(b, line);
        if (b->log_count > at) b->log_event[at] = (uint8_t)(PX_LOG_FAINT | (side ? 0 : PX_LOG_SIDE1));
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
    b->party[0] = *player; b->party_count = 1; b->active = 0;
    b->rng.state = seed ? seed : 1;
    b->state = PX_BATTLE_CHOOSING;
}

int px_battle_set_party(struct px_battle *b, const struct px_battler *party, uint32_t count, uint32_t active)
{
    if (!b || !party || !count || count > 6 || active >= count || !party[active].hp) return 0;
    memcpy(b->party, party, count * sizeof *party);
    b->party_count = count; b->active = active;
    b->side[0] = party[active];
    memset(b->side[0].stages, 0, sizeof b->side[0].stages);
    return 1;
}

/* party[active] follows side[0]; stat stages belong to the battle only. */
static void sync_party(struct px_battle *b)
{
    b->party[b->active] = b->side[0];
    memset(b->party[b->active].stages, 0, sizeof b->party[b->active].stages);
}

/* The player's active Pokémon fainted: another must be sent out, or the
 * battle is lost. */
static void player_fainted(struct px_battle *b)
{
    sync_party(b);
    for (uint32_t i = 0; i < b->party_count; ++i)
        if (i != b->active && b->party[i].hp) { b->state = PX_BATTLE_SWITCH; return; }
    say(b, "You have no more Pokémon that can fight!");
    b->state = PX_BATTLE_LOST;
}

static int wild_choice(struct px_battle *b)
{
    unsigned usable[4], n = 0;
    for (unsigned k = 0; k < 4; ++k) if (b->side[1].moves[k] && b->side[1].pp[k]) usable[n++] = k;
    return n ? (int)usable[px_rng_below(&b->rng, n)] : -1;
}

/* Experience, level-ups and new moves after the wild Pokémon fainted. */
static void award(struct px_battle *b, const char *const *species_names, uint32_t species_count,
                  const char *const *move_names, uint32_t move_count)
{
    struct px_battler *me = &b->side[0];
    char name[64], line[96];
    species_name(b, 0, species_names, species_count, name, sizeof name);
    const uint32_t gain = px_battle_exp_gain(b->data, &b->side[1]);
    if (!b->data->growth || !gain) return;
    me->exp += gain;
    snprintf(line, sizeof line, "%s gained %u Exp. Points!", name, (unsigned)gain);
    say(b, line);
    const uint8_t target = px_battle_level_for(b->data, me->species, me->exp);
    while (me->level < target && me->level < 100) {
        const uint16_t old_max = me->stats[0];
        ++me->level;
        compute_stats(b->data, me);
        me->hp = (uint16_t)(me->hp + (me->stats[0] - old_max));
        snprintf(line, sizeof line, "%s grew to Lv. %u!", name, me->level);
        say(b, line);
        const struct px_learnset *l = b->data->learnsets && me->species < b->data->learnset_count ?
                                      &b->data->learnsets[me->species] : NULL;
        for (uint32_t i = 0; l && i < l->count; ++i) {
            if (l->level[i] != me->level) continue;
            const uint16_t id = l->move[i];
            int known = 0, free_slot = -1;
            for (int k = 0; k < 4; ++k) {
                known |= me->moves[k] == id;
                if (!me->moves[k] && free_slot < 0) free_slot = k;
            }
            if (known || id >= b->data->move_count) continue;
            const char *move = move_names && id < move_count ? move_names[id] : "a new move";
            if (free_slot >= 0) {
                me->moves[free_slot] = id; me->pp[free_slot] = b->data->moves[id].pp;
                snprintf(line, sizeof line, "%s learned %s!", name, move);
            } else snprintf(line, sizeof line, "%s did not learn %s.", name, move);
            say(b, line);
        }
    }
}

/* A Poké Ball: 1 when caught. */
static int throw_ball(struct px_battle *b, const char *const *species_names, uint32_t species_count)
{
    char name[64], line[96];
    --b->balls;
    say(b, "You threw a Poké Ball!");
    const uint32_t threshold = px_battle_catch_threshold(b->data, &b->side[1], 10);
    unsigned shakes = 0;
    while (shakes < 4 && (threshold >= 65536 || px_rng_below(&b->rng, 65536) < threshold)) ++shakes;
    const uint16_t s = b->side[1].species;
    if (species_names && s < species_count) snprintf(name, sizeof name, "%s", species_names[s]);
    else snprintf(name, sizeof name, "#%u", s);
    if (shakes == 4) {
        snprintf(line, sizeof line, "Gotcha! %s was caught!", name);
        say(b, line);
        return 1;
    }
    static const char *const free[4] = {"Oh no! The Pokémon broke free!", "Aww! It appeared to be caught!",
                                        "Aargh! Almost had it!", "Gah! It was so close, too!"};
    say(b, free[shakes]);
    return 0;
}

int px_battle_turn(struct px_battle *b, int player_move, const char *const *species_names, uint32_t species_count,
                   const char *const *move_names, uint32_t move_count)
{
    if (b->state != PX_BATTLE_CHOOSING) return 0;
    if (player_move >= 4 || player_move < -2 || (player_move == -2 && !b->balls) ||
        (player_move >= 0 && (!b->side[0].moves[player_move] || !b->side[0].pp[player_move])))
        return 0;
    b->log_count = 0;
    ++b->turn;
    if (player_move == -2) {
        if (throw_ball(b, species_names, species_count)) { b->state = PX_BATTLE_CAUGHT; return 1; }
        const int w = wild_choice(b);
        if (w >= 0 && use_move(b, 1, (unsigned)w, species_names, species_count, move_names, move_count))
            player_fainted(b);
        sync_party(b);
        return 1;
    }
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
            player_fainted(b);
        sync_party(b);
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
            if (side == 0) {
                b->state = PX_BATTLE_WON;
                award(b, species_names, species_count, move_names, move_count);
            } else player_fainted(b);
            break;
        }
    }
    sync_party(b);
    return 1;
}

int px_battle_send_out(struct px_battle *b, uint32_t slot, const char *const *species_names, uint32_t species_count,
                       const char *const *move_names, uint32_t move_count)
{
    if ((b->state != PX_BATTLE_CHOOSING && b->state != PX_BATTLE_SWITCH) || slot >= b->party_count ||
        slot == b->active || !b->party[slot].hp) return 0;
    const int forced = b->state == PX_BATTLE_SWITCH;
    char name[64], line[96];
    b->log_count = 0;
    if (!forced) {
        ++b->turn;
        species_name(b, 0, species_names, species_count, name, sizeof name);
        snprintf(line, sizeof line, "Come back, %s!", name);
        say(b, line);
    }
    sync_party(b);
    b->active = slot;
    b->side[0] = b->party[slot];
    memset(b->side[0].stages, 0, sizeof b->side[0].stages);
    species_name(b, 0, species_names, species_count, name, sizeof name);
    snprintf(line, sizeof line, "Go, %s!", name);
    const uint32_t at = b->log_count;
    say(b, line);
    if (b->log_count > at) b->log_event[at] = PX_LOG_SEND;
    b->state = PX_BATTLE_CHOOSING;
    if (!forced) {
        const int w = wild_choice(b);
        if (w >= 0 && use_move(b, 1, (unsigned)w, species_names, species_count, move_names, move_count))
            player_fainted(b);
    }
    sync_party(b);
    return 1;
}
