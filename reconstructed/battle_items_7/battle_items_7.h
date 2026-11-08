#ifndef PX_BATTLE_ITEMS_7_H
#define PX_BATTLE_ITEMS_7_H

#include "../battle_items_1/battle_items_1.h"
#include "../battle_abilities_1/battle_abilities_1.h"
#include "../battle_modifier/battle_modifier.h"

/* DllBattle item handlers, seventh batch: residual held items (static
 * analysis addresses; runtime addresses are module-relative). Analyst names.
 * Item ids are the original ids.
 *
 * Event 0x80 (end of turn) gives Toxic Orb's and Flame Orb's status and
 * Sticky Barb's damage; event 0x7c (the item's use, as when flung) the
 * orbs' status to battler key 0x03; event 0x7f asks for Leftovers and runs
 * Black Sludge; event 0x7b restores with Leftovers; event 0x8e (after the
 * holder's hit) runs Shell Bell and Life Orb's recoil; event 0x96 scales
 * Big Root's drain; event 0x53 (after a hit) moves Sticky Barb to a
 * contact attacker with no item.
 *
 * Records: kind 0xb status (+4 status halfword, +8 condition word, +0xc
 * cleared, +0xf the battler); kind 4 HP restore (+4 amount, +6 battler,
 * message at +8); kind 6 HP loss (+4 amount, +6 battler, +7 flags, +8
 * halfword, +0xa side, +0xb 0xa, message at +0xc). Statuses: 4 burn, 5
 * poison (condition word 0x79 for a bad poisoning). */
struct px_battle_items_7_api {
    /* Event stack, battler lookup, side_of, item_param, 1084c948, records,
     * messages and commit. */
    const struct px_battle_items_api *items;
    /* For 1084d2f4 (max HP / divisor) and the item factor 1084cbe4; the
     * same base as items. */
    const struct px_battle_abilities_1_api *abilities;
    const struct px_battle_modifier_api *modifier;
    uint32_t (*condition)(void *user, uint32_t status);                 /* 10860e00 */
    uint32_t (*full_hp)(void *user, uint32_t pokemon);                  /* 1084d318 */
    uint32_t (*packed_types)(void *user, uint32_t pokemon);             /* 10870994: three 5-bit types */
    uint32_t (*battle_decided)(void *user, uint32_t context);           /* 10865c48 */
    uint32_t (*move_flag)(void *user, uint32_t move, uint32_t flag);    /* 1080049c; 0 contact */
};

#define PX_BATTLE_ITEMS_7_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_items_7_api *api)
PX_BATTLE_ITEMS_7_HANDLER(px_battle_toxic_orb);          /* 108a74f0, item 272, event 0x80: badly poisoned */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_toxic_orb_use);      /* 108aa8e8, event 0x7c */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_flame_orb);          /* 108a61a8, item 273, event 0x80: burned */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_flame_orb_use);      /* 108a90a0, event 0x7c */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_sticky_barb);        /* 108ab434, item 288, event 0x80: loses parameter-3 share */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_sticky_barb_move);   /* 108abbec, event 0x53: to a contact attacker */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_leftovers_check);    /* 108aad38, item 234, event 0x7f */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_leftovers);          /* 108a8988, event 0x7b: 1/16 */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_black_sludge);       /* 108a6f60, item 281, event 0x7f */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_shell_bell);         /* 108a7f0c, item 253, event 0x8e */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_life_orb_recoil);    /* 108ab2cc, item 270, event 0x8e: 1/10 */
PX_BATTLE_ITEMS_7_HANDLER(px_battle_big_root);           /* 108a7304, item 296, event 0x96 */

#endif
