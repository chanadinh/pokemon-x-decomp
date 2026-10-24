#include "skin.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

/* c = a * b for 3x4 affine matrices (implicit last row 0 0 0 1). */
static void multiply(const double a[12], const double b[12], double c[12])
{
    double out[12];
    for (unsigned r = 0; r < 3; ++r)
        for (unsigned k = 0; k < 4; ++k) {
            double v = k == 3 ? a[r * 4 + 3] : 0;
            for (unsigned j = 0; j < 3; ++j) v += a[r * 4 + j] * b[j * 4 + k];
            out[r * 4 + k] = v;
        }
    memcpy(c, out, sizeof out);
}
static void local_srt(const float t[3], const float r[3], const float sc[3], double m[12])
{
    const double cx = cos(r[0]), sx = sin(r[0]);
    const double cy = cos(r[1]), sy = sin(r[1]);
    const double cz = cos(r[2]), sz = sin(r[2]);
    /* Column-vector form of S * Rx * Ry * Rz * T (row vectors): T Rz Ry Rx S. */
    const double rx[12] = {1, 0, 0, 0,  0, cx, -sx, 0,  0, sx, cx, 0};
    const double ry[12] = {cy, 0, sy, 0,  0, 1, 0, 0,  -sy, 0, cy, 0};
    const double rz[12] = {cz, -sz, 0, 0,  sz, cz, 0, 0,  0, 0, 1, 0};
    const double s[12] = {sc[0], 0, 0, 0,  0, sc[1], 0, 0,  0, 0, sc[2], 0};
    double out[12];
    multiply(rz, ry, out);
    multiply(out, rx, out);
    multiply(out, s, out);
    for (unsigned row = 0; row < 3; ++row) out[row * 4 + 3] = t[row];
    memcpy(m, out, sizeof out);
}
static void local(const struct px_bch_bone *bone, double m[12])
{
    local_srt(bone->translation, bone->rotation, bone->scale, m);
}
int px_skin_rest_world(const struct px_bch_bone *bones, uint32_t count, double *world)
{
    if ((count && (!bones || !world))) return 0;
    for (uint32_t i = 0; i < count; ++i) {
        double m[12];
        if (bones[i].parent < -1 || bones[i].parent >= (int32_t)i) return 0;
        local(bones + i, m);
        if (bones[i].parent >= 0) multiply(world + (size_t)bones[i].parent * 12, m, m);
        for (unsigned j = 0; j < 12; ++j) if (!isfinite(m[j])) return 0;
        memcpy(world + (size_t)i * 12, m, sizeof m);
    }
    return 1;
}
int px_skin_affine_inverse(const double in[12], double out[12])
{
    const double a = in[0], b = in[1], c = in[2], d = in[4], e = in[5], f = in[6];
    const double g = in[8], h = in[9], k = in[10];
    const double det = a * (e * k - f * h) - b * (d * k - f * g) + c * (d * h - e * g);
    if (!isfinite(det) || det == 0) return 0;
    double r[12] = {
        (e * k - f * h) / det, (c * h - b * k) / det, (b * f - c * e) / det, 0,
        (f * g - d * k) / det, (a * k - c * g) / det, (c * d - a * f) / det, 0,
        (d * h - e * g) / det, (b * g - a * h) / det, (a * e - b * d) / det, 0};
    for (unsigned row = 0; row < 3; ++row)
        r[row * 4 + 3] = -(r[row * 4] * in[3] + r[row * 4 + 1] * in[7] + r[row * 4 + 2] * in[11]);
    for (unsigned j = 0; j < 12; ++j) if (!isfinite(r[j])) return 0;
    memcpy(out, r, sizeof r);
    return 1;
}
static int palette(const struct px_bch_model *model, const struct px_bch_submesh *submesh,
                   const double *world, unsigned slot, double m[12])
{
    if (slot >= submesh->bone_count) return 0;
    const uint32_t bone = submesh->bone_indices[slot];
    if (bone >= model->bone_count) return 0;
    memcpy(m, world + (size_t)bone * 12, 12 * sizeof *m);
    if (submesh->skinning == 1) {
        double inverse[12];
        for (unsigned j = 0; j < 12; ++j) inverse[j] = model->bones[bone].inverse[j];
        multiply(m, inverse, m);
    }
    return 1;
}
static void apply(const double m[12], const double p[3], const double n[3], double w,
                  double out_p[3], double out_n[3])
{
    for (unsigned r = 0; r < 3; ++r) {
        out_p[r] += w * (m[r * 4] * p[0] + m[r * 4 + 1] * p[1] + m[r * 4 + 2] * p[2] + m[r * 4 + 3]);
        out_n[r] += w * (m[r * 4] * n[0] + m[r * 4 + 1] * n[1] + m[r * 4 + 2] * n[2]);
    }
}
int px_skin_vertex(const struct px_bch_model *model, const struct px_bch_mesh *mesh,
                   const struct px_bch_submesh *submesh, const double *world,
                   uint32_t vertex, double position[3], double normal[3])
{
    if (!model || !mesh || !submesh || !world || !model->bone_indices || !model->bone_weights ||
        vertex >= mesh->vertex_count || submesh->skinning > 2) return 0;
    const size_t at = (size_t)mesh->first_vertex + vertex;
    const double p[3] = {model->positions[at * 3], model->positions[at * 3 + 1], model->positions[at * 3 + 2]};
    const double n[3] = {model->normals[at * 3], model->normals[at * 3 + 1], model->normals[at * 3 + 2]};
    const uint8_t *index = model->bone_indices + at * 4;
    const float *weight = model->bone_weights + at * 4;
    double out_p[3] = {0, 0, 0}, out_n[3] = {0, 0, 0}, m[12];
    if (submesh->skinning == 1) {
        for (unsigned c = 0; c < 4; ++c) {
            if (c == 2 && weight[2] == 0) continue;
            if (c == 3 && (mesh->bone_weight_elements != 4 || weight[3] == 0)) continue;
            if (!palette(model, submesh, world, index[c] < 19 ? index[c] : 19, m)) return 0;
            apply(m, p, n, weight[c], out_p, out_n);
        }
    } else {
        const unsigned slot = submesh->skinning == 2 ? (index[0] < 19 ? index[0] : 19) : 0;
        if (!palette(model, submesh, world, slot, m)) return 0;
        apply(m, p, n, 1, out_p, out_n);
    }
    for (unsigned c = 0; c < 3; ++c) {
        if (!isfinite(out_p[c]) || !isfinite(out_n[c])) return 0;
        position[c] = out_p[c];
        normal[c] = out_n[c];
    }
    return 1;
}

