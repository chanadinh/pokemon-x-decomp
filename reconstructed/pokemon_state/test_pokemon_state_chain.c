#include "pokemon_state.h"
#include "../encounter_builder/encounter_builder.h"
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
    TEMP = 0x3000, DATA = 0x3200, POKEMON_MANAGER = 0x3400, EXTRA = 0x3500,
    SCRATCH_MANAGER = 0x3600, COPIED_DATA = 0x3800, COPIED_EXTRA = 0x3900,
    COPIED_MANAGER = 0x3940, FIELD = 0x4000, FIELD_CONTEXT = 0x5000,
    LOCATION = 0x5200, BYTE_OBJECT = 0x5400, SCRIPT_CONTEXT = 0x6000,
    FINISH_OBJECT = 0x6800, WEIGHTS = 0x7200, SETTINGS = 0x7300,
    GROWTH_TABLE = 0x7400
};

struct px_cwb_context { uint32_t address; };

struct chain_fixture {
    uint8_t memory[MEM_SIZE];
    uint8_t workspace[PX_WILD_WORKSPACE_SIZE];
    uint8_t record[PX_WILD_RECORD_SIZE];
    uint8_t parameters[PX_ENCOUNTER_PARAMS_SIZE];
    uint8_t normalized[PX_POKEMON_PARAMETERS_SIZE];
    uint8_t retained_parameters[PX_POKEMON_PARAMETERS_SIZE];
    uint8_t retained_data[PX_POKEMON_DATA_SIZE];
    uint8_t retained_extra[PX_POKEMON_EXTRA_SIZE];
    uint8_t permutation_table[32 * 4];
    struct px_parameter_api parameter_api;
    struct px_pokemon_api pokemon_api;
    struct px_data_api data_api;
    struct px_state_api state_api;
    uint8_t name_scratch[PX_POKEMON_NAME_SCRATCH_SIZE];
    unsigned learn_calls, learnset_count_calls, maximum_pp_calls, state_level_calls;
    unsigned update_stat_calls;
    unsigned population_species_calls;
    unsigned population_attribute_calls;
    unsigned settings_reads;
    unsigned growth_select_calls;
    unsigned growth_table_reads;
    unsigned permutation_reads;
    unsigned allocation_count;
    unsigned constructor_phase;
    unsigned random_word_calls;
    unsigned random_below_calls;
    unsigned normalizer_species_calls;
    unsigned normalizer_attribute_calls;
    unsigned gender_calls;
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
    if (address >= UINT32_C(0x00570eec) && address < UINT32_C(0x00570f6c)) {
        ++f->permutation_reads;
        return f->permutation_table[address - UINT32_C(0x00570eec)];
    }
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
    if (address == UINT32_C(0x00598a58)) return UINT32_C(0x0039a728);
    if (address == UINT32_C(0x005d1870)) {
        CHECK(f->constructor_phase == 1 || f->constructor_phase == 3);
        ++f->settings_reads;
        return SETTINGS;
    }
    if (address == UINT32_C(0x005d17ac)) {
        CHECK(f->constructor_phase == 1 && f->growth_select_calls == 1);
        ++f->growth_table_reads;
        return GROWTH_TABLE;
    }
    CHECK(address < MEM_SIZE - 3);
    return read32(f->memory + address);
}

static void store8(void *user, uint32_t address, uint8_t value)
{
    struct chain_fixture *f = user;
    CHECK(address < MEM_SIZE);
    f->memory[address] = value;
}

