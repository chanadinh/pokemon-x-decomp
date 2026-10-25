#include "pokemon_names.h"
#include "../pokemon_state/pokemon_state.h"
#include "../pokemon_moves/pokemon_moves.h"
#include "../pokemon_learnset/pokemon_learnset.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); exit(1); } } while (0)
enum { SIZE = 0x649000, OBJECT = 0x100, MANAGER = 0x200, DATA = 0x300, EXTRA = 0x800,
       STRING = 0x500, STRING2 = 0x520, TEXT = 0x600, TEXT2 = 0x640, SCRATCH = 0x700,
       SOURCE = 0x10000, DEST = 0x40000, PERSONAL = 0x200000, CURRENT = 0x5d1790,
       TABLE = 0x570eec, FALLBACK = 0x6483d4 };
struct fixture {
    uint8_t *bytes;
    struct px_pokemon_api memory;
    struct px_name_api names;
    uint32_t index, result;
    unsigned loads, mutation, table_reads, personal_reads;
};
static uint8_t load8(void *user, uint32_t address)
{ struct fixture *f = user; CHECK(address < SIZE); return f->bytes[address]; }
static uint16_t load16(void *user, uint32_t address)
{ return (uint16_t)((uint32_t)load8(user, address) | (uint32_t)load8(user, address + 1) << 8); }
static uint32_t load32(void *user, uint32_t address)
{
    struct fixture *f = user;
    if (address == 0x5d177c) ++f->table_reads;
    if (address >= PERSONAL && address < PERSONAL + 724 * 64) ++f->personal_reads;
    return (uint32_t)load16(user, address) | (uint32_t)load16(user, address + 2) << 16;
}
static void store8(void *user, uint32_t address, uint8_t value)
{ struct fixture *f = user; CHECK(address < SIZE); f->bytes[address] = value; }
static void store16(void *user, uint32_t address, uint16_t value)
{ store8(user, address, (uint8_t)value); store8(user, address + 1, (uint8_t)(value >> 8)); }
static void store32(void *user, uint32_t address, uint32_t value)
{ store16(user, address, (uint16_t)value); store16(user, address + 2, (uint16_t)(value >> 16)); }
static uint32_t text_load(void *user, uint32_t resource, uint32_t index, uint32_t string)
{
    struct fixture *f = user; CHECK(resource == 0x900 && index == f->index && string == STRING); ++f->loads;
    store16(f, TEXT, 'N'); store16(f, TEXT + 2, 'a'); store16(f, TEXT + 4, 'm');
    store16(f, TEXT + 6, 'e'); store16(f, TEXT + 8, 0);
    if (f->mutation) { store32(f, CURRENT, STRING2); store16(f, TEXT2, 0x2640); store16(f, TEXT2 + 2, 0); }
    return f->result;
}
static void reset(struct fixture *f)
{
    memset(f->bytes, 0, 0x6000);
    f->loads = f->mutation = f->table_reads = f->personal_reads = 0; f->result = 0x1234;
    f->memory = (struct px_pokemon_api){.user = f, .load8 = load8, .load16 = load16, .load32 = load32,
        .store8 = store8, .store16 = store16, .store32 = store32};
    f->names = (struct px_name_api){&f->memory, f, text_load};
    store32(f, CURRENT, STRING); store32(f, CURRENT + 4, 0x900);
    store32(f, STRING + 4, TEXT); store32(f, STRING2 + 4, TEXT2);
    store32(f, 0x5d177c, PERSONAL);
    store16(f, TEXT, 'O'); store16(f, TEXT + 2, 'l'); store16(f, TEXT + 4, 'd'); store16(f, TEXT + 6, 0);
}
static void copy_cases(struct fixture *f)
{
    reset(f);
    for (unsigned unit = 0; unit <= UINT16_MAX; ++unit) {
        if (unit == 0x10) continue;
        store16(f, SOURCE, (uint16_t)unit); store16(f, SOURCE + 2, 0);
        memset(f->bytes + DEST, 0xaa, 8);
        CHECK(px_copy_game_string(DEST, SOURCE, &f->memory) == (unit ? 1 : 0));
        CHECK(load16(f, DEST) == unit && load16(f, DEST + (unit ? 4 : 2)) == 0xaaaa);
        if (unit) CHECK(load16(f, DEST + 2) == 0);
    }
    for (unsigned length = 0; length <= 2050; ++length) {
        for (unsigned i = 0; i < length; ++i) store16(f, SOURCE + 2 * i, (uint16_t)(0x410 + i % 100));
        store16(f, SOURCE + 2 * length, 0); memset(f->bytes + DEST, 0xa5, 2 * 2053);
        const unsigned copied = length < 2048 ? length : 2048;
        CHECK(px_copy_game_string(DEST, SOURCE, &f->memory) == (length < 2048 ? length : 2047));
        CHECK(!memcmp(f->bytes + DEST, f->bytes + SOURCE, 2 * copied));
        CHECK(load16(f, DEST + 2 * copied) == 0 && load16(f, DEST + 2 * copied + 2) == 0xa5a5);
    }
    /* Control arguments containing zeros are copied in full; the following
     * unit is copied too, before checking for termination or the guard. */
    const unsigned counts[] = {0, 1, 2, 3, 4, 15, 16, 17, 2043, 2044, 2045, 2046, 2047, 2048, 65534, 65535};
    const unsigned prefixes[] = {0, 1, 2046, 2047};
    for (unsigned c = 0; c < 16; ++c) for (unsigned p = 0; p < 4; ++p) for (unsigned follows = 0; follows < 2; ++follows) {
        const unsigned prefix = prefixes[p], span = counts[c] + 2;
        for (unsigned i = 0; i < prefix; ++i) store16(f, SOURCE + 2 * i, 'A');
        store16(f, SOURCE + 2 * prefix, 0x10); store16(f, SOURCE + 2 * (prefix + 1), (uint16_t)counts[c]);
        for (unsigned i = 2; i < span; ++i) store16(f, SOURCE + 2 * (prefix + i), (uint16_t)(i % 3 ? 0 : 0xff00));
        store16(f, SOURCE + 2 * (prefix + span), follows ? 'Z' : 0);
        store16(f, SOURCE + 2 * (prefix + span + 1), 0);
        const unsigned length = prefix + span + follows;
        memset(f->bytes + DEST, 0xcc, 2 * (length + 2));
        const unsigned expected = follows && length >= 2048 ? length - 1 : length;
        CHECK(px_copy_game_string(DEST, SOURCE, &f->memory) == expected);
        CHECK(!memcmp(f->bytes + DEST, f->bytes + SOURCE, 2 * length));
        CHECK(load16(f, DEST + 2 * length) == 0 && load16(f, DEST + 2 * length + 2) == 0xcccc);
    }
    /* The unit following a token is not interpreted as another token. */
    const uint16_t adjacent[] = {0x10, 1, 0xff00, 0x10, 2, 0, 0x9999, 0};
    for (unsigned i = 0; i < 8; ++i) store16(f, SOURCE + 2 * i, adjacent[i]);
    memset(f->bytes + DEST, 0xaa, 20);
    CHECK(px_copy_game_string(DEST, SOURCE, &f->memory) == 5);
    CHECK(!memcmp(f->bytes + SOURCE, f->bytes + DEST, 12) && load16(f, DEST + 12) == 0xaaaa);
    store16(f, SOURCE, 'X'); store16(f, SOURCE + 2, 0);
    CHECK(px_copy_game_string(SOURCE, SOURCE, &f->memory) == 1);
    /* Forward overlap propagates X over the source terminator. */
    CHECK(px_copy_game_string(SOURCE + 2, SOURCE, &f->memory) == 2047);
    for (unsigned i = 0; i < 2048; ++i) CHECK(load16(f, SOURCE + 2 + 2 * i) == 'X');
    CHECK(load16(f, SOURCE + 2 + 4096) == 0);
}
static void index_cases(struct fixture *f)
{
    const uint32_t species[] = {0, 1, 25, 721, 722, 723, 724, 0x10001, UINT32_MAX};
    const uint32_t forms[] = {0, 1, 2, 255, 256, 0x10001, UINT32_MAX};
    const unsigned first[] = {0, 17, 65535}, counts[] = {0, 1, 2, 255, 0x10004};
    unsigned cases = 0;
    for (unsigned s = 0; s < 9; ++s) for (unsigned v = 0; v < 7; ++v)
        for (unsigned b = 0; b < 3; ++b) for (unsigned c = 0; c < 5; ++c) {
            reset(f); const uint32_t sp = species[s], form = forms[v];
            if (sp <= 721) {
                store32(f, PERSONAL + sp * 64 + 0x1c, 0xabcd0000u | first[b]);
                store32(f, PERSONAL + sp * 64 + 0x20, counts[c]);
            }
            uint32_t expected = sp <= 721 ? sp : 0;
            if (sp <= 721 && form && first[b] && form < (counts[c] & 255)) expected = first[b] + form - 1;
            CHECK(px_pokemon_form_index(sp, form, &f->memory) == expected);
            CHECK(f->table_reads == (unsigned)(sp <= 721 && form != 0));
            CHECK(f->personal_reads == 2 * f->table_reads); ++cases;
        }
    CHECK(cases == 945);
    CHECK(px_pokemon_form_index(25, 0, NULL) == 25 && px_pokemon_form_index(UINT32_MAX, 1, NULL) == 0);
}
static void builder_cases(struct fixture *f)
{
    for (unsigned species = 0; species <= 725; ++species) {
        reset(f); f->index = species <= 721 ? species : 0;
        memset(f->bytes + SCRATCH, 0xcd, 32);
        CHECK(px_build_pokemon_name(SCRATCH, species, &f->names) == (species < 724 ? 0x1234u : 0));
        CHECK(f->loads == (species < 724));
        CHECK(load16(f, SCRATCH) == (species < 724 ? 'N' : 'O'));
        CHECK(load16(f, SCRATCH + (species < 724 ? 10 : 8)) == 0xcdcd);
    }
    reset(f); f->index = 25; f->mutation = 1; f->result = 0xfedcba98;
    CHECK(px_build_pokemon_name(SCRATCH, 25, &f->names) == 0xfedcba98);
    CHECK(load16(f, SCRATCH) == 0x2640 && load16(f, SCRATCH + 2) == 0);
    reset(f); CHECK(px_build_pokemon_name(SCRATCH, 0x10001, &f->names) == 0 && f->loads == 0);
    CHECK(load16(f, SCRATCH) == 'O');
}
static void writer_cases(struct fixture *f)
{
    unsigned cases = 0;
    for (unsigned row = 0; row < 32; ++row) for (unsigned length = 0; length <= 12; ++length)
        for (unsigned state = 0; state < 4; ++state) for (unsigned corrupt = 0; corrupt < 2; ++corrupt) {
            reset(f);
            for (unsigned b = 0; b < 4; ++b) store8(f, TABLE + 4 * row + b, (uint8_t)((row + b) % 4));
            store32(f, MANAGER + 8, DATA); store32(f, MANAGER + 4, EXTRA);
            store32(f, DATA, row << 13); store16(f, DATA + 4, corrupt ? 4 : 0);
            for (unsigned i = 8; i < 0xe8; ++i) store8(f, DATA + i, (uint8_t)(0x80 + i));
            memset(f->bytes + EXTRA, 0x5a, 0x1c); memset(f->bytes + FALLBACK, 0xa5, 64);
            for (unsigned i = 0; i < length; ++i) store16(f, SOURCE + 2 * i, (uint16_t)(0x410 + i));
            store16(f, SOURCE + 2 * length, 0);
            uint8_t expected[0xe0]; memcpy(expected, f->bytes + DATA + 8, sizeof expected);
            const unsigned physical = (row + 1) % 4;
            if (!corrupt) memcpy(expected + physical * 0x38, f->bytes + SOURCE, 2 * (length + 1));
            if (state & 1) px_encode_pokemon_manager(MANAGER, &f->memory);
            store8(f, MANAGER + 13, (uint8_t)(state >> 1));
            px_write_pokemon_name(MANAGER, SOURCE, &f->memory);
            CHECK(load8(f, MANAGER + 12) == (state >> 1 ? 0 : 1));
            px_decode_pokemon_manager(MANAGER, &f->memory);
            CHECK(!memcmp(expected, f->bytes + DATA + 8, sizeof expected));
            CHECK(load16(f, DATA + 4) == (corrupt ? 4 : 0));
            for (unsigned i = 0; i < 0x1c; ++i) CHECK(load8(f, EXTRA + i) == 0x5a);
            if (corrupt) {
                CHECK(!memcmp(f->bytes + FALLBACK, f->bytes + SOURCE, 2 * (length + 1)));
                CHECK(load16(f, FALLBACK + 2 * (length + 1)) == 0xa5a5);
            } else CHECK(load16(f, FALLBACK) == 0xa5a5);
            ++cases;
        }
    CHECK(cases == 3328);
}
static void select_personal(void *u, uint32_t r, uint32_t s, uint32_t form)
{ (void)u; (void)r; (void)s; (void)form; CHECK(0); }
static uint32_t personal_value(void *u, uint32_t r, uint32_t a)
{ (void)u; (void)r; (void)a; CHECK(0); return 0; }
static void read_learnset(void *user, uint32_t archive, uint32_t index, uint32_t destination)
{
    struct fixture *f = user; CHECK(archive == 0xa00 && index == 25);
    for (unsigned i = 0; i < 4; ++i) {
        store16(f, destination + 4 * i, (uint16_t)(i < 3 ? 10 + i : 0xffff));
        store16(f, destination + 4 * i + 2, (uint16_t)(i < 3 ? 1 : 0xffff));
    }
}
static uint32_t state_level(void *u, uint32_t s, uint32_t form, uint32_t growth)
{ (void)u; CHECK(s == 25 && form == 0 && growth == 100); return 5; }
static void state_select(void *user, uint32_t species, uint32_t form)
{ struct fixture *f = user; const struct px_learnset_api a = {&f->memory, f, select_personal, personal_value, read_learnset}; px_select_pokemon_learnset(species, form, &a); }
static uint32_t state_count(void *user)
{ struct fixture *f = user; return px_current_learnset_count(&f->memory); }
static uint32_t state_entry_level(void *user, uint32_t i)
{ struct fixture *f = user; return px_current_learnset_level(i, &f->memory); }
static uint32_t state_move(void *user, uint32_t i)
{ struct fixture *f = user; return px_current_learnset_move(i, &f->memory); }
static uint32_t move_value(void *u, uint32_t move, uint32_t a)
{ (void)u; CHECK(a == 5); return 10 + move; }
static void state_learn(void *user, uint32_t object, uint32_t move)
{ struct fixture *f = user; const struct px_move_api a = {&f->memory, f, move_value}; px_pokemon_learn_move(object, move, &a); }
static uint32_t state_pp(void *user, uint32_t move, uint32_t ups)
{ struct fixture *f = user; const struct px_move_api a = {&f->memory, f, move_value}; return px_pokemon_maximum_pp(move, ups, &a); }
static void state_build_name(void *user, uint8_t *scratch, uint32_t species)
{
    struct fixture *f = user;
    memcpy(f->bytes + SCRATCH, scratch, PX_POKEMON_NAME_SCRATCH_SIZE);
    (void)px_build_pokemon_name(SCRATCH, species, &f->names);
    memcpy(scratch, f->bytes + SCRATCH, PX_POKEMON_NAME_SCRATCH_SIZE);
}
static void state_write_name(void *user, uint32_t manager, const uint8_t *scratch)
{
    struct fixture *f = user; memcpy(f->bytes + SCRATCH, scratch, PX_POKEMON_NAME_SCRATCH_SIZE);
    px_write_pokemon_name(manager, SCRATCH, &f->memory);
}
static void integration(struct fixture *f)
{
    reset(f); f->index = 25;
    store32(f, OBJECT + 12, MANAGER); store32(f, MANAGER + 8, DATA); store8(f, MANAGER + 13, 1);
    for (unsigned b = 0; b < 4; ++b) store8(f, TABLE + b, (uint8_t)b);
    store16(f, DATA + 8, 25); store32(f, DATA + 16, 100); store32(f, 0x5d1870, 0x1600);
    store32(f, 0x5d17b4, 0x1000); store32(f, 0x5d17b8, 0x1100);
    store16(f, 0x1000, 0xffff); store32(f, 0x1004, 0x1400);
    store32(f, 0x1104, 0x1300); store32(f, 0x1108, 1); store32(f, 0x1300, 0x1200);
    store16(f, 0x1200, 0xffff); store32(f, 0x1204, 0x1500); store32(f, 0x5d17ec, 0xa00);
    const struct px_state_api api = {.memory = &f->memory, .user = f, .level_from_growth = state_level,
        .select_learnset = state_select, .learnset_count = state_count, .learnset_level = state_entry_level,
        .learnset_move = state_move, .learn_move = state_learn, .maximum_pp = state_pp,
        .build_name = state_build_name, .write_name = state_write_name};
    uint8_t scratch[PX_POKEMON_NAME_SCRATCH_SIZE]; memset(scratch, 0xcd, sizeof scratch);
    px_initialize_pokemon_base_state(OBJECT, scratch, &api);
    CHECK(f->loads == 1 && load16(f, DATA + 8 + 0x38) == 'N');
    CHECK(load16(f, DATA + 8 + 0x38 + 8) == 0 && scratch[10] == 0xcd);
    for (unsigned i = 0; i < 3; ++i) {
        CHECK(px_read_pokemon_state_slot(MANAGER, i, PX_STATE_MOVE, &f->memory) == 10 + i);
        CHECK(px_read_pokemon_state_slot(MANAGER, i, PX_STATE_PP, &f->memory) == 20 + i);
    }
    CHECK((load32(f, DATA + 8 + 0x38 + 0x34) >> 31) == 0);
}
int main(void)
{
    struct fixture f = {0}; f.bytes = calloc(1, SIZE); CHECK(f.bytes);
    copy_cases(&f); index_cases(&f); builder_cases(&f); writer_cases(&f); integration(&f);
    free(f.bytes);
    puts("pokemon_names: 65,535 UTF-16 values, 2,051 plain lengths, 128 token cases, 945 form indices, builder reloads, 3,328 writer/codec cases and state integration passed");
    return 0;
}
