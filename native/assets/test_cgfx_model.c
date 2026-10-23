#include "cgfx_model.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define SIZE 8192u
static size_t mutation_offset;

static void put32(uint8_t *p, size_t offset, uint32_t value) {
    unsigned i;
    for (i = 0; i < 4; ++i) p[offset + i] = (uint8_t)(value >> (i * 8));
}

static void put16(uint8_t *p, size_t offset, int value) {
    unsigned raw = (unsigned)value & 0xffffu;
    p[offset] = (uint8_t)raw; p[offset + 1] = (uint8_t)(raw >> 8);
}

static void putf(uint8_t *p, size_t offset, float value) {
    uint32_t raw;
    memcpy(&raw, &value, 4); put32(p, offset, raw);
}

static void pointer(uint8_t *p, size_t field, size_t target) {
    put32(p, field, (uint32_t)(target - field));
}

static void make_shape(uint8_t *p, unsigned which) {
    size_t s = 0x220 + which * 0x60, object = 0x300 + which * 0x40;
    size_t v = 0x400 + which * 0x200, image = 0x1008 + which * 0x100;
    size_t indices = image + 0x80;
    unsigned i;
    pointer(p, 0x200 + which * 4, s);
    pointer(p, 0x208 + which * 4, object);
    put32(p, object, 0x01000000); memcpy(p + object + 4, "SOBJ", 4);
    put32(p, object + 24, which); pointer(p, object + 32, 0x100);
    p[object + 36] = 1;
    put32(p, s, 0x10000001); memcpy(p + s + 4, "SOBJ", 4);
    put32(p, s + 44, 1); pointer(p, s + 48, v + 0x90);
    put32(p, s + 56, which ? 1 : 2); pointer(p, s + 60, v + 0x38);
    pointer(p, s + 64, 0x740); /* Empty, typed blend descriptor. */
    if (which) {
        putf(p, s + 32, 10); putf(p, s + 36, 20); putf(p, s + 40, 30);
    }
    pointer(p, v + 0x38, v);
    if (!which) pointer(p, v + 0x3c, 0x500);
    put32(p, v, 0x40000002); put32(p, v + 4, 21); put32(p, v + 8, 2);
    put32(p, v + 20, which ? 18 : 36); pointer(p, v + 24, image);
    put32(p, v + 36, which ? 6 : 12); put32(p, v + 40, 1);
    pointer(p, v + 44, v + 0x44); pointer(p, v + 0x44, v + 0x50);
    put32(p, v + 0x50, 0x40000001);
    put32(p, v + 0x50 + 36, which ? 0x1402 : 0x1406);
    put32(p, v + 0x50 + 40, 3); putf(p, v + 0x50 + 44, which ? 0.5f : 1.0f);
    pointer(p, v + 0x90, v + 0xa0);
    put32(p, v + 0xa0 + 12, 1); pointer(p, v + 0xa0 + 16, v + 0xb4);
    pointer(p, v + 0xb4, v + 0xc0);
    put32(p, v + 0xc0, 1); pointer(p, v + 0xc0 + 4, v + 0xd4);
    pointer(p, v + 0xd4, v + 0xe0);
    put32(p, v + 0xe0, which ? 0x1403 : 0x1401);
    p[v + 0xe0 + 5] = 1;
    put32(p, v + 0xe0 + 8, which ? 6 : 3); pointer(p, v + 0xe0 + 12, indices);
    if (which) {
        const int values[9] = {-2, 0, 1, 0, 2, -1, 2, -2, 0};
        for (i = 0; i < 9; ++i) put16(p, image + i * 2, values[i]);
        put16(p, indices, 2); put16(p, indices + 2, 0); put16(p, indices + 4, 1);
    } else {
        const float values[9] = {0, 0, 0, 2, 0, 0, 0, 3, 0};
        for (i = 0; i < 9; ++i) putf(p, image + i * 4, values[i]);
        p[indices] = 0; p[indices + 1] = 2; p[indices + 2] = 1;
    }
}

