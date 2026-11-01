#ifndef PX_BATTLE_KINDS_H
#define PX_BATTLE_KINDS_H

#include "../battle_process/battle_process.h"

/* DllBattle routines that the action processor hands record kinds to
 * (static analysis addresses, runtime addresses module-relative). Analyst
 * names. Each returns the record's outcome (nonzero success). The kinds here
 * are the ones the reconstructed handlers push: 0xd (stat stages), 0x1e
 * (weather) and 0x20 (ability change). Callees other than the event stack
 * go through the processor API's call callback.
 *
 * Kind 0xd record: +4 stat, +8 word, +0xc target ids, +0x16 target count,
 * +0x17 stages (signed), +0x18 flags (bit 0 clear: report, bit 1, bit 3
 * keep the message at +0x1c, bit 4 skip event 5), message at +0x1c.
 * Kind 0x1e record: +4 weather (0 clears, below 5), +5 turns (0xff
 * permanent), +6 clear allowed, message at +8. The field (context +0x1c)
 * holds the weather at +1 and its turns at +4.
 * Kind 0x20 record: +4 ability (u16), +6 battler id, +7 force, message at
 * +0xc. */

/* 10823014: change stat stages of the listed targets. */
uint32_t px_battle_kind_stages(uint32_t context, uint32_t record, uint32_t low, const struct px_battle_process_api *api);
/* 10824774: start or end weather. */
uint32_t px_battle_kind_weather(uint32_t context, uint32_t record, const struct px_battle_process_api *api);
/* 10824978: change a battler's ability. */
uint32_t px_battle_kind_ability(uint32_t context, uint32_t record, const struct px_battle_process_api *api);

#define PX_BATTLE_FIXED_ABILITIES UINT32_C(0x108d10f0) /* literal 10824b98 (ABS32): two abilities that cannot change */

#endif
