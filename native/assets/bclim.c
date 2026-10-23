#include "bclim.h"

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
static size_t tiled_pixel(uint32_t x, uint32_t y, uint32_t stored_width)
{
    const size_t tile = (size_t)(y / 8) * (stored_width / 8) + x / 8;
    unsigned within = 0;
    for (unsigned bit = 0; bit < 3; ++bit) {
        within |= ((x >> bit) & 1u) << (2 * bit);
        within |= ((y >> bit) & 1u) << (2 * bit + 1);
    }
    return tile * 64 + within;
}
static uint8_t expand5(unsigned value)
{
    return (uint8_t)(value * 8 + value / 4);
}
static uint8_t expand6(unsigned value)
{
    return (uint8_t)(value * 4 + value / 16);
}
static void rgba5551(uint16_t color, uint8_t *pixel)
{
    pixel[0] = expand5(color >> 11);
    pixel[1] = expand5((color >> 6) & 31);
    pixel[2] = expand5((color >> 1) & 31);
    pixel[3] = color & 1 ? 255 : 0;
}
static void raw_pixel(const uint8_t *data, size_t index, uint32_t format,
                      uint8_t *pixel)
{
    unsigned value;
    pixel[3] = 255;
    switch (format) {
    case 0: /* L8 */
        pixel[0] = pixel[1] = pixel[2] = data[index];
        break;
    case 1: /* A8: missing color channels are zero. */
        pixel[0] = pixel[1] = pixel[2] = 0;
        pixel[3] = data[index];
        break;
    case 2: /* LA4 */
        pixel[0] = pixel[1] = pixel[2] = (uint8_t)((data[index] >> 4) * 17);
        pixel[3] = (uint8_t)((data[index] & 15) * 17);
        break;
    case 3: /* LA8 */
        pixel[0] = pixel[1] = pixel[2] = data[2 * index + 1];
        pixel[3] = data[2 * index];
        break;
    case 4: /* HILO8 / RG8 */
        pixel[0] = data[2 * index + 1];
        pixel[1] = data[2 * index];
        pixel[2] = 0;
        break;
    case 5: /* RGB565 */
        value = u16(data + 2 * index);
        pixel[0] = expand5(value >> 11);
        pixel[1] = expand6((value >> 5) & 63);
        pixel[2] = expand5(value & 31);
        break;
    case 6: /* RGB8: bytes B,G,R */
        pixel[0] = data[3 * index + 2];
        pixel[1] = data[3 * index + 1];
        pixel[2] = data[3 * index];
        break;
    case 7:
        rgba5551(u16(data + 2 * index), pixel);
        break;
    case 8: /* RGBA4 */
        value = u16(data + 2 * index);
        for (unsigned channel = 0; channel < 4; ++channel)
            pixel[channel] = (uint8_t)(((value >> (12 - 4 * channel)) & 15) * 17);
        break;
    case 9: /* RGBA8: bytes A,B,G,R */
        for (unsigned channel = 0; channel < 4; ++channel)
            pixel[channel] = data[4 * index + 3 - channel];
        break;
    case 12: /* L4: low nibble comes first, unlike XY palettes. */
        value = (data[index / 2] >> (4 * (index & 1))) & 15;
        pixel[0] = pixel[1] = pixel[2] = (uint8_t)(value * 17);
        break;
    case 13: /* A4 */
        value = (data[index / 2] >> (4 * (index & 1))) & 15;
        pixel[0] = pixel[1] = pixel[2] = 0;
        pixel[3] = (uint8_t)(value * 17);
        break;
    }
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
    if (format > 13 || format == 10 || format == 11) return PX_BCLIM_UNSUPPORTED;
    static const uint8_t bits[14] = {8,8,8,16,16,16,24,16,16,32,0,0,4,4};

    uint32_t stored_width = bound_dimension(width);
    uint32_t stored_height = bound_dimension(height);
    size_t expected = (size_t)stored_width * stored_height * bits[format] / 8;
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

    const size_t output_size = (size_t)width * height * 4;
    uint8_t *rgba = malloc(output_size);
    if (rgba == NULL) return PX_BCLIM_NO_MEMORY;
    for (uint32_t y = 0; y < height; ++y)
        for (uint32_t x = 0; x < width; ++x)
            raw_pixel(data, tiled_pixel(x, y, stored_width), format,
                       rgba + ((size_t)y * width + x) * 4);
    out->width = width;
    out->height = height;
    out->format = format;
    out->rgba = rgba;
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
