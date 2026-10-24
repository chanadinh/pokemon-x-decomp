#include "raster.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Coverage convention: Microsoft's documented pixel-center/top-left rule.
 * https://learn.microsoft.com/en-us/windows/uwp/graphics-concepts/introduction-to-rasterization-rules
 * Sampling follows texel-center clamp/repeat/mirror and linear interpolation:
 * https://registry.khronos.org/OpenGL/specs/gl/glspec14.pdf
 * These are explicit software preview rules, not an emulation of PICA. */

struct point { int64_t x, y; struct px_raster_vertex attr; };

static int target_valid(const struct px_raster_target *target)
{
    return target && target->width && target->height &&
        target->width <= PX_RASTER_MAX_SIDE && target->height <= PX_RASTER_MAX_SIDE &&
        (uint64_t)target->width * target->height <= PX_RASTER_MAX_PIXELS &&
        target->rgba && target->depth;
}

enum px_raster_result px_raster_create(uint32_t width, uint32_t height,
                                      struct px_raster_target *out)
{
    size_t pixels;
    if (!out) return PX_RASTER_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (!width || !height) return PX_RASTER_INVALID_ARGUMENT;
    if (width > PX_RASTER_MAX_SIDE || height > PX_RASTER_MAX_SIDE ||
        (uint64_t)width * height > PX_RASTER_MAX_PIXELS) return PX_RASTER_LIMIT_EXCEEDED;
    pixels = (size_t)width * height;
    out->rgba = malloc(pixels * 4);
    out->depth = malloc(pixels * sizeof(*out->depth));
    if (!out->rgba || !out->depth) {
        px_raster_free(out);
        return PX_RASTER_NO_MEMORY;
    }
    out->width = width;
    out->height = height;
    return PX_RASTER_OK;
}

enum px_raster_result px_raster_clear(struct px_raster_target *target,
                                     const uint8_t rgba[4], double depth)
{
    size_t count;
    uint8_t color[4];
    if (!target_valid(target) || !rgba || isnan(depth) || depth == -INFINITY)
        return PX_RASTER_INVALID_ARGUMENT;
    memcpy(color, rgba, 4);
    count = (size_t)target->width * target->height;
    for (size_t i = 0; i < count; ++i) {
        memcpy(target->rgba + i * 4, color, 4);
        target->depth[i] = depth;
    }
    target->candidate_pixels = 0;
    return PX_RASTER_OK;
}

void px_raster_free(struct px_raster_target *target)
{
    if (!target) return;
    free(target->rgba);
    free(target->depth);
    memset(target, 0, sizeof(*target));
}

struct px_raster_state px_raster_default_state(void)
{
    const struct px_raster_state state = {
        1, 1, PX_RASTER_LESS, PX_RASTER_ALWAYS, 0,
        PX_RASTER_CULL_NONE, PX_RASTER_CW, PX_RASTER_REPLACE, 15
    };
    return state;
}

static int compare_valid(enum px_raster_compare comparison)
{
    return comparison >= PX_RASTER_NEVER && comparison <= PX_RASTER_GREATER;
}

static int state_valid(const struct px_raster_state *state)
{
    return state && (state->depth_test == 0 || state->depth_test == 1) &&
        (state->depth_write == 0 || state->depth_write == 1) &&
        compare_valid(state->depth_compare) && compare_valid(state->alpha_compare) &&
        state->cull >= PX_RASTER_CULL_NONE && state->cull <= PX_RASTER_CULL_BOTH &&
        state->front >= PX_RASTER_CW && state->front <= PX_RASTER_CCW &&
        state->blend >= PX_RASTER_REPLACE && state->blend <= PX_RASTER_ADD_SOURCE_ALPHA &&
        state->color_write_mask <= 15;
}

static int sample_valid(const struct px_raster_image *image,
                        const struct px_raster_sampler *sampler)
{
    if (!image) return 1;
    return image->width && image->height && image->width <= PX_RASTER_MAX_SIDE &&
        image->height <= PX_RASTER_MAX_SIDE && image->rgba &&
        image->rgba_size >= (size_t)image->width * image->height * 4 && sampler &&
        sampler->wrap_u >= PX_RASTER_CLAMP && sampler->wrap_u <= PX_RASTER_MIRROR &&
        sampler->wrap_v >= PX_RASTER_CLAMP && sampler->wrap_v <= PX_RASTER_MIRROR &&
        sampler->filter >= PX_RASTER_NEAREST && sampler->filter <= PX_RASTER_LINEAR;
}

