#ifndef PX_SKIN_H
#define PX_SKIN_H

#include "../assets/bch_model.h"
#include "../assets/gf_motion.h"

/* Skinning for models from px_bch_model_decode_skinned: rest pose, and
 * animated poses from GF1 motions (below).
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
 * palette[index.x]. None: palette[0]. Normals use the same 3x3 parts. */

/* world: 12 floats per bone. Returns 0 on invalid input. */
int px_skin_rest_world(const struct px_bch_bone *bones, uint32_t count, double *world);
/* Inverse of an affine 3x4 matrix; 0 if singular or non-finite. */
int px_skin_affine_inverse(const double in[12], double out[12]);

/* Pose one vertex of one submesh. world: from px_skin_rest_world or
 * px_skin_world_from_locals.
 * Returns 0 if the submesh's palette or an index the shader reads is out of
 * range. position/normal may alias their outputs. */
int px_skin_vertex(const struct px_bch_model *model, const struct px_bch_mesh *mesh,
                   const struct px_bch_submesh *submesh, const double *world,
                   uint32_t vertex, double position[3], double normal[3]);

/* Replace a skinned model's vertex arrays by rest-posed copies, one vertex
 * per index (a vertex shared by submeshes with different palettes poses
 * differently in each). Indices become 0..index_count-1; each mesh's vertex
 * range becomes its index range; normals are renormalized when nonzero;
 * bounds are recomputed. Bone arrays and shader inputs are released. Returns 0 (model left
 * unchanged) on invalid input, allocation failure or a pose failure. */
int px_skin_bake_rest(struct px_bch_model *model);

/* Animated pose. Local transforms are 9 floats per bone: translation x y z,
 * Euler rotation x y z, scale x y z, composed like the rest pose. */
int px_skin_world_from_locals(const struct px_bch_bone *bones, uint32_t count,
                              const float (*locals)[9], double *world);
/* Local transforms for one integer frame of a GF1 motion, following SPICA
 * bd29a78 GF1Motion.ToH3DSkeletalAnimation and SkeletalAnimation: each H3D bone
 * starts from its stored scale, rotation and translation; a motion bone of the
 * same name overrides the channels it has. For a motion bone flagged world
 * space (flags & 8) whose H3D bone has a parent, SPICA's conversion is applied:
 * the target translation is brought into the parent's frame (ancestor chain of
 * rotation and translation only, animated where the motion has channels),
 * the parent receives the rotation that turns its own translation toward that
 * target (Euler via SPICA's ToEuler) unless the motion animates its rotation,
 * and the bone's translation becomes the target with that rotation removed.
 * SPICA calls this conversion improvable; it is not the game's own solver.
 * Frames are clamped to 0..frames. Returns 0 on invalid input. */
int px_skin_motion_locals(const struct px_bch_model *model, const struct px_gf_motion_pack *pack,
                          const struct px_gf_motion *motion, uint32_t frame, float (*locals)[9]);
/* Re-parent bone child onto bone target for an animated pose: world[child] =
 * world[target] x inverse(rest[target]) x rest[child], so the child keeps its
 * rest offset from the target. rest: from px_skin_rest_world. Returns 0 on a
 * singular target or invalid input. */
int px_skin_attach(const double *rest, uint32_t count, uint32_t child, uint32_t target, double *world);
/* Pose every index (submesh by submesh) into positions/normals (3 floats per
 * index; normals renormalized) for the given bone world matrices. The model
 * keeps its skinned arrays. Returns 0 on a pose failure. */
int px_skin_pose_indices(const struct px_bch_model *model, const double *world,
                         float *positions, float *normals);

#endif
