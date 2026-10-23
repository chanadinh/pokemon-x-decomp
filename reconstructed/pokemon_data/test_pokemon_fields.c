#include "pokemon_data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "field test failed at %s:%d: %s\n", __FILE__, __LINE__, #condition); \
    abort(); \
} } while (0)

enum {
    LOCAL_BASE = 0x1000, MANAGER = 0x1000, DATA = 0x1100, EXTRA = 0x1200,
    TABLE = 0x570eec, FALLBACK_BASE = 0x648280,
    READ_A = 0x6482a0, WRITE_A = 0x64839c, EXTRA_FALLBACK = 0x648380,
    FIELD_COUNT = PX_DATA_GROWTH + 1
};

struct field_fixture {
    uint8_t local[0x400];
    uint8_t fallback[0x200];
    uint8_t permutation[32 * 4];
    unsigned reads;
    unsigned writes;
};

static uint8_t *raw(struct field_fixture *f, uint32_t address)
{
    if (address >= LOCAL_BASE && address < LOCAL_BASE + sizeof(f->local))
        return &f->local[address - LOCAL_BASE];
    if (address >= FALLBACK_BASE && address < FALLBACK_BASE + sizeof(f->fallback))
        return &f->fallback[address - FALLBACK_BASE];
    if (address >= TABLE && address < TABLE + sizeof(f->permutation))
        return &f->permutation[address - TABLE];
    fprintf(stderr, "unexpected logical field-test address: %08x\n", (unsigned)address);
    abort();
}

static uint32_t raw_load(struct field_fixture *f, uint32_t address, unsigned width)
{
    uint32_t value = 0;
    for (unsigned i = 0; i < width; ++i)
        value |= (uint32_t)*raw(f, address + i) << (8 * i);
    return value;
}

static void raw_store(struct field_fixture *f, uint32_t address, uint32_t value,
                      unsigned width)
{
    for (unsigned i = 0; i < width; ++i)
        *raw(f, address + i) = (uint8_t)(value >> (8 * i));
}

static uint8_t load8(void *user, uint32_t address)
{
    struct field_fixture *f = user;
    ++f->reads;
    return (uint8_t)raw_load(f, address, 1);
}

static uint16_t load16(void *user, uint32_t address)
{
    struct field_fixture *f = user;
    ++f->reads;
    return (uint16_t)raw_load(f, address, 2);
}

static uint32_t load32(void *user, uint32_t address)
{
    struct field_fixture *f = user;
    ++f->reads;
    return raw_load(f, address, 4);
}

static void store8(void *user, uint32_t address, uint8_t value)
{
    struct field_fixture *f = user;
    ++f->writes;
    raw_store(f, address, value, 1);
}

static void store16(void *user, uint32_t address, uint16_t value)
{
    struct field_fixture *f = user;
    ++f->writes;
    raw_store(f, address, value, 2);
}

static void store32(void *user, uint32_t address, uint32_t value)
{
    struct field_fixture *f = user;
    ++f->writes;
    raw_store(f, address, value, 4);
}

static struct px_pokemon_api memory_api(struct field_fixture *f)
{
    const struct px_pokemon_api api = {
        .user = f, .load8 = load8, .load16 = load16, .load32 = load32,
        .store8 = store8, .store16 = store16, .store32 = store32
    };
    return api;
}

static void initialize(struct field_fixture *f, unsigned row, uint8_t pattern)
{
    memset(f, 0, sizeof(*f));
    memset(f->local, pattern, sizeof(f->local));
    memset(f->fallback, pattern ^ UINT8_C(0x5a), sizeof(f->fallback));
    /* Invented permutations, not copied game resources. Enumerate 24 distinct
     * orders and repeat the first eight for the full five-bit row domain. */
    for (unsigned r = 0; r < 32; ++r) {
        uint8_t pool[] = { 0, 1, 2, 3 };
        const unsigned divisor[] = { 6, 2, 1, 1 };
        unsigned rank = r % 24;
        for (unsigned block = 0; block < 4; ++block) {
            const unsigned chosen = rank / divisor[block];
            rank %= divisor[block];
            f->permutation[4 * r + block] = pool[chosen];
            for (unsigned j = chosen; j + 1 < 4 - block; ++j)
                pool[j] = pool[j + 1];
        }
    }
    raw_store(f, MANAGER + 4, EXTRA, 4);
    raw_store(f, MANAGER + 8, DATA, 4);
    raw_store(f, MANAGER + 0x0c, 0, 1);
    raw_store(f, MANAGER + 0x0d, 1, 1); /* Plain, held state: no codec writes. */
    raw_store(f, DATA, UINT32_C(0xa500001b) | ((uint32_t)row << 13), 4);
    raw_store(f, DATA + 4, UINT32_C(0xb120), 2); /* Corrupt bit is clear. */
}