static double clamp(double value)
{
    return value < 0 ? 0 : value > 1 ? 1 : value;
}

static uint8_t byte(double value)
{
    return (uint8_t)floor(clamp(value) * 255.0 + 0.5);
}

static int compare(double a, double b, enum px_raster_compare comparison)
{
    switch (comparison) {
    case PX_RASTER_NEVER: return 0;
    case PX_RASTER_ALWAYS: return 1;
    case PX_RASTER_LESS: return a < b;
    case PX_RASTER_LEQUAL: return a <= b;
    case PX_RASTER_EQUAL: return a == b;
    case PX_RASTER_NOTEQUAL: return a != b;
    case PX_RASTER_GEQUAL: return a >= b;
    case PX_RASTER_GREATER: return a > b;
    }
    return 0;
}

static double coordinate(double value, enum px_raster_wrap wrap)
{
    if (wrap == PX_RASTER_CLAMP) return clamp(value);
    if (wrap == PX_RASTER_REPEAT) return value - floor(value);
    value = fmod(value, 2.0);
    if (value < 0) value += 2;
    return value > 1 ? 2 - value : value;
}

static uint32_t texel(int value, uint32_t size, enum px_raster_wrap wrap)
{
    const int n = (int)size;
    if (wrap == PX_RASTER_CLAMP) return value < 0 ? 0 : value >= n ? size - 1 : (uint32_t)value;
    if (wrap == PX_RASTER_REPEAT) {
        value %= n;
        if (value < 0) value += n;
    } else {
        value %= 2 * n;
        if (value < 0) value += 2 * n;
        if (value >= n) value = 2 * n - 1 - value;
    }
    return (uint32_t)value;
}

static void sample(const struct px_raster_image *image,
                   const struct px_raster_sampler *sampler, double u, double v,
                   double rgba[4])
{
    double x, y;
    if (!image) {
        for (unsigned c = 0; c < 4; ++c) rgba[c] = 1;
        return;
    }
    x = coordinate(u, sampler->wrap_u) * image->width;
    y = coordinate(v, sampler->wrap_v) * image->height;
    if (sampler->filter == PX_RASTER_NEAREST) {
        uint32_t tx = texel((int)floor(x), image->width, sampler->wrap_u);
        uint32_t ty = texel((int)floor(y), image->height, sampler->wrap_v);
        const uint8_t *color = image->rgba + ((size_t)ty * image->width + tx) * 4;
        for (unsigned c = 0; c < 4; ++c) rgba[c] = color[c] / 255.0;
    } else {
        int ix, iy;
        double fx, fy;
        uint32_t tx[2], ty[2];
        x -= 0.5;
        y -= 0.5;
        ix = (int)floor(x);
        iy = (int)floor(y);
        fx = x - ix;
        fy = y - iy;
        for (unsigned i = 0; i < 2; ++i) {
            tx[i] = texel(ix + (int)i, image->width, sampler->wrap_u);
            ty[i] = texel(iy + (int)i, image->height, sampler->wrap_v);
        }
        for (unsigned c = 0; c < 4; ++c) {
            double a = image->rgba[((size_t)ty[0] * image->width + tx[0]) * 4 + c];
            double b = image->rgba[((size_t)ty[0] * image->width + tx[1]) * 4 + c];
            double d = image->rgba[((size_t)ty[1] * image->width + tx[0]) * 4 + c];
            double e = image->rgba[((size_t)ty[1] * image->width + tx[1]) * 4 + c];
            rgba[c] = ((a + (b - a) * fx) * (1 - fy) + (d + (e - d) * fx) * fy) / 255.0;
        }
    }
}

static int64_t edge(const struct point *a, const struct point *b, int64_t x, int64_t y)
{
    return (b->x - a->x) * (y - a->y) - (b->y - a->y) * (x - a->x);
}

static int top_left(const struct point *a, const struct point *b)
{
    return b->y < a->y || (b->y == a->y && b->x > a->x);
}

