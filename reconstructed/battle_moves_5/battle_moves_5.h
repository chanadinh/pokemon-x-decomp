#ifndef PX_BATTLE_MOVES_5_H
#define PX_BATTLE_MOVES_5_H

#include "../battle_ability/battle_ability.h"

/* DllBattle move handlers that set field conditions (event 0xaa) and side
 * conditions (event 0xad) (static analysis addresses; runtime addresses are
 * module-relative). Analyst names. Each requires key 0x03 (attacker) to be
 * its battler and commits action records (10869f7c / 10867b48):
 *   kind 0x1c field condition: +4 condition, +5 terrain, word +8 turn count,
 *        message at +0xc; kind 0x1d ends field condition +4;
 *   kind 0x19 side condition: +4 condition, word +8 count, +0xc side,
 *        message at +0x10;
 *   kind 0x3 message at +4; kind 0xf (Haze) +4 count, battler ids from +5;
 *   kind 0x27 (Quick and Wide Guard) +4 battler, +5 counter 3, +6 value,
 *        flag 0x1000000.
 * Field conditions: 1 Trick Room, 2 Gravity, 4 Water Sport, 5 Mud Sport,
 * 6 Wonder Room, 7 Magic Room, 8 Ion Deluge, 9 Fairy Lock, 10 terrain
 * (1 grassy, 2 misty, 3 electric). Side conditions: 0 Reflect, 1 Light
 * Screen, 2 Safeguard, 3 Mist, 4 Tailwind, 5 Lucky Chant, 6 Spikes, 7 Toxic
 * Spikes, 8 Stealth Rock, 9 Wide Guard, 10 Quick Guard, 14 Sticky Web,
 * 15 Mat Block, 16 Crafty Shield. */
struct px_battle_moves_5_api {
    /* Event stack, battler lookup and side_battlers (1084bc20). */
    const struct px_battle_ability_api *base;
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id);    /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);                  /* 10867b48 */
    void (*message)(void *user, uint32_t at, uint32_t kind, uint32_t message);              /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t at, uint32_t value);                           /* 1084d29c */
    uint32_t (*field_active)(void *user, uint32_t context, uint32_t condition);             /* 1084c374 */
    uint32_t (*terrain)(void *user, uint32_t context);                                      /* 1084b8d4 */
    void (*effect)(void *user, uint32_t context, uint32_t code, uint32_t id);               /* 1084c948 */
    uint32_t (*opposing_selector)(void *user, uint32_t context, uint32_t id);               /* 1084cb78 */
    uint32_t (*side_of)(void *user, uint32_t id);                                           /* 1084bfc0 */
    uint32_t (*opposing_side_of)(void *user, uint32_t id);                                  /* 1084bab4 */
    void (*happy_hour)(void *user, uint32_t context, uint32_t id, uint32_t value);          /* 1084c3cc */
    uint32_t (*counter)(void *user, uint32_t pokemon, uint32_t index);                      /* 1084bb2c */
};

/* 1084be3c: a turn count word for `turns` turns. */
uint32_t px_battle_turn_count(uint32_t turns);
/* 1084bcb0: a side-condition record (kind 0x19) for the attacker; its message
 * names the side. The last four arguments are passed on the stack. */
void px_battle_side_condition(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, uint32_t side,
                              uint32_t condition, uint32_t count, uint32_t message, const struct px_battle_moves_5_api *api);

#define PX_BATTLE_MOVES_5_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_5_api *api)
/* Event 0xaa. */
PX_BATTLE_MOVES_5_HANDLER(px_battle_gravity);          /* 108af744, move 356 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_mud_sport);        /* 108b14a8, move 300 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_water_sport);      /* 108b2940, move 346 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_fairy_lock);       /* 108b1700, move 587 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_ion_deluge);       /* 108b6294, move 569 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_trick_room);       /* 108b3164, move 433 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_wonder_room);      /* 108b4988, move 472 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_magic_room);       /* 108b2738, move 478 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_grassy_terrain);   /* 108b3684, move 580 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_misty_terrain);    /* 108b28bc, move 581 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_electric_terrain); /* 108b15c8, move 604 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_haze);             /* 108b2630, move 114 */
/* Event 0xad. */
PX_BATTLE_MOVES_5_HANDLER(px_battle_reflect);          /* 108b2d6c, move 115 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_light_screen);     /* 108b59f0, move 113 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_safeguard);        /* 108b70bc, move 219 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_mist);             /* 108b2e34, move 54 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_tailwind);         /* 108aed68, move 366 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_lucky_chant);      /* 108b0db4, move 381 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_crafty_shield);    /* 108b4784, move 578 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_quick_guard);      /* 108b176c, move 501 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_wide_guard);       /* 108b320c, move 469 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_mat_block);        /* 108b6300, move 561 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_spikes);           /* 108b092c, move 191 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_toxic_spikes);     /* 108b0140, move 390 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_stealth_rock);     /* 108b5440, move 446 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_sticky_web);       /* 108b5074, move 564 */
PX_BATTLE_MOVES_5_HANDLER(px_battle_happy_hour);       /* 108b1ae0, move 603 */

#endif
