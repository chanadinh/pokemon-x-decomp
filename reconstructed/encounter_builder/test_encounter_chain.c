#include "encounter_builder.h"
#include "../battle_task/battle_task.h"
#include "../wild_battle_setup/wild_battle_setup.h"
#include "../call_wild_battle/call_wild_battle.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { \
    fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #c); \
    exit(EXIT_FAILURE); \
} } while (0)

enum {
    MEM_SIZE = 0x8000, MANAGER = 0x200, TASK = 0x1000, AUX = 0x2000,
    TEMP = 0x3000, VTABLE = 0x3100, FIELD = 0x4000, FIELD_CONTEXT = 0x5000,
    LOCATION = 0x5200, BYTE_OBJECT = 0x5400, SCRIPT_CONTEXT = 0x6000,
    FINISH_OBJECT = 0x6800, WEIGHTS = 0x7200
};

struct px_cwb_context { uint32_t address; };

struct chain_fixture {
    uint8_t memory[MEM_SIZE];
    uint8_t workspace[PX_WILD_WORKSPACE_SIZE];
    uint8_t record[PX_WILD_RECORD_SIZE];
    uint8_t parameters[PX_ENCOUNTER_PARAMS_SIZE];
    uint8_t temporary_parameters[PX_ENCOUNTER_PARAMS_SIZE];
    uint8_t retained_parameters[PX_ENCOUNTER_PARAMS_SIZE];
    uint32_t temporary_value;
    uint32_t retained_value;
    uint32_t submitted_task;
    unsigned heap_calls;
    unsigned append_calls;
    unsigned destroy_calls;
    struct px_cwb_context script_context;
    char events[64];
    size_t event_count;
};

static void event(struct chain_fixture *f, char c)
{
    CHECK(f->event_count + 1 < sizeof f->events);
    f->events[f->event_count++] = c;
    f->events[f->event_count] = '\0';
}

static uint32_t read32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static void write32(uint8_t *p, uint32_t value)
{
    for (unsigned i = 0; i < 4; ++i) p[i] = (uint8_t)(value >> (8 * i));
}

static uint8_t load8(void *user, uint32_t address)
{
    struct chain_fixture *f = user;
    CHECK(address < MEM_SIZE);
    return f->memory[address];
}

static uint16_t load16(void *user, uint32_t address)
{
    struct chain_fixture *f = user;
    CHECK(address < MEM_SIZE - 1);
    return (uint16_t)((unsigned)f->memory[address] + 256u * f->memory[address + 1]);
}

static uint32_t load32(void *user, uint32_t address)
{
    struct chain_fixture *f = user;
    CHECK(address < MEM_SIZE - 3);
    return read32(f->memory + address);
}

static void store8(void *user, uint32_t address, uint8_t value)
{
    struct chain_fixture *f = user;
    CHECK(address < MEM_SIZE);
    f->memory[address] = value;
}

static void store32(void *user, uint32_t address, uint32_t value)
{
    struct chain_fixture *f = user;
    CHECK(address < MEM_SIZE - 3);
    write32(f->memory + address, value);
}

static uint32_t heap_for_id(void *user, uint32_t id)
{
    struct chain_fixture *f = user;
    CHECK(id == 8);
    event(f, 'H');
    ++f->heap_calls;
    return UINT32_C(0x11112222);
}

static uint32_t allocate(void *user, uint32_t size, uint32_t heap)
{
    struct chain_fixture *f = user;
    CHECK(heap == UINT32_C(0x11112222));
    switch (size) {
    case PX_BATTLE_TASK_SIZE: event(f, 'T'); return TASK;
    case PX_BATTLE_AUX_SIZE: event(f, 'A'); return AUX;
    case 16: event(f, 'B'); return TEMP;
    default: CHECK(0); return 0;
    }
}

static void select_species(void *user, uint32_t species, uint32_t form)
{
    struct chain_fixture *f = user;
    event(f, 'L');
    CHECK(species == UINT32_C(0x1234));
    CHECK(form == 7);
}

static uint32_t personal_value(void *user, uint32_t selector)
{
    struct chain_fixture *f = user;
    if (selector == 0x18) { event(f, 'X'); return 2; }
    CHECK(selector == 0x19);
    event(f, 'Y');
    return 3;
}

static uint32_t context_index(void *user, uint32_t context)
{
    (void)user; (void)context;
    CHECK(0); return 0;
}

static uint32_t context_probe(void *user, uint32_t context, uint32_t index)
{
    (void)user; (void)context; (void)index;
    CHECK(0); return 0;
}

static uint32_t random_below(void *user, uint32_t bound)
{
    (void)user; (void)bound;
    CHECK(0); return 0;
}

