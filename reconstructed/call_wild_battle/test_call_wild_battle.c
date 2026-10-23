#include "call_wild_battle.h"

#include <stdio.h>
#include <stdlib.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #condition); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

/* Logical test state only: field offset and host layout are intentionally unrelated. */
struct px_cwb_context {
    uint32_t logical_word_at_0c;
};

struct fixture {
    uint32_t args[2];
    uint32_t expected_argument;
    uint32_t expected_field;
    unsigned step;
    int mutate_args;
    struct px_cwb_context first;
    struct px_cwb_context replacement;
    struct px_cwb_context *active;
};

static struct px_cwb_context *get_context(void *user)
{
    struct fixture *f = user;
    CHECK(f->step++ == 0);
    if (f->mutate_args) {
        /* An implementation that reads args after this callback must fail. */
        f->args[1] = UINT32_C(0xdeadbeef);
    }
    /* The context word must be read after this accessor returns. */
    f->first.logical_word_at_0c = f->expected_field;
    return f->active;
}

static uint32_t read_context_word_0c(void *user, struct px_cwb_context *context)
{
    struct fixture *f = user;
    CHECK(f->step++ == 1);
    CHECK(context == &f->first);
    return context->logical_word_at_0c;
}

static void field_helper(void *user, uint32_t field_value,
                         uint32_t argument, uint32_t zero)
{
    struct fixture *f = user;
    CHECK(f->step++ == 2);
    CHECK(field_value == f->expected_field);
    CHECK(argument == f->expected_argument);
    CHECK(zero == UINT32_C(0));
    /* The final callback must still receive the original context. */
    f->active = &f->replacement;
    f->first.logical_word_at_0c = UINT32_C(0x11223344);
}

static void context_helper(void *user, struct px_cwb_context *context)
{
    struct fixture *f = user;
    CHECK(f->step++ == 3);
    CHECK(context == &f->first);
    CHECK(context != f->active);
    CHECK(context->logical_word_at_0c == UINT32_C(0x11223344));
}

static void run_case(uint32_t input, uint32_t expected, int mutate_args)
{
    struct fixture f = {0};
    f.args[0] = UINT32_C(0xcafebabe);
    f.args[1] = input;
    f.expected_argument = expected;
    f.expected_field = UINT32_C(0x89abcdef);
    f.mutate_args = mutate_args;
    f.active = &f.first;

    const struct px_cwb_api api = {
        &f, get_context, read_context_word_0c, field_helper, context_helper
    };

    CHECK(px_call_wild_battle(f.args, &api) == UINT32_C(0));
    CHECK(f.step == 4);
    CHECK(f.args[0] == UINT32_C(0xcafebabe));
    CHECK(f.args[1] == (mutate_args ? UINT32_C(0xdeadbeef) : input));
}

int main(void)
{
    static const struct {
        uint32_t input;
        uint32_t expected;
    } cases[] = {
        {UINT32_C(0),          UINT32_C(0)},
        {UINT32_C(1),          UINT32_C(1)},
        {UINT32_C(0xfffe),     UINT32_C(0xfffe)},
        {UINT32_C(0xffff),     UINT32_C(0xffff)},
        {UINT32_C(0x10000),    UINT32_C(0)},
        {UINT32_C(0x12345678), UINT32_C(0x5678)},
        {UINT32_C(0xffff0000), UINT32_C(0)},
        {UINT32_C(0xffffffff), UINT32_C(0xffff)}
    };

    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i) {
        run_case(cases[i].input, cases[i].expected, 0);
    }
    run_case(UINT32_C(0x12345678), UINT32_C(0x5678), 1);
    puts("PASS: 9 cases; argument snapshot, narrowing, call order, context identity, zero return");
    return EXIT_SUCCESS;
}
