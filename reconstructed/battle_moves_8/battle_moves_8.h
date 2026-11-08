#ifndef PX_BATTLE_MOVES_8_H
#define PX_BATTLE_MOVES_8_H

#include "../battle_ability/battle_ability.h"

/* DllBattle move handlers of event 0x2e, which decides whether a move fails
 * against its target (static analysis addresses; runtime addresses are
 * module-relative). Analyst names. Each requires key 0x03 (attacker) to be
 * its battler; the defender is the low byte of key 0x04; key 0x41 = 1 makes
 * the move fail, and key 0x40 holds the address of the failure message.
 * Values (10874420): 0xd HP, 0x11 ability, 0x12 gender (2 genderless).
 * Statuses (10856a88): 2 sleep. Types (1086f960): 2 Flying, 7 Ghost; the
 * packed type word (10870994) holds three 5-bit types (0x12 none). */
struct px_battle_moves_8_api {
    /* Event stack, battler lookup and pokemon_value. */
    const struct px_battle_ability_api *base;
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);               /* 10856a88 */
    uint32_t (*has_type)(void *user, uint32_t pokemon, uint32_t type);                   /* 1086f960 */
    uint32_t (*types)(void *user, uint32_t pokemon);                                     /* 10870994 */
    uint32_t (*airborne)(void *user, uint32_t context, uint32_t id);                     /* 1084bf24 */
    uint32_t (*turn_flag)(void *user, uint32_t pokemon, uint32_t flag);                  /* 1084c818; 3 moved */
    /* Copies the battler's chosen action to `out`; 0 when there is none. */
    uint32_t (*chosen_action)(void *user, uint32_t context, uint32_t id, uint32_t out);  /* 1084b784 */
    uint32_t (*action_move)(void *user, uint32_t action);                                /* 1084b648; 0 not a move */
    uint32_t (*move_damages)(void *user, uint32_t move);                                 /* 10800434 */
    void (*message)(void *user, uint32_t at, uint32_t kind, uint32_t message);           /* 1084d2d4 */
    /* Logical word standing in for Sucker Punch's stack slot (not cleared). */
    uint32_t action_scratch;
};

#define PX_BATTLE_MOVES_8_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_8_api *api)
PX_BATTLE_MOVES_8_HANDLER(px_battle_attract_check);      /* 108bb560, move 213: genders */
PX_BATTLE_MOVES_8_HANDLER(px_battle_captivate_check);    /* 108af1e4, move 445: genders */
PX_BATTLE_MOVES_8_HANDLER(px_battle_dream_eater_check);  /* 108afe50, move 138: asleep */
PX_BATTLE_MOVES_8_HANDLER(px_battle_nightmare_check);    /* 108b4b64, move 171: asleep */
PX_BATTLE_MOVES_8_HANDLER(px_battle_synchronoise_check); /* 108b5494, move 485: a shared type */
PX_BATTLE_MOVES_8_HANDLER(px_battle_sucker_punch_check); /* 108b5624, move 389: a damaging move chosen and not yet used */
PX_BATTLE_MOVES_8_HANDLER(px_battle_rototiller_check);   /* 108b9034, move 563: not airborne */
PX_BATTLE_MOVES_8_HANDLER(px_battle_sky_drop_check);     /* 108b96ac, move 507: not Flying */
PX_BATTLE_MOVES_8_HANDLER(px_battle_worry_seed_check);   /* 108b9b70, move 388: not Truant or Insomnia */
PX_BATTLE_MOVES_8_HANDLER(px_battle_mean_look_check);    /* 108ba338, moves 169, 212, 335 (Spider Web, Mean Look, Block): not Ghost */
PX_BATTLE_MOVES_8_HANDLER(px_battle_endeavor_check);     /* 108bb9cc, move 283: defender HP above the attacker's */

#endif
