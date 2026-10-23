#include "pokemon_constructor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { \
    fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #c); \
    exit(EXIT_FAILURE); \
} } while (0)

/* Scripted services check independently specified ARM call arguments and
 * ordering. Edits model ordinary service side effects on the caller's block. */
struct parameter_edit {
    unsigned offset, width, source;
    uint32_t value;
};

struct parameter_step {
    char kind;
    uint32_t arguments[3], result;
    unsigned inspect_copy, check_count, edit_count;
    struct parameter_edit checks[4], edits[4];
};

struct parameter_fixture {
    uint8_t source[PX_POKEMON_PARAMETERS_SIZE];
    uint8_t original[PX_POKEMON_PARAMETERS_SIZE];
    uint8_t guarded[PX_POKEMON_PARAMETERS_SIZE + 16];
    struct parameter_step steps[64];
    unsigned count, cursor, source_may_change;
};

static unsigned parameter_runs;

static uint8_t *output(struct parameter_fixture *f) { return f->guarded + 8; }

static uint32_t get_value(const uint8_t *p, unsigned width)
{
    uint32_t value = 0;
    for (unsigned i = 0; i < width; ++i) value |= (uint32_t)p[i] << (8 * i);
    return value;
}

static void put_value(uint8_t *p, unsigned width, uint32_t value)
{
    for (unsigned i = 0; i < width; ++i) p[i] = (uint8_t)(value >> (8 * i));
}

static void initialize(struct parameter_fixture *f)
{
    memset(f, 0, sizeof *f);
    memset(f->guarded, 0x5a, sizeof f->guarded);
    for (unsigned i = 0; i < sizeof f->source; ++i) f->source[i] = (uint8_t)(0x80 + i);
    put_value(f->source + 0x00, 4, UINT32_C(0x11223344));
    put_value(f->source + 0x04, 4, UINT32_C(0x55667788));
    put_value(f->source + 0x08, 4, UINT32_C(0x12345678));
    put_value(f->source + 0x0c, 4, 0);
    put_value(f->source + 0x10, 4, UINT32_C(0x89abcdef));
    put_value(f->source + 0x14, 4, UINT32_C(0x10203040));
    put_value(f->source + 0x18, 2, 25);
    f->source[0x1a] = 3;
    put_value(f->source + 0x1c, 2, 50);
    put_value(f->source + 0x1e, 2, UINT32_C(0x1234));
    put_value(f->source + 0x20, 2, 7);
    f->source[0x22] = 1;
    f->source[0x23] = 0;
    for (unsigned i = 0; i < 6; ++i) put_value(f->source + 0x24 + 2 * i, 2, 10 + i);
    put_value(f->source + 0x30, 4, UINT32_C(0x99887766));
    f->source[0x34] = 0;
}

static struct parameter_step *expect(struct parameter_fixture *f, char kind,
                                      uint32_t a, uint32_t b, uint32_t c, uint32_t result)
{
    CHECK(f->count < sizeof f->steps / sizeof f->steps[0]);
    struct parameter_step *step = &f->steps[f->count++];
    step->kind = kind;
    step->arguments[0] = a;
    step->arguments[1] = b;
    step->arguments[2] = c;
    step->result = result;
    return step;
}

static void edit(struct parameter_step *step, unsigned offset, unsigned width, uint32_t value)
{
    CHECK(step->edit_count < 4);
    step->edits[step->edit_count++] = (struct parameter_edit){offset, width, 0, value};
}

static void observe(struct parameter_step *step, unsigned offset, unsigned width, uint32_t value)
{
    CHECK(step->check_count < 4);
    step->checks[step->check_count++] = (struct parameter_edit){offset, width, 0, value};
}

static uint32_t service(struct parameter_fixture *f, char kind,
                         uint32_t a, uint32_t b, uint32_t c)
{
    CHECK(f->cursor < f->count);
    const struct parameter_step *step = &f->steps[f->cursor++];
    CHECK(step->kind == kind);
    CHECK(step->arguments[0] == a && step->arguments[1] == b && step->arguments[2] == c);
    if (step->inspect_copy) CHECK(memcmp(output(f), f->original, sizeof f->original) == 0);
    for (unsigned i = 0; i < step->check_count; ++i) {
        const struct parameter_edit *p = &step->checks[i];
        CHECK(p->width >= 1 && p->width <= 4 && p->offset + p->width <= sizeof f->source);
        CHECK(get_value(output(f) + p->offset, p->width) == p->value);
    }
    for (unsigned i = 0; i < step->edit_count; ++i) {
        const struct parameter_edit *p = &step->edits[i];
        CHECK(p->width >= 1 && p->width <= 4 && p->offset + p->width <= sizeof f->source);
        put_value((p->source ? f->source : output(f)) + p->offset, p->width, p->value);
    }
    return step->result;
}

