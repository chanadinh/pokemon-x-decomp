#ifndef PX_BATTLE_CORE_1_H
#define PX_BATTLE_CORE_1_H

#include "../battle_items_1/battle_items_1.h"

/* DllBattle core battle-state helpers, first batch: small accessors and
 * queries that earlier milestones left as external callbacks (static analysis
 * addresses; runtime addresses are module-relative). The names are analyst
 * interpretations.
 *
 * Layouts used here. Battle context: byte +0x108c (4236) the number of
 * battler entries; entries of 16 bytes from +0x10bc (+0x1000 + 0xbc), each
 * with a battler pointer at +0 (its slot byte at +0x19) and a word at +4 (low
 * nibble a kind) and a flag byte at +0xd; bit array +0x166 and +0x169; halfwords
 * +0x15c and +0x160; word +0x1080. Battler: conditions as words at +0x1c +
 * 4 index (active when the low 3 bits are set); type bytes +0x100, +0x101 and
 * +0x102; word +0x7c (low 3 bits: a flag); byte +0x153. A slot belongs to side
 * 1 from slot 12; 0x1f means no battler. */
#define PX_BATTLE_NO_TYPE 0x12   /* "no type" in the type bytes */

struct px_battle_core_api {
    /* Event stack, memory, battler lookup, records, messages and commit. */
    const struct px_battle_items_api *items;
};

/* Pure value helpers. */
uint32_t px_battle_same_side(uint32_t a, uint32_t b);                    /* 1084c5a8 */
uint32_t px_battle_pack_1(uint32_t value);                               /* 1084c3b0: value << 9 | 1 */
uint32_t px_battle_pack_3(uint32_t value);                               /* 10876e20: (value << 3 & 0x1f8) | 3 */

/* Battler queries. */
uint32_t px_battle_has_type(uint32_t pokemon, uint32_t type, const struct px_pokemon_api *memory);       /* 1086f960 */
uint32_t px_battle_pokemon_status(uint32_t pokemon, const struct px_pokemon_api *memory);                /* 1086d8ac */
uint32_t px_battle_find_entry(uint32_t array, uint32_t value, const struct px_pokemon_api *memory);      /* 1086c2cc */
uint32_t px_battle_first_set_id(uint32_t context, uint32_t table, const struct px_pokemon_api *memory);  /* 10874348 */
uint32_t px_battle_byte_9(uint32_t pokemon, const struct px_pokemon_api *memory);                        /* 1084b8d4 */
uint32_t px_battle_bit_5_of_1b(uint32_t pokemon, const struct px_pokemon_api *memory);                   /* 10851c28 */
uint32_t px_battle_field_16(uint32_t word_address, const struct px_pokemon_api *memory);                 /* 1084b648 */
uint32_t px_battle_array_word(uint32_t array, uint32_t index, const struct px_pokemon_api *memory);      /* 1085daf8 */
uint32_t px_battle_low_half(uint32_t address, const struct px_pokemon_api *memory);                      /* 1084d364 */
uint32_t px_battle_count_matches(uint32_t log, uint32_t id, uint32_t value, const struct px_pokemon_api *memory); /* 108c9ac8 */

/* Context queries. */
uint32_t px_battle_bit_set(uint32_t context, uint32_t index, const struct px_pokemon_api *memory);       /* 1084c818 */
uint32_t px_battle_half_160(uint32_t context, const struct px_pokemon_api *memory);                      /* 1084c7fc */
uint32_t px_battle_half_15c(uint32_t context, const struct px_pokemon_api *memory);                      /* 108c86c8 */
uint32_t px_battle_word_1080_set(uint32_t context, const struct px_pokemon_api *memory);                 /* 1084c70c */
uint32_t px_battle_first_kind_word(uint32_t context, uint32_t slot, uint32_t out, const struct px_pokemon_api *memory); /* 1084b784 */
uint32_t px_battle_others_flagged(uint32_t context, uint32_t slot, const struct px_pokemon_api *memory); /* 1084b8e0 */

/* Updates. */
void px_battle_normalize_word(uint32_t address, const struct px_pokemon_api *memory);                    /* 1086b284 */
void px_battle_set_kind_bit(uint32_t address, uint32_t bit, const struct px_pokemon_api *memory);        /* 10876ca8 */
void px_battle_mark_if_small(uint32_t address, uint32_t value, const struct px_pokemon_api *memory);     /* 10876c28 */
void px_battle_store_byte_db4(uint32_t context, uint32_t value, const struct px_pokemon_api *memory);    /* 1084bef8 */
void px_battle_nop(void);                                              /* 10865c48, 1084d1c8, 108676cc */

/* 1084be58: runs event 0x83 with key 0x42 cleared; 0 when a handler set the key,
 * otherwise the byte at +1 of the word at +0x1c of the context. */
uint32_t px_battle_event_0x83(uint32_t context, const struct px_battle_core_api *api);
/* 1084a944: a battler's byte +0x153 changes to `value`: kind 0x3a record (id, value,
 * `flag` into bit 23 of the header) and message 0xde; nothing when it already equals. */
void px_battle_record_change(uint32_t unused, uint32_t context, uint32_t id, uint32_t value, uint32_t flag,
                             const struct px_battle_core_api *api);

#endif
