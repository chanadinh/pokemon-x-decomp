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
static void local(const struct px_bch_bone *bone, double m[12])
{
    const double cx = cos(bone->rotation[0]), sx = sin(bone->rotation[0]);
    const double cy = cos(bone->rotation[1]), sy = sin(bone->rotation[1]);
    const double cz = cos(bone->rotation[2]), sz = sin(bone->rotation[2]);
    /* Column-vector form of S * Rx * Ry * Rz * T (row vectors): T Rz Ry Rx S. */
    const double rx[12] = {1, 0, 0, 0,  0, cx, -sx, 0,  0, sx, cx, 0};
    const double ry[12] = {cy, 0, sy, 0,  0, 1, 0, 0,  -sy, 0, cy, 0};
    const double rz[12] = {cz, -sz, 0, 0,  sz, cz, 0, 0,  0, 0, 1, 0};
    const double s[12] = {bone->scale[0], 0, 0, 0,  0, bone->scale[1], 0, 0,  0, 0, bone->scale[2], 0};
    double t[12];
    multiply(rz, ry, t);
    multiply(t, rx, t);
    multiply(t, s, t);
    for (unsigned r = 0; r < 3; ++r) t[r * 4 + 3] = bone->translation[r];
    memcpy(m, t, sizeof t);
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
