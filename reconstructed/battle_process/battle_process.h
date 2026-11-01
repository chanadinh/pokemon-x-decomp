#ifndef PX_BATTLE_PROCESS_H
#define PX_BATTLE_PROCESS_H

#include "../battle_event/battle_event.h"

/* The DllBattle action processor 10819600 (static analysis address; runtime
 * addresses module-relative), which battle_action's commit runs on every
 * record. Analyst names.
 *
 * Context: +8 battler table (battlers at +0x84 + 4 id, ids below 0x18),
 * +0xc command state, +0x18 byte, +0x1c side table, +0xdb4 field pointer,
 * +0x1093 flag byte, +0x108c entry count, entries of 16 bytes from +0x10bc,
 * +0x120c position table, +0x437c action-stack word. Stack word bits:
 * 0x40000000 a record ran, 0x20000000 one succeeded, 0x10000000 the last
 * one succeeded.
 *
 * Record header bits: 0x1000000 run only after a success (or before any
 * record), 0x2000000 the battler (bits 8-12) must have value 0xd (HP)
 * nonzero, 0x800000 show the ability popup around the effect. */
struct px_battle_process_api {
    /* Memory, the fault routine (abort, 10800394) and the real event stack. */
    const struct px_battle_event_api *events;
    /* Every other callee, by its static address, with its arguments in
     * order (registers then stack words). Values are returned in r0. */
    uint32_t (*call)(void *user, uint32_t address, const uint32_t *args, unsigned count);
    /* A logical word standing in for the frame word kind 0x2f passes out. */
    uint32_t scratch;
};

enum { PX_BATTLE_PROCESS_KINDS = 0x3f };

/* 10819600: run one action record. */
void px_battle_action_execute(uint32_t context, uint32_t record, const struct px_battle_process_api *api);

/* The routine a kind hands over to (0 for kinds handled inline, the fault
 * path or none). Arguments: context, record and, for the four kinds marked
 * in the table, the low 10 bits of the stack word. */
uint32_t px_battle_process_routine(uint32_t kind);

/* Callees, for the host's call table. */
enum {
    PX_BATTLE_POKEMON_VALUE = 0x10874420, PX_BATTLE_SHOW_MESSAGE = 0x1086bbe8, PX_BATTLE_COMMAND = 0x1086c22c,
    PX_BATTLE_MESSAGE_COMMAND = 0x1086b5e0, PX_BATTLE_POSITION = 0x1086c2cc
};

#endif
