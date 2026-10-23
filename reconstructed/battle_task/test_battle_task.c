#include "battle_task.h"
#include "../call_wild_battle/call_wild_battle.h"
#include "../wild_battle_setup/wild_battle_setup.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #condition); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

enum {
    MEMORY_SIZE = 0x8000, MANAGER = 0x200, TASK = 0x1000, FIELD = 0x2000,
    CONTEXT_A = 0x3000, CONTEXT_B = 0x4000, CONTEXT_C = 0x4500,
    LOCATION_A = 0x5000, LOCATION_B = 0x5100,
    AUX_STORAGE = 0x6000, AUX_CONSTRUCTED = 0x6100,
    AUX_POPULATED = 0x6200, AUX_SELECTED = 0x6300, OTHER_STATE = 0x7000,
    INLINE_STATE = TASK + 0x2c
};

static const uint32_t initial_flags = UINT32_C(0x10200040);
static const uint32_t old_head = UINT32_C(0x76543210);

struct px_cwb_context {
    uint32_t address;
};

struct fixture {
    uint8_t memory[MEMORY_SIZE];
    uint8_t request[0xac];
    uint8_t record[PX_WILD_RECORD_SIZE];
    struct px_cwb_context script_context;
    uint32_t submitted_task;
    unsigned heap_calls;
    unsigned task_allocations;
    unsigned aux_allocations;
    unsigned count;
    uint32_t last_heap;
    uint32_t consumer_heap;
    int allocation_fails;
    int retarget_during_decode;
    char events[32];
    size_t event_count;
};

static void event(struct fixture *f, char c)
{
    CHECK(f->event_count + 1 < sizeof f->events);
    f->events[f->event_count++] = c;
    f->events[f->event_count] = '\0';
}

static void put16(uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
}

static void put32(uint8_t *p, uint32_t value)
{
    for (unsigned i = 0; i < 4; ++i) {
        p[i] = (uint8_t)(value >> (i * 8));
    }
}

static uint8_t load8(void *user, uint32_t address)
{
    struct fixture *f = user;
    CHECK(address < MEMORY_SIZE);
    return f->memory[address];
}

static uint16_t load16(void *user, uint32_t address)
{
    struct fixture *f = user;
    CHECK(address < MEMORY_SIZE - 1);
    return (uint16_t)((unsigned)f->memory[address] +
                      256u * f->memory[address + 1]);
}

static uint32_t load32(void *user, uint32_t address)
{
    struct fixture *f = user;
    CHECK(address < MEMORY_SIZE - 3);
    uint32_t value = 0;
    for (unsigned i = 0; i < 4; ++i) {
        value += (uint32_t)f->memory[address + i] << (i * 8);
    }
    return value;
}

static void store8(void *user, uint32_t address, uint8_t value)
{
    struct fixture *f = user;
    CHECK(address < MEMORY_SIZE);
    f->memory[address] = value;
}

static void store32(void *user, uint32_t address, uint32_t value)
{
    struct fixture *f = user;
    CHECK(address < MEMORY_SIZE - 3);
    put32(f->memory + address, value);
}

static uint32_t heap_for_id(void *user, uint32_t id)
{
    struct fixture *f = user;
    CHECK(id == 8);
    event(f, 'H');
    /* Distinct returns catch incorrectly merging separate accessor calls. */
    f->last_heap = UINT32_C(0xf0000000) + ++f->heap_calls;
    return f->last_heap;
}

static uint32_t allocate(void *user, uint32_t size, uint32_t heap)
{
    struct fixture *f = user;
    CHECK(heap == f->last_heap);
    if (size == PX_BATTLE_TASK_SIZE) {
        event(f, 'T');
        ++f->task_allocations;
        return f->allocation_fails ? 0 : TASK;
    }
    CHECK(size == PX_BATTLE_AUX_SIZE);
    event(f, 'A');
    ++f->aux_allocations;
    f->consumer_heap = heap;
    return AUX_STORAGE;
}

