#ifndef PX_BATTLE_ABILITIES_7_H
#define PX_BATTLE_ABILITIES_7_H

#include "../battle_abilities_6/battle_abilities_6.h"

/* DllBattle trapping, escape, priority and endurance abilities (static
 * analysis addresses; runtime addresses are module-relative). Analyst
 * names. Keys: 0x02 the battler concerned, 0x03 attacker, 0x04 target,
 * 0x05 target count with the targets from key 0x06, 0x12 move, 0x16 type,
 * 0x19 priority, 0x21 PP used, 0x3b survival reason, 0x42 the action is
 * prevented, 0x53 flag. Full HP (1084d318) comes from battle_abilities_6. */
struct px_battle_abilities_7_api {
    /* Event stack, battler lookup, pokemon_value (10874420; 0x11 effective
     * ability), move_category (108007dc) and side_battlers (1084bc20). */
    const struct px_battle_ability_api *base;
    uint32_t (*is_ally)(void *user, uint32_t id, uint32_t other);                     /* 1084c5a8 */
    uint32_t (*opposing_selector)(void *user, uint32_t context, uint32_t id);         /* 1084cb78 */
    uint32_t (*has_type)(void *user, uint32_t pokemon, uint32_t type);                /* 1086f960; 8 Steel */
    uint32_t (*airborne)(void *user, uint32_t context, uint32_t id);                  /* 1084bf24 */
    uint32_t (*move_kind)(void *user, uint32_t move);                                 /* 108007d4 */
    uint32_t (*pressure_move)(void *user, uint32_t move);                             /* 108a48d4 */
    void (*ability_popup)(void *user, uint32_t context, uint32_t phase, uint32_t id); /* 1084c948; 1 show, 2 hide */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);        /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                /* 1084d29c */
    void (*message_extra)(void *user, uint32_t message, uint32_t value);              /* 10876c28 */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);            /* 10867b48 */
    /* Logical stack area standing in for the 12-byte battler list of the trapping handlers. */
    uint32_t list_scratch;
};

#define PX_BATTLE_ABILITIES_7_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_abilities_7_api *api)
/* Switching (event 0x0c) and escape (0x0b, 0x0d). */
PX_BATTLE_ABILITIES_7_HANDLER(px_battle_shadow_tag);       /* 108be518, ability 23 */
PX_BATTLE_ABILITIES_7_HANDLER(px_battle_arena_trap);       /* 108bef74, ability 71 */
PX_BATTLE_ABILITIES_7_HANDLER(px_battle_magnet_pull);      /* 108bd98c, ability 42 */
PX_BATTLE_ABILITIES_7_HANDLER(px_battle_run_away);         /* 108bdd94, ability 50, event 0x0b */
PX_BATTLE_ABILITIES_7_HANDLER(px_battle_run_away_message); /* 108c0828, ability 50, event 0x0d */
/* Forced switches (0x95) and Ground moves (0x12, 0x1b). */
PX_BATTLE_ABILITIES_7_HANDLER(px_battle_suction_cups);     /* 108bda2c, ability 21 */
PX_BATTLE_ABILITIES_7_HANDLER(px_battle_levitate_check);   /* 108bc89c, ability 26, event 0x12 */
PX_BATTLE_ABILITIES_7_HANDLER(px_battle_levitate_message); /* 108bfd64, ability 26, event 0x1b */
/* PP (0x56) and priority (0x11). */
PX_BATTLE_ABILITIES_7_HANDLER(px_battle_pressure_pp);      /* 108be848, ability 46 */
PX_BATTLE_ABILITIES_7_HANDLER(px_battle_prankster);        /* 108c1728, ability 158 */
PX_BATTLE_ABILITIES_7_HANDLER(px_battle_gale_wings);       /* 108c297c, ability 177 */
/* Sturdy (5): one-hit KO moves (0x79), surviving at full HP (0x7d), its message (0x7e). */
PX_BATTLE_ABILITIES_7_HANDLER(px_battle_sturdy_ohko);      /* 108c2850 */
PX_BATTLE_ABILITIES_7_HANDLER(px_battle_sturdy_endure);    /* 108c3a18 */
PX_BATTLE_ABILITIES_7_HANDLER(px_battle_sturdy_message);   /* 108c28e4 */

#endif
