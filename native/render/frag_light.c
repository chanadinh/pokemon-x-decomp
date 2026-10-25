#include "frag_light.h"

#include <math.h>
#include <string.h>

void px_frag_light_inputs(struct px_frag_light_config *c, uint32_t absolute, uint32_t select, uint32_t scale)
{
    static const float scales[8] = {1, 2, 4, 8, 1, 1, 0.25f, 0.5f};
    for (unsigned k = 0; k < PX_LUT_SLOTS; ++k) {
        c->input[k] = (uint8_t)((select >> (4 * k)) & 7);
        c->absolute[k] = !((absolute >> (4 * k + 1)) & 1);
        c->scale[k] = scales[(scale >> (4 * k)) & 7];
    }
}

float px_frag_lut(const uint32_t *lut, float x, int absolute, int two_sided)
{
    unsigned index;
    float delta;
    if (absolute) {
        x = two_sided ? fabsf(x) : (x > 0 ? x : 0);
        float f = floorf(x * 256.0f);
        if (f < 0) f = 0;
        if (f > 255) f = 255;
        index = (unsigned)f;
        delta = x * 256.0f - (float)index;
    } else {
        float f = floorf(x * 128.0f);
        if (f < -128) f = -128;
        if (f > 127) f = 127;
        const int s = (int)f;
        delta = x * 128.0f - (float)s;
        index = (unsigned)(s & 255);
    }
    const uint32_t e = lut[index];
    const float value = (float)(e & 0xfff) / 4095.0f;
    float diff = (float)((e >> 12) & 0x7ff) / 2047.0f;
    if ((e >> 23) & 1) diff = -diff;
    return value + diff * delta;
}

static float dot3(const float a[3], const float b[3]) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; }
static int normalize(float v[3])
{
    const float l = sqrtf(dot3(v, v));
    if (!(l > 0) || !isfinite(l)) return 0;
    for (unsigned k = 0; k < 3; ++k) v[k] /= l;
    return 1;
}
static float clamp01(float x) { return x < 0 ? 0 : x > 1 ? 1 : x; }

int px_frag_light(const struct px_frag_light_config *c, const float quaternion[4], const float view[3],
                  const uint8_t bump[4], uint8_t primary[4], uint8_t secondary[4])
{
    if (!c || !quaternion || !view || !primary || !secondary || c->count > 8 || (c->bump && !bump)) return 0;
    float q[4] = {quaternion[0], quaternion[1], quaternion[2], quaternion[3]};
    const float ql = sqrtf(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
    if (!(ql > 0) || !isfinite(ql)) return 0;
    for (unsigned k = 0; k < 4; ++k) q[k] /= ql;
    float p[3] = {0, 0, 1};
    if (c->bump) {
        for (unsigned k = 0; k < 3; ++k) p[k] = (float)bump[k] / 127.5f - 1.0f;
        if (c->bump_renormalize) {
            const float z2 = 1 - (p[0] * p[0] + p[1] * p[1]);
            p[2] = sqrtf(z2 > 0 ? z2 : 0);
        }
    }
    /* N = p + 2 q.xyz x (q.xyz x p + w p) */
    const float t[3] = {q[1] * p[2] - q[2] * p[1] + q[3] * p[0], q[2] * p[0] - q[0] * p[2] + q[3] * p[1],
                        q[0] * p[1] - q[1] * p[0] + q[3] * p[2]};
    const float n[3] = {p[0] + 2 * (q[1] * t[2] - q[2] * t[1]), p[1] + 2 * (q[2] * t[0] - q[0] * t[2]),
                        p[2] + 2 * (q[0] * t[1] - q[1] * t[0])};
    float v[3] = {view[0], view[1], view[2]};
    if (!normalize(v)) v[0] = v[1] = v[2] = 0; /* a zero view: every view-based input is 0 */
    float diffuse[4] = {0, 0, 0, 1}, specular[4] = {0, 0, 0, 1};
    for (uint32_t i = 0; i < c->count; ++i) {
        const struct px_frag_light_source *s = &c->lights[i];
        float l[3] = {s->vector[0], s->vector[1], s->vector[2]};
        if (!normalize(l)) return 0;
        float h[3] = {v[0] + l[0], v[1] + l[1], v[2] + l[2]}, hn[3] = {h[0], h[1], h[2]};
        const int h_ok = normalize(hn);
        float inputs[4];
        inputs[PX_LUT_NH] = h_ok ? dot3(n, hn) : 0;
        inputs[PX_LUT_VH] = h_ok ? dot3(v, hn) : 0;
        inputs[PX_LUT_NV] = dot3(n, v);
        inputs[PX_LUT_LN] = dot3(l, n);
#define LOOKUP(slot) (c->input[slot] < 4 ? c->scale[slot] * px_frag_lut(c->lut[slot], inputs[c->input[slot]], \
                      c->absolute[slot], s->two_sided) : 0.0f)
        const float d0 = c->lut[PX_LUT_D0] ? LOOKUP(PX_LUT_D0) : 1.0f;
        const float d1 = c->lut[PX_LUT_D1] ? LOOKUP(PX_LUT_D1) : 1.0f;
        float refl[3];
        refl[0] = c->lut[PX_LUT_RR] ? LOOKUP(PX_LUT_RR) : 1.0f;
        refl[1] = c->lut[PX_LUT_RG] ? LOOKUP(PX_LUT_RG) : refl[0];
        refl[2] = c->lut[PX_LUT_RB] ? LOOKUP(PX_LUT_RB) : refl[0];
        if (i + 1 == c->count && c->lut[PX_LUT_FR]) {
            const float fresnel = LOOKUP(PX_LUT_FR);
            if (c->fresnel_primary) diffuse[3] = fresnel;
            if (c->fresnel_secondary) specular[3] = fresnel;
        }
#undef LOOKUP
        float d = dot3(l, n);
        d = s->two_sided ? fabsf(d) : (d > 0 ? d : 0);
        const float highlight = c->clamp_highlights ? (d == 0 ? 0.0f : 1.0f) : 1.0f;
        float geometric = 1;
        if (s->geometric0 || s->geometric1) {
            const float h2 = dot3(h, h);
            geometric = h2 == 0 ? 0 : (d / h2 < 1 ? d / h2 : 1);
        }
        for (unsigned k = 0; k < 3; ++k) {
            const float s0 = d0 * s->specular0[k] / 255.0f * (s->geometric0 ? geometric : 1);
            const float s1 = d1 * refl[k] * s->specular1[k] / 255.0f * (s->geometric1 ? geometric : 1);
            diffuse[k] += s->diffuse[k] / 255.0f * d + s->ambient[k] / 255.0f;
            specular[k] += (s0 + s1) * highlight;
        }
    }
    for (unsigned k = 0; k < 3; ++k) diffuse[k] += c->global_ambient[k] / 255.0f;
    for (unsigned k = 0; k < 4; ++k) {
        primary[k] = (uint8_t)(clamp01(diffuse[k]) * 255.0f);
        secondary[k] = (uint8_t)(clamp01(specular[k]) * 255.0f);
    }
    return 1;
}
