/* The fragment lighting unit on hand-built LUTs and lights. */
#include "frag_light.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static uint32_t entry(unsigned value, int diff) /* value/4095, diff/2047 */
{
    return (value & 0xfff) | (uint32_t)((diff < 0 ? -diff : diff) & 0x7ff) << 12 | (uint32_t)(diff < 0) << 23;
}

int main(void)
{
    static uint32_t ramp[256], constant[256];
    for (unsigned k = 0; k < 256; ++k) { ramp[k] = entry(16 * k, 32); constant[k] = entry(2047, 0); }
    /* Absolute: x = 0.5 -> index 128, fraction 0; x = 0.501953125 -> 128.5. */
    assert(fabsf(px_frag_lut(ramp, 0.5f, 1, 0) - 2048.0f / 4095.0f) < 1e-6f);
    assert(fabsf(px_frag_lut(ramp, 128.5f / 256.0f, 1, 0) - (2048.0f / 4095.0f + 0.5f * 32.0f / 2047.0f)) < 1e-6f);
    assert(px_frag_lut(ramp, -0.5f, 1, 0) == 0.0f);                     /* clamped at 0 */
    assert(fabsf(px_frag_lut(ramp, -0.5f, 1, 1) - 2048.0f / 4095.0f) < 1e-6f); /* two-sided: |x| */
    /* Signed: x = -0.5 -> index -64 stored as byte 192. */
    assert(fabsf(px_frag_lut(ramp, -0.5f, 0, 0) - 16.0f * 192 / 4095.0f) < 1e-6f);
    assert(fabsf(px_frag_lut(ramp, 0.25f, 0, 0) - 16.0f * 32 / 4095.0f) < 1e-6f);
    /* Negative differences. */
    static uint32_t down[256];
    for (unsigned k = 0; k < 256; ++k) down[k] = entry(4000, -100);
    assert(fabsf(px_frag_lut(down, 0.5f / 256.0f, 1, 0) - (4000.0f / 4095.0f - 0.5f * 100.0f / 2047.0f)) < 1e-6f);

    /* Register decoding: d0 absolute (bit 1 clear), rr signed; select rr = NV;
     * scale d0 = 2x, rr = 0.5x. */
    struct px_frag_light_config c;
    memset(&c, 0, sizeof c);
    px_frag_light_inputs(&c, 0x2000000, 0x2000000, 0x7000001);
    assert(c.absolute[PX_LUT_D0] && !c.absolute[PX_LUT_RR] && c.input[PX_LUT_RR] == PX_LUT_NV);
    assert(c.scale[PX_LUT_D0] == 2 && c.scale[PX_LUT_RR] == 0.5f && c.scale[PX_LUT_D1] == 1);

    /* Diffuse only: one light along +Z onto the +Z normal (identity quaternion). */
    memset(&c, 0, sizeof c);
    c.count = 1;
    c.lights[0] = (struct px_frag_light_source){{0, 0, 0}, {0, 0, 0}, {255, 128, 0}, {10, 10, 10}, {0, 0, 2}, 0, 0, 0};
    c.global_ambient[0] = 20;
    const float identity[4] = {0, 0, 0, 1}, view[3] = {0, 0, 3};
    uint8_t p[4], s[4];
    assert(px_frag_light(&c, identity, view, NULL, p, s));
    assert(p[0] == 255 && p[1] == 138 && p[2] == 10 && p[3] == 255);
    assert(s[0] == 0 && s[1] == 0 && s[2] == 0 && s[3] == 255);
    /* Light at 60 degrees: dot 0.5. A quaternion turning +Z by 90 degrees about
     * X makes the normal -Y, away from the light: ambient only. */
    c.lights[0].vector[0] = 0; c.lights[0].vector[1] = sqrtf(3.0f); c.lights[0].vector[2] = 1;
    assert(px_frag_light(&c, identity, view, NULL, p, s) && p[0] == (uint8_t)((255 * 0.5f + 10 + 20) / 255.0f * 255.0f));
    const float turn[4] = {sinf(0.785398163f), 0, 0, cosf(0.785398163f)};
    assert(px_frag_light(&c, turn, view, NULL, p, s) && p[0] == 30 && p[1] == 10);

    /* Reflection: specular1 white, RR = 0.5 constant, RG/RB fall back to RR. */
    c.lights[0].vector[1] = 0;
    for (unsigned k = 0; k < 3; ++k) c.lights[0].specular1[k] = 255;
    c.lut[PX_LUT_RR] = constant;
    px_frag_light_inputs(&c, 0x2222222, 0x2220000, 0);
    assert(px_frag_light(&c, identity, view, NULL, p, s) && s[0] == 127 && s[1] == 127 && s[2] == 127);
    /* RG from a ramp of N.V (signed): N.V = 1 -> index 127. */
    c.lut[PX_LUT_RG] = ramp;
    assert(px_frag_light(&c, identity, view, NULL, p, s) && s[1] == (uint8_t)((16.0f * 127 / 4095.0f + 32.0f / 2047.0f) * 255.0f));
    /* clamp_highlights: no specular where the light is behind. */
    c.clamp_highlights = 1;
    c.lights[0].vector[2] = -1;
    assert(px_frag_light(&c, identity, view, NULL, p, s) && s[0] == 0);
    /* Bump: texel (255, 128, 128) is about +X; with renormalization z is rebuilt. */
    c.clamp_highlights = 0; c.lut[PX_LUT_RR] = c.lut[PX_LUT_RG] = NULL;
    c.lights[0].vector[0] = 1; c.lights[0].vector[2] = 0;
    c.bump = 1; c.bump_renormalize = 1;
    const uint8_t texel[4] = {255, 128, 128, 255};
    assert(px_frag_light(&c, identity, view, texel, p, s) && p[0] == 255);
    c.bump_renormalize = 0;
    const uint8_t flat[4] = {128, 128, 255, 255};
    assert(px_frag_light(&c, identity, view, flat, p, s) && p[0] >= 30 && p[0] <= 31); /* normal ~ +Z, light +X: dot = 1/255 */
    /* Invalid input. */
    const float zero[4] = {0, 0, 0, 0};
    assert(!px_frag_light(&c, zero, view, texel, p, s) && !px_frag_light(&c, identity, view, NULL, p, s));
    c.count = 9;
    assert(!px_frag_light(&c, identity, view, texel, p, s));
    printf("fragment lighting tests passed\n");
    return 0;
}
