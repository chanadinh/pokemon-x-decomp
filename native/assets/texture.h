#ifndef PX_TEXTURE_H
#define PX_TEXTURE_H

#include <stddef.h>
#include <stdint.h>

/* PICA/CGFX hardware numbering, deliberately distinct from BCLIM IDs. */
enum px_texture_format {
    PX_TEXTURE_RGBA8 = 0, PX_TEXTURE_RGB8, PX_TEXTURE_RGBA5551,
    PX_TEXTURE_RGB565, PX_TEXTURE_RGBA4, PX_TEXTURE_LA8, PX_TEXTURE_HILO8,
    PX_TEXTURE_L8, PX_TEXTURE_A8, PX_TEXTURE_LA4, PX_TEXTURE_L4,
    PX_TEXTURE_A4, PX_TEXTURE_ETC1, PX_TEXTURE_ETC1A4
};
enum px_texture_result {
    PX_TEXTURE_OK = 0, PX_TEXTURE_INVALID_ARGUMENT, PX_TEXTURE_TRUNCATED,
    PX_TEXTURE_UNSUPPORTED, PX_TEXTURE_LIMIT_EXCEEDED, PX_TEXTURE_NO_MEMORY
};
struct px_texture_image {
    uint32_t width, height, format;
    uint8_t *rgba; /* Owned, tightly packed top-down RGBA8. */
};

/* Decode one mip's exact tiled payload; no container headers are synthesized.
 * Stored dimensions must be multiples of eight, cover the positive logical
 * dimensions, and be <=4096. Both dimensions and allocation are bounded.
 * Errors leave a fresh/previously freed output zeroed. */
enum px_texture_result px_texture_decode_payload(const uint8_t *data, size_t size,
    uint32_t width, uint32_t height, uint32_t stored_width, uint32_t stored_height,
    uint32_t format, struct px_texture_image *out);
unsigned px_texture_bits_per_pixel(uint32_t format); /* Zero if unsupported. */
void px_texture_free(struct px_texture_image *image);
const char *px_texture_result_string(enum px_texture_result result);

#endif
