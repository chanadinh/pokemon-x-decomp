#ifndef PX_BATTLE_MOVES_17_H
#define PX_BATTLE_MOVES_17_H

#include "../battle_ability/battle_ability.h"
#include "../battle_handler/battle_handler.h"

/* DllBattle move handlers of events 0x54-0x6d (static analysis addresses;
 * runtime addresses are module-relative). Analyst names. Keys: 0x03
 * attacker, 0x04 defender, 0x06 target (ids), 0x12 and 0x14 the move (a
 * halfword), 0x1e (the effect kind) and 0x1f (the effect value), 0x27, 0x40
 * (a message area), 0x47, 0x53 (values).
 * Records (10869f7c/10867b48): kind 3 message at +4; kind 0xa cure (+4
 * condition, +5 battler, +0x11); kind 0x12 faint (+4, +5, halfword +6,
 * message at +8); kind 0x1a side conditions (+8 side, +4 mask size then mask
 * bytes +5..+7); kind 0x21 (+4 halfword, +6 battler, message at +0xc); kind
 * 0x2e (+4 battler, halfword +6, message at +8). Scratch memory: one word the
 * value service updates in place. */
struct px_battle_moves_17_api {
    /* Event stack, battler lookup, pokemon_value (10874420) and side_of (10873158). */
    const struct px_battle_ability_api *base;
    /* Registered handlers (Destiny Bond removes itself). */
    const struct px_battle_handler_api *handlers;
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id);    /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);                  /* 10867b48 */
    void (*message)(void *user, uint32_t at, uint32_t kind, uint32_t message);              /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t at, uint32_t value);                           /* 1084d29c */
    uint32_t (*item_burnable)(void *user, uint32_t item);                                   /* 108007a4 */
    uint32_t (*item_flammable)(void *user, uint32_t item);                                  /* 10801cfc */
    uint32_t (*no_hp)(void *user, uint32_t pokemon);                                        /* 1085f844 */
    uint32_t (*disable_blocked)(void *user, uint32_t move);                                 /* 1084c808 */
    uint32_t (*is_ally)(void *user, uint32_t id, uint32_t other);                           /* 1084c5a8 */
    void (*note_handler)(void *user, uint32_t handler);                                     /* 1084c700 */
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);                  /* 10856a88 */
    uint32_t (*side_id)(void *user, uint32_t id);                                           /* 1084bfc0 */
    uint32_t (*can_pursue)(void *user, uint32_t context, uint32_t id, uint32_t out);        /* 1084b784 */
    uint32_t (*redirect_blocked)(void *user, uint32_t context, uint32_t attacker, uint32_t id, uint32_t move); /* 1084b67c */
    void (*pursue)(void *user, uint32_t context, uint32_t id);                              /* 1081d748 */
    uint32_t (*streak)(void *user, uint32_t pokemon);                                       /* 108c8754 */
    uint32_t (*scale_value)(void *user, uint32_t word, uint32_t parameter);                 /* 1084b990 (updates the word) */
    uint32_t (*opposing_selector)(void *user, uint32_t context, uint32_t id);               /* 1084cb78 */
    uint32_t (*selector_value)(void *user, uint32_t selector);                              /* 1084c3b0 */
    uint32_t (*random)(void *user, uint32_t range);                                         /* 1084c7bc */
    uint32_t (*type_value)(void *user, uint32_t type);                                      /* 10860e00 */
    /* The three Tri Attack values (literal 108b3160, relocated; static 108d2c34). */
    uint32_t tri_attack_table;
    /* One logical word of scratch memory. */
    uint32_t scratch;
};

#define PX_BATTLE_MOVES_17_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_17_api *api)
PX_BATTLE_MOVES_17_HANDLER(px_battle_incinerate_item);      /* 108b4a24, move 510, event 0x54 */
PX_BATTLE_MOVES_17_HANDLER(px_battle_destiny_bond_faint);   /* 108babcc, move 194, event 0x54 */
PX_BATTLE_MOVES_17_HANDLER(px_battle_rapid_spin_clear);     /* 108b4ef8, move 229, event 0x55 */
PX_BATTLE_MOVES_17_HANDLER(px_battle_struggle_ready);       /* 108b9de8, move 165, event 0x58 */
PX_BATTLE_MOVES_17_HANDLER(px_battle_pursuit_strike);       /* 108b3e78, move 228, event 0x5b */
PX_BATTLE_MOVES_17_HANDLER(px_battle_streak_heal);          /* 108b1368, move 475, event 0x67 */
PX_BATTLE_MOVES_17_HANDLER(px_battle_effect_message);       /* 108b5574, move 477, event 0x69 */
PX_BATTLE_MOVES_17_HANDLER(px_battle_effect_text);          /* 108b5fac, 8 moves, event 0x6a */
PX_BATTLE_MOVES_17_HANDLER(px_battle_scale_193);            /* 108b09d0, moves 193 and 316, event 0x6b */
PX_BATTLE_MOVES_17_HANDLER(px_battle_scale_357);            /* 108b3ad8, move 357, event 0x6b */
PX_BATTLE_MOVES_17_HANDLER(px_battle_selector_value);       /* 108bb084, move 73, event 0x6b */
PX_BATTLE_MOVES_17_HANDLER(px_battle_scale_by_move);        /* 108b0980, 8 moves, event 0x6d */
PX_BATTLE_MOVES_17_HANDLER(px_battle_tri_attack_effect);    /* 108b30fc, move 161, event 0x6d */

#endif
