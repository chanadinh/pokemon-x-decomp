#ifndef PX_CGFX_H
#define PX_CGFX_H

#include "texture.h"

enum px_cgfx_result {
    PX_CGFX_OK = 0, PX_CGFX_INVALID_ARGUMENT, PX_CGFX_TRUNCATED,
    PX_CGFX_INVALID_HEADER, PX_CGFX_UNSUPPORTED, PX_CGFX_LIMIT_EXCEEDED,
    PX_CGFX_NO_MEMORY, PX_CGFX_NOT_FOUND
};
struct px_cgfx;
struct px_cgfx_texture {
    const char *name;             /* TXOB name, NUL terminated, at most 1024 bytes. */
    size_t name_size;
    const char *dictionary_name;  /* DICT key may differ from TXOB name. */
    size_t dictionary_name_size;
    uint32_t object_offset, image_offset, flags, revision;
    uint32_t width, height, stored_width, stored_height;
    uint32_t format, bits_per_pixel, mipmap_levels;
    uint32_t data_offset, data_size;
    enum px_cgfx_result decode_status;
};

/* Borrow a complete uncompressed CGFX buffer, immutable until close. Only the
 * index is allocated/owned here. Structural errors leave *out NULL. Supported
 * header revision is 0x05000000; maximum file 64 MiB / 65536 DICT entries.
 * Every DATA dictionary/name/object prefix is bounds checked. Texture metadata
 * with unsupported variants or layouts remains listable via decode_status;
 * structurally invalid pointers/extents reject the file. Pixel data is decoded
 * only on selection. Names remain borrowed until close and are arbitrary bytes. */
enum px_cgfx_result px_cgfx_open(const uint8_t *data, size_t size,
                                struct px_cgfx **out);
size_t px_cgfx_texture_count(const struct px_cgfx *cgfx);
const struct px_cgfx_texture *px_cgfx_texture(const struct px_cgfx *cgfx,
                                             size_t index);
enum px_cgfx_result px_cgfx_decode_texture(const struct px_cgfx *cgfx,
    size_t index, struct px_texture_image *out);
void px_cgfx_close(struct px_cgfx *cgfx);
const char *px_cgfx_result_string(enum px_cgfx_result result);

#endif