static uint32_t random_word(void *user) { return service(user, 'R', 0, 0, 0); }
static uint32_t random_below(void *user, uint32_t bound) { return service(user, 'B', bound, 0, 0); }
static uint32_t mode1(void *user, uint32_t key, uint32_t value) { return service(user, '1', key, value, 0); }
static uint32_t mode2(void *user, uint32_t key, uint32_t value) { return service(user, '2', key, value, 0); }
static uint32_t matches(void *user, uint32_t key, uint32_t value) { return service(user, 'M', key, value, 0); }
static void select_base(void *user, uint32_t species) { (void)service(user, 'S', species, 0, 0); }
static uint32_t form_probe(void *user) { return service(user, 'P', 0, 0, 0); }
static void select_species(void *user, uint32_t species, uint32_t form) { (void)service(user, 'L', species, form, 0); }
static uint32_t personal_value(void *user, uint32_t selector) { return service(user, 'A', selector, 0, 0); }
static uint32_t gender(void *user, uint32_t species, uint32_t form, uint32_t value) { return service(user, 'G', species, form, value); }

static void standard_gender(struct parameter_fixture *f)
{
    (void)expect(f, 'G', 25, 3, UINT32_C(0x34), UINT32_C(0x1234));
}

static void run(struct parameter_fixture *f)
{
    const struct px_parameter_api api = {
        .user = f, .random_word = random_word, .random_below = random_below,
        .mode1_value = mode1, .mode2_value = mode2, .candidate_matches = matches,
        .select_species_base = select_base, .form_probe = form_probe,
        .select_species = select_species, .personal_value = personal_value,
        .resolve_gender = gender
    };
    memcpy(f->original, f->source, sizeof f->original);
    px_normalize_pokemon_parameters(output(f), f->source, &api);
    CHECK(f->cursor == f->count);
    for (unsigned i = 0; i < 8; ++i) {
        CHECK(f->guarded[i] == UINT8_C(0x5a));
        CHECK(f->guarded[PX_POKEMON_PARAMETERS_SIZE + 8 + i] == UINT8_C(0x5a));
    }
    if (!f->source_may_change) CHECK(memcmp(f->original, f->source, sizeof f->source) == 0);
    ++parameter_runs;
}

static void test_copy_and_pair_sentinels(void)
{
    struct parameter_fixture f;
    initialize(&f);
    standard_gender(&f);
    f.steps[0].inspect_copy = 1;
    run(&f);
    CHECK(memcmp(output(&f), f.source, sizeof f.source) == 0);

    const uint32_t words[] = {UINT32_MAX, UINT32_MAX - 1, 0};
    const unsigned offsets[] = {0, 0x10};
    for (unsigned k = 0; k < 2; ++k) for (unsigned i = 0; i < 3; ++i) for (unsigned j = 0; j < 3; ++j) {
        initialize(&f);
        put_value(f.source + offsets[k], 4, words[i]);
        put_value(f.source + offsets[k] + 4, 4, words[j]);
        uint8_t expected[PX_POKEMON_PARAMETERS_SIZE];
        memcpy(expected, f.source, sizeof expected);
        if (i == 0 && j == 0) {
            expect(&f, 'R', 0, 0, 0, UINT32_C(0xfedcba98));
            put_value(expected + offsets[k], 4, UINT32_C(0xfedcba98));
            put_value(expected + offsets[k] + 4, 4, 0);
        }
        standard_gender(&f);
        run(&f);
        CHECK(memcmp(output(&f), expected, sizeof expected) == 0);
    }

    initialize(&f);
    put_value(f.source, 4, UINT32_MAX);
    put_value(f.source + 4, 4, UINT32_MAX);
    struct parameter_step *s = expect(&f, 'R', 0, 0, 0, 17);
    s->inspect_copy = 1;
    edit(s, 0x10, 4, UINT32_MAX);
    edit(s, 0x14, 4, UINT32_MAX);
    edit(s, 0x30, 4, UINT32_C(0xffff));
    s->edits[2].source = 1;
    f.source_may_change = 1;
    expect(&f, 'R', 0, 0, 0, 19);
    standard_gender(&f);
    run(&f);
    CHECK(get_value(output(&f), 4) == 17 && get_value(output(&f) + 4, 4) == 0);
    CHECK(get_value(output(&f) + 0x10, 4) == 19 && get_value(output(&f) + 0x14, 4) == 0);
    CHECK(get_value(output(&f) + 0x30, 4) == UINT32_C(0x99887766));
    CHECK(get_value(f.source + 0x30, 4) == UINT32_C(0xffff));
}

