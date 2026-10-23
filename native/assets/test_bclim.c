#include "bclim.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { \
    fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #c); \
    exit(EXIT_FAILURE); \
} } while (0)

static void put16(uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
}
static void put32(uint8_t *p, uint32_t value)
{
    put16(p, (uint16_t)value);
    put16(p + 2, (uint16_t)(value >> 16));
}
static size_t footer(uint8_t *data, size_t payload, uint16_t width,
                     uint16_t height, uint32_t format)
{
    uint8_t *p = data + payload;
    memset(p, 0, 40);
    memcpy(p, "CLIM", 4);
    put16(p + 4, UINT16_C(0xfeff));
    put16(p + 6, 20);
    put32(p + 8, UINT32_C(0x02020000));
    put32(p + 12, (uint32_t)payload + 40);
    put16(p + 16, 1);
    memcpy(p + 20, "imag", 4);
    put32(p + 24, 16);
    put16(p + 28, width);
    put16(p + 30, height);
    put32(p + 32, format);
    put32(p + 36, (uint32_t)payload);
    return payload + 40;
}
static void pixel(const struct px_bclim_image *image, unsigned x, unsigned y,
                   const uint8_t expected[4])
{
    CHECK(x < image->width && y < image->height);
    CHECK(memcmp(image->rgba + ((size_t)y * image->width + x) * 4, expected, 4) == 0);
}
static void test_colors(void)
{
    static const struct {
        uint32_t format;
        unsigned bits;
        uint8_t encoded[4], expected[4];
    } cases[] = {
        {0, 8, {0x81}, {129,129,129,255}},
        {1, 8, {0x81}, {0,0,0,129}},
        {2, 8, {0xa3}, {170,170,170,51}},
        {3,16, {0x12,0x34}, {52,52,52,18}},
        {4,16, {0x12,0x34}, {52,18,0,255}},
        {5,16, {0x85,0x24}, {33,146,41,255}},
        {6,24, {0x12,0x34,0x56}, {86,52,18,255}},
        {7,16, {0x4b,0x21}, {33,41,41,255}},
        {8,16, {0x34,0x12}, {17,34,51,68}},
        {9,32, {0x12,0x34,0x56,0x78}, {120,86,52,18}},
        {12,4, {0xa3}, {51,51,51,255}},
        {13,4, {0xa3}, {0,0,0,51}}
    };
    uint8_t data[256 + 40];
    struct px_bclim_image image = {0};
    for (unsigned i = 0; i < sizeof cases / sizeof cases[0]; ++i) {
        memset(data, 0, sizeof data);
        memcpy(data, cases[i].encoded, sizeof cases[i].encoded);
        const size_t size = footer(data, 64 * cases[i].bits / 8, 8, 8, cases[i].format);
        CHECK(px_bclim_decode(data, size, &image) == PX_BCLIM_OK);
        CHECK(image.width == 8 && image.height == 8 && image.format == cases[i].format);
        pixel(&image, 0, 0, cases[i].expected);
        if (cases[i].format == 12) {
            const uint8_t high[4] = {170,170,170,255};
            pixel(&image, 1, 0, high);
        } else if (cases[i].format == 13) {
            const uint8_t high[4] = {0,0,0,170};
            pixel(&image, 1, 0, high);
        }
        px_bclim_free(&image);
        CHECK(image.rgba == NULL && image.width == 0 && image.height == 0 && image.format == 0);
    }
    memset(data, 0xff, 128);
    CHECK(px_bclim_decode(data, footer(data, 128, 8, 8, 5), &image) == PX_BCLIM_OK);
    const uint8_t white[4] = {255,255,255,255};
    pixel(&image, 7, 7, white);
    px_bclim_free(&image);
    put16(data, UINT16_C(0xfffe));
    CHECK(px_bclim_decode(data, footer(data, 128, 8, 8, 7), &image) == PX_BCLIM_OK);
    const uint8_t clear_white[4] = {255,255,255,0};
    pixel(&image, 0, 0, clear_white);
    px_bclim_free(&image);
}

