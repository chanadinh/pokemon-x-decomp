#ifndef PX_BATTLE_OUTCOMES_H
#define PX_BATTLE_OUTCOMES_H

#include "../battle_process/battle_process.h"

/* Five more DllBattle routines that the action processor hands record kinds
 * to (static analysis addresses, runtime addresses module-relative). Analyst
 * names. Each returns the record's outcome (nonzero success); callees other
 * than the event stack go through the processor API's call callback.
 * Battlers: current HP u16 +0x10, maximum HP u16 +0xe.
 *
 * Kind 0x10: +4..+0xc five u16 values for keys 8..0xc, +0xe battler,
 *   +0xf mask, message at +0x10.
 * Kind 0x12: +4 battler, +5 force, +6 u16 passed with the source, message
 *   at +8; the source is the header battler. Uses 8 bytes at the API's
 *   scratch address (the original frame: source word, u16).
 * Kind 0x2a: messages at +4 and +0x28, +0x4c battler, +0x4d flag.
 * Kind 0x2d: +4 battler, +6 u16 HP to restore, message at +8.
 * Kind 0x33: +4 and +5 battlers of one side, message at +8. */

uint32_t px_battle_kind_set_values(uint32_t context, uint32_t record, const struct px_battle_process_api *api); /* 10821d98, kind 0x10 */
uint32_t px_battle_kind_faint(uint32_t context, uint32_t record, const struct px_battle_process_api *api);      /* 1081cf54, kind 0x12 */
uint32_t px_battle_kind_leave(uint32_t context, uint32_t record, const struct px_battle_process_api *api);      /* 10824288, kind 0x2a */
uint32_t px_battle_kind_heal(uint32_t context, uint32_t record, const struct px_battle_process_api *api);       /* 1081e0f8, kind 0x2d */
uint32_t px_battle_kind_swap(uint32_t context, uint32_t record, const struct px_battle_process_api *api);       /* 10820f3c, kind 0x33 */

#endif
