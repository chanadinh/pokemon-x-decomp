#ifndef PX_BATTLE_ITEMS_6_H
#define PX_BATTLE_ITEMS_6_H

#include "../battle_modifier/battle_modifier.h"

/* DllBattle item handlers, sixth batch: accuracy items (event 0x39) and
 * items that lengthen an effect (static analysis addresses; runtime
 * addresses are module-relative). Analyst names. Item ids are the original
 * ids.
 *
 * Event 0x39 multiplies the accuracy multiplier key 0x36 (1084ce84): key
 * 0x03 is the attacker, key 0x04 the defender. Event 0x85 sets a new
 * weather's duration key 0x25 when key 0x3a is the weather (1 sun, 2 rain,
 * 3 hail, 4 sandstorm). Events 0xab and 0x6b edit the condition word key
 * 0x1f through helpers that take its address (a stack word in the original,
 * the logical address `scratch` here) and rewrite the key (108658d4). */
struct px_battle_items_6_api {
    /* Event stack, battler lookup and item parameters; the item factor
     * 1084cbe4. */
    const struct px_battle_modifier_api *modifier;
    uint32_t (*turn_flag)(void *user, uint32_t pokemon, uint32_t flag);   /* 1084c818; 1: has moved this turn */
    void (*set_turns)(void *user, uint32_t word, uint32_t turns);        /* 10876c4c: the word's turn count */
    void (*set_trap_turns)(void *user, uint32_t word, uint32_t turns);   /* 10876cf4 */
    void (*set_trap_flag)(void *user, uint32_t word, uint32_t value);    /* 10876ca8 */
    uint32_t scratch;
};

/* BrightPowder's and Lax Incense's factor for a percentage reduction:
 * (100 - p)% in 4096 units, through single precision as the original. */
uint32_t px_battle_accuracy_drop(uint32_t reduction);

#define PX_BATTLE_ITEMS_6_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_items_6_api *api)
/* Event 0x39. */
PX_BATTLE_ITEMS_6_HANDLER(px_battle_bright_powder);  /* 108a7598, item 213: the defender, (100 - p)% */
PX_BATTLE_ITEMS_6_HANDLER(px_battle_lax_incense);    /* 108a7230, item 255: the same, 16-bit (100 - p) */
PX_BATTLE_ITEMS_6_HANDLER(px_battle_wide_lens);      /* 108a6f28, item 265: the attacker, item factor */
PX_BATTLE_ITEMS_6_HANDLER(px_battle_zoom_lens);      /* 108a6504, item 276: the attacker after the defender moved */
/* Event 0x85: the weather's turns become parameter 3. */
PX_BATTLE_ITEMS_6_HANDLER(px_battle_heat_rock);      /* 108a5d60, item 284: sun */
PX_BATTLE_ITEMS_6_HANDLER(px_battle_damp_rock);      /* 108a6c74, item 285: rain */
PX_BATTLE_ITEMS_6_HANDLER(px_battle_icy_rock);       /* 108a6ce8, item 282: hail */
PX_BATTLE_ITEMS_6_HANDLER(px_battle_smooth_rock);    /* 108a7344, item 283: sandstorm */
/* Events 0xab and 0x6b: the condition word key 0x1f. */
PX_BATTLE_ITEMS_6_HANDLER(px_battle_light_clay);     /* 108a7e18, item 269, event 0xab: screens, parameter 3 turns */
PX_BATTLE_ITEMS_6_HANDLER(px_battle_grip_claw);      /* 108a98f8, item 286, event 0x6b: condition 8, 8 turns */
PX_BATTLE_ITEMS_6_HANDLER(px_battle_binding_band);   /* 108a7af4, item 544, event 0x6b */

#endif
