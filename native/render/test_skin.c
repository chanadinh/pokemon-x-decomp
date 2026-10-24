#include "skin.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Hand-built two-bone model. Bone 0: translation (0, 10, 0), rotation 90
 * degrees about Z. Bone 1 (child): translation (2, 0, 0), scale 2. Expected
 * values are worked by hand from S, Rx, Ry, Rz, T applied in that order. */
static void bones(struct px_bch_bone b[2])
{
    memset(b, 0, 2 * sizeof *b);
    b[0].parent = -1;
    b[0].scale[0] = b[0].scale[1] = b[0].scale[2] = 1;
    b[0].rotation[2] = (float)(M_PI / 2);
    b[0].translation[1] = 10;
    b[1].parent = 0;
    b[1].scale[0] = b[1].scale[1] = b[1].scale[2] = 2;
    b[1].translation[0] = 2;
}
static int near(double a, double b) { return fabs(a - b) < 1e-5; }

static void test_rest_world_and_inverse(void)
{
    struct px_bch_bone b[2];
    double w[24], inv[12], id[12];
    bones(b);
    assert(px_skin_rest_world(b, 2, w));
    /* Bone 0: x -> (0, 1, 0), y -> (-1, 0, 0), origin (0, 10, 0). */
    assert(near(w[0], 0) && near(w[1], -1) && near(w[4], 1) && near(w[5], 0) && near(w[10], 1));
    assert(near(w[3], 0) && near(w[7], 10) && near(w[11], 0));
    /* Bone 1 origin: parent maps (2, 0, 0) to (0, 12, 0); its x axis is
     * parent x scaled by 2: (0, 2, 0). */
    assert(near(w[12 + 3], 0) && near(w[12 + 7], 12) && near(w[12 + 4], 2));
    assert(px_skin_affine_inverse(w + 12, inv));
    for (unsigned r = 0; r < 3; ++r)
        for (unsigned k = 0; k < 4; ++k) {
            double v = k == 3 ? w[12 + r * 4 + 3] : 0;
            for (unsigned j = 0; j < 3; ++j) v += w[12 + r * 4 + j] * inv[j * 4 + k];
            id[r * 4 + k] = v;
        }
    for (unsigned j = 0; j < 12; ++j) assert(near(id[j], j % 5 == 0 ? 1 : 0));
    const double singular[12] = {0};
    assert(!px_skin_affine_inverse(singular, inv));
    b[1].parent = 1;
    assert(!px_skin_rest_world(b, 2, w)); /* parent must precede */
    b[1].parent = -2;
    assert(!px_skin_rest_world(b, 2, w));
}

