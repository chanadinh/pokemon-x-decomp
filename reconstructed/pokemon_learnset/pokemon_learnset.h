#ifndef PX_POKEMON_LEARNSET_H
#define PX_POKEMON_LEARNSET_H

#include "../pokemon_constructor/pokemon_constructor.h"

enum { PX_LEARNSET_RECORD_SIZE = 12, PX_LEARNSET_ENTRY_LIMIT = 26,
       PX_LEARNSET_BUFFER_SIZE = 104 };

/* Analyst names; addresses and layouts are documented in README.md.
 * All addresses are logical ARM32 addresses. Resource services may mutate
 * logical memory; required memory and callbacks must remain valid. */
struct px_learnset_api {
    const struct px_pokemon_api *memory;
    void *user;
    void (*select_personal)(void *user, uint32_t record, uint32_t species,
                             uint32_t form);                       /* 00174c0c */
    uint32_t (*personal_value)(void *user, uint32_t record,
                                uint32_t selector);                /* 00175c78 */
    void (*read_member)(void *user, uint32_t archive, uint32_t index,
                          uint32_t destination);                   /* 00138214 */
};

/* Record: species u16 +0, form u8 +2, buffer pointer u32 +4,
 * count u8 +8. Invalid full-width indices fall back to entry zero, even
 * when count is zero. The backing buffer must contain that entry. */
uint32_t px_learnset_record_count(uint32_t record, const struct px_pokemon_api *memory);
uint32_t px_learnset_record_level(uint32_t record, uint32_t index, const struct px_pokemon_api *memory);
uint32_t px_learnset_record_move(uint32_t record, uint32_t index, const struct px_pokemon_api *memory);

/* 0014f294 / 0014f2a4 / 0014f310: record pointer at 005d1790+24. */
uint32_t px_current_learnset_count(const struct px_pokemon_api *memory);
uint32_t px_current_learnset_level(uint32_t index, const struct px_pokemon_api *memory);
uint32_t px_current_learnset_move(uint32_t index, const struct px_pokemon_api *memory);

/* 00175fb4: copies all 104 payload bytes, then species, form and count.
 * Pointers, padding and other descriptor bytes stay with the destination.
 * Payloads must be disjoint or identical and disjoint from descriptors and
 * cache metadata; ordinary-memory byte copying omits ARM store grouping. */
void px_copy_learnset_record(uint32_t source, uint32_t destination, const struct px_pokemon_api *memory);

/* Cache: pointer table u32 +4, slot count u32 +8, replacement cursor u32
 * +c. Every slot is a valid initialized descriptor with its own buffer.
 * Store requires a nonzero slot count and a valid replacement cursor. */
uint32_t px_find_cached_learnset(uint32_t cache, uint32_t species, uint32_t form,
                                uint32_t destination, const struct px_pokemon_api *memory);
void px_store_cached_learnset(uint32_t cache, uint32_t source, const struct px_pokemon_api *memory);

/* 00167ca4: form-aware resource index, bounded terminator scan and identity
 * update. 0014f2b8: current record/cache at 005d1790+24/+28, then find/load/store. */
void px_load_learnset_record(uint32_t record, uint32_t species, uint32_t form, const struct px_learnset_api *api);
void px_select_pokemon_learnset(uint32_t species, uint32_t form, const struct px_learnset_api *api);

#endif