static void same_memory(const struct field_fixture *actual,
                        const struct field_fixture *expected)
{
    CHECK(memcmp(actual->local, expected->local, sizeof(actual->local)) == 0);
    CHECK(memcmp(actual->fallback, expected->fallback, sizeof(actual->fallback)) == 0);
    CHECK(memcmp(actual->permutation, expected->permutation,
                 sizeof(actual->permutation)) == 0);
}

/* The descriptors below were transcribed from the STR/STRH/STRB and mask
 * assembly exports in outputs/pokemon-data/core, independently of the host
 * setter implementation. A/B/C/D are logical blocks 0/1/2/3. Special block
 * values denote the direct key, auxiliary byte, and conditional parameter. */
struct field_spec {
    int block;
    unsigned offset;
    unsigned width;
    uint32_t clear_mask;
    uint32_t incoming_mask;
    unsigned shift;
    unsigned write_mode;
};

static const struct field_spec specs[FIELD_COUNT] = {
    [PX_DATA_KEY]            = { -1, 0,    4, UINT32_MAX, UINT32_MAX, 0, 1 },
    [PX_DATA_WORD_10]        = {  0, 4,    4, UINT32_MAX, UINT32_MAX, 0, 1 },
    [PX_DATA_WORD_08]        = {  0, 0x10, 4, UINT32_MAX, UINT32_MAX, 0, 1 },
    [PX_DATA_SPECIES]        = {  0, 0,    2, 0xffff, 0xffff, 0, 1 },
    [PX_DATA_FORM]           = {  0, 0x15, 1, 0xf8, 0xf8, 3, 1 },
    [PX_DATA_EXTRA_LEVEL]    = { -2, 4,    1, 0xff, 0xff, 0, 1 },
    [PX_DATA_NATURE]         = {  0, 0x14, 1, 0xff, 0xff, 0, 1 },
    [PX_DATA_GENDER]         = {  0, 0x15, 1, 6, 6, 1, 1 },
    [PX_DATA_LEVEL]          = {  3, 0x2d, 1, 0x7f, 0x7f, 0, 1 },
    [PX_DATA_ZERO_BYTE]      = {  3, 0x2f, 1, 0xff, 0xff, 0, 1 },
    [PX_DATA_PARAM_30]       = { -3, 0,    1, 0xff, 0xff, 0, 1 },
    [PX_DATA_SETTING_68]     = {  3, 0x33, 1, 0xff, 0xff, 0, 0 },
    [PX_DATA_SETTING_69]     = {  3, 0x30, 1, 0xff, 0xff, 0, 1 },
    [PX_DATA_SETTING_6A]     = {  3, 0x31, 1, 0xff, 0xff, 0, 0 },
    [PX_DATA_SETTING_6B]     = {  3, 0x32, 1, 0xff, 0xff, 0, 0 },
    [PX_DATA_STAT_24]        = {  1, 0x34, 4, 0x0000001f, 0x0000001f, 0, 1 },
    [PX_DATA_STAT_26]        = {  1, 0x34, 4, 0x000003e0, 0x000003e0, 5, 1 },
    [PX_DATA_STAT_28]        = {  1, 0x34, 4, 0x00007c00, 0x00007c00, 10, 1 },
    [PX_DATA_STAT_2A]        = {  1, 0x34, 4, 0x01f00000, 0x01f00000, 20, 1 },
    [PX_DATA_STAT_2C]        = {  1, 0x34, 4, 0x3e000000, 0x3e000000, 25, 1 },
    [PX_DATA_STAT_2E]        = {  1, 0x34, 4, 0x000f8000, 0x000f8000, 15, 1 },
    [PX_DATA_ABILITY]        = {  0, 0x0c, 1, 0xff, 0xff, 0, 1 },
    /* These three OR the incoming value without Boolean normalization. */
    [PX_DATA_ABILITY_FLAG_0] = {  0, 0x0d, 1, 1, UINT32_MAX, 0, 1 },
    [PX_DATA_ABILITY_FLAG_1] = {  0, 0x0d, 1, 2, UINT32_MAX, 1, 1 },
    [PX_DATA_ABILITY_FLAG_2] = {  0, 0x0d, 1, 4, UINT32_MAX, 2, 1 },
    [PX_DATA_GROWTH]         = {  0, 8,    4, UINT32_MAX, UINT32_MAX, 0, 1 }
};

