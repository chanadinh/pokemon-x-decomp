#ifndef PX_BATTLE_MOVES_3_H
#define PX_BATTLE_MOVES_3_H

#include "../battle_ability/battle_ability.h"

/* DllBattle move handlers for fixed damage and type matchups (static analysis
 * addresses; runtime addresses are module-relative). Analyst names. Events:
 * 0x4e fixed damage (a nonzero key 0x38 is the damage), 0x50 after it,
 * 0x44 single type matchup (keys 0x15 defending type, 0x16 attacking type,
 * 0x17 second attacking type, 0x39 effectiveness code), 0x45 immunity
 * (key 0x39 replaces it), 0x46 airborne check (key 0x42 keeps the code),
 * 0x43 and 0x38 (keys 0x53 and 0x4c). Key 0x03 is the attacker, 0x04 the
 * defender. Values (10874420): 0xd HP, 0xf level. Types: 2 Flying, 4 Ground,
 * 10 Water. Move categories (108007dc): 1 physical, 2 special. */
struct px_battle_moves_3_api {
    /* Event stack, battler lookup, pokemon_value and move_category (108007dc). */
    const struct px_battle_ability_api *base;
    uint32_t (*random)(void *user, uint32_t range);                                     /* 1084c7bc */
    /* Copies damage record `index` of the battler to the 8 bytes at `out`
     * (+0 move, +2 damage, +5 attacker id); 0 when there is none. */
    uint32_t (*damage_record)(void *user, uint32_t pokemon, uint32_t kind, uint32_t index, uint32_t out); /* 1084bac4 */
    uint32_t (*is_ally)(void *user, uint32_t id, uint32_t other);                       /* 1084c5a8 */
    /* Logical 12 bytes standing in for the counter routine's stack buffer. */
    uint32_t record_scratch;
    /* Relocated code address Brick Break stores (literal 108ba334, static 108baba4). */
    uint32_t brick_break_hook;
};

/* 1084b028: the latest damage record of a battler not from an ally (and of the
 * category `kind` when nonzero) sets the damage to its damage times `factor`
 * (4096 = x1). The arguments are those of the original; `work` is unused. */
void px_battle_counter_damage(uint32_t context, uint32_t id, uint32_t work, uint32_t kind, uint32_t factor,
                              const struct px_battle_moves_3_api *api);
/* 1084aef8 / 1084aeec: handler word +0xc. */
void px_battle_set_handler_word(uint32_t handler, uint32_t value, const struct px_pokemon_api *memory);
void px_battle_clear_handler_word(uint32_t handler, const struct px_pokemon_api *memory);

#define PX_BATTLE_MOVES_3_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_3_api *api)
/* Event 0x4e. */
PX_BATTLE_MOVES_3_HANDLER(px_battle_sonic_boom);       /* 108b2e8c, move 49 */
PX_BATTLE_MOVES_3_HANDLER(px_battle_dragon_rage);      /* 108b5414, move 82 */
PX_BATTLE_MOVES_3_HANDLER(px_battle_seismic_toss);     /* 108b4578, moves 69 and 101 (Night Shade) */
PX_BATTLE_MOVES_3_HANDLER(px_battle_psywave);          /* 108b2cfc, move 149 */
PX_BATTLE_MOVES_3_HANDLER(px_battle_super_fang);       /* 108b5b88, move 162 */
PX_BATTLE_MOVES_3_HANDLER(px_battle_endeavor);         /* 108b1824, move 283 */
PX_BATTLE_MOVES_3_HANDLER(px_battle_final_gambit);     /* 108bab60, move 515 */
PX_BATTLE_MOVES_3_HANDLER(px_battle_bide_damage);      /* 108b68c4, move 117 (Bide) */
PX_BATTLE_MOVES_3_HANDLER(px_battle_counter);          /* 108b94c0, move 68 */
PX_BATTLE_MOVES_3_HANDLER(px_battle_mirror_coat);      /* 108bacfc, move 243 */
PX_BATTLE_MOVES_3_HANDLER(px_battle_metal_burst);      /* 108bb3a4, move 368 */
PX_BATTLE_MOVES_3_HANDLER(px_battle_brick_break_mark); /* 108ba304, move 280 (Brick Break) */
/* Event 0x50. */
PX_BATTLE_MOVES_3_HANDLER(px_battle_brick_break_clear);/* 108bb244, move 280 (Brick Break) */
/* Events 0x43, 0x44, 0x45, 0x46 and 0x38. */
PX_BATTLE_MOVES_3_HANDLER(px_battle_thunder_wave_check);    /* 108af540, move 86 (Thunder Wave), event 0x43 */
PX_BATTLE_MOVES_3_HANDLER(px_battle_thousand_arrows_check); /* 108bb87c, move 614 (Thousand Arrows), event 0x43 */
PX_BATTLE_MOVES_3_HANDLER(px_battle_freeze_dry);            /* 108b020c, move 573, event 0x44 */
PX_BATTLE_MOVES_3_HANDLER(px_battle_flying_press);          /* 108b4cdc, move 560, event 0x44 */
PX_BATTLE_MOVES_3_HANDLER(px_battle_thousand_arrows_immunity); /* 108bbcf8, move 614, event 0x45 */
PX_BATTLE_MOVES_3_HANDLER(px_battle_thousand_arrows_airborne); /* 108bbccc, move 614, event 0x46 */
PX_BATTLE_MOVES_3_HANDLER(px_battle_chip_away_matchup);     /* 108ba544, moves 498 and 533 (Sacred Sword), event 0x38 */

#endif
