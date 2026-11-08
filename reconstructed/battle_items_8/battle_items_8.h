#ifndef PX_BATTLE_ITEMS_8_H
#define PX_BATTLE_ITEMS_8_H

#include "../battle_items_1/battle_items_1.h"
#include "../battle_abilities_1/battle_abilities_1.h"

/* DllBattle item handlers, eighth batch: items set off by a hit (events 0x53
 * and 0x54, key 0x04 the battler hit, key 0x03 the attacker, key 0x47
 * suppressing berries and after-effects) and their uses on event 0x7b
 * (static analysis addresses; runtime addresses are module-relative).
 * Analyst names. Item ids are the original ids.
 *
 * A check that passes either asks for the item (re-entry mark and the
 * kind-0 record of 1084c948) or, for Weakness Policy and the Jaboca and
 * Rowap Berries, records the attacker + 1 in work word 0 and commits a kind-0
 * record with +4 bit 1 itself. Stat records are kind 0xd (+4 stat, +0xc
 * battler, +0x16 count 1, +0x17 stages). Key 0x39 is the effectiveness code
 * (above 7 super effective), key 0x16 the move type, key 0x1b its category
 * (1 physical, 2 special). */
struct px_battle_items_8_api {
    /* Event stack, battler lookup, has_status, item_param, 1084c948,
     * records, messages and commit. */
    const struct px_battle_items_api *items;
    /* For 1084d2f4 (max HP / divisor) and 1085f844 (fainted); the same base. */
    const struct px_battle_abilities_1_api *abilities;
    uint32_t (*move_flag)(void *user, uint32_t move, uint32_t flag);                   /* 1080049c; 0 contact */
    uint32_t (*stage_can_change)(void *user, uint32_t pokemon, uint32_t stat, int32_t delta); /* 108722e8 */
    uint32_t (*field_active)(void *user, uint32_t context, uint32_t condition);        /* 1084c374; 2 gravity */
    uint32_t (*move_excluded)(void *user, uint32_t move);                             /* 1084c808 */
    void (*note_berry)(void *user, uint32_t context, uint32_t id);                    /* 10821720: id into a 5-bit slot of a context word */
};

#define PX_BATTLE_ITEMS_8_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_items_8_api *api)
PX_BATTLE_ITEMS_8_HANDLER(px_battle_rocky_helmet);          /* 108a6e58, item 540, event 0x53 */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_air_balloon_announce);  /* 108a8f24, item 541, event 0x5e */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_air_balloon_float);     /* 108aa524, event 0x12 */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_air_balloon_pop);       /* 108ab254, event 0x53 */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_absorb_bulb_check);     /* 108aaa9c, item 545, event 0x53: Water, Sp. Atk */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_absorb_bulb_use);       /* 108a7130, event 0x7b */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_cell_battery_check);    /* 108aaf64, item 546, event 0x53: Electric, Attack */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_snowball_check);        /* 108ab194, item 649, event 0x53: Ice, Attack */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_attack_raise_use);      /* 108a7788, items 546 and 649, event 0x7b */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_luminous_moss_check);   /* 108ab61c, item 648, event 0x53: Water, Sp. Def */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_luminous_moss_use);     /* 108a8530, event 0x7b */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_weakness_policy_check); /* 108aafe8, item 639, event 0x53 */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_weakness_policy_use);   /* 108a9fc0, event 0x7b: Attack and Sp. Atk +2 */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_resist_berry_eaten);    /* 108ab960, items 184-200 and 686, event 0x53 */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_enigma_check);          /* 108a6310, item 208, event 0x53 */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_enigma_use);            /* 108a7840, event 0x7b */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_jaboca_check);          /* 108a8f98, item 211, event 0x54: physical */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_rowap_check);           /* 108a99a8, item 212, event 0x54: special */
PX_BATTLE_ITEMS_8_HANDLER(px_battle_jaboca_rowap_use);      /* 108a76a0, items 211 and 212, event 0x7b */

#endif
