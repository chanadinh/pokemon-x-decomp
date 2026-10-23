#include "bch_texture.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Independently authored one-texture BCH. Offsets deliberately put the first
 * name and payload at relative zero, where a pointer must still be relocated. */
enum { CONTENT = 56, STRING = 312, COMMAND = 320, RAW = 416, RELOC = 480 };
struct fixture { uint8_t data[1024]; size_t size; };
static void word(uint8_t *p, uint32_t n)
{
    for (unsigned i = 0; i < 4; ++i) p[i] = (uint8_t)(n >> (8 * i));
}
static void ptr(struct fixture *f, size_t at, size_t target, unsigned tag)
{
    size_t target_base = tag == 1 ? STRING : tag == 2 ? COMMAND : tag == 0x24 ? RAW : CONTENT;
    size_t source_base = tag == 0x24 ? COMMAND : CONTENT;
    word(f->data + at, (uint32_t)(target - target_base));
    word(f->data + f->size, (uint32_t)(tag << 25) |
         (uint32_t)((at - source_base) / (tag == 1 ? 1 : 4)));
    f->size += 4;
}
static struct fixture fixture(int alpha)
{
    struct fixture f = {{0}, RELOC};
    memcpy(f.data, "BCH\0\7\7", 6);
    const uint32_t starts[] = {CONTENT, STRING, COMMAND, RAW, RELOC};
    const uint32_t sizes[] = {256, 8, 96, 64};
    for (unsigned i = 0; i < 5; ++i) word(f.data + 8 + 4 * i, starts[i]);
    for (unsigned i = 0; i < 4; ++i) word(f.data + 28 + 4 * i, sizes[i]);
    ptr(&f, 92, 236, 0); word(f.data + 96, 1); ptr(&f, 100, 240, 0);
    ptr(&f, 236, 272, 0); ptr(&f, 260, STRING, 1); ptr(&f, 300, STRING, 1);
    memcpy(f.data + STRING, "sample", 7);
    word(f.data + 296, alpha ? 0x10d : 0x10c);
    const unsigned dims[] = {0x82, 0x92, 0x9a};
    const unsigned addresses[] = {0x85, 0x95, 0x9d};
    const unsigned formats[] = {0x8e, 0x96, 0x9e};
    for (unsigned unit = 0; unit < 3; ++unit) {
        size_t cmd = COMMAND + unit * 32;
        ptr(&f, 272 + unit * 8, cmd, 2); word(f.data + 276 + unit * 8, 8);
        word(f.data + cmd, 0x80008); word(f.data + cmd + 4, 0xf0000 | dims[unit]);
        word(f.data + cmd + 12, 0xf0000 | addresses[unit]); ptr(&f, cmd + 8, RAW, 0x24);
        word(f.data + cmd + 16, alpha ? 13 : 12); word(f.data + cmd + 20, 0xf0000 | formats[unit]);
        word(f.data + cmd + 24, 1); word(f.data + cmd + 28, 0xf023d);
    }
    for (unsigned block = 0; block < 4; ++block) {
        size_t at = RAW + block * (alpha ? 16 : 8);
        if (alpha) {
            const uint8_t a[] = {0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe};
            memcpy(f.data + at, a, 8); at += 8;
        }
        /* ETC individual endpoints 8,4,2; selector0/table0 adds2. */
        word(f.data + at + 4, 0x88442200);
    }
    word(f.data + 44, (uint32_t)(f.size - RELOC));
    return f;
}
static void reject(struct fixture f, size_t at, uint32_t value)
{
    struct px_bch *bch = (struct px_bch *)1;
    word(f.data + at, value);
    assert(px_bch_open(f.data, f.size, &bch) != PX_BCH_OK);
    assert(bch == NULL);
}
static void test_pixels_and_ownership(void)
{
    for (int alpha = 0; alpha <= 1; ++alpha) {
        struct fixture f = fixture(alpha), before = f;
        struct px_bch *bch = NULL;
        struct px_texture_image image = {0};
        assert(px_bch_open(f.data, f.size, &bch) == PX_BCH_OK);
        assert(px_bch_texture_count(bch) == 1);
        const struct px_bch_texture *info = px_bch_texture(bch, 0);
        assert(info && !strcmp(info->name, "sample") && info->name_size == 6);
        assert(info->data_offset == RAW && info->data_size == (unsigned)(alpha ? 64 : 32));
        assert(info->width == 8 && info->height == 8 && info->decode_status == PX_BCH_OK);
        assert(px_bch_texture(bch, 1) == NULL);
        assert(px_bch_decode_texture(bch, 1, &image) == PX_BCH_NOT_FOUND && !image.rgba);
        assert(px_bch_decode_texture(bch, 0, &image) == PX_BCH_OK);
        assert(image.width == 8 && image.height == 8);
        for (unsigned y = 0; y < 8; ++y) for (unsigned x = 0; x < 8; ++x) {
            const uint8_t *pixel = image.rgba + (y * 8 + x) * 4;
            assert(pixel[0] == 138 && pixel[1] == 70 && pixel[2] == 36);
            assert(pixel[3] == (alpha ? ((x % 4) * 4 + y % 4) * 17 : 255));
        }
        px_bch_close(bch); /* Pixels own their allocation after index close. */
        assert(image.rgba[0] == 138);
        px_texture_free(&image);
        assert(!memcmp(f.data, before.data, sizeof f.data));
    }
}
static void test_invalid(void)
{
    struct fixture f = fixture(1);
    for (size_t size = 0; size < f.size; ++size) {
        struct px_bch *bch = NULL;
        assert(px_bch_open(f.data, size, &bch) != PX_BCH_OK && !bch);
    }
    const uint32_t cases[][2] = {
        {8, 0}, {12, 56}, {28, UINT32_MAX}, {44, 3}, {48, 1}, {96, 65536},
        {92, 256}, {100, 256}, {236, 255}, {260, 8}, {300, 8},
        {256, 2}, {272, 96}, {276, 4098}, {COMMAND + 4, 0x7ff00082},
        {COMMAND + 4, 0xf0400}, {COMMAND + 8, 64},
        {COMMAND + 12, 0xf0086}, {COMMAND + 24, 2},
        {COMMAND + 28, 0xf023c}, {COMMAND + 32, 0x100010},
        {RELOC, 0xfe000000}, {RELOC, 0x1ffffff}, {RELOC + 4, 9},
        /* Byte-addressed string relocation overlaps root pointer by1 byte. */
        {RELOC + 4, 0x02000025},
    };
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
        reject(f, cases[i][0], cases[i][1]);
    memset(f.data + STRING, 'x', 8);
    struct px_bch *bch = NULL;
    assert(px_bch_open(f.data, f.size, &bch) == PX_BCH_INVALID_HEADER && !bch);
    assert(px_bch_open(f.data, 64u * 1024 * 1024 + 1, &bch) == PX_BCH_LIMIT_EXCEEDED);
    assert(px_bch_open(NULL, 0, &bch) == PX_BCH_INVALID_ARGUMENT);
    assert(px_bch_open(f.data, f.size, NULL) == PX_BCH_INVALID_ARGUMENT);
    assert(px_bch_texture_count(NULL) == 0 && px_bch_texture(NULL, 0) == NULL);
    px_bch_close(NULL);
}
static void test_unsupported_is_listable(void)
{
    struct fixture f = fixture(0);
    word(f.data + 296, 0x20c);
    struct px_bch *bch = NULL;
    struct px_texture_image image = {0};
    assert(px_bch_open(f.data, f.size, &bch) == PX_BCH_OK);
    assert(px_bch_texture(bch, 0)->decode_status == PX_BCH_UNSUPPORTED);
    assert(px_bch_decode_texture(bch, 0, &image) == PX_BCH_UNSUPPORTED && !image.rgba);
    px_bch_close(bch);
}
int main(void)
{
    test_pixels_and_ownership(); test_invalid(); test_unsupported_is_listable();
    puts("PASS: BCH texture relocation, section and command bounds; ETC1/A4 pixels; unsupported descriptors; immutable input and owned output");
    return 0;
}
