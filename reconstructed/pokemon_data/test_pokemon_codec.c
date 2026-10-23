#include "pokemon_data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Synthetic memory and expectations reviewed against 00168548, 001685f0,
 * 00168660, 00174e0c, 00174e6c and the four block-selector ARM listings.
 * These tests do not execute the original game. No game data is embedded. */

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "pokemon codec: %s:%d: %s\n", \
            __FILE__, __LINE__, #condition); abort(); \
} } while (0)

enum {
    MANAGER = 0x100,
    DATA = 0x1000,
    EXTRA = 0x2000,
    RAW = 0x3000,
    LOW_SIZE = 0x5000,
    TABLE = 0x570eec,
    READ_FALLBACK = 0x6482a0,
    WRITE_FALLBACK = 0x64839c,
    BODY_SIZE = 0xe0,
    BODY_WORDS = BODY_SIZE / 2,
    EXTRA_WORDS = PX_POKEMON_EXTRA_SIZE / 2
};

struct codec_fixture {
    uint8_t low[LOW_SIZE];
    uint8_t table[32 * 4];
    uint8_t read_fallback[BODY_SIZE];
    uint8_t write_fallback[BODY_SIZE];
    unsigned reads;
    unsigned writes;
    unsigned body_reads;
    unsigned extra_reads;
    unsigned key_reads;
    unsigned table_reads;
    unsigned raw_reads;
    uint32_t raw_highest;
};

/* Frozen test vector: seed 12345678, halfword plaintext 0..111. The LCG
 * constants/halfword XOR were evaluated independently of the C under test. */
static const uint16_t ciphertext[BODY_WORDS] = {
    0x0b71,0x84eb,0xd988,0xf4e3,0x2680,0x9832,0x91aa,0x0582,
    0x7314,0x6206,0xc458,0x10e3,0x768b,0x9857,0x242c,0xb535,
    0xbfdd,0x4009,0xbaf3,0xae66,0xe658,0x2a48,0x5c30,0x851b,
    0xdd9c,0x3d82,0x8f4a,0x82d8,0xb233,0x0f3a,0x24a4,0x11c5,
    0x04b5,0x90d4,0xea0e,0xea53,0xd439,0xd783,0x32b5,0xb201,
    0x700e,0x3ced,0x24de,0xf228,0xa23b,0x6166,0x06b0,0x86b8,
    0x6484,0xd45a,0xf4ad,0xee51,0xbc87,0x62b9,0xc04e,0x1a42,
    0x86b6,0x50ab,0xe429,0xa562,0x4da6,0x075c,0xc0ca,0x940f,
    0x8018,0xf3e2,0x9eed,0x0659,0x964f,0x9d8d,0x1cdf,0x759e,
    0xf9c2,0x3e07,0x0a88,0x71fc,0xcd1a,0x520c,0xde6f,0xede8,
    0xdcbf,0xf454,0x3443,0x93f9,0x4f36,0xfc06,0x18aa,0xb623,
    0xef7e,0x2fd9,0x060f,0xca15,0x1b74,0xfb58,0xcb94,0x8862,
    0xb544,0x875a,0xd7d8,0x1f07,0xa152,0x2100,0x9283,0x18c1,
    0xaad8,0x419a,0xc308,0xd0e4,0xe241,0xb368,0x2abb,0xab1c
};

/* Separate stream restart for the extra buffer, plaintext 8000..800d. */
static const uint16_t extra_ciphertext[EXTRA_WORDS] = {
    0x8b71,0x04eb,0x5988,0x74e3,0xa680,0x1832,0x11aa,
    0x8582,0xf314,0xe206,0x4458,0x90e3,0xf68b,0x1857
};

/* Synthetic permutations: the last eight deliberately differ from rows 0..7.
 * This is a test table, not a copy of the original game's table. */
