#ifndef PX_BATTLE_EVENT_H
#define PX_BATTLE_EVENT_H

#include "../pokemon_constructor/pokemon_constructor.h"

/* DllBattle event system (static analysis addresses; runtime addresses are
 * module-relative). Analyst names. Logical ARM32 addresses.
 *
 * Variable stack (96 entries): word +0 position, u16 keys +4 + 2i (0 empty,
 * 1 frame marker), values +0xc4 + 4i, maxima +0x244 + 4i, minima +0x3c4 + 4i,
 * kind bytes +0x544 + i (4 id, 1 value, 0 scaled, 3 multiplier).
 *
 * Battle state object: +0x60 first handler, +0x64 handlers in use (the pool
 * array holds free handlers from that index on), +0x68 dispatch depth.
 * Handler (0x3c bytes): +0 previous, +4 next, +8 entries
 * (8 bytes each: event byte, function word +4), byte +0x10 owner, flags
 * +0x18, work area +0x1c, byte +0x3a value. Flags: low 16 bits registration
 * depth, bits 16..23 entry count, 0x1000000 running, 0x2000000 and 0x40000000
 * disabled, 0x4000000 accepts event 0x7c, 0x8000000 removal requested,
 * 0x10000000 re-entry, 0x20000000 registered, 0x80000000 visit mark. */
#define PX_BATTLE_EVENT_STACK_STATIC UINT32_C(0x10b07350)   /* literals 1086b5dc ... 10867d60 */
#define PX_BATTLE_STATE_STATIC UINT32_C(0x108fca00)         /* literals 1086b9f8, 10865110 */
#define PX_BATTLE_FREE_HANDLERS_STATIC UINT32_C(0x10b1196c) /* literal 10865114 */
#define PX_BATTLE_EVENT_CAPACITY 0x60
#define PX_BATTLE_ANY_OWNER 7

struct px_battle_event_api {
    const struct px_pokemon_api *memory;
    void *user;
    void (*abort)(void *user);       /* 10800394(0, 0, 0); returns */
    void (*overflow)(void *user);    /* 108003a4 -> 0011e864 (0, 0, 0); returns */
    uint32_t (*blocked)(void *user, uint32_t context, uint32_t handler, uint32_t event); /* 108c59b4 */
    /* Indirect call of a handler function (guest address). */
    void (*call)(void *user, uint32_t function, uint32_t handler, uint32_t context,
                 uint32_t value, uint32_t work);
    uint32_t stack;           /* variable stack */
    uint32_t battle;          /* battle state object */
    uint32_t free_handlers;   /* handler pool array (free from index +0x64 on) */
};

void px_battle_event_push(const struct px_battle_event_api *api);                     /* 1086bb84 */
void px_battle_event_pop(const struct px_battle_event_api *api);                      /* 1086b880 */
void px_battle_event_set_id(uint32_t key, uint32_t value, const struct px_battle_event_api *api);     /* 1086bac0 */
void px_battle_event_set_value(uint32_t key, uint32_t value, const struct px_battle_event_api *api);  /* 1086b9fc */
void px_battle_event_set_scaled(uint32_t key, uint32_t value, const struct px_battle_event_api *api); /* 1086b520 */
void px_battle_event_set_multiplier(uint32_t key, uint32_t value, uint32_t minimum, uint32_t maximum,
                                    const struct px_battle_event_api *api);                           /* 10867c9c */
uint32_t px_battle_event_get(uint32_t key, const struct px_battle_event_api *api);    /* 1086b91c */
void px_battle_event_dispatch(uint32_t context, uint32_t event, const struct px_battle_event_api *api); /* 1086b9a4 */
/* 10864e98: one pass over the handler list. owner 7 accepts every owner;
 * nonzero check consults the blocked callback first. */
void px_battle_event_run_handlers(uint32_t context, uint32_t event, uint32_t owner, uint32_t check,
                                  const struct px_battle_event_api *api);

#endif
