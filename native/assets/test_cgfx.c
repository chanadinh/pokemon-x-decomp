#include "cgfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { \
    fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #c); \
    exit(EXIT_FAILURE); \
} } while (0)

enum { DICTIONARY = 0x480, OBJECT0 = 0x200, OBJECT1 = 0x300,
       IMAGE0 = 0x180, IMAGE1 = 0x380, NAME0 = 0xc0, NAME1 = 0xd0,
       IMAG = 0x600, PAYLOAD = IMAG + 8, CAPACITY = 4096 };

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

static void relative(uint8_t *data, unsigned field, unsigned target)
{
    /* Unsigned subtraction specifies the exact on-disk two's-complement
     * displacement, including backward pointers, without signed overflow. */
    put32(data + field, (uint32_t)target - (uint32_t)field);
}

static const struct {
    unsigned format, bits;
    uint8_t encoded[4], expected[4];
} formats[] = {
    {0,32,{0x12,0x34,0x56,0x78},{120,86,52,18}},
    {1,24,{0x12,0x34,0x56},{86,52,18,255}},
    {2,16,{0x4b,0x21},{33,41,41,255}},
    {3,16,{0x85,0x24},{33,146,41,255}},
    {4,16,{0x34,0x12},{17,34,51,68}},
    {5,16,{0x12,0x34},{52,52,52,18}},
    {6,16,{0x12,0x34},{52,18,0,255}},
    {7,8,{0x81},{129,129,129,255}},
    {8,8,{0x81},{0,0,0,129}},
    {9,8,{0xa3},{170,170,170,51}},
    {10,4,{0xa3},{51,51,51,255}},
    {11,4,{0xa3},{0,0,0,51}},
    {12,4,{0},{2,2,2,255}},
    {13,8,{0},{2,2,2,0}}
};

static void texture(uint8_t *data, unsigned object, unsigned image,
                    unsigned name, unsigned payload, unsigned format,
                    unsigned bits)
{
    put32(data + object, UINT32_C(0x20000011));
    memcpy(data + object + 4, "TXOB", 4);
    put32(data + object + 8, UINT32_C(0x05000000));
    relative(data, object + 12, name);
    put32(data + object + 0x18, 8);
    put32(data + object + 0x1c, 8);
    put32(data + object + 0x28, 1);
    put32(data + object + 0x34, format);
    relative(data, object + 0x38, image);
    put32(data + image, 8);
    put32(data + image + 4, 8);
    put32(data + image + 8, 64 * bits / 8);
    relative(data, image + 12, payload);
    put32(data + image + 20, bits);
}

static size_t fixture(uint8_t data[CAPACITY], unsigned format, unsigned bits)
{
    const unsigned payload_size = 64 * bits / 8;
    const unsigned size = PAYLOAD + payload_size + 64;
    memset(data, 0, CAPACITY);
    memcpy(data, "CGFX\xff\xfe\x14\0", 8);
    put32(data + 8, UINT32_C(0x05000000));
    put32(data + 12, size);
    put32(data + 16, 2);
    memcpy(data + 20, "DATA", 4);
    put32(data + 24, IMAG - 20);
    put32(data + 36, 2); /* DATA resource slot 1 is the texture dictionary. */
    relative(data, 40, DICTIONARY);
    memcpy(data + DICTIONARY, "DICT", 4);
    put32(data + DICTIONARY + 4, 60);
    put32(data + DICTIONARY + 8, 2);
    relative(data, DICTIONARY + 28 + 8, NAME0);
    relative(data, DICTIONARY + 28 + 12, OBJECT0);
    relative(data, DICTIONARY + 44 + 8, NAME1);
    relative(data, DICTIONARY + 44 + 12, OBJECT1);
    memcpy(data + NAME0, "first", 6);
    memcpy(data + NAME1, "second", 7);
    texture(data, OBJECT0, IMAGE0, NAME0, PAYLOAD, format, bits);
    texture(data, OBJECT1, IMAGE1, NAME1, PAYLOAD + payload_size, 7, 8);
    memset(data + PAYLOAD + payload_size, 0x49, 64);
    memcpy(data + IMAG, "IMAG", 4);
    put32(data + IMAG + 4, size - IMAG);
    return size;
}

