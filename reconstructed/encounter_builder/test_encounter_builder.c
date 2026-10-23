#include "encounter_builder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #condition); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

void test_encounter_chain(void);

enum { WEIGHTS = 0x100, CONFIG = 0x200, OBJECT = 0x300,
       OLD_VTABLE = 0x400, NEW_VTABLE = 0x500, DESTINATION = 0x800,
       CONTEXT = 0x900, HEAP = 0xa00 };

struct fixture {
    uint8_t request[0xac];
    uint8_t slot[PX_ENCOUNTER_SLOT_SIZE];
    uint8_t guarded[PX_ENCOUNTER_PARAMS_SIZE + 2];
    uint8_t captured[PX_ENCOUNTER_PARAMS_SIZE];
    uint8_t weights[6];
    uint32_t attr18, attr19, candidates[3], draw;
    uint32_t probe_result, selected, expected_species, expected_form;
    uint32_t vtable, config_offset, expected_object;
    unsigned selects, attr18_calls, attr19_calls, candidate_calls;
    unsigned allocation_calls, construction_calls, set_calls, append_calls;
    unsigned destroy_calls, random_calls, weight_reads, config_reads, context_calls;
    int mutate_preload, mutate_context, mutate_construct, mutate_random;
    int fail_allocation, zero_construct, population_mode;
    uint16_t constructed_value;
    uint32_t population_next_count;
    char events[512];
    size_t event_count;
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

static void event(struct fixture *f, char value)
{
    CHECK(f->event_count + 1 < sizeof f->events);
    f->events[f->event_count++] = value;
    f->events[f->event_count] = '\0';
}

static uint8_t load8(void *user, uint32_t address)
{
    struct fixture *f = user;
    if (address >= WEIGHTS && address < WEIGHTS + 6) {
        event(f, 'w');
        ++f->weight_reads;
        return f->weights[address - WEIGHTS];
    }
    CHECK(address == CONFIG + 4 || address == CONFIG + 5);
    ++f->config_reads;
    f->config_offset = address - CONFIG;
    return address == CONFIG + 4 ? 0xe4 : 0xe5;
}

static uint32_t load32(void *user, uint32_t address)
{
    struct fixture *f = user;
    if (address == OBJECT) {
        event(f, 'v');
        return f->vtable;
    }
    CHECK(address == OLD_VTABLE + 8 || address == NEW_VTABLE + 8);
    event(f, 'f');
    return address == NEW_VTABLE + 8 ? UINT32_C(0xcafebabe) : UINT32_C(0xdeadbeef);
}

static void select_species(void *user, uint32_t species, uint32_t form)
{
    struct fixture *f = user;
    event(f, 'S');
    if (f->population_mode) {
        CHECK(species == UINT32_C(0x1200) + f->selects);
        CHECK(form == 5);
    } else {
        CHECK(species == f->expected_species);
        CHECK(form == f->expected_form);
    }
    ++f->selects;
    if (f->mutate_preload) {
        put16(f->slot, 0xabcd);
        f->slot[4] = 77;
        f->slot[5] = 9;
        f->slot[0x10] = 2;
        f->slot[7] = 3;
        f->slot[0x11] = 8;
        put32(f->request + 0x9c, UINT32_C(0x87654321));
        put32(f->request + 0xa4, UINT32_C(0x12));
        put16(f->request + 0xa2, UINT32_C(0x8001));
    }
}

static uint32_t personal_value(void *user, uint32_t selector)
{
    struct fixture *f = user;
    if (selector == 0x18) {
        event(f, 'a'); ++f->attr18_calls; return f->attr18;
    }
    if (selector == 0x19) {
        event(f, 'b'); ++f->attr19_calls; return f->attr19;
    }
    CHECK(selector >= 0x11 && selector <= 0x13);
    event(f, (char)('x' + selector - 0x11));
    ++f->candidate_calls;
    return f->candidates[selector - 0x11];
}

static uint32_t context_index(void *user, uint32_t context)
{
    struct fixture *f = user;
    event(f, 'i');
    CHECK(context == CONTEXT);
    ++f->context_calls;
    if (f->mutate_context) {
        put32(f->request + 0x78, UINT32_C(0xbad));
        put32(f->request + 0xa4, UINT32_C(0x400));
        f->request[0x97] = 0xed;
        f->request[0x85] = 1;
    }
    return UINT32_C(0xfedcba98);
}

static uint32_t context_probe(void *user, uint32_t context, uint32_t index)
{
    struct fixture *f = user;
    event(f, 'j');
    CHECK(context == CONTEXT); /* Must retain the value before context_index. */
    CHECK(index == UINT32_C(0xfedcba98));
    return f->probe_result;
}

static uint32_t allocate(void *user, uint32_t size, uint32_t heap)
{
    struct fixture *f = user;
    event(f, 'L');
    CHECK(size == 16 && heap == HEAP);
    ++f->allocation_calls;
    return f->fail_allocation ? 0 : UINT32_C(0x1000);
}

static uint32_t construct(void *user, uint32_t storage, uint32_t heap,
                           const uint8_t *parameters)
{
    struct fixture *f = user;
    event(f, 'C');
    CHECK(storage == UINT32_C(0x1000) && heap == HEAP);
    CHECK(parameters == f->guarded + 1);
    memcpy(f->captured, parameters, sizeof f->captured);
    ++f->construction_calls;
    if (f->mutate_construct) put16(f->slot + 2, f->constructed_value);
    return f->zero_construct ? 0 : OBJECT;
}

static uint32_t random_below(void *user, uint32_t bound)
{
    struct fixture *f = user;
    event(f, 'R');
    CHECK(bound == 100);
    ++f->random_calls;
    if (f->mutate_random) {
        put32(f->request + 0xa4, UINT32_C(0x200));
        f->weights[3] = 10;
        f->weights[4] = 20;
        f->weights[5] = 30;
    }
    return f->draw;
}

static void set_value(void *user, uint32_t object, uint32_t value)
{
    struct fixture *f = user;
    event(f, 'V');
    CHECK(object == f->expected_object);
    f->selected = value;
    ++f->set_calls;
}

static void append(void *user, uint32_t destination, uint32_t object)
{
    struct fixture *f = user;
    event(f, 'P');
    CHECK(destination == DESTINATION && object == f->expected_object);
    ++f->append_calls;
    f->vtable = NEW_VTABLE;
    if (f->population_mode && f->append_calls == 1) {
        put32(f->request + 0x6c, f->population_next_count);
    }
}

static void destroy_virtual(void *user, uint32_t function, uint32_t object)
{
    struct fixture *f = user;
    event(f, 'D');
    CHECK(function == UINT32_C(0xcafebabe));
    CHECK(object == OBJECT);
    ++f->destroy_calls;
    f->vtable = OLD_VTABLE;
}

static struct px_encounter_api make_api(struct fixture *f)
{
    const struct px_encounter_api api = {
        f, WEIGHTS, load8, load32, select_species, personal_value,
        context_index, context_probe, allocate, construct, random_below,
        set_value, append, destroy_virtual
    };
    return api;
}

static void reset(struct fixture *f)
{
    memset(f, 0, sizeof *f);
    memset(f->guarded, 0xa5, sizeof f->guarded);
    put16(f->slot, 0x1234);
    put16(f->slot + 2, 716);
    f->slot[4] = 20;
    f->slot[5] = 5;
    f->slot[0x11] = 7;
    put32(f->request + 0x9c, UINT32_C(0x12345678));
    put32(f->request + 0x78, CONTEXT);
    f->request[0x85] = 1;
    f->attr18 = 1;
    f->attr19 = 2;
    f->candidates[0] = UINT32_C(0x10010);
    f->candidates[1] = UINT32_C(0x20020);
    f->candidates[2] = UINT32_C(0x30030);
    f->expected_species = 0x1234;
    f->expected_form = 5;
    f->expected_object = OBJECT;
    f->vtable = OLD_VTABLE;
    const uint8_t weights[6] = {50, 5, 0, 60, 20, 0};
    memcpy(f->weights, weights, sizeof weights);
}

static void run_build(struct fixture *f)
{
    const struct px_encounter_api api = make_api(f);
    px_build_encounter(f->request, HEAP, f->slot, DESTINATION, f->guarded + 1, &api);
    CHECK(f->guarded[0] == 0xa5);
    CHECK(f->guarded[sizeof f->guarded - 1] == 0xa5);
}

/* Byte expectations come from the ARM stack stores, independent of source helpers. */
static void expected_base(uint8_t *p)
{
    memset(p, 0xa5, PX_ENCOUNTER_PARAMS_SIZE);
    memset(p, 0xff, 12);
    put32(p + 12, 3);
    put32(p + 0x10, UINT32_C(0x12345678));
    put32(p + 0x14, 0);
    put16(p + 0x18, 0x1234);
    p[0x1a] = 5;
    put16(p + 0x1c, 20);
    put16(p + 0x1e, 255);
    put16(p + 0x20, 65535);
    p[0x22] = 255;
    p[0x23] = 1;
    memset(p + 0x24, 0xff, 12);
    put32(p + 0x30, 65535);
    p[0x34] = 7;
}

static void test_parameters_and_mutations(void)
{
    struct fixture f;
    uint8_t expected[PX_ENCOUNTER_PARAMS_SIZE];
    reset(&f); expected_base(expected); run_build(&f);
    CHECK(memcmp(f.captured, expected, sizeof expected) == 0);
    CHECK(strcmp(f.events, "SabLCVPvfD") == 0);
    CHECK(f.selected == 716);

    reset(&f); expected_base(expected); f.mutate_preload = 1;
    put16(expected + 0x1e, 1); expected[0x22] = 2;
    expected[0x23] = 5; expected[0x34] = 8;
    run_build(&f);
    CHECK(memcmp(f.captured, expected, sizeof expected) == 0);

    for (unsigned value = 0; value < 256; ++value) {
        reset(&f); expected_base(expected);
        f.slot[0x10] = (uint8_t)value;
        f.slot[7] = (uint8_t)value;
        put16(expected + 0x1e, value == 1 ? 0 : value == 2 ? 1 : 255);
        expected[0x22] = value >= 1 && value <= 3 ? (uint8_t)(value - 1) : 255;
        run_build(&f);
        CHECK(memcmp(f.captured, expected, sizeof expected) == 0);
    }
    for (unsigned value = 0; value < 4; ++value) {
        reset(&f); expected_base(expected);
        f.slot[6] = (uint8_t)value;
        put32(f.request + 0xa4, 0x12);
        put16(f.request + 0xa2, 0xffff);
        if (value == 1 || value == 2) put32(expected + 12, value == 1 ? 2 : 1);
        else expected[0x23] = 1; /* 3 + 2*65535, narrowed to one byte. */
        run_build(&f);
        CHECK(memcmp(f.captured, expected, sizeof expected) == 0);
    }
}

static void test_attribute_short_circuit_and_context(void)
{
    struct fixture f;
    reset(&f); f.attr18 = 15; run_build(&f);
    CHECK(f.attr18_calls == 1 && f.attr19_calls == 0);
    CHECK(f.captured[0x34] == 3);
    reset(&f); f.attr18 = UINT32_C(0x1000f); f.attr19 = 15; run_build(&f);
    CHECK(f.attr19_calls == 1 && f.captured[0x34] == 3);
    reset(&f); f.attr18 = UINT32_C(0x1000f); f.attr19 = UINT32_C(0x1000f); run_build(&f);
    CHECK(f.captured[0x34] == 7);
    for (unsigned result = 0; result < 3; ++result) {
        reset(&f); f.request[0x85] = 2; f.slot[7] = 1;
        f.mutate_context = 1; f.probe_result = result;
        run_build(&f);
        CHECK(f.context_calls == 1);
        CHECK(f.captured[0x22] == (result ? 0xfe : 0xff));
        CHECK(f.captured[0x23] == 5);
        CHECK(f.captured[0x34] == 2); /* Before callback changed mode to 1. */
        CHECK(f.captured[0x20] == 0xed && f.captured[0x21] == 0);
        CHECK(strcmp(f.events, "SabijLCVPvfD") == 0);
    }
}

static void test_explicit_values_and_zero_objects(void)
{
    static const uint16_t values[] = {1, 716, 717, 65535};
    struct fixture f;
    for (size_t i = 0; i < sizeof values / sizeof values[0]; ++i) {
        reset(&f); f.mutate_construct = 1; f.constructed_value = values[i];
        put16(f.slot + 2, 0); run_build(&f);
        CHECK(f.set_calls == (values[i] <= 716 ? 1u : 0u));
        if (f.set_calls) CHECK(f.selected == values[i]);
        CHECK(f.candidate_calls == 0 && f.random_calls == 0);
        CHECK(f.append_calls == 1 && f.destroy_calls == 1);
    }
    reset(&f); f.fail_allocation = 1; f.expected_object = 0; run_build(&f);
    CHECK(f.construction_calls == 0 && f.set_calls == 1 && f.append_calls == 1);
    CHECK(f.destroy_calls == 0 && strcmp(f.events, "SabLVP") == 0);
    reset(&f); f.zero_construct = 1; f.expected_object = 0; run_build(&f);
    CHECK(f.construction_calls == 1 && f.set_calls == 1 && f.append_calls == 1);
    CHECK(f.destroy_calls == 0 && strcmp(f.events, "SabLCVP") == 0);
}

static void test_weighted_selection(void)
{
    struct fixture f;
    for (unsigned row = 0; row < 2; ++row) {
        for (unsigned draw = 0; draw < 256; ++draw) {
            reset(&f); put16(f.slot + 2, 0);
            put32(f.request + 0xa4, row ? 0x200 : 0);
            f.draw = UINT32_C(0x12340000) + draw;
            run_build(&f);
            const unsigned first = row ? 60 : 50;
            const unsigned second = row ? 80 : 55;
            CHECK(f.selected == (draw < first ? 16u : draw < second ? 32u : 0u));
            CHECK(f.weight_reads == (draw < first ? 1u : draw < second ? 2u : 3u));
            CHECK(f.candidate_calls == 3 && f.random_calls == 1);
        }
    }
    /* Synthetic mutable rows exercise wrap, including a reachable third candidate. */
    for (unsigned draw = 0; draw < 256; ++draw) {
        reset(&f); put16(f.slot + 2, 0);
        f.weights[0] = 10; f.weights[1] = 250; f.weights[2] = 20;
        f.draw = draw; run_build(&f);
        CHECK(f.selected == (draw < 10 ? 16u : draw < 24 ? 48u : 0u));
    }
    reset(&f); put16(f.slot + 2, 0);
    f.candidates[0] = UINT32_C(0x1002a); f.candidates[1] = UINT32_C(0x2002a);
    run_build(&f);
    CHECK(f.selected == 42 && f.candidate_calls == 3);
    CHECK(f.random_calls == 0 && f.weight_reads == 0);
    reset(&f); put16(f.slot + 2, 0); f.mutate_random = 1; f.draw = 45;
    run_build(&f);
    CHECK(f.selected == 48 && f.weight_reads == 3);
    CHECK(strcmp(f.events, "SabLCxyzRwwwVPvfD") == 0);
}

static void test_population(void)
{
    static const struct { uint32_t initial, next; unsigned builds; uint32_t result; } cases[] = {
        {0, 0, 0, 0}, {UINT32_MAX, 0, 0, 1}, {UINT32_C(0x80000000), 0, 0, 1},
        {1, 3, 3, 1}, {3, 1, 1, 1}, {3, 0, 1, 1}, {3, UINT32_MAX, 1, 1}
    };
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i) {
        struct fixture f; reset(&f); f.population_mode = 1;
        f.population_next_count = cases[i].next;
        for (unsigned slot = 0; slot < 3; ++slot) {
            memcpy(f.request + slot * 18, f.slot, 18);
            put16(f.request + slot * 18, 0x1200 + slot);
        }
        put32(f.request + 0x6c, cases[i].initial);
        const struct px_encounter_api api = make_api(&f);
        CHECK(px_populate_encounters(f.request, HEAP, DESTINATION, f.guarded + 1, &api)
              == cases[i].result);
        CHECK(f.selects == cases[i].builds && f.append_calls == cases[i].builds);
    }
}

