#include "texture.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { \
    fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #c); \
    exit(EXIT_FAILURE); \
} } while (0)

static void test_explicit_stride(void)
{
    /* Explicit 24x16 storage differs from a 32x16 inferred power of two.
     * The six source tiles have distinct luminance; crop to 17x9. */
    uint8_t data[384];
    for (unsigned tile = 0; tile < 6; ++tile) memset(data + 64 * tile, 10 + 20 * tile, 64);
    struct px_texture_image image = {0};
    CHECK(px_texture_decode_payload(data, sizeof data, 17, 9, 24, 16,
                                     PX_TEXTURE_L8, &image) == PX_TEXTURE_OK);
    CHECK(image.width == 17 && image.height == 9 && image.format == PX_TEXTURE_L8);
    static const uint8_t tile_values[2][3] = {{10,30,50},{70,90,110}};
    for (unsigned y = 0; y < 9; ++y)
        for (unsigned x = 0; x < 17; ++x) {
            const uint8_t value = tile_values[y / 8][x / 8];
            const uint8_t expected[4] = {value,value,value,255};
            CHECK(memcmp(image.rgba + (y * 17 + x) * 4, expected, 4) == 0);
        }
    px_texture_free(&image);
    CHECK(image.rgba == NULL && image.width == 0 && image.height == 0 && image.format == 0);
    /* CGFX RGBA5551 is always raw; BCLIM's XY palette policy is container-specific. */
    memset(data, 0, sizeof data); data[0] = 2;
    CHECK(px_texture_decode_payload(data, 128, 8, 8, 8, 8,
                                     PX_TEXTURE_RGBA5551, &image) == PX_TEXTURE_OK);
    const uint8_t blue[4] = {0,0,8,0};
    CHECK(memcmp(image.rgba, blue, 4) == 0);
    px_texture_free(&image);
}

static void bad(const uint8_t *data, size_t size, uint32_t w, uint32_t h,
                 uint32_t sw, uint32_t sh, uint32_t format)
{
    struct px_texture_image image = {91,92,93,NULL};
    CHECK(px_texture_decode_payload(data, size, w, h, sw, sh, format, &image) != PX_TEXTURE_OK);
    CHECK(image.rgba == NULL && image.width == 0 && image.height == 0 && image.format == 0);
}

static void test_bounds(void)
{
    static const unsigned expected_bits[14] = {32,24,16,16,16,16,16,8,8,8,4,4,4,8};
    uint8_t data[256] = {0};
    for (uint32_t format = 0; format < 14; ++format) {
        CHECK(px_texture_bits_per_pixel(format) == expected_bits[format]);
        const size_t expected = expected_bits[format] * 8;
        for (size_t size = 0; size < expected; ++size) bad(data,size,8,8,8,8,format);
        bad(data,expected+1,8,8,8,8,format);
    }
    CHECK(px_texture_bits_per_pixel(14) == 0 && px_texture_bits_per_pixel(UINT32_MAX) == 0);
    CHECK(px_texture_decode_payload(data,64,8,8,8,8,PX_TEXTURE_L8,NULL) == PX_TEXTURE_INVALID_ARGUMENT);
    bad(NULL,64,8,8,8,8,PX_TEXTURE_L8);
    bad(data,64,8,8,8,8,14);
    bad(data,SIZE_MAX,8,8,8,8,PX_TEXTURE_L8);
    static const uint32_t dimensions[][4] = {
        {0,8,8,8}, {8,0,8,8}, {8,8,0,8}, {8,8,8,0},
        {8,8,7,8}, {8,8,8,7}, {8,8,9,8}, {8,8,8,9},
        {9,8,8,8}, {8,9,8,8}, {4097,8,4096,8}, {8,8,4104,8},
        {8,8,8,4104}, {UINT32_MAX,8,UINT32_MAX,8}
    };
    for (unsigned i = 0; i < sizeof dimensions / sizeof dimensions[0]; ++i)
        bad(data,64,dimensions[i][0],dimensions[i][1],dimensions[i][2],dimensions[i][3],PX_TEXTURE_L8);
    /* Final stored ETC block is cropped out of 1x1 output, but still invalid. */
    memset(data,0,sizeof data); data[3*8+4] = 2; data[3*8+7] = 7;
    bad(data,32,1,1,8,8,PX_TEXTURE_ETC1);
    memset(data,0,sizeof data); data[3*16+8+4] = 2; data[3*16+8+7] = 7;
    bad(data,64,1,1,8,8,PX_TEXTURE_ETC1A4);
    px_texture_free(NULL);
    CHECK(strcmp(px_texture_result_string(PX_TEXTURE_OK), "success") == 0);
}

int main(void)
{
    test_explicit_stride();
    test_bounds();
    puts("PASS: shared PICA payload format widths, explicit non-power-of-two stride, crop, palette separation and bounded cleanup");
    return 0;
}
