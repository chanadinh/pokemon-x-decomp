#ifndef PX_BCH_TEXTURE_H
#define PX_BCH_TEXTURE_H

#include "texture.h"

enum px_bch_result {
    PX_BCH_OK = 0, PX_BCH_INVALID_ARGUMENT, PX_BCH_TRUNCATED,
    PX_BCH_INVALID_HEADER, PX_BCH_UNSUPPORTED, PX_BCH_LIMIT_EXCEEDED,
    PX_BCH_NO_MEMORY, PX_BCH_NOT_FOUND
};
struct px_bch;
struct px_bch_texture {
    const char *name;             /* Borrowed, at most 1024 bytes before NUL. */
    size_t name_size;
    const char *dictionary_name;  /* Required to agree with the object name. */
    size_t dictionary_name_size;
    uint32_t object_offset, flags;
    uint32_t width, height, format, mipmap_levels;
    uint32_t data_offset, data_size;
    enum px_bch_result decode_status;
};

/* Borrow an immutable complete BCH 7/7 buffer until close; own only the index.
 * Sections, resource tables, names, relocation sources/targets and all three
 * texture command streams are bounded before a texture is exposed. Only the
 * texture-only subset with relocation tags 00/01/02/24 is supported. Limits:
 * 64 MiB file, 65535 resources, 262144 relocations, 4096 words per command
 * stream, 4194304 total command words and 4096 pixels per dimension.
 * ETC1/ETC1A4, power-of-two dimensions >=8 and descriptor flags 0x10c/0x10d
 * are the validated layouts: flags == (levels << 8) | format with 1..13
 * levels; only the base level is decoded (mipmaps are not sampled). Unsupported texture metadata stays
 * listable through decode_status. Pixel allocation occurs only on selection.
 * Errors leave *out NULL. Arbitrary-byte names are borrowed until close. */
enum px_bch_result px_bch_open(const uint8_t *data, size_t size,
                              struct px_bch **out);
/* As px_bch_open, for a BCH that also holds other resources (models,
 * materials, lookup tables...): only category 3 is indexed, and relocation
 * tags 03 and 25..28 are additionally accepted (bounded, not interpreted).
 * Raw formats 0..11 also decode. */
enum px_bch_result px_bch_open_mixed(const uint8_t *data, size_t size,
                                    struct px_bch **out);
size_t px_bch_texture_count(const struct px_bch *bch);
const struct px_bch_texture *px_bch_texture(const struct px_bch *bch,
                                           size_t index);
enum px_bch_result px_bch_decode_texture(const struct px_bch *bch,
    size_t index, struct px_texture_image *out);
void px_bch_close(struct px_bch *bch);
const char *px_bch_result_string(enum px_bch_result result);

#endif
