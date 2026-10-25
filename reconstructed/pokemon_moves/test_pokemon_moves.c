#include "pokemon_moves.h"
#include "../pokemon_state/pokemon_state.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { \
    fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #c); \
    exit(EXIT_FAILURE); \
} } while (0)

enum { SIZE = 0x649000, OBJECT = 0x100, MANAGER = 0x200, MANAGER2 = 0x220,
       DATA = 0x400, DATA2 = 0x600, TABLE = 0x570eec };
struct fixture {
    uint8_t *bytes;
    struct px_pokemon_api memory;
    struct px_move_api api;
    uint32_t base, expected_move, block, block2;
    unsigned calls, manager_reads, trace, swap, resources;
    char writes[32];
    unsigned write_count;
};
static uint8_t load8(void *user, uint32_t address)
{
    struct fixture *f = user;
    CHECK(address < SIZE);
    return f->bytes[address];
}
static uint16_t load16(void *user, uint32_t address)
{
    return (uint16_t)((uint32_t)load8(user, address) | (uint32_t)load8(user, address + 1) << 8);
}
static uint32_t load32(void *user, uint32_t address)
{
    struct fixture *f = user;
    if (address == OBJECT + 0xc && f->trace) ++f->manager_reads;
    return (uint32_t)load16(user, address) | (uint32_t)load16(user, address + 2) << 16;
}
static void event(struct fixture *f, char c)
{
    if (!f->trace) return;
    CHECK(f->write_count + 1 < sizeof f->writes);
    f->writes[f->write_count++] = c;
    f->writes[f->write_count] = 0;
}
static void store8(void *user, uint32_t address, uint8_t value)
{
    struct fixture *f = user;
    CHECK(address < SIZE);
    if ((address >= f->block + 0x22 && address < f->block + 0x26) ||
        (address >= f->block2 + 0x22 && address < f->block2 + 0x26)) event(f, 'P');
    if ((address >= f->block + 0x26 && address < f->block + 0x2a) ||
        (address >= f->block2 + 0x26 && address < f->block2 + 0x2a)) event(f, 'U');
    f->bytes[address] = value;
}
static void store16(void *user, uint32_t address, uint16_t value)
{
    struct fixture *f = user;
    if ((address >= f->block + 0x1a && address < f->block + 0x22) ||
        (address >= f->block2 + 0x1a && address < f->block2 + 0x22)) event(f, 'M');
    store8(user, address, (uint8_t)value);
    store8(user, address + 1, (uint8_t)(value >> 8));
}
static void store32(void *user, uint32_t address, uint32_t value)
{
    store16(user, address, (uint16_t)value);
    store16(user, address + 2, (uint16_t)(value >> 16));
}
static uint32_t move_value(void *user, uint32_t move, uint32_t selector)
{
    struct fixture *f = user;
    CHECK((f->resources || move == f->expected_move) && selector == 5);
    ++f->calls;
    event(f, 'V');
    if (f->swap) store32(f, OBJECT + 0xc, MANAGER2);
    return f->resources ? 10 + move : f->base;
}
static void reset(struct fixture *f, unsigned row)
{
    memset(f->bytes, 0, SIZE);
    f->calls = f->manager_reads = f->trace = f->swap = f->write_count = f->resources = 0;
    f->writes[0] = 0;
    f->base = 0x12340123;
    f->memory = (struct px_pokemon_api){.user = f, .load8 = load8, .load16 = load16,
        .load32 = load32, .store8 = store8, .store16 = store16, .store32 = store32};
    f->api = (struct px_move_api){&f->memory, f, move_value};
    store32(f, OBJECT + 0xc, MANAGER);
    /* Invented block permutations: every selector row and every physical
     * location, without embedding the game's table. */
    for (unsigned b = 0; b < 4; ++b) store8(f, TABLE + 4 * row + b, (uint8_t)((row + b) % 4));
    f->block = DATA + 8 + ((row + 1) % 4) * 0x38;
    f->block2 = DATA2 + 8 + ((row + 1) % 4) * 0x38;
    for (unsigned i = 0; i < 2; ++i) {
        store32(f, MANAGER + 0x20 * i + 8, DATA + 0x200 * i);
        store8(f, MANAGER + 0x20 * i + 0xd, 1); /* acquired, decoded */
        store32(f, DATA + 0x200 * i, row << 13);
    }
}
static void slot_set(struct fixture *f, unsigned slot, uint16_t move, uint8_t pp, uint8_t ups)
{
    store16(f, f->block + 0x1a + 2 * slot, move);
    store8(f, f->block + 0x22 + slot, pp);
    store8(f, f->block + 0x26 + slot, ups);
}
static void maximum_pp_cases(struct fixture *f)
{
    reset(f, 0);
    f->expected_move = UINT32_C(0xfedcba98);
    for (unsigned base = 0; base < 256; ++base) {
        f->base = UINT32_C(0xabcdef00) | base;
        for (unsigned ups = 0; ups < 256; ++ups) {
            const unsigned capped = ups < 3 ? ups : 3;
            const uint32_t expected = (base + base * capped / 5) % 256;
            CHECK(px_pokemon_maximum_pp(f->expected_move, ups, &f->api) == expected);
        }
        CHECK(px_pokemon_maximum_pp(f->expected_move, UINT32_MAX, &f->api) == (base + base * 3 / 5) % 256);
        CHECK(px_pokemon_maximum_pp(f->expected_move, UINT32_C(0x80000000), &f->api) == (base + base * 3 / 5) % 256);
    }
    CHECK(f->calls == 256 * 258 && f->manager_reads == 0);
}
static void insertion_cases(struct fixture *f)
{
    const uint32_t incoming[] = {0, 10, 11, 13, 99, 0x1000a};
    unsigned cases = 0;
    for (unsigned row = 0; row < 32; ++row)
        for (unsigned mask = 0; mask < 16; ++mask)
            for (unsigned n = 0; n < sizeof incoming / sizeof incoming[0]; ++n) {
                reset(f, row);
                uint16_t moves[4]; uint8_t pp[4], ups[4];
                for (unsigned s = 0; s < 4; ++s) {
                    moves[s] = mask >> s & 1 ? (uint16_t)(10 + s) : 0;
                    pp[s] = (uint8_t)(23 + s); ups[s] = (uint8_t)(7 + s);
                    slot_set(f, s, moves[s], pp[s], ups[s]);
                }
                unsigned known = 4, empty = 4;
                for (unsigned s = 0; s < 4; ++s) {
                    if (known == 4 && moves[s] == incoming[n]) known = s;
                    if (empty == 4 && moves[s] == 0) empty = s;
                }
                if (known == 4) {
                    if (empty == 4) {
                        memmove(moves, moves + 1, 3 * sizeof *moves);
                        memmove(pp, pp + 1, 3); memmove(ups, ups + 1, 3);
                    }
                    const unsigned target = empty < 4 ? empty : 3;
                    moves[target] = (uint16_t)incoming[n]; pp[target] = 0x23; ups[target] = 0;
                }
                /* Alternate acquired/plain and fully encrypted manager use. */
                if (row & 1) {
                    store8(f, MANAGER + 0xd, 0);
                    px_encode_pokemon_manager(MANAGER, &f->memory);
                }
                f->expected_move = incoming[n]; f->trace = !(row & 1);
                px_pokemon_learn_move(OBJECT, incoming[n], &f->api);
                CHECK(f->calls == (known == 4));
                if (!(row & 1)) {
                    CHECK(f->manager_reads == (known < 4 ? known + 1 : empty < 4 ? 8 + empty : 29));
                    CHECK(!strcmp(f->writes, known < 4 ? "" : empty < 4 ? "VMUP" : "MPUMPUMPUVMUP"));
                }
                f->trace = 0;
                for (unsigned s = 0; s < 4; ++s) {
                    CHECK(px_read_pokemon_state_slot(MANAGER, s, PX_STATE_MOVE, &f->memory) == moves[s]);
                    CHECK(px_read_pokemon_state_slot(MANAGER, s, PX_STATE_PP, &f->memory) == pp[s]);
                    CHECK(px_read_pokemon_state_slot(MANAGER, s, PX_STATE_PP_UPS, &f->memory) == ups[s]);
                }
                CHECK((load16(f, DATA + 4) & 4) == 0); /* codec round trips remain valid */
                ++cases;
            }
    CHECK(cases == 3072);
}
static void callback_mutation(struct fixture *f)
{
    for (unsigned full = 0; full < 2; ++full) {
        reset(f, 0);
        if (full) for (unsigned s = 0; s < 4; ++s) slot_set(f, s, (uint16_t)(10 + s), 20, 2);
        /* Destination selection is captured on manager 1 before the callback
         * swaps manager 2 into the object. Existing data in manager 2 must not
         * trigger a new duplicate/empty scan. */
        for (unsigned s = 0; s < 4; ++s) store16(f, f->block2 + 0x1a + 2 * s, 99);
        f->swap = 1; f->expected_move = 99; f->trace = 1;
        px_pokemon_learn_move(OBJECT, 99, &f->api);
        f->trace = 0;
        const unsigned target = full ? 3 : 0;
        CHECK(load16(f, f->block2 + 0x1a + 2 * target) == 99);
        CHECK(load8(f, f->block2 + 0x22 + target) == 0x23);
        CHECK(load8(f, f->block2 + 0x26 + target) == 0);
        CHECK(load8(f, f->block + 0x22 + target) == (full ? 20 : 0));
        CHECK(f->calls == 1 && f->manager_reads == (full ? 29 : 8));
    }
}
static uint32_t level_from_growth(void *user, uint32_t species, uint32_t form, uint32_t growth)
{
    (void)user;
    CHECK(species == 25 && form == 0 && growth == 100);
    return 5;
}
static void select_learnset(void *user, uint32_t species, uint32_t form)
{
    (void)user;
    CHECK(species == 25 && form == 0);
}
static uint32_t learnset_count(void *user) { (void)user; return 8; }
static uint32_t learnset_level(void *user, uint32_t index)
{
    (void)user; CHECK(index < 8); return index == 7 ? 10 : 1;
}
static uint32_t learnset_move(void *user, uint32_t index)
{
    static const uint16_t moves[] = {10, 11, 12, 13, 14, 11, 15, 16};
    (void)user; CHECK(index < 7); return moves[index];
}
static void learn_move(void *user, uint32_t object, uint32_t move)
{
    struct fixture *f = user;
    px_pokemon_learn_move(object, move, &f->api);
}
static uint32_t maximum_pp(void *user, uint32_t move, uint32_t ups)
{
    struct fixture *f = user;
    return px_pokemon_maximum_pp(move, ups, &f->api);
}
static void build_name(void *user, uint8_t *scratch, uint32_t species)
{
    (void)user; CHECK(species == 25); memset(scratch, 0x42, PX_POKEMON_NAME_SCRATCH_SIZE);
}
static void write_name(void *user, uint32_t manager, const uint8_t *scratch)
{
    (void)user; CHECK(manager == MANAGER && scratch[0] == 0x42);
}
static void base_state_integration(struct fixture *f)
{
    reset(f, 0);
    f->resources = 1;
    store16(f, DATA + 8, 25); store32(f, DATA + 0x10, 100);
    store32(f, UINT32_C(0x005d1870), 0x1000);
    for (unsigned s = 0; s < 4; ++s) slot_set(f, s, 99, 2, 3);
    const struct px_state_api state = {.memory = &f->memory, .user = f,
        .level_from_growth = level_from_growth, .select_learnset = select_learnset,
        .learnset_count = learnset_count, .learnset_level = learnset_level, .learnset_move = learnset_move,
        .learn_move = learn_move, .maximum_pp = maximum_pp, .build_name = build_name, .write_name = write_name};
    uint8_t scratch[PX_POKEMON_NAME_SCRATCH_SIZE];
    px_initialize_pokemon_base_state(OBJECT, scratch, &state);
    for (unsigned s = 0; s < 4; ++s) {
        CHECK(load16(f, f->block + 0x1a + 2 * s) == 12 + s);
        CHECK(load8(f, f->block + 0x22 + s) == 22 + s);
        CHECK(load8(f, f->block + 0x26 + s) == 0);
    }
    CHECK(f->calls == 18); /* six unique insertions, twelve refresh PP calls */
    /* Refresh now uses the reconstructed PP Ups calculation as well. */
    store8(f, f->block + 0x26, 3); store8(f, f->block + 0x22, 0);
    px_refresh_pokemon_state(OBJECT, &state);
    CHECK(load8(f, f->block + 0x22) == 35); /* 22 + floor(22*3/5) */
}
int main(void)
{
    struct fixture f = {0};
    f.bytes = calloc(1, SIZE); CHECK(f.bytes);
    maximum_pp_cases(&f);
    insertion_cases(&f);
    callback_mutation(&f);
    base_state_integration(&f);
    free(f.bytes);
    puts("pokemon_moves: 66,048 maximum-PP cases, 3,072 insertion/codec cases, callback mutation and base-state integration passed");
    return 0;
}
