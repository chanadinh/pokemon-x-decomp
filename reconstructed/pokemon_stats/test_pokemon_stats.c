#include "pokemon_stats.h"
#include "stats_math.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { \
    fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #c); \
    exit(EXIT_FAILURE); \
} } while (0)

enum { SIZE = 0x649000, OBJECT = 0x100, MANAGER = 0x200, MANAGER2 = 0x220,
       DATA = 0x400, EXTRA = 0x600, DATA2 = 0x800, EXTRA2 = 0xa00,
       GROWTH = 0x1000, GROWTH2 = 0x1400, NATURE = 0x570f6c };
struct fixture {
    uint8_t *bytes;
    struct px_parameter_api parameters;
    struct px_pokemon_api memory;
    struct px_data_api api;
    uint32_t base[6], species, form, growth_table;
    uint32_t nature_address;
    unsigned accesses, selects, attributes, growth_selects, growth_reads;
    unsigned mutation, expected_attribute;
};
static uint8_t load8(void *user, uint32_t address)
{
    struct fixture *f = user;
    CHECK(address < SIZE);
    ++f->accesses;
    if (address >= NATURE - 5 && address < NATURE + 5 * 300)
        f->nature_address = address;
    return f->bytes[address];
}
static uint16_t load16(void *user, uint32_t address)
{
    return (uint16_t)((uint32_t)load8(user, address) |
                      (uint32_t)load8(user, address + 1) << 8);
}
static uint32_t load32(void *user, uint32_t address)
{
    struct fixture *f = user;
    if (address == UINT32_C(0x005d17ac)) {
        ++f->growth_reads;
        return f->growth_table;
    }
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
static void select_species(void *user, uint32_t species, uint32_t form)
{
    struct fixture *f = user;
    CHECK(species == f->species && form == f->form);
    ++f->selects;
    if (f->mutation == 1) {
        /* Caller inputs have already been captured before resource selection. */
        store8(f, EXTRA + 4, 99);
        store32(f, DATA + 0x74, UINT32_MAX);
        memset(f->bytes + DATA + 0x1e, 0xff, 6);
        store8(f, DATA + 0x1c, 24);
    }
}
static uint32_t personal_value(void *user, uint32_t attribute)
{
    struct fixture *f = user;
    CHECK(attribute == f->expected_attribute && attribute < 6);
    CHECK(f->selects == f->attributes + 1);
    ++f->attributes;
    if (f->mutation == 1) store32(f, OBJECT + 0xc, MANAGER2);
    if (f->mutation == 4 && f->attributes == 1) {
        /* A resource callback can attach an extra buffer after old max HP
         * has entered its no-extra path but before current HP is selected. */
        const uint32_t original = f->base[attribute];
        f->base[attribute] = 95;
        store32(f, MANAGER + 4, EXTRA);
        return original;
    }
    return f->base[attribute];
}
static void select_growth(void *user, uint32_t species, uint32_t form)
{
    struct fixture *f = user;
    CHECK(species == f->species && form == f->form);
    ++f->growth_selects;
    if (f->mutation == 3) {
        f->growth_table = GROWTH2;
        store32(f, DATA + 0x10, UINT32_MAX);
    }
    if (f->mutation == 5) {
        store16(f, DATA + 8, 99);
        store8(f, DATA + 0x1d, 5 << 3);
        store32(f, DATA + 0x74, UINT32_C(31));
        store8(f, DATA + 0x1e, 252);
        f->form = 5;
    }
}
static void reset(struct fixture *f, unsigned row)
{
    memset(f->bytes + OBJECT, 0, 0x1800);
    memset(f->bytes + NATURE - 5, 0, 5 * 301);
    f->accesses = f->selects = f->attributes = f->growth_selects = f->growth_reads = 0;
    f->mutation = f->expected_attribute = 0;
    f->species = 25; f->form = 3; f->growth_table = GROWTH;
    f->parameters = (struct px_parameter_api) {
        .user = f, .select_species = select_species, .personal_value = personal_value
    };
    f->memory = (struct px_pokemon_api) {
        .user = f, .parameters = &f->parameters,
        .load8 = load8, .load16 = load16, .load32 = load32,
        .store8 = store8, .store16 = store16, .store32 = store32
    };
    f->api = (struct px_data_api) {&f->memory, f, select_growth};
    store32(f, OBJECT + 0xc, MANAGER);
    for (unsigned i = 0; i < 2; ++i) {
        store32(f, MANAGER + 0x20 * i + 4, EXTRA + 0x400 * i);
        store32(f, MANAGER + 0x20 * i + 8, DATA + 0x400 * i);
        store8(f, MANAGER + 0x20 * i + 0xd, 1);
    }
    for (unsigned block = 0; block < 4; ++block)
        store8(f, UINT32_C(0x00570eec) + 4 * row + block, (uint8_t)((row + block) % 4));
    store32(f, DATA, row << 13);
    const uint32_t a = DATA + 8 + (row % 4) * 0x38;
    const uint32_t b = DATA + 8 + ((row + 1) % 4) * 0x38;
    store16(f, a, 25);
    store8(f, a + 0x15, 3 << 3);
    store8(f, a + 0x14, 17);
    store32(f, a + 8, 20000);
    /* IVs HP/Atk/Def/Speed/SpA/SpD = 2/5/8/11/14/17. */
    store32(f, b + 0x34, UINT32_C(0x22e5a0a2));
    for (unsigned stat = 0; stat < 6; ++stat) {
        store8(f, a + 0x16 + stat, (uint8_t)(4 + 4 * stat));
        f->base[stat] = UINT32_C(0x12340000) + 45 + 5 * stat;
    }
    store8(f, EXTRA + 4, 20);
    store16(f, EXTRA + 8, 7);
    store16(f, EXTRA + 0xa, 48);
    memcpy(f->bytes + DATA2, f->bytes + DATA, PX_POKEMON_DATA_SIZE);
    memcpy(f->bytes + EXTRA2, f->bytes + EXTRA, PX_POKEMON_EXTRA_SIZE);
    for (unsigned level = 0; level <= 100; ++level) {
        store32(f, GROWTH + 8 + 4 * level, level * 1000);
        store32(f, GROWTH2 + 8 + 4 * level, level * 2000);
    }
}

static void test_math(struct fixture *f)
{
    static const struct { uint32_t species, level, base, iv, ev, expected; } hp[] = {
        {1, 20, 45, 2, 0, 48}, {292, UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, 1},
        {1, 100, 255, 31, 255, 714}, {1, 0, 65535, 31, 255, 10},
        {1, 255, 65535, 65535, 65535, 19096},
        {1, UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, 34087},
        {1, 1, 0, UINT32_MAX, 0, 23603}
    };
    for (unsigned i = 0; i < sizeof hp / sizeof hp[0]; ++i)
        CHECK(px_pokemon_hp_value(hp[i].species, hp[i].level, hp[i].base,
                                   hp[i].iv, hp[i].ev) == hp[i].expected);
    reset(f, 0);
    static const struct { uint32_t value, down, up; } nature[] = {
        {0, 0, 0}, {1, 0, 1}, {5, 4, 5}, {25, 22, 27}, {33, 29, 36},
        {100, 90, 110}, {595, 535, 654}, {596, 536, 0},
        {65535, 654, 654}, {65536, 0, 0}, {UINT32_MAX, 654, 654}
    };
    for (unsigned i = 0; i < sizeof nature / sizeof nature[0]; ++i) {
        store8(f, NATURE + 5 * 17, 255);
        store8(f, NATURE + 5 * 17 + 1, 1);
        CHECK(px_apply_pokemon_nature(nature[i].value, 17, 1, &f->memory) == nature[i].down);
        CHECK(px_apply_pokemon_nature(nature[i].value, 17, 2, &f->memory) == nature[i].up);
        for (unsigned column = 3; column <= 5; ++column) {
            store8(f, NATURE + 5 * 17 + column - 1, column == 3 ? 0 : column == 4 ? 2 : 128);
            CHECK(px_apply_pokemon_nature(nature[i].value, 17, column, &f->memory) == nature[i].value);
        }
    }
    const uint32_t invalid[] = {0, 6, 255, UINT32_MAX};
    for (unsigned i = 0; i < sizeof invalid / sizeof invalid[0]; ++i) {
        f->accesses = 0;
        CHECK(px_apply_pokemon_nature(UINT32_C(0xfedcba98), 17, invalid[i], &f->memory) == UINT32_C(0xfedcba98));
        CHECK(f->accesses == 0);
    }
    store8(f, NATURE + 5 * 257 + 4, 1);
    CHECK(px_apply_pokemon_nature(100, 257, 5, &f->memory) == 110);
    CHECK(f->nature_address == NATURE + 5 * 257 + 4);
    store8(f, NATURE - 5, 1);
    CHECK(px_apply_pokemon_nature(100, UINT32_MAX, 1, &f->memory) == 110);
    CHECK(f->nature_address == NATURE - 5);
    static const struct { uint32_t level, base, iv, ev, expected; } stats[] = {
        {20, 49, 5, 0, 25}, {100, 255, 31, 255, 609},
        {255, 65535, 65535, 65535, 18836},
        {UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, 34083},
        /* The reciprocal produces 42949672 for this product, while ordinary
         * integer division by 100 gives 42949671. */
        {1, 0, UINT32_C(4294967199), 0, 23597}
    };
    for (unsigned stat = 1; stat < 6; ++stat)
        for (unsigned i = 0; i < sizeof stats / sizeof stats[0]; ++i)
            CHECK(px_pokemon_stat_value(stats[i].level, stats[i].base, stats[i].iv,
                                         stats[i].ev, 0, (enum px_stat)stat, &f->memory) == stats[i].expected);
    /* Enum order differs from the nature table's five columns. */
    const uint8_t columns[5] = {1, 255, 0, 2, 128};
    memcpy(f->bytes + NATURE + 5 * 17, columns, sizeof columns);
    const unsigned expected[5] = {27, 22, 25, 25, 25};
    for (unsigned stat = 1; stat < 6; ++stat)
        CHECK(px_pokemon_stat_value(20, 49, 5, 0, 17, (enum px_stat)stat,
                                    &f->memory) == expected[stat - 1]);
    const unsigned stat_columns[5] = {1, 2, 5, 3, 4};
    for (unsigned column = 1; column <= 5; ++column) {
        memset(f->bytes + NATURE + 5 * 17, 0, 5);
        store8(f, NATURE + 5 * 17 + column - 1, 1);
        for (unsigned stat = 1; stat < 6; ++stat)
            CHECK(px_pokemon_stat_value(20, 49, 5, 0, 17, (enum px_stat)stat,
                                        &f->memory) == (stat_columns[stat - 1] == column ? 27u : 25u));
    }
    puts("PASS: fixed HP/stat/nature arithmetic; reciprocal boundary; overflow; signed table bytes; full-width nature indices");
}

static void test_growth(struct fixture *f)
{
    const uint32_t values[] = {0, 999, 1000, 1999, 2000, 99999, 100000, UINT32_MAX};
    const unsigned levels[] = {1, 1, 1, 1, 2, 99, 100, 100};
    const unsigned reads[] = {1, 1, 2, 2, 3, 100, 100, 100};
    for (unsigned i = 0; i < sizeof values / sizeof values[0]; ++i) {
        reset(f, 0);
        CHECK(px_pokemon_level_from_growth(25, 3, values[i], &f->api) == levels[i]);
        CHECK(f->growth_selects == 1 && f->growth_reads == reads[i]);
    }
    reset(f, 0);
    store32(f, GROWTH + 12, UINT32_C(0x80000000));
    store32(f, GROWTH + 16, UINT32_MAX);
    CHECK(px_pokemon_level_from_growth(25, 3, UINT32_C(0x7fffffff), &f->api) == 1);
    CHECK(f->growth_reads == 1);
    CHECK(px_pokemon_level_from_growth(25, 3, UINT32_C(0x80000000), &f->api) == 1);
    CHECK(f->growth_reads == 3);
    reset(f, 0);
    f->mutation = 3;
    CHECK(px_pokemon_level_from_growth(25, 3, 20000, &f->api) == 10);
    CHECK(f->growth_reads == 11 && f->growth_selects == 1);
    puts("PASS: growth levels1..100; exact threshold boundaries; unsigned growth; selected resource mutation");
}

static void test_fields(struct fixture *f)
{
    for (unsigned row = 0; row < 32; ++row)
        for (unsigned state = 0; state < 4; ++state) {
            reset(f, row);
            store8(f, EXTRA - 1, 0xa5);
            store8(f, EXTRA + PX_POKEMON_EXTRA_SIZE, 0xa5);
            store8(f, MANAGER + 0xd, 0);
            if (state & 1) px_encode_pokemon_manager(MANAGER, &f->memory);
            store8(f, MANAGER + 0xd, (uint8_t)(state >> 1));
            CHECK(px_pokemon_has_extra(MANAGER, &f->memory) == 1);
            CHECK(px_read_pokemon_nature(MANAGER, &f->memory) == 17);
            CHECK(px_read_pokemon_extra_level(MANAGER, &f->memory) == 20);
            CHECK(px_read_pokemon_current_hp(MANAGER, &f->memory) == 7);
            for (unsigned stat = 0; stat < 6; ++stat) {
                CHECK(px_read_pokemon_iv(MANAGER, (enum px_stat)stat, &f->memory) == 2 + 3 * stat);
                CHECK(px_read_pokemon_ev(MANAGER, (enum px_stat)stat, &f->memory) == 4 + 4 * stat);
                px_write_pokemon_stat(MANAGER, (enum px_stat)stat,
                                      UINT32_C(0xfedc1234) + stat, &f->memory);
            }
            CHECK(load8(f, MANAGER + 0xc) == (state >> 1 ? 0 : 1));
            CHECK(load8(f, MANAGER + 0xd) == state >> 1);
            px_decode_pokemon_manager(MANAGER, &f->memory);
            static const uint8_t expected[PX_POKEMON_EXTRA_SIZE] = {
                [4] = 20, [8] = 7, [0xa] = 0x34, [0xb] = 0x12,
                [0xc] = 0x35, [0xd] = 0x12, [0xe] = 0x36, [0xf] = 0x12,
                [0x10] = 0x37, [0x11] = 0x12, [0x12] = 0x38, [0x13] = 0x12,
                [0x14] = 0x39, [0x15] = 0x12
            };
            CHECK(memcmp(f->bytes + EXTRA, expected, sizeof expected) == 0);
            CHECK(load8(f, EXTRA - 1) == 0xa5 && load8(f, EXTRA + PX_POKEMON_EXTRA_SIZE) == 0xa5);
        }
    reset(f, 0);
    store16(f, DATA + 4, 4);
    store8(f, UINT32_C(0x00648284) + 4, 231);
    store16(f, UINT32_C(0x00648284) + 8, UINT16_C(0x9876));
    store8(f, UINT32_C(0x006482a0) + 0x14, 255);
    store32(f, UINT32_C(0x006482d8) + 0x34, UINT32_C(0xc410c41f));
    CHECK(px_read_pokemon_extra_level(MANAGER, &f->memory) == 231);
    CHECK(px_read_pokemon_current_hp(MANAGER, &f->memory) == 0x9876);
    CHECK(px_read_pokemon_nature(MANAGER, &f->memory) == 255);
    const unsigned fallback_iv[6] = {31, 0, 17, 1, 1, 2};
    for (unsigned stat = 0; stat < 6; ++stat) {
        store8(f, UINT32_C(0x006482a0) + 0x16 + stat, (uint8_t)(250 + stat));
        CHECK(px_read_pokemon_iv(MANAGER, (enum px_stat)stat, &f->memory) == fallback_iv[stat]);
        CHECK(px_read_pokemon_ev(MANAGER, (enum px_stat)stat, &f->memory) == 250 + stat);
        px_write_pokemon_stat(MANAGER, (enum px_stat)stat, UINT32_C(0xfedcba98) + stat, &f->memory);
        CHECK(load16(f, UINT32_C(0x00648380) + 0xa + 2 * stat) == 0xba98 + stat);
    }
    store32(f, MANAGER + 4, 0);
    store8(f, MANAGER + 0xc, 0x81);
    CHECK(px_pokemon_has_extra(MANAGER, &f->memory) == 0);
    CHECK(px_read_pokemon_extra_level(MANAGER, &f->memory) == 0);
    CHECK(px_read_pokemon_current_hp(MANAGER, &f->memory) == 0);
    for (unsigned stat = 0; stat < 6; ++stat)
        px_write_pokemon_stat(MANAGER, (enum px_stat)stat, 0, &f->memory);
    CHECK(load8(f, MANAGER + 0xc) == 0x81);
    f->accesses = 0;
    CHECK(px_read_pokemon_iv(MANAGER, (enum px_stat)6, &f->memory) == 0);
    CHECK(px_read_pokemon_ev(MANAGER, (enum px_stat)-1, &f->memory) == 0);
    px_write_pokemon_stat(MANAGER, (enum px_stat)6, 1, &f->memory);
    CHECK(f->accesses == 0);
    puts("PASS: 768 six-stat field cases;32 block rows; codec states; complete extra buffers; fallback and absent-extra guards");
}

static void test_updates(struct fixture *f)
{
    static const unsigned attributes[5] = {1, 2, 4, 5, 3};
    static const unsigned expected[5] = {28, 26, 34, 40, 28};
    static const uint8_t nature[5] = {1, 255, 0, 1, 255};
    for (unsigned row = 0; row < 32; ++row)
        for (unsigned stat = 0; stat < 5; ++stat) {
            reset(f, row);
            memcpy(f->bytes + NATURE + 17 * 5, nature, sizeof nature);
            f->expected_attribute = attributes[stat];
            px_update_pokemon_stat(OBJECT, (enum px_state_stat)stat, &f->api);
            CHECK(f->selects == 1 && f->attributes == 1 && f->growth_selects == 0);
            CHECK(load16(f, EXTRA + 0xa + 2 * attributes[stat]) == expected[stat]);
        }
    for (unsigned stat = 0; stat < 5; ++stat) {
        reset(f, 0);
        memcpy(f->bytes + NATURE + 17 * 5, nature, sizeof nature);
        f->expected_attribute = attributes[stat];
        f->mutation = 1;
        store16(f, EXTRA + 0xa + 2 * attributes[stat], UINT16_C(0xa5a5));
        px_update_pokemon_stat(OBJECT, (enum px_state_stat)stat, &f->api);
        CHECK(load16(f, EXTRA2 + 0xa + 2 * attributes[stat]) == expected[stat]);
        CHECK(load16(f, EXTRA + 0xa + 2 * attributes[stat]) == UINT16_C(0xa5a5));
        CHECK(f->selects == 1 && f->attributes == 1);
    }
    static const struct { uint16_t old_max, current; uint32_t mode; uint16_t expected; } hp[] = {
        {40, 7, 0, 15}, {60, 7, 0, 7}, {60, 55, 0, 48}, {40, 0, 0, 8},
        {40, 0, 1, 0}, {40, 0, UINT32_C(0x80000000), 0},
        {40, 65530, 1, 2}, {48, 7, 0, 7}
    };
    for (unsigned i = 0; i < sizeof hp / sizeof hp[0]; ++i) {
        reset(f, 0);
        store16(f, EXTRA + 0xa, hp[i].old_max);
        store16(f, EXTRA + 8, hp[i].current);
        px_update_pokemon_hp(OBJECT, hp[i].mode, &f->api);
        CHECK(load16(f, EXTRA + 0xa) == 48);
        CHECK(load16(f, EXTRA + 8) == hp[i].expected);
        CHECK(f->selects == 1 && f->attributes == 1 && f->growth_selects == 0);
    }
    reset(f, 0);
    f->species = 292;
    store16(f, DATA + 8, 292);
    px_update_pokemon_hp(OBJECT, 0, &f->api);
    CHECK(load16(f, EXTRA + 0xa) == 1 && load16(f, EXTRA + 8) == 1);
    CHECK(f->selects == 1 && f->attributes == 1);
    reset(f, 0);
    f->mutation = 1;
    store16(f, EXTRA2 + 8, 0xaaaa);
    px_update_pokemon_hp(OBJECT, 0, &f->api);
    CHECK(load16(f, EXTRA + 0xa) == 48 && load16(f, EXTRA + 8) == 7);
    CHECK(load16(f, EXTRA2 + 0xa) == 48 && load16(f, EXTRA2 + 8) == 7);
    CHECK(load8(f, EXTRA + 4) == 99);
    f->accesses = 0;
    px_update_pokemon_stat(OBJECT, (enum px_state_stat)5, &f->api);
    CHECK(f->accesses == 0);
    puts("PASS: all five stat updates; captured inputs and manager replacement; HP rise/fall/faint/mode/overflow cases");
}

static void test_object_helpers(struct fixture *f)
{
    reset(f, 0);
    CHECK(px_pokemon_object_level(OBJECT, &f->api) == 20);
    CHECK(px_pokemon_object_max_hp(OBJECT, &f->api) == 48);
    CHECK(f->growth_selects == 0 && f->selects == 0);
    store32(f, MANAGER + 4, 0);
    CHECK(px_pokemon_object_level(OBJECT, &f->api) == 20);
    CHECK(f->growth_selects == 1 && f->growth_reads == 21);
    CHECK(px_pokemon_object_max_hp(OBJECT, &f->api) == 48);
    CHECK(f->growth_selects == 2 && f->selects == 1);
    reset(f, 0);
    store32(f, MANAGER + 4, 0);
    f->expected_attribute = 1;
    px_update_pokemon_stat(OBJECT, PX_STATE_STAT_0014FB00, &f->api);
    CHECK(f->growth_selects == 1 && f->selects == 1 && f->attributes == 1);
    CHECK(load16(f, EXTRA + 0xc) == 0);
    reset(f, 0);
    store32(f, MANAGER + 4, 0);
    px_update_pokemon_hp(OBJECT, 0, &f->api);
    /* No extra buffer: old max, captured current, new max, final max each
     * perform their own level and personal-resource computation. */
    CHECK(f->growth_selects == 4 && f->selects == 4 && f->attributes == 4);
    CHECK(load16(f, EXTRA + 8) == 7 && load16(f, EXTRA + 0xa) == 48);
    reset(f, 0);
    store32(f, MANAGER + 4, 0);
    f->mutation = 4;
    px_update_pokemon_hp(OBJECT, 0, &f->api);
    CHECK(f->growth_selects == 1 && f->selects == 2 && f->attributes == 2);
    CHECK(load16(f, EXTRA + 0xa) == 68 && load16(f, EXTRA + 8) == 27);
    reset(f, 0);
    store32(f, MANAGER + 4, 0);
    f->mutation = 5;
    CHECK(px_pokemon_object_max_hp(OBJECT, &f->api) == 66);
    CHECK(f->growth_selects == 1 && f->selects == 1 && f->attributes == 1);
    puts("PASS: object cached/fallback level and max HP; repeated no-extra resource computations");
}

void test_pokemon_stats_chain(void);
int main(void)
{
    struct fixture f = {.bytes = calloc(SIZE, 1)};
    CHECK(f.bytes != NULL);
    test_math(&f);
    test_growth(&f);
    test_fields(&f);
    test_updates(&f);
    test_object_helpers(&f);
    free(f.bytes);
    test_pokemon_stats_chain();
    puts("PASS: Pokemon stats reconstruction and eight-module integration");
    return 0;
}