static void test_candidate_modes(void)
{
    struct parameter_fixture f;
    const uint32_t low[] = {UINT32_MAX, UINT32_MAX - 1, 0};
    const uint32_t high[] = {0, 1, 2, 3, 4, UINT32_MAX};
    for (unsigned i = 0; i < 3; ++i) for (unsigned j = 0; j < 6; ++j) {
        initialize(&f);
        put_value(f.source + 8, 4, low[i]);
        put_value(f.source + 0x0c, 4, high[j]);
        const int transformed = i == 0 && (high[j] == 1 || high[j] == 2);
        if (transformed) {
            struct parameter_step *s = expect(&f, 'R', 0, 0, 0, UINT32_C(0xcafef00d));
            edit(s, 0x10, 4, UINT32_C(0x10203040));
            edit(s, 8, 4, UINT32_C(0x55555555));
            edit(s, 0x0c, 4, 77);
            s = expect(&f, high[j] == 1 ? '1' : '2', UINT32_C(0x10203040), UINT32_C(0xcafef00d), 0,
                       UINT32_C(0x87654321));
            observe(s, 8, 4, UINT32_C(0x55555555));
            observe(s, 0x0c, 4, 77);
        }
        standard_gender(&f);
        run(&f);
        CHECK(get_value(output(&f) + 8, 4) == (transformed ? UINT32_C(0x87654321) : low[i]));
        CHECK(get_value(output(&f) + 0x0c, 4) == (transformed ? 0 : high[j]));
    }

    /* The attempt count is reloaded after a failed predicate; its high-bit
     * result is still true, and a successful predicate's edits survive. */
    initialize(&f);
    put_value(f.source + 8, 4, UINT32_MAX);
    put_value(f.source + 0x0c, 4, 3);
    f.source[0x23] = 1;
    for (unsigned i = 0; i < 3; ++i) {
        expect(&f, 'R', 0, 0, 0, 100 + i);
        struct parameter_step *s = expect(&f, 'M', UINT32_C(0x89abcdef), 100 + i, 0,
                                           i == 2 ? UINT32_C(0x100) : 0);
        observe(s, 8, 4, 100 + i);
        observe(s, 0x0c, 4, 0);
        if (i == 0) edit(s, 0x23, 1, 3);
        if (i == 2) {
            edit(s, 8, 4, UINT32_C(0xdeadbeef));
            edit(s, 0x0c, 4, UINT32_C(0xabcdef01));
        }
    }
    standard_gender(&f);
    run(&f);
    CHECK(get_value(output(&f) + 8, 4) == UINT32_C(0xdeadbeef));
    CHECK(get_value(output(&f) + 0x0c, 4) == UINT32_C(0xabcdef01));

    initialize(&f);
    put_value(f.source + 8, 4, UINT32_MAX);
    put_value(f.source + 0x0c, 4, 3);
    f.source[0x23] = 255;
    struct parameter_step *s = expect(&f, 'R', 0, 0, 0, 42);
    edit(s, 0x10, 4, 99);
    edit(s, 0x23, 1, 0);
    expect(&f, 'M', 99, 42, 0, 0);
    standard_gender(&f);
    run(&f);
    CHECK(get_value(output(&f) + 8, 4) == 42 && output(&f)[0x23] == 0);
}

