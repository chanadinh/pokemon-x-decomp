#ifndef PX_POKEMON_RESOURCES_H
#define PX_POKEMON_RESOURCES_H

#include "../archive_index/archive_index.h"

/* Cells observed through literals; analyst names. */
enum {
    PX_RESOURCE_CONFIG_CELL = 0x005d1870,   /* pointer to the archive/capacity table */
    PX_RESOURCE_BASE = 0x005d1790,          /* resource cells +0x00..+0x44 */
    PX_PERSONAL_ARCHIVE_CELL = 0x005d1778,  /* +4: flat personal table (005d177c) */
    PX_LEARNSET_RESOURCE_CELL = 0x005d17e8, /* +0 descriptor, +4 archive */
    PX_ARCHIVE_5E5288_CELL = 0x005e5288,
    PX_ARCHIVE_5D1760_CELL = 0x005d1760,
    PX_PERSONAL_RECORD_BYTES = 0x40
};

struct px_resource_api {
    const struct px_archive_index_api *index; /* archive, heap and constructor services */
    void *user;
    /* 0012b094: heap block allocation used for loaded tables, external. */
    uint32_t (*allocate_block)(void *user, uint32_t heap, uint32_t size);
    /* 0011e264: message-resource construction (archive id, member, mode, flags). */
    uint32_t (*construct_message)(void *user, uint32_t object, uint32_t archive_id, uint32_t member,
                                  uint32_t heap, uint32_t mode, uint32_t flags);
};

/* Archive services that read the GARC header/index directly. */
uint32_t px_garc_parser_count_offset(void); /* 00495d38: u16 FATO entry count at 0x24 */
/* 00127ccc, 00127d3c..00127ea8: index re-read for archives without a resident
 * table. Matches px_archive_api.stream_info through a small adapter. */
void px_archive_member_info_streamed(uint32_t archive, uint32_t index, uint32_t info[3],
                                     const struct px_archive_index_api *api);
uint32_t px_archive_member_count(uint32_t archive, const struct px_archive_index_api *api); /* 00127bd8 */
uint32_t px_archive_member_size(uint32_t archive, uint32_t index, const struct px_archive_api *api); /* 0011c3cc */
void px_resource_abort(const struct px_pokemon_api *memory, const struct px_archive_api *api); /* 0011e864, never returns */
uint32_t px_allocate_aligned(uint32_t size, uint32_t heap, const struct px_archive_index_api *api); /* 001228ec */

/* Record and cache constructors; heaps are logical objects. Each returns object. */
uint32_t px_construct_personal_descriptor(uint32_t object, uint32_t heap, const struct px_resource_api *api); /* 0012b554 */
uint32_t px_construct_growth_record(uint32_t object, const struct px_resource_api *api);                   /* 0012b6a4 */
uint32_t px_construct_learnset_record(uint32_t object, uint32_t heap, const struct px_resource_api *api);   /* 0012b5d4 */
uint32_t px_construct_record_30(uint32_t object, uint32_t heap, const struct px_resource_api *api);         /* 0012b610 */
uint32_t px_construct_record_18(uint32_t object, uint32_t heap, const struct px_resource_api *api);         /* 0012b650 */
uint32_t px_construct_halfword_buffer(uint32_t object, uint32_t count, uint32_t heap,
                                      const struct px_resource_api *api);                                   /* 0012acd8 */
uint32_t px_construct_growth_cache(uint32_t object, uint32_t heap, uint32_t capacity, const struct px_resource_api *api);   /* 0011ea34 */
uint32_t px_construct_personal_cache(uint32_t object, uint32_t heap, uint32_t capacity, const struct px_resource_api *api); /* 0011eb08 */
uint32_t px_construct_learnset_cache(uint32_t object, uint32_t heap, uint32_t capacity, const struct px_resource_api *api); /* 0011eb84 */
uint32_t px_construct_cache_30(uint32_t object, uint32_t heap, uint32_t capacity, const struct px_resource_api *api);       /* 0011ec00 */
uint32_t px_construct_cache_18(uint32_t object, uint32_t heap, uint32_t capacity, const struct px_resource_api *api);       /* 0011ec7c */

/* 0011e884: open the personal archive once, check member sizes and load its
 * final flat member into a heap block stored at 005d177c. */
void px_load_personal_table(uint32_t heap, uint32_t id, const struct px_resource_api *api);
/* 0011e960: open/index the learnset archive and create its personal descriptor. */
void px_open_learnset_resources(uint32_t heap, uint32_t id, const struct px_resource_api *api);
/* 00112368: create every Pokemon data resource from the configuration table. */
void px_initialize_pokemon_resources(uint32_t heap, const struct px_resource_api *api);

#endif