static void store16(void *user, uint32_t address, uint16_t value)
{
    struct chain_fixture *f = user;
    CHECK(address < MEM_SIZE - 1);
    f->memory[address] = (uint8_t)value;
    f->memory[address + 1] = (uint8_t)(value >> 8);
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
    static const uint32_t sizes[] = {
        PX_BATTLE_TASK_SIZE, PX_BATTLE_AUX_SIZE, PX_POKEMON_OBJECT_SIZE,
        PX_POKEMON_DATA_SIZE, PX_POKEMON_MANAGER_SIZE, PX_POKEMON_EXTRA_SIZE
    };
    static const uint32_t addresses[] = {TASK, AUX, TEMP, DATA, POKEMON_MANAGER, EXTRA};
    const unsigned index = f->allocation_count++;
    CHECK(heap == UINT32_C(0x11112222));
    CHECK(index < sizeof sizes / sizeof sizes[0]);
    CHECK(size == sizes[index]);
    if (index == 0) event(f, 'T');
    if (index == 1) event(f, 'A');
    if (index == 2) event(f, 'B');
    if (index == 3) {
        CHECK(f->constructor_phase == 0);
        CHECK(f->random_word_calls == 2 && f->random_below_calls == 7);
        CHECK(f->normalizer_species_calls == 1 && f->normalizer_attribute_calls == 1);
        CHECK(f->gender_calls == 1);
        f->constructor_phase = 1;
    }
    if (index == 4) {
        CHECK(f->constructor_phase == 2);
        f->constructor_phase = 3;
    }
    if (index == 5) {
        CHECK(f->constructor_phase == 4);
        f->constructor_phase = 5;
    }
    return addresses[index];
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

static uint32_t normalization_word(void *user)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 0);
    CHECK(f->random_word_calls < 2);
    return f->random_word_calls++ == 0 ? UINT32_C(0x10203040) : UINT32_C(0x50607080);
}

static uint32_t normalization_below(void *user, uint32_t bound)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 0);
    const unsigned index = f->random_below_calls++;
    if (index < 6) {
        CHECK(bound == 32);
        return 2 + 3 * index;
    }
    CHECK(index == 6 && bound == 25);
    return 24;
}

static uint32_t normalization_mode2(void *user, uint32_t key, uint32_t value)
{
    struct chain_fixture *f = user;
    CHECK(f->random_word_calls == 2 && f->random_below_calls == 0);
    CHECK(key == UINT32_C(0xabcdef01) && value == UINT32_C(0x50607080));
    return UINT32_C(0x90a0b0c0);
}

static uint32_t unexpected_pair(void *user, uint32_t key, uint32_t value)
{
    (void)user; (void)key; (void)value;
    CHECK(0); return 0;
}

static void unexpected_species_base(void *user, uint32_t species)
{
    (void)user; (void)species;
    CHECK(0);
}

static uint32_t unexpected_probe(void *user)
{
    (void)user;
    CHECK(0); return 0;
}

static void parameter_species(void *user, uint32_t species, uint32_t form)
{
    struct chain_fixture *f = user;
    CHECK(species == UINT32_C(0x1234) && form == 7);
    if (f->constructor_phase == 0) {
        CHECK(f->gender_calls == 1);
        ++f->normalizer_species_calls;
    } else {
        CHECK(f->constructor_phase == 1 && f->normalizer_species_calls == 1);
        ++f->population_species_calls;
    }
}

static uint32_t parameter_attribute(void *user, uint32_t selector)
{
    struct chain_fixture *f = user;
    if (f->constructor_phase == 0) {
        CHECK(selector == 0x16 && f->normalizer_species_calls == 1);
        ++f->normalizer_attribute_calls;
        return UINT32_C(0x12340042);
    }
    CHECK(f->constructor_phase == 1 && selector == 0x1b);
    CHECK(f->population_species_calls == 1);
    ++f->population_attribute_calls;
    return UINT32_C(0x1234009a); /* Ability resolution narrows to a byte. */
}

static uint32_t normalization_gender(void *user, uint32_t species,
                                      uint32_t form, uint32_t value)
{
    struct chain_fixture *f = user;
    CHECK(species == UINT32_C(0x1234) && form == 7 && value == 1);
    CHECK(f->random_below_calls == 7);
    ++f->gender_calls;
    return UINT32_C(0x10001); /* The target stores only the low halfword. */
}

/* Resource services remain synthetic, while the actual population, field and
 * codec implementations must turn the normalized parameters into real bytes. */
