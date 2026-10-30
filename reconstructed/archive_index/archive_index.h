#ifndef PX_ARCHIVE_INDEX_H
#define PX_ARCHIVE_INDEX_H

#include "../archive_io/archive_io.h"

enum { PX_GARC_PARSER_SIZE = 0x14, PX_GARC_PARSER_VTABLE = 0x00597728,
       PX_ARCHIVE_OBJECT_SIZE = 0x78, PX_ARCHIVE_SELECTOR_CELL = 0x005d1124,
       PX_GROWTH_ARCHIVE_CELL = 0x005e52b0, PX_MOVE_ARCHIVE_CELL = 0x005e5338 };

/* Services below the reconstructed boundary; heaps are logical objects. */
struct px_archive_index_api {
    const struct px_archive_api *archive; /* memory and reconstructed reads */
    void *user;
    uint32_t (*try_enter)(void *user, uint32_t section);  /* 00123a6c, archive + 0x6c */
    void (*signal)(void *user, uint32_t event);           /* 0016f1e8, archive + 0x4c */
    uint32_t (*temporary_heap)(void *user, uint32_t heap); /* heap virtual +0x34 */
    uint32_t (*alignment)(void *user, uint32_t heap);      /* heap virtual +0x1c */
    uint32_t (*allocate)(void *user, uint32_t heap, uint32_t size,
                         uint32_t alignment, uint32_t flag); /* 0012b0c8 */
    void (*release)(void *user, uint32_t pointer);           /* 002ff520 */
    uint32_t (*construct)(void *user, uint32_t object, uint32_t heap,
                          uint32_t id, uint32_t flags);      /* 00128288 */
    /* Logical 4-byte region standing in for the builder's stack slot. */
    uint32_t scratch;
};

/* GARC parser object (vtable 00597728): header +4, FATO +8, FATB +0x0c,
 * FIMB +0x10. Entry indices are narrowed to u16. A sub-entry selector picks
 * the record whose bit is set in the entry's FATB bitmap; an unset bit uses
 * record zero. Shifts follow ARM register-shift rules. */
void px_garc_parser_parse(uint32_t parser, uint32_t buffer, const struct px_pokemon_api *memory); /* 00355f0c */
uint32_t px_garc_parser_count(uint32_t parser, const struct px_pokemon_api *memory);   /* 00495c24 */
uint32_t px_garc_parser_largest(uint32_t parser, const struct px_pokemon_api *memory); /* 00495c30 */
uint32_t px_garc_parser_span(uint32_t parser, uint32_t entry, uint32_t selector,
                             const struct px_pokemon_api *memory); /* 00495b60: end - start */
uint32_t px_garc_parser_length(uint32_t parser, uint32_t entry, uint32_t selector,
                               const struct px_pokemon_api *memory); /* 00495cbc */
uint32_t px_garc_parser_offset(uint32_t parser, uint32_t entry, uint32_t selector,
                               const struct px_pokemon_api *memory); /* 00495c3c */
uint32_t px_garc_parser_length_offset(void); /* 00495d40: header field holding the index length */

uint32_t px_create_garc_parser(uint32_t heap, const struct px_archive_index_api *api); /* 00128134 */
void px_destroy_garc_parser(uint32_t parser, const struct px_archive_index_api *api);  /* 00355f4c */

/* 00127efc: build the resident information table (span, length, offset) at
 * archive + 0x3c, then count u16 +0x34, largest member +0x38, and signal
 * +0x4c. Skipped when the section at +0x6c cannot be entered. */
void px_build_archive_index(uint32_t archive, uint32_t heap, const struct px_archive_index_api *api);
/* 00127eb4: read size bytes at offset within a member, without a count. */
void px_read_archive_member_part(uint32_t archive, uint32_t index, uint32_t offset, uint32_t size,
                                 uint32_t buffer, const struct px_archive_api *api);
/* 0011ecf8 / 0011ee68: create the growth or move archive once. */
void px_open_resource_archive(uint32_t cell, uint32_t heap, uint32_t id,
                              const struct px_archive_index_api *api);

#endif
