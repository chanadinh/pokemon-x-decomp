#ifndef PX_BATTLE_CURES_H
#define PX_BATTLE_CURES_H

#include "../battle_process/battle_process.h"

/* The last five DllBattle routines that the action processor hands record
 * kinds to (static analysis addresses, runtime addresses module-relative).
 * Analyst names. Each returns the record's outcome (nonzero success);
 * callees other than the event stack go through the processor API's call
 * callback. Battlers: held item u16 +0x12, conditions as words at +0x1c +
 * 4 index (index below 0x26, active when the low 3 bits are set).
 *
 * Kind 0xa: +4 cure (0x26 the current status, 0x27 it and then condition
 *   6, 0x28 every listed condition, else that condition), +5.. targets,
 *   +0x11 count, +0x12 use the message at +0x14.
 * Kind 0x21: +4 u16 new item, +6 battler, +7 clear its +0x14, +8 clear
 *   +0x14 of battler +9, +0xa event 0x9b when the item is removed, message
 *   at +0xc.
 * Kind 0x25: +4 battler swapping items with the header battler, messages at
 *   +8, +0x2c and +0x50.
 * Kind 0x28: +4 user, +5 target, +6 field byte, +8 u16 move.
 * Kind 0x36: the header battler's side is cured. */
struct px_battle_cures_api {
    const struct px_battle_process_api *base;
    /* Logical address of kind 0xa's six u16 conditions (the original list at
     * 108d146a, literal 10820c98). */
    uint32_t cures;
    /* Logical scratch standing in for frame locals: +0 kind 0xa's out word,
     * +0x10 kind 0x36's two 8-byte id buffers, +0x20 kind 0x28's 20-byte move
     * frame. Not shared with a nested call of the same kind. */
    uint32_t frames;
};

uint32_t px_battle_kind_cure(uint32_t context, uint32_t record, uint32_t low, const struct px_battle_cures_api *api); /* 10820904, kind 0xa */
uint32_t px_battle_kind_set_item(uint32_t context, uint32_t record, const struct px_battle_cures_api *api);            /* 1081f36c, kind 0x21 */
uint32_t px_battle_kind_swap_items(uint32_t context, uint32_t record, const struct px_battle_cures_api *api);          /* 10820c9c, kind 0x25 */
uint32_t px_battle_kind_use_move(uint32_t context, uint32_t record, const struct px_battle_cures_api *api);            /* 10825eb8, kind 0x28 */
uint32_t px_battle_kind_cure_side(uint32_t context, uint32_t record, const struct px_battle_cures_api *api);           /* 10824414, kind 0x36 */

#define PX_BATTLE_CURE_LIST_STATIC UINT32_C(0x108d146a)

#endif