static void pixel(const struct px_texture_image *image, unsigned x, unsigned y,
                   const uint8_t expected[4])
{
    CHECK(image->rgba != NULL && x < image->width && y < image->height);
    CHECK(memcmp(image->rgba + ((size_t)y * image->width + x) * 4, expected, 4) == 0);
}

static void test_formats_and_selection(void)
{
    uint8_t data[CAPACITY];
    for (unsigned f = 0; f < sizeof formats / sizeof formats[0]; ++f) {
        const size_t size = fixture(data, formats[f].format, formats[f].bits);
        memcpy(data + PAYLOAD, formats[f].encoded, sizeof formats[f].encoded);
        /* A dictionary key and the object's own name are distinct fields. */
        memcpy(data + 0xe0, "object-name", 12);
        relative(data, OBJECT0 + 12, 0xe0);
        struct px_cgfx *cgfx = NULL;
        struct px_texture_image image = {0};
        CHECK(px_cgfx_open(data, size, &cgfx) == PX_CGFX_OK);
        CHECK(cgfx != NULL && px_cgfx_texture_count(cgfx) == 2);
        const struct px_cgfx_texture *texture0 = px_cgfx_texture(cgfx, 0);
        const struct px_cgfx_texture *texture1 = px_cgfx_texture(cgfx, 1);
        CHECK(texture0 != NULL && texture1 != NULL);
        CHECK(texture0->name_size == 11 && strcmp(texture0->name, "object-name") == 0);
        CHECK(texture0->dictionary_name_size == 5 && strcmp(texture0->dictionary_name, "first") == 0);
        CHECK(texture0->name == (const char *)data + 0xe0);
        CHECK(texture0->dictionary_name == (const char *)data + NAME0);
        CHECK(texture0->object_offset == OBJECT0 && texture0->image_offset == IMAGE0);
        CHECK(texture0->flags == UINT32_C(0x20000011) && texture0->revision == UINT32_C(0x05000000));
        CHECK(texture0->width == 8 && texture0->height == 8);
        CHECK(texture0->stored_width == 8 && texture0->stored_height == 8);
        CHECK(texture0->format == formats[f].format && texture0->bits_per_pixel == formats[f].bits);
        CHECK(texture0->mipmap_levels == 1 && texture0->data_offset == PAYLOAD);
        CHECK(texture0->data_size == 64 * formats[f].bits / 8);
        CHECK(texture0->decode_status == PX_CGFX_OK);
        CHECK(strcmp(texture1->name, "second") == 0 && texture1->image_offset == IMAGE1);
        CHECK(px_cgfx_texture(cgfx, 2) == NULL && px_cgfx_texture(cgfx, SIZE_MAX) == NULL);
        CHECK(px_cgfx_decode_texture(cgfx, 0, &image) == PX_CGFX_OK);
        CHECK(image.width == 8 && image.height == 8 && image.format == formats[f].format);
        pixel(&image, 0, 0, formats[f].expected);
        if (f == 10 || f == 11) {
            const uint8_t high[4] = {f == 10 ? 170 : 0, f == 10 ? 170 : 0,
                                     f == 10 ? 170 : 0, f == 10 ? 255 : 170};
            pixel(&image, 1, 0, high);
        }
        px_texture_free(&image);
        CHECK(image.rgba == NULL && image.width == 0 && image.height == 0 && image.format == 0);
        CHECK(px_cgfx_decode_texture(cgfx, 1, &image) == PX_CGFX_OK);
        const uint8_t second[4] = {73,73,73,255};
        for (unsigned y = 0; y < 8; ++y)
            for (unsigned x = 0; x < 8; ++x) pixel(&image, x, y, second);
        px_texture_free(&image);
        image.width = 123;
        CHECK(px_cgfx_decode_texture(cgfx, 2, &image) == PX_CGFX_NOT_FOUND);
        CHECK(image.rgba == NULL && image.width == 0 && image.height == 0 && image.format == 0);
        CHECK(px_cgfx_decode_texture(cgfx, 0, NULL) == PX_CGFX_INVALID_ARGUMENT);
        px_cgfx_close(cgfx);
    }
    /* RGBA5551's literal first pixel 2 is valid here: BCLIM's separate XY
     * palette marker policy must not leak into shared PICA/CGFX decoding. */
    const size_t size = fixture(data, 2, 16);
    put16(data + PAYLOAD, 2);
    struct px_cgfx *cgfx = NULL;
    struct px_texture_image image = {0};
    CHECK(px_cgfx_open(data, size, &cgfx) == PX_CGFX_OK);
    CHECK(px_cgfx_decode_texture(cgfx, 0, &image) == PX_CGFX_OK);
    const uint8_t low_blue[4] = {0,0,8,0};
    pixel(&image, 0, 0, low_blue);
    px_texture_free(&image);
    px_cgfx_close(cgfx);
}

