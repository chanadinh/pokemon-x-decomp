#ifndef PX_BCLIM_H
#define PX_BCLIM_H

#include <stddef.h>
#include <stdint.h>

enum px_bclim_result {
    PX_BCLIM_OK = 0,
    PX_BCLIM_INVALID_ARGUMENT,
    PX_BCLIM_TRUNCATED,
    PX_BCLIM_INVALID_HEADER,
    PX_BCLIM_UNSUPPORTED,
    PX_BCLIM_LIMIT_EXCEEDED,
    PX_BCLIM_NO_MEMORY
};

struct px_bclim_image {
    uint32_t width;
    uint32_t height;
    uint32_t format;
    /* Owned, tightly packed RGBA8. Row zero is the top of the image. */
    uint8_t *rgba;
};

/* Decode one complete, decompressed BCLIM file. The output must be fresh or
 * previously freed. All errors leave it zeroed. Dimensions are limited to
 * 4096 per side, and unknown format/layout variants fail explicitly. */
enum px_bclim_result px_bclim_decode(const uint8_t *data, size_t size,
                                    struct px_bclim_image *out);
void px_bclim_free(struct px_bclim_image *image);
const char *px_bclim_result_string(enum px_bclim_result result);

#endif
