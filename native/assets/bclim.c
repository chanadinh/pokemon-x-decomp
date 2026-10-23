#include "bclim.h"
#include "texture.h"

#include <stdlib.h>
#include <string.h>

/* Independent implementation of the CLIM/imag footer and 8x8 Morton layout.
 * Format references are recorded in docs/native-textures.md. */
static uint16_t u16(const uint8_t *p)
{
    return (uint16_t)((uint32_t)p[0] | ((uint32_t)p[1] << 8));
}
static uint32_t u32(const uint8_t *p)
{
    return (uint32_t)u16(p) | ((uint32_t)u16(p + 2) << 16);
}
static uint32_t bound_dimension(uint32_t value)
{
    uint32_t result = 8;
    while (result < value) result *= 2;
    return result;
}

enum px_bclim_result px_bclim_decode(const uint8_t *data, size_t size,
                                    struct px_bclim_image *out)
{
    if (out == NULL) return PX_BCLIM_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (data == NULL) return PX_BCLIM_INVALID_ARGUMENT;
    if (size < 40) return PX_BCLIM_TRUNCATED;
    if (size > UINT32_MAX) return PX_BCLIM_LIMIT_EXCEEDED;
    const uint8_t *footer = data + size - 40;
    if (memcmp(footer, "CLIM", 4) != 0 || memcmp(footer + 20, "imag", 4) != 0)
        return PX_BCLIM_INVALID_HEADER;
    if (u16(footer + 4) != UINT16_C(0xfeff)) return PX_BCLIM_UNSUPPORTED;
    if (u16(footer + 6) != 20 || u32(footer + 12) != size ||
        u16(footer + 16) != 1 || u16(footer + 18) != 0 || u32(footer + 24) != 16)
        return PX_BCLIM_INVALID_HEADER;
    if (u32(footer + 8) != UINT32_C(0x02020000)) return PX_BCLIM_UNSUPPORTED;

    const uint32_t width = u16(footer + 28), height = u16(footer + 30);
    const uint32_t format = u32(footer + 32), payload = u32(footer + 36);
    if (width == 0 || height == 0) return PX_BCLIM_INVALID_HEADER;
    if (width > 4096 || height > 4096) return PX_BCLIM_LIMIT_EXCEEDED;
    if (payload > size - 40) return PX_BCLIM_TRUNCATED;
    if (payload != size - 40) return PX_BCLIM_INVALID_HEADER;
    if (format > 13) return PX_BCLIM_UNSUPPORTED;
    static const uint8_t formats[14] = {
        PX_TEXTURE_L8, PX_TEXTURE_A8, PX_TEXTURE_LA4, PX_TEXTURE_LA8,
        PX_TEXTURE_HILO8, PX_TEXTURE_RGB565, PX_TEXTURE_RGB8, PX_TEXTURE_RGBA5551,
        PX_TEXTURE_RGBA4, PX_TEXTURE_RGBA8, PX_TEXTURE_ETC1, PX_TEXTURE_ETC1A4,
        PX_TEXTURE_L4, PX_TEXTURE_A4
    };

    uint32_t stored_width = bound_dimension(width);
    uint32_t stored_height = bound_dimension(height);
    size_t expected = (size_t)stored_width * stored_height * px_texture_bits_per_pixel(formats[format]) / 8;
    /* Observed RGBA8 files can use tile-rounded, non-power-of-two storage. */
    if (format == 9 && payload != expected) {
        stored_width = (width + 7) & ~UINT32_C(7);
        stored_height = (height + 7) & ~UINT32_C(7);
        expected = (size_t)stored_width * stored_height * 4;
    }
    /* XY's format-7 palette is deliberately detected before raw RGBA5551;
     * its prefix is ambiguous with a raw first pixel of value2. */
    if (format == 7 && payload >= 2 && u16(data) == 2) return PX_BCLIM_UNSUPPORTED;
    if (payload < expected) return PX_BCLIM_TRUNCATED;
    if (payload != expected) return PX_BCLIM_UNSUPPORTED;

    struct px_texture_image image = {0};
    const enum px_texture_result result = px_texture_decode_payload(data, payload,
        width, height, stored_width, stored_height, formats[format], &image);
    switch (result) {
    case PX_TEXTURE_OK: break;
    case PX_TEXTURE_INVALID_ARGUMENT: return PX_BCLIM_INVALID_ARGUMENT;
    case PX_TEXTURE_TRUNCATED: return PX_BCLIM_TRUNCATED;
    case PX_TEXTURE_UNSUPPORTED: return PX_BCLIM_UNSUPPORTED;
    case PX_TEXTURE_LIMIT_EXCEEDED: return PX_BCLIM_LIMIT_EXCEEDED;
    case PX_TEXTURE_NO_MEMORY: return PX_BCLIM_NO_MEMORY;
    }
    out->width = width;
    out->height = height;
    out->format = format;
    out->rgba = image.rgba;
    return PX_BCLIM_OK;
}

void px_bclim_free(struct px_bclim_image *image)
{
    if (image == NULL) return;
    free(image->rgba);
    memset(image, 0, sizeof *image);
}
const char *px_bclim_result_string(enum px_bclim_result result)
{
    switch (result) {
    case PX_BCLIM_OK: return "success";
    case PX_BCLIM_INVALID_ARGUMENT: return "invalid decoder argument";
    case PX_BCLIM_TRUNCATED: return "truncated BCLIM data";
    case PX_BCLIM_INVALID_HEADER: return "invalid BCLIM footer";
    case PX_BCLIM_UNSUPPORTED: return "unsupported BCLIM format or layout";
    case PX_BCLIM_LIMIT_EXCEEDED: return "BCLIM size limit exceeded";
    case PX_BCLIM_NO_MEMORY: return "cannot allocate decoded texture";
    }
    return "unknown BCLIM result";
}
