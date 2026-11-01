#ifndef PX_BATTLE_RESTORES_H
#define PX_BATTLE_RESTORES_H

#include "../battle_process/battle_process.h"

/* Four more DllBattle routines that the action processor hands record kinds
 * to (static analysis addresses, runtime addresses module-relative). Analyst
 * names. Each returns the record's outcome (nonzero success); callees other
 * than the event stack go through the processor API's call callback. The
 * source is the header battler.
 *
 * Kind 4: +4 u16 HP, +6 target, +7 force (skips the heal-block check),
 *   message at +8 (its kind and count u16 at +0xa); low = the stack word's
 *   low 10 bits, shown in message 0x392 when the record has none.
 * Kind 8: +4 PP, +5 target, +6 move slot (below 4), +7 flags (bit 0 the
 *   second PP pair, bit 1 even when fainted), message at +8. Move slots are
 *   14 bytes from +0x116: u16 move, PP and maximum at +2/+3 and +8/+9, and
 *   +0xc linking the two pairs.
 * Kind 0x23: +4 battler, +5 mark the item, +6 u16 item.
 * Kind 0x34: +4 target to transform into, message at +8. */

uint32_t px_battle_kind_restore(uint32_t context, uint32_t record, uint32_t low, const struct px_battle_process_api *api); /* 10821948, kind 4 */
uint32_t px_battle_kind_pp(uint32_t context, uint32_t record, uint32_t low, const struct px_battle_process_api *api);      /* 10821b80, kind 8 */
uint32_t px_battle_kind_item(uint32_t context, uint32_t record, const struct px_battle_process_api *api);                  /* 1081db1c, kind 0x23 */
uint32_t px_battle_kind_transform(uint32_t context, uint32_t record, const struct px_battle_process_api *api);             /* 1081defc, kind 0x34 */

#endif