int px_skin_bake_rest(struct px_bch_model *m)
{
    if (!m || !m->bone_indices || !m->bone_weights || !m->normals || !m->bones) return 0;
    const size_t n = m->index_count;
    double *world = calloc(m->bone_count ? m->bone_count : 1, 12 * sizeof *world);
    float *positions = malloc((n ? n : 1) * 3 * sizeof *positions);
    float *normals = malloc((n ? n : 1) * 3 * sizeof *normals);
    float *uvs = malloc((n ? n : 1) * 2 * sizeof *uvs);
    float *colors = malloc((n ? n : 1) * 4 * sizeof *colors);
    uint32_t *indices = malloc((n ? n : 1) * sizeof *indices);
    int ok = world && positions && normals && uvs && colors && indices &&
             px_skin_rest_world(m->bones, m->bone_count, world);
    for (uint32_t i = 0; ok && i < m->mesh_count; ++i) {
        const struct px_bch_mesh *mesh = m->meshes + i;
        for (uint32_t s = 0; ok && s < mesh->submesh_count; ++s) {
            const struct px_bch_submesh *sub = m->submeshes + mesh->first_submesh + s;
            for (uint32_t k = sub->first_index; ok && k < sub->first_index + sub->index_count; ++k) {
                const uint32_t source = m->indices[k];
                double p[3], q[3];
                ok = source >= mesh->first_vertex &&
                     px_skin_vertex(m, mesh, sub, world, source - mesh->first_vertex, p, q);
                if (!ok) break;
                const double length = sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2]);
                for (unsigned c = 0; c < 3; ++c) {
                    positions[(size_t)k * 3 + c] = (float)p[c];
                    normals[(size_t)k * 3 + c] = (float)(length > 0 ? q[c] / length : 0);
                }
                memcpy(uvs + (size_t)k * 2, m->uvs + (size_t)source * 2, 2 * sizeof *uvs);
                memcpy(colors + (size_t)k * 4, m->colors + (size_t)source * 4, 4 * sizeof *colors);
                indices[k] = k;
            }
        }
    }
    free(world);
    if (!ok) { free(positions); free(normals); free(uvs); free(colors); free(indices); return 0; }
    free(m->positions); free(m->normals); free(m->uvs); free(m->colors); free(m->indices);
    free(m->bone_indices); free(m->bone_weights);
    m->positions = positions; m->normals = normals; m->uvs = uvs; m->colors = colors; m->indices = indices;
    m->bone_indices = NULL; m->bone_weights = NULL;
    m->vertex_count = (uint32_t)n;
    for (uint32_t i = 0; i < m->mesh_count; ++i) {
        struct px_bch_mesh *mesh = m->meshes + i;
        mesh->first_vertex = mesh->first_index;
        mesh->vertex_count = mesh->index_count;
        for (uint32_t k = 0; k < mesh->index_count; ++k)
            for (unsigned c = 0; c < 3; ++c) {
                const float v = positions[((size_t)mesh->first_index + k) * 3 + c];
                if (!k || v < mesh->bounds_min[c]) mesh->bounds_min[c] = v;
                if (!k || v > mesh->bounds_max[c]) mesh->bounds_max[c] = v;
            }
        for (unsigned c = 0; c < 3; ++c) {
            if (!i || mesh->bounds_min[c] < m->bounds_min[c]) m->bounds_min[c] = mesh->bounds_min[c];
            if (!i || mesh->bounds_max[c] > m->bounds_max[c]) m->bounds_max[c] = mesh->bounds_max[c];
        }
    }
    return 1;
}

