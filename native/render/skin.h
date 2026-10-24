#ifndef PX_SKIN_H
#define PX_SKIN_H

#include "../assets/bch_model.h"

/* Rest-pose skinning for models from px_bch_model_decode_skinned.
 *
 * Bone local transform follows SPICA bd29a78 H3DBone.Transform:
 * scale, then rotation about X, Y and Z, then translation, applied to a point
 * in that order; a bone's world transform applies its local transform and
 * then its parent's. Matrices here are 3x4, rows of four, acting on column
 * vectors (x, y, z, 1) as the vertex shader's dot products with its matrix
 * rows do.
 *
 * Vertex posing follows SPICA's DefaultVertexShader translation and its
 * palette setup (Mesh.cs): palette entry k is the world matrix of skeleton
 * bone bone_indices[k], times the inverse bind matrix for smooth submeshes.
 * Smooth: position = sum of weight * palette[index] * position over x and y,
 * z when its weight is nonzero, and w when the stored weight attribute has
 * four elements and its weight is nonzero (indices clamped to 19). Rigid:
 * palette[index.x]. None: palette[0]. Normals use the same 3x3 parts.
 * Pose matrices other than the rest pose are not supported. */

/* world: 12 floats per bone. Returns 0 on invalid input. */
int px_skin_rest_world(const struct px_bch_bone *bones, uint32_t count, double *world);
/* Inverse of an affine 3x4 matrix; 0 if singular or non-finite. */
int px_skin_affine_inverse(const double in[12], double out[12]);

/* Pose one vertex of one submesh. world: from px_skin_rest_world.
 * Returns 0 if the submesh's palette or an index the shader reads is out of
 * range. position/normal may alias their outputs. */
int px_skin_vertex(const struct px_bch_model *model, const struct px_bch_mesh *mesh,
                   const struct px_bch_submesh *submesh, const double *world,
                   uint32_t vertex, double position[3], double normal[3]);

/* Replace a skinned model's vertex arrays by rest-posed copies, one vertex
 * per index (a vertex shared by submeshes with different palettes poses
 * differently in each). Indices become 0..index_count-1; each mesh's vertex
 * range becomes its index range; normals are renormalized when nonzero;
 * bounds are recomputed. Bone arrays are released. Returns 0 (model left
 * unchanged) on invalid input, allocation failure or a pose failure. */
int px_skin_bake_rest(struct px_bch_model *model);

#endif
