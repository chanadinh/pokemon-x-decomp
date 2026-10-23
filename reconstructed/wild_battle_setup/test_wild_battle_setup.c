#include "wild_battle_setup.h"
#include "../call_wild_battle/call_wild_battle.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #condition); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

static uint32_t read_le32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* Independent byte-level expectations derived from the ARM store offsets. */
static void expected_initializer(uint8_t *expected)
{
    static const unsigned zero_bytes[] = {
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x84, 0x85, 0x86, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8f,
        0x90, 0x91, 0x92, 0x93, 0x95, 0x96, 0x97, 0x98,
        0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa4, 0xa5, 0xa6, 0xa7
    };
    memset(expected, 0xa5, PX_WILD_WORKSPACE_SIZE);
    memset(expected, 0, 0x6c);
    expected[0x6c] = 1;
    expected[0x6d] = expected[0x6e] = expected[0x6f] = 0;
    expected[0x8e] = 0x40;
    for (size_t i = 0; i < sizeof zero_bytes / sizeof zero_bytes[0]; ++i) {
        expected[zero_bytes[i]] = 0;
    }
}

static void test_initializer(void)
{
    uint8_t guarded[PX_WILD_WORKSPACE_SIZE + 2];
    uint8_t expected[PX_WILD_WORKSPACE_SIZE];
    memset(guarded, 0xa5, sizeof guarded);
    expected_initializer(expected);
    CHECK(px_init_wild_workspace(guarded + 1) == guarded + 1);
    CHECK(memcmp(guarded + 1, expected, sizeof expected) == 0);
    CHECK(guarded[0] == 0xa5);
    CHECK(guarded[sizeof guarded - 1] == 0xa5);
}

static void expected_patch(uint8_t *expected, const uint8_t *record)
{
    const unsigned flags = (unsigned)record[6] + 256u * record[7];
    static const unsigned one_offsets[] = {0x6c, 0x70, 0x74};
    for (size_t i = 0; i < sizeof one_offsets / sizeof one_offsets[0]; ++i) {
        const unsigned offset = one_offsets[i];
        expected[offset] = 1;
        expected[offset + 1] = expected[offset + 2] = expected[offset + 3] = 0;
    }
    expected[0] = record[0];
    expected[1] = record[1];
    expected[2] = record[4];
    expected[3] = record[5];
    expected[4] = record[3];
    expected[5] = record[2];
    expected[6] = (uint8_t)(flags % 4u);
    expected[7] = (uint8_t)((flags / 16u) % 8u);
    expected[0x10] = (uint8_t)((flags / 4u) % 4u);
    expected[0x8c] = record[8];
    expected[0x8d] = record[9];
    expected[0x8e] = record[10];
    expected[0x8f] = (uint8_t)((flags / 128u) % 256u);
}

static void test_record_decoding(void)
{
    uint8_t guarded[PX_WILD_WORKSPACE_SIZE + 2];
    uint8_t expected[PX_WILD_WORKSPACE_SIZE];
    uint8_t record[PX_WILD_RECORD_SIZE] = {
        0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0, 0, 0xde, 0xf0, 0x81, 0x92
    };
    uint8_t original[PX_WILD_RECORD_SIZE];

    for (unsigned flags = 0; flags < 65536u; ++flags) {
        record[6] = (uint8_t)(flags % 256u);
        record[7] = (uint8_t)(flags / 256u);
        memcpy(original, record, sizeof original);
        memset(guarded, 0xa5, sizeof guarded);
        memset(expected, 0xa5, sizeof expected);
        expected_patch(expected, record);
        px_patch_wild_record(guarded + 1, record);
        CHECK(memcmp(guarded + 1, expected, sizeof expected) == 0);
        CHECK(memcmp(record, original, sizeof original) == 0);
        CHECK(guarded[0] == 0xa5);
        CHECK(guarded[sizeof guarded - 1] == 0xa5);
    }

    /* The unused last record byte must not affect any output byte. */
    record[11] ^= 0xff;
    memset(guarded + 1, 0xa5, PX_WILD_WORKSPACE_SIZE);
    px_patch_wild_record(guarded + 1, record);
    CHECK(memcmp(guarded + 1, expected, sizeof expected) == 0);
}

struct px_cwb_context {
    uint32_t address;
};