static const uint8_t synthetic_table[32][4] = {
    {0,1,2,3}, {0,1,3,2}, {0,2,1,3}, {0,2,3,1},
    {0,3,1,2}, {0,3,2,1}, {1,0,2,3}, {1,0,3,2},
    {1,2,0,3}, {1,2,3,0}, {1,3,0,2}, {1,3,2,0},
    {2,0,1,3}, {2,0,3,1}, {2,1,0,3}, {2,1,3,0},
    {2,3,0,1}, {2,3,1,0}, {3,0,1,2}, {3,0,2,1},
    {3,1,0,2}, {3,1,2,0}, {3,2,0,1}, {3,2,1,0},
    {2,3,1,0}, {3,1,0,2}, {1,3,0,2}, {2,0,1,3},
    {3,2,1,0}, {1,0,3,2}, {0,3,2,1}, {2,1,0,3}
};

static int in_region(uint32_t address, uint32_t base, uint32_t size)
{
    return address >= base && address - base < size;
}

static uint8_t *map_byte(struct codec_fixture *f, uint32_t address)
{
    if (address < LOW_SIZE)
        return &f->low[address];
    if (in_region(address, TABLE, sizeof(f->table)))
        return &f->table[address - TABLE];
    if (in_region(address, READ_FALLBACK, sizeof(f->read_fallback)))
        return &f->read_fallback[address - READ_FALLBACK];
    if (in_region(address, WRITE_FALLBACK, sizeof(f->write_fallback)))
        return &f->write_fallback[address - WRITE_FALLBACK];
    fprintf(stderr, "pokemon codec: unmapped address %08lx\n",
            (unsigned long)address);
    abort();
}

static uint8_t memory_load8(void *user, uint32_t address)
{
    struct codec_fixture *f = user;
    ++f->reads;
    if (in_region(address, DATA + 8, BODY_SIZE)) ++f->body_reads;
    if (in_region(address, EXTRA, PX_POKEMON_EXTRA_SIZE)) ++f->extra_reads;
    if (in_region(address, DATA, 4)) ++f->key_reads;
    if (in_region(address, TABLE, sizeof(f->table))) ++f->table_reads;
    if (in_region(address, RAW, 64)) {
        ++f->raw_reads;
        if (address > f->raw_highest) f->raw_highest = address;
    }
    return *map_byte(f, address);
}

static uint16_t memory_load16(void *user, uint32_t address)
{
    uint16_t lo = memory_load8(user, address);
    uint16_t hi = memory_load8(user, address + 1);
    return (uint16_t)(lo | (uint16_t)(hi << 8));
}

static uint32_t memory_load32(void *user, uint32_t address)
{
    uint32_t lo = memory_load16(user, address);
    uint32_t hi = memory_load16(user, address + 2);
    return lo | (hi << 16);
}

static void memory_store8(void *user, uint32_t address, uint8_t value)
{
    struct codec_fixture *f = user;
    ++f->writes;
    *map_byte(f, address) = value;
}

static void memory_store16(void *user, uint32_t address, uint16_t value)
{
    memory_store8(user, address, (uint8_t)value);
    memory_store8(user, address + 1, (uint8_t)(value >> 8));
}

static void memory_store32(void *user, uint32_t address, uint32_t value)
{
    memory_store16(user, address, (uint16_t)value);
    memory_store16(user, address + 2, (uint16_t)(value >> 16));
}

static struct px_pokemon_api memory_api(struct codec_fixture *f)
{
    struct px_pokemon_api api;
    memset(&api, 0, sizeof(api));
    api.user = f;
    api.load8 = memory_load8;
    api.load16 = memory_load16;
    api.load32 = memory_load32;
    api.store8 = memory_store8;
    api.store16 = memory_store16;
    api.store32 = memory_store32;
    return api;
}

/* Setup and expectations bypass the adapter, so they cannot satisfy read or
 * write assertions themselves. */
static void put16(struct codec_fixture *f, uint32_t address, uint16_t value)
{
    *map_byte(f, address) = (uint8_t)value;
    *map_byte(f, address + 1) = (uint8_t)(value >> 8);
}