static uint32_t expected_block(struct field_fixture *f, unsigned block,
                               unsigned write_mode)
{
    if ((raw_load(f, DATA + 4, 2) & 4) != 0)
        return (write_mode != 0 ? WRITE_A : READ_A) + UINT32_C(0x38) * block;
    const unsigned row = (raw_load(f, DATA, 4) >> 13) & 31u;
    return DATA + 8 + UINT32_C(0x38) * f->permutation[4 * row + block];
}

static void expected_set(struct field_fixture *f, enum px_pokemon_field field,
                         uint32_t value)
{
    const struct field_spec *s = &specs[field];
    uint32_t address;
    if (s->block == -1) {
        address = DATA;
    } else if (s->block == -2) {
        const uint32_t extra = raw_load(f, MANAGER + 4, 4);
        if (extra == 0) return;
        address = ((raw_load(f, DATA + 4, 2) & 4) != 0 ? EXTRA_FALLBACK : extra) + 4;
    } else if (s->block == -3) {
        const uint32_t probe = expected_block(f, 2, 0) + 0x1b;
        address = *raw(f, probe) != 0 ? expected_block(f, 2, 1) + 0x2a :
                                        expected_block(f, 3, 1) + 0x1a;
    } else {
        address = expected_block(f, (unsigned)s->block, s->write_mode) + s->offset;
    }
    const uint32_t old = raw_load(f, address, s->width);
    const uint32_t updated = (old & ~s->clear_mask) |
                              ((value << s->shift) & s->incoming_mask);
    raw_store(f, address, updated, s->width);
}

static const uint32_t values[] = {
    0, 1, 0x7f, 0x80, 0xff, 0x100, 0xffff, 0x10000,
    UINT32_C(0x89abcdef), UINT32_MAX
};

static void mapping_cases(void)
{
    for (unsigned row = 0; row < 32; ++row) {
        for (unsigned field = 0; field < FIELD_COUNT; ++field) {
            for (unsigned v = 0; v < sizeof(values) / sizeof(values[0]); ++v) {
                struct field_fixture actual, expected;
                initialize(&actual, row, (uint8_t)(row * 7u + v * 19u));
                *raw(&actual, expected_block(&actual, 2, 0) + 0x1b) =
                    row % 2 == 0 ? 0 : UINT8_C(0x80);
                expected = actual;
                expected_set(&expected, (enum px_pokemon_field)field, values[v]);
                const struct px_pokemon_api api = memory_api(&actual);
                px_set_pokemon_field(MANAGER, (enum px_pokemon_field)field, values[v], &api);
                same_memory(&actual, &expected);
            }
        }
    }
}

static void packed_cases(void)
{
    static const enum px_pokemon_field fields[] = {
        PX_DATA_FORM, PX_DATA_GENDER, PX_DATA_LEVEL,
        PX_DATA_STAT_24, PX_DATA_STAT_26, PX_DATA_STAT_28,
        PX_DATA_STAT_2A, PX_DATA_STAT_2C, PX_DATA_STAT_2E,
        PX_DATA_ABILITY_FLAG_0, PX_DATA_ABILITY_FLAG_1, PX_DATA_ABILITY_FLAG_2
    };
    static const uint8_t patterns[] = { 0, 0xff, 0x55, 0xaa };
    for (unsigned p = 0; p < sizeof(patterns); ++p) {
        for (unsigned i = 0; i < sizeof(fields) / sizeof(fields[0]); ++i) {
            for (unsigned value = 0; value < 256; ++value) {
                struct field_fixture actual, expected;
                initialize(&actual, value & 31u, patterns[p]);
                expected = actual;
                expected_set(&expected, fields[i], value);
                const struct px_pokemon_api api = memory_api(&actual);
                px_set_pokemon_field(MANAGER, fields[i], value, &api);
                same_memory(&actual, &expected);
            }
        }
    }
}

