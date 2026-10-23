#include "state_fields.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { \
    fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #c); \
    exit(EXIT_FAILURE); \
} } while (0)

enum { SIZE = 0x649000, MANAGER = 0x100, DATA = 0x200, EXTRA = 0x400 };
struct fixture { uint8_t *bytes; struct px_pokemon_api memory; unsigned accesses; };
static uint8_t load8(void *user, uint32_t address)
{
    struct fixture *f = user;
    CHECK(address < SIZE);
    ++f->accesses;
    return f->bytes[address];
}
static uint16_t load16(void *user, uint32_t address)
{
    return (uint16_t)((uint32_t)load8(user, address) |
                      (uint32_t)load8(user, address + 1) << 8);
}
static uint32_t load32(void *user, uint32_t address)
{
    return (uint32_t)load16(user, address) | (uint32_t)load16(user, address + 2) << 16;
}
static void store8(void *user, uint32_t address, uint8_t value)
{
    struct fixture *f = user;
    CHECK(address < SIZE);
    ++f->accesses;
    f->bytes[address] = value;
}
static void store16(void *user, uint32_t address, uint16_t value)
{
    store8(user, address, (uint8_t)value);
    store8(user, address + 1, (uint8_t)(value >> 8));
}
static void store32(void *user, uint32_t address, uint32_t value)
{
    store16(user, address, (uint16_t)value);
    store16(user, address + 2, (uint16_t)(value >> 16));
}
static uint16_t expected_checksum(const uint8_t *bytes)
{
    unsigned sum = 0;
    for (unsigned i = 8; i < PX_POKEMON_DATA_SIZE; i += 2)
        sum += (unsigned)bytes[i] + 256u * bytes[i + 1];
    return (uint16_t)sum;
}
static void reset(struct fixture *f, unsigned row)
{
    memset(f->bytes + MANAGER, 0, PX_POKEMON_MANAGER_SIZE);
    memset(f->bytes + DATA - 1, 0x5a, PX_POKEMON_DATA_SIZE + 2);
    memset(f->bytes + EXTRA - 1, 0xa5, PX_POKEMON_EXTRA_SIZE + 2);
    store32(f, MANAGER + 4, EXTRA);
    store32(f, MANAGER + 8, DATA);
    store32(f, DATA, row << 13);
    store16(f, DATA + 4, 0);
    store16(f, DATA + 6, 0);
    for (unsigned block = 0; block < 4; ++block)
        store8(f, UINT32_C(0x00570eec) + 4 * row + block, (uint8_t)((row + block) % 4));
}
static void guards(struct fixture *f)
{
    CHECK(load8(f, DATA - 1) == 0x5a);
    CHECK(load8(f, DATA + PX_POKEMON_DATA_SIZE) == 0x5a);
    CHECK(load8(f, EXTRA - 1) == 0xa5);
    CHECK(load8(f, EXTRA + PX_POKEMON_EXTRA_SIZE) == 0xa5);
}
static void test_slots(struct fixture *f)
{
    for (unsigned row = 0; row < 32; ++row)
        for (unsigned field = 0; field < 3; ++field)
            for (unsigned slot = 0; slot < 4; ++slot)
                for (unsigned state = 0; state < 4; ++state) {
                    reset(f, row);
                    uint8_t expected[PX_POKEMON_DATA_SIZE];
                    memcpy(expected, f->bytes + DATA, sizeof expected);
                    const unsigned encoded = state & 1, locked = state >> 1;
                    if (encoded) {
                        const uint16_t sum = expected_checksum(expected);
                        expected[6] = (uint8_t)sum;
                        expected[7] = (uint8_t)(sum >> 8);
                        px_encode_pokemon_manager(MANAGER, &f->memory);
                    }
                    store8(f, MANAGER + 0xd, (uint8_t)locked);
                    const unsigned block = 8 + ((row + 1) % 4) * 0x38;
                    const unsigned offset = field == PX_STATE_MOVE ? 0x1a + 2 * slot :
                                            field == PX_STATE_PP_UPS ? 0x26 + slot : 0x22 + slot;
                    expected[block + offset] = 0xd4;
                    if (field == PX_STATE_MOVE) expected[block + offset + 1] = 0xc3;
                    px_write_pokemon_state_slot(MANAGER, slot, (enum px_state_slot)field,
                                                UINT32_C(0xa1b2c3d4), &f->memory);
                    CHECK(px_read_pokemon_state_slot(MANAGER, slot, (enum px_state_slot)field,
                                                     &f->memory) == (field == PX_STATE_MOVE ? 0xc3d4u : 0xd4u));
                    CHECK(load8(f, MANAGER + 0xc) == (locked ? 0 : 1));
                    CHECK(load8(f, MANAGER + 0xd) == locked);
                    if (!locked) {
                        const uint16_t sum = expected_checksum(expected);
                        expected[6] = (uint8_t)sum;
                        expected[7] = (uint8_t)(sum >> 8);
                    }
                    px_decode_pokemon_manager(MANAGER, &f->memory);
                    CHECK(memcmp(f->bytes + DATA, expected, sizeof expected) == 0);
                    guards(f);
                }
    const uint32_t invalid[] = {4, 255, 256, UINT32_C(0x80000000), UINT32_MAX};
    for (unsigned i = 0; i < sizeof invalid / sizeof invalid[0]; ++i)
        for (unsigned field = 0; field < 3; ++field) {
            f->accesses = 0;
            CHECK(px_read_pokemon_state_slot(MANAGER, invalid[i], (enum px_state_slot)field,
                                             &f->memory) == 0);
            px_write_pokemon_state_slot(MANAGER, invalid[i], (enum px_state_slot)field,
                                        UINT32_MAX, &f->memory);
            CHECK(f->accesses == 0);
        }
    puts("PASS: 1536 slot field cases; 32 block rows; encoded/locked input; narrowing; invalid slot guards");
}
static void test_values_and_fallbacks(struct fixture *f)
{
    reset(f, 0);
    store8(f, MANAGER + 0xd, 1);
    store16(f, DATA + 8, UINT16_C(0xabcd));
    store8(f, DATA + 0x1d, UINT8_C(0xfe));
    store32(f, DATA + 0x10, UINT32_C(0xfedcba98));
    store16(f, EXTRA + 0xa, UINT16_C(0x9876));
    CHECK(px_read_pokemon_state_value(MANAGER, PX_STATE_SPECIES, &f->memory) == 0xabcd);
    CHECK(px_read_pokemon_state_value(MANAGER, PX_STATE_FORM, &f->memory) == 31);
    CHECK(px_read_pokemon_state_value(MANAGER, PX_STATE_GROWTH, &f->memory) == UINT32_C(0xfedcba98));
    CHECK(px_read_pokemon_state_value(MANAGER, PX_STATE_MAX_HP, &f->memory) == 0x9876);
    px_write_pokemon_state_value(MANAGER, PX_STATE_CURRENT_HP, UINT32_C(0x12345678), &f->memory);
    px_write_pokemon_state_value(MANAGER, PX_STATE_STATUS, UINT32_C(0xfedcba98), &f->memory);
    CHECK(load16(f, EXTRA + 8) == 0x5678);
    CHECK(load32(f, EXTRA) == UINT32_C(0xfedcba98));
    const uint32_t flags[] = {0, 1, 2, 3, UINT32_C(0x80000000), UINT32_MAX};
    for (unsigned i = 0; i < sizeof flags / sizeof flags[0]; ++i) {
        store32(f, DATA + 0x74, UINT32_C(0xa5a5a5a5));
        px_write_pokemon_state_value(MANAGER, PX_STATE_NAME_FLAG, flags[i], &f->memory);
        CHECK(load32(f, DATA + 0x74) == (UINT32_C(0x25a5a5a5) | ((flags[i] & 1) << 31)));
    }
    store16(f, DATA + 4, UINT16_C(4));
    store16(f, UINT32_C(0x00648284) + 0xa, UINT16_C(0x1357));
    store16(f, UINT32_C(0x006482a0), UINT16_C(0x2468));
    store8(f, UINT32_C(0x006482a0) + 0x15, UINT8_C(0xd9));
    store32(f, UINT32_C(0x006482a0) + 8, UINT32_C(0x88776655));
    CHECK(px_read_pokemon_state_value(MANAGER, PX_STATE_MAX_HP, &f->memory) == 0x1357);
    CHECK(px_read_pokemon_state_value(MANAGER, PX_STATE_SPECIES, &f->memory) == 0x2468);
    CHECK(px_read_pokemon_state_value(MANAGER, PX_STATE_FORM, &f->memory) == 27);
    CHECK(px_read_pokemon_state_value(MANAGER, PX_STATE_GROWTH, &f->memory) == UINT32_C(0x88776655));
    px_write_pokemon_state_value(MANAGER, PX_STATE_CURRENT_HP, UINT32_C(0xfedc1357), &f->memory);
    px_write_pokemon_state_value(MANAGER, PX_STATE_STATUS, UINT32_C(0xaabbccdd), &f->memory);
    CHECK(load16(f, UINT32_C(0x00648380) + 8) == 0x1357);
    CHECK(load32(f, UINT32_C(0x00648380)) == UINT32_C(0xaabbccdd));
    CHECK(load16(f, EXTRA + 8) == 0x5678 && load32(f, EXTRA) == UINT32_C(0xfedcba98));
    for (unsigned field = 0; field < 3; ++field) {
        const unsigned offset = field == PX_STATE_MOVE ? 0x20 : field == PX_STATE_PP_UPS ? 0x29 : 0x25;
        store16(f, UINT32_C(0x006482a0) + 0x38 + offset, UINT16_C(0x1234));
        CHECK(px_read_pokemon_state_slot(MANAGER, 3, (enum px_state_slot)field, &f->memory) ==
              (field == PX_STATE_MOVE ? 0x1234u : 0x34u));
        px_write_pokemon_state_slot(MANAGER, 3, (enum px_state_slot)field,
                                    UINT32_C(0x778899aa), &f->memory);
        CHECK(load8(f, UINT32_C(0x0064839c) + 0x38 + offset) == 0xaa);
        if (field == PX_STATE_MOVE)
            CHECK(load16(f, UINT32_C(0x0064839c) + 0x38 + offset) == 0x99aa);
    }
    store32(f, UINT32_C(0x0064839c) + 0x38 + 0x34, UINT32_C(0x7fffffff));
    px_write_pokemon_state_value(MANAGER, PX_STATE_NAME_FLAG, 1, &f->memory);
    CHECK(load32(f, UINT32_C(0x0064839c) + 0x38 + 0x34) == UINT32_MAX);
    store32(f, MANAGER + 4, 0);
    store8(f, MANAGER + 0xc, 0x81);
    CHECK(px_read_pokemon_state_value(MANAGER, PX_STATE_MAX_HP, &f->memory) == 0);
    px_write_pokemon_state_value(MANAGER, PX_STATE_CURRENT_HP, 7, &f->memory);
    px_write_pokemon_state_value(MANAGER, PX_STATE_STATUS, 9, &f->memory);
    CHECK(load8(f, MANAGER + 0xc) == 0x81);
    puts("PASS: direct value widths; name-bit mask; distinct corrupt read/write fallbacks; absent extra");
}
void test_pokemon_state_fields(void)
{
    struct fixture f = { .bytes = calloc(SIZE, 1) };
    CHECK(f.bytes != NULL);
    f.memory = (struct px_pokemon_api) {
        .user = &f, .load8 = load8, .load16 = load16, .load32 = load32,
        .store8 = store8, .store16 = store16, .store32 = store32
    };
    test_slots(&f);
    test_values_and_fallbacks(&f);
    free(f.bytes);
}
