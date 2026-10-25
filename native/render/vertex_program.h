#ifndef PX_VERTEX_PROGRAM_H
#define PX_VERTEX_PROGRAM_H

#include "../assets/bch_model.h"
#include "../platform/pica_shader.h"

/* A model's own vertex shader, run through the software PICA interpreter.
 *
 * Binding: a material's shader reference "E@Name" names the shader resource
 * of the same name (BCH index slot 2), a DVLB stored in the file, and its
 * entry point E.
 *
 * Uniform state for a draw, in the order the GPU would receive it: the
 * entry's own constants (as the shader loader uploads them), then every
 * uniform the material's, the mesh's and the submesh's command streams write
 * (float registers and the bool register, later streams overriding), then
 * the per-draw matrices the engine supplies, located by the uniform names
 * the program declares: WrldMtx (3 rows), NormMtx (3 rows), ViewMtx (3
 * rows), ProjMtx (4 rows) and UnivReg, the bone palette, three rows per
 * palette entry. The engine's values are not traced in the game: here the
 * palette is built as render/skin.h builds it (bone world matrix, times the
 * inverse bind matrix for smooth submeshes) and NormMtx is the inverse
 * transpose of WrldMtx's 3x3 part. Matrices are rows of four, as the
 * shader's DP4s read them. Inputs come from the model's raw input registers
 * (px_bch_model.inputs). */
struct px_vertex_program {
    struct px_dvlb dvlb;
    const struct px_pica_entry *entry;
    uint32_t resource, entry_index;
    int world, normal, view, projection, palette, palette_rows; /* first c register, -1 when not declared */
};
struct px_vertex_frame {
    float world[12], view[12], projection[16];
    const double *bones; /* skeleton world matrices, 12 per bone (render/skin.h) */
};
enum { PX_VERTEX_OUTPUT_POSITION = 0, PX_VERTEX_OUTPUT_QUATERNION = 1, PX_VERTEX_OUTPUT_COLOR = 2,
       PX_VERTEX_OUTPUT_TEXCOORD0 = 3, PX_VERTEX_OUTPUT_TEXCOORD0_W = 4, PX_VERTEX_OUTPUT_TEXCOORD1 = 5,
       PX_VERTEX_OUTPUT_TEXCOORD2 = 6, PX_VERTEX_OUTPUT_VIEW = 8 };

/* 1 on success; 0 when the material has no shader reference, it names no
 * resource, or the DVLB or entry is invalid. */
int px_vertex_program_load(const uint8_t *data, size_t size, const struct px_bch_model *model,
                           uint32_t material, struct px_vertex_program *out);
/* Uniform state for one submesh's draw. 0 on invalid input or a palette
 * entry naming a missing bone. */
int px_vertex_program_prepare(const struct px_vertex_program *program, const struct px_bch_model *model,
                              uint32_t mesh, uint32_t submesh, const struct px_vertex_frame *frame,
                              struct px_pica_state *state);
/* Runs one vertex (an index into the model's vertex arrays) from a prepared
 * state, leaving the result in *work. Returns a px_pica_result. */
int px_vertex_program_run(const struct px_vertex_program *program, const struct px_bch_model *model,
                          uint32_t vertex, const struct px_pica_state *prepared, struct px_pica_state *work);
/* Gathers one output semantic from the output registers by the entry's
 * output map (components the map does not supply are left as given).
 * Returns the mask of components found. */
unsigned px_vertex_program_output(const struct px_vertex_program *program, const struct px_pica_state *state,
                                  unsigned semantic, float out[4]);
#endif