static void init_scene(void *user, uint32_t destination, uint32_t field,
                       uint32_t primary, uint32_t secondary, uint32_t zero)
{
    struct fixture *f = user;
    event(f, 'S');
    CHECK(destination == TASK + 0x230);
    CHECK(field == FIELD);
    CHECK(primary == 0x12);
    CHECK(secondary == 0x34);
    CHECK(zero == 0);
    /* Location later uses the saved context; category later rereads it. */
    store32(f, FIELD + 0x1c, CONTEXT_B);
    f->request[0x8c] = 0xfe;
}

static uint32_t construct_aux(void *user, uint32_t storage, uint32_t heap)
{
    struct fixture *f = user;
    event(f, 'C');
    CHECK(storage == AUX_STORAGE);
    CHECK(heap == f->consumer_heap);
    return AUX_CONSTRUCTED;
}

static void populate_aux(void *user, const uint8_t *request,
                         uint32_t heap, uint32_t aux)
{
    struct fixture *f = user;
    event(f, 'P');
    CHECK(request == f->request);
    CHECK(heap == f->consumer_heap);
    CHECK(aux == AUX_CONSTRUCTED);
    CHECK(load32(f, TASK + 0x258) == AUX_CONSTRUCTED);
    store32(f, TASK + 0x258, AUX_POPULATED);
    store8(f, AUX_POPULATED + 0x18, (uint8_t)f->count);
}

static uint32_t select_config(void *user, const uint8_t *request)
{
    struct fixture *f = user;
    event(f, 'Q');
    CHECK(request == f->request);
    store32(f, TASK + 0x258, AUX_SELECTED);
    /* The count must already have been sampled before this callback. */
    store8(f, AUX_POPULATED + 0x18, f->count > 1 ? 0 : 255);
    store8(f, AUX_SELECTED + 0x18, f->count > 1 ? 0 : 255);
    return UINT32_C(0xcafebabe);
}

static void init_state(void *user, uint32_t destination, uint32_t field,
                       uint32_t aux, uint32_t scene, uint32_t extra,
                       uint32_t config, uint32_t heap)
{
    struct fixture *f = user;
    event(f, 'I');
    CHECK(destination == INLINE_STATE);
    CHECK(field == FIELD);
    CHECK(aux == AUX_SELECTED);
    CHECK(scene == TASK + 0x230);
    CHECK(extra == (f->count > 1 ? 4u : 0u));
    CHECK(config == UINT32_C(0xcafebabe));
    CHECK(heap == f->consumer_heap);
    store32(f, INLINE_STATE + 0xa8, initial_flags);
    store32(f, OTHER_STATE + 0xa8, UINT32_C(0x08000000));
    /* Caller must overwrite this after the callback returns. */
    store32(f, TASK + 0x20, OTHER_STATE);
}

static void decode_environment(void *user, uint32_t primary,
                               uint32_t secondary, uint8_t *output)
{
    struct fixture *f = user;
    event(f, 'D');
    CHECK(primary == 0x12); /* Initial snapshot, despite scene mutation. */
    CHECK(secondary == 0x56);
    CHECK(load32(f, TASK + 0x20) == INLINE_STATE);
    for (unsigned i = 0; i < 10; ++i) {
        output[i] = (uint8_t)(0x70 + i);
    }
    if (f->retarget_during_decode) {
        store32(f, TASK + 0x20, OTHER_STATE);
        store32(f, FIELD + 0x1c, CONTEXT_C);
    }
}

static struct px_battle_task_api make_api(struct fixture *f)
{
    const struct px_battle_task_api api = {
        f, load8, load16, load32, store8, store32, heap_for_id, allocate,
        init_scene, construct_aux, populate_aux, select_config,
        init_state, decode_environment
    };
    return api;
}

