#ifndef PX_BATTLE_CHANGES_H
#define PX_BATTLE_CHANGES_H

#include "../battle_process/battle_process.h"

/* Four more DllBattle routines that the action processor hands record kinds
 * to (static analysis addresses, runtime addresses module-relative). Analyst
 * names. Each returns the record's outcome (nonzero success); callees other
 * than the event stack go through the processor API's call callback. The
 * source is the header battler (0x1f none); with the popup flag (bit 23)
 * its id byte is read even without a source, as in the original.
 *
 * Kind 6: +4 u16, +6 target, +7 flags (bit 0 needs 10874348 = 0x12, bit 1
 *   runs 1086a6e0 with +8 u16, +0xa, +0xb), message at +0xc.
 * Kind 7: +4 count, +5 no command 0x48 when set, +6 no item event when set,
 *   target ids from +7, signed HP changes (words) from +0x14.
 * Kind 0xb: +4 u16, +8 word, +0xc, +0xd, +0xe, +0xf target, +0x10, message
 *   at +0x14 (its kind and count u16 at +0x16).
 * Kind 0x13: +4 three 5-bit types, +6 target, +7 no message, +8 generic
 *   message. */

uint32_t px_battle_kind_condition(uint32_t context, uint32_t record, const struct px_battle_process_api *api); /* 1081dd1c, kind 6 */
uint32_t px_battle_kind_hp(uint32_t context, uint32_t record, const struct px_battle_process_api *api);        /* 1081f7a0, kind 7 */
uint32_t px_battle_kind_status(uint32_t context, uint32_t record, const struct px_battle_process_api *api);    /* 1081f1a0, kind 0xb */
uint32_t px_battle_kind_types(uint32_t context, uint32_t record, const struct px_battle_process_api *api);     /* 10822e28, kind 0x13 */

#define PX_BATTLE_ARCEUS 0x1ed /* species 493: its types cannot be set */

#endif
