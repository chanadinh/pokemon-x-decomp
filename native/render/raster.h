#ifndef PX_RASTER_H
#define PX_RASTER_H

#include <stddef.h>
#include <stdint.h>

#define PX_RASTER_MAX_SIDE 4096u
#define PX_RASTER_MAX_PIXELS 4194304u
#define PX_RASTER_MAX_CANDIDATES UINT64_C(67108864)

enum px_raster_result {
    PX_RASTER_OK = 0, PX_RASTER_INVALID_ARGUMENT, PX_RASTER_LIMIT_EXCEEDED,
    PX_RASTER_NO_MEMORY
};
enum px_raster_wrap { PX_RASTER_CLAMP = 0, PX_RASTER_REPEAT, PX_RASTER_MIRROR };
enum px_raster_filter { PX_RASTER_NEAREST = 0, PX_RASTER_LINEAR };
/* These enum values are renderer API values, not PICA register encodings. */
enum px_raster_compare {
    PX_RASTER_NEVER = 0, PX_RASTER_ALWAYS, PX_RASTER_LESS, PX_RASTER_LEQUAL,
    PX_RASTER_EQUAL, PX_RASTER_NOTEQUAL, PX_RASTER_GEQUAL, PX_RASTER_GREATER
};
enum px_raster_cull {
    PX_RASTER_CULL_NONE = 0, PX_RASTER_CULL_FRONT, PX_RASTER_CULL_BACK,
    PX_RASTER_CULL_BOTH
};
enum px_raster_winding { PX_RASTER_CW = 0, PX_RASTER_CCW };
enum px_raster_blend {
    PX_RASTER_REPLACE = 0,
    PX_RASTER_SOURCE_OVER, /* Straight RGBA Porter-Duff source-over. */
    PX_RASTER_SOURCE_ALPHA, /* Every channel: src*srcA + dst*(1-srcA). */
    PX_RASTER_ADD_SOURCE_ALPHA /* Every channel: src*srcA + dst, clamped to 1. */
};

struct px_raster_target {
    uint32_t width, height;
    uint8_t *rgba; /* Owned, tightly packed top-down straight RGBA8. */
    double *depth; /* Owned, width*height values. Lower z is closer. */
    uint64_t candidate_pixels; /* Work budget since the last clear. */
};
struct px_raster_image {
    uint32_t width, height;
    const uint8_t *rgba; /* Borrowed, tightly packed top-down RGBA8. */
    size_t rgba_size;   /* Must cover width*height*4 bytes. */
};
struct px_raster_vertex {
    double x, y, z, u, v;
    double rgba[4]; /* Finite normalized colors, clamped to 0..1 per vertex. */
};
struct px_raster_sampler {
    enum px_raster_wrap wrap_u, wrap_v;
    enum px_raster_filter filter;
};
struct px_raster_state {
    int depth_test, depth_write; /* Each must be 0 or 1; independently controlled. */
    enum px_raster_compare depth_compare, alpha_compare;
    uint8_t alpha_reference;
    enum px_raster_cull cull;
    enum px_raster_winding front;
    enum px_raster_blend blend;
    uint8_t color_write_mask; /* RGBA bits0..3; default15. */
};

/* Create a fresh/freed target; failures leave it zeroed. Clears are explicit.
 * Clear depth may be finite or +INFINITY. Call clear before the first draw.
 * Draw requires a successfully created target and caller-controlled draw order.
 * Free is safe on NULL/zero targets. */
enum px_raster_result px_raster_create(uint32_t width, uint32_t height,
                                       struct px_raster_target *out);
enum px_raster_result px_raster_clear(struct px_raster_target *target,
                                      const uint8_t rgba[4], double depth);
void px_raster_free(struct px_raster_target *target);
struct px_raster_state px_raster_default_state(void);

/* Orthographic affine interpolation. Pixel centers are (x+.5,y+.5), screen y
 * grows down, UV(0,0) is the decoded image's top left. A NULL image means white
 * texels and permits NULL sampler. Texture RGBA multiplies vertex RGBA.
 * Coverage snaps x/y to 1/256 pixel (nearest, ties away from zero), then uses
 * exact integer top-left edges. Positive area is clockwise in y-down space.
 * x/y must be finite and |value|<=1e6; z/u/v finite and |value|<=1e12.
 * Degenerate, culled, and offscreen triangles succeed without drawing.
 * Alpha compares the rounded source alpha byte before blend/depth write.
 * Sampling, modulation and blending retain floating precision until final
 * RGBA8 rounding; bilinear filtering operates on straight color channels.
 * Clear resets the 64M candidate-pixel budget. All argument/limit failures are
 * detected before any writes for this triangle, including budget consumption.
 * Sampling supports one mip only, without perspective or PICA shader state. */
enum px_raster_result px_raster_draw(struct px_raster_target *target,
    const struct px_raster_vertex vertices[3], const struct px_raster_image *image,
    const struct px_raster_sampler *sampler, const struct px_raster_state *state);
/* Same coverage, depth, alpha-test and blend rules as px_raster_draw, but
 * each covered fragment's straight RGBA comes from shader(context, u, v,
 * rgba, out): u/v and rgba are the interpolated vertex attributes (rgba
 * clamped to 0..1); out is clamped to 0..1 afterwards. The shader runs after
 * the depth test and before the alpha test, once per surviving fragment, in
 * row-major order. A NULL shader is invalid. */
typedef void (*px_raster_shader)(void *context, double u, double v,
                                 const double rgba[4], double out[4]);
enum px_raster_result px_raster_draw_shaded(struct px_raster_target *target,
    const struct px_raster_vertex vertices[3], px_raster_shader shader, void *context,
    const struct px_raster_state *state);
/* As px_raster_draw_shaded, plus four unclamped per-vertex varyings
 * (varyings[4*vertex+c], 12 values; finite,
 * |value|<=1e12) interpolated with the same affine weights and passed to the
 * shader. Callers renormalize interpolated directions themselves. */
typedef void (*px_raster_varying_shader)(void *context, double u, double v,
    const double rgba[4], const double varying[4], double out[4]);
enum px_raster_result px_raster_draw_varying(struct px_raster_target *target,
    const struct px_raster_vertex vertices[3], const double *varyings,
    px_raster_varying_shader shader, void *context, const struct px_raster_state *state);
/* Sampling used by px_raster_draw, exposed for shaders. Check the image and
 * sampler once with px_raster_image_valid; sampling an invalid image is
 * undefined. Returns normalized straight RGBA. */
int px_raster_image_valid(const struct px_raster_image *image,
                          const struct px_raster_sampler *sampler);
void px_raster_sample(const struct px_raster_image *image,
                      const struct px_raster_sampler *sampler, double u, double v,
                      double rgba[4]);
const char *px_raster_result_string(enum px_raster_result result);

#endif
