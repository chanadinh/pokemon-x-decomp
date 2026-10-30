#ifndef PX_BATTLE_MATCHUP_H
#define PX_BATTLE_MATCHUP_H

#include "../battle_types/battle_types.h"

/* DllBattle type matchup (static analysis addresses; runtime addresses are
 * module-relative). Analyst names. Logical ARM32 addresses.
 * Battle Pokemon: id byte +0x19, flags byte +0x1b (bit 0x40 selects a
 * replacement battler), status word +0x7c, types bytes +0x100..+0x102.
 * Move: type byte +6. Context: +4, +8 battler table (word +0x84 + 4 x index),
 * +0xc message target, +0x1c field state (word +0x27c), +0x1080 nesting
 * counter, +0x437c message state. Type 2 is Flying, 4 is Ground. */
struct px_battle_matchup_api {
    const struct px_pokemon_api *memory;
    void *user;
    /* Chart, power table, Inverse Battle flag and the abort helper 10800394. */
    const struct px_battle_types_api *types;
    /* Battle event system (DllBattle). */
    void (*event_push)(void *user);                                        /* 1086bb84 */
    void (*event_pop)(void *user);                                         /* 1086b880 */
    void (*event_set_id)(void *user, uint32_t key, uint32_t value);        /* 1086bac0 */
    void (*event_set_value)(void *user, uint32_t key, uint32_t value);     /* 1086b9fc */
    void (*event_dispatch)(void *user, uint32_t context, uint32_t event);  /* 1086b9a4 */
    uint32_t (*event_get)(void *user, uint32_t key);                       /* 1086b91c */
    /* Battle helpers (DllBattle). */
    void (*type_event_state)(void *user, uint32_t defender);               /* 10888a88 */
    uint32_t (*airborne)(void *user, uint32_t context, uint32_t pokemon,
                         uint32_t check_flying);                           /* 108651d8 */
    uint32_t (*message_state)(void *user, uint32_t state);                 /* 1086b284 */
    void (*message)(void *user, uint32_t target, uint32_t group, uint32_t id,
                    uint32_t pokemon_id, uint32_t extra);                  /* 1086b5e0 */
    uint32_t (*replacement)(void *user, uint32_t owner, uint32_t table,
                            uint32_t pokemon);                             /* 10872ff8 */
    void (*load_move)(void *user, uint32_t context, uint32_t move,
                      uint32_t attacker, uint32_t out);                    /* 10869c5c */
    /* Logical 20-byte area standing in for the original stack move record. */
    uint32_t move_scratch;
};

/* 10870994: the Pokemon's types as three 5-bit fields (type 1, 2, 3).
 * A nonzero low three bits of +0x7c remove Flying (Roost). */
uint32_t px_battle_packed_types(uint32_t pokemon, const struct px_battle_matchup_api *api);
/* 1086094c: one attacking type against one defending type with event 0x44. */
uint32_t px_battle_single_matchup(uint32_t context, uint32_t attacker, uint32_t defender,
                                  uint32_t attack_type, uint32_t defend_type,
                                  const struct px_battle_matchup_api *api);
/* 10826264: a move against every defending type, with Ground immunity rules;
 * nonzero show_message reports an immunity. */
uint32_t px_battle_matchup(uint32_t context, uint32_t attacker, uint32_t defender, uint32_t move,
                           uint32_t show_message, const struct px_battle_matchup_api *api);
/* 10863d44: battler indices and a move id; no message. */
uint32_t px_battle_matchup_by_index(uint32_t context, uint32_t attacker_index,
                                    uint32_t defender_index, uint32_t move,
                                    const struct px_battle_matchup_api *api);

#endif
