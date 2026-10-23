#include "pokemon_constructor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #condition); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

void test_pokemon_parameters(void);
void test_pokemon_constructor_chain(void);

enum {
    MEMORY_SIZE = 0x2000, OBJECT = 0x100, MANAGER = 0x200,
    MANAGER_B = 0x300, MANAGER_C = 0x340, MANAGER_D = 0x380,
    DATA = 0x400, DATA_B = 0x600, EXTRA = 0x800, HEAP = 0x12345678
};

struct write_record {
    uint32_t address, value;
    unsigned width;
};

struct fixture {
    uint8_t memory[MEMORY_SIZE];
    uint8_t parameters[PX_POKEMON_PARAMETERS_SIZE];
    uint8_t normalized_guard[PX_POKEMON_PARAMETERS_SIZE + 2];
    struct write_record writes[2048];
    unsigned write_count, reads, allocations, populations, decodes, encodes;
    unsigned data_clear_bytes, extra_clear_bytes;
    uint32_t data_result, manager_result, extra_result, expected_manager;
    uint32_t object, expected_key, gender_result;
    int mutate_manager_allocation, mutate_base, mutate_extra_allocation;
    int mutate_decode, mutate_extra, mutate_refresh, mutate_encode;
    char events[64];
    unsigned event_count;
};

static void put16(uint8_t *p, uint32_t value)
{
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
}

static void put32(uint8_t *p, uint32_t value)
{
    for (unsigned i = 0; i < 4; ++i) p[i] = (uint8_t)(value >> (8 * i));
}

static uint16_t get16(const uint8_t *p)
{
    return (uint16_t)((uint32_t)p[0] | ((uint32_t)p[1] << 8));
}

static uint32_t get32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static void event(struct fixture *f, char value)
{
    CHECK(f->event_count + 1 < sizeof f->events);
    f->events[f->event_count++] = value;
    f->events[f->event_count] = '\0';
}

static void record_write(struct fixture *f, uint32_t address,
                         uint32_t value, unsigned width)
{
    CHECK(address <= MEMORY_SIZE - width);
    CHECK(f->write_count < sizeof f->writes / sizeof f->writes[0]);
    f->writes[f->write_count++] = (struct write_record){address, value, width};
}

static uint8_t load8(void *user, uint32_t address)
{
    struct fixture *f = user;
    CHECK(address < MEMORY_SIZE);
    ++f->reads;
    return f->memory[address];
}

static uint16_t load16(void *user, uint32_t address)
{
    struct fixture *f = user;
    CHECK(address <= MEMORY_SIZE - 2);
    ++f->reads;
    return get16(f->memory + address);
}

static uint32_t load32(void *user, uint32_t address)
{
    struct fixture *f = user;
    CHECK(address <= MEMORY_SIZE - 4);
    ++f->reads;
    return get32(f->memory + address);
}

static void store8(void *user, uint32_t address, uint8_t value)
{
    struct fixture *f = user;
    record_write(f, address, value, 1);
    f->memory[address] = value;
}

static void store16(void *user, uint32_t address, uint16_t value)
{
    struct fixture *f = user;
    record_write(f, address, value, 2);
    put16(f->memory + address, value);
}

static void store32(void *user, uint32_t address, uint32_t value)
{
    struct fixture *f = user;
    record_write(f, address, value, 4);
    put32(f->memory + address, value);
}

static unsigned zeroed_bytes(const struct fixture *f, uint32_t start,
                             uint32_t size)
{
    unsigned count = 0;
    for (unsigned i = 0; i < f->write_count; ++i) {
        const struct write_record *w = f->writes + i;
        if (w->address >= start && w->address + w->width <= start + size &&
            w->value == 0) count += w->width;
    }
    return count;
}

static uint32_t unused_random_word(void *user)
{
    (void)user;
    CHECK(0);
    return 0;
}

static uint32_t unused_random_below(void *user, uint32_t bound)
{
    (void)user; (void)bound;
    CHECK(0);
    return 0;
}

static uint32_t unused_pair(void *user, uint32_t a, uint32_t b)
{
    (void)user; (void)a; (void)b;
    CHECK(0);
    return 0;
}

static void unused_species_base(void *user, uint32_t species)
{
    (void)user; (void)species;
    CHECK(0);
}