static void test_crop_and_empty(void)
{
    static const uint8_t expected[3][5] = {
        {0,1,4,5,16}, {2,3,6,7,18}, {8,9,12,13,24}
    };
    uint8_t data[CAPACITY];
    size_t size = fixture(data, 7, 8);
    put32(data + OBJECT0 + 0x18, 3);
    put32(data + OBJECT0 + 0x1c, 5);
    for (unsigned p = 0; p < 64; ++p) data[PAYLOAD + p] = (uint8_t)p;
    struct px_cgfx *cgfx = NULL;
    struct px_texture_image image = {0};
    CHECK(px_cgfx_open(data, size, &cgfx) == PX_CGFX_OK);
    CHECK(px_cgfx_decode_texture(cgfx, 0, &image) == PX_CGFX_OK);
    CHECK(image.width == 5 && image.height == 3);
    for (unsigned y = 0; y < 3; ++y)
        for (unsigned x = 0; x < 5; ++x) {
            const uint8_t v = expected[y][x];
            const uint8_t rgba[4] = {v,v,v,255};
            pixel(&image, x, y, rgba);
        }
    px_texture_free(&image);
    px_cgfx_close(cgfx);
    /* A valid DATA-only container need not contain any textures or IMAG. */
    fixture(data, 7, 8);
    put32(data + 36, 0); put32(data + 40, 0);
    put32(data + 16, 1); put32(data + 12, IMAG);
    CHECK(px_cgfx_open(data, IMAG, &cgfx) == PX_CGFX_OK);
    CHECK(px_cgfx_texture_count(cgfx) == 0 && px_cgfx_texture(cgfx, 0) == NULL);
    CHECK(px_cgfx_decode_texture(cgfx, 0, &image) == PX_CGFX_NOT_FOUND);
    px_cgfx_close(cgfx);
    /* Names are bounded byte strings; do not require UTF-8 or printability. */
    size = fixture(data, 7, 8);
    data[NAME0] = 0xff; data[NAME0 + 1] = '\n'; data[NAME0 + 2] = 0;
    CHECK(px_cgfx_open(data, size, &cgfx) == PX_CGFX_OK);
    CHECK(px_cgfx_texture(cgfx, 0)->name_size == 2);
    CHECK((uint8_t)px_cgfx_texture(cgfx, 0)->name[0] == 0xff);
    px_cgfx_close(cgfx);
}

static void bad_open(const uint8_t *data, size_t size)
{
    struct px_cgfx *cgfx = (struct px_cgfx *)(uintptr_t)1;
    CHECK(px_cgfx_open(data, size, &cgfx) != PX_CGFX_OK);
    CHECK(cgfx == NULL);
}