static void reset(struct fixture *f, unsigned selector, unsigned category,
                  unsigned count, uint16_t location, int optional)
{
    memset(f, 0, sizeof *f);
    memset(f->memory, 0xa5, sizeof f->memory);
    f->count = count;
    f->request[0x8c] = 0x12;
    f->request[0x8d] = 0x34;
    f->request[0x8f] = (uint8_t)selector;
    put32(f->request + 0x80, UINT32_C(0x12563478));
    put32(f->request + 0xa4, optional ? 2 : 0);
    store32(f, MANAGER, old_head);
    store32(f, FIELD + 0x1c, CONTEXT_A);
    store32(f, CONTEXT_A + 0x28, LOCATION_A);
    store32(f, CONTEXT_B + 0x28, LOCATION_B);
    put16(f->memory + LOCATION_A + 0x0c, location);
    put16(f->memory + LOCATION_B + 0x0c, 0x102);
    put16(f->memory + CONTEXT_A + 0x128, 0xa500);
    put16(f->memory + CONTEXT_B + 0x128, (uint16_t)(0xa500u + category));
    put16(f->memory + CONTEXT_C + 0x128, 0xbe07);
}

static uint32_t expected_mask(unsigned selector, unsigned category)
{
    uint32_t result = 0;
    if (selector == 1) result = 0x80;
    if (selector == 2) result = 0x04;
    if (selector == 3) result = 0x200;
    switch (category) {
    case 3: case 11: result |= 0x4000; break;
    case 4: case 5: case 6: case 7: result |= 0x8000; break;
    default: break;
    }
    return result;
}

static void test_factory(void)
{
    struct fixture f;
    uint8_t expected[PX_BATTLE_TASK_SIZE];
    reset(&f, 0, 0, 0, 0, 0);
    memset(expected, 0xa5, sizeof expected);
    static const unsigned zeros[] = {8, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x258, 0x264};
    for (size_t i = 0; i < sizeof zeros / sizeof zeros[0]; ++i) {
        put32(expected + zeros[i], 0);
    }
    put32(expected, UINT32_C(0x0059911c));
    put32(expected + 4, old_head);
    put32(expected + 0x0c, UINT32_C(0xf0000002));
    memset(expected + 0x28, 0, 4);
    expected[0x262] = 0;
    put32(expected + 0x268, UINT32_MAX);
    const struct px_battle_task_api api = make_api(&f);
    CHECK(px_create_battle_task(MANAGER, &api) == TASK);
    CHECK(strcmp(f.events, "HTH") == 0);
    CHECK(f.task_allocations == 1 && f.aux_allocations == 0);
    CHECK(memcmp(f.memory + TASK, expected, sizeof expected) == 0);
    CHECK(load32(&f, MANAGER) == TASK);
    CHECK(f.memory[TASK - 1] == 0xa5);
    CHECK(f.memory[TASK + PX_BATTLE_TASK_SIZE] == 0xa5);

    /* This models raw accesses, not a claim that null is safe in the game. */
    reset(&f, 0, 0, 0, 0, 0);
    f.allocation_fails = 1;
    CHECK(px_create_battle_task(MANAGER, &api) == 0);
    CHECK(strcmp(f.events, "HT") == 0);
    CHECK(load32(&f, 4) == old_head);
    CHECK(load32(&f, MANAGER) == 0);
}

static void test_flag_matrix(void)
{
    struct fixture f;
    const struct px_battle_task_api api = make_api(&f);
    for (unsigned selector = 0; selector < 256; ++selector) {
        for (unsigned category = 0; category < 256; ++category) {
            reset(&f, selector, category, 1, 0x102, 0);
            px_consume_wild_request(TASK, FIELD, f.request, UINT32_C(0xabcdef00), &api);
            CHECK(strcmp(f.events, "HSACPQI") == 0);
            CHECK(load32(&f, INLINE_STATE + 0xa8) ==
                  (initial_flags | expected_mask(selector, category)));
            CHECK(load32(&f, TASK + 0x20) == INLINE_STATE);
            CHECK(load32(&f, TASK + 0x1c) == UINT32_C(0xabcdef00));
            CHECK(load8(&f, TASK + 0x28) == 1);
            CHECK(load8(&f, TASK + 0x29) == 0xa5);
            CHECK(load8(&f, TASK + 0x2a) == 0xa5);
            CHECK(load8(&f, INLINE_STATE + 0xb2) == 0xa5);
        }
    }
}

