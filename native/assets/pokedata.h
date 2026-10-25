#ifndef PX_POKEDATA_H
#define PX_POKEDATA_H
#include <stddef.h>
#include <stdint.h>
/* Pokémon game data for the wild-encounter slice, as read by pk3DS 6daaca9
 * (PersonalInfoXY.cs, Move6.cs, Learnset6.cs, XYWE.cs; GARCReference.cs for
 * the archives) and checked against the English text archive (species
 * names a/0/7/4 file 80, move names file 14, type names file 17). Not traced
 * in the game's code.
 *
 *   personal  a/2/1/8 entry = species: 0x40 bytes
 *   moves     a/2/1/2 entry = move: 0x22 bytes (+2 padding)
 *   learnsets a/2/1/4 entry = species: (u16 move, s16 level) pairs ended by
 *             0xFFFF, 0xFFFF
 *   wild      ZO section 3 of the zone: 0x10-byte header, then 94 slots of
 *             u16 species (low 11 bits) | form << 11, u8 min level, u8 max
 *             level: grass 12, yellow/purple/red flowers 12 each, rough
 *             terrain 12, surf 5, rock smash 5, old/good/super rod 3 each,
 *             hordes 15 (3 x 5)
 *   types     DllBattle.cro at 0xD12A8 (pk3DS TypeChart6.cs, XY): 18 x 18
 *             bytes, attacking type by row, 0/2/4/8 = x0, x0.5, x1, x2 */
enum px_pokedata_result {
    PX_POKEDATA_OK = 0, PX_POKEDATA_INVALID_ARGUMENT, PX_POKEDATA_TRUNCATED, PX_POKEDATA_INVALID,
    PX_POKEDATA_NO_MEMORY
};
enum { PX_TYPE_COUNT = 18, PX_SLOT_COUNT = 94 };
/* Stats in personal order: HP, Attack, Defense, Speed, Sp. Atk, Sp. Def. */
struct px_personal {
    uint8_t base[6], types[2], catch_rate, evolution_stage, gender, growth, escape_rate;
    uint16_t abilities[3], base_exp, form_index;
    uint8_t form_count;
};
struct px_move {
    uint8_t type, quality, category, power, accuracy, pp, priority, hit_min, hit_max;
    uint16_t inflict, effect;
    uint8_t inflict_percent, inflict_duration, turn_min, turn_max, crit_stage, flinch, recoil, healing, target;
    uint8_t stat[3], stat_percent[3];
    int8_t stage[3];
    uint32_t flags;
};
enum { PX_MAX_LEARNSET = 256 };
struct px_learnset {
    uint32_t count;
    uint16_t move[PX_MAX_LEARNSET];
    int16_t level[PX_MAX_LEARNSET];
};
struct px_encounter_slot { uint16_t species, form; uint8_t min_level, max_level; };
struct px_encounter_table {
    uint8_t header[16];
    struct px_encounter_slot slots[PX_SLOT_COUNT];
};
enum px_slot_kind { PX_SLOTS_GRASS = 0, PX_SLOTS_YELLOW, PX_SLOTS_PURPLE, PX_SLOTS_RED, PX_SLOTS_ROUGH,
                    PX_SLOTS_SURF, PX_SLOTS_ROCK_SMASH, PX_SLOTS_OLD_ROD, PX_SLOTS_GOOD_ROD, PX_SLOTS_SUPER_ROD,
                    PX_SLOTS_HORDE, PX_SLOT_KINDS };
/* First slot and slot count of a kind. */
void px_slot_range(enum px_slot_kind kind, unsigned *first, unsigned *count);

enum px_pokedata_result px_personal_decode(const uint8_t *data, size_t size, struct px_personal *out);
enum px_pokedata_result px_move_decode(const uint8_t *data, size_t size, struct px_move *out);
/* Pairs up to the terminator; a missing terminator or more than 256 pairs
 * is invalid; level must be 0..100. */
enum px_pokedata_result px_learnset_decode(const uint8_t *data, size_t size, struct px_learnset *out);
/* Constructor move ordering (00150064): keep unique eligible moves in order,
 * shifting the oldest out when full. Pure native reference helper; the viewer
 * uses game/battle.h's reconstructed path with the loader's 26-entry limit.
 * Returns how many slots hold a move (unused are 0). */
unsigned px_learnset_moves_at(const struct px_learnset *learnset, unsigned level, uint16_t out[4]);
/* An empty section (no wild Pokémon) gives PX_POKEDATA_TRUNCATED. */
enum px_pokedata_result px_encounter_table_decode(const uint8_t *data, size_t size, struct px_encounter_table *out);
/* Copies the chart; every byte must be 0, 2, 4 or 8. */
enum px_pokedata_result px_type_chart_decode(const uint8_t *cro, size_t size, uint8_t out[PX_TYPE_COUNT][PX_TYPE_COUNT]);
const char *px_pokedata_result_string(enum px_pokedata_result result);
#endif