static void unused_species(void *user, uint32_t species, uint32_t form)
{
    (void)user; (void)species; (void)form;
    CHECK(0);
}

static uint32_t resolve_gender(void *user, uint32_t species,
                                uint32_t form, uint32_t value)
{
    struct fixture *f = user;
    CHECK(species == 25 && form == 2 && value == 1);
    event(f, 'G');
    return f->gender_result;
}

static void reset(struct fixture *f)
{
    memset(f, 0, sizeof *f);
    memset(f->memory, 0xa5, sizeof f->memory);
    memset(f->normalized_guard, 0x6d, sizeof f->normalized_guard);
    put32(f->parameters, UINT32_C(0x12345678));
    put32(f->parameters + 8, UINT32_C(0x87654321));
    put32(f->parameters + 0x10, UINT32_C(0x11223344));
    put16(f->parameters + 0x18, 25);
    f->parameters[0x1a] = 2;
    f->parameters[0x1b] = 0xa7;
    put16(f->parameters + 0x1c, 50);
    put16(f->parameters + 0x1e, 1);
    put16(f->parameters + 0x20, 3);
    f->parameters[0x22] = 1;
    f->parameters[0x23] = 1;
    for (unsigned i = 0; i < 6; ++i) put16(f->parameters + 0x24 + 2 * i, i);
    put32(f->parameters + 0x30, 100);
    f->parameters[0x35] = 0xb1;
    f->parameters[0x36] = 0xb2;
    f->parameters[0x37] = 0xb3;
    f->data_result = DATA;
    f->manager_result = MANAGER;
    f->extra_result = EXTRA;
    f->expected_manager = MANAGER;
    f->object = OBJECT;
    f->expected_key = UINT32_C(0x12345678);
    f->gender_result = 2;
}

static uint32_t allocate(void *user, uint32_t size, uint32_t heap)
{
    struct fixture *f = user;
    CHECK(heap == HEAP);
    ++f->allocations;
    if (size == PX_POKEMON_DATA_SIZE) {
        event(f, 'D');
        return f->data_result;
    }
    if (size == PX_POKEMON_MANAGER_SIZE) {
        event(f, 'M');
        if (f->mutate_manager_allocation) {
            put32(f->memory + f->object + 4, DATA_B);
            put32(f->memory + DATA_B, UINT32_C(0xdeadbeef));
        }
        return f->manager_result;
    }
    CHECK(size == PX_POKEMON_EXTRA_SIZE);
    event(f, 'X');
    if (f->mutate_extra_allocation) {
        put32(f->memory + f->object + 0xc, MANAGER_C);
        put32(f->memory + f->object + 4, DATA);
        f->expected_manager = MANAGER_C;
    }
    return f->extra_result;
}

static void populate_data(void *user, uint32_t data, const uint8_t *parameters)
{
    struct fixture *f = user;
    CHECK(data == f->data_result);
    CHECK(parameters == f->normalized_guard + 1);
    CHECK(get16(parameters + 0x1e) == (uint16_t)f->gender_result);
    CHECK(parameters[0x1b] == 0xa7 && parameters[0x35] == 0xb1 &&
          parameters[0x36] == 0xb2 && parameters[0x37] == 0xb3);
    for (unsigned i = 0; i < PX_POKEMON_DATA_SIZE; ++i) {
        CHECK(f->memory[data + i] == 0);
    }
    f->data_clear_bytes = zeroed_bytes(f, data, PX_POKEMON_DATA_SIZE);
    put32(f->memory + data, UINT32_C(0x12345678));
    /* The source constructor must overwrite this when create_data returns. */
    put32(f->memory + f->object + 4, DATA_B);
    event(f, 'P');
    ++f->populations;
}

