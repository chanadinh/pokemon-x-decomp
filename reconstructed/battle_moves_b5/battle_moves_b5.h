#ifndef PX_BATTLE_MOVES_B5_H
#define PX_BATTLE_MOVES_B5_H

#include "../battle_moves_5/battle_moves_5.h"

/* DllBattle move handlers of event 0xac, the effect of a status move, fourth
 * group: 14 handlers without a Ghidra function in the live REA project, read
 * as bytes and listed by the battle_power decoder (static analysis
 * addresses; runtime addresses are module-relative). Analyst names. Every
 * handler acts for the user (key 0x03); key 0x05 is the target count and
 * key 0x06.. the targets (ids, low byte). Records (10869f7c/10867b48): kind 3
 * message at +4; kind 0xb condition (+4 halfword, +8 word, +0xf battler,
 * message at +0x14); kind 0xd stat stage (+4 stat, +0xc battler, +0x16 count,
 * +0x17 stages, +0x18 flags); kind 0x14 added type (+4 type, +5 battler);
 * kind 0x20 ability (+4 halfword, +6 battler, message at +0xc); kind 0x30
 * (+4 battler, message at +8). 1084c948(context, kind, id) commits a
 * header-only record. Work word 6 (+0x18) marks a move as done. The turn-count
 * word (1084be3c) comes from battle_moves_5. */
struct px_battle_moves_b5_api {
    /* Event stack, battler lookup and pokemon_value (10874420). */
    const struct px_battle_ability_api *base;
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);               /* 10856a88 */
    uint32_t (*has_type)(void *user, uint32_t pokemon, uint32_t type);                   /* 1086f960 */
    uint32_t (*turn_flag)(void *user, uint32_t pokemon, uint32_t flag);                  /* 1084c818 */
    void (*header_record)(void *user, uint32_t context, uint32_t kind, uint32_t id);     /* 1084c948 */
    uint32_t (*owner_of)(void *user, uint32_t id);                                       /* 1087425c */
    uint32_t (*owner_valid)(void *user, uint32_t context, uint32_t owner);               /* 108c8400 */
    uint32_t (*trap_condition)(void *user, uint32_t context);                            /* 1089e638 */
    uint32_t (*ability_blocked)(void *user, uint32_t ability);                           /* 108a4924 */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);           /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                   /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);               /* 10867b48 */
};

#define PX_BATTLE_MOVES_B5_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_b5_api *api)
PX_BATTLE_MOVES_B5_HANDLER(px_battle_gastro_acid_effect);       /* 108ad640, Gastro Acid (380) */
PX_BATTLE_MOVES_B5_HANDLER(px_battle_celebrate_effect);         /* 108adf14, Celebrate (606) */
PX_BATTLE_MOVES_B5_HANDLER(px_battle_powder_effect);            /* 108ae754, Powder (600) */
PX_BATTLE_MOVES_B5_HANDLER(px_battle_splash_effect);            /* 108ae800, Splash (150) */
PX_BATTLE_MOVES_B5_HANDLER(px_battle_aqua_ring_effect);         /* 108b000c, Aqua Ring (392) */
PX_BATTLE_MOVES_B5_HANDLER(px_battle_taunt_effect);             /* 108b00a0, Taunt (269) */
PX_BATTLE_MOVES_B5_HANDLER(px_battle_trap_effect);              /* 108b0848, Spider Web (169), Mean Look (212), Block (335) */
PX_BATTLE_MOVES_B5_HANDLER(px_battle_trick_or_treat_effect);    /* 108b1a6c, Trick-or-Treat (567) */
PX_BATTLE_MOVES_B5_HANDLER(px_battle_simple_beam_effect);       /* 108b43f4, Simple Beam (493) */
PX_BATTLE_MOVES_B5_HANDLER(px_battle_flower_shield_effect);     /* 108b4c1c, Flower Shield (579) */
PX_BATTLE_MOVES_B5_HANDLER(px_battle_entrainment_effect);       /* 108b60a4, Entrainment (494) */
PX_BATTLE_MOVES_B5_HANDLER(px_battle_worry_seed_effect);        /* 108b619c, Worry Seed (388) */
PX_BATTLE_MOVES_B5_HANDLER(px_battle_after_you_effect);         /* 108b6224, After You (495) */
PX_BATTLE_MOVES_B5_HANDLER(px_battle_destiny_bond_effect);      /* 108b7e50, Destiny Bond (194) */

#endif
