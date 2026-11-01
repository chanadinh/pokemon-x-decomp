#ifndef PX_BATTLE_ACTION_H
#define PX_BATTLE_ACTION_H

#include "../pokemon_constructor/pokemon_constructor.h"

/* The DllBattle action stack and message builders shared by every handler
 * (static analysis addresses, runtime addresses module-relative). Analyst
 * names.
 *
 * The stack header is the word at context + 0x437c: bits 10-18 count the
 * bytes in use, which start at context + 0x4380 (at most 0x1f4). A record
 * starts with a header word: kind (bits 0-7), battler id (8-12), size in
 * bytes (13-22), bit 23 cleared, bit 26 set. A message is a u16 id, a u16
 * of kind (bits 0-6) and argument count (7-13), and up to 8 word arguments. */
struct px_battle_action_api {
    const struct px_pokemon_api *memory;
    /* (kind, size) byte pairs, 0x3d of them: the original table at 108d1ed8. */
    const uint8_t *sizes;
    void (*execute)(void *user, uint32_t context, uint32_t record); /* 10819600 */
    void (*fault)(void *user);                                       /* 10800394(0, 0) */
};

enum { PX_BATTLE_ACTION_STACK = 0x437c, PX_BATTLE_ACTION_KINDS = 0x3d, PX_BATTLE_ACTION_CAPACITY = 0x1f4,
       PX_BATTLE_TEMP_BUFFER = 0x467a, PX_BATTLE_TEMP_BUFFER_SIZE = 0x140 };

/* 10869f88: push a zeroed record of the given kind; 0 (after a fault) if the
 * kind is unknown, has size 0, or does not fit. */
uint32_t px_battle_action_push(uint32_t stack, uint32_t kind, uint32_t id, const struct px_battle_action_api *api);
/* 10869f7c: the same on the context's stack. */
uint32_t px_battle_action_record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_action_api *api);
/* 10867b48: execute a record, then pop it; it must be the top record. */
void px_battle_action_commit(uint32_t context, uint32_t record, const struct px_battle_action_api *api);
/* 1084c948: push and commit a header-only record (phase 1 shows the
 * ability popup, phase 2 hides it). */
void px_battle_ability_popup(uint32_t context, uint32_t phase, uint32_t id, const struct px_battle_action_api *api);
/* 1084d2d4: start a message, clearing its arguments. */
void px_battle_message(uint32_t message, uint32_t kind, uint32_t id, const struct px_battle_action_api *api);
/* 1084d29c: append an argument; a ninth is dropped. */
void px_battle_message_arg(uint32_t message, uint32_t value, const struct px_battle_action_api *api);
/* 1084c7d0: the context's temporary buffer; a size of 0x140 or more faults. */
uint32_t px_battle_temp_buffer(uint32_t context, uint32_t size, const struct px_battle_action_api *api);

#endif