static void test_level_and_stat_counts(void)
{
    struct parameter_fixture f;
    for (uint32_t level = 0; level <= UINT16_MAX; ++level) {
        initialize(&f);
        put_value(f.source + 0x1c, 2, level);
        standard_gender(&f);
        run(&f);
        CHECK(get_value(output(&f) + 0x1c, 2) == (level > 100 ? 100 : level));
        CHECK(output(&f)[0x1b] == f.source[0x1b]);
        CHECK(memcmp(output(&f) + 0x35, f.source + 0x35, 3) == 0);
    }
    for (unsigned count = 0; count < 256; ++count) {
        initialize(&f);
        f.source[0x34] = (uint8_t)count;
        unsigned chosen = count <= 5 ? count : 0;
        if (count != 0) {
            for (unsigned i = 0; i < chosen; ++i) expect(&f, 'B', 6, 0, 0, i);
            for (unsigned i = chosen; i < 6; ++i) expect(&f, 'B', 32, 0, 0, 20 + i);
        }
        standard_gender(&f);
        run(&f);
        CHECK(output(&f)[0x34] == (count > 5 ? 0 : count));
        for (unsigned i = 0; i < 6; ++i) {
            const uint32_t expected = count == 0 ? 10 + i : (i < chosen ? 31 : 20 + i);
            CHECK(get_value(output(&f) + 0x24 + 2 * i, 2) == expected);
        }
    }

    initialize(&f);
    f.source[0x34] = 2;
    expect(&f, 'B', 6, 0, 0, UINT32_C(0x100));
    expect(&f, 'B', 6, 0, 0, 0); /* Repeated slot must retry. */
    expect(&f, 'B', 6, 0, 0, UINT32_C(0x105));
    for (unsigned i = 1; i <= 4; ++i) expect(&f, 'B', 32, 0, 0, UINT32_C(0x12340000) + i);
    standard_gender(&f);
    run(&f);
    CHECK(get_value(output(&f) + 0x24, 2) == 31 && get_value(output(&f) + 0x2e, 2) == 31);
    for (unsigned i = 1; i <= 4; ++i) CHECK(get_value(output(&f) + 0x24 + 2 * i, 2) == i);

    initialize(&f);
    f.source[0x34] = 1;
    struct parameter_step *s = expect(&f, 'B', 6, 0, 0, 0);
    edit(s, 0x34, 1, 2);
    edit(s, 0x1c, 2, 500); /* Clamping already happened; it is not repeated. */
    s = expect(&f, 'B', 6, 0, 0, 1);
    edit(s, 0x34, 1, 0);
    for (unsigned i = 2; i < 6; ++i) expect(&f, 'B', 32, 0, 0, 20 + i);
    standard_gender(&f);
    run(&f);
    CHECK(output(&f)[0x34] == 0 && get_value(output(&f) + 0x1c, 2) == 500);
    CHECK(get_value(output(&f) + 0x24, 2) == 31 && get_value(output(&f) + 0x26, 2) == 31);

    initialize(&f);
    f.source[0x34] = 1;
    s = expect(&f, 'B', 6, 0, 0, 0);
    edit(s, 0x24, 2, 7); /* Slot is checked after the draw callback. */
    expect(&f, 'B', 6, 0, 0, 1);
    for (unsigned i = 2; i < 6; ++i) expect(&f, 'B', 32, 0, 0, 20 + i);
    standard_gender(&f);
    run(&f);
    CHECK(get_value(output(&f) + 0x24, 2) == 7 && get_value(output(&f) + 0x26, 2) == 31);

    initialize(&f);
    for (unsigned i = 0; i < 6; ++i) put_value(f.source + 0x24 + 2 * i, 2, UINT16_MAX);
    s = expect(&f, 'B', 32, 0, 0, UINT32_C(0x10123));
    edit(s, 0x24, 2, 99);
    edit(s, 0x26, 2, 19);
    for (unsigned i = 2; i < 6; ++i) expect(&f, 'B', 32, 0, 0, 20 + i);
    standard_gender(&f);
    run(&f);
    CHECK(get_value(output(&f) + 0x24, 2) == UINT32_C(0x123));
    CHECK(get_value(output(&f) + 0x26, 2) == 19);
}