static void put32(struct codec_fixture *f, uint32_t address, uint32_t value)
{
    put16(f, address, (uint16_t)value);
    put16(f, address + 2, (uint16_t)(value >> 16));
}

static uint16_t get16(const struct codec_fixture *f, uint32_t address)
{
    CHECK(address + 1 < LOW_SIZE);
    return (uint16_t)(f->low[address] | (uint16_t)(f->low[address + 1] << 8));
}

static void reset_accesses(struct codec_fixture *f)
{
    f->reads = f->writes = f->body_reads = f->extra_reads = 0;
    f->key_reads = f->table_reads = f->raw_reads = 0;
    f->raw_highest = 0;
}

static void install_plaintext(struct codec_fixture *f, int with_extra)
{
    unsigned i;
    for (i = 0; i < BODY_WORDS; ++i)
        put16(f, DATA + 8 + i * 2, (uint16_t)i);
    if (with_extra) {
        for (i = 0; i < EXTRA_WORDS; ++i)
            put16(f, EXTRA + i * 2, (uint16_t)(0x8000 + i));
    }
}

static void install_ciphertext(struct codec_fixture *f, int with_extra)
{
    unsigned i;
    for (i = 0; i < BODY_WORDS; ++i)
        put16(f, DATA + 8 + i * 2, ciphertext[i]);
    if (with_extra) {
        for (i = 0; i < EXTRA_WORDS; ++i)
            put16(f, EXTRA + i * 2, extra_ciphertext[i]);
    }
}

static void initialize(struct codec_fixture *f, int with_extra)
{
    memset(f, 0, sizeof(*f));
    memset(f->low, 0xa5, sizeof(f->low));
    memset(f->read_fallback, 0x36, sizeof(f->read_fallback));
    memset(f->write_fallback, 0xc9, sizeof(f->write_fallback));
    memcpy(f->table, synthetic_table, sizeof(f->table));
    put32(f, MANAGER, UINT32_C(0xf00dcafe));
    put32(f, MANAGER + 4, with_extra ? EXTRA : 0);
    put32(f, MANAGER + 8, DATA);
    f->low[MANAGER + 12] = 0;
    f->low[MANAGER + 13] = 0;
    f->low[MANAGER + 14] = 0x77;
    f->low[MANAGER + 15] = 0x88;
    put32(f, DATA, UINT32_C(0x12345678));
    put16(f, DATA + 4, 0x9002);
    put16(f, DATA + 6, 0xbabe);
    install_plaintext(f, 1);
}

static void same_memory(const struct codec_fixture *actual,
                        const struct codec_fixture *expected)
{
    CHECK(memcmp(actual->low, expected->low, sizeof(actual->low)) == 0);
    CHECK(memcmp(actual->table, expected->table, sizeof(actual->table)) == 0);
    CHECK(memcmp(actual->read_fallback, expected->read_fallback,
                 sizeof(actual->read_fallback)) == 0);
    CHECK(memcmp(actual->write_fallback, expected->write_fallback,
                 sizeof(actual->write_fallback)) == 0);
}

static void test_checksum(void)
{
    static const uint8_t bytes[] = {
        0x01,0x02,0xff,0xff,0x00,0x80,0x34,0x12,0xfe,
        0xff,0x01,0x00,0xff,0x7f,0x55,0xaa,0x99
    };
    static const uint16_t expected[] = {
        0,0x0201,0x0200,0x8200,0x9434,0x9432,0x9433,0x1432,0xbe87
    };
    struct codec_fixture f;
    struct px_pokemon_api api = memory_api(&f);
    unsigned size;
    initialize(&f, 0);
    memcpy(f.low + RAW, bytes, sizeof(bytes));
    for (size = 0; size <= sizeof(bytes); ++size) {
        reset_accesses(&f);
        CHECK(px_pokemon_checksum(RAW, size, &api) == expected[size / 2]);
        CHECK(f.writes == 0);
        CHECK(f.raw_reads == 2 * (size / 2));
        if (size < 2) CHECK(f.reads == 0);
        else CHECK(f.raw_highest == RAW + 2 * (size / 2) - 1);
    }
    /* Zero/one-byte sizes need no valid address at all. */
    CHECK(px_pokemon_checksum(UINT32_MAX, 0, &api) == 0);
    CHECK(px_pokemon_checksum(UINT32_MAX, 1, &api) == 0);
    CHECK(px_pokemon_checksum(DATA + 8, BODY_SIZE, &api) == 0x1848);
}

