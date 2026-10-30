#ifndef PX_ARCHIVE_IO_H
#define PX_ARCHIVE_IO_H

#include "../pokemon_constructor/pokemon_constructor.h"

/* Result words observed as literals; analyst names. */
#define PX_FS_RESULT_NULL_ARGUMENT UINT32_C(0xe0e046bc) /* 00133158 */
#define PX_FS_RESULT_BAD_WHENCE UINT32_C(0xe0e046bd)     /* 00133768 */
#define PX_FS_RESULT_BAD_POSITION UINT32_C(0xe0e046c1)   /* 00133628 */
enum { PX_ARCHIVE_PANIC_HOOK = 0x005d1144, PX_ARCHIVE_PANIC_MESSAGE = 0x00563574 };

/* Services below the reconstructed boundary. Results are ARM r0 words; bit 31
 * marks failure. Handles are the stream word with bit 0 cleared. */
struct px_archive_api {
    const struct px_pokemon_api *memory;
    void *user;
    void (*lock)(void *user, uint32_t section);   /* 0015f828, archive + 0x60 */
    void (*unlock)(void *user, uint32_t section); /* 0015f8f0, archive + 0x60 */
    /* Virtual +0x28 of the object at archive + 8, called by 00127b6c. */
    uint32_t (*is_open)(void *user, uint32_t object);
    /* Virtual +0x0c of a nonzero handle, reached through 00133130. */
    uint32_t (*get_size)(void *user, uint32_t handle, uint64_t *size);
    /* 00146588: platform read at an absolute 64-bit position. */
    uint32_t (*read)(void *user, uint32_t handle, uint64_t position,
                     uint32_t buffer, uint32_t size, uint32_t *count);
    void (*fatal)(void *user, uint32_t result, uint32_t site); /* 00124504 */
    /* 00127d3c..00127ea8: non-resident member-information path, external. */
    void (*stream_info)(void *user, uint32_t archive, uint32_t index, uint32_t info[3]);
    /* Called with the nonzero hook word and message; it must not return
     * normally if the caller expects to leave the original's endless loop. */
    void (*panic)(void *user, uint32_t hook, uint32_t message);
};

/* File stream: handle word +0, position s64 +4, cached size s64 +0x0c. */
uint32_t px_file_get_size(uint32_t handle, uint64_t *size, const struct px_archive_api *api); /* 00133130 */
uint32_t px_file_stream_set_position(uint32_t stream, uint64_t position,
                                     const struct px_archive_api *api); /* 0013358c */
uint32_t px_file_stream_seek(uint32_t stream, uint64_t offset, uint32_t whence,
                             const struct px_archive_api *api); /* 001336bc */
uint32_t px_file_stream_read(uint32_t stream, uint32_t buffer, uint32_t size,
                             uint32_t *count, const struct px_archive_api *api); /* 00133634 */

/* Archive: object +8, stream +0x18, member count u16 +0x34, resident
 * information table +0x3c (12-byte rows: size, unused word, offset), resident
 * flag word +0x4c (negative selects the external path), lock +0x60. */
void px_archive_member_info(uint32_t archive, uint32_t index, uint32_t info[3],
                            const struct px_archive_api *api); /* 00127ccc */
uint32_t px_archive_read(uint32_t archive, uint32_t size, uint32_t buffer,
                         const struct px_archive_api *api); /* 00127b6c */
/* count may be NULL, as the original tests its out pointer. */
void px_archive_read_at(uint32_t archive, uint32_t offset, uint32_t size, uint32_t buffer,
                        uint32_t *count, const struct px_archive_api *api); /* 00127c5c */
uint32_t px_read_archive_member(uint32_t archive, uint32_t index, uint32_t buffer,
                                const struct px_archive_api *api); /* 00138214 */
void px_seek_archive_member(uint32_t archive, uint32_t index, uint32_t offset,
                            const struct px_archive_api *api); /* 001381c4 */

#endif
