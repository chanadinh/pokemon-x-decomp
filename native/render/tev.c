#include "tev.h"

#include <string.h>

enum { REPLACE = 0, MODULATE, ADD, ADD_SIGNED, LERP, SUBTRACT, DOT3_RGB, DOT3_RGBA,
       MULTIPLY_ADD, ADD_MULTIPLY };

static int source_valid(uint32_t s) { return s <= 6 || (s >= 13 && s <= 15); }
static int rgb_operand_valid(uint32_t o) { return o <= 5 || o == 8 || o == 9 || o == 12 || o == 13; }

int px_tev_valid(const struct px_tev_config *config)
{
    if (!config || (config->buffer_update & ~UINT32_C(0xff00))) return 0;
    for (unsigned i = 0; i < 6; ++i) {
        const struct px_tev_stage *s = &config->stages[i];
        const uint32_t rgb_op = s->combine & 0xffff, alpha_op = s->combine >> 16;
        if ((s->source & 0xf000f000) || (s->operand & 0xff000000) ||
            (s->scale & ~UINT32_C(0x30003)) || rgb_op > 9) return 0;
        if (rgb_op != DOT3_RGBA && (alpha_op > 9 || alpha_op == DOT3_RGB || alpha_op == DOT3_RGBA))
            return 0;
        for (unsigned j = 0; j < 3; ++j) {
            if (!source_valid((s->source >> (4 * j)) & 15) ||
                !source_valid((s->source >> (16 + 4 * j)) & 15) ||
                !rgb_operand_valid((s->operand >> (4 * j)) & 15) ||
                ((s->operand >> (12 + 4 * j)) & 15) > 7) return 0;
        }
    }
    return 1;
}

static uint32_t rgb_source(const struct px_tev_stage *s, unsigned stage, unsigned j)
{
    uint32_t value = (s->source >> (4 * j)) & 15;
    if (stage == 0 && j < 2 && value == PX_TEV_PREVIOUS) value = (s->source >> 8) & 15;
    return value;
}

uint32_t px_tev_sources(const struct px_tev_config *config)
{
    uint32_t mask = 0;
    for (unsigned i = 0; i < 6; ++i) {
        const struct px_tev_stage *s = &config->stages[i];
        for (unsigned j = 0; j < 3; ++j) {
            mask |= UINT32_C(1) << rgb_source(s, i, j);
            if ((s->combine & 0xffff) != DOT3_RGBA)
                mask |= UINT32_C(1) << ((s->source >> (16 + 4 * j)) & 15);
        }
    }
    return mask;
}

static void unpack(uint32_t value, uint8_t out[4])
{
    for (unsigned c = 0; c < 4; ++c) out[c] = (uint8_t)(value >> (8 * c));
}

static const uint8_t *fetch(uint32_t source, const struct px_tev_inputs *in,
                            const uint8_t constant[4], const uint8_t buffer[4],
                            const uint8_t previous[4])
{
    switch (source) {
    case PX_TEV_PRIMARY: return in->primary;
    case PX_TEV_FRAGMENT_PRIMARY: return in->fragment_primary;
    case PX_TEV_FRAGMENT_SECONDARY: return in->fragment_secondary;
    case PX_TEV_TEXTURE0: case PX_TEV_TEXTURE1: case PX_TEV_TEXTURE2: case PX_TEV_TEXTURE3:
        return in->texture[source - PX_TEV_TEXTURE0];
    case PX_TEV_PREVIOUS_BUFFER: return buffer;
    case PX_TEV_CONSTANT: return constant;
    default: return previous;
    }
}

/* RGB operand: 0 color, 1 inverse color, 2/3 alpha, 4/5 red, 8/9 green,
 * 12/13 blue; odd codes invert. */
static void rgb_operand(uint32_t code, const uint8_t v[4], int out[3])
{
    int value[3];
    switch (code & ~1u) {
    case 0: for (unsigned c = 0; c < 3; ++c) value[c] = v[c]; break;
    case 2: value[0] = value[1] = value[2] = v[3]; break;
    case 4: value[0] = value[1] = value[2] = v[0]; break;
    case 8: value[0] = value[1] = value[2] = v[1]; break;
    default: value[0] = value[1] = value[2] = v[2]; break;
    }
    for (unsigned c = 0; c < 3; ++c) out[c] = (code & 1) ? 255 - value[c] : value[c];
}

