#ifndef PX_BATTLE_H
#define PX_BATTLE_H
#include <stdint.h>

#include "../assets/pokedata.h"

/* A one-on-one wild battle for the playable slice.
 *
 * From the game: species stats, types, moves and learnsets (assets/
 * pokedata.h), the type chart (DllBattle.cro), the nature table (code.bin
 * 0x00570f6c) and the HP and stat formulas, which run the reconstructed
 * routines of reconstructed/pokemon_stats (0x00168b84 and the five scalar
 * entries). Not yet reconstructed from DllBattle.cro, and so community-
 * documented formulas stated here (Generation VI as published by Bulbapedia
 * and Pokémon Showdown), each an inspection choice to be replaced:
 *
 *   damage = floor(floor(floor(2 * L / 5 + 2) * power * A / D) / 50) + 2,
 *            then x1.5 on a critical hit, x(85..100)/100, x1.5 for the same
 *            type as the user (STAB), x each target type's chart factor / 4,
 *            each step floored; at least 1 unless a factor is 0.
 *   A, D:    physical uses Attack/Defense, special Sp. Atk/Sp. Def, each with
 *            its stage (2 + s) / 2 or 2 / (2 - s); a critical hit ignores the
 *            attacker's negative and the defender's positive stages.
 *   critical: 1 in 16 at the move's stage 0, 1/8 at 1, 1/2 at 2, always at 3+.
 *   accuracy: hits when accuracy is 101 (never misses) or a roll below
 *            accuracy x (3 + s) / 3 or 3 / (3 - s), s = accuracy - evasion.
 *   order:    higher priority first, then higher speed (with stage), ties
 *            by a coin flip.
 *   status moves change stats by the move's own stat/stage/percent fields
 *            (targets Self, AllyOrSelf and SideSelf mean the user); ailments
 *            (inflict) and other effect sequences are not executed.
 *   running:  always when faster, else floor(speed x 128 / wild speed) +
 *            30 x earlier attempts, compared with a byte roll.
 *   wild AI:  a random move with PP.
 *
 * Every random number comes from the battle's xorshift32 generator. */
struct px_rng { uint32_t state; };
uint32_t px_rng_next(struct px_rng *rng);
uint32_t px_rng_below(struct px_rng *rng, uint32_t n); /* 0..n-1; n >= 1 */

struct px_battle_data {
    const struct px_personal *personal; uint32_t personal_count;
    const struct px_move *moves; uint32_t move_count;
    const uint8_t (*chart)[PX_TYPE_COUNT];
    const uint8_t *natures; /* 25 x 5 signed bytes from code.bin, or NULL: neutral */
};

/* Stats in personal order: HP, Attack, Defense, Speed, Sp. Atk, Sp. Def.
 * stages: index by the move data's stat number (1 Attack, 2 Defense,
 * 3 Sp. Atk, 4 Sp. Def, 5 Speed, 6 accuracy, 7 evasion), -6..6. */
struct px_battler {
    uint16_t species;
    uint8_t level, nature, types[2], iv[6];
    uint16_t stats[6], hp;
    uint16_t moves[4];
    uint8_t pp[4];
    int8_t stages[8];
};
/* Stats from the reconstructed formulas (EVs 0); HP full; PP from the move
 * data. 0 when the species or a move is outside the tables. */
int px_battler_make(const struct px_battle_data *data, uint16_t species, uint8_t level, const uint8_t iv[6],
                    uint8_t nature, const uint16_t moves[4], struct px_battler *out);

enum px_battle_state { PX_BATTLE_CHOOSING = 0, PX_BATTLE_WON, PX_BATTLE_LOST, PX_BATTLE_ESCAPED };
struct px_battle {
    const struct px_battle_data *data;
    struct px_battler side[2]; /* 0 the player's, 1 the wild Pokémon */
    struct px_rng rng;
    uint32_t turn, escape_attempts;
    enum px_battle_state state;
    char log[16][96];          /* this turn's messages, in order */
    uint32_t log_count;
};
void px_battle_start(struct px_battle *battle, const struct px_battle_data *data, const struct px_battler *player,
                     const struct px_battler *wild, uint32_t seed);
/* One turn: the player's move slot 0..3, or -1 to run. A move without PP
 * or a finished battle does nothing and returns 0. Names: species and move
 * name tables (may be NULL; numbers are printed instead). */
int px_battle_turn(struct px_battle *battle, int player_move, const char *const *species_names,
                   uint32_t species_count, const char *const *move_names, uint32_t move_count);
/* The damage formula above, exposed for the independent comparison. roll is
 * 85..100. Returns the damage; *effectiveness gets the chart product in
 * sixteenths (0, 4, 8, 16, 32, 64). */
uint32_t px_battle_damage(const struct px_battle_data *data, const struct px_battler *attacker,
                          const struct px_battler *defender, uint16_t move, int critical, uint32_t roll,
                          uint32_t *effectiveness);
/* Stat with its stage, as used above (index in personal order 1..5). */
uint32_t px_battle_stat(const struct px_battler *b, unsigned personal_index, int ignore);
#endif
