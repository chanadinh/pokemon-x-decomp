#include "pokemon_learnset.h"
#include "../pokemon_moves/pokemon_moves.h"
#include "../pokemon_state/pokemon_state.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { \
    fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #c); exit(EXIT_FAILURE); \
} } while (0)

enum { SIZE = 0x5e0000, SELECTED = 0x5d17b4, CACHE_CELL = 0x5d17b8,
       PERSONAL = 0x5d17e8, ARCHIVE = 0x5d17ec, RECORD = 0x300, RECORD2 = 0x320,
       CACHE = 0x500, CACHE2 = 0x520, TABLE = 0x600, BUFFER = 0x1000, BUFFER2 = 0x2000,
       BUFFER3 = 0x3000, OBJECT = 0x100, MANAGER = 0x200, DATA = 0x800 };
struct fixture {
    uint8_t *bytes;
    struct px_pokemon_api memory;
    struct px_learnset_api api;
    uint32_t species, form, first, forms, expected_index, terminator;
    uint32_t last_index, last_destination, last_halfword;
    unsigned selects, attributes, reads, mutation, integration;
};
static uint8_t load8(void *user, uint32_t address)
{
    struct fixture *f = user; CHECK(address < SIZE); return f->bytes[address];
}
static uint16_t load16(void *user, uint32_t address)
{
    struct fixture *f = user; f->last_halfword = address;
    return (uint16_t)((uint32_t)load8(user, address) | (uint32_t)load8(user, address + 1) << 8);
}
static uint32_t load32(void *user, uint32_t address)
{
    return (uint32_t)load16(user, address) | (uint32_t)load16(user, address + 2) << 16;
}
static void store8(void *user, uint32_t address, uint8_t value)
{
    struct fixture *f = user; CHECK(address < SIZE); f->bytes[address] = value;
}
static void store16(void *user, uint32_t address, uint16_t value)
{
    store8(user, address, (uint8_t)value); store8(user, address + 1, (uint8_t)(value >> 8));
}
static void store32(void *user, uint32_t address, uint32_t value)
{
    store16(user, address, (uint16_t)value); store16(user, address + 2, (uint16_t)(value >> 16));
}
static void select_personal(void *user, uint32_t record, uint32_t species, uint32_t form)
{
    struct fixture *f = user;
    CHECK(record == 0x700 && species == f->species && form == 0);
    ++f->selects;
    if (f->mutation == 1) store32(f, PERSONAL, 0x720);
}
static uint32_t personal_value(void *user, uint32_t record, uint32_t selector)
{
    struct fixture *f = user;
    CHECK(f->selects == 1);
    CHECK(record == (f->mutation == 1 ? (selector == 0x1e ? 0x720 : 0x740) : 0x700));
    CHECK(selector == (f->attributes == 0 ? 0x1e : 0x20));
    ++f->attributes;
    if (f->mutation == 1) {
        if (selector == 0x1e) store32(f, PERSONAL, 0x740);
        else { store32(f, ARCHIVE, 0x920); store32(f, RECORD + 4, BUFFER2); }
    }
    return selector == 0x1e ? f->first : f->forms;
}
static void fill_resource(struct fixture *f, uint32_t destination, unsigned count)
{
    for (unsigned i = 0; i < 26; ++i) {
        store16(f, destination + 4 * i, (uint16_t)(10 + i));
        store16(f, destination + 4 * i + 2, (uint16_t)(20 + i));
    }
    /* One halfword alone is not a terminator. */
    store16(f, destination, UINT16_MAX); store16(f, destination + 6, UINT16_MAX);
    if (count < 26) {
        store16(f, destination + 4 * count, UINT16_MAX);
        store16(f, destination + 4 * count + 2, UINT16_MAX);
    }
}
static void read_member(void *user, uint32_t archive, uint32_t index, uint32_t destination)
{
    struct fixture *f = user;
    CHECK(archive == (f->mutation == 1 ? 0x920 : 0x900));
    CHECK(index == f->expected_index);
    ++f->reads; f->last_index = index; f->last_destination = destination;
    fill_resource(f, destination, f->terminator);
    if (f->mutation == 1) {
        CHECK(destination == BUFFER2);
        fill_resource(f, BUFFER3, 7);
        store32(f, RECORD + 4, BUFFER3);
    } else if (f->mutation == 2) {
        store32(f, SELECTED, RECORD2); store32(f, CACHE_CELL, CACHE2);
    }
    if (f->integration) {
        static const uint16_t moves[] = {10, 11, 12, 13, 14, 11, 15, 16};
        for (unsigned i = 0; i < 8; ++i) {
            store16(f, destination + 4 * i, moves[i]);
            store16(f, destination + 4 * i + 2, (uint16_t)(i == 7 ? 10 : 1));
        }
    }
}
static void reset(struct fixture *f)
{
    memset(f->bytes, 0, 0x6000);
    f->selects = f->attributes = f->reads = f->mutation = f->integration = 0;
    f->species = 25; f->form = 0; f->first = 800; f->forms = 3; f->terminator = 8;
    f->expected_index = 25;
    f->memory = (struct px_pokemon_api){.user = f, .load8 = load8, .load16 = load16, .load32 = load32,
        .store8 = store8, .store16 = store16, .store32 = store32};
    f->api = (struct px_learnset_api){&f->memory, f, select_personal, personal_value, read_member};
    store32(f, SELECTED, RECORD); store32(f, CACHE_CELL, CACHE);
    store32(f, PERSONAL, 0x700); store32(f, ARCHIVE, 0x900);
    store16(f, RECORD, UINT16_MAX); store8(f, RECORD + 2, 0xff); store32(f, RECORD + 4, BUFFER);
    store16(f, RECORD2, UINT16_MAX); store8(f, RECORD2 + 2, 0xff); store32(f, RECORD2 + 4, BUFFER2);
    for (unsigned i = 0; i < 8; ++i) {
        const uint32_t r = 0x380 + 12 * i;
        store32(f, TABLE + 4 * i, r);
        store16(f, r, (uint16_t)(100 + i)); store8(f, r + 2, (uint8_t)i);
        store32(f, r + 4, 0x4000 + 104 * i);
        store8(f, r + 8, (uint8_t)(i + 1));
        for (unsigned k = 0; k < 104; ++k) store8(f, 0x4000 + 104 * i + k, (uint8_t)(17 * i + k));
    }
    store32(f, CACHE + 4, TABLE); store32(f, CACHE + 8, 4);
    store32(f, CACHE2 + 4, TABLE + 16); store32(f, CACHE2 + 8, 4);
}
static void accessor_cases(struct fixture *f)
{
    reset(f);
    for (unsigned i = 0; i < 256; ++i) {
        store16(f, BUFFER + 4 * i, (uint16_t)(0x8100 + i));
        store16(f, BUFFER + 4 * i + 2, (uint16_t)(0x9000 + i));
    }
    const uint32_t special[] = {UINT32_MAX, 0x10001, 0x80000001};
    unsigned cases = 0;
    for (unsigned count = 0; count < 256; ++count) {
        store8(f, RECORD + 8, (uint8_t)count);
        CHECK(px_current_learnset_count(&f->memory) == count);
        for (unsigned n = 0; n < 261; ++n) {
            const uint32_t index = n < 258 ? n : special[n - 258];
            const uint32_t selected = index < count ? index : 0;
            CHECK(px_current_learnset_move(index, &f->memory) == 0x8100 + selected);
            CHECK(f->last_halfword == BUFFER + 4 * selected);
            CHECK(px_current_learnset_level(index, &f->memory) == 0x9000 + selected);
            CHECK(f->last_halfword == BUFFER + 4 * selected + 2);
            ++cases;
        }
    }
    CHECK(cases == 66816);
    store32(f, SELECTED, RECORD2); store8(f, RECORD2 + 8, 1);
    store16(f, BUFFER2, 123); store16(f, BUFFER2 + 2, 456);
    CHECK(px_current_learnset_count(&f->memory) == 1);
    CHECK(px_current_learnset_move(0, &f->memory) == 123 && px_current_learnset_level(0, &f->memory) == 456);
}
static void loader_cases(struct fixture *f)
{
    const uint32_t species[] = {0, 1, 25, 721, 722, 723, 724, 0x10001, UINT32_MAX};
    const uint32_t form[] = {0, 1, 2, 3, 255, 256, UINT32_MAX};
    const uint32_t first[] = {0, 50, UINT32_MAX - 1};
    const uint32_t forms[] = {0, 1, 2, 4, UINT32_MAX};
    unsigned cases = 0;
    for (unsigned s = 0; s < 9; ++s) for (unsigned v = 0; v < 7; ++v)
        for (unsigned b = 0; b < 3; ++b) for (unsigned c = 0; c < 5; ++c)
            for (unsigned end = 0; end <= 26; ++end) {
                reset(f); f->species = species[s]; f->form = form[v];
                f->first = first[b]; f->forms = forms[c]; f->terminator = end;
                const int personal = f->species <= 721 && f->form != 0;
                f->expected_index = f->species > 721 ? 0 : f->species;
                if (personal && f->first && f->form < f->forms)
                    f->expected_index = f->first + f->form - 1;
                px_load_learnset_record(RECORD, f->species, f->form, &f->api);
                CHECK(f->selects == (unsigned)personal && f->attributes == 2 * (unsigned)personal);
                CHECK(f->reads == 1 && f->last_destination == BUFFER);
                CHECK(load8(f, RECORD + 8) == end);
                CHECK(load16(f, RECORD) == (uint16_t)f->species && load8(f, RECORD + 2) == (uint8_t)f->form);
                if (f->species <= UINT16_MAX && f->form <= UINT8_MAX) {
                    px_load_learnset_record(RECORD, f->species, f->form, &f->api);
                    CHECK(f->reads == 1); /* identity hit preserves data */
                }
                ++cases;
            }
    CHECK(cases == 25515);
    reset(f); f->mutation = 1; f->form = 1; f->expected_index = 800;
    px_load_learnset_record(RECORD, 25, 1, &f->api);
    CHECK(load8(f, RECORD + 8) == 7 && load32(f, RECORD + 4) == BUFFER3);
    CHECK(f->attributes == 2 && f->reads == 1 && f->last_destination == BUFFER2);
}
static void cache_cases(struct fixture *f)
{
    unsigned cases = 0;
    for (unsigned count = 1; count <= 8; ++count) for (unsigned cursor = 0; cursor < count; ++cursor) {
        for (unsigned hit = 0; hit < count; ++hit) {
            reset(f); store32(f, CACHE + 8, count); store32(f, CACHE + 12, cursor);
            memset(f->bytes + RECORD, 0xa5, 12); store32(f, RECORD + 4, BUFFER);
            CHECK(px_find_cached_learnset(CACHE, 100 + hit, hit, RECORD, &f->memory) == 1);
            CHECK(!memcmp(f->bytes + BUFFER, f->bytes + 0x4000 + 104 * hit, 104));
            CHECK(load16(f, RECORD) == 100 + hit && load8(f, RECORD + 2) == hit);
            CHECK(load8(f, RECORD + 8) == hit + 1 && load32(f, RECORD + 4) == BUFFER);
            CHECK(load8(f, RECORD + 3) == 0xa5 && load8(f, RECORD + 9) == 0xa5);
            CHECK(load32(f, CACHE + 12) == cursor);
            /* Registering an existing key does not overwrite its data. */
            memset(f->bytes + BUFFER, 0xee, 104);
            px_store_cached_learnset(CACHE, RECORD, &f->memory);
            CHECK(load8(f, 0x4000 + 104 * hit) == (uint8_t)(17 * hit));
            CHECK(load32(f, CACHE + 12) == cursor);
            CHECK(px_find_cached_learnset(CACHE, 0x10064 + hit, hit, RECORD, &f->memory) == 0);
            CHECK(px_find_cached_learnset(CACHE, 100 + hit, 0x100 + hit, RECORD, &f->memory) == 0);
            ++cases;
        }
        reset(f); store32(f, CACHE + 8, count); store32(f, CACHE + 12, cursor);
        store16(f, RECORD, 25); store8(f, RECORD + 2, 2); store8(f, RECORD + 8, 9);
        memset(f->bytes + BUFFER, 0xcc, 104);
        px_store_cached_learnset(CACHE, RECORD, &f->memory);
        const uint32_t target = 0x380 + 12 * cursor;
        CHECK(load16(f, target) == 25 && load8(f, target + 2) == 2 && load8(f, target + 8) == 9);
        CHECK(!memcmp(f->bytes + BUFFER, f->bytes + 0x4000 + 104 * cursor, 104));
        CHECK(load32(f, target + 4) == 0x4000 + 104 * cursor);
        CHECK(load32(f, CACHE + 12) == (cursor + 1) % count);
    }
    CHECK(cases == 204);
    reset(f); store32(f, CACHE + 8, 0); store32(f, CACHE + 4, UINT32_MAX);
    CHECK(px_find_cached_learnset(CACHE, 25, 0, RECORD, &f->memory) == 0);

    reset(f); px_select_pokemon_learnset(25, 0, &f->api);
    CHECK(f->reads == 1 && load32(f, CACHE + 12) == 1);
    /* The next selection is served from cache after invalidating the active key. */
    store16(f, RECORD, UINT16_MAX); memset(f->bytes + BUFFER, 0, 104);
    px_select_pokemon_learnset(25, 0, &f->api);
    CHECK(f->reads == 1 && load8(f, RECORD + 8) == 8 && load16(f, BUFFER) == UINT16_MAX);

    reset(f); f->mutation = 2;
    px_select_pokemon_learnset(25, 0, &f->api);
    CHECK(load32(f, SELECTED) == RECORD2 && load16(f, RECORD2) == UINT16_MAX);
    CHECK(load32(f, CACHE + 12) == 0 && load32(f, CACHE2 + 12) == 1);
    CHECK(load16(f, 0x380 + 4 * 12) == 25); /* original selected record, new cache */
}
static uint32_t state_level(void *user, uint32_t species, uint32_t form, uint32_t growth)
{
    (void)user; CHECK(species == 25 && form == 0 && growth == 100); return 5;
}
static void state_select(void *user, uint32_t species, uint32_t form)
{ struct fixture *f = user; px_select_pokemon_learnset(species, form, &f->api); }
static uint32_t state_count(void *user)
{ struct fixture *f = user; return px_current_learnset_count(&f->memory); }
static uint32_t state_entry_level(void *user, uint32_t index)
{ struct fixture *f = user; return px_current_learnset_level(index, &f->memory); }
static uint32_t state_move(void *user, uint32_t index)
{ struct fixture *f = user; return px_current_learnset_move(index, &f->memory); }
static uint32_t move_value(void *user, uint32_t move, uint32_t selector)
{ (void)user; CHECK(selector == 5); return 10 + move; }
static void state_learn(void *user, uint32_t object, uint32_t move)
{
    struct fixture *f = user;
    const struct px_move_api api = {&f->memory, f, move_value};
    px_pokemon_learn_move(object, move, &api);
}
static uint32_t state_pp(void *user, uint32_t move, uint32_t ups)
{
    struct fixture *f = user;
    const struct px_move_api api = {&f->memory, f, move_value};
    return px_pokemon_maximum_pp(move, ups, &api);
}
static void build_name(void *user, uint8_t *scratch, uint32_t species)
{ (void)user; CHECK(species == 25); memset(scratch, 0x42, PX_POKEMON_NAME_SCRATCH_SIZE); }
static void write_name(void *user, uint32_t manager, const uint8_t *scratch)
{ (void)user; CHECK(manager == MANAGER && scratch[0] == 0x42); }
static void base_state_integration(struct fixture *f)
{
    reset(f); f->integration = 1;
    store32(f, OBJECT + 12, MANAGER); store32(f, MANAGER + 8, DATA); store8(f, MANAGER + 13, 1);
    for (unsigned b = 0; b < 4; ++b) store8(f, 0x570eec + b, (uint8_t)b);
    store16(f, DATA + 8, 25); store32(f, DATA + 16, 100); store32(f, 0x5d1870, 0x900);
    const struct px_state_api state = {.memory = &f->memory, .user = f, .level_from_growth = state_level,
        .select_learnset = state_select, .learnset_count = state_count, .learnset_level = state_entry_level,
        .learnset_move = state_move, .learn_move = state_learn, .maximum_pp = state_pp,
        .build_name = build_name, .write_name = write_name};
    uint8_t scratch[PX_POKEMON_NAME_SCRATCH_SIZE];
    px_initialize_pokemon_base_state(OBJECT, scratch, &state);
    for (unsigned slot = 0; slot < 4; ++slot) {
        CHECK(px_read_pokemon_state_slot(MANAGER, slot, PX_STATE_MOVE, &f->memory) == 12 + slot);
        CHECK(px_read_pokemon_state_slot(MANAGER, slot, PX_STATE_PP, &f->memory) == 22 + slot);
    }
    CHECK(f->reads == 1 && load32(f, CACHE + 12) == 1);
    px_initialize_pokemon_base_state(OBJECT, scratch, &state);
    CHECK(f->reads == 1 && load32(f, CACHE + 12) == 1); /* constructor uses cache */
}
int main(void)
{
    struct fixture f = {0}; f.bytes = calloc(1, SIZE); CHECK(f.bytes);
    accessor_cases(&f); loader_cases(&f); cache_cases(&f); base_state_integration(&f);
    free(f.bytes);
    puts("pokemon_learnset: 66,816 accessor cases, 25,515 loader cases, cache ownership/replacement, callback mutation and base-state integration passed");
    return 0;
}