static void test_optional_and_locations(void)
{
    static const unsigned counts[] = {0, 1, 2, 255};
    static const uint16_t locations[] = {0x102, 0x103, 0x104, 0x11d, 0x11e, 0x11f};
    struct fixture f;
    const struct px_battle_task_api api = make_api(&f);
    for (size_t i = 0; i < sizeof counts / sizeof counts[0]; ++i) {
        for (size_t j = 0; j < sizeof locations / sizeof locations[0]; ++j) {
            reset(&f, 2, 11, counts[i], locations[j], 1);
            px_consume_wild_request(TASK, FIELD, f.request, UINT32_C(0xabcdef01), &api);
            CHECK(strcmp(f.events, "HSACPQID") == 0);
            CHECK(load32(&f, INLINE_STATE + 0xa8) == (initial_flags | UINT32_C(0x4005)));
            CHECK(load8(&f, INLINE_STATE + 0x0a) == 0x72);
            CHECK(load8(&f, TASK + 0x29) == 1);
            CHECK(load8(&f, TASK + 0x2a) == 1);
            CHECK(load8(&f, INLINE_STATE + 0xb2) ==
                  ((locations[j] == 0x103 || locations[j] == 0x11e) ? 1 : 0xa5));
        }
    }

    reset(&f, 3, 3, 2, 0x103, 1);
    f.retarget_during_decode = 1;
    px_consume_wild_request(TASK, FIELD, f.request, 0, &api);
    CHECK(load32(&f, INLINE_STATE + 0xa8) == (initial_flags | UINT32_C(0x201)));
    CHECK(load32(&f, OTHER_STATE + 0xa8) == UINT32_C(0x08008000));
    CHECK(load8(&f, INLINE_STATE + 0x0a) == 0x72);
    CHECK(load8(&f, OTHER_STATE + 0x0a) == 0xa5);
    CHECK(load32(&f, TASK + 0x20) == OTHER_STATE);
}

static void test_key_decoder(void)
{
    for (uint32_t high = 0; high < 256; ++high) {
        for (uint32_t middle = 0; middle < 256; ++middle) {
            const uint32_t value = high * UINT32_C(0x1000000) +
                                   middle * UINT32_C(0x10000) + UINT32_C(0x9876);
            CHECK(px_decode_optional_key(value) == (high == 255 ? 0 : middle));
        }
    }
    CHECK(px_decode_optional_key(UINT32_MAX) == 0);
    CHECK(px_decode_optional_key(0) == 0);
}

static void test_submit(void)
{
    struct fixture f;
    reset(&f, 1, 5, 2, 0x11e, 1);
    const struct px_battle_task_api api = make_api(&f);
    CHECK(px_submit_wild_request(MANAGER, FIELD, f.request, UINT32_C(0xfedcba98), &api) == TASK);
    CHECK(strcmp(f.events, "HTHHSACPQID") == 0);
    CHECK(f.task_allocations == 1 && f.aux_allocations == 1);
    CHECK(f.heap_calls == 3);
    CHECK(load32(&f, TASK + 0x0c) == UINT32_C(0xf0000002));
    CHECK(load32(&f, TASK + 4) == old_head);
    CHECK(load32(&f, MANAGER) == TASK);
    CHECK(load32(&f, TASK + 0x1c) == UINT32_C(0xfedcba98));
    CHECK(load8(&f, TASK + 0x29) == 0); /* Factory zero survives even mode. */
    CHECK(load32(&f, INLINE_STATE + 0xa8) == (initial_flags | UINT32_C(0x8081)));
}

static const uint8_t *integration_record(void *user, uint32_t index)
{
    struct fixture *f = user;
    event(f, 'R');
    CHECK(index == UINT32_C(0x5678));
    return f->record;
}