static void initialize_base_state(void *user, uint32_t object)
{
    struct fixture *f = user;
    CHECK(object == f->object);
    CHECK(get32(f->memory + object) == UINT32_C(0x00598a88));
    CHECK(get32(f->memory + object + 8) == 0);
    CHECK(get32(f->memory + object + 0xc) == f->manager_result);
    uint32_t data = f->mutate_manager_allocation ? DATA_B : f->data_result;
    CHECK(get32(f->memory + object + 4) == data);
    CHECK(get32(f->memory + f->manager_result + 8) == data);
    CHECK(get32(f->memory + f->manager_result + 4) == 0);
    CHECK(f->memory[f->manager_result + 0xc] == 1);
    CHECK(f->memory[f->manager_result + 0xd] == 0);
    if (f->manager_result != 0) {
        CHECK(get32(f->memory + f->manager_result) == UINT32_C(0x00598a78));
        CHECK(f->memory[f->manager_result + 0xe] == 0);
        CHECK(f->memory[f->manager_result + 0xf] == 0);
    }
    if (f->mutate_base) {
        put32(f->memory + object + 0xc, MANAGER_B);
        put32(f->memory + object + 4, DATA_B);
        put32(f->memory + MANAGER_B + 8, DATA_B);
        put32(f->memory + DATA_B, UINT32_C(0x12345678));
        f->memory[MANAGER_B + 0xd] = 0;
        f->expected_manager = MANAGER_B;
    }
    event(f, 'B');
}

static void decode_manager(void *user, uint32_t manager)
{
    struct fixture *f = user;
    CHECK(manager == f->expected_manager);
    event(f, 'd');
    ++f->decodes;
    f->memory[manager + 0xc] = 0;
    if (f->mutate_decode) {
        put32(f->memory + f->object + 0xc, MANAGER_B);
        put32(f->memory + MANAGER_B + 4, f->extra_result);
        f->memory[MANAGER_B + 0xc] = 0;
        f->memory[MANAGER_B + 0xd] = 1;
        f->expected_manager = MANAGER_B;
    }
}

static void encode_manager(void *user, uint32_t manager)
{
    struct fixture *f = user;
    CHECK(manager == f->expected_manager);
    CHECK(f->memory[manager + 0xd] == 0);
    event(f, 'e');
    ++f->encodes;
    f->memory[manager + 0xc] = 1;
    if (f->mutate_encode) {
        put32(f->memory + f->object + 0xc, MANAGER_D);
        f->memory[manager + 0xd] = 0x80;
    }
}

static void initialize_extra_state(void *user, uint32_t manager)
{
    struct fixture *f = user;
    CHECK(manager == f->expected_manager);
    CHECK(get32(f->memory + f->object) == UINT32_C(0x00598a50));
    CHECK(get32(f->memory + manager + 4) == f->extra_result);
    event(f, 'I');
    if (f->mutate_extra) {
        put32(f->memory + f->object + 0xc, MANAGER_C);
        f->memory[MANAGER_C + 0xd] = 1;
        f->expected_manager = MANAGER_C;
    }
}

static void update_derived_state(void *user, uint32_t object, uint32_t mode)
{
    struct fixture *f = user;
    CHECK(object == f->object && mode == 1);
    event(f, 'U');
}

static void refresh_state(void *user, uint32_t object)
{
    struct fixture *f = user;
    CHECK(object == f->object);
    event(f, 'R');
    if (f->mutate_refresh) {
        put32(f->memory + object + 0xc, MANAGER_D);
        f->memory[MANAGER_D + 0xd] = 1;
        f->expected_manager = MANAGER_D;
    }
}

static struct px_parameter_api make_parameter_api(struct fixture *f)
{
    const struct px_parameter_api api = {
        f, unused_random_word, unused_random_below, unused_pair, unused_pair,
        unused_pair, unused_species_base, unused_random_word, unused_species,
        unused_random_below, resolve_gender
    };
    return api;
}

static struct px_pokemon_api make_api(struct fixture *f,
                                      const struct px_parameter_api *parameters)
{
    const struct px_pokemon_api api = {
        f, parameters, load8, load16, load32, store8, store16, store32,
        allocate, populate_data, initialize_base_state, decode_manager,
        encode_manager, initialize_extra_state, update_derived_state, refresh_state
    };
    return api;
}

static void check_guards(const struct fixture *f)
{
    CHECK(f->normalized_guard[0] == 0x6d);
    CHECK(f->normalized_guard[sizeof f->normalized_guard - 1] == 0x6d);
}