static void blend(uint8_t *dst, const double src[4], const struct px_raster_state *state)
{
    double result[4];
    if (state->blend == PX_RASTER_REPLACE) {
        memcpy(result, src, sizeof(result));
    } else if (state->blend == PX_RASTER_SOURCE_ALPHA) {
        for (unsigned c = 0; c < 4; ++c)
            result[c] = src[c] * src[3] + (dst[c] / 255.0) * (1 - src[3]);
    } else if (state->blend == PX_RASTER_ADD_SOURCE_ALPHA) {
        for (unsigned c = 0; c < 4; ++c) {
            result[c] = src[c] * src[3] + dst[c] / 255.0;
            if (result[c] > 1) result[c] = 1;
        }
    } else {
        double da = dst[3] / 255.0;
        result[3] = src[3] + da * (1 - src[3]);
        for (unsigned c = 0; c < 3; ++c)
            result[c] = result[3] > 0 ?
                (src[c] * src[3] + (dst[c] / 255.0) * da * (1 - src[3])) / result[3] : 0;
    }
    for (unsigned c = 0; c < 4; ++c)
        if (state->color_write_mask & (1u << c)) dst[c] = byte(result[c]);
}

int px_raster_image_valid(const struct px_raster_image *image,
                          const struct px_raster_sampler *sampler)
{
    return image && sample_valid(image, sampler);
}

void px_raster_sample(const struct px_raster_image *image,
                      const struct px_raster_sampler *sampler, double u, double v,
                      double rgba[4])
{
    sample(image, sampler, u, v, rgba);
}

