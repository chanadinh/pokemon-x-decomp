#include "tev.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Hand-computed expectations. Each config passes stage 0's result through
 * five replace-previous stages unless a test configures more. */
static struct px_tev_config passthrough(void)
{
    struct px_tev_config c;
    memset(&c, 0, sizeof c);
    for (unsigned i = 1; i < 6; ++i) c.stages[i].source = 0x00ff00ff; /* previous */
    return c;
}
static struct px_tev_inputs inputs(void)
{
    struct px_tev_inputs in;
    memset(&in, 0, sizeof in);
    const uint8_t primary[4] = {200, 100, 50, 128}, light[4] = {255, 128, 64, 32};
    const uint8_t secondary[4] = {10, 20, 30, 40};
    memcpy(in.primary, primary, 4);
    memcpy(in.fragment_primary, light, 4);
    memcpy(in.fragment_secondary, secondary, 4);
    const uint8_t t0[4] = {90, 180, 255, 60}, t1[4] = {1, 2, 3, 4}, t2[4] = {250, 5, 128, 200};
    memcpy(in.texture[0], t0, 4); memcpy(in.texture[1], t1, 4); memcpy(in.texture[2], t2, 4);
    return in;
}
static void expect(const struct px_tev_config *c, const struct px_tev_inputs *in,
                   int r, int g, int b, int a)
{
    uint8_t out[4];
    assert(px_tev_valid(c));
    px_tev_evaluate(c, in, out);
    if (out[0] != r || out[1] != g || out[2] != b || out[3] != a) {
        fprintf(stderr, "got %d %d %d %d want %d %d %d %d\n", out[0], out[1], out[2], out[3], r, g, b, a);
        assert(0);
    }
}
/* Stage 0 with RGB sources (a, b, c) and alpha sources (a, b, c). */
static struct px_tev_config op(uint32_t rgb_op, uint32_t alpha_op, uint32_t sources)
{
    struct px_tev_config c = passthrough();
    c.stages[0].source = sources | sources << 16;
    c.stages[0].combine = rgb_op | alpha_op << 16;
    return c;
}
static void test_operations(void)
{
    const struct px_tev_inputs in = inputs();
    /* Sources: a = texture0 (3), b = primary (0), c = texture2 (5). */
    const uint32_t abc = 0x503;
    expect(&(struct px_tev_config){0}, &in, 200, 100, 50, 128); /* every stage: replace primary */
    struct px_tev_config c = op(0, 0, abc);
    expect(&c, &in, 90, 180, 255, 60);
    c = op(1, 1, abc); /* a*b/255 */
    expect(&c, &in, 90 * 200 / 255, 180 * 100 / 255, 255 * 50 / 255, 60 * 128 / 255);
    c = op(2, 2, abc); /* min(a+b,255) */
    expect(&c, &in, 255, 255, 255, 188);
    c = op(3, 3, abc); /* clamp(a+b-128) */
    expect(&c, &in, 162, 152, 177, 60);
    c = op(4, 4, abc); /* (a*c + b*(255-c))/255 */
    expect(&c, &in, (90 * 250 + 200 * 5) / 255, (180 * 5 + 100 * 250) / 255,
           (255 * 128 + 50 * 127) / 255, (60 * 200 + 128 * 55) / 255);
    c = op(5, 5, abc); /* max(a-b,0) */
    expect(&c, &in, 0, 80, 205, 0);
    c = op(8, 8, abc); /* min((a*b + 255*c)/255, 255) */
    expect(&c, &in, 255, (180 * 100 + 255 * 5) / 255, 178, 230);
    c = op(9, 9, abc); /* min(a+b,255)*c/255 */
    expect(&c, &in, 255 * 250 / 255, 255 * 5 / 255, 255 * 128 / 255, 188 * 200 / 255);
    /* Dot3: sum of ((2a-255)(2b-255)+128)/256 with truncation toward zero. */
    c = op(6, 0, abc);
    {
        const int d = ((2 * 90 - 255) * (2 * 200 - 255) + 128) / 256 +
                      ((2 * 180 - 255) * (2 * 100 - 255) + 128) / 256 +
                      ((2 * 255 - 255) * (2 * 50 - 255) + 128) / 256;
        const int clamped = d < 0 ? 0 : d > 255 ? 255 : d;
        expect(&c, &in, clamped, clamped, clamped, 60);
    }
    /* Positive dot3 and Dot3_RGBA writing alpha, before alpha scale x2. */
    struct px_tev_inputs bright = in;
    const uint8_t p[4] = {255, 255, 200, 0};
    memcpy(bright.primary, p, 4);
    memcpy(bright.texture[0], p, 4);
    c = op(7, 9, abc); /* alpha op ignored (even an op Dot3 would reject) */
    c.stages[0].scale = 0x10000;
    expect(&c, &bright, 255, 255, 255, 255); /* 254 + 254 + 82, clamped */
    c.stages[0].combine = 7 | 6u << 16;
    assert(px_tev_valid(&c)); /* Dot3_RGBA ignores its alpha op field */
    c.stages[0].combine = 6 | 6u << 16;
    assert(!px_tev_valid(&c));
}
static void test_operands_scale_and_constant(void)
{
    const struct px_tev_inputs in = inputs();
    struct px_tev_config c = op(0, 0, 3);
    const uint32_t rgb_codes[10] = {0, 1, 2, 3, 4, 5, 8, 9, 12, 13};
    const int expected[10][3] = {
        {90, 180, 255}, {165, 75, 0}, {60, 60, 60}, {195, 195, 195}, {90, 90, 90},
        {165, 165, 165}, {180, 180, 180}, {75, 75, 75}, {255, 255, 255}, {0, 0, 0}};
    const int alpha_expected[8] = {60, 195, 90, 165, 180, 75, 255, 0};
    for (unsigned i = 0; i < 10; ++i) {
        c.stages[0].operand = rgb_codes[i] | (uint32_t)(i < 8 ? i : 0) << 12;
        expect(&c, &in, expected[i][0], expected[i][1], expected[i][2], alpha_expected[i < 8 ? i : 0]);
    }
    c.stages[0].operand = 6;
    assert(!px_tev_valid(&c));
    c.stages[0].operand = 0x8000;
    assert(!px_tev_valid(&c));
    /* Scales 2x, 4x and reserved 3 (=1x), with saturation. */
    c = op(0, 0, 0); /* primary */
    c.stages[0].scale = 1 | 2u << 16;
    expect(&c, &in, 255, 200, 100, 255);
    c.stages[0].scale = 3 | 3u << 16;
    expect(&c, &in, 200, 100, 50, 128);
    c.stages[0].scale = 4;
    assert(!px_tev_valid(&c));
    /* Constant RGBA byte order R,G,B,A. */
    c = op(0, 0, 14);
    c.stages[0].constant = 0x44332211;
    expect(&c, &in, 0x11, 0x22, 0x33, 0x44);
    c = op(1, 1, 0x21); /* fragment primary * fragment secondary */
    expect(&c, &in, 255 * 10 / 255, 128 * 20 / 255, 64 * 30 / 255, 32 * 40 / 255);
    assert(px_tev_sources(&c) == ((1u << 1) | (1u << 2) | 1u | (1u << 15)));
    /* Reserved source codes 7..12 reject. */
    for (uint32_t s = 7; s <= 12; ++s) { c = op(0, 0, s); assert(!px_tev_valid(&c)); }
    c = op(0, 0, 3); c.stages[0].source |= 0x1000; assert(!px_tev_valid(&c));
    c = op(10, 0, 3); assert(!px_tev_valid(&c));
    c = op(0, 0, 3); c.buffer_update = 1; assert(!px_tev_valid(&c));
    assert(!px_tev_valid(NULL));
}
static void test_previous_and_buffer(void)
{
    const struct px_tev_inputs in = inputs();
    /* Stage 0: previous as first source is replaced by the third (texture2).
     * Previous as the third source itself reads zero. */
    struct px_tev_config c = op(0, 0, 0xf);
    c.stages[0].source = 0x5ff | 0x0ff0000; /* RGB (prev, prev, tex2); alpha prev */
    expect(&c, &in, 250, 5, 128, 0); /* both substituted; alpha previous is zero */
    c.stages[0].source = 0x50f | 0x0ff0000;
    expect(&c, &in, 250, 5, 128, 0);
    /* texture2, primary (second RGB source and pass-through stages), previous */
    assert(px_tev_sources(&c) == ((1u << 5) | 1u | (1u << 15)));
    /* Buffer: initial color, then the one-stage delay. Stage 0 writes the
     * buffer (RGB+A); stage 1 still sees the initial color; stage 2 sees
     * stage 0's output. */
    c = passthrough();
    c.buffer_color = 0x0a141e28; /* R=0x28 G=0x1e B=0x14 A=0x0a */
    c.buffer_update = 0x1100;     /* stage 0 updates RGB and alpha */
    c.stages[0].source = 0x00030003; /* texture0 */
    c.stages[1].source = 0x000d000d; /* previous buffer */
    expect(&c, &in, 0x28, 0x1e, 0x14, 0x0a);
    c.stages[1].source = 0x00ff00ff;
    c.stages[2].source = 0x000d000d;
    expect(&c, &in, 90, 180, 255, 60);
    /* Separate RGB and alpha update masks. */
    c.buffer_update = 0x0100;
    expect(&c, &in, 90, 180, 255, 0x0a);
    /* Stages 4 and 5 never update the buffer: masks cover stages 0..3. */
    c = passthrough();
    c.buffer_update = 0xff00;
    c.stages[5].source = 0x000d000d;
    c.stages[3].source = 0x000e000e; c.stages[3].constant = 0x04030201;
    c.stages[4].source = 0x00030003;
    expect(&c, &in, 1, 2, 3, 4);
}
static void test_field_projected_chain(void)
{
    /* A chain modeled on field material t01r02_floor's decoded stages:
     * stage 4 computes (1 - tex0) * const(0) = 0, stage 5 multiplies the
     * buffered stage-3 result by (1 - 0). The projected texture cannot
     * affect the result with these constants. */
    struct px_tev_config c;
    memset(&c, 0, sizeof c);
    const uint32_t regs[6][5] = {
        {0x04440404, 0, 0x00000001, 0xff000000, 0}, {0x0fff0f1f, 0, 0x00000001, 0xff000000, 0},
        {0x0f0f0fff, 0, 0x00010000, 0xff000000, 0}, {0x0fef0fff, 0, 0x00010000, 0xff000000, 0},
        {0x0fff0fe3, 1, 0x00000001, 0, 0}, {0x0fff0ffd, 0x10, 0x00000001, 0xff000000, 0}};
    for (unsigned i = 0; i < 6; ++i) {
        struct px_tev_stage *s = &c.stages[i];
        s->source = regs[i][0]; s->operand = regs[i][1]; s->combine = regs[i][2];
        s->constant = regs[i][3]; s->scale = regs[i][4];
    }
    c.buffer_update = 0x0800;
    struct px_tev_inputs in = inputs();
    uint8_t a[4], b[4];
    assert(px_tev_valid(&c));
    px_tev_evaluate(&c, &in, a);
    memset(in.texture[0], 0, 4);
    px_tev_evaluate(&c, &in, b);
    assert(memcmp(a, b, 4) == 0);
    memset(in.texture[0], 255, 4);
    px_tev_evaluate(&c, &in, b);
    assert(memcmp(a, b, 4) == 0);
}
int main(void)
{
    test_operations();
    test_operands_scale_and_constant();
    test_previous_and_buffer();
    test_field_projected_chain();
    puts("TEV combiner tests passed");
    return 0;
}