static void test_defaults(void)
{
    uint8_t guarded[PX_POKEMON_PARAMETERS_SIZE + 2];
    uint8_t expected[PX_POKEMON_PARAMETERS_SIZE];
    memset(guarded, 0xa5, sizeof guarded);
    memset(expected, 0xa5, sizeof expected);
    for (unsigned i = 0; i < 0x18; i += 4) put32(expected + i, UINT32_MAX);
    put32(expected + 0xc, 3);
    put16(expected + 0x18, 0x2d2);
    expected[0x1a] = 0;
    put16(expected + 0x1c, 1);
    put16(expected + 0x1e, 0xff);
    put16(expected + 0x20, 0xffff);
    expected[0x22] = 0xff;
    expected[0x23] = 1;
    for (unsigned i = 0; i < 6; ++i) put16(expected + 0x24 + 2 * i, 0xffff);
    put32(expected + 0x30, 0xffff);
    expected[0x34] = 0;
    px_default_pokemon_parameters(guarded + 1);
    CHECK(memcmp(guarded + 1, expected, sizeof expected) == 0);
    CHECK(guarded[0] == 0xa5 && guarded[sizeof guarded - 1] == 0xa5);
}

static const uint16_t xor_vector[14] = {
    0x0b71, 0x84ea, 0xd98a, 0xf4e0, 0x2684, 0x9837, 0x91ac,
    0x0585, 0x731c, 0x620f, 0xc452, 0x10e8, 0x7687, 0x985a
};

static void test_xor(void)
{
    struct fixture f;
    reset(&f);
    struct px_parameter_api parameters = make_parameter_api(&f);
    struct px_pokemon_api api = make_api(&f, &parameters);
    uint8_t original[29];
    for (unsigned i = 0; i < sizeof original; ++i) original[i] = (uint8_t)(3 * i + 7);
    memcpy(f.memory + EXTRA, original, sizeof original);
    px_xor_pokemon_data(EXTRA, sizeof original, UINT32_C(0x12345678), &api);
    for (unsigned i = 0; i < 14; ++i) {
        CHECK(get16(f.memory + EXTRA + 2 * i) ==
              (uint16_t)(get16(original + 2 * i) ^ xor_vector[i]));
    }
    CHECK(f.memory[EXTRA + 28] == original[28]);
    CHECK(f.memory[EXTRA - 1] == 0xa5 && f.memory[EXTRA + 29] == 0xa5);
    px_xor_pokemon_data(EXTRA, sizeof original, UINT32_C(0x12345678), &api);
    CHECK(memcmp(f.memory + EXTRA, original, sizeof original) == 0);
    unsigned reads = f.reads, writes = f.write_count;
    px_xor_pokemon_data(UINT32_MAX, 0, UINT32_MAX, &api);
    px_xor_pokemon_data(UINT32_MAX, 1, UINT32_MAX, &api);
    CHECK(f.reads == reads && f.write_count == writes);
    const uint32_t keys[] = {0, UINT32_MAX, UINT32_C(0x80000000)};
    const uint16_t words[][4] = {
        {0x0000, 0xe97e, 0x5271, 0x31b0},
        {0xbe3a, 0x26db, 0xd1f3, 0x43aa},
        {0x8000, 0x697e, 0xd271, 0xb1b0}
    };
    for (unsigned k = 0; k < 3; ++k) {
        memset(f.memory + EXTRA, 0, 8);
        px_xor_pokemon_data(EXTRA, 8, keys[k], &api);
        for (unsigned i = 0; i < 4; ++i) CHECK(get16(f.memory + EXTRA + 2 * i) == words[k][i]);
    }
}