static void test_encode_flag_matrix(void)
{
    static const uint8_t flags[] = {0,1,2,0x7f,0x80,0xff};
    struct codec_fixture f, expected;
    struct px_pokemon_api api = memory_api(&f);
    unsigned ci, di;
    int with_extra;
    for (with_extra = 0; with_extra <= 1; ++with_extra) {
        for (ci = 0; ci < sizeof(flags); ++ci) {
            for (di = 0; di < sizeof(flags); ++di) {
                initialize(&f, with_extra);
                f.low[MANAGER + 12] = flags[ci];
                f.low[MANAGER + 13] = flags[di];
                if (ci != 0 || di != 0) {
                    /* Skipping must happen before following either pointer. */
                    put32(&f, MANAGER + 8, UINT32_C(0xbad00000));
                    put32(&f, MANAGER + 4, UINT32_C(0xbad10000));
                }
                expected = f;
                if (ci == 0 && di == 0) {
                    install_ciphertext(&expected, with_extra);
                    put16(&expected, DATA + 6, 0x1848);
                    expected.low[MANAGER + 12] = 1;
                }
                px_encode_pokemon_manager(MANAGER, &api);
                same_memory(&f, &expected);
                if (ci != 0 || di != 0) {
                    CHECK(f.writes == 0);
                    CHECK(f.body_reads == 0 && f.extra_reads == 0);
                } else if (!with_extra) {
                    CHECK(f.extra_reads == 0);
                }
            }
        }
    }
}

static void test_decode_flag_matrix(void)
{
    static const uint8_t flags[] = {0,1,2,0x7f,0x80,0xff};
    struct codec_fixture f, expected;
    struct px_pokemon_api api = memory_api(&f);
    unsigned ci, di;
    int with_extra;
    for (with_extra = 0; with_extra <= 1; ++with_extra) {
        for (ci = 0; ci < sizeof(flags); ++ci) {
            for (di = 0; di < sizeof(flags); ++di) {
                initialize(&f, with_extra);
                install_ciphertext(&f, with_extra);
                put16(&f, DATA + 6, 0x1848);
                f.low[MANAGER + 12] = flags[ci];
                f.low[MANAGER + 13] = flags[di];
                if (ci == 0) {
                    put32(&f, MANAGER + 8, UINT32_C(0xbad00000));
                    put32(&f, MANAGER + 4, UINT32_C(0xbad10000));
                }
                expected = f;
                if (ci != 0) {
                    install_plaintext(&expected, with_extra);
                    expected.low[MANAGER + 12] = 0;
                }
                px_decode_pokemon_manager(MANAGER, &api);
                same_memory(&f, &expected);
                if (ci == 0) {
                    CHECK(f.writes == 0);
                    CHECK(f.body_reads == 0 && f.extra_reads == 0);
                } else if (!with_extra) {
                    CHECK(f.extra_reads == 0);
                }
            }
        }
    }
}

static void test_bind(void)
{
    struct codec_fixture f, expected;
    struct px_pokemon_api api = memory_api(&f);
    int with_extra;
    for (with_extra = 0; with_extra <= 1; ++with_extra) {
        initialize(&f, with_extra);
        expected = f;
        install_ciphertext(&expected, with_extra);
        put16(&expected, DATA + 6, 0x1848);
        expected.low[MANAGER + 12] = 1;
        put32(&f, MANAGER + 4, UINT32_C(0xbad00000));
        put32(&f, MANAGER + 8, UINT32_C(0xbad10000));
        f.low[MANAGER + 12] = 0xff;
        f.low[MANAGER + 13] = 0xff;
        px_bind_pokemon_data(MANAGER, DATA, with_extra ? EXTRA : 0, &api);
        same_memory(&f, &expected);
        if (!with_extra) CHECK(f.extra_reads == 0);
        /* The matching decode restores the caller's buffers, not just a
         * prefix, while the checksum and bound pointers remain installed. */
        install_plaintext(&expected, with_extra);
        expected.low[MANAGER + 12] = 0;
        px_decode_pokemon_manager(MANAGER, &api);
        same_memory(&f, &expected);
    }
}

