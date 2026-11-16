#ifndef PX_BATTLE_MOVES_B1_H
#define PX_BATTLE_MOVES_B1_H

#include "../battle_abilities_1/battle_abilities_1.h"

/* DllBattle move handlers of the highest-numbered move events, 0xae to 0xb4
 * (static analysis addresses; runtime addresses are module-relative).
 * Analyst names. Events: 0xb4 Final Gambit's own faint, 0xb3 the forced
 * switch message of Whirlwind and Roar, 0xb1 a contact move into Spiky
 * Shield or King's Shield, 0xae a Pledge combining with its partner's.
 * Keys: 0x03 attacker, 0x04 defender (ids), 0x12 move, 0x16 type, 0x53 and
 * 0x59 (values). The handler's id halfword +0x38 is its move. Records
 * (10869f7c/10867b48): kind 6 damage (+4 HP, +6 battler, +7 flags, message
 * at +0xc), kind 0xd stat stages (+4 stat, +0xc battler, +0x16 count, +0x17
 * stages, +0x18 flags), kind 0x12 (+4 battler). The max-HP share (1084d2f4)
 * comes from battle_abilities_1. */
struct px_battle_moves_b1_api {
    /* Event stack, battler lookup and pokemon_value (10874420). */
    const struct px_battle_ability_api *base;
    uint32_t (*move_flag)(void *user, uint32_t move, uint32_t flag);                  /* 1080049c; 0 contact */
    uint32_t (*pledge_partner)(void *user, uint32_t pokemon, uint32_t partner_out, uint32_t move_out); /* 108c86a0 */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);        /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);            /* 10867b48 */
    /* Logical stack area standing in for the Pledge handler's frame: the
     * partner's move halfword at +0, its battler byte at +4. */
    uint32_t pledge_scratch;
};

#define PX_BATTLE_MOVES_B1_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_b1_api *api)
PX_BATTLE_MOVES_B1_HANDLER(px_battle_final_gambit_faint); /* 108ba248, event 0xb4, Final Gambit (515) */
PX_BATTLE_MOVES_B1_HANDLER(px_battle_roar_message);       /* 108adce8, event 0xb3, Whirlwind (18), Roar (46) */
PX_BATTLE_MOVES_B1_HANDLER(px_battle_spiky_shield_hit);   /* 108ba570, event 0xb1, Spiky Shield (596) */
PX_BATTLE_MOVES_B1_HANDLER(px_battle_kings_shield_hit);   /* 108baa30, event 0xb1, King's Shield (588) */
PX_BATTLE_MOVES_B1_HANDLER(px_battle_pledge_combine);     /* 108b93a4, event 0xae, Water, Fire and Grass Pledge (518-520) */

#endif