int px_skin_world_from_locals(const struct px_bch_bone *bones, uint32_t count,
                              const float (*locals)[9], double *world)
{
    if (count && (!bones || !locals || !world)) return 0;
    for (uint32_t i = 0; i < count; ++i) {
        double m[12];
        if (bones[i].parent < -1 || bones[i].parent >= (int32_t)i) return 0;
        local_srt(locals[i], locals[i] + 3, locals[i] + 6, m);
        if (bones[i].parent >= 0) multiply(world + (size_t)bones[i].parent * 12, m, m);
        for (unsigned j = 0; j < 12; ++j) if (!isfinite(m[j])) return 0;
        memcpy(world + (size_t)i * 12, m, sizeof m);
    }
    return 1;
}

static int name_equals(const struct px_bch_name *a, const char *b)
{
    return a->present && strlen(b) == a->size && !memcmp(a->bytes, b, a->size);
}
/* Rotation and translation only, column form T Rz Ry Rx (SPICA GetBoneRT
 * chain without scale). */
static void rt_matrix(const float *t, const float *r, double m[12])
{
    static const float one[3] = {1, 1, 1};
    local_srt(t, r, one, m);
}
static void quat_rotate(const double q[4], const double v[3], double out[3])
{
    /* v' = v + 2 q.xyz x (q.xyz x v + w v) */
    const double x = q[0], y = q[1], z = q[2], w = q[3];
    const double c1[3] = {y * v[2] - z * v[1] + w * v[0], z * v[0] - x * v[2] + w * v[1], x * v[1] - y * v[0] + w * v[2]};
    out[0] = v[0] + 2 * (y * c1[2] - z * c1[1]);
    out[1] = v[1] + 2 * (z * c1[0] - x * c1[2]);
    out[2] = v[2] + 2 * (x * c1[1] - y * c1[0]);
}
int px_skin_motion_locals(const struct px_bch_model *model, const struct px_gf_motion_pack *pack,
                          const struct px_gf_motion *motion, uint32_t frame, float (*locals)[9])
{
    if (!model || !pack || !motion || !locals || (model->bone_count && !model->bones)) return 0;
    if (frame > motion->frames) frame = motion->frames;
    /* motion bone for each H3D bone, or -1 */
    int32_t map[1024];
    if (model->bone_count > 1024) return 0;
    for (uint32_t i = 0; i < model->bone_count; ++i) {
        const struct px_bch_bone *b = &model->bones[i];
        for (unsigned j = 0; j < 3; ++j) {
            locals[i][j] = b->translation[j]; locals[i][3 + j] = b->rotation[j]; locals[i][6 + j] = b->scale[j];
        }
        map[i] = -1;
        for (uint32_t k = 0; k < pack->bone_count; ++k)
            if (name_equals(&b->name, pack->bones[k].name)) { map[i] = (int32_t)k; break; }
        if (map[i] < 0) continue;
        const struct px_gf_channel *ch = motion->channels + (size_t)map[i] * 9;
        /* Channel order TX TY TZ RX RY RZ SX SY SZ matches the local layout. */
        for (unsigned c = 0; c < 9; ++c) locals[i][c] = px_gf_channel_value(pack, ch + c, (float)frame, locals[i][c]);
        for (unsigned c = 0; c < 9; ++c) if (!isfinite(locals[i][c])) return 0;
    }
    /* World-space conversion, from the values before any conversion. */
    float original[1024][9];
    memcpy(original, locals, (size_t)model->bone_count * sizeof *original);
    for (uint32_t i = 0; i < model->bone_count; ++i) {
        if (map[i] < 0 || !(pack->bones[map[i]].flags & 8)) continue;
        const int32_t parent = model->bones[i].parent;
        if (parent < 0) return 0;
        /* World of the parent from the rotation/translation chain. */
        double chain[12] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0};
        for (int32_t b = parent; b >= 0; b = model->bones[b].parent) {
            double m[12];
            rt_matrix(original[b], original[b] + 3, m);
            multiply(m, chain, chain); /* ancestors apply after descendants */
        }
        double inverse[12];
        if (!px_skin_affine_inverse(chain, inverse)) return 0;
        const double target[3] = {original[i][0], original[i][1], original[i][2]};
        double bt[3];
        for (unsigned r = 0; r < 3; ++r)
            bt[r] = inverse[r * 4] * target[0] + inverse[r * 4 + 1] * target[1] + inverse[r * 4 + 2] * target[2] + inverse[r * 4 + 3];
        const double pt[3] = {original[parent][0], original[parent][1], original[parent][2]};
        /* SPICA CreateRotationBetweenVectors(PT, BT): (PT x BT, PT.BT + |PT||BT|), normalized. */
        double q[4] = {pt[1] * bt[2] - pt[2] * bt[1], pt[2] * bt[0] - pt[0] * bt[2], pt[0] * bt[1] - pt[1] * bt[0],
                       pt[0] * bt[0] + pt[1] * bt[1] + pt[2] * bt[2] +
                       sqrt((pt[0] * pt[0] + pt[1] * pt[1] + pt[2] * pt[2]) * (bt[0] * bt[0] + bt[1] * bt[1] + bt[2] * bt[2]))};
        const double length = sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
        if (!(length > 0)) return 0;
        for (unsigned j = 0; j < 4; ++j) q[j] /= length;
        const double qi[4] = {-q[0], -q[1], -q[2], q[3]};
        double local_t[3];
        quat_rotate(qi, bt, local_t);
        for (unsigned j = 0; j < 3; ++j) locals[i][j] = (float)local_t[j];
        const int32_t motion_parent = map[parent];
        const int parent_rotation_animated = motion_parent >= 0 &&
            (motion->channels[(size_t)motion_parent * 9 + 3].kind || motion->channels[(size_t)motion_parent * 9 + 4].kind ||
             motion->channels[(size_t)motion_parent * 9 + 5].kind);
        if (!parent_rotation_animated) {
            const double x = q[0], y = q[1], z = q[2], w = q[3];
            double s = 2 * (x * z - w * y);
            if (s > 1) s = 1;
            if (s < -1) s = -1;
            locals[parent][3] = (float)atan2(2 * (x * w + y * z), 1 - 2 * (x * x + y * y));
            locals[parent][4] = (float)-asin(s);
            locals[parent][5] = (float)atan2(2 * (x * y + z * w), 1 - 2 * (y * y + z * z));
        }
    }
    return 1;
}