static void select_growth(void *user, uint32_t species, uint32_t form)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 1 && species == UINT32_C(0x1234) && form == 7);
    CHECK(f->population_attribute_calls == 1 && f->growth_select_calls == 0);
    ++f->growth_select_calls;
}

/* Independent byte oracle for fixture permutation [2,0,3,1]. Logical blocks
 * 0,1,2,3 therefore begin at raw offsets 0x78,0x08,0xb0,0x40. The six stats
 * pack as 0x1cb8a0a2. Two synthetic learned moves 0x1111/0x2222 and
 * real refresh PP 15/25 add 0x4c42 to the prior payload sum 0xcae2,
 * yielding 0x1724 modulo 65536. These bytes are an independent oracle. */
static void check_plain_data(struct chain_fixture *f, uint32_t data)
{
    static const uint8_t expected[PX_POKEMON_DATA_SIZE] = {
        [0x00] = 0x40, [0x01] = 0x30, [0x02] = 0x20, [0x03] = 0x10,
        [0x06] = 0x24, [0x07] = 0x17,
        [0x22] = 0x11, [0x23] = 0x11, [0x24] = 0x22, [0x25] = 0x22,
        [0x2a] = 15, [0x2b] = 25,
        [0x3c] = 0xa2, [0x3d] = 0xa0, [0x3e] = 0xb8, [0x3f] = 0x1c,
        [0x5a] = 0x42, [0x6d] = 0x14,
        [0x70] = 0xb2, [0x71] = 0xc3, [0x72] = 0xd4, [0x73] = 0xa1,
        [0x78] = 0x34, [0x79] = 0x12,
        [0x7c] = 0x01, [0x7d] = 0xef, [0x7e] = 0xcd, [0x7f] = 0xab,
        [0x80] = 0x78, [0x81] = 0x56, [0x82] = 0x34, [0x83] = 0x12,
        [0x84] = 0x9a, [0x85] = 0x02,
        [0x88] = 0xc0, [0x89] = 0xb0, [0x8a] = 0xa0, [0x8b] = 0x90,
        [0x8c] = 0x18, [0x8d] = 0x3a
    };
    CHECK(data < MEM_SIZE - PX_POKEMON_DATA_SIZE);
    CHECK(memcmp(f->memory + data, expected, sizeof expected) == 0);
    CHECK(px_pokemon_checksum(data + 8, 0xe0, &f->pokemon_api) == UINT16_C(0x1724));
    CHECK((load16(f, data + 4) & 4) == 0);
}

static void populate_data(void *user, uint32_t data, const uint8_t *parameters)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 1 && data == DATA && parameters == f->normalized);
    for (unsigned i = 0; i < PX_POKEMON_DATA_SIZE; ++i) CHECK(load8(f, DATA + i) == 0);
    CHECK(read32(parameters) == UINT32_C(0x10203040) && read32(parameters + 4) == 0);
    CHECK(read32(parameters + 8) == UINT32_C(0x90a0b0c0) && read32(parameters + 0x0c) == 0);
    CHECK(read32(parameters + 0x10) == UINT32_C(0xabcdef01) && read32(parameters + 0x14) == 0);
    CHECK(parameters[0x18] == 0x34 && parameters[0x19] == 0x12 && parameters[0x1a] == 7);
    CHECK(parameters[0x1b] == 0xa5 && parameters[0x1c] == 20 && parameters[0x1d] == 0);
    CHECK(parameters[0x1e] == 1 && parameters[0x1f] == 0);
    CHECK(parameters[0x20] == 24 && parameters[0x21] == 0 && parameters[0x22] == 1);
    CHECK(parameters[0x23] == 1 && parameters[0x34] == 0);
    for (unsigned i = 0; i < 6; ++i) {
        CHECK(parameters[0x24 + 2 * i] == 2 + 3 * i);
        CHECK(parameters[0x25 + 2 * i] == 0);
    }
    CHECK(read32(parameters + 0x30) == UINT32_C(0x12340042));
    CHECK(parameters[0x35] == 0xa5 && parameters[0x36] == 0xa5 && parameters[0x37] == 0xa5);
    px_populate_pokemon_data(data, parameters, SCRATCH_MANAGER, &f->data_api);
    CHECK(f->population_species_calls == 1 && f->population_attribute_calls == 1);
    CHECK(f->settings_reads == 4 && f->growth_select_calls == 1 && f->growth_table_reads == 1);
    CHECK(load32(f, SCRATCH_MANAGER + 8) == DATA && load32(f, SCRATCH_MANAGER + 4) == 0);
    CHECK(load8(f, SCRATCH_MANAGER + 0x0c) == 1 && load8(f, SCRATCH_MANAGER + 0x0d) == 0);
    CHECK(load32(f, data) == UINT32_C(0x10203040));
    CHECK(load16(f, data + 6) == UINT16_C(0xcae2));
    f->constructor_phase = 2;
}

