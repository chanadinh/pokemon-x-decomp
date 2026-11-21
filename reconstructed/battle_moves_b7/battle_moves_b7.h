#ifndef PX_BATTLE_MOVES_B7_H
#define PX_BATTLE_MOVES_B7_H

#include "../battle_ability/battle_ability.h"

/* DllBattle move handlers of event 0xac, the effect of a status move, last
 * group: Heal Bell (move 215, 108b87e0) and Aromatherapy (move 312, 108b5620),
 * which share the body at 108b87e4 (static analysis addresses; runtime
 * addresses are module-relative). Analyst names. Heal Bell's entry is a nop
 * word falling into the body, Aromatherapy's a branch to it.
 *
 * The body walks one or two battler lists (the user's side, 1084b950, and a
 * second one, 1082001c, when it exists): a list holds its size in a byte at
 * +0x18, 1085daf8(list, index) gives a member whose battler id is the byte at
 * +0x19. A member is cured when its state (1084cb78) is 0xa or when it is one
 * of the move's targets (keys 0x06.., key 0x05 is their count), and it
 * exists and has a status (1086d8ac). The cured battlers are collected in
 * the handler's work area (one byte each) and committed in a kind-0xa record
 * (+4 0x26, +5.. the battler ids, +0x11 their count); with none, key 0x53
 * is rewritten to 1. */
struct px_battle_moves_b7_api {
    /* Event stack, battler lookup and pokemon_status (1086d8ac). */
    const struct px_battle_ability_api *base;
    uint32_t (*side_list)(void *user, uint32_t context);                                 /* 1084b950 */
    uint32_t (*partner_list)(void *user, uint32_t context, uint32_t id);                 /* 1082001c; 0 none */
    uint32_t (*list_member)(void *user, uint32_t list, uint32_t index);                  /* 1085daf8 */
    uint32_t (*member_state)(void *user, uint32_t context, uint32_t battler);            /* 1084cb78 */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);               /* 10867b48 */
};

#define PX_BATTLE_MOVES_B7_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_b7_api *api)
PX_BATTLE_MOVES_B7_HANDLER(px_battle_heal_bell_effect);       /* 108b87e0 (falls into 108b87e4), Heal Bell (215) */
PX_BATTLE_MOVES_B7_HANDLER(px_battle_aromatherapy_effect);    /* 108b5620 (branches to 108b87e4), Aromatherapy (312) */

#endif