/* Alpha operand: 0/1 alpha, 2/3 red, 4/5 green, 6/7 blue; odd codes invert. */
static int alpha_operand(uint32_t code, const uint8_t v[4])
{
    static const unsigned channel[4] = {3, 0, 1, 2};
    const int value = v[channel[code >> 1]];
    return (code & 1) ? 255 - value : value;
}

static int clamp255(int v) { return v < 0 ? 0 : v > 255 ? 255 : v; }

static int combine(uint32_t op, int a, int b, int c)
{
    switch (op) {
    case REPLACE: return a;
    case MODULATE: return a * b / 255;
    case ADD: return clamp255(a + b);
    case ADD_SIGNED: return clamp255(a + b - 128);
    case LERP: return (a * c + b * (255 - c)) / 255;
    case SUBTRACT: return clamp255(a - b);
    case MULTIPLY_ADD: return clamp255((a * b + 255 * c) / 255);
    default: return clamp255(a + b) * c / 255; /* ADD_MULTIPLY */
    }
}

static int dot3(const int a[3], const int b[3])
{
    int sum = 0;
    /* Each product is nonnegative-or-negative; C division truncates toward
     * zero, matching the reference's signed integer division. */
    for (unsigned c = 0; c < 3; ++c) sum += ((a[c] * 2 - 255) * (b[c] * 2 - 255) + 128) / 256;
    return clamp255(sum);
}

static int multiplier(uint32_t bits) { return bits < 3 ? 1 << bits : 1; }

void px_tev_evaluate(const struct px_tev_config *config, const struct px_tev_inputs *in,
                     uint8_t out[4])
{
    uint8_t previous[4] = {0, 0, 0, 0}, buffer[4] = {0, 0, 0, 0}, next[4];
    unpack(config->buffer_color, next);
    for (unsigned i = 0; i < 6; ++i) {
        const struct px_tev_stage *s = &config->stages[i];
        uint8_t constant[4], result[4];
        if (i && !s->combine && !s->scale && (s->source & 0xf000f) == 0xf000f && !(s->operand & 0xf00f)) {
            /* Pass-through: REPLACE of the previous color and alpha, unscaled,
             * leaves previous as it is; only the buffer bookkeeping remains. */
            memcpy(buffer, next, 4);
            if (i < 4 && (config->buffer_update >> (8 + i)) & 1) memcpy(next, previous, 3);
            if (i < 4 && (config->buffer_update >> (12 + i)) & 1) next[3] = previous[3];
            continue;
        }
        unpack(s->constant, constant);
        int rgb[3][3], alpha[3];
        for (unsigned j = 0; j < 3; ++j) {
            rgb_operand((s->operand >> (4 * j)) & 15,
                        fetch(rgb_source(s, i, j), in, constant, buffer, previous), rgb[j]);
            alpha[j] = alpha_operand((s->operand >> (12 + 4 * j)) & 15,
                fetch((s->source >> (16 + 4 * j)) & 15, in, constant, buffer, previous));
        }
        const uint32_t rgb_op = s->combine & 0xffff, alpha_op = s->combine >> 16;
        int color[3], a;
        if (rgb_op == DOT3_RGB || rgb_op == DOT3_RGBA) {
            color[0] = color[1] = color[2] = dot3(rgb[0], rgb[1]);
        } else {
            for (unsigned c = 0; c < 3; ++c) color[c] = combine(rgb_op, rgb[0][c], rgb[1][c], rgb[2][c]);
        }
        a = rgb_op == DOT3_RGBA ? color[0] : combine(alpha_op, alpha[0], alpha[1], alpha[2]);
        const int rgb_scale = multiplier(s->scale & 3), alpha_scale = multiplier((s->scale >> 16) & 3);
        for (unsigned c = 0; c < 3; ++c) result[c] = (uint8_t)(color[c] * rgb_scale > 255 ? 255 : color[c] * rgb_scale);
        result[3] = (uint8_t)(a * alpha_scale > 255 ? 255 : a * alpha_scale);
        memcpy(previous, result, 4);
        memcpy(buffer, next, 4);
        if (i < 4 && (config->buffer_update >> (8 + i)) & 1) memcpy(next, result, 3);
        if (i < 4 && (config->buffer_update >> (12 + i)) & 1) next[3] = result[3];
    }
    memcpy(out, previous, 4);
}