static void initialize_base_state(void *user, uint32_t object)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 3 && object == TEMP);
    CHECK(load32(f, object) == UINT32_C(0x00598a88));
    CHECK(load32(f, object + 4) == DATA && load32(f, object + 8) == 0);
    CHECK(load32(f, object + 0x0c) == POKEMON_MANAGER);
    CHECK(load32(f, POKEMON_MANAGER) == UINT32_C(0x00598a78));
    CHECK(load32(f, POKEMON_MANAGER + 4) == 0 && load32(f, POKEMON_MANAGER + 8) == DATA);
    CHECK(load8(f, POKEMON_MANAGER + 0x0c) == 1 && load8(f, POKEMON_MANAGER + 0x0d) == 0);
    px_initialize_pokemon_base_state(object, f->name_scratch, &f->state_api);
    CHECK(f->learn_calls == 2 && f->learnset_count_calls == 3);
    CHECK(f->maximum_pp_calls == 8 && f->state_level_calls == 1);
    CHECK(f->settings_reads == 8);
    f->constructor_phase = 4;
}

static void decode_manager(void *user, uint32_t manager)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 5 && manager == POKEMON_MANAGER);
    CHECK(load32(f, TEMP) == UINT32_C(0x00598a50));
    CHECK(load32(f, TEMP + 8) == EXTRA);
    CHECK(load32(f, manager + 4) == EXTRA && load32(f, manager + 8) == DATA);
    CHECK(load8(f, manager + 0x0d) == 0);
    /* Fixed independent stream prefix for seed 0x10203040. */
    CHECK(load16(f, EXTRA) == UINT16_C(0x3de8));
    CHECK(load16(f, EXTRA + 2) == UINT16_C(0xec38));
    CHECK(load16(f, EXTRA + 4) == UINT16_C(0xebe2));
    CHECK(load16(f, EXTRA + 0x1a) == UINT16_C(0xd791));
    CHECK(load16(f, DATA + 0x78) == UINT16_C(0xaa73));
    px_decode_pokemon_manager(manager, &f->pokemon_api);
    CHECK(load8(f, manager + 0x0c) == 0 && load8(f, manager + 0x0d) == 0);
    check_plain_data(f, DATA);
    for (unsigned i = 0; i < PX_POKEMON_EXTRA_SIZE; ++i) CHECK(load8(f, EXTRA + i) == 0);
    f->constructor_phase = 6;
}

static void initialize_extra_state(void *user, uint32_t manager)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 6 && manager == POKEMON_MANAGER);
    CHECK(load8(f, manager + 0x0d) == 1);
    px_initialize_pokemon_extra_state(manager, &f->state_api);
    for (unsigned i = 0; i < PX_POKEMON_EXTRA_SIZE; ++i) CHECK(load8(f, EXTRA + i) == 0);
    f->constructor_phase = 7;
}