static enum px_raster_result draw(struct px_raster_target *target,
    const struct px_raster_vertex vertices[3], const struct px_raster_image *image,
    const struct px_raster_sampler *sampler, px_raster_shader shader, void *context,
    const struct px_raster_state *state)
{
    struct point p[3];
    int64_t area, minx, miny, maxx, maxy;
    int xmin, ymin, xmax, ymax, inclusive[3];
    uint64_t candidates;
    if (!target_valid(target) || !vertices || !state_valid(state) || !sample_valid(image, sampler))
        return PX_RASTER_INVALID_ARGUMENT;
    for (unsigned i = 0; i < 3; ++i) {
        const struct px_raster_vertex *v = vertices + i;
        if (!isfinite(v->x) || !isfinite(v->y) || !isfinite(v->z) ||
            !isfinite(v->u) || !isfinite(v->v)) return PX_RASTER_INVALID_ARGUMENT;
        for (unsigned c = 0; c < 4; ++c)
            if (!isfinite(v->rgba[c])) return PX_RASTER_INVALID_ARGUMENT;
        if (fabs(v->x) > 1e6 || fabs(v->y) > 1e6 || fabs(v->z) > 1e12 ||
            fabs(v->u) > 1e12 || fabs(v->v) > 1e12) return PX_RASTER_LIMIT_EXCEEDED;
        p[i].x = llround(v->x * 256);
        p[i].y = llround(v->y * 256);
        p[i].attr = *v;
        for (unsigned c = 0; c < 4; ++c) p[i].attr.rgba[c] = clamp(v->rgba[c]);
    }
    area = edge(p, p + 1, p[2].x, p[2].y);
    if (!area || state->cull == PX_RASTER_CULL_BOTH) return PX_RASTER_OK;
    {
        int front = (area > 0) == (state->front == PX_RASTER_CW);
        if ((state->cull == PX_RASTER_CULL_FRONT && front) ||
            (state->cull == PX_RASTER_CULL_BACK && !front)) return PX_RASTER_OK;
    }
    if (area < 0) {
        struct point temporary = p[1];
        p[1] = p[2];
        p[2] = temporary;
        area = -area;
    }
    minx = maxx = p[0].x;
    miny = maxy = p[0].y;
    for (unsigned i = 1; i < 3; ++i) {
        if (p[i].x < minx) minx = p[i].x;
        if (p[i].x > maxx) maxx = p[i].x;
        if (p[i].y < miny) miny = p[i].y;
        if (p[i].y > maxy) maxy = p[i].y;
    }
    xmin = (int)ceil(minx / 256.0 - 0.5);
    xmax = (int)floor(maxx / 256.0 - 0.5);
    ymin = (int)ceil(miny / 256.0 - 0.5);
    ymax = (int)floor(maxy / 256.0 - 0.5);
    if (xmin < 0) xmin = 0;
    if (ymin < 0) ymin = 0;
    if (xmax >= (int)target->width) xmax = (int)target->width - 1;
    if (ymax >= (int)target->height) ymax = (int)target->height - 1;
    if (xmin > xmax || ymin > ymax) return PX_RASTER_OK;
    candidates = (uint64_t)(xmax - xmin + 1) * (uint64_t)(ymax - ymin + 1);
    if (target->candidate_pixels > PX_RASTER_MAX_CANDIDATES ||
        candidates > PX_RASTER_MAX_CANDIDATES - target->candidate_pixels)
        return PX_RASTER_LIMIT_EXCEEDED;
    target->candidate_pixels += candidates;
    inclusive[0] = top_left(p + 1, p + 2);
    inclusive[1] = top_left(p + 2, p);
    inclusive[2] = top_left(p, p + 1);
    for (int y = ymin; y <= ymax; ++y) {
        for (int x = xmin; x <= xmax; ++x) {
            int64_t e[3];
            double w[3], z, u, v, color[4];
            size_t pixel = (size_t)y * target->width + (unsigned)x;
            int64_t sx = (int64_t)x * 256 + 128, sy = (int64_t)y * 256 + 128;
            e[0] = edge(p + 1, p + 2, sx, sy);
            e[1] = edge(p + 2, p, sx, sy);
            e[2] = edge(p, p + 1, sx, sy);
            if (e[0] < 0 || (!e[0] && !inclusive[0]) ||
                e[1] < 0 || (!e[1] && !inclusive[1]) ||
                e[2] < 0 || (!e[2] && !inclusive[2])) continue;
            for (unsigned i = 0; i < 3; ++i) w[i] = (double)e[i] / (double)area;
            /* Difference form preserves constant attributes exactly, including
             * integer UV seams and equal-depth surfaces. */
            z = p[0].attr.z + w[1] * (p[1].attr.z - p[0].attr.z) +
                w[2] * (p[2].attr.z - p[0].attr.z);
            u = p[0].attr.u + w[1] * (p[1].attr.u - p[0].attr.u) +
                w[2] * (p[2].attr.u - p[0].attr.u);
            v = p[0].attr.v + w[1] * (p[1].attr.v - p[0].attr.v) +
                w[2] * (p[2].attr.v - p[0].attr.v);
            if (state->depth_test && !compare(z, target->depth[pixel], state->depth_compare)) continue;
            if (shader) {
                double interpolated[4];
                for (unsigned c = 0; c < 4; ++c)
                    interpolated[c] = clamp(p[0].attr.rgba[c] +
                        w[1] * (p[1].attr.rgba[c] - p[0].attr.rgba[c]) +
                        w[2] * (p[2].attr.rgba[c] - p[0].attr.rgba[c]));
                shader(context, u, v, interpolated, color);
                for (unsigned c = 0; c < 4; ++c) color[c] = clamp(color[c]);
            } else {
                sample(image, sampler, u, v, color);
                for (unsigned c = 0; c < 4; ++c) {
                    double modulation = p[0].attr.rgba[c] +
                        w[1] * (p[1].attr.rgba[c] - p[0].attr.rgba[c]) +
                        w[2] * (p[2].attr.rgba[c] - p[0].attr.rgba[c]);
                    color[c] = clamp(color[c] * modulation);
                }
            }
            if (!compare(byte(color[3]), state->alpha_reference, state->alpha_compare)) continue;
            blend(target->rgba + pixel * 4, color, state);
            if (state->depth_write) target->depth[pixel] = z;
        }
    }
    return PX_RASTER_OK;
}

enum px_raster_result px_raster_draw(struct px_raster_target *target,
    const struct px_raster_vertex vertices[3], const struct px_raster_image *image,
    const struct px_raster_sampler *sampler, const struct px_raster_state *state)
{
    return draw(target, vertices, image, sampler, NULL, NULL, state);
}

enum px_raster_result px_raster_draw_shaded(struct px_raster_target *target,
    const struct px_raster_vertex vertices[3], px_raster_shader shader, void *context,
    const struct px_raster_state *state)
{
    if (!shader) return PX_RASTER_INVALID_ARGUMENT;
    return draw(target, vertices, NULL, NULL, shader, context, state);
}

const char *px_raster_result_string(enum px_raster_result result)
{
    switch (result) {
    case PX_RASTER_OK: return "ok";
    case PX_RASTER_INVALID_ARGUMENT: return "invalid raster argument";
    case PX_RASTER_LIMIT_EXCEEDED: return "raster limit exceeded";
    case PX_RASTER_NO_MEMORY: return "out of memory";
    }
    return "unknown raster result";
}
