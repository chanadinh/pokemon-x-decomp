#ifndef PX_BATTLE_EFFECTS_H
#define PX_BATTLE_EFFECTS_H

#include "../battle_process/battle_process.h"

/* Six more DllBattle routines that the action processor hands record kinds
 * to (static analysis addresses, runtime addresses module-relative). Analyst
 * names. Each returns the record's outcome (nonzero success); callees other
 * than the event stack go through the processor API's call callback.
 *
 * Kind 5: +4 u16 value, +6 target, +7 source (0x1f none), message at +8
 *   (shown when it has a kind).
 * Kind 0xe: +4 battler, +5..+0xb the seven stat stages, +0xc byte (stored at
 *   +0x155 when at most 3).
 * Kind 0x11: +4 battler.
 * Kind 0x1a: +4 length, bit bytes from +5, +8 side.
 * Kind 0x1c: +4 side-table index (0xa special), +5 index for 0xa, +8 word,
 *   message at +0xc, +0x30 fallback flag.
 * Kind 0x2b: +4 and +5 battlers. */

uint32_t px_battle_kind_value(uint32_t context, uint32_t record, const struct px_battle_process_api *api);        /* 1081d558, kind 5 */
uint32_t px_battle_kind_set_stages(uint32_t context, uint32_t record, const struct px_battle_process_api *api);   /* 1081f648, kind 0xe */
uint32_t px_battle_kind_clear_drops(uint32_t context, uint32_t record, const struct px_battle_process_api *api);  /* 10823704, kind 0x11 */
uint32_t px_battle_kind_clear_field(uint32_t context, uint32_t record, const struct px_battle_process_api *api);  /* 108266b8, kind 0x1a */
uint32_t px_battle_kind_side_entry(uint32_t context, uint32_t record, const struct px_battle_process_api *api);   /* 10824fb0, kind 0x1c */
uint32_t px_battle_kind_transfer(uint32_t context, uint32_t record, const struct px_battle_process_api *api);     /* 10822cbc, kind 0x2b */

/* Kind 0x1a's message per field flag (inner jump table 10826724, 15 ABS32
 * relocations); -1 for none. */
int32_t px_battle_field_clear_message(uint32_t flag);

#endif