static void test_config_selection(void)
{
    struct fixture f;
    for (unsigned value = 0; value < 256; ++value) {
        reset(&f); f.request[0x8e] = (uint8_t)value;
        const struct px_encounter_api api = make_api(&f);
        CHECK(px_select_encounter_config(f.request, &api) == (value == 64 ? 0 : value));
        CHECK(f.config_reads == 0);
    }
    for (unsigned mode = 0; mode < 256; ++mode) {
        reset(&f); f.request[0x8e] = 64; f.request[0x84] = (uint8_t)mode;
        put32(f.request + 0x88, CONFIG);
        const struct px_encounter_api api = make_api(&f);
        CHECK(px_select_encounter_config(f.request, &api) == (mode == 10 ? 0xe5 : 0xe4));
        CHECK(f.config_reads == 1 && f.config_offset == (mode == 10 ? 5u : 4u));
    }
}

int main(void)
{
    test_parameters_and_mutations();
    test_attribute_short_circuit_and_context();
    test_explicit_values_and_zero_objects();
    test_weighted_selection();
    test_population();
    test_config_selection();
    test_encounter_chain();
    puts("PASS: parameter bytes and holes; branch boundaries; callback timing; 768 weighted draws; signed dynamic counts; 512 config cases; four-milestone integration");
    return EXIT_SUCCESS;
}