static void test_name_limit(void)
{
    uint8_t data[CAPACITY];
    fixture(data, 7, 8);
    const unsigned imag = 0xc00, payload = imag + 8, name = 0x700;
    const unsigned size = payload + 128;
    memcpy(data + payload, data + PAYLOAD, 128);
    memcpy(data + imag, "IMAG", 4);
    put32(data + imag + 4, 136);
    put32(data + 12, size); put32(data + 24, imag - 20);
    relative(data, IMAGE0 + 12, payload);
    relative(data, IMAGE1 + 12, payload + 64);
    relative(data, DICTIONARY + 36, name);
    relative(data, OBJECT0 + 12, name);
    memset(data + name, 'a', 1024); data[name + 1024] = 0;
    struct px_cgfx *cgfx = NULL;
    CHECK(px_cgfx_open(data, size, &cgfx) == PX_CGFX_OK);
    CHECK(px_cgfx_texture(cgfx, 0)->name_size == 1024);
    CHECK(px_cgfx_texture(cgfx, 0)->dictionary_name_size == 1024);
    px_cgfx_close(cgfx);
    data[name + 1024] = 'b'; data[name + 1025] = 0;
    bad_open(data, size);
}

static void unsupported_texture(uint8_t *data, size_t size)
{
    struct px_cgfx *cgfx = NULL;
    struct px_texture_image image = {91,92,93,NULL};
    CHECK(px_cgfx_open(data, size, &cgfx) == PX_CGFX_OK);
    CHECK(px_cgfx_texture_count(cgfx) == 2);
    CHECK(px_cgfx_texture(cgfx, 0)->decode_status != PX_CGFX_OK);
    CHECK(px_cgfx_decode_texture(cgfx, 0, &image) != PX_CGFX_OK);
    CHECK(image.rgba == NULL && image.width == 0 && image.height == 0 && image.format == 0);
    /* Unsupported selection must not prevent a different supported texture. */
    CHECK(px_cgfx_decode_texture(cgfx, 1, &image) == PX_CGFX_OK);
    px_texture_free(&image);
    px_cgfx_close(cgfx);
}

static void test_unsupported_layouts(void)
{
    uint8_t data[CAPACITY];
    static const struct { unsigned offset; uint32_t value; } changes[] = {
        {OBJECT0,UINT32_C(0x20000004)}, {OBJECT0,UINT32_C(0x20000009)},
        {OBJECT0 + 8,UINT32_C(0x06000000)}, {OBJECT0 + 0x34,14},
        {OBJECT0 + 0x28,0}, {OBJECT0 + 0x28,2},
        {OBJECT0 + 0x18,0}, {OBJECT0 + 0x1c,4097},
        {IMAGE0,4}, {IMAGE0 + 4,24}, {IMAGE0 + 20,16},
        {IMAGE0 + 8,63}, {IMAGE0 + 8,65},
        {OBJECT0 + 0x2c,1}, {OBJECT0 + 0x30,1},
        {IMAGE0 + 0x10,1}, {IMAGE0 + 0x18,1}, {IMAGE0 + 0x1c,1}
    };
    for (unsigned i = 0; i < sizeof changes / sizeof changes[0]; ++i) {
        const size_t size = fixture(data, 7, 8);
        put32(data + changes[i].offset, changes[i].value);
        unsupported_texture(data, size);
    }
    /* Unknown variants are listable without interpreting their descriptor. */
    size_t size = fixture(data, 7, 8);
    put32(data + OBJECT0, UINT32_C(0x20000004));
    put32(data + OBJECT0 + 0x38, UINT32_C(0x80000000));
    unsupported_texture(data, size);
    /* Valid metadata can still contain unsupported ETC2/invalid ETC1 bits. */
    size = fixture(data, 12, 4);
    put32(data + PAYLOAD + 4, UINT32_C(0x07000002));
    struct px_cgfx *cgfx = NULL;
    struct px_texture_image image = {0};
    CHECK(px_cgfx_open(data, size, &cgfx) == PX_CGFX_OK);
    CHECK(px_cgfx_texture(cgfx, 0)->decode_status == PX_CGFX_OK);
    CHECK(px_cgfx_decode_texture(cgfx, 0, &image) == PX_CGFX_UNSUPPORTED);
    CHECK(image.rgba == NULL && image.width == 0);
    px_cgfx_close(cgfx);
}

