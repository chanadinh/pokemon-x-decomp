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

/* State values retain the primary source's enum numbering:
 * compare: never0,always1,equal2,notequal3,less4,lequal5,greater6,gequal7;
 * wrap: edge0,border1,repeat2,mirror3; filter: nearest0,linear1;
 * cull (CGFX): front0,back1,always2,never3;
 * blend factors (PICA): zero0,one1,source-alpha6,one-minus-source-alpha7.
 * The initial textured subset accepts only replace or add blending with the
 * factors above, UV0 with identity material transform, and no stencil or
 * polygon offset. GPU combiner/shader equivalence is not claimed. */
struct px_cgfx_material {
    char name[1025];
    uint32_t texture_index;
    uint32_t wrap_u, wrap_v, min_filter, mag_filter, mip_filter;
    uint32_t alpha_test, alpha_compare, alpha_reference;
    uint32_t depth_test, depth_write, depth_compare;
    uint32_t color_write_mask, color_write_mask_declared;
    uint32_t cull_mode, blend_mode;
    uint32_t blend_src_rgb, blend_dst_rgb, blend_src_alpha, blend_dst_alpha;
    uint32_t blend_equation_rgb, blend_equation_alpha;
    uint32_t translucency_kind;
    float uv_scale[2], uv_rotation, uv_translation[2];
    float uv_transform[12];
};

struct px_cgfx_draw {
    uint32_t first_index, index_count;
    uint32_t material_index, object_index, shape_index;
    uint32_t render_priority, visible, layer_id;
};

struct px_cgfx_scene {
    struct px_cgfx_model mesh;
    float *uvs;    /* Owned UV0 pairs after attribute scale; CGFX V convention. */
    float *colors; /* Owned normalized RGBA float quads; fixed colors supported. */
    struct px_cgfx_draw *draws; /* Resource object order; not game draw sorting. */
    struct px_cgfx_material *materials;
    uint32_t draw_count, material_count;
};

/* Additive preview API: preserves the wireframe decoder's semantics. Texture
 * indices refer to the same complete CGFX's px_cgfx texture index. Images are
 * decoded separately by the caller. A caller using top-down images converts
 * V to 1-V. Shader output is approximated by texture * vertex color; material
 * animation, lighting, and game draw ordering are outside this contract.
 * The output owns all arrays and does not borrow the input after return. */
enum px_cgfx_model_result px_cgfx_model_decode_textured(const uint8_t *data,
    size_t size, uint32_t model_index, struct px_cgfx_scene *out);
void px_cgfx_scene_free(struct px_cgfx_scene *scene);

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