static void test_checksum_mismatch_and_sticky_corruption(void)
{
    struct codec_fixture f, expected;
    struct px_pokemon_api api = memory_api(&f);
    initialize(&f, 1);
    install_ciphertext(&f, 1);
    f.low[MANAGER + 12] = 0x80;
    put16(&f, DATA + 6, 0x1849);
    expected = f;
    install_plaintext(&expected, 1);
    expected.low[MANAGER + 12] = 0;
    put16(&expected, DATA + 4, 0x9006);
    px_decode_pokemon_manager(MANAGER, &api);
    same_memory(&f, &expected);

    /* Encoding repairs the checksum, but neither it nor a successful decode
     * clears the corruption marker established by the previous mismatch. */
    install_ciphertext(&expected, 1);
    put16(&expected, DATA + 6, 0x1848);
    expected.low[MANAGER + 12] = 1;
    px_encode_pokemon_manager(MANAGER, &api);
    same_memory(&f, &expected);
    install_plaintext(&expected, 1);
    expected.low[MANAGER + 12] = 0;
    px_decode_pokemon_manager(MANAGER, &api);
    same_memory(&f, &expected);

    /* A ciphertext mutation must also produce a mismatch, rather than
     * testing the stored checksum against the encrypted bytes. */
    initialize(&f, 0);
    install_ciphertext(&f, 0);
    f.low[DATA + 8] ^= 1;
    put16(&f, DATA + 6, 0x1848);
    f.low[MANAGER + 12] = 1;
    expected = f;
    install_plaintext(&expected, 0);
    expected.low[DATA + 8] = 1;
    expected.low[MANAGER + 12] = 0;
    put16(&expected, DATA + 4, 0x9006);
    px_decode_pokemon_manager(MANAGER, &api);
    same_memory(&f, &expected);
    CHECK(px_pokemon_checksum(DATA + 8, BODY_SIZE, &api) == 0x1849);

    /* A different header flag must not be mistaken for corruption, and a
     * preexisting corruption bit survives even without a prior failure. */
    initialize(&f, 0);
    install_ciphertext(&f, 0);
    put16(&f, DATA + 4, 0xffff);
    put16(&f, DATA + 6, 0x1848);
    f.low[MANAGER + 12] = 1;
    expected = f;
    install_plaintext(&expected, 0);
    expected.low[MANAGER + 12] = 0;
    px_decode_pokemon_manager(MANAGER, &api);
    same_memory(&f, &expected);
}

static void test_key_change_round_trip(void)
{
    static const uint16_t changed_key_prefix[] = {
        0x1c01,0xb4da,0x1631,0xd88e,0xf4af,0x9817,0x9a8a,0x8ae1,
        0xef94,0xe96e,0xb033,0xbf7a,0xc377,0xd599,0x9b89,0xc60a
    };
    struct codec_fixture f, expected;
    struct px_pokemon_api api = memory_api(&f);
    unsigned i;
    initialize(&f, 1);
    px_encode_pokemon_manager(MANAGER, &api);
    px_decode_pokemon_manager(MANAGER, &api);
    put32(&f, DATA, UINT32_C(0xdeadbeef));
    expected = f;
    px_encode_pokemon_manager(MANAGER, &api);
    for (i = 0; i < sizeof(changed_key_prefix) / sizeof(changed_key_prefix[0]); ++i)
        CHECK(get16(&f, DATA + 8 + i * 2) == changed_key_prefix[i]);
    for (i = 0; i < EXTRA_WORDS; ++i)
        CHECK(get16(&f, EXTRA + i * 2) == (uint16_t)(changed_key_prefix[i] ^ 0x8000));
    CHECK(get16(&f, DATA + 6) == 0x1848);
    px_decode_pokemon_manager(MANAGER, &api);
    same_memory(&f, &expected);
}