static void fixture(uint8_t *p) {
    unsigned i;
    memset(p, 0, SIZE);
    memcpy(p, "CGFX\xff\xfe\x14\0", 8);
    put32(p, 8, 0x05000000); put32(p, 12, SIZE); put32(p, 16, 2);
    memcpy(p + 20, "DATA", 4); put32(p, 24, 4096 - 20);
    memcpy(p + 4096, "IMAG", 4); put32(p, 4100, SIZE - 4096);
    put32(p, 28, 1); pointer(p, 32, 0x9c);
    memcpy(p + 0x9c, "DICT", 4); put32(p, 0xa0, 44); put32(p, 0xa4, 1);
    pointer(p, 0xc0, 0xf0); pointer(p, 0xc4, 0x100);
    memcpy(p + 0xf0, "fixture", 8);
    put32(p, 0x100, 0x40000012); memcpy(p + 0x104, "CMDL", 4);
    put32(p, 0x108, 0x09000000); pointer(p, 0x10c, 0xf0);
    for (i = 0; i < 3; ++i) putf(p, 0x130 + i * 4, 1);
    for (i = 0; i < 24; ++i)
        putf(p, 0x154 + i * 4, i % 12 == 0 || i % 12 == 5 || i % 12 == 10 ? 1.0f : 0.0f);
    put32(p, 0x1b4, 2); pointer(p, 0x1b8, 0x208);
    put32(p, 0x1bc, 1); put32(p, 0x1c4, 2); pointer(p, 0x1c8, 0x200);
    make_shape(p, 0); make_shape(p, 1);
    put32(p, 0x500, 0x80000000); put32(p, 0x504, 3); put32(p, 0x508, 1);
    put32(p, 0x50c, 0x1406); put32(p, 0x510, 4); putf(p, 0x514, 0);
    put32(p, 0x518, 4); pointer(p, 0x51c, 0x520);
    for (i = 0; i < 4; ++i) putf(p, 0x520 + i * 4, 1);
}

static void rejected(const uint8_t *data, size_t size, enum px_cgfx_model_result expected) {
    struct px_cgfx_model out;
    enum px_cgfx_model_result actual;
    memset(&out, 0xa5, sizeof(out));
    actual = px_cgfx_model_decode(data, size, 0, &out);
    if (actual != expected)
        fprintf(stderr, "mutation 0x%zx: expected %d, got %d\n", mutation_offset, expected, actual);
    assert(actual == expected);
    assert(!out.vertex_count && !out.index_count && !out.shape_count);
    assert(!out.positions && !out.indices && !out.name[0]);
    px_cgfx_model_free(&out);
}