/* One mesh of four vertices, three submeshes (none, rigid, smooth). */
struct fixture {
    struct px_bch_model m;
    struct px_bch_mesh mesh;
    struct px_bch_submesh sub[3];
    struct px_bch_bone b[2];
    float positions[12], normals[12], uvs[8], colors[16], weights[16];
    uint8_t indices4[16];
    uint32_t indices[9];
    double world[24];
};
static void fixture(struct fixture *f)
{
    memset(f, 0, sizeof *f);
    bones(f->b);
    for (unsigned v = 0; v < 4; ++v) {
        f->positions[v * 3] = (float)v; f->positions[v * 3 + 1] = 1; f->positions[v * 3 + 2] = 0;
        f->normals[v * 3] = 1;
        f->uvs[v * 2] = (float)v / 4;
        for (unsigned c = 0; c < 4; ++c) f->colors[v * 4 + c] = 1;
    }
    /* Vertex 0: index (1, 0), weights (1, 0). Vertex 1: (0, 1), (0.5, 0.5).
     * Vertex 2: four equal weights. Vertex 3: index 25 (clamped to 19).
     * Four weight elements are stored, so every w weight is read. */
    const uint8_t idx[16] = {1, 0, 0, 1,  0, 1, 0, 1,  0, 1, 0, 1,  25, 0, 0, 1};
    const float wt[16] = {1, 0, 0, 0,  .5f, .5f, 0, 0,  .25f, .25f, .25f, .25f,  1, 0, 0, 0};
    memcpy(f->indices4, idx, sizeof idx);
    memcpy(f->weights, wt, sizeof wt);
    for (unsigned k = 0; k < 9; ++k) f->indices[k] = k % 3;
    f->mesh.vertex_count = 4; f->mesh.index_count = 9; f->mesh.submesh_count = 3;
    f->mesh.bone_weight_elements = 4;
    for (unsigned s = 0; s < 3; ++s) {
        f->sub[s].first_index = 3 * s; f->sub[s].index_count = 3;
        f->sub[s].skinning = s == 0 ? 0 : s == 1 ? 2 : 1;
        f->sub[s].bone_count = 2; f->sub[s].bone_indices[0] = 0; f->sub[s].bone_indices[1] = 1;
    }
    f->m.positions = f->positions; f->m.normals = f->normals; f->m.uvs = f->uvs; f->m.colors = f->colors;
    f->m.bone_indices = f->indices4; f->m.bone_weights = f->weights;
    f->m.bones = f->b; f->m.bone_count = 2;
    f->m.meshes = &f->mesh; f->m.mesh_count = 1; f->m.submeshes = f->sub; f->m.submesh_count = 3;
    f->m.indices = f->indices; f->m.vertex_count = 4; f->m.index_count = 9;
    assert(px_skin_rest_world(f->b, 2, f->world));
    /* Inverse bind = inverse rest world: smooth skinning is then the identity. */
    for (unsigned b = 0; b < 2; ++b) {
        double inv[12];
        assert(px_skin_affine_inverse(f->world + 12 * b, inv));
        for (unsigned j = 0; j < 12; ++j) f->b[b].inverse[j] = (float)inv[j];
    }
}
static void test_vertex_modes(void)
{
    struct fixture f;
    double p[3], n[3];
    fixture(&f);
    /* None: palette[0] = bone 0. (1, 1, 0) -> (-1, 11, 0); normal x -> y. */
    assert(px_skin_vertex(&f.m, &f.mesh, &f.sub[0], f.world, 1, p, n));
    assert(near(p[0], -1) && near(p[1], 11) && near(p[2], 0) && near(n[1], 1));
    /* Rigid: palette[index.x]. Vertex 0 uses bone 1: (0,1,0) -> scale 2,
     * then parent: (0, 2, 0) + (2, 0, 0) -> rotate -> (-2, 2, 0) + (0, 10, 0). */
    assert(px_skin_vertex(&f.m, &f.mesh, &f.sub[1], f.world, 0, p, n));
    assert(near(p[0], -2) && near(p[1], 12) && near(n[1], 2)); /* normals keep scale */
    /* Smooth with inverse bind: identity whatever the weights (sum 1). */
    for (uint32_t v = 0; v < 3; ++v) {
        assert(px_skin_vertex(&f.m, &f.mesh, &f.sub[2], f.world, v, p, n));
        assert(near(p[0], v) && near(p[1], 1) && near(p[2], 0) && near(n[0], 1));
    }
    /* The fourth weight counts only with four stored weight elements. */
    f.mesh.bone_weight_elements = 3;
    assert(px_skin_vertex(&f.m, &f.mesh, &f.sub[2], f.world, 2, p, n));
    assert(near(p[0], 2 * 0.75) && near(p[1], 0.75));
    /* A zero third weight is skipped even when its index is out of range. */
    f.indices4[1 * 4 + 2] = 7;
    assert(px_skin_vertex(&f.m, &f.mesh, &f.sub[2], f.world, 1, p, n));
    /* Index 25 clamps to 19, beyond a two-entry palette: rejected. */
    assert(!px_skin_vertex(&f.m, &f.mesh, &f.sub[1], f.world, 3, p, n));
    f.sub[1].bone_indices[1] = 9; /* palette names a missing bone */
    assert(!px_skin_vertex(&f.m, &f.mesh, &f.sub[1], f.world, 0, p, n));
    assert(!px_skin_vertex(&f.m, &f.mesh, &f.sub[2], f.world, 4, p, n));
    f.sub[0].skinning = 3;
    assert(!px_skin_vertex(&f.m, &f.mesh, &f.sub[0], f.world, 0, p, n));
}
static void test_bake(void)
{
    struct fixture f;
    fixture(&f);
    /* Bake needs owned arrays: copy the fixture's into the heap. */
    struct px_bch_model m = f.m;
    struct px_bch_mesh mesh = f.mesh;
    struct px_bch_submesh sub[3];
    memcpy(sub, f.sub, sizeof sub);
    m.meshes = &mesh; m.submeshes = sub;
#define DUP(field, count) do { void *copy = malloc(sizeof *f.m.field * (count)); assert(copy); \
        memcpy(copy, f.m.field, sizeof *f.m.field * (count)); m.field = copy; } while (0)
    DUP(positions, 12); DUP(normals, 12); DUP(uvs, 8); DUP(colors, 16); DUP(indices, 9);
    DUP(bone_indices, 16); DUP(bone_weights, 16);
    assert(px_skin_bake_rest(&m));
    assert(m.vertex_count == 9 && m.bone_indices == NULL && m.bone_weights == NULL);
    for (uint32_t k = 0; k < 9; ++k) assert(m.indices[k] == k);
    /* Index 4 is submesh 1 (rigid), source vertex 1 with index.x 0: bone 0. */
    assert(near(m.positions[4 * 3], -1) && near(m.positions[4 * 3 + 1], 11));
    /* Index 3 is source vertex 0, rigid on bone 1; normal renormalized. */
    assert(near(m.positions[3 * 3], -2) && near(m.normals[3 * 3 + 1], 1));
    assert(near(m.uvs[5 * 2], .5f) && mesh.first_vertex == 0 && mesh.vertex_count == 9);
    assert(near(m.bounds_min[0], -2) && near(m.bounds_max[1], 12));
    free(m.positions); free(m.normals); free(m.uvs); free(m.colors); free(m.indices);
    /* A pose failure leaves the model unchanged. */
    fixture(&f);
    m = f.m; mesh = f.mesh; memcpy(sub, f.sub, sizeof sub);
    m.meshes = &mesh; m.submeshes = sub;
    sub[1].bone_count = 1; /* vertex 0 of the rigid submesh needs slot 1 */
    float *before = m.positions;
    assert(!px_skin_bake_rest(&m));
    assert(m.positions == before && m.bone_indices == f.indices4);
    assert(!px_skin_bake_rest(NULL));
}
int main(void)
{
    test_rest_world_and_inverse();
    test_vertex_modes();
    test_bake();
    puts("rest-pose skinning tests passed");
    return 0;
}