static void test_normal_blocks(void)
{
    static const uint32_t outside_bits[] = {
        0, UINT32_C(0xfffc1fff), UINT32_C(0x80000000)
    };
    static const uint32_t block_offsets[] = {8,64,120,176};
    struct codec_fixture f, expected;
    struct px_pokemon_api api = memory_api(&f);
    unsigned row, column, variant;
    initialize(&f, 1);
    /* All other data flags and both nonzero manager flags are irrelevant. */
    put16(&f, DATA + 4, 0xfffb);
    f.low[MANAGER + 12] = 0xff;
    f.low[MANAGER + 13] = 0x80;
    for (row = 0; row < 32; ++row) {
        for (variant = 0; variant < sizeof(outside_bits) / sizeof(outside_bits[0]); ++variant) {
            put32(&f, DATA, (uint32_t)(row * 0x2000) | outside_bits[variant]);
            expected = f;
            for (column = 0; column < 4; ++column) {
                reset_accesses(&f);
                CHECK(px_pokemon_block(MANAGER, column, variant, &api) ==
                      DATA + block_offsets[synthetic_table[row][column]]);
                CHECK(f.table_reads != 0);
                CHECK(f.writes == 0);
                same_memory(&f, &expected);
            }
        }
    }
    /* Reading the logical table on each call, including unsigned byte
     * arithmetic, is observable even for a deliberately non-permutation. */
    put32(&f, DATA, 31 * 0x2000);
    f.table[31 * 4 + 3] = 0xff;
    reset_accesses(&f);
    CHECK(px_pokemon_block(MANAGER, 3, 0, &api) == DATA + 8 + 56 * 255);
    CHECK(f.table_reads != 0 && f.writes == 0);
    f.table[31 * 4 + 3] = 0;
    CHECK(px_pokemon_block(MANAGER, 3, UINT32_MAX, &api) == DATA + 8);
}

static void test_corrupt_blocks(void)
{
    static const uint16_t corrupt_flags[] = {4,0xffff,0x9006};
    static const uint32_t write_modes[] = {
        0,1,2,0x100,UINT32_C(0x80000000),UINT32_MAX
    };
    static const uint32_t read_addresses[] = {
        0x6482a0,0x6482d8,0x648310,0x648348
    };
    static const uint32_t write_addresses[] = {
        0x64839c,0x6483d4,0x64840c,0x648444
    };
    struct codec_fixture f, expected;
    struct px_pokemon_api api = memory_api(&f);
    unsigned flag, column, mode;
    initialize(&f, 1);
    for (flag = 0; flag < sizeof(corrupt_flags) / sizeof(corrupt_flags[0]); ++flag) {
        put16(&f, DATA + 4, corrupt_flags[flag]);
        expected = f;
        for (column = 0; column < 4; ++column) {
            for (mode = 0; mode < sizeof(write_modes) / sizeof(write_modes[0]); ++mode) {
                reset_accesses(&f);
                CHECK(px_pokemon_block(MANAGER, column, write_modes[mode], &api) ==
                      (mode == 0 ? read_addresses[column] : write_addresses[column]));
                CHECK(f.key_reads == 0 && f.table_reads == 0);
                CHECK(f.writes == 0);
                same_memory(&f, &expected);
            }
        }
    }
}

void test_pokemon_codec(void)
{
    test_checksum();
    test_encode_flag_matrix();
    test_decode_flag_matrix();
    test_bind();
    test_checksum_mismatch_and_sticky_corruption();
    test_key_change_round_trip();
    test_normal_blocks();
    test_corrupt_blocks();
}