int main(void) {
    uint8_t original[SIZE], data[SIZE];
    struct px_cgfx_model out;
    const float expected[18] = {0, 0, 0, 2, 0, 0, 0, 3, 0, 9, 20, 30.5f, 10, 21, 29.5f, 11, 19, 30};
    const uint32_t indices[6] = {0, 2, 1, 5, 3, 4};
    size_t i;
    struct mutation { size_t offset; uint32_t value; enum px_cgfx_model_result result; };
    const struct mutation cases[] = {
        {8, 0, PX_CGFX_MODEL_UNSUPPORTED}, {12, SIZE - 1, PX_CGFX_MODEL_INVALID_HEADER},
        {16, 1, PX_CGFX_MODEL_UNSUPPORTED}, {24, 7, PX_CGFX_MODEL_INVALID_HEADER},
        {28, 65537, PX_CGFX_MODEL_LIMIT_EXCEEDED}, {32, 0x80000000, PX_CGFX_MODEL_INVALID_HEADER},
        {0xa0, 43, PX_CGFX_MODEL_INVALID_HEADER}, {0xa4, 2, PX_CGFX_MODEL_INVALID_HEADER},
        {0xc4, 0, PX_CGFX_MODEL_INVALID_HEADER}, {0x100, 0x40000092, PX_CGFX_MODEL_UNSUPPORTED},
        {0x130, 0, PX_CGFX_MODEL_UNSUPPORTED}, {0x154, 0x7fc00000, PX_CGFX_MODEL_INVALID_HEADER},
        {0x1c4, 1025, PX_CGFX_MODEL_LIMIT_EXCEEDED}, {0x1b4, 1, PX_CGFX_MODEL_UNSUPPORTED},
        {0x358, 0, PX_CGFX_MODEL_INVALID_HEADER}, {0x318, 2, PX_CGFX_MODEL_INVALID_HEADER},
        {0x220, 0x10000002, PX_CGFX_MODEL_UNSUPPORTED}, {0x258, 0, PX_CGFX_MODEL_UNSUPPORTED},
        {0x228, 1, PX_CGFX_MODEL_UNSUPPORTED}, {0x308, 1, PX_CGFX_MODEL_UNSUPPORTED},
        {0x744, 1, PX_CGFX_MODEL_UNSUPPORTED}, {0x508, 2, PX_CGFX_MODEL_UNSUPPORTED},
        {0x520, 0x7f800000, PX_CGFX_MODEL_INVALID_HEADER},
        {0x414, 12582924, PX_CGFX_MODEL_LIMIT_EXCEEDED},
        {0x418, 0x400 - 0x418, PX_CGFX_MODEL_INVALID_HEADER},
        {0x424, 0, PX_CGFX_MODEL_UNSUPPORTED}, {0x428, 17, PX_CGFX_MODEL_UNSUPPORTED},
        {0x454, 7, PX_CGFX_MODEL_UNSUPPORTED}, {0x474, 0x140c, PX_CGFX_MODEL_UNSUPPORTED},
        {0x478, 4, PX_CGFX_MODEL_INVALID_HEADER}, {0x47c, 0x7fc00000, PX_CGFX_MODEL_INVALID_HEADER},
        {0x480, 12, PX_CGFX_MODEL_INVALID_HEADER}, {0x4a0, 1, PX_CGFX_MODEL_UNSUPPORTED},
        {0x4a8, 1, PX_CGFX_MODEL_UNSUPPORTED}, {0x4e0, 0x1400, PX_CGFX_MODEL_UNSUPPORTED},
        {0x4e4, 0x101, PX_CGFX_MODEL_UNSUPPORTED}, {0x4e8, 2, PX_CGFX_MODEL_UNSUPPORTED},
        {0x4e8, 3145731, PX_CGFX_MODEL_LIMIT_EXCEEDED},
        {0x6e8, 5, PX_CGFX_MODEL_INVALID_HEADER}, {0x1008, 0x7f800000, PX_CGFX_MODEL_INVALID_HEADER}
    };
    fixture(original);
    assert(px_cgfx_model_decode(original, SIZE, 0, &out) == PX_CGFX_MODEL_OK);
    assert(out.vertex_count == 6 && out.index_count == 6 && out.shape_count == 2);
    assert(!strcmp(out.name, "fixture"));
    assert(!memcmp(out.positions, expected, sizeof(expected)));
    assert(!memcmp(out.indices, indices, sizeof(indices)));
    assert(out.bounds_min[0] == 0 && out.bounds_min[1] == 0 && out.bounds_min[2] == 0);
    assert(out.bounds_max[0] == 11 && out.bounds_max[1] == 21 && out.bounds_max[2] == 30.5f);
    px_cgfx_model_free(&out); px_cgfx_model_free(&out);
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        mutation_offset = cases[i].offset;
        memcpy(data, original, SIZE); put32(data, cases[i].offset, cases[i].value);
        rejected(data, SIZE, cases[i].result);
    }
    for (i = 0; i < SIZE; ++i)
        rejected(original, i, i < 20 ? PX_CGFX_MODEL_TRUNCATED : PX_CGFX_MODEL_INVALID_HEADER);
    memcpy(data, original, SIZE); data[0x1088] = 3;
    rejected(data, SIZE, PX_CGFX_MODEL_INVALID_HEADER);
    memcpy(data, original, SIZE); putf(data, 0x1008, 1.0e13f);
    rejected(data, SIZE, PX_CGFX_MODEL_LIMIT_EXCEEDED);
    memcpy(data, original, SIZE); memset(data + 0xf0, 'x', 16); pointer(data, 0x10c, 4090);
    memset(data + 4090, 'x', 6); rejected(data, SIZE, PX_CGFX_MODEL_INVALID_HEADER);
    assert(px_cgfx_model_decode(original, SIZE, 1, &out) == PX_CGFX_MODEL_INVALID_ARGUMENT);
    rejected(NULL, SIZE, PX_CGFX_MODEL_INVALID_ARGUMENT);
    rejected(original, 64u * 1024u * 1024u + 1, PX_CGFX_MODEL_LIMIT_EXCEEDED);
    assert(px_cgfx_model_decode(original, SIZE, 0, NULL) == PX_CGFX_MODEL_INVALID_ARGUMENT);
    /* Both byte component signedness paths have fixed, independent expectations. */
    for (i = 0; i < 2; ++i) {
        memcpy(data, original, SIZE);
        put32(data, 0x474, i ? 0x1401 : 0x1400); put32(data, 0x414, 9); put32(data, 0x424, 3);
        memset(data + 0x1008, 0, 9); data[0x1008] = 255;
        assert(px_cgfx_model_decode(data, SIZE, 0, &out) == PX_CGFX_MODEL_OK);
        assert(out.positions[0] == (i ? 255.0f : -1.0f));
        px_cgfx_model_free(&out);
    }
    puts("CGFX static geometry: fixed vectors, shape rebasing, finite/bounds/format rejection passed");
    return 0;
}
