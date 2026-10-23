#ifndef PX_ETC1_H
#define PX_ETC1_H

#include <stddef.h>
#include <stdint.h>

enum px_etc1_result {
    PX_ETC1_OK = 0,
    PX_ETC1_INVALID_ARGUMENT,
    PX_ETC1_TRUNCATED,
    PX_ETC1_OUTPUT_TOO_SMALL,
    PX_ETC1_UNSUPPORTED_BLOCK
};

/* Decode one PICA 4x4 block into 64 row-major RGBA8 bytes, top row first.
 * alpha=0 consumes 8 bytes: a little-endian ETC1 color word.
 * alpha=1 consumes 16 bytes: a little-endian alpha word, then the color word.
 * Alpha nibbles use index 4*x+y, with the first nibble least significant.
 * Larger buffers are permitted; only one block and 64 output bytes are used.
 * alpha must be 0 or 1. All errors leave the output unchanged.
 * Differential endpoints outside 0..31 are rejected: ETC2 modes are not
 * supported, and those combinations have undefined results in ETC1. */
enum px_etc1_result px_etc1_decode_block(const uint8_t *data, size_t size,
                                        int alpha, uint8_t *rgba,
                                        size_t rgba_size);

#endif