static void test_manager_and_extra(void)
{
    struct fixture f;
    reset(&f);
    struct px_parameter_api parameters = make_parameter_api(&f);
    struct px_pokemon_api api = make_api(&f, &parameters);
    CHECK(px_initialize_pokemon_manager(MANAGER, &api) == MANAGER);
    CHECK(get32(f.memory + MANAGER) == UINT32_C(0x00598a78));
    for (unsigned i = 4; i < 16; ++i) CHECK(f.memory[MANAGER + i] == 0);
    CHECK(f.memory[MANAGER - 1] == 0xa5 && f.memory[MANAGER + 16] == 0xa5);
    f.memory[MANAGER + 0xe] = 0x6e;
    f.memory[MANAGER + 0xf] = 0x6f;
    px_attach_pokemon_buffers(MANAGER, DATA, EXTRA, &api);
    CHECK(get32(f.memory + MANAGER + 8) == DATA);
    CHECK(get32(f.memory + MANAGER + 4) == EXTRA);
    CHECK(f.memory[MANAGER + 0xc] == 1 && f.memory[MANAGER + 0xd] == 0);
    CHECK(f.memory[MANAGER + 0xe] == 0x6e && f.memory[MANAGER + 0xf] == 0x6f);
    for (unsigned failure = 0; failure < 2; ++failure) {
        reset(&f);
        f.extra_result = failure ? 0 : EXTRA;
        uint32_t result = px_create_pokemon_extra(HEAP, UINT32_C(0x12345678), &api);
        CHECK(result == f.extra_result);
        CHECK(strcmp(f.events, "X") == 0);
        CHECK(zeroed_bytes(&f, result, PX_POKEMON_EXTRA_SIZE) ==
              (failure ? PX_POKEMON_EXTRA_SIZE : 2 * PX_POKEMON_EXTRA_SIZE));
        for (unsigned i = 0; i < 14; ++i) CHECK(get16(f.memory + result + 2 * i) == xor_vector[i]);
        CHECK(f.memory[result + PX_POKEMON_EXTRA_SIZE] == 0xa5);
    }
}

static void test_state_wrappers(void)
{
    const uint32_t masks[] = {
        0, 1, 2, 0x80, 0x100, 0x10000, UINT32_C(0x80000000),
        UINT32_C(0xffffff00), UINT32_MAX
    };
    struct fixture f;
    struct px_parameter_api parameters = make_parameter_api(&f);
    struct px_pokemon_api api = make_api(&f, &parameters);
    for (unsigned state = 0; state < 256; ++state) {
        reset(&f);
        put32(f.memory + OBJECT + 0xc, MANAGER);
        f.memory[MANAGER + 0xd] = (uint8_t)state;
        CHECK(px_acquire_pokemon_state(OBJECT, &api) == (state == 0));
        CHECK(f.decodes == (state == 0));
        CHECK(f.memory[MANAGER + 0xd] == (state == 0 ? 1 : state));
        for (unsigned m = 0; m < sizeof masks / sizeof masks[0]; ++m) {
            reset(&f);
            put32(f.memory + OBJECT + 0xc, MANAGER);
            f.memory[MANAGER + 0xd] = (uint8_t)state;
            uint32_t extended = state < 128 ? state : UINT32_C(0xffffff00) | state;
            uint32_t expected = (extended & masks[m]) != 0;
            CHECK(px_restore_pokemon_state(OBJECT, masks[m], &api) == expected);
            CHECK(f.encodes == expected);
            CHECK(f.memory[MANAGER + 0xd] == (expected ? 0 : state));
        }
    }
    reset(&f);
    put32(f.memory + OBJECT + 0xc, MANAGER);
    f.memory[MANAGER + 0xd] = 0;
    f.mutate_decode = 1;
    CHECK(px_acquire_pokemon_state(OBJECT, &api) == 1);
    CHECK(f.memory[MANAGER + 0xd] == 1);
    CHECK(get32(f.memory + OBJECT + 0xc) == MANAGER_B);
    f.mutate_encode = 1;
    CHECK(px_restore_pokemon_state(OBJECT, 1, &api) == 1);
    CHECK(strcmp(f.events, "de") == 0);
    CHECK(f.memory[MANAGER_B + 0xd] == 0x80);
    CHECK(get32(f.memory + OBJECT + 0xc) == MANAGER_D);
}

