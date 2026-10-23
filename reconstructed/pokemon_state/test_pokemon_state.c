#include "pokemon_state.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { \
    fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #c); \
    exit(EXIT_FAILURE); \
} } while (0)

enum { MEM_SIZE = 0x2000, OBJECT = 0x100, MANAGER = 0x200,
       MANAGER2 = 0x220, DATA = 0x400, EXTRA = 0x600,
       SETTINGS = 0x800, SETTINGS2 = 0x900, DATA2 = 0xa00, EXTRA2 = 0xc00 };

struct fixture {
    uint8_t bytes[MEM_SIZE];
    struct px_pokemon_api memory;
    struct px_state_api api;
    uint8_t scratch[PX_POKEMON_NAME_SCRATCH_SIZE];
    unsigned kind, maximum_calls, level_calls, mode_calls, stat_calls;
    unsigned settings_reads, count_calls, learned, level_indices, move_indices;
    unsigned decodes, encodes;
    uint32_t settings_order;
    uint32_t count, level_limit, mode, settings;
    int final_lock, swap_manager, mutate_species;
    uint32_t store_addresses[10];
    uint8_t store_widths[10];
    unsigned store_count;
    char events[32];
    size_t event_count;
};

static uint8_t load8(void *user, uint32_t address)
{
    struct fixture *f = user;
    if (address >= UINT32_C(0x00570eec) && address < UINT32_C(0x00570f6c))
        return (uint8_t)((address - UINT32_C(0x00570eec)) % 4);
    if (address >= SETTINGS2 + 0x68 && address <= SETTINGS2 + 0x6b)
        f->settings_order = (f->settings_order << 8) | (address - SETTINGS2);
    CHECK(address < MEM_SIZE);
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
    if (address == UINT32_C(0x005d1870)) {
        ++f->settings_reads;
        return f->settings;
    }
    return (uint32_t)load16(user, address) |
           (uint32_t)load16(user, address + 2) << 16;
}
static void store8(void *user, uint32_t address, uint8_t value)
{
    struct fixture *f = user;
    CHECK(address < MEM_SIZE);
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
static void trace_store(struct fixture *f, uint32_t address, uint8_t width)
{
    CHECK(f->store_count < sizeof f->store_widths / sizeof f->store_widths[0]);
    f->store_addresses[f->store_count] = address;
    f->store_widths[f->store_count++] = width;
}
static void traced_store8(void *user, uint32_t address, uint8_t value)
{
    CHECK(value == 0);
    trace_store(user, address, 1);
    store8(user, address, value);
}
static void traced_store16(void *user, uint32_t address, uint16_t value)
{
    CHECK(value == 0);
    trace_store(user, address, 2);
    store16(user, address, value);
}
static void traced_store32(void *user, uint32_t address, uint32_t value)
{
    CHECK(value == 0);
    trace_store(user, address, 4);
    store32(user, address, value);
}
static void decode(void *user, uint32_t manager)
{
    struct fixture *f = user;
    ++f->decodes;
    px_decode_pokemon_manager(manager, &f->memory);
    if (f->kind == 8) {
        store32(f, manager + 4, EXTRA2);
        store8(f, manager + 0xd, UINT8_C(0x80));
    }
}
static void encode(void *user, uint32_t manager)
{
    struct fixture *f = user;
    ++f->encodes;
    CHECK(load8(f, manager + 0xd) == 0);
    px_encode_pokemon_manager(manager, &f->memory);
}
static void event(struct fixture *f, char value)
{
    CHECK(f->event_count + 1 < sizeof f->events);
    f->events[f->event_count++] = value;
    f->events[f->event_count] = '\0';
}
static uint32_t level_from_growth(void *user, uint32_t species,
                                  uint32_t form, uint32_t growth)
{
    struct fixture *f = user;
    CHECK(species == UINT32_C(0x1234) && form == 7);
    CHECK(growth == UINT32_C(0x12345678));
    ++f->level_calls;
    event(f, 'L');
    if (f->mutate_species) {
        store16(f, DATA + 8, UINT16_C(0x5678));
        store8(f, DATA + 0x1d, UINT8_C(0x22));
    }
    if (f->kind == 9) store32(f, OBJECT + 0xc, MANAGER2);
    return f->level_limit;
}
static void select_learnset(void *user, uint32_t species, uint32_t form)
{
    struct fixture *f = user;
    CHECK(f->level_calls == 1);
    CHECK(species == UINT32_C(0x1234) && form == 7);
    event(f, 'S');
}
static uint32_t learnset_count(void *user)
{
    struct fixture *f = user;
    ++f->count_calls;
    /* The second query can shorten the active resource after a learned move. */
    return f->kind == 4 && f->count_calls > 1 ? 1 : f->count;
}
static uint32_t learnset_level(void *user, uint32_t index)
{
    struct fixture *f = user;
    CHECK(index == (f->level_indices & UINT32_C(0xffff)));
    ++f->level_indices;
    if (f->kind == 5) return UINT32_C(0x80000000);
    if (f->kind == 6 && index == 1) return f->level_limit + 1;
    return f->level_limit;
}
static uint32_t learnset_move(void *user, uint32_t index)
{
    struct fixture *f = user;
    CHECK(index == (f->move_indices & UINT32_C(0xffff)));
    ++f->move_indices;
    return UINT32_C(0x12340000) + index;
}
static void learn_move(void *user, uint32_t object, uint32_t move)
{
    struct fixture *f = user;
    CHECK(object == OBJECT);
    CHECK(move == UINT32_C(0x12340000) + (f->learned & UINT32_C(0xffff)));
    ++f->learned;
}
static void build_name(void *user, uint8_t *scratch, uint32_t species)
{
    struct fixture *f = user;
    CHECK(scratch == f->scratch);
    CHECK(species == (f->mutate_species ? UINT32_C(0x5678) : UINT32_C(0x1234)));
    for (unsigned i = 0; i < sizeof f->scratch; ++i) CHECK(scratch[i] == 0xa7);
    scratch[0] = 0x31;
    scratch[1] = 0x32;
    event(f, 'B');
    /* A callback can replace the object's manager before the name write. */
    if (f->swap_manager) store32(f, OBJECT + 0xc, MANAGER2);
}
static void write_name(void *user, uint32_t manager, const uint8_t *scratch)
{
    struct fixture *f = user;
    CHECK(manager == (f->swap_manager ? MANAGER2 : MANAGER));
    CHECK(scratch == f->scratch && scratch[0] == 0x31 && scratch[1] == 0x32);
    for (unsigned i = 2; i < sizeof f->scratch; ++i) CHECK(scratch[i] == 0xa7);
    event(f, 'W');
}
static uint32_t maximum_pp(void *user, uint32_t move, uint32_t pp_ups)
{
    struct fixture *f = user;
    const unsigned call = f->maximum_calls++;
    if (f->kind == 7) {
        CHECK(pp_ups == 0);
        if (call == 0) {
            CHECK(move == 0);
            store16(f, DATA + 0x5a, UINT16_C(0x777));
            return 6;
        }
        if (call < 3) {
            CHECK(move == UINT32_C(0x777));
            return call == 1 ? 8 : 9;
        }
        CHECK(move == 0 && call < 6);
        return 0;
    }
    if (f->kind != 2) {
        CHECK(move == 0 && pp_ups == 0);
        if (call == 3) f->settings = SETTINGS2;
        return 0;
    }
    /* Slot zero disappears after its first calculation. The other three
     * slots exercise wraparound, narrowing between minima, and unsigned min. */
    static const uint32_t results[10] = {
        17, UINT32_C(0xfffffffe), UINT32_C(0x80000000), 2,
        300, 400, 40, UINT32_C(0x80000000), UINT32_C(0x7fffffff), 300
    };
    CHECK(call < 10);
    const unsigned slot = call == 0 ? 0 : call < 4 ? 1 : call < 7 ? 2 : 3;
    CHECK(move == 0x100 + slot + (call == 3 ? 0x10u : 0));
    CHECK(pp_ups == slot + (call == 3 ? 7u : 0));
    if (call == 0) store16(f, DATA + 0x5a, 0);
    if (call == 2) {
        store16(f, DATA + 0x5c, UINT16_C(0x111));
        store8(f, DATA + 0x67, 8);
    }
    if (call == 3) store32(f, OBJECT + 0xc, MANAGER2);
    return results[call];
}
static void update_mode(void *user, uint32_t object, uint32_t mode)
{
    struct fixture *f = user;
    CHECK(object == OBJECT && mode == f->mode);
    CHECK(load8(f, (f->kind == 9 ? EXTRA2 : EXTRA) + 4) == (uint8_t)f->level_limit);
    CHECK(f->level_calls == 1 && f->stat_calls == 0);
    CHECK(load8(f, MANAGER + 0xd) != 0);
    if (f->swap_manager) store32(f, OBJECT + 0xc, MANAGER2);
    ++f->mode_calls;
    event(f, 'M');
}
static void update_stat(void *user, uint32_t object, enum px_state_stat stat)
{
    struct fixture *f = user;
    CHECK(object == OBJECT && (unsigned)stat == f->stat_calls);
    CHECK(f->mode_calls == 1);
    event(f, (char)('0' + f->stat_calls));
    ++f->stat_calls;
    if (f->stat_calls == 5 && f->final_lock >= 0)
        store8(f, load32(f, OBJECT + 0xc) + 0xd, (uint8_t)f->final_lock);
}
static void reset(struct fixture *f)
{
    memset(f, 0, sizeof *f);
    f->final_lock = -1;
    f->settings = SETTINGS;
    f->memory = (struct px_pokemon_api) {
        .user = f, .load8 = load8, .load16 = load16, .load32 = load32,
        .store8 = store8, .store16 = store16, .store32 = store32,
        .decode_manager = decode, .encode_manager = encode
    };
    f->api = (struct px_state_api) {
        .memory = &f->memory, .user = f, .level_from_growth = level_from_growth,
        .select_learnset = select_learnset, .learnset_count = learnset_count,
        .learnset_level = learnset_level, .learnset_move = learnset_move,
        .learn_move = learn_move, .build_name = build_name,
        .write_name = write_name, .maximum_pp = maximum_pp,
        .update_mode = update_mode, .update_stat = update_stat
    };
    store32(f, OBJECT + 0xc, MANAGER);
    for (unsigned i = 0; i < 2; ++i) {
        const uint32_t manager = MANAGER + 0x20 * i;
        store32(f, manager + 4, EXTRA);
        store32(f, manager + 8, DATA);
        store8(f, manager + 0xd, 1);
    }
    store16(f, DATA + 8, UINT16_C(0x1234));
    store8(f, DATA + 0x1d, UINT8_C(0x3a));
    store32(f, DATA + 0x10, UINT32_C(0x12345678));
    for (unsigned i = 0; i < 4; ++i) {
        store8(f, SETTINGS + 0x68 + i, (uint8_t)(0x10 + i));
        store8(f, SETTINGS2 + 0x68 + i, (uint8_t)(0x20 + i));
    }
    memset(f->scratch, 0xa7, sizeof f->scratch);
}

static void test_extra(void)
{
    struct fixture f;
    for (unsigned lock = 0; lock < 256; ++lock) {
        for (unsigned encoded = 0; encoded < 2; ++encoded) {
            reset(&f);
            memset(f.bytes + EXTRA - 1, 0xa5, PX_POKEMON_EXTRA_SIZE + 2);
            store8(&f, MANAGER + 0xd, 0);
            if (encoded) px_encode_pokemon_manager(MANAGER, &f.memory);
            store8(&f, MANAGER + 0xd, (uint8_t)lock);
            px_initialize_pokemon_extra_state(MANAGER, &f.api);
            CHECK(load8(&f, MANAGER + 0xd) == lock);
            CHECK(load8(&f, MANAGER + 0xc) == (lock ? encoded : 1));
            if (!lock) px_decode_pokemon_manager(MANAGER, &f.memory);
            for (unsigned i = 0; i < PX_POKEMON_EXTRA_SIZE; ++i)
                CHECK(load8(&f, EXTRA + i) == 0);
            CHECK(load8(&f, EXTRA - 1) == 0xa5);
            CHECK(load8(&f, EXTRA + PX_POKEMON_EXTRA_SIZE) == 0xa5);
        }
    }
    reset(&f);
    store32(&f, MANAGER + 4, 0);
    store8(&f, MANAGER + 0xc, 0x81);
    store8(&f, MANAGER + 0xd, 0x80);
    uint8_t before[MEM_SIZE];
    memcpy(before, f.bytes, sizeof before);
    px_initialize_pokemon_extra_state(MANAGER, &f.api);
    CHECK(memcmp(before, f.bytes, sizeof before) == 0);
    reset(&f);
    f.kind = 8;
    store8(&f, MANAGER + 0xd, 0);
    memset(f.bytes + EXTRA, 0x55, PX_POKEMON_EXTRA_SIZE);
    memset(f.bytes + EXTRA2, 0x66, PX_POKEMON_EXTRA_SIZE);
    px_initialize_pokemon_extra_state(MANAGER, &f.api);
    CHECK(f.decodes == 1 && f.encodes == 1);
    CHECK(load8(&f, MANAGER + 0xd) == 0);
    px_decode_pokemon_manager(MANAGER, &f.memory);
    for (unsigned i = 0; i < PX_POKEMON_EXTRA_SIZE; ++i) {
        CHECK(load8(&f, EXTRA + i) == 0x55);
        CHECK(load8(&f, EXTRA2 + i) == 0);
    }
    /* Independent access-width oracle from the four entry alignments in
     * 0015e914 and its 00301aa0 tail. Only top-level API stores are traced. */
    static const uint8_t expected_widths[4][9] = {
        {4, 4, 4, 4, 4, 4, 4},
        {1, 2, 4, 4, 4, 4, 4, 4, 1},
        {2, 4, 4, 4, 4, 4, 4, 2},
        {1, 4, 4, 4, 4, 4, 4, 2, 1}
    };
    static const unsigned expected_counts[4] = {7, 9, 8, 9};
    for (unsigned alignment = 0; alignment < 4; ++alignment) {
        reset(&f);
        const uint32_t extra = EXTRA + alignment;
        store32(&f, MANAGER + 4, extra);
        memset(f.bytes + extra - 1, 0xa5, PX_POKEMON_EXTRA_SIZE + 2);
        CHECK(load8(&f, MANAGER + 0xd) == 1);
        f.memory.store8 = traced_store8;
        f.memory.store16 = traced_store16;
        f.memory.store32 = traced_store32;
        px_initialize_pokemon_extra_state(MANAGER, &f.api);
        CHECK(f.store_count == expected_counts[alignment]);
        uint32_t expected_address = extra;
        for (unsigned i = 0; i < f.store_count; ++i) {
            CHECK(f.store_addresses[i] == expected_address);
            CHECK(f.store_widths[i] == expected_widths[alignment][i]);
            expected_address += expected_widths[alignment][i];
        }
        CHECK(expected_address == extra + PX_POKEMON_EXTRA_SIZE);
        for (unsigned i = 0; i < PX_POKEMON_EXTRA_SIZE; ++i)
            CHECK(load8(&f, extra + i) == 0);
        CHECK(load8(&f, extra - 1) == 0xa5);
        CHECK(load8(&f, extra + PX_POKEMON_EXTRA_SIZE) == 0xa5);
    }
    puts("PASS: extra clear; 512 encoded/lock combinations; null extra; four alignment width/order traces; bounds");
}

static void test_derived(void)
{
    struct fixture f;
    const int finals[] = {-1, 0, 1, 2, 0x80, 0x81, 0xff};
    for (unsigned lock = 0; lock < 256; ++lock) {
        for (unsigned i = 0; i < sizeof finals / sizeof finals[0]; ++i) {
            reset(&f);
            f.kind = 1;
            f.level_limit = UINT32_C(0x123456ab);
            f.mode = UINT32_C(0xfedcba98);
            f.final_lock = finals[i];
            store8(&f, MANAGER + 0xd, (uint8_t)lock);
            px_update_pokemon_derived_state(OBJECT, f.mode, &f.api);
            const unsigned end = finals[i] < 0 ? (lock ? lock : 1) : (unsigned)finals[i];
            const int restored = !lock && (end & 1);
            CHECK(load8(&f, MANAGER + 0xd) == (restored ? 0 : end));
            CHECK(load8(&f, MANAGER + 0xc) == (restored ? 1 : 0));
            CHECK(strcmp(f.events, "LM01234") == 0);
        }
    }
    reset(&f);
    f.kind = 1;
    f.level_limit = UINT32_C(0x123456ab);
    f.mode = UINT32_C(0x87654321);
    f.swap_manager = 1;
    store8(&f, MANAGER + 0xd, 0);
    px_update_pokemon_derived_state(OBJECT, f.mode, &f.api);
    CHECK(load8(&f, MANAGER + 0xd) == 1 && load8(&f, MANAGER + 0xc) == 0);
    CHECK(load8(&f, MANAGER2 + 0xd) == 0 && load8(&f, MANAGER2 + 0xc) == 1);
    reset(&f);
    f.kind = 9;
    f.level_limit = UINT32_C(0x123456ab);
    f.mode = UINT32_C(0xabcdef01);
    store8(&f, MANAGER + 0xd, 0);
    store32(&f, MANAGER2 + 4, EXTRA2);
    px_update_pokemon_derived_state(OBJECT, f.mode, &f.api);
    CHECK(load8(&f, EXTRA + 4) == 0);
    CHECK(load8(&f, MANAGER + 0xd) == 1 && load8(&f, MANAGER2 + 0xd) == 0);
    reset(&f);
    f.level_limit = 20;
    f.mode = 1;
    store8(&f, MANAGER + 0xd, 0);
    px_encode_pokemon_manager(MANAGER, &f.memory);
    px_update_pokemon_derived_state(OBJECT, f.mode, &f.api);
    CHECK(f.decodes == 1 && f.encodes == 1);
    CHECK(load8(&f, MANAGER + 0xc) == 1 && load8(&f, MANAGER + 0xd) == 0);
    puts("PASS: derived order; 1792 initial/final lock cases; mode width; manager replacement");
}

static void test_refresh(void)
{
    struct fixture f;
    reset(&f);
    f.kind = 2;
    store16(&f, EXTRA + 0xa, UINT16_C(0xabcd));
    store16(&f, EXTRA + 8, UINT16_C(0xffff));
    store32(&f, EXTRA, UINT32_C(0xdeadbeef));
    for (unsigned i = 0; i < 4; ++i) {
        store16(&f, DATA + 0x5a + 2 * i, (uint16_t)(0x100 + i));
        store8(&f, DATA + 0x66 + i, (uint8_t)i);
        store8(&f, DATA + 0x62 + i, i == 1 ? 5 : i == 2 ? 2 : 0);
    }
    memcpy(f.bytes + DATA2, f.bytes + DATA, PX_POKEMON_DATA_SIZE);
    store32(&f, MANAGER2 + 8, DATA2);
    px_refresh_pokemon_state(OBJECT, &f.api);
    CHECK(load16(&f, EXTRA + 8) == UINT16_C(0xabcd));
    CHECK(load32(&f, EXTRA) == 0);
    CHECK(f.maximum_calls == 10);
    CHECK(load8(&f, DATA + 0x62) == 0);
    CHECK(load8(&f, DATA + 0x63) == 5);
    CHECK(load8(&f, DATA2 + 0x63) == 2);
    CHECK(load8(&f, DATA2 + 0x64) == 40);
    CHECK(load8(&f, DATA2 + 0x65) == 255);
    CHECK(load32(&f, OBJECT + 0xc) == MANAGER2);
    reset(&f);
    f.kind = 7;
    px_refresh_pokemon_state(OBJECT, &f.api);
    CHECK(f.maximum_calls == 6);
    CHECK(load8(&f, DATA + 0x62) == 6);
    puts("PASS: refresh dynamic moves; repeated PP reads; wraparound; unsigned minima; early byte narrowing");
}

static void test_base(void)
{
    struct fixture f;
    const unsigned kinds[] = {0, 3, 4, 5, 6};
    const uint32_t counts[] = {0, UINT32_C(65537), UINT32_C(0x80000000), 3, 3};
    const unsigned learned[] = {0, 65537, 1, 0, 1};
    const unsigned count_calls[] = {1, 65538, 2, 1, 2};
    for (unsigned i = 0; i < sizeof kinds / sizeof kinds[0]; ++i) {
        reset(&f);
        f.kind = kinds[i];
        f.count = counts[i];
        f.level_limit = i == 3 ? UINT32_C(0x7fffffff) : 20;
        f.mutate_species = 1;
        f.swap_manager = 1;
        memset(f.bytes + DATA + 0x5a, 0xff, 16);
        store32(&f, DATA + 0x74, UINT32_C(0xc0000000));
        px_initialize_pokemon_base_state(OBJECT, f.scratch, &f.api);
        CHECK(strcmp(f.events, "LSBW") == 0);
        CHECK(f.learned == learned[i] && f.move_indices == learned[i]);
        CHECK(f.count_calls == count_calls[i]);
        CHECK(f.maximum_calls == 4 && f.settings_reads == 4);
        CHECK(f.settings_order == UINT32_C(0x696a686b));
        CHECK(load32(&f, DATA + 0x74) == UINT32_C(0x40000000));
        for (unsigned n = 0; n < 16; ++n) CHECK(load8(&f, DATA + 0x5a + n) == 0);
        CHECK(load8(&f, DATA + 0xe0) == 0x21);
        CHECK(load8(&f, DATA + 0xe1) == 0x22);
        CHECK(load8(&f, DATA + 0xe2) == 0x23);
        CHECK(load8(&f, DATA + 0xe3) == 0x20);
    }
    puts("PASS: base clear and callback order; captured species; count reload; unsigned levels; 16-bit learnset index wrap; name scratch");
}

void test_pokemon_state_chain(void);
void test_pokemon_state_fields(void);
int main(void)
{
    test_pokemon_state_fields();
    test_extra();
    test_derived();
    test_refresh();
    test_base();
    test_pokemon_state_chain();
    puts("PASS: Pokemon state orchestration and seven-module integration");
    return 0;
}
