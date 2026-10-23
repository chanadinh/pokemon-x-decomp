#ifndef PX_CGFX_MODEL_H
#define PX_CGFX_MODEL_H

#include <stddef.h>
#include <stdint.h>

enum px_cgfx_model_result {
    PX_CGFX_MODEL_OK = 0,
    PX_CGFX_MODEL_INVALID_ARGUMENT,
    PX_CGFX_MODEL_TRUNCATED,
    PX_CGFX_MODEL_INVALID_HEADER,
    PX_CGFX_MODEL_UNSUPPORTED,
    PX_CGFX_MODEL_LIMIT_EXCEEDED,
    PX_CGFX_MODEL_NO_MEMORY
};

struct px_cgfx_model {
    uint32_t vertex_count;
    uint32_t index_count;
    uint32_t shape_count;
    float *positions; /* Owned XYZ triples, shape offsets/scales applied. */
    uint32_t *indices; /* Owned triangle-list indices into positions. */
    float bounds_min[3];
    float bounds_max[3];
    char name[1025];
};

/* Restricted static CGFX mesh extraction for a geometry preview. Supports
 * ordinary SOBJ shapes, one interleaved buffer plus fixed nonposition
 * attributes, triangle lists, no bones,
 * no blend shapes, and identity model transforms. Declared unused attributes
 * are validated but not returned. All shapes are included regardless of
 * visibility; materials, animation, and renderer state are not reproduced.
 * Input cap 64 MiB; output caps 1,048,576 vertices and 3,145,728 indices;
 * absolute output coordinates are capped at 1e12 for preview arithmetic.
 * Pass a fresh or freed output object. Failure leaves it zeroed. */
enum px_cgfx_model_result px_cgfx_model_decode(const uint8_t *data, size_t size,
                                             uint32_t model_index,
                                             struct px_cgfx_model *out);
void px_cgfx_model_free(struct px_cgfx_model *model);
const char *px_cgfx_model_result_string(enum px_cgfx_model_result result);

#endif
