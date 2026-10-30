#ifndef PX_MESSAGE_CONSTRUCTION_H
#define PX_MESSAGE_CONSTRUCTION_H

#include "../pokemon_resources/pokemon_resources.h"

enum { PX_MESSAGE_RESOURCE_SIZE = 0x28, PX_MESSAGE_RESOURCE_VTABLE = 0x005985b4,
       PX_MESSAGE_LANGUAGE_CELL = 0x005d113c, PX_MOVE_RESOURCE_BASE = 0x005d1888 };

struct px_message_construction_api {
    const struct px_resource_api *resources; /* archives, heaps, allocators */
    void *user;
    void (*release_block)(void *user, uint32_t block);   /* 0013a50c */
    void (*destroy_archive)(void *user, uint32_t archive); /* archive virtual +4 */
    uint32_t (*construct_listed)(void *user, uint32_t object, uint32_t heap,
                                 uint32_t id, uint32_t flags); /* 0011c3e8, path-table archive */
    /* Logical 12-byte region standing in for 0012ad74's stack header. */
    uint32_t header_scratch;
};

/* Message resource (0x28 bytes): vtable +0, heap +4, archive +8, mode byte
 * +0x0c, member +0x10, language section +0x14, header +0x18, section +0x1c,
 * entry scratch +0x20 (cleared as u32 +0x20, u16 +0x24 and u16 +0x26). */

/* 0012ad74: read the member's 12-byte header and section offsets, select the
 * language from 005d113c (zero when out of range), and in cached mode load
 * that language's whole section. A zero archive enters the abort loop. */
void px_open_message_member(uint32_t resource, uint32_t member, uint32_t heap,
                            const struct px_message_construction_api *api);
/* 0011e264 / 0011e308: construct around a new archive object, then open the
 * member. The second form uses the path-table archive constructor. */
uint32_t px_construct_message_resource(uint32_t object, uint32_t archive_id, uint32_t member, uint32_t heap,
                                       uint32_t mode, uint32_t flags, const struct px_message_construction_api *api);
uint32_t px_construct_listed_message_resource(uint32_t object, uint32_t archive_id, uint32_t member, uint32_t heap,
                                              uint32_t mode, uint32_t flags,
                                              const struct px_message_construction_api *api);
/* 0038fc20 / 0038fbc4: release section and header, delete the archive; the
 * second form also frees the object through 002ff520. */
uint32_t px_destroy_message_resource(uint32_t resource, const struct px_message_construction_api *api);
void px_delete_message_resource(uint32_t resource, const struct px_message_construction_api *api);

/* 0012b968 / 0011edec: move record (36-byte payload) and byte-capacity cache. */
uint32_t px_construct_move_record(uint32_t object, uint32_t heap, const struct px_message_construction_api *api);
uint32_t px_construct_move_cache(uint32_t object, uint32_t heap, uint32_t capacity,
                                 const struct px_message_construction_api *api);
/* 00112894: open the move archive (configuration word 12), create the move
 * message resource (words 0x11 and 0x15), record and cache (word 13). */
void px_initialize_move_resources(uint32_t heap, const struct px_message_construction_api *api);

#endif