static void test_data_and_base(void)
{
    struct fixture f;
    struct px_parameter_api parameters = make_parameter_api(&f);
    struct px_pokemon_api api = make_api(&f, &parameters);
    for (unsigned failure = 0; failure < 2; ++failure) {
        reset(&f);
        f.data_result = failure ? 0 : DATA;
        CHECK(px_create_pokemon_data(HEAP, f.parameters, f.normalized_guard + 1, &api) == f.data_result);
        CHECK(strcmp(f.events, "GDP") == 0 && f.populations == 1);
        CHECK(f.data_clear_bytes == (failure ? PX_POKEMON_DATA_SIZE : 2 * PX_POKEMON_DATA_SIZE));
        CHECK(get32(f.memory + f.data_result) == UINT32_C(0x12345678));
        check_guards(&f);
    }
    for (unsigned failure = 0; failure < 2; ++failure) {
        reset(&f);
        f.manager_result = failure ? 0 : MANAGER;
        f.mutate_manager_allocation = 1;
        CHECK(px_initialize_pokemon_base(OBJECT, HEAP, f.parameters, f.normalized_guard + 1, &api) == OBJECT);
        CHECK(strcmp(f.events, "GDPMB") == 0);
        CHECK(f.writes[0].address == OBJECT + 4 && f.writes[0].value == 0);
        CHECK(f.writes[1].address == OBJECT && f.writes[1].value == UINT32_C(0x00598a88));
        CHECK(f.writes[2].address == OBJECT + 8 && f.writes[2].value == 0);
        CHECK(f.writes[3].address == OBJECT + 0xc && f.writes[3].value == 0);
        if (failure) CHECK(get32(f.memory) == UINT32_C(0xa5a5a5a5));
        check_guards(&f);
    }
}

static void test_complete_constructor(void)
{
    struct fixture f;
    struct px_parameter_api parameters = make_parameter_api(&f);
    struct px_pokemon_api api = make_api(&f, &parameters);
    for (unsigned variant = 0; variant < 5; ++variant) {
        reset(&f);
        if (variant == 1) f.data_result = 0;
        if (variant == 2) f.manager_result = f.expected_manager = 0;
        if (variant == 3) f.extra_result = 0;
        if (variant == 4) f.object = 0;
        CHECK(px_construct_pokemon(f.object, HEAP, f.parameters,
                                   f.normalized_guard + 1, &api) == f.object);
        CHECK(strcmp(f.events, "GDPMBXdIURe") == 0);
        CHECK(f.allocations == 3 && f.populations == 1 && f.decodes == 1 && f.encodes == 1);
        CHECK(get32(f.memory + f.object) == UINT32_C(0x00598a50));
        CHECK(get32(f.memory + f.object + 4) == f.data_result);
        CHECK(get32(f.memory + f.object + 8) == f.extra_result);
        CHECK(get32(f.memory + f.object + 0xc) == f.manager_result);
        CHECK(f.memory[f.manager_result + 0xd] == 0);
        for (unsigned i = 0; i < 14; ++i) CHECK(get16(f.memory + f.extra_result + 2 * i) == xor_vector[i]);
        check_guards(&f);
    }
    reset(&f);
    f.mutate_base = f.mutate_extra_allocation = 1;
    CHECK(px_construct_pokemon(OBJECT, HEAP, f.parameters,
                               f.normalized_guard + 1, &api) == OBJECT);
    CHECK(get32(f.memory + OBJECT + 0xc) == MANAGER_C);
    CHECK(get32(f.memory + MANAGER_C + 8) == DATA);
    CHECK(get32(f.memory + MANAGER_C + 4) == EXTRA);
    for (unsigned i = 0; i < 14; ++i) CHECK(get16(f.memory + EXTRA + 2 * i) == xor_vector[i]);
    CHECK(strcmp(f.events, "GDPMBXdIURe") == 0);
    reset(&f);
    f.mutate_decode = f.mutate_extra = f.mutate_refresh = 1;
    CHECK(px_construct_pokemon(OBJECT, HEAP, f.parameters,
                               f.normalized_guard + 1, &api) == OBJECT);
    CHECK(get32(f.memory + OBJECT + 0xc) == MANAGER_D);
    CHECK(f.memory[MANAGER + 0xd] == 1 && f.memory[MANAGER_D + 0xd] == 0);
    CHECK(strcmp(f.events, "GDPMBXdIURe") == 0);
}

int main(void)
{
    test_defaults();
    test_xor();
    test_manager_and_extra();
    test_state_wrappers();
    test_data_and_base();
    test_complete_constructor();
    test_pokemon_parameters();
    test_pokemon_constructor_chain();
    puts("PASS: constructor sequencing; raw zero allocations; preserved parameter holes; XOR vectors/round trip; 256 acquire states and 2304 restore masks; callback mutation; parameter normalization; five-milestone integration");
    return EXIT_SUCCESS;
}