static void update_derived_state(void *user, uint32_t object, uint32_t mode)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 7 && object == TEMP && mode == 1);
    CHECK(load8(f, POKEMON_MANAGER + 0x0d) == 1);
    check_plain_data(f, DATA);
    px_update_pokemon_derived_state(object, mode, &f->state_api);
    CHECK(f->state_level_calls == 2 && f->update_stat_calls == 5);
    CHECK(load8(f, EXTRA + 4) == 20);
    f->constructor_phase = 8;
}

static void refresh_state(void *user, uint32_t object)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 8 && object == TEMP);
    CHECK(load8(f, POKEMON_MANAGER + 0x0d) == 1);
    px_refresh_pokemon_state(object, &f->state_api);
    CHECK(f->maximum_pp_calls == 16 && load16(f, EXTRA + 8) == 0x3456);
    f->constructor_phase = 9;
}

static void encode_manager(void *user, uint32_t manager)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 9 && manager == POKEMON_MANAGER);
    CHECK(load8(f, manager + 0x0d) == 0);
    CHECK(load32(f, manager + 4) == EXTRA && load32(f, manager + 8) == DATA);
    check_plain_data(f, DATA);
    px_encode_pokemon_manager(manager, &f->pokemon_api);
    CHECK(load8(f, manager + 0x0c) == 1 && load8(f, manager + 0x0d) == 0);
    CHECK(load16(f, DATA + 6) == UINT16_C(0x1724));
    CHECK(load16(f, DATA + 8) == UINT16_C(0x3de8));
    CHECK(load16(f, DATA + 0x3c) == UINT16_C(0xb84d));
    CHECK(load16(f, DATA + 0x3e) == UINT16_C(0x517b));
    CHECK(load16(f, DATA + 0x78) == UINT16_C(0xaa73));
    CHECK(load16(f, DATA + 0xe6) == UINT16_C(0xa9e0));
    CHECK(load16(f, EXTRA + 0x10) == UINT16_C(0x9e2a));
    CHECK(load16(f, EXTRA + 0x12) == UINT16_C(0x781a));
    f->constructor_phase = 10;
}

/* These deeper resource/stat services intentionally remain synthetic. The
 * four state orchestrators, field reads/writes, and codec are the real host
 * reconstructions; the callback-owned writes have fixed independent oracles. */
static uint32_t state_level(void *user, uint32_t species, uint32_t form,
                            uint32_t growth)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 3 || f->constructor_phase == 7);
    CHECK(species == 0x1234 && form == 7 && growth == UINT32_C(0x12345678));
    ++f->state_level_calls;
    return 20;
}

static void state_select_learnset(void *user, uint32_t species, uint32_t form)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 3 && species == 0x1234 && form == 7);
    CHECK(f->state_level_calls == 1);
}

static uint32_t state_learnset_count(void *user)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 3);
    ++f->learnset_count_calls;
    return 4;
}

static uint32_t state_learnset_level(void *user, uint32_t index)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 3 && index <= 2);
    const uint32_t levels[3] = {5, 20, 21};
    return levels[index];
}

static uint32_t state_learnset_move(void *user, uint32_t index)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 3 && index < 2);
    return index ? UINT32_C(0x2222) : UINT32_C(0x1111);
}

static void state_learn_move(void *user, uint32_t object, uint32_t move)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 3 && object == TEMP && f->learn_calls < 2);
    CHECK(move == (f->learn_calls ? UINT32_C(0x2222) : UINT32_C(0x1111)));
    px_write_pokemon_state_slot(POKEMON_MANAGER, f->learn_calls++, PX_STATE_MOVE,
                                move, &f->pokemon_api);
}

static void state_build_name(void *user, uint8_t *scratch, uint32_t species)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 3 && species == 0x1234);
    CHECK(f->learn_calls == 2 && scratch == f->name_scratch);
    for (unsigned i = 0; i < PX_POKEMON_NAME_SCRATCH_SIZE; ++i)
        scratch[i] = (uint8_t)(0x80 + i);
}