static uint32_t construct(void *user, uint32_t storage, uint32_t heap,
                           const uint8_t *parameters)
{
    struct chain_fixture *f = user;
    event(f, 'N');
    CHECK(storage == TEMP && heap == UINT32_C(0x11112222));
    CHECK(parameters == f->parameters);
    CHECK(parameters[0x18] == 0x34 && parameters[0x19] == 0x12);
    CHECK(parameters[0x1a] == 7);
    CHECK(parameters[0x1c] == 20 && parameters[0x1d] == 0);
    CHECK(parameters[0x1e] == 1 && parameters[0x1f] == 0);
    CHECK(parameters[0x22] == 1 && parameters[0x23] == 1);
    CHECK(read32(parameters + 0x0c) == 2);
    CHECK(read32(parameters + 0x10) == UINT32_C(0xabcdef01));
    CHECK(read32(parameters + 0x14) == 0);
    CHECK(parameters[0x1b] == 0xa5 && parameters[0x35] == 0xa5);
    memcpy(f->temporary_parameters, parameters, sizeof f->temporary_parameters);
    store32(f, TEMP, VTABLE);
    store32(f, VTABLE + 8, UINT32_C(0x0039a728));
    return TEMP;
}

static void set_value(void *user, uint32_t object, uint32_t value)
{
    struct chain_fixture *f = user;
    event(f, 'V');
    CHECK(object == TEMP && value == 37);
    f->temporary_value = value;
}

static void append(void *user, uint32_t destination, uint32_t object)
{
    struct chain_fixture *f = user;
    event(f, 'J');
    CHECK(destination == AUX && object == TEMP);
    CHECK(f->destroy_calls == 0 && f->append_calls == 0);
    CHECK(load8(f, AUX + 0x18) == 0);
    memcpy(f->retained_parameters, f->temporary_parameters, sizeof f->retained_parameters);
    f->retained_value = f->temporary_value;
    ++f->append_calls;
    store8(f, AUX + 0x18, 1);
}

static void destroy_virtual(void *user, uint32_t function, uint32_t object)
{
    struct chain_fixture *f = user;
    event(f, 'D');
    CHECK(function == UINT32_C(0x0039a728) && object == TEMP);
    CHECK(f->append_calls == 1 && f->retained_value == 37);
    ++f->destroy_calls;
    memset(f->temporary_parameters, 0xdd, sizeof f->temporary_parameters);
    f->temporary_value = UINT32_MAX;
    store32(f, TEMP, 0);
}

static struct px_encounter_api encounter_api(struct chain_fixture *f)
{
    const struct px_encounter_api api = {
        f, WEIGHTS, load8, load32, select_species, personal_value,
        context_index, context_probe, allocate, construct, random_below,
        set_value, append, destroy_virtual
    };
    return api;
}

static void init_scene(void *user, uint32_t destination, uint32_t field,
                       uint32_t primary, uint32_t secondary, uint32_t zero)
{
    struct chain_fixture *f = user;
    event(f, 'S');
    CHECK(destination == TASK + 0x230 && field == FIELD);
    CHECK(primary == 0x12 && secondary == 0x34 && zero == 0);
}

static uint32_t construct_aux(void *user, uint32_t storage, uint32_t heap)
{
    struct chain_fixture *f = user;
    event(f, 'C');
    CHECK(storage == AUX && heap == UINT32_C(0x11112222));
    store8(f, AUX + 0x18, 0);
    return AUX;
}

static void populate_aux(void *user, const uint8_t *request,
                         uint32_t heap, uint32_t aux)
{
    struct chain_fixture *f = user;
    event(f, 'P');
    CHECK(request == f->workspace && aux == AUX);
    const struct px_encounter_api api = encounter_api(f);
    CHECK(px_populate_encounters(request, heap, aux, f->parameters, &api) == 1);
}

static uint32_t select_config(void *user, const uint8_t *request)
{
    struct chain_fixture *f = user;
    event(f, 'Q');
    const struct px_encounter_api api = encounter_api(f);
    const uint32_t result = px_select_encounter_config(request, &api);
    CHECK(result == 0x21);
    return result;
}

static void init_state(void *user, uint32_t destination, uint32_t field,
                       uint32_t aux, uint32_t scene, uint32_t extra,
                       uint32_t config, uint32_t heap)
{
    struct chain_fixture *f = user;
    event(f, 'I');
    CHECK(destination == TASK + 0x2c && field == FIELD && aux == AUX);
    CHECK(scene == TASK + 0x230 && extra == 0 && config == 0x21);
    CHECK(heap == UINT32_C(0x11112222));
    CHECK(load8(f, AUX + 0x18) == 1 && f->destroy_calls == 1);
    CHECK(f->retained_parameters[0x18] == 0x34 && f->retained_value == 37);
    store32(f, destination + 0xa8, UINT32_C(0x100));
}

static void decode_environment(void *user, uint32_t primary,
                               uint32_t secondary, uint8_t *output)
{
    (void)user; (void)primary; (void)secondary; (void)output;
    CHECK(0);
}

static struct px_battle_task_api task_api(struct chain_fixture *f)
{
    const struct px_battle_task_api api = {
        f, load8, load16, load32, store8, store32, heap_for_id, allocate,
        init_scene, construct_aux, populate_aux, select_config, init_state,
        decode_environment
    };
    return api;
}

