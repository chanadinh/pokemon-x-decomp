#include "pokemon_data.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_pokemon_codec(void);
void test_pokemon_fields(void);
void test_pokemon_data_chain(void);

enum { MEMORY_SIZE = 0x650000, DATA = 0x1000, MANAGER = 0x2000,
       SETTINGS = 0x3000, GROWTH = 0x4000 };
struct fixture {
    uint8_t *bytes;
    uint8_t parameters[0x38];
    uint32_t species, form, attribute, growth_species, growth_form;
    unsigned selections, lookups, growth_selections, settings_reads;
    unsigned mutate;
    struct px_parameter_api personal;
    struct px_pokemon_api memory;
    struct px_data_api api;
};

static uint8_t load8(void *user, uint32_t address)
{
    struct fixture *f = user;
    assert(address < MEMORY_SIZE);
    return f->bytes[address];
}
static uint16_t load16(void *user, uint32_t address)
{
    return (uint16_t)((uint16_t)load8(user, address) |
                     (uint16_t)((uint16_t)load8(user, address + 1) << 8));
}
static uint32_t load32(void *user, uint32_t address)
{
    struct fixture *f = user;
    if (address == UINT32_C(0x005d1870)) ++f->settings_reads;
    return (uint32_t)load16(user, address) | (uint32_t)load16(user, address + 2) << 16;
}
static void store8(void *user, uint32_t address, uint8_t value)
{
    struct fixture *f = user;
    assert(address < MEMORY_SIZE);
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
static void put16(uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)value; p[1] = (uint8_t)(value >> 8);
}
static void put32(uint8_t *p, uint32_t value)
{
    put16(p, (uint16_t)value); put16(p + 2, (uint16_t)(value >> 16));
}
static void select_species(void *user, uint32_t species, uint32_t form)
{
    struct fixture *f = user;
    f->species = species; f->form = form; ++f->selections;
    if (f->mutate) {
        /* A resource callback changes later inputs; already-written fields
         * and the resolver's selected attribute must retain their old values. */
        put16(f->parameters + 0x18, UINT16_C(777));
        f->parameters[0x1a] = 9;
        f->parameters[0x22] = 2;
    }
}
static uint32_t personal_value(void *user, uint32_t attribute)
{
    struct fixture *f = user;
    f->attribute = attribute; ++f->lookups;
    return UINT32_C(0x12340040) + attribute;
}
static void select_growth(void *user, uint32_t species, uint32_t form)
{
    struct fixture *f = user;
    f->growth_species = species; f->growth_form = form; ++f->growth_selections;
    if (f->mutate) put16(f->parameters + 0x1c, UINT16_C(0x103));
}
static void reset(struct fixture *f)
{
    uint8_t *bytes = f->bytes;
    memset(bytes, 0, MEMORY_SIZE);
    memset(f, 0, sizeof(*f));
    f->bytes = bytes;
    f->personal = (struct px_parameter_api){ .user = f,
        .select_species = select_species, .personal_value = personal_value };
    f->memory = (struct px_pokemon_api){ .user = f, .parameters = &f->personal,
        .load8 = load8, .load16 = load16, .load32 = load32,
        .store8 = store8, .store16 = store16, .store32 = store32 };
    f->api = (struct px_data_api){ .memory = &f->memory, .user = f,
        .select_growth = select_growth };
    /* Synthetic valid rotations exercise all 32 lookup rows, not game assets. */
    for (unsigned row = 0; row < 32; ++row)
        for (unsigned column = 0; column < 4; ++column)
            store8(f, UINT32_C(0x00570eec) + 4 * row + column,
                   (uint8_t)((row + column) % 4));
    store32(f, UINT32_C(0x005d1870), SETTINGS);
    store32(f, UINT32_C(0x005d17ac), GROWTH);
    for (uint32_t level = 0; level <= 100; ++level)
        store32(f, GROWTH + 8 + 4 * level, UINT32_C(0x12340000) + level * level);
    for (unsigned i = 0; i < 4; ++i) store8(f, SETTINGS + 0x68 + i, (uint8_t)(0xaa + 0x11 * i));
    memset(bytes + DATA - 4, 0x75, 4);
    memset(bytes + DATA + 0xe8, 0x76, 4);
    memset(bytes + MANAGER - 4, 0x77, 4);
    memset(bytes + MANAGER + 0x10, 0x78, 4);
}
static void guard_check(struct fixture *f)
{
    for (unsigned i = 0; i < 4; ++i) {
        assert(load8(f, DATA - 4 + i) == 0x75);
        assert(load8(f, DATA + 0xe8 + i) == 0x76);
        assert(load8(f, MANAGER - 4 + i) == 0x77);
        assert(load8(f, MANAGER + 0x10 + i) == 0x78);
    }
}
static void prepare_parameters(struct fixture *f, unsigned row, unsigned slot)
{
    memset(f->parameters, 0xa5, sizeof(f->parameters));
    put32(f->parameters, UINT32_C(0x80000001) | (uint32_t)row << 13);
    put32(f->parameters + 8, UINT32_C(0x12345678));
    put32(f->parameters + 0x10, UINT32_C(0xfedcba98));
    put16(f->parameters + 0x18, UINT16_C(0x1234));
    f->parameters[0x1a] = 0x23;
    put16(f->parameters + 0x1c, UINT16_C(0x1ff));
    put16(f->parameters + 0x1e, UINT16_C(0xfffe));
    put16(f->parameters + 0x20, UINT16_C(0xabcd));
    f->parameters[0x22] = (uint8_t)slot;
    for (unsigned i = 0; i < 6; ++i) put16(f->parameters + 0x24 + 2 * i, (uint16_t)(0x1101 + i));
    put32(f->parameters + 0x30, UINT32_C(0xfedcba98));
}
static void check_population(struct fixture *f, unsigned row, unsigned slot, unsigned mutate)
{
    reset(f);
    prepare_parameters(f, row, slot);
    f->mutate = mutate;
    uint8_t original[0x38]; memcpy(original, f->parameters, sizeof(original));
    px_populate_pokemon_data(DATA, f->parameters, MANAGER, &f->api);
    assert(f->selections == 1 && f->lookups == 1 && f->growth_selections == 1);
    assert(f->species == 0x1234 && f->form == 0x23);
    assert(f->attribute == (slot == 1 ? 0x1bu : slot == 2 ? 0x1cu : 0x1au));
    assert(f->growth_species == (mutate ? 777u : 0x1234u));
    assert(f->growth_form == (mutate ? 9u : 0x23u));
    assert(f->settings_reads == 4);
    assert(load32(f, MANAGER) == UINT32_C(0x00598a78));
    assert(load32(f, MANAGER + 4) == 0 && load32(f, MANAGER + 8) == DATA);
    assert(load8(f, MANAGER + 0xc) == 1 && load8(f, MANAGER + 0xd) == 0);
    px_decode_pokemon_manager(MANAGER, &f->memory);
    assert(load16(f, DATA + 4) == 0); /* Correct checksum did not mark corruption. */
    uint8_t expected[0xe8] = {0};
    memcpy(expected, original, 4);
    uint8_t *a = expected + 8 + (row % 4) * 0x38;
    uint8_t *b = expected + 8 + ((row + 1) % 4) * 0x38;
    uint8_t *d = expected + 8 + ((row + 3) % 4) * 0x38;
    put16(a, 0x1234); put32(a + 4, UINT32_C(0xfedcba98));
    put32(a + 8, UINT32_C(0x12340000) + (mutate ? 9 : 10000));
    a[0xc] = (uint8_t)(0x40 + f->attribute);
    a[0xd] = mutate ? 4 : slot < 3 ? (uint8_t)(1u << slot) : 0;
    put32(a + 0x10, UINT32_C(0x12345678));
    a[0x14] = 0xcd; a[0x15] = 0x1c;
    put32(b + 0x34, UINT32_C(0x0a430c41));
    d[0x1a] = 0x98; d[0x2d] = 0x7f;
    d[0x30] = 0xbb; d[0x31] = 0xcc; d[0x32] = 0xdd; d[0x33] = 0xaa;
    uint32_t sum = 0;
    for (unsigned i = 8; i < sizeof(expected); i += 2)
        sum += (unsigned)expected[i] + ((unsigned)expected[i + 1] << 8);
    put16(expected + 6, (uint16_t)sum);
    assert(memcmp(f->bytes + DATA, expected, sizeof(expected)) == 0);
    if (!mutate) assert(memcmp(f->parameters, original, sizeof(original)) == 0);
    guard_check(f);
}
static void test_population(void)
{
    struct fixture f = { .bytes = calloc(MEMORY_SIZE, 1) };
    assert(f.bytes != NULL);
    reset(&f);
    f.api.select_growth = NULL; f.memory.parameters = NULL;
    px_populate_pokemon_data(DATA, f.parameters, MANAGER, &f.api);
    assert(f.selections == 0 && f.growth_selections == 0 && f.settings_reads == 0);
    assert(load8(&f, MANAGER + 0xc) == 1);
    assert(load16(&f, DATA + 6) == 0 && load32(&f, DATA + 8) != 0);
    px_decode_pokemon_manager(MANAGER, &f.memory);
    for (unsigned i = 0; i < 0xe8; ++i) assert(load8(&f, DATA + i) == 0);
    guard_check(&f);
    for (unsigned row = 0; row < 32; ++row)
        for (unsigned slot = 0; slot < 4; ++slot)
            check_population(&f, row, slot == 3 ? 255 : slot, 0);
    check_population(&f, 11, 1, 1);
    reset(&f);
    for (uint32_t slot = 0; slot < 256; ++slot) {
        uint32_t ability = px_resolve_pokemon_ability(UINT32_C(0xfedc1234), UINT32_C(0x12345678), slot, &f.api);
        assert(ability == (slot == 1 ? 0x5bu : slot == 2 ? 0x5cu : 0x5au));
        assert(f.species == UINT32_C(0xfedc1234) && f.form == UINT32_C(0x12345678));
    }
    assert(px_resolve_pokemon_ability(1, 2, UINT32_MAX, &f.api) == 0x5a);
    assert(px_resolve_pokemon_ability(1, 2, 257, &f.api) == 0x5a);
    const uint32_t levels[] = {0, 1, 99, 100, 101, 255, 256, UINT32_MAX};
    for (unsigned i = 0; i < sizeof(levels) / sizeof(levels[0]); ++i) {
        const uint32_t level = levels[i] > 100 ? 100 : levels[i];
        assert(px_pokemon_growth_value(levels[i], &f.api) == UINT32_C(0x12340000) + level * level);
    }
    free(f.bytes);
    puts("PASS: 130 population cases; species-zero encoding; 32 block-order rows; 258 ability slots; growth bounds; resource mutation; complete payload comparisons");
}

int main(void)
{
    test_pokemon_codec();
    test_pokemon_fields();
    test_population();
    test_pokemon_data_chain();
    puts("PASS: Pokemon data population, field setters, storage codec, and six-module integration");
    return 0;
}