static void state_write_name(void *user, uint32_t manager, const uint8_t *scratch)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 3 && manager == POKEMON_MANAGER);
    CHECK(scratch == f->name_scratch);
    for (unsigned i = 0; i < PX_POKEMON_NAME_SCRATCH_SIZE; ++i)
        CHECK(scratch[i] == (uint8_t)(0x80 + i));
    /* Resource-derived name serialization stays an explicit boundary. */
}

static uint32_t state_maximum_pp(void *user, uint32_t move, uint32_t pp_ups)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 3 || f->constructor_phase == 8);
    CHECK(pp_ups == 0);
    ++f->maximum_pp_calls;
    if (move == 0x1111) return 15;
    if (move == 0x2222) return 25;
    CHECK(move == 0);
    return 0;
}

static void state_update_mode(void *user, uint32_t object, uint32_t mode)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 7 && object == TEMP && mode == 1);
    CHECK(load8(f, EXTRA + 4) == 20 && f->state_level_calls == 2);
    store16(f, EXTRA + 0xa, UINT16_C(0x3456));
}

static void state_update_stat(void *user, uint32_t object, enum px_state_stat stat)
{
    struct chain_fixture *f = user;
    CHECK(f->constructor_phase == 7 && object == TEMP);
    CHECK((unsigned)stat == f->update_stat_calls && f->update_stat_calls < 5);
    store16(f, EXTRA + 0xc + 2 * f->update_stat_calls,
             (uint16_t)(0x1101 + f->update_stat_calls));
    ++f->update_stat_calls;
}

static uint32_t construct(void *user, uint32_t storage, uint32_t heap,
                           const uint8_t *parameters)
{
    struct chain_fixture *f = user;
    event(f, 'N');
    CHECK(storage == TEMP && heap == UINT32_C(0x11112222));
    CHECK(parameters == f->parameters);
    CHECK(read32(parameters) == UINT32_MAX && read32(parameters + 4) == UINT32_MAX);
    CHECK(read32(parameters + 8) == UINT32_MAX && read32(parameters + 0x0c) == 2);
    CHECK(parameters[0x18] == 0x34 && parameters[0x19] == 0x12);
    CHECK(parameters[0x1a] == 7);
    CHECK(parameters[0x1c] == 20 && parameters[0x1d] == 0);
    CHECK(parameters[0x1e] == 1 && parameters[0x1f] == 0);
    CHECK(parameters[0x22] == 1 && parameters[0x23] == 1);
    CHECK(read32(parameters + 0x10) == UINT32_C(0xabcdef01));
    CHECK(read32(parameters + 0x14) == 0);
    f->parameter_api = (struct px_parameter_api) {
        .user = f, .random_word = normalization_word, .random_below = normalization_below,
        .mode1_value = unexpected_pair, .mode2_value = normalization_mode2,
        .candidate_matches = unexpected_pair, .select_species_base = unexpected_species_base,
        .form_probe = unexpected_probe, .select_species = parameter_species,
        .personal_value = parameter_attribute, .resolve_gender = normalization_gender
    };
    f->pokemon_api = (struct px_pokemon_api) {
        .user = f, .parameters = &f->parameter_api,
        .load8 = load8, .load16 = load16, .load32 = load32,
        .store8 = store8, .store16 = store16, .store32 = store32,
        .allocate = allocate, .populate_data = populate_data,
        .initialize_base_state = initialize_base_state, .decode_manager = decode_manager,
        .encode_manager = encode_manager, .initialize_extra_state = initialize_extra_state,
        .update_derived_state = update_derived_state, .refresh_state = refresh_state
    };
    f->data_api = (struct px_data_api) {&f->pokemon_api, f, select_growth};
    f->state_api = (struct px_state_api) {
        .memory = &f->pokemon_api, .user = f, .level_from_growth = state_level,
        .select_learnset = state_select_learnset, .learnset_count = state_learnset_count,
        .learnset_level = state_learnset_level, .learnset_move = state_learnset_move,
        .learn_move = state_learn_move, .build_name = state_build_name,
        .write_name = state_write_name, .maximum_pp = state_maximum_pp,
        .update_mode = state_update_mode, .update_stat = state_update_stat
    };
    const uint32_t result = px_construct_pokemon(storage, heap, parameters, f->normalized, &f->pokemon_api);
    CHECK(result == TEMP && f->constructor_phase == 10);
    CHECK(read32(parameters) == UINT32_MAX && read32(parameters + 0x0c) == 2);
    return result;
}