static void test_malformed(void)
{
    uint8_t data[CAPACITY];
    size_t size = fixture(data, 7, 8);
    CHECK(px_cgfx_open(data, size, NULL) == PX_CGFX_INVALID_ARGUMENT);
    bad_open(NULL, size);
    for (size_t n = 0; n < size; ++n) bad_open(data, n);
    bad_open(data, size + 1);
    bad_open(data, 64u * 1024u * 1024u + 1u);
    static const struct { unsigned offset; uint32_t value; } changes[] = {
        {0,0}, {4,UINT32_C(0x0014fffe)}, {8,0}, {12,0}, {16,0}, {16,3},
        {20,0}, {24,7}, {24,UINT32_MAX}, {24,128},
        {IMAG,0}, {IMAG + 4,7}, {IMAG + 4,UINT32_MAX},
        {36,65537}, {40,0}, {40,UINT32_C(0x80000000)},
        {DICTIONARY,0}, {DICTIONARY + 4,59}, {DICTIONARY + 8,1},
        {DICTIONARY + 36,0}, {DICTIONARY + 40,0},
        {OBJECT0 + 4,0}, {OBJECT0 + 12,0}, {OBJECT0 + 0x38,0},
        {IMAGE0 + 12,0}, {IMAGE0 + 12,UINT32_C(0x7fffffff)},
        {IMAGE0 + 8,UINT32_MAX}
    };
    for (unsigned i = 0; i < sizeof changes / sizeof changes[0]; ++i) {
        size = fixture(data, 7, 8);
        put32(data + changes[i].offset, changes[i].value);
        bad_open(data, size);
    }
    size = fixture(data, 7, 8);
    relative(data, IMAGE0 + 12, IMAG); /* Image bytes cannot alias block header. */
    bad_open(data, size);
    size = fixture(data, 7, 8);
    relative(data, IMAGE0 + 12, PAYLOAD + 65); /* Extent exceeds IMAG by one. */
    bad_open(data, size);
    size = fixture(data, 7, 8);
    relative(data, DICTIONARY + 40, IMAG - 7); /* Eight-byte object prefix truncated. */
    bad_open(data, size);
    size = fixture(data, 7, 8);
    relative(data, OBJECT0 + 0x38, IMAG - 31); /* Descriptor crosses DATA end. */
    bad_open(data, size);
    size = fixture(data, 7, 8);
    relative(data, OBJECT0 + 12, IMAG - 1); data[IMAG - 1] = 'x';
    bad_open(data, size);
    size = fixture(data, 7, 8);
    relative(data, DICTIONARY + 36, IMAG - 1); data[IMAG - 1] = 'x';
    bad_open(data, size);
    size = fixture(data, 7, 8);
    /* Validate unrelated resource dictionaries, too; their malformed names
     * must not escape validation merely because only textures are decoded. */
    put32(data + 28, 1); relative(data, 32, 0x500);
    memcpy(data + 0x500, "DICT", 4); put32(data + 0x504, 44); put32(data + 0x508, 1);
    relative(data, 0x528, OBJECT1); relative(data, 0x524, IMAG - 1); data[IMAG - 1] = 'x';
    bad_open(data, size);
    CHECK(px_cgfx_texture_count(NULL) == 0 && px_cgfx_texture(NULL, 0) == NULL);
    struct px_texture_image image = {1,2,3,NULL};
    CHECK(px_cgfx_decode_texture(NULL, 0, &image) == PX_CGFX_INVALID_ARGUMENT);
    CHECK(image.rgba == NULL && image.width == 0 && image.height == 0 && image.format == 0);
    px_cgfx_close(NULL);
    CHECK(strcmp(px_cgfx_result_string(PX_CGFX_OK), "success") == 0);
}

int main(void)
{
    test_formats_and_selection();
    test_crop_and_empty();
    test_name_limit();
    test_unsupported_layouts();
    test_malformed();
    puts("PASS: CGFX 14 PICA formats, borrowed names, backward pointers, selection, crop, layout rejection and bounded malformed cleanup");
    return 0;
}
