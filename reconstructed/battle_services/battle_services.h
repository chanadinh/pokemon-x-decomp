#ifndef PX_BATTLE_SERVICES_H
#define PX_BATTLE_SERVICES_H

#include "../battle_process/battle_process.h"

/* DllBattle battler services that the action processor's kind routines
 * call (static analysis addresses, runtime addresses module-relative).
 * Analyst names. Callees other than the event stack go through the
 * processor API's call callback.
 *
 * Battlers: stat stages (signed bytes, neutral 6, 0-12) at +0x104 for
 * stats 1-7, values (u16) at +0xf6 for keys 8-0xc, HP +0x10, maximum +0xe,
 * conditions (words) at +0x1c + 4 index with turn bytes at +0xb4 + index,
 * protection u16 +0x208, field effect pointer +0x210. */

/* 108722e8: whether a stage can still move in the direction of delta. */
uint32_t px_battle_stage_can_change(uint32_t pokemon, uint32_t stat, int32_t delta, const struct px_battle_process_api *api);
/* 108714fc: set a stage (0-12). */
void px_battle_set_stage(uint32_t pokemon, uint32_t stat, uint32_t value, const struct px_battle_process_api *api);
/* 108620d4: a value key, with 9 and 0xb exchanged while the field effect
 * at +0x210 has its word +0x28c set. */
uint32_t px_battle_value_key(uint32_t pokemon, uint32_t key, const struct px_battle_process_api *api);
/* 10871250: set value key 8-0xc. */
void px_battle_set_value(uint32_t pokemon, uint32_t key, uint32_t value, const struct px_battle_process_api *api);
/* 10865788: restore (positive) or lose (negative) HP, optionally with
 * command 0x48. */
void px_battle_change_hp(uint32_t context, uint32_t pokemon, int32_t amount, uint32_t report, const struct px_battle_process_api *api);
/* 10867a04: lose HP (command 1, or 2 when flagged). */
void px_battle_lose_hp(uint32_t context, uint32_t pokemon, uint32_t amount, uint32_t flag, const struct px_battle_process_api *api);
/* 108695c0: cure a condition, copying its word to out first when given. */
void px_battle_cure_condition(uint32_t context, uint32_t pokemon, uint32_t condition, uint32_t out, const struct px_battle_process_api *api);
/* 10862858: nonzero when a status cannot be applied. */
uint32_t px_battle_status_blocked(uint32_t context, uint32_t target, uint32_t source, uint32_t status, uint32_t word, uint32_t byte,
                                  int8_t report, const struct px_battle_process_api *api);

/* 10860e74's arguments after context, source id, battler and stat. */
struct px_battle_stage_change {
    int32_t delta;
    uint32_t source_key; /* key 3 of event 0x63 */
    uint32_t low;        /* the stack word's low bits, passed on to 1081c0a0 */
    uint32_t word;       /* key 0x1a */
    uint32_t report, quiet, message, forced;
};
/* 10860e74: change a battler's stat stage; 1 when it changed. */
uint32_t px_battle_apply_stage(uint32_t context, uint32_t source, uint32_t pokemon, uint32_t stat, const struct px_battle_stage_change *change,
                               const struct px_battle_process_api *api);

#endif
