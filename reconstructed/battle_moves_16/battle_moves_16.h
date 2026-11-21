#ifndef PX_BATTLE_MOVES_16_H
#define PX_BATTLE_MOVES_16_H

#include "../battle_ability/battle_ability.h"
#include "../battle_handler/battle_handler.h"

/* DllBattle move handlers of events 0x2a-0x53 (static analysis addresses;
 * runtime addresses are module-relative). Analyst names. Events: 0x2a and
 * 0x2b choose a target (key 0x04, a value); 0x35, 0x36, 0x39, 0x3a, 0x42 and
 * 0x48 are small effects; 0x53 follows the move. Keys: 0x03 attacker, 0x04
 * defender (ids except where rewritten), 0x12 move, 0x14 the move used
 * (halfwords), 0x2b, 0x2c, 0x31-0x33, 0x43, 0x47, 0x48, 0x53 (values).
 * Work words: +0 (Present's flag, Bide's damage count), +4, +8 (Bide's chosen
 * target), +0x18 (word 6: Rage active).
 * Records (10869f7c/10867b48): kind 3 message at +4; kind 4 damage to a
 * battler (+4 halfword, +6 battler, message at +8); kind 6 damage (+4, +6,
 * +7 flags, message at +0xc); kind 9 PP (+5, +6, +4 signed, +7, message at
 * +8); kind 0xd stat change (+4 stat, +0xc, +0x16, +0x17, +0x18, message at
 * +0x1c); kind 0xf (+4, +5); kind 0x23 (+4, +5, halfword +6) and kind 0x24
 * (+4, +5). Scratch memory: 16 bytes for the stack buffer the damage-record
 * and side-battler services fill. */
struct px_battle_moves_16_api {
    /* Event stack, battler lookup, side_of (10873158), move_category
     * (108007dc), pokemon_status (1086d8ac) and side_battlers (1084bc20). */
    const struct px_battle_ability_api *base;
    /* Registered handlers (a handler removes itself). */
    const struct px_battle_handler_api *handlers;
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id);    /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);                  /* 10867b48 */
    void (*message)(void *user, uint32_t at, uint32_t kind, uint32_t message);              /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t at, uint32_t value);                           /* 1084d29c */
    uint32_t (*chance)(void *user, uint32_t percent);                                       /* 1084c0f8 */
    void (*set_effect_flag)(void *user, uint32_t context, uint32_t value);                  /* 1084bef8 */
    uint32_t (*full_hp)(void *user, uint32_t pokemon);                                      /* 1084d318 */
    uint32_t (*hp_fraction)(void *user, uint32_t pokemon, uint32_t divisor);                /* 1084d2f4 */
    uint32_t (*weather)(void *user, uint32_t context);                                      /* 1084be58 */
    uint32_t (*no_hp)(void *user, uint32_t pokemon);                                        /* 1085f844 */
    uint32_t (*is_ally)(void *user, uint32_t id, uint32_t other);                           /* 1084c5a8 */
    /* Fills `out` for the hit `index` of `kind`: halfword move at +0, target at +5, selector at +6; 0 when none. */
    uint32_t (*damage_record)(void *user, uint32_t pokemon, uint32_t kind, uint32_t index, uint32_t out); /* 1084bac4 */
    uint32_t (*opposing_selector)(void *user, uint32_t context, uint32_t id);               /* 1084cb78 */
    uint32_t (*target_for)(void *user, uint32_t context, uint32_t selector);                /* 1084b394; 0x1f none */
    uint32_t (*choose_target)(void *user, uint32_t context, uint32_t id, uint32_t mode);    /* 10871bac; 0xa none */
    uint32_t (*move_slot)(void *user, uint32_t pokemon, uint32_t move);                     /* 1084c224; 4 none */
    uint32_t (*move_pp)(void *user, uint32_t pokemon, uint32_t slot);                       /* 108520e4 */
    uint32_t (*disable_blocked)(void *user, uint32_t move);                                 /* 1084c808 */
    uint32_t (*item_param)(void *user, uint32_t item, uint32_t param);                      /* 1084cf74 */
    uint32_t (*counter)(void *user, uint32_t pokemon, uint32_t index);                      /* 1084bb2c */
    uint32_t (*battle_mode)(void *user, uint32_t context);                                  /* 1084bf14 */
    uint32_t (*random)(void *user, uint32_t range);                                         /* 1084c7bc */
    uint32_t (*slot_used)(void *user, uint32_t pokemon, uint32_t index);                    /* 108c86d4 */
    uint32_t (*party)(void *user, uint32_t context, uint32_t id);                           /* 1084b950 */
    uint32_t (*party_member)(void *user, uint32_t party, uint32_t index);                   /* 1085daf8 */
    uint32_t (*can_battle)(void *user, uint32_t pokemon);                                   /* 108551d4 */
    uint32_t (*position)(void *user, uint32_t context, uint32_t id);                        /* 1084b590; 0xa none */
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);                  /* 10856a88 */
    uint32_t (*has_type)(void *user, uint32_t pokemon, uint32_t type);                      /* 1086f960 */
    uint32_t (*held_item)(void *user, uint32_t context, uint32_t id);                       /* 1084ae18 */
    uint32_t (*switch_pending)(void *user, uint32_t context);                               /* 1084c020 */
    uint32_t (*redirect_blocked)(void *user, uint32_t context, uint32_t attacker, uint32_t id, uint32_t move); /* 1084b67c */
    /* 16 bytes of logical scratch memory (a stack buffer). */
    uint32_t scratch;
};

