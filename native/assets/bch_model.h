#ifndef PX_BCH_MODEL_H
#define PX_BCH_MODEL_H

#include <stddef.h>
#include <stdint.h>

enum px_bch_model_result {
    PX_BCH_MODEL_OK = 0,
    PX_BCH_MODEL_INVALID_ARGUMENT,
    PX_BCH_MODEL_TRUNCATED,
    PX_BCH_MODEL_INVALID_HEADER,
    PX_BCH_MODEL_UNSUPPORTED,
    PX_BCH_MODEL_LIMIT_EXCEEDED,
    PX_BCH_MODEL_NO_MEMORY
};

/* Names preserve the original bytes, including non-UTF8 bytes. A terminating
 * zero is supplied for convenience; size excludes it. Nullable names use
 * present=0, whereas an empty, relocated name has present=1 and size=0. */
struct px_bch_name { char bytes[1025]; uint32_t size, present; };
/* One observed PICA register: value bits are meaningful only where the 32-bit
 * known_mask is set. Observations come from one stream; nothing is inherited. */
struct px_bch_register { uint32_t value, known_mask; };
/* Coordinator layout follows the pinned CTRMap H3DMaterial reader: packed
 * word (mapping bits 16..23, reference camera bits 24..31), scale U/V,
 * rotation and translation U/V. Interpretations are not executable evidence. */
struct px_bch_coordinator { uint32_t packed; float scale[2], rotation, translation[2]; };
/* Register observations from the material's fragment command stream. */
struct px_bch_fragment_state {
    struct px_bch_register cull;            /* 0x040 */
    struct px_bch_register color_operation; /* 0x100 */
    struct px_bch_register blend;           /* 0x101 */
    struct px_bch_register alpha_test;      /* 0x104 */
    struct px_bch_register depth_color;     /* 0x107 */
    struct px_bch_register tev_buffer;      /* 0x0e0 */
    struct px_bch_register buffer_color;    /* 0x0fd */
    /* Stages 0..3 at 0xc0+8n, stages 4..5 at 0xf0/0xf8; each holds the
     * source, operand, combine, constant and scale registers in order. */
    struct px_bch_register tev[6][5];
};
struct px_bch_material {
    struct px_bch_name name, texture_names[3];
    uint32_t texture_unit_config;
    /* Texture unit parameter registers 0x83/0x93/0x9b from the material's
     * texture command stream; zero known_mask when that stream omits one. */
    struct px_bch_register unit_parameters[3];
    /* Nonzero only when the relocated parameter object spans 0xd0 bytes and
     * its +0xc8 field is a command-section relocation (tag 2). Then the three
     * coordinators at +12+24*unit and the fragment stream are returned. */
    uint32_t parameters_present;
    struct px_bch_coordinator coordinators[3];
    struct px_bch_fragment_state fragment;
};
struct px_bch_submesh {
    uint32_t first_index, index_count, index_width;
    /* Final observed PRIMITIVE_CONFIG writes, with a bit-level known mask.
     * These are stream observations, not the topology at DRAW_ELEMENTS.
     * In particular, a mask-8 cache clear does not establish bits 8..9. */
    uint32_t primitive_value, primitive_known_mask, primitive_write_count;
    /* State observed at the sole DRAW_ELEMENTS write, when present. Unknown
     * bits stay unknown: no state is inherited from other command streams. */
    uint32_t draw_primitive_value, draw_primitive_known_mask, draw_count;
    uint32_t draw_source_offset, index_source_offset;
};
struct px_bch_mesh {
    struct px_bch_name name;
    uint32_t first_vertex, vertex_count, first_index, index_count;
    uint32_t first_submesh, submesh_count, material_index, node_index;
    uint32_t visible, layer, priority;
    /* Vertex color input (semantic 3): 0 absent (colors hold 1,1,1,1),
     * 1 stored attribute after its uniform scale, 2 fixed attribute decoded
     * from float24 registers 0x232..0x235 (x,y,z,w = R,G,B,A), unscaled. */
    uint32_t color_source;
    float bounds_min[3], bounds_max[3];
};
struct px_bch_model {
    struct px_bch_name name;
    float *positions; /* Owned XYZ triples, scales and position offset applied. */
    float *uvs;       /* Owned UV0 pairs after scale; stored BCH convention. */
    float *colors;    /* Owned RGBA quads per vertex; see mesh color_source.
                       * Values are not clamped; the shader is not executed. */
    uint32_t *indices; /* Owned indices into the complete positions array. */
    struct px_bch_mesh *meshes;
    struct px_bch_submesh *submeshes;
    struct px_bch_material *materials;
    uint32_t vertex_count, index_count, mesh_count, submesh_count, material_count;
    float bounds_min[3], bounds_max[3];
};

/* Read-only extraction of a restricted static BCH 7/7 model. Requires an
 * identity model transform, no skinning, one interleaved vertex buffer, stored
 * position and UV0, and complete float32 geometry uniforms 6..8. Other stored
 * attributes are validated but not returned. Material names are associations;
 * shaders, render state and inherited GPU topology are not executed or inferred.
 * A submesh permits at most one full-mask DRAW_ELEMENTS value=1, and its index
 * pointer/count cannot change after that draw. Draw-free extraction is allowed.
 * No triangle count/divisibility assumption is made. A preview may explicitly
 * choose a triangle-list interpretation after checking each submesh count.
 * Caps: 64 MiB input; 1,024 meshes/materials/nodes; 65,535 resources;
 * 262,144 vertices and relocation entries; 786,432 indices; 65,536 words per
 * command stream and 262,144 total command words; absolute coordinates 1e12.
 * Attribute arithmetic uses double intermediates and rounds once to float.
 * Material parameter coordinators and fragment register observations are
 * metadata: this decoder does not execute combiners or select texture units.
 * Output owns every array and name; it outlives the unmodified input. Pass a
 * fresh or freed output. On any failure the output is zeroed. */
enum px_bch_model_result px_bch_model_decode(const uint8_t *data, size_t size,
    uint32_t model_index, struct px_bch_model *out);
void px_bch_model_free(struct px_bch_model *model);
const char *px_bch_model_result_string(enum px_bch_model_result result);

#endif