static void integration_setup(void *user, uint8_t *workspace, uint32_t field,
                              uint32_t one, uint32_t optional, uint32_t ff,
                              uint32_t context_byte)
{
    struct fixture *f = user;
    event(f, 'W');
    CHECK(workspace == f->request);
    CHECK(field == FIELD && one == 1 && optional == 0);
    CHECK(ff == 255 && context_byte == 0xe7);
    put32(workspace + 0x80, UINT32_C(0x12563478));
    put32(workspace + 0xa4, 2);
}

static uint32_t integration_submit(void *user, uint32_t manager, uint32_t field,
                                   const uint8_t *request, uint32_t mode)
{
    struct fixture *f = user;
    const struct px_battle_task_api api = make_api(f);
    CHECK(manager == MANAGER && field == FIELD && mode == 0);
    f->submitted_task = px_submit_wild_request(manager, field, request, mode, &api);
    return f->submitted_task;
}

static struct px_wild_api integration_prepare_api(struct fixture *f)
{
    const struct px_wild_api api = {
        f, load32, load8, store32, integration_record,
        integration_setup, integration_submit
    };
    return api;
}

static struct px_cwb_context *integration_get_context(void *user)
{
    struct fixture *f = user;
    event(f, 'g');
    return &f->script_context;
}

static uint32_t integration_read_field(void *user, struct px_cwb_context *context)
{
    struct fixture *f = user;
    event(f, 'f');
    CHECK(context == &f->script_context);
    return load32(f, context->address + 0x0c);
}

static void integration_prepare(void *user, uint32_t field,
                                uint32_t argument, uint32_t zero)
{
    struct fixture *f = user;
    const struct px_wild_api api = integration_prepare_api(f);
    CHECK(px_prepare_wild_battle(field, argument, zero, f->request, &api) == TASK);
}

static void integration_finish(void *user, struct px_cwb_context *context)
{
    struct fixture *f = user;
    const struct px_wild_api api = integration_prepare_api(f);
    CHECK(context == &f->script_context);
    CHECK(px_finish_wild_script(context->address, &api) == 0);
}

static void test_three_milestone_integration(void)
{
    struct fixture f;
    reset(&f, 0, 5, 2, 0x11e, 0);
    f.script_context.address = 0x400;
    store32(&f, 0x40c, FIELD);
    store32(&f, 0x434, 0x5400);
    store32(&f, FIELD + 0x18, MANAGER);
    store32(&f, FIELD + 0x48, 0);
    store32(&f, FIELD + 0x2c, 0x5200);
    store8(&f, 0x5234, 0xe7);
    f.record[6] = 0x80; /* Decodes buffer selector +0x8f as 1. */
    f.record[8] = 0x12;
    f.record[9] = 0x34;
    const uint32_t args[2] = {UINT32_C(0x11223344), UINT32_C(0xdead5678)};
    const struct px_cwb_api api = {
        &f, integration_get_context, integration_read_field,
        integration_prepare, integration_finish
    };
    CHECK(px_call_wild_battle(args, &api) == 0);
    CHECK(strcmp(f.events, "gfWRHTHHSACPQID") == 0);
    CHECK(f.submitted_task == TASK);
    CHECK(load32(&f, MANAGER) == TASK);
    CHECK(load32(&f, INLINE_STATE + 0xa8) == (initial_flags | UINT32_C(0x8081)));
    CHECK(load32(&f, 0x5458) == 12);
    CHECK(args[0] == UINT32_C(0x11223344));
    CHECK(args[1] == UINT32_C(0xdead5678));
}

int main(void)
{
    test_factory();
    test_flag_matrix();
    test_optional_and_locations();
    test_key_decoder();
    test_submit();
    test_three_milestone_integration();
    puts("PASS: 65536 selector/category pairs; 65536 key pairs; task factory and registration; optional paths; count boundaries; pointer mutation timing; full submit; all three source milestones integrated");
    return EXIT_SUCCESS;
}