struct fixture {
    uint8_t *workspace;
    uint8_t record[PX_WILD_RECORD_SIZE];
    uint32_t optional_object;
    uint32_t manager;
    uint32_t context;
    uint32_t finish_target;
    uint32_t stored_address;
    uint32_t stored_value;
    uint32_t expected_index;
    uint32_t expected_mode;
    struct px_cwb_context host_context;
    char events[32];
    size_t event_count;
};

static void event(struct fixture *f, char value)
{
    CHECK(f->event_count + 1 < sizeof f->events);
    f->events[f->event_count++] = value;
    f->events[f->event_count] = '\0';
}

static uint32_t load32(void *user, uint32_t address)
{
    struct fixture *f = user;
    if (address == f->context + UINT32_C(0x34)) {
        event(f, 'f');
        return f->finish_target;
    }
    if (address == f->context + UINT32_C(0x0c)) {
        event(f, 'h');
        return UINT32_C(0x1000);
    }
    switch (address) {
    case 0x1048: event(f, 'o'); return f->optional_object;
    case 0x2158: event(f, 'a'); return UINT32_C(0x3000);
    case 0x300c: event(f, 'b'); return UINT32_C(0x4000);
    case 0x4148: event(f, 'c'); return UINT32_C(0xfedcba98);
    case 0x102c: event(f, 'd'); return UINT32_C(0x5000);
    case 0x1018: event(f, 'm'); return f->manager;
    default: CHECK(0); return 0;
    }
}

static uint8_t load8(void *user, uint32_t address)
{
    struct fixture *f = user;
    CHECK(address == UINT32_C(0x5034));
    event(f, 'e');
    return UINT8_C(0xe7);
}

static void store32(void *user, uint32_t address, uint32_t value)
{
    struct fixture *f = user;
    event(f, 'w');
    f->stored_address = address;
    f->stored_value = value;
}

static const uint8_t *get_record(void *user, uint32_t index)
{
    struct fixture *f = user;
    CHECK(index == f->expected_index); /* Direct helper calls must not narrow. */
    event(f, 'r');
    CHECK(read_le32(f->workspace + 0x6c) == 1);
    CHECK(read_le32(f->workspace + 0x70) == 1);
    CHECK(read_le32(f->workspace + 0x74) == 1);
    CHECK(f->record[0] == 0x45); /* Setup changed the row before lookup. */
    return f->record;
}

static void setup(void *user, uint8_t *workspace, uint32_t field,
                  uint32_t one, uint32_t optional, uint32_t ff,
                  uint32_t context_byte)
{
    struct fixture *f = user;
    uint8_t expected[PX_WILD_WORKSPACE_SIZE];
    event(f, 's');
    CHECK(workspace == f->workspace);
    expected_initializer(expected);
    CHECK(memcmp(workspace, expected, sizeof expected) == 0);
    CHECK(field == UINT32_C(0x1000));
    CHECK(one == 1);
    CHECK(optional == (f->optional_object ? UINT32_C(0xfedcba98) : 0));
    CHECK(ff == 255);
    CHECK(context_byte == UINT32_C(0xe7));
    /* Later loads and patching must observe setup's changes. */
    f->record[0] = 0x45;
    f->record[1] = 0x67;
    f->record[6] = 0xd6;
    f->record[7] = 0xa9;
    f->manager = UINT32_C(0xabcdef01);
    workspace[0x94] = 0x33;
    memset(workspace + 0x6c, 0xcc, 12);
}

static uint32_t submit(void *user, uint32_t manager, uint32_t field,
                       const uint8_t *workspace, uint32_t mode)
{
    struct fixture *f = user;
    uint8_t expected[PX_WILD_WORKSPACE_SIZE];
    event(f, 't');
    CHECK(manager == UINT32_C(0xabcdef01));
    CHECK(field == UINT32_C(0x1000));
    CHECK(workspace == f->workspace);
    CHECK(mode == f->expected_mode);
    expected_initializer(expected);
    expected[0x94] = 0x33;
    expected_patch(expected, f->record);
    CHECK(memcmp(workspace, expected, sizeof expected) == 0);
    return UINT32_C(0xa1b2c3d4);
}