static void test_ability_nature_and_forms(void)
{
    struct parameter_fixture f;
    for (unsigned ability = 0; ability < 256; ++ability) {
        initialize(&f);
        f.source[0x22] = (uint8_t)ability;
        if (ability == 255) expect(&f, 'B', 2, 0, 0, UINT32_C(0xab000101));
        if (ability == 254) expect(&f, 'B', 3, 0, 0, UINT32_C(0x12345602));
        standard_gender(&f);
        run(&f);
        CHECK(output(&f)[0x22] == (ability == 255 ? 1 : (ability == 254 ? 2 : ability)));
    }
    const unsigned nature_values[] = {0, 1, 24, 25, 255, 65534, 65535};
    for (unsigned i = 0; i < sizeof nature_values / sizeof nature_values[0]; ++i) {
        initialize(&f);
        put_value(f.source + 0x20, 2, nature_values[i]);
        if (nature_values[i] == 65535) expect(&f, 'B', 25, 0, 0, UINT32_C(0x12345678));
        standard_gender(&f);
        run(&f);
        CHECK(get_value(output(&f) + 0x20, 2) == (nature_values[i] == 65535 ? UINT32_C(0x5678) : nature_values[i]));
    }

    initialize(&f);
    f.source[0x22] = 255;
    struct parameter_step *s = expect(&f, 'B', 2, 0, 0, 1);
    edit(s, 0x20, 2, UINT16_MAX);
    s = expect(&f, 'B', 25, 0, 0, 17);
    edit(s, 0x1a, 1, 255);
    expect(&f, 'S', 25, 0, 0, 0);
    s = expect(&f, 'P', 0, 0, 0, UINT32_C(0x100));
    edit(s, 0x1a, 1, 9);
    expect(&f, 'G', 25, 0, UINT32_C(0x34), UINT32_C(0x1234));
    run(&f);
    CHECK(get_value(output(&f) + 0x20, 2) == 17 && output(&f)[0x1a] == 0);

    for (unsigned form = 0; form < 255; ++form) {
        initialize(&f);
        f.source[0x1a] = (uint8_t)form;
        expect(&f, 'G', 25, form, UINT32_C(0x34), UINT32_C(0x1234));
        run(&f);
        CHECK(output(&f)[0x1a] == form);
    }

    initialize(&f);
    f.source[0x1a] = 255;
    s = expect(&f, 'S', 25, 0, 0, 0);
    edit(s, 0x18, 2, 26);
    s = expect(&f, 'P', 0, 0, 0, 0);
    edit(s, 0x18, 2, 27);
    s = expect(&f, 'L', 27, 0, 0, 0);
    edit(s, 0x18, 2, 28);
    expect(&f, 'A', UINT32_C(0x20), 0, 0, UINT32_C(0x102));
    expect(&f, 'B', 2, 0, 0, UINT32_C(0x101));
    expect(&f, 'G', 28, 1, UINT32_C(0x34), UINT32_C(0x1234));
    run(&f);
    CHECK(output(&f)[0x1a] == 1 && get_value(output(&f) + 0x18, 2) == 28);

    /* The original forwards a zero low-byte count instead of adding a guard. */
    initialize(&f);
    f.source[0x1a] = 255;
    expect(&f, 'S', 25, 0, 0, 0);
    expect(&f, 'P', 0, 0, 0, 0);
    expect(&f, 'L', 25, 0, 0, 0);
    expect(&f, 'A', UINT32_C(0x20), 0, 0, UINT32_C(0x100));
    expect(&f, 'B', 0, 0, 0, UINT32_C(0x102));
    expect(&f, 'G', 25, 2, UINT32_C(0x34), UINT32_C(0x1234));
    run(&f);
    CHECK(output(&f)[0x1a] == 2);
}

