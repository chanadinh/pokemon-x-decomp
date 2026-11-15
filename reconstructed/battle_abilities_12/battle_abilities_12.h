#ifndef PX_BATTLE_ABILITIES_12_H
#define PX_BATTLE_ABILITIES_12_H

#include "../battle_abilities_1/battle_abilities_1.h"

/* DllBattle transformation, status and move-reaction abilities (static
 * analysis addresses; runtime addresses are module-relative). Analyst
 * names. Keys: 0x02 the battler concerned, 0x03 attacker, 0x04 target,
 * 0x10 the new ability, 0x12 move, 0x16 type, 0x1e status given, 0x1f its
 * condition word, 0x23 failure reason, 0x36 multiplier, 0x47 suppressed.
 * Battlers: species +0xc, form byte +0x153. Fainted (1085f844) comes from
 * battle_abilities_1. */
struct px_battle_abilities_12_api {
    /* Event stack, battler lookup, pokemon_value (10874420) and side_of (10873158, the position). */
    const struct px_battle_ability_api *base;
    uint32_t (*battle_mode)(void *user, uint32_t context);                            /* 1084bf14 */
    uint32_t (*facing_position)(void *user, uint32_t mode, uint32_t position);        /* 1086dddc */
    uint32_t (*battler_at)(void *user, uint32_t context, uint32_t position);          /* 1084d348 */
    uint32_t (*badly_poisoned)(void *user, uint32_t condition);                       /* 1087a1c0 */
    void (*condition_for)(void *user, uint32_t status, uint32_t pokemon, uint32_t out); /* 1084c438; word at out */
    uint32_t (*illusion_active)(void *user, uint32_t pokemon);                        /* 1084aa9c */
    uint32_t (*damaging_move)(void *user, uint32_t move);                             /* 10800434 */
    void (*ability_popup)(void *user, uint32_t context, uint32_t phase, uint32_t id); /* 1084c948; 1 show, 2 hide */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);        /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);            /* 10867b48 */
};

#define PX_BATTLE_ABILITIES_12_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_abilities_12_api *api)
PX_BATTLE_ABILITIES_12_HANDLER(px_battle_imposter);          /* 108bcf0c, ability 150, event 0x5e */
PX_BATTLE_ABILITIES_12_HANDLER(px_battle_truant_loaf);       /* 108bd1b4, ability 54, event 0x1e */
PX_BATTLE_ABILITIES_12_HANDLER(px_battle_truant_message);    /* 108c190c, ability 54, event 0x21 */
PX_BATTLE_ABILITIES_12_HANDLER(px_battle_synchronize);       /* 108bd33c, ability 28, event 0x71 */
PX_BATTLE_ABILITIES_12_HANDLER(px_battle_aura_break_reverse); /* 108bf054, ability 188, event 0x3c */
PX_BATTLE_ABILITIES_12_HANDLER(px_battle_protean);           /* 108c0578, ability 168, event 0x23 */
PX_BATTLE_ABILITIES_12_HANDLER(px_battle_stance_change);     /* 108c0e14, ability 176, event 0x03 */
PX_BATTLE_ABILITIES_12_HANDLER(px_battle_illusion_leave);    /* 108c16a0, ability 149, event 0x73 */
PX_BATTLE_ABILITIES_12_HANDLER(px_battle_illusion_hit);      /* 108c29c0, ability 149, event 0x53 */
PX_BATTLE_ABILITIES_12_HANDLER(px_battle_illusion_lost);     /* 108c419c, ability 149, event 0x93 */
PX_BATTLE_ABILITIES_12_HANDLER(px_battle_steadfast);         /* 108c27ac, ability 80, event 0x21 */
PX_BATTLE_ABILITIES_12_HANDLER(px_battle_mega_launcher);     /* 108c4f60, ability 178, event 0x99 */

#endif
