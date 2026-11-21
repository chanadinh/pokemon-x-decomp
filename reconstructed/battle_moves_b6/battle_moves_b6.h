#ifndef PX_BATTLE_MOVES_B6_H
#define PX_BATTLE_MOVES_B6_H

#include "../battle_ability/battle_ability.h"

/* DllBattle move handlers of event 0xac, the effect of a status move, fifth
 * group: 8 of the 10 handlers of the upper half of the last 21 unreconstructed
 * move handlers (static analysis addresses; runtime addresses are
 * module-relative). Analyst names. Every handler but the Spiky Shield stub
 * acts for the user (key 0x03); key 0x06 is the target. Records
 * (10869f7c/10867b48): kind 3 message at +4; kind 0xa cure (+4 status, +5
 * battler, +0x11 1); kind 0xb condition (+4 halfword, +8 word, +0xc, +0xf
 * battler); kind 0xd stat stage (+4 stat, +0xc battler, +0x16 count, +0x17
 * stages, +0x18 flags); kind 0xe stages (+4 battler, +5..+0xb seven stages, +0xc);
 * kind 0x17 (+4, +5 battler); kind 0x1a side bits (+4 count, +5..+7 bit
 * sets, +8 side); kind 0x1f (+4, +5 position, +8, +0x18); kind 0x27 counter
 * (+4 battler, +5 counter, +6 value); kind 0x2a (+0x4c battler, +0x4d); kind
 * 0x33 (+4 battler, +5 partner, message at +8). The first word of a record
 * holds flag 0x1000000. */
struct px_battle_moves_b6_api {
    /* Event stack, battler lookup, pokemon_value (10874420), pokemon_status (1086d8ac) and side_of (10873158). */
    const struct px_battle_ability_api *base;
    uint32_t (*side_mask)(void *user, uint32_t battler);                                 /* 1084bfc0 */
    uint32_t (*counter)(void *user, uint32_t pokemon, uint32_t index);                   /* 1084bb2c */
    uint32_t (*can_switch)(void *user, uint32_t context, uint32_t id);                   /* 1084ca78 */
    uint32_t (*can_pass)(void *user, uint32_t context);                                  /* 1084c000 */
    uint32_t (*battle_mode)(void *user, uint32_t context);                               /* 1084bf14 */
    uint32_t (*owner_of)(void *user, uint32_t id);                                       /* 1087425c */
    uint32_t (*owner_state)(void *user, uint32_t context, uint32_t owner);               /* 1081e28c */
    uint32_t (*side_slot)(void *user, uint32_t side, uint32_t index);                    /* 1084b664 */
    uint32_t (*battler_at)(void *user, uint32_t context, uint32_t slot);                 /* 1084d348 */
    uint32_t (*side_active)(void *user, uint32_t side);                                  /* 10814f9c */
    uint32_t (*status_data)(void *user, uint32_t pokemon, uint32_t status);              /* 108c84e0 */
    uint32_t (*poison_flag)(void *user, uint32_t data);                                  /* 1087a1c0 */
    uint32_t (*status_word)(void *user, uint32_t status);                                /* 10860e00 */
    uint32_t (*stat_can_rise)(void *user, uint32_t pokemon, uint32_t stat, uint32_t stages); /* 108722e8 */
    uint32_t (*random)(void *user, uint32_t count);                                      /* 1084c7bc */
    void (*protect_start)(void *user, uint32_t context, uint32_t id);                    /* 1084bd48 */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);           /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                   /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);               /* 10867b48 */
};

#define PX_BATTLE_MOVES_B6_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_b6_api *api)
PX_BATTLE_MOVES_B6_HANDLER(px_battle_defog_effect);          /* 108b23bc, Defog (432) */
PX_BATTLE_MOVES_B6_HANDLER(px_battle_power_swap_effect);     /* 108b2b2c, Power Swap (384) */
PX_BATTLE_MOVES_B6_HANDLER(px_battle_stockpile_effect);      /* 108b2eb8, Stockpile (254) */
PX_BATTLE_MOVES_B6_HANDLER(px_battle_baton_pass_effect);     /* 108b3458, Baton Pass (226) */
PX_BATTLE_MOVES_B6_HANDLER(px_battle_psycho_shift_effect);   /* 108b40fc, Psycho Shift (375) */
PX_BATTLE_MOVES_B6_HANDLER(px_battle_ally_switch_effect);    /* 108b42b4, Ally Switch (502) */
PX_BATTLE_MOVES_B6_HANDLER(px_battle_acupressure_effect);    /* 108b481c, Acupressure (367) */
PX_BATTLE_MOVES_B6_HANDLER(px_battle_spiky_shield_effect);   /* 108b50c8, Spiky Shield (596) */

#endif