static const uint8_t *get_record(void *user, uint32_t index)
{
    struct chain_fixture *f = user;
    event(f, 'R');
    CHECK(index == UINT32_C(0x1234));
    return f->record;
}

static void setup(void *user, uint8_t *workspace, uint32_t field,
                  uint32_t one, uint32_t optional, uint32_t ff,
                  uint32_t context_byte)
{
    struct chain_fixture *f = user;
    event(f, 'W');
    CHECK(workspace == f->workspace && field == FIELD);
    CHECK(one == 1 && optional == 0 && ff == 255 && context_byte == 0xe7);
    write32(workspace + 0x9c, UINT32_C(0xabcdef01));
    workspace[0x84] = 255;
    workspace[0x85] = 1;
}

static uint32_t submit(void *user, uint32_t manager, uint32_t field,
                       const uint8_t *request, uint32_t mode)
{
    struct chain_fixture *f = user;
    CHECK(manager == MANAGER && field == FIELD && mode == 0);
    const struct px_battle_task_api api = task_api(f);
    f->submitted_task = px_submit_wild_request(manager, field, request, mode, &api);
    return f->submitted_task;
}

static struct px_wild_api prepare_api(struct chain_fixture *f)
{
    const struct px_wild_api api = {
        f, load32, load8, store32, get_record, setup, submit
    };
    return api;
}

static struct px_cwb_context *get_context(void *user)
{
    struct chain_fixture *f = user;
    event(f, 'G');
    return &f->script_context;
}

static uint32_t read_field(void *user, struct px_cwb_context *context)
{
    struct chain_fixture *f = user;
    CHECK(context == &f->script_context);
    return load32(f, context->address + 0x0c);
}

static void field_helper(void *user, uint32_t field, uint32_t argument, uint32_t zero)
{
    struct chain_fixture *f = user;
    const struct px_wild_api api = prepare_api(f);
    CHECK(px_prepare_wild_battle(field, argument, zero, f->workspace, &api) == TASK);
}

static void context_helper(void *user, struct px_cwb_context *context)
{
    struct chain_fixture *f = user;
    event(f, 'F');
    CHECK(context == &f->script_context);
    const struct px_wild_api api = prepare_api(f);
    CHECK(px_finish_wild_script(context->address, &api) == 0);
}

void test_encounter_chain(void)
{
    struct chain_fixture f = {0};
    memset(f.parameters, 0xa5, sizeof f.parameters);
    f.script_context.address = SCRIPT_CONTEXT;
    store32(&f, SCRIPT_CONTEXT + 0x0c, FIELD);
    store32(&f, SCRIPT_CONTEXT + 0x34, FINISH_OBJECT);
    store32(&f, FIELD + 0x18, MANAGER);
    store32(&f, FIELD + 0x1c, FIELD_CONTEXT);
    store32(&f, FIELD + 0x2c, BYTE_OBJECT);
    store8(&f, BYTE_OBJECT + 0x34, 0xe7);
    store32(&f, FIELD_CONTEXT + 0x28, LOCATION);
    store8(&f, LOCATION + 0x0c, 3);
    store8(&f, LOCATION + 0x0d, 1);
    store8(&f, FIELD_CONTEXT + 0x128, 5);
    const uint8_t row[12] = {0x34, 0x12, 7, 20, 37, 0, 0x29, 0, 0x12, 0x34, 0x21, 0xff};
    memcpy(f.record, row, sizeof row);
    const uint8_t weights[6] = {50, 5, 0, 60, 20, 0};
    memcpy(f.memory + WEIGHTS, weights, sizeof weights);
    const uint32_t args[2] = {UINT32_C(0x11223344), UINT32_C(0xdead1234)};
    const struct px_cwb_api api = {&f, get_context, read_field, field_helper, context_helper};

    CHECK(px_call_wild_battle(args, &api) == 0);
    CHECK(strcmp(f.events, "GWRHTHHSACPLXYBNVJDQIF") == 0);
    CHECK(f.heap_calls == 3 && f.append_calls == 1 && f.destroy_calls == 1);
    CHECK(f.submitted_task == TASK && load32(&f, MANAGER) == TASK);
    CHECK(load8(&f, AUX + 0x18) == 1);
    CHECK(load32(&f, TASK + 0x2c + 0xa8) == UINT32_C(0x8100));
    CHECK(load8(&f, TASK + 0x2c + 0xb2) == 1);
    CHECK(load32(&f, FINISH_OBJECT + 0x58) == 12);
    CHECK(f.retained_parameters[0x18] == 0x34 && f.retained_parameters[0x19] == 0x12);
    CHECK(f.retained_value == 37 && f.temporary_value == UINT32_MAX);
    CHECK(load32(&f, TEMP) == 0);
    CHECK(args[0] == UINT32_C(0x11223344) && args[1] == UINT32_C(0xdead1234));
    CHECK(memcmp(f.record, row, sizeof row) == 0);
}