static void test_tiles_and_crop(void)
{
    /* Independent fixed position oracle, intentionally asymmetric in X/Y.
     * Source byte n is luminance n; this literal matrix proves orientation,
     * all six Morton bits, and the absence of a vertical flip or transpose. */
    static const uint8_t expected[8][8] = {
        {0,1,4,5,16,17,20,21}, {2,3,6,7,18,19,22,23},
        {8,9,12,13,24,25,28,29}, {10,11,14,15,26,27,30,31},
        {32,33,36,37,48,49,52,53}, {34,35,38,39,50,51,54,55},
        {40,41,44,45,56,57,60,61}, {42,43,46,47,58,59,62,63}
    };
    uint8_t data[2048 + 40] = {0};
    struct px_bclim_image image = {0};
    for (unsigned i = 0; i < 64; ++i) data[i] = (uint8_t)i;
    CHECK(px_bclim_decode(data, footer(data, 64, 8, 8, 0), &image) == PX_BCLIM_OK);
    for (unsigned y = 0; y < 8; ++y)
        for (unsigned x = 0; x < 8; ++x) {
            const uint8_t rgba[4] = {expected[y][x],expected[y][x],expected[y][x],255};
            pixel(&image, x, y, rgba);
        }
    px_bclim_free(&image);
    CHECK(px_bclim_decode(data, footer(data, 64, 5, 3, 0), &image) == PX_BCLIM_OK);
    CHECK(image.width == 5 && image.height == 3);
    for (unsigned y = 0; y < 3; ++y)
        for (unsigned x = 0; x < 5; ++x) {
            const uint8_t rgba[4] = {expected[y][x],expected[y][x],expected[y][x],255};
            pixel(&image, x, y, rgba);
        }
    px_bclim_free(&image);
    for (unsigned tile = 0; tile < 4; ++tile) memset(data + 64 * tile, 10 + tile * 20, 64);
    CHECK(px_bclim_decode(data, footer(data, 256, 16, 16, 0), &image) == PX_BCLIM_OK);
    for (unsigned y = 0; y < 16; ++y)
        for (unsigned x = 0; x < 16; ++x) {
            const uint8_t value = (uint8_t)(10 + (y >= 8 ? 40 : 0) + (x >= 8 ? 20 : 0));
            const uint8_t rgba[4] = {value,value,value,255};
            pixel(&image, x, y, rgba);
        }
    px_bclim_free(&image);
    /* 17x9 RGBA: 24x16 tile-rounded storage and 32x16 power-of-two storage
     * place the lower-right visible pixel in different source tiles. */
    for (unsigned layout = 0; layout < 2; ++layout) {
        memset(data, 0, sizeof data);
        const unsigned payload = layout ? 2048 : 1536;
        const unsigned source_pixel = layout ? 384 : 320;
        const uint8_t first[4] = {0x11,0x22,0x33,0x44};
        const uint8_t last[4] = {0x55,0x66,0x77,0x88};
        data[0] = 0x44; data[1] = 0x33; data[2] = 0x22; data[3] = 0x11;
        data[4 * source_pixel] = 0x88; data[4 * source_pixel + 1] = 0x77;
        data[4 * source_pixel + 2] = 0x66; data[4 * source_pixel + 3] = 0x55;
        CHECK(px_bclim_decode(data, footer(data, payload, 17, 9, 9), &image) == PX_BCLIM_OK);
        pixel(&image, 0, 0, first);
        pixel(&image, 16, 8, last);
        px_bclim_free(&image);
    }
}

static void error(const uint8_t *data, size_t size, enum px_bclim_result expected)
{
    struct px_bclim_image image = {99,98,97,NULL};
    CHECK(px_bclim_decode(data, size, &image) == expected);
    CHECK(image.rgba == NULL && image.width == 0 && image.height == 0 && image.format == 0);
}
static void test_errors(void)
{
    uint8_t data[256 + 40] = {0};
    const size_t size = footer(data, 256, 8, 8, 9);
    CHECK(px_bclim_decode(data, size, NULL) == PX_BCLIM_INVALID_ARGUMENT);
    error(NULL, size, PX_BCLIM_INVALID_ARGUMENT);
    error(data, 0, PX_BCLIM_TRUNCATED);
    error(data, 39, PX_BCLIM_TRUNCATED);
    if (SIZE_MAX > UINT32_MAX) error(data, SIZE_MAX, PX_BCLIM_LIMIT_EXCEEDED);
    for (size_t n = 40; n < size; ++n) {
        struct px_bclim_image image = {0};
        CHECK(px_bclim_decode(data, n, &image) != PX_BCLIM_OK);
        CHECK(image.rgba == NULL);
    }
    static const struct { unsigned offset; uint8_t value; enum px_bclim_result result; } mutations[] = {
        {0,'X',PX_BCLIM_INVALID_HEADER}, {20,'X',PX_BCLIM_INVALID_HEADER},
        {4,0,PX_BCLIM_UNSUPPORTED}, {6,19,PX_BCLIM_INVALID_HEADER},
        {8,1,PX_BCLIM_UNSUPPORTED}, {12,1,PX_BCLIM_INVALID_HEADER},
        {16,2,PX_BCLIM_INVALID_HEADER}, {18,1,PX_BCLIM_INVALID_HEADER},
        {24,20,PX_BCLIM_INVALID_HEADER}, {28,0,PX_BCLIM_INVALID_HEADER},
        {30,0,PX_BCLIM_INVALID_HEADER}, {32,10,PX_BCLIM_UNSUPPORTED},
        {32,11,PX_BCLIM_UNSUPPORTED}, {32,14,PX_BCLIM_UNSUPPORTED},
        {33,1,PX_BCLIM_UNSUPPORTED}, {36,1,PX_BCLIM_TRUNCATED}
    };
    for (unsigned i = 0; i < sizeof mutations / sizeof mutations[0]; ++i) {
        footer(data, 256, 8, 8, 9);
        data[256 + mutations[i].offset] = mutations[i].value;
        error(data, size, mutations[i].result);
    }
    footer(data, 256, 4097, 8, 9);
    error(data, size, PX_BCLIM_LIMIT_EXCEEDED);
    footer(data, 256, 4096, 4096, 9);
    error(data, size, PX_BCLIM_TRUNCATED);
    footer(data, 256, 8, 8, 0);
    error(data, size, PX_BCLIM_UNSUPPORTED);
    footer(data, 256, 8, 8, 9);
    put32(data + 256 + 36, 255);
    error(data, size, PX_BCLIM_INVALID_HEADER);
    footer(data, 128, 8, 8, 7);
    put16(data, 2);
    error(data, 168, PX_BCLIM_UNSUPPORTED);
    px_bclim_free(NULL);
    CHECK(strcmp(px_bclim_result_string(PX_BCLIM_OK), "success") == 0);
    CHECK(strcmp(px_bclim_result_string((enum px_bclim_result)99), "unknown BCLIM result") == 0);
}
int main(void)
{
    test_colors();
    test_tiles_and_crop();
    test_errors();
    puts("PASS: BCLIM raw formats, fixed Morton/orientation/color vectors, crop/stride variants, bounded malformed-input handling");
    return 0;
}