static struct px_wild_api make_api(struct fixture *f)
{
    const struct px_wild_api api = {
        f, load32, load8, store32, get_record, setup, submit
    };
    return api;
}

static void test_prepare(int optional_present)
{
    uint8_t workspace[PX_WILD_WORKSPACE_SIZE];
    struct fixture f = {0};
    f.workspace = workspace;
    f.optional_object = optional_present ? UINT32_C(0x2000) : 0;
    f.context = UINT32_C(0x8000);
    f.expected_index = UINT32_C(0x12345678);
    f.expected_mode = UINT32_C(0x98765432);
    memset(workspace, 0xa5, sizeof workspace);
    memset(f.record, 0x72, sizeof f.record);
    const struct px_wild_api api = make_api(&f);
    CHECK(px_prepare_wild_battle(UINT32_C(0x1000), UINT32_C(0x12345678),
                                UINT32_C(0x98765432), workspace, &api)
          == UINT32_C(0xa1b2c3d4));
    CHECK(strcmp(f.events, optional_present ? "oabcdesrmt" : "odesrmt") == 0);
}

static void test_finish(uint32_t context, uint32_t target)
{
    struct fixture f = {0};
    f.context = context;
    f.finish_target = target;
    const struct px_wild_api api = make_api(&f);
    CHECK(px_finish_wild_script(context, &api) == 0);
    CHECK(strcmp(f.events, "fw") == 0);
    CHECK(f.stored_address == target + UINT32_C(0x58));
    CHECK(f.stored_value == 12);
}

static struct px_cwb_context *wrapper_get_context(void *user)
{
    struct fixture *f = user;
    event(f, 'g');
    return &f->host_context;
}

static uint32_t wrapper_read_field(void *user, struct px_cwb_context *context)
{
    struct fixture *f = user;
    CHECK(context == &f->host_context);
    return load32(user, context->address + UINT32_C(0x0c));
}

static void wrapper_field_helper(void *user, uint32_t field,
                                  uint32_t argument, uint32_t zero)
{
    struct fixture *f = user;
    const struct px_wild_api api = make_api(f);
    CHECK(zero == 0);
    CHECK(px_prepare_wild_battle(field, argument, zero, f->workspace, &api)
          == UINT32_C(0xa1b2c3d4));
}

static void wrapper_context_helper(void *user, struct px_cwb_context *context)
{
    struct fixture *f = user;
    const struct px_wild_api api = make_api(f);
    CHECK(context == &f->host_context);
    CHECK(px_finish_wild_script(context->address, &api) == 0);
}

static void test_original_wrapper_integration(void)
{
    uint8_t workspace[PX_WILD_WORKSPACE_SIZE];
    const uint32_t args[2] = {UINT32_C(0x87654321), UINT32_C(0xdead5678)};
    struct fixture f = {0};
    f.workspace = workspace;
    f.optional_object = UINT32_C(0x2000);
    f.context = UINT32_C(0x8000);
    f.finish_target = UINT32_C(0x9000);
    f.expected_index = UINT32_C(0x5678);
    f.expected_mode = 0;
    f.host_context.address = f.context;
    memset(workspace, 0xa5, sizeof workspace);
    memset(f.record, 0x72, sizeof f.record);
    const struct px_cwb_api wrapper_api = {
        &f, wrapper_get_context, wrapper_read_field,
        wrapper_field_helper, wrapper_context_helper
    };
    CHECK(px_call_wild_battle(args, &wrapper_api) == 0);
    CHECK(strcmp(f.events, "ghoabcdesrmtfw") == 0);
    CHECK(f.stored_address == UINT32_C(0x9058));
    CHECK(f.stored_value == 12);
    CHECK(args[0] == UINT32_C(0x87654321));
    CHECK(args[1] == UINT32_C(0xdead5678));
}

int main(void)
{
    test_initializer();
    test_record_decoding();
    test_prepare(0);
    test_prepare(1);
    test_finish(UINT32_C(0x8000), UINT32_C(0x9000));
    test_finish(UINT32_C(0xfffffff0), UINT32_C(0xfffffff0));
    test_original_wrapper_integration();
    puts("PASS: 65536 flag values; initializer holes; row decoding; optional paths; call order; mutation timing; return propagation; finisher; original wrapper integration");
    return EXIT_SUCCESS;
}
