#ifndef PX_BATTLE_MOVES_15_H
#define PX_BATTLE_MOVES_15_H

#include "../battle_ability/battle_ability.h"

/* DllBattle move handlers of events 0x15-0x27 (static analysis addresses;
 * runtime addresses are module-relative). Analyst names.
 * Keys: 0x03 attacker (an id); 0x02 battler (an id, except on event 0x1a,
 * where Snatch rewrites it and key 0x04); 0x12 move, 0x23, 0x42 failure,
 * 0x53 (values). Work word 0: Snatch's flag, Fling's state.
 * Records (10869f7c/10867b48): kind 3 message at +4; kind 0xb condition
 * (+4 halfword condition, +8 data, +0xd, +0xf battler); kind 0xd stat change
 * (+4 stat, +0xc battler, +0x16, +0x17 signed stages, +0x18 flag bits);
 * kind 0x12 (+4 battler); kind 0x15 (+4, +5 battler); kind 0x24 item use
 * (+4, +5); kind 0x27 counter reset (+4 battler, +5 counter, +6 value);
 * kind 0x38 (+4 halfword, +6 side, +7, +0xb). Counters (1084bb2c): 0
 * stockpile, 1 and 2 its Defense and Sp. Def stages. */
struct px_battle_moves_15_api {
    /* Event stack, battler lookup and side_of (10873158). */
    const struct px_battle_ability_api *base;
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id);    /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);                  /* 10867b48 */
    void (*message)(void *user, uint32_t at, uint32_t kind, uint32_t message);              /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t at, uint32_t value);                           /* 1084d29c */
    /* The pokemon test battle_moves_14 names reflection_blocked. */
    uint32_t (*state_check)(void *user, uint32_t pokemon);                                  /* 1084b804 */
    uint32_t (*move_flag)(void *user, uint32_t move, uint32_t flag);                        /* 1080049c */
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);                  /* 10856a88 */
    uint32_t (*lock_count)(void *user, uint32_t a, uint32_t b);                             /* 1084beac */
    void (*condition_flag)(void *user, uint32_t at, uint32_t value);                        /* 10876ca8 */
    uint32_t (*counter)(void *user, uint32_t pokemon, uint32_t index);                      /* 1084bb2c */
};

#define PX_BATTLE_MOVES_15_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_15_api *api)
PX_BATTLE_MOVES_15_HANDLER(px_battle_focus_punch_start);   /* 108b22d0, move 264, event 0x15 */
PX_BATTLE_MOVES_15_HANDLER(px_battle_snatch_steal);        /* 108b91bc, move 289, event 0x1a */
PX_BATTLE_MOVES_15_HANDLER(px_battle_support_done);        /* 108b9d90, moves 215, 270, 312, event 0x1c */
PX_BATTLE_MOVES_15_HANDLER(px_battle_snore_awake);         /* 108b8c84, moves 173 and 214, event 0x1d */
PX_BATTLE_MOVES_15_HANDLER(px_battle_struggle_message);    /* 108b9e14, move 165, event 0x22 */
PX_BATTLE_MOVES_15_HANDLER(px_battle_roost_land);          /* 108b3834, move 355, event 0x25 */
PX_BATTLE_MOVES_15_HANDLER(px_battle_perish_song_message); /* 108b7a88, move 195, event 0x25 */
/* Event 0x27. */
PX_BATTLE_MOVES_15_HANDLER(px_battle_stockpile_release);   /* 108b6af0, moves 255 and 256 (Spit Up, Swallow) */
PX_BATTLE_MOVES_15_HANDLER(px_battle_fling_item);          /* 108b7ebc, move 374 */
PX_BATTLE_MOVES_15_HANDLER(px_battle_natural_gift_item);   /* 108b9bdc, move 363 */
PX_BATTLE_MOVES_15_HANDLER(px_battle_explosion_done);      /* 108ba040, moves 120 and 153 */

#endif
