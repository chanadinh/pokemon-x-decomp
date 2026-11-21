#ifndef PX_BATTLE_LOW_1_H
#define PX_BATTLE_LOW_1_H

#include "../battle_core_1/battle_core_1.h"

/* DllBattle low-address helpers, first batch: 15 functions in 10802000-10840000
 * that committed milestones call but never reconstructed (static analysis
 * addresses; runtime addresses are module-relative). The names are analyst
 * interpretations.
 *
 * Layouts used here. Battle context: word +4 a slot-state object, word +8 the
 * battler pointer table (24 entries from +0x84), word +0xc an event writer
 * context; byte +0x1091 the length of a byte list at +0x1094 (at most 10),
 * word +0x1084 a counter, byte +0x108c the number of battler entries (16 bytes
 * each from +0x10bc: battler pointer at +0, kind word at +4 with the low
 * nibble 8 meaning "skip", flag byte +0xd), halfword +0x4674, bytes
 * +0x189c.. a list of at most 6 values with its length at +0x18a6. Battler:
 * id byte +0x19, bit array +0x169, word +0x84 (low 3 bits: a flag).
 *
 * Aborts are the abort helper 10800394(0, 0, 0): REA's listing spells its
 * four instructions "FAULT". */
struct px_battle_low_api {
    /* Event stack, battler lookup and pokemon_value (10874420). */
    const struct px_battle_ability_api *base;
    uint32_t (*slot_present)(void *user, uint32_t object, uint32_t index);                    /* 10870368 */
    uint32_t (*owner_of)(void *user, uint32_t id);                                            /* 1087425c */
    uint32_t (*entry_score)(void *user, uint32_t context, uint32_t entry, uint32_t arg);      /* 108676d0 */
    void (*entry_first)(void *user, uint32_t context, uint32_t iterator);                     /* 108656c4 */
    uint32_t (*entry_next)(void *user, uint32_t context, uint32_t iterator, uint32_t out);    /* 10865568 */
    void (*record_event)(void *user, uint32_t writer, uint32_t kind, uint32_t a, uint32_t b,
                         uint32_t c, uint32_t d, uint32_t e);                                  /* 1086c22c */
    void (*queue_change)(void *user, uint32_t context, uint32_t battler, uint32_t kind, uint32_t flag); /* 108695c0 */
    uint32_t (*random_below)(void *user, uint32_t state, uint32_t limit);                     /* 108003dc */
    uint32_t (*base_pp)(void *user, uint32_t move, uint32_t flag);                            /* 10800984 */
    void (*commit_slot)(void *user, uint32_t slot, uint32_t record);                          /* 1085fa8c */
    uint32_t rng_object; /* the relocated pointer at 10826260 (108fca00): the random state object */
    uint32_t iterator;   /* logical scratch: two words standing in for 1081d2b0's stack frame */
};

/* Pure value helpers. */
uint32_t px_battle_is_mode_2_or_3(uint32_t value);                                     /* 10814f9c */
uint32_t px_battle_context_0ca4(uint32_t context);                                     /* 1081cc04: context + 0xca4 */
uint32_t px_battle_half_4674(uint32_t context, const struct px_pokemon_api *memory);    /* 1081cf08 */
uint32_t px_battle_next_counter(uint32_t context, const struct px_pokemon_api *memory); /* 1081d83c */
void px_battle_push_byte(uint32_t context, uint32_t value, const struct px_pokemon_api *memory); /* 1081d748 */
uint32_t px_battle_add_capped(uint32_t object, uint32_t amount, const struct px_pokemon_api *memory); /* 1081ca98 */
void px_battle_add_unique(uint32_t context, uint32_t value, const struct px_battle_low_api *api); /* 10810cc0 */
/* Services. */
uint32_t px_battle_slot_state(uint32_t context, uint32_t index, const struct px_battle_low_api *api);     /* 1081e28c */
uint32_t px_battle_partner_entry(uint32_t context, uint32_t id, const struct px_battle_low_api *api);     /* 1082001c */
void px_battle_record_0x37(uint32_t context, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e,
                           const struct px_battle_low_api *api);                                           /* 1081cf14 */
uint32_t px_battle_count_better(uint32_t context, uint32_t entry, uint32_t arg, const struct px_battle_low_api *api); /* 1081d2b0 */
uint32_t px_battle_event_0xb_allows(uint32_t context, uint32_t id, const struct px_battle_low_api *api);  /* 1081d770 */
uint32_t px_battle_chance_event(uint32_t context, uint32_t key3, uint32_t key4, uint32_t value,
                                const struct px_battle_low_api *api);                                      /* 108261e8 */
void px_battle_set_move_slot(uint32_t unused, uint32_t slot_object, uint32_t move, uint32_t limit, uint32_t first,
                             const struct px_battle_low_api *api);                                         /* 1082b064 */
uint32_t px_battle_clear_bit_flag(uint32_t context, uint32_t battler, uint32_t index, uint32_t quiet,
                                  const struct px_battle_low_api *api);                                    /* 1081fc20 */

#endif