int px_skin_pose_indices(const struct px_bch_model *m, const double *world, float *positions, float *normals)
{
    if (!m || !world || !positions || !normals || !m->bone_indices || !m->bone_weights) return 0;
    for (uint32_t i = 0; i < m->mesh_count; ++i) {
        const struct px_bch_mesh *mesh = m->meshes + i;
        for (uint32_t s = 0; s < mesh->submesh_count; ++s) {
            const struct px_bch_submesh *sub = m->submeshes + mesh->first_submesh + s;
            for (uint32_t k = sub->first_index; k < sub->first_index + sub->index_count; ++k) {
                const uint32_t source = m->indices[k];
                double p[3], q[3];
                if (source < mesh->first_vertex ||
                    !px_skin_vertex(m, mesh, sub, world, source - mesh->first_vertex, p, q)) return 0;
                const double length = sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2]);
                for (unsigned c = 0; c < 3; ++c) {
                    positions[(size_t)k * 3 + c] = (float)p[c];
                    normals[(size_t)k * 3 + c] = (float)(length > 0 ? q[c] / length : 0);
                }
            }
        }
    }
    return 1;
}

int px_skin_attach(const double *rest, uint32_t count, uint32_t child, uint32_t target, double *world)
{
    if (!rest || !world || child >= count || target >= count || child == target) return 0;
    double inverse[12], m[12];
    if (!px_skin_affine_inverse(rest + (size_t)target * 12, inverse)) return 0;
    multiply(world + (size_t)target * 12, inverse, m);
    multiply(m, rest + (size_t)child * 12, m);
    for (unsigned j = 0; j < 12; ++j) if (!isfinite(m[j])) return 0;
    memcpy(world + (size_t)child * 12, m, sizeof m);
    return 1;
}