static void fallback_cases(void)
{
    for (unsigned branch = 0; branch < 2; ++branch) {
        for (unsigned field = 0; field < FIELD_COUNT; ++field) {
            for (unsigned v = 0; v < sizeof(values) / sizeof(values[0]); ++v) {
                struct field_fixture actual, expected;
                initialize(&actual, 23, UINT8_C(0xa6));
                raw_store(&actual, DATA + 4, UINT32_C(0xb124), 2);
                /* The read and write C regions deliberately disagree. */
                *raw(&actual, READ_A + 2 * 0x38 + 0x1b) = branch != 0 ? 0x80 : 0;
                *raw(&actual, WRITE_A + 2 * 0x38 + 0x1b) = branch != 0 ? 0 : 0x80;
                expected = actual;
                expected_set(&expected, (enum px_pokemon_field)field, values[v]);
                const struct px_pokemon_api api = memory_api(&actual);
                px_set_pokemon_field(MANAGER, (enum px_pokemon_field)field, values[v], &api);
                same_memory(&actual, &expected);
            }
        }
    }
    /* EXTRA_LEVEL must return before decoding when the initial extra is zero,
     * even if the manager says the data is encrypted or corrupt. */
    for (unsigned encrypted = 0; encrypted < 2; ++encrypted) {
        for (unsigned corrupt = 0; corrupt < 2; ++corrupt) {
            struct field_fixture actual, expected;
            initialize(&actual, 7, UINT8_C(0x5c));
            raw_store(&actual, MANAGER + 4, 0, 4);
            raw_store(&actual, MANAGER + 0x0c, encrypted, 1);
            raw_store(&actual, DATA + 4, corrupt * 4u, 2);
            expected = actual;
            const struct px_pokemon_api api = memory_api(&actual);
            px_set_pokemon_field(MANAGER, PX_DATA_EXTRA_LEVEL, UINT32_MAX, &api);
            same_memory(&actual, &expected);
            CHECK(actual.reads == 1 && actual.writes == 0);
        }
    }
}

static void selector_cases(void)
{
    static const uint32_t modes[] = { 0, 1, UINT32_MAX };
    for (unsigned corrupt = 0; corrupt < 2; ++corrupt) {
        for (unsigned row = 0; row < 32; ++row) {
            struct field_fixture actual, expected;
            initialize(&actual, row, UINT8_C(0x37));
            raw_store(&actual, DATA + 4, UINT32_C(0xb120) | corrupt * 4u, 2);
            expected = actual;
            const struct px_pokemon_api api = memory_api(&actual);
            for (unsigned block = 0; block < 4; ++block) {
                for (unsigned mode = 0; mode < sizeof(modes) / sizeof(modes[0]); ++mode)
                    CHECK(px_pokemon_block(MANAGER, block, modes[mode], &api) ==
                          expected_block(&expected, block, modes[mode]));
            }
            same_memory(&actual, &expected);
            CHECK(actual.writes == 0);
        }
    }
}

static uint16_t expected_checksum(struct field_fixture *f)
{
    uint32_t sum = 0;
    for (unsigned offset = 8; offset < 0xe8; offset += 2)
        sum += raw_load(f, DATA + offset, 2);
    return (uint16_t)sum;
}

static void encrypted_cases(void)
{
    /* Unlike the mapping-only cases, invoke actual encode/decode work around
     * every setter, including rekeying and the auxiliary-buffer transform. */
    for (unsigned branch = 0; branch < 2; ++branch) {
        for (unsigned field = 0; field < FIELD_COUNT; ++field) {
            struct field_fixture actual, expected;
            initialize(&actual, (field + 7u) & 31u, UINT8_C(0x96));
            raw_store(&actual, MANAGER + 0x0d, 0, 1);
            *raw(&actual, expected_block(&actual, 2, 0) + 0x1b) = branch != 0 ? 0xff : 0;
            expected = actual;
            const uint32_t value = UINT32_C(0xfedcba98) + field;
            expected_set(&expected, (enum px_pokemon_field)field, value);
            raw_store(&expected, DATA + 6, expected_checksum(&expected), 2);
            const struct px_pokemon_api api = memory_api(&actual);
            px_encode_pokemon_manager(MANAGER, &api);
            CHECK(*raw(&actual, MANAGER + 0x0c) == 1);
            px_set_pokemon_field(MANAGER, (enum px_pokemon_field)field, value, &api);
            CHECK(*raw(&actual, MANAGER + 0x0c) == 1);
            px_decode_pokemon_manager(MANAGER, &api);
            CHECK(*raw(&actual, MANAGER + 0x0c) == 0);
            same_memory(&actual, &expected);
        }
    }
}

void test_pokemon_fields(void)
{
    CHECK(FIELD_COUNT == 26);
    mapping_cases();
    packed_cases();
    fallback_cases();
    selector_cases();
    encrypted_cases();
    puts("PASS: all 26 setters; 8320 mapping/width cases; 12288 packed-byte cases; "
         "520 corrupt fallback cases; 768 block selections; 4 absent-extra guards; "
         "52 encrypted setter round trips");
}
