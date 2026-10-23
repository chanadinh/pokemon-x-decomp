#include "texture.h"
#include "etc1.h"

#include <stdlib.h>
#include <string.h>

/* Shared PICA payload decoding, extracted from the validated BCLIM decoder.
 * Container-specific formats and palette variants are handled by adapters. */
static uint16_t u16(const uint8_t *p)
{
    return (uint16_t)((uint32_t)p[0] | ((uint32_t)p[1] << 8));
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
    case PX_TEXTURE_L8: /* L8 */
        pixel[0] = pixel[1] = pixel[2] = data[index];
        break;
    case PX_TEXTURE_A8: /* A8: missing color channels are zero. */
        pixel[0] = pixel[1] = pixel[2] = 0;
        pixel[3] = data[index];
        break;
    case PX_TEXTURE_LA4: /* LA4 */
        pixel[0] = pixel[1] = pixel[2] = (uint8_t)((data[index] >> 4) * 17);
        pixel[3] = (uint8_t)((data[index] & 15) * 17);
        break;
    case PX_TEXTURE_LA8: /* LA8 */
        pixel[0] = pixel[1] = pixel[2] = data[2 * index + 1];
        pixel[3] = data[2 * index];
        break;
    case PX_TEXTURE_HILO8: /* HILO8 / RG8 */
        pixel[0] = data[2 * index + 1];
        pixel[1] = data[2 * index];
        pixel[2] = 0;
        break;
    case PX_TEXTURE_RGB565: /* RGB565 */
        value = u16(data + 2 * index);
        pixel[0] = expand5(value >> 11);
        pixel[1] = expand6((value >> 5) & 63);
        pixel[2] = expand5(value & 31);
        break;
    case PX_TEXTURE_RGB8: /* RGB8: bytes B,G,R */
        pixel[0] = data[3 * index + 2];
        pixel[1] = data[3 * index + 1];
        pixel[2] = data[3 * index];
        break;
    case PX_TEXTURE_RGBA5551:
        rgba5551(u16(data + 2 * index), pixel);
        break;
    case PX_TEXTURE_RGBA4: /* RGBA4 */
        value = u16(data + 2 * index);
        for (unsigned channel = 0; channel < 4; ++channel)
            pixel[channel] = (uint8_t)(((value >> (12 - 4 * channel)) & 15) * 17);
        break;
    case PX_TEXTURE_RGBA8: /* RGBA8: bytes A,B,G,R */
        for (unsigned channel = 0; channel < 4; ++channel)
            pixel[channel] = data[4 * index + 3 - channel];
        break;
    case PX_TEXTURE_L4: /* L4: low nibble comes first, unlike XY palettes. */
        value = (data[index / 2] >> (4 * (index & 1))) & 15;
        pixel[0] = pixel[1] = pixel[2] = (uint8_t)(value * 17);
        break;
    case PX_TEXTURE_A4: /* A4 */
        value = (data[index / 2] >> (4 * (index & 1))) & 15;
        pixel[0] = pixel[1] = pixel[2] = 0;
        pixel[3] = (uint8_t)(value * 17);
        break;
    }
}

static int compressed_pixels(const uint8_t *data, uint32_t stored_width,
                              uint32_t stored_height, uint32_t width,
                              uint32_t height, int alpha, uint8_t *rgba)
{
    const size_t block_size = alpha ? 16 : 8;
    size_t offset = 0;
    for (uint32_t tile_y = 0; tile_y < stored_height; tile_y += 8) {
        for (uint32_t tile_x = 0; tile_x < stored_width; tile_x += 8) {
            for (unsigned block = 0; block < 4; ++block) {
                uint8_t pixels[64];
                /* Validate padding blocks as well as visible blocks. Exact
                 * payload validation above bounds every block read. */
                if (px_etc1_decode_block(data + offset, block_size, alpha,
                                         pixels, sizeof pixels) != PX_ETC1_OK)
                    return 0;
                offset += block_size;
                const uint32_t origin_x = tile_x + (block % 2) * 4;
                const uint32_t origin_y = tile_y + (block / 2) * 4;
                for (unsigned y = 0; y < 4 && origin_y + y < height; ++y)
                    for (unsigned x = 0; x < 4 && origin_x + x < width; ++x)
                        memcpy(rgba + ((size_t)(origin_y + y) * width + origin_x + x) * 4,
                               pixels + (y * 4 + x) * 4, 4);
            }
        }
    }
    return 1;
}

unsigned px_texture_bits_per_pixel(uint32_t format)
{
    static const uint8_t bits[14] = {32,24,16,16,16,16,16,8,8,8,4,4,4,8};
    return format < 14 ? bits[format] : 0;
}

enum px_texture_result px_texture_decode_payload(const uint8_t *data, size_t size,
    uint32_t width, uint32_t height, uint32_t stored_width, uint32_t stored_height,
    uint32_t format, struct px_texture_image *out)
{
    if (out == NULL) return PX_TEXTURE_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (data == NULL || width == 0 || height == 0)
        return PX_TEXTURE_INVALID_ARGUMENT;
    if (width > 4096 || height > 4096 || stored_width > 4096 || stored_height > 4096)
        return PX_TEXTURE_LIMIT_EXCEEDED;
    if (stored_width < width || stored_height < height ||
        stored_width % 8 != 0 || stored_height % 8 != 0)
        return PX_TEXTURE_INVALID_ARGUMENT;
    const unsigned bits = px_texture_bits_per_pixel(format);
    if (bits == 0) return PX_TEXTURE_UNSUPPORTED;
    const size_t expected = (size_t)stored_width * stored_height * bits / 8;
    if (size < expected) return PX_TEXTURE_TRUNCATED;
    if (size != expected) return PX_TEXTURE_UNSUPPORTED;
    uint8_t *rgba = malloc((size_t)width * height * 4);
    if (rgba == NULL) return PX_TEXTURE_NO_MEMORY;
    if (format == PX_TEXTURE_ETC1 || format == PX_TEXTURE_ETC1A4) {
        if (!compressed_pixels(data, stored_width, stored_height, width, height,
                               format == PX_TEXTURE_ETC1A4, rgba)) {
            free(rgba);
            return PX_TEXTURE_UNSUPPORTED;
        }
    } else {
        for (uint32_t y = 0; y < height; ++y)
            for (uint32_t x = 0; x < width; ++x)
                raw_pixel(data, tiled_pixel(x, y, stored_width), format,
                           rgba + ((size_t)y * width + x) * 4);
    }
    out->width = width;
    out->height = height;
    out->format = format;
    out->rgba = rgba;
    return PX_TEXTURE_OK;
}

void px_texture_free(struct px_texture_image *image)
{
    if (image == NULL) return;
    free(image->rgba);
    memset(image, 0, sizeof *image);
}
const char *px_texture_result_string(enum px_texture_result result)
{
    switch (result) {
    case PX_TEXTURE_OK: return "success";
    case PX_TEXTURE_INVALID_ARGUMENT: return "invalid texture dimensions or argument";
    case PX_TEXTURE_TRUNCATED: return "truncated texture payload";
    case PX_TEXTURE_UNSUPPORTED: return "unsupported texture format or layout";
    case PX_TEXTURE_LIMIT_EXCEEDED: return "texture dimension limit exceeded";
    case PX_TEXTURE_NO_MEMORY: return "cannot allocate decoded texture";
    }
    return "unknown texture result";
}