#define PX_BATTLE_MOVES_16_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_16_api *api)
/* Event 0x2a: the target of a reflecting move. */
PX_BATTLE_MOVES_16_HANDLER(px_battle_counter_target);        /* 1084b458, Counter (68): physical (r3 = 1) */
PX_BATTLE_MOVES_16_HANDLER(px_battle_mirror_coat_target);    /* 108b8404, Mirror Coat (243): special (r3 = 2) */
PX_BATTLE_MOVES_16_HANDLER(px_battle_metal_burst_target);    /* 108b8f78, Metal Burst (368): any (r3 = 0) */
PX_BATTLE_MOVES_16_HANDLER(px_battle_bide_target);           /* 108b5848, Bide (117) */
/* Event 0x2b. */
PX_BATTLE_MOVES_16_HANDLER(px_battle_sky_drop_target);       /* 108b7a20, Sky Drop (507) */
PX_BATTLE_MOVES_16_HANDLER(px_battle_follow_me_target);      /* 108bb270, moves 266 and 476 */
/* Events 0x35, 0x36, 0x39, 0x3a, 0x42, 0x48. */
PX_BATTLE_MOVES_16_HANDLER(px_battle_present_flag);          /* 108b6ff8, move 217, event 0x35 */
PX_BATTLE_MOVES_16_HANDLER(px_battle_present_effect);        /* 108b51d0, move 217, event 0x36 */
PX_BATTLE_MOVES_16_HANDLER(px_battle_weather_accuracy);      /* 108b8d3c, moves 87 and 542, event 0x39 */
PX_BATTLE_MOVES_16_HANDLER(px_battle_beat_up);               /* 108b56f0, move 251, event 0x3a */
PX_BATTLE_MOVES_16_HANDLER(px_battle_fixed_flag);            /* 108ba7f8, move 167, event 0x3a */
PX_BATTLE_MOVES_16_HANDLER(px_battle_support_ready);         /* 108bbdf4, moves 215 and 312, event 0x42 */
PX_BATTLE_MOVES_16_HANDLER(px_battle_pledge_ready);          /* 108ba004, moves 518-520, event 0x48 */
/* Event 0x53. */
PX_BATTLE_MOVES_16_HANDLER(px_battle_move_message_499);      /* 108b1404, move 499 */
PX_BATTLE_MOVES_16_HANDLER(px_battle_rage_boost);            /* 108b4db8, move 99 */
PX_BATTLE_MOVES_16_HANDLER(px_battle_bide_damage);           /* 108b57f8, move 117 */
PX_BATTLE_MOVES_16_HANDLER(px_battle_chip_allies);           /* 108b69f4, move 481 */
PX_BATTLE_MOVES_16_HANDLER(px_battle_grudge_drain);          /* 108b847c, move 288 */
PX_BATTLE_MOVES_16_HANDLER(px_battle_fling_effect);          /* 108ba3cc, move 374 */

#endif
