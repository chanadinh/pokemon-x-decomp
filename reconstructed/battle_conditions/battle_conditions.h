#ifndef PX_BATTLE_CONDITIONS_H
#define PX_BATTLE_CONDITIONS_H

#include "../battle_process/battle_process.h"

/* The DllBattle status applier 108629b8 (static analysis address, runtime
 * address module-relative), which the status kind routine (battle_changes,
 * kind 0xb) calls once the status is not blocked. Analyst names. Callees
 * other than the event stack go through the processor API's call callback.
 *
 * Statuses: 1-5 the major ones (each with an animation, command 0x57), 6
 * and 7 with their own animations, 0x10 handled by event 0x73. */

/* The animation of statuses 0-7 (inner jump table 108629fc, literals
 * 10862de8-10862dfc); 0 for none. */
uint32_t px_battle_status_animation(uint32_t status);

/* 108629b8: apply status to target. word is passed to 10865118, 1087afa0
 * and key 0x1f; with build set, the message comes from 1087afa0 (into the
 * context's message buffer +0x433c, cleared after), otherwise message (if
 * any) is shown; keep_item set skips the held-item event. */
void px_battle_apply_status(uint32_t context, uint32_t target, uint32_t source, uint32_t status, uint32_t word, uint32_t build,
                            uint32_t keep_item, uint32_t message, const struct px_battle_process_api *api);

#define PX_BATTLE_SHAYMIN 0x1ec /* species 492: its Sky Forme (form 1) reverts when frozen (status 3) */

#endif