static void set_value(void *user, uint32_t object, uint32_t value)
{
    struct chain_fixture *f = user;
    event(f, 'V');
    CHECK(object == TEMP && value == 37);
    CHECK(f->constructor_phase == 10);
    f->temporary_value = value;
    /* The actual selected-value setter remains external. Preserve its value
     * in the fixture side channel without inventing a serialized field. */
}

static void append(void *user, uint32_t destination, uint32_t object)
{
    struct chain_fixture *f = user;
    event(f, 'J');
    CHECK(destination == AUX && object == TEMP);
    CHECK(f->destroy_calls == 0 && f->append_calls == 0);
    CHECK(load8(f, AUX + 0x18) == 0);
    CHECK(f->constructor_phase == 10 && load32(f, object + 4) == DATA);
    CHECK(load32(f, object + 8) == EXTRA);
    memcpy(f->retained_parameters, f->normalized, sizeof f->retained_parameters);
    memcpy(f->retained_data, f->memory + DATA, sizeof f->retained_data);
    memcpy(f->retained_extra, f->memory + EXTRA, sizeof f->retained_extra);
    memcpy(f->memory + COPIED_DATA, f->memory + DATA, PX_POKEMON_DATA_SIZE);
    memcpy(f->memory + COPIED_EXTRA, f->memory + EXTRA, PX_POKEMON_EXTRA_SIZE);
    (void)px_initialize_pokemon_manager(COPIED_MANAGER, &f->pokemon_api);
    px_attach_pokemon_buffers(COPIED_MANAGER, COPIED_DATA, COPIED_EXTRA, &f->pokemon_api);
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
    memset(f->memory + DATA, 0xdd, PX_POKEMON_DATA_SIZE);
    memset(f->memory + EXTRA, 0xdd, PX_POKEMON_EXTRA_SIZE);
    memset(f->memory + POKEMON_MANAGER, 0xdd, PX_POKEMON_MANAGER_SIZE);
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

void test_pokemon_state_chain(void)
{
    struct chain_fixture f = {0};
    memset(f.parameters, 0xa5, sizeof f.parameters);
    memset(f.normalized, 0x5a, sizeof f.normalized);
    memset(f.memory + DATA, 0xa5, PX_POKEMON_DATA_SIZE);
    memset(f.memory + POKEMON_MANAGER, 0xa5, PX_POKEMON_MANAGER_SIZE);
    memset(f.memory + EXTRA, 0xa5, PX_POKEMON_EXTRA_SIZE);
    memset(f.memory + SCRATCH_MANAGER, 0xa5, PX_POKEMON_MANAGER_SIZE);
    for (unsigned row_index = 0; row_index < 32; ++row_index)
        for (unsigned block = 0; block < 4; ++block)
            f.permutation_table[4 * row_index + block] = (uint8_t)block;
    /* Key 0x10203040 selects table row 1, with a non-identity permutation. */
    const uint8_t permutation[4] = {2, 0, 3, 1};
    memcpy(f.permutation_table + 4, permutation, sizeof permutation);
    const uint8_t settings[4] = {0xa1, 0xb2, 0xc3, 0xd4};
    memcpy(f.memory + SETTINGS + 0x68, settings, sizeof settings);
    for (unsigned level = 0; level <= 100; ++level)
        store32(&f, GROWTH_TABLE + 8 + 4 * level, UINT32_C(0xdead0000) + level);
    store32(&f, GROWTH_TABLE + 8 + 4 * 20, UINT32_C(0x12345678));
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
    CHECK(f.allocation_count == 6 && f.constructor_phase == 10);
    CHECK(f.random_word_calls == 2 && f.random_below_calls == 7);
    CHECK(f.normalizer_species_calls == 1 && f.normalizer_attribute_calls == 1);
    CHECK(f.population_species_calls == 1 && f.population_attribute_calls == 1);
    CHECK(f.settings_reads == 8 && f.growth_select_calls == 1 && f.growth_table_reads == 1);
    CHECK(f.permutation_reads > 0);
    CHECK(read32(f.retained_data) == UINT32_C(0x10203040));
    CHECK(memcmp(f.memory + COPIED_DATA, f.retained_data, sizeof f.retained_data) == 0);
    CHECK(memcmp(f.memory + COPIED_EXTRA, f.retained_extra, sizeof f.retained_extra) == 0);
    CHECK(load8(&f, COPIED_MANAGER + 0x0c) == 1);
    /* The actual decoder now reads only the retained copy, after the original
     * data, extra and manager have all been poisoned by temporary cleanup. */
    px_decode_pokemon_manager(COPIED_MANAGER, &f.pokemon_api);
    CHECK(load8(&f, COPIED_MANAGER + 0x0c) == 0);
    check_plain_data(&f, COPIED_DATA);
    CHECK(read32(f.retained_data + 8) == UINT32_C(0xec383de8));
    CHECK(memcmp(f.memory + COPIED_DATA + 8, f.retained_data + 8, 0xe0) != 0);
    static const uint8_t expected_extra[PX_POKEMON_EXTRA_SIZE] = {
        [0x04] = 20,
        [0x08] = 0x56, [0x09] = 0x34, [0x0a] = 0x56, [0x0b] = 0x34,
        [0x0c] = 1, [0x0d] = 0x11, [0x0e] = 2, [0x0f] = 0x11,
        [0x10] = 3, [0x11] = 0x11, [0x12] = 4, [0x13] = 0x11,
        [0x14] = 5, [0x15] = 0x11
    };
    CHECK(memcmp(f.memory + COPIED_EXTRA, expected_extra, sizeof expected_extra) == 0);
    /* A second actual encode must restore both copied ciphertexts byte-for-byte. */
    px_encode_pokemon_manager(COPIED_MANAGER, &f.pokemon_api);
    CHECK(load8(&f, COPIED_MANAGER + 0x0c) == 1);
    CHECK(memcmp(f.memory + COPIED_DATA, f.retained_data, sizeof f.retained_data) == 0);
    CHECK(memcmp(f.memory + COPIED_EXTRA, f.retained_extra, sizeof f.retained_extra) == 0);
    for (unsigned i = 0; i < PX_POKEMON_DATA_SIZE; ++i) CHECK(f.memory[DATA + i] == 0xdd);
    for (unsigned i = 0; i < PX_POKEMON_EXTRA_SIZE; ++i) CHECK(f.memory[EXTRA + i] == 0xdd);
    CHECK(f.submitted_task == TASK && load32(&f, MANAGER) == TASK);
    CHECK(load8(&f, AUX + 0x18) == 1);
    CHECK(load32(&f, TASK + 0x2c + 0xa8) == UINT32_C(0x8100));
    CHECK(load8(&f, TASK + 0x2c + 0xb2) == 1);
    CHECK(load32(&f, FINISH_OBJECT + 0x58) == 12);
    CHECK(f.retained_parameters[0x18] == 0x34 && f.retained_parameters[0x19] == 0x12);
    CHECK(f.retained_value == 37 && f.temporary_value == UINT32_MAX);
    for (unsigned i = 0; i < PX_POKEMON_MANAGER_SIZE; ++i)
        CHECK(f.memory[POKEMON_MANAGER + i] == 0xdd);
    CHECK(load32(&f, TEMP) == 0);
    CHECK(args[0] == UINT32_C(0x11223344) && args[1] == UINT32_C(0xdead1234));
    CHECK(memcmp(f.record, row, sizeof row) == 0);
}