static void test_gender_and_final_word(void)
{
    struct parameter_fixture f;
    const unsigned genders[] = {0, 1, 2, 3, 254, 255, 256, 511, 65535};
    for (unsigned i = 0; i < sizeof genders / sizeof genders[0]; ++i) {
        initialize(&f);
        put_value(f.source + 0x1e, 2, genders[i]);
        if (genders[i] == 255) {
            expect(&f, 'L', 25, 3, 0, 0);
            expect(&f, 'A', UINT32_C(0x14), 0, 0, 0);
        }
        expect(&f, 'G', 25, 3, genders[i] == 255 ? 0 : (genders[i] & 255), UINT32_C(0xaabbccdd));
        run(&f);
        CHECK(get_value(output(&f) + 0x1e, 2) == UINT32_C(0xccdd));
    }
    const struct { uint32_t ratio, draw, expected; unsigned random; } cases[] = {
        {0, 0, 0, 0}, {254, 0, 1, 0}, {255, 0, 2, 0},
        {1, 0, 0, 1}, {1, UINT32_MAX, 1, 1},
        {2, 0, 1, 1}, {2, 1, 0, 1},
        {253, 251, 1, 1}, {253, 252, 0, 1},
        {256, 251, 1, 1}, {256, UINT32_MAX, 1, 1},
        {256, UINT32_MAX - 1, 0, 1},
        {UINT32_MAX, UINT32_MAX - 1, 0, 1}, {UINT32_MAX, UINT32_MAX, 1, 1},
        {UINT32_C(0x100fe), 251, 1, 1}, {UINT32_C(0x100ff), 251, 1, 1}
    };
    for (unsigned i = 0; i < sizeof cases / sizeof cases[0]; ++i) {
        initialize(&f);
        put_value(f.source + 0x1e, 2, 255);
        expect(&f, 'L', 25, 3, 0, 0);
        expect(&f, 'A', UINT32_C(0x14), 0, 0, cases[i].ratio);
        if (cases[i].random) expect(&f, 'B', 252, 0, 0, cases[i].draw);
        expect(&f, 'G', 25, 3, cases[i].expected, cases[i].expected);
        run(&f);
        CHECK(get_value(output(&f) + 0x1e, 2) == cases[i].expected);
    }

    initialize(&f);
    put_value(f.source + 0x1e, 2, 255);
    expect(&f, 'L', 25, 3, 0, 0);
    struct parameter_step *s = expect(&f, 'A', UINT32_C(0x14), 0, 0, 2);
    edit(s, 0x18, 2, 1000);
    edit(s, 0x1a, 1, 7);
    edit(s, 0x1e, 2, UINT32_C(0xbeef));
    s = expect(&f, 'B', 252, 0, 0, 0);
    observe(s, 0x1e, 2, UINT32_C(0xbeef));
    s = expect(&f, 'G', 1000, 7, 1, UINT32_C(0x12340002));
    observe(s, 0x1e, 2, 1);
    run(&f);
    CHECK(get_value(output(&f) + 0x1e, 2) == 2);

    const uint32_t final_values[] = {0, UINT32_C(0xffff), UINT32_C(0x1ffff), UINT32_MAX};
    for (unsigned i = 0; i < sizeof final_values / sizeof final_values[0]; ++i) {
        initialize(&f);
        put_value(f.source + 0x30, 4, final_values[i]);
        standard_gender(&f);
        if (final_values[i] == UINT32_C(0xffff)) {
            expect(&f, 'L', 25, 3, 0, 0);
            expect(&f, 'A', UINT32_C(0x16), 0, 0, UINT32_C(0xfedcba98));
        }
        run(&f);
        CHECK(get_value(output(&f) + 0x30, 4) ==
              (final_values[i] == UINT32_C(0xffff) ? UINT32_C(0xfedcba98) : final_values[i]));
    }

    initialize(&f);
    s = expect(&f, 'G', 25, 3, UINT32_C(0x34), UINT32_C(0x87654321));
    edit(s, 0x30, 4, UINT32_C(0xffff));
    edit(s, 0x18, 2, UINT32_C(0x2222));
    edit(s, 0x1a, 1, 7);
    edit(s, 0x1e, 2, UINT32_C(0xaaaa));
    s = expect(&f, 'L', UINT32_C(0x2222), 7, 0, 0);
    observe(s, 0x1e, 2, UINT32_C(0x4321));
    edit(s, 0x1e, 2, UINT32_C(0xbeef));
    expect(&f, 'A', UINT32_C(0x16), 0, 0, UINT32_C(0xf00dcafe));
    run(&f);
    CHECK(get_value(output(&f) + 0x1e, 2) == UINT32_C(0xbeef));
    CHECK(get_value(output(&f) + 0x30, 4) == UINT32_C(0xf00dcafe));
}

void test_pokemon_parameters(void)
{
    test_copy_and_pair_sentinels();
    test_candidate_modes();
    test_level_and_stat_counts();
    test_ability_nature_and_forms();
    test_gender_and_final_word();
    printf("PASS: %u parameter normalization cases; 65536 levels; 256 stat counts; "
           "256 ability selectors; 255 explicit forms; sentinel pairs; retries; "
           "callback mutation; narrowing and unsigned wrap\n", parameter_runs);
}
