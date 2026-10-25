/* Binding and running a model's vertex program: a hand-assembled DVLB inside
 * a buffer, a hand-built model. Checks the reference lookup, the uniform
 * order (constants, material, mesh, submesh, then the frame), the bone
 * palette (world x inverse bind for a smooth submesh, at UnivReg + 3k),
 * NormMtx as the inverse transpose, and output gathering by the output map. */
#include "vertex_program.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static uint8_t data[8192];
static void p32(size_t o, uint32_t v) { for (int k = 0; k < 4; ++k) data[o + k] = (uint8_t)(v >> (8 * k)); }
static void p16(size_t o, uint32_t v) { data[o] = (uint8_t)v; data[o + 1] = (uint8_t)(v >> 8); }
static uint32_t sw(const char *s) { uint32_t v = 0; for (int k = 0; k < 4; ++k) v = v << 2 | (uint32_t)(strchr("xyzw", s[k]) - "xyzw"); return v; }
static uint32_t desc(unsigned mask, const char *a, const char *b) { return mask | sw(a) << 5 | sw(b) << 14; }
static uint32_t f1(unsigned op, unsigned dst, unsigned s1, unsigned s2, unsigned idx, unsigned d)
{ return op << 26 | dst << 21 | idx << 19 | s1 << 12 | s2 << 7 | d; }
static uint32_t ifu(unsigned dst, unsigned num, unsigned b) { return 0x27u << 26 | b << 22 | dst << 10 | num; }

enum { BASE = 16 }; /* the DVLB's offset in the buffer */
enum { D_FULL, D_X, D_Y, D_Z, D_W, D_WS, D_XS, D_YS, D_WY, D_COUNT };
static size_t build(void)
{
    const uint32_t ops[D_COUNT] = {desc(15, "xyzw", "xyzw"), desc(8, "xyzw", "xyzw"), desc(4, "xyzw", "xyzw"),
                                   desc(2, "xyzw", "xyzw"), desc(1, "xyzw", "xyzw"), desc(15, "wwww", "xyzw"),
                                   desc(15, "xxxx", "xyzw"), desc(15, "yyyy", "xyzw"), desc(1, "yyyy", "xyzw")};
    const uint32_t program[] = {
        f1(0x08, 0x11, 0x20 + 93, 0x07, 0, D_WS),   /* r1 = c93.w (3) * v7 */
        f1(0x12, 0x00, 0x11, 0x00, 0, D_X),         /* MOVA a0.x = r1.x */
        f1(0x02, 0x10, 0x20 + 25, 0x00, 1, D_X),    /* r0.x = c[25 + a0.x] . v0 */
        f1(0x02, 0x10, 0x20 + 26, 0x00, 1, D_Y),
        f1(0x02, 0x10, 0x20 + 27, 0x00, 1, D_Z),
        f1(0x13, 0x10, 0x20 + 93, 0x00, 0, D_WY),   /* r0.w = c93.y (1) */
        f1(0x02, 0x00, 0x20 + 86, 0x10, 0, D_X),    /* o0 = ProjMtx . r0 */
        f1(0x02, 0x00, 0x20 + 87, 0x10, 0, D_Y),
        f1(0x02, 0x00, 0x20 + 88, 0x10, 0, D_Z),
        f1(0x02, 0x00, 0x20 + 89, 0x10, 0, D_W),
        f1(0x08, 0x04, 0x20 + 8, 0x04, 0, D_XS),    /* o4 = c8.x * v4 */
        ifu(13, 1, 1),                              /* if b1 */
        f1(0x13, 0x03, 0x20 + 93, 0x00, 0, D_YS),   /*   o3 = c93.y (1) */
        f1(0x13, 0x03, 0x20 + 93, 0x00, 0, D_XS),   /* else o3 = c93.x (0) */
        0x22u << 26};                               /* END */
    const unsigned n = sizeof program / sizeof *program;
    memset(data, 0, sizeof data);
    const size_t d = BASE;
    memcpy(data + d, "DVLB", 4); p32(d + 4, 1); p32(d + 8, 0x400);
    memcpy(data + d + 12, "DVLP", 4); p32(d + 20, 0x40); p32(d + 24, n); p32(d + 28, 0x40 + 4 * n); p32(d + 32, D_COUNT);
    for (unsigned k = 0; k < n; ++k) p32(d + 12 + 0x40 + 4 * k, program[k]);
    for (unsigned k = 0; k < D_COUNT; ++k) p32(d + 12 + 0x40 + 4 * n + 8 * k, ops[k]);
    const size_t e = d + 0x400;
    memcpy(data + e, "DVLE", 4); p32(e + 8, 0); p32(e + 12, n - 1);
    p32(e + 0x18, 0x40); p32(e + 0x1c, 1);
    p16(e + 0x40, 2); p16(e + 0x42, 93);
    p32(e + 0x44, 0); p32(e + 0x48, 0x3f0000); p32(e + 0x4c, 0x400000); p32(e + 0x50, 0x408000); /* 0, 1, 2, 3 */
    p32(e + 0x28, 0x60); p32(e + 0x2c, 3);
    p16(e + 0x60, 0); p16(e + 0x62, 0); p32(e + 0x64, 0xf);
    p16(e + 0x68, 2); p16(e + 0x6a, 3); p32(e + 0x6c, 0xf);
    p16(e + 0x70, 3); p16(e + 0x72, 4); p32(e + 0x74, 0x3);
    static const struct { const char *name; unsigned first, last; } uniforms[] = {
        {"WrldMtx", 16, 18}, {"NormMtx", 19, 21}, {"UnivReg", 41, 100}, {"ProjMtx", 102, 105},
        {"ViewMtx", 106, 108}, {"IsSmoSk", 121, 121}};
    p32(e + 0x30, 0x80); p32(e + 0x34, 6);
    size_t symbol = 0;
    for (unsigned k = 0; k < 6; ++k) {
        p32(e + 0x80 + 8 * k, (uint32_t)symbol); p16(e + 0x84 + 8 * k, uniforms[k].first); p16(e + 0x86 + 8 * k, uniforms[k].last);
        memcpy(data + e + 0x100 + symbol, uniforms[k].name, strlen(uniforms[k].name) + 1);
        symbol += strlen(uniforms[k].name) + 1;
    }
    p32(e + 0x38, 0x100); p32(e + 0x3c, (uint32_t)symbol);
    return e + 0x100 + symbol - d;
}

static void name(struct px_bch_name *n, const char *s)
{
    memset(n, 0, sizeof *n);
    n->present = 1; n->size = (uint32_t)strlen(s); memcpy(n->bytes, s, n->size);
}

int main(void)
{
    const size_t size = build();
    static struct px_bch_model m;
    static struct px_bch_material material;
    static struct px_bch_mesh mesh;
    static struct px_bch_submesh sub;
    static struct px_bch_shader shader;
    static struct px_bch_bone bones[2];
    static float inputs[48];
    name(&material.shader, "0@Test");
    name(&shader.name, "0@Test");
    shader.offset = BASE;
    shader.size = (uint32_t)size;
    material.uniforms.written[8] = 1; material.uniforms.c[8][0] = .5f;   /* overridden by the mesh */
    material.uniforms.bools_known = 2; material.uniforms.bools = 0;      /* overridden by the submesh */
    mesh.uniforms.written[8] = 1; mesh.uniforms.c[8][0] = .25f;
    mesh.submesh_count = 1; mesh.vertex_count = 1;
    sub.uniforms.bools_known = 2; sub.uniforms.bools = 2;
    sub.skinning = 1; sub.bone_count = 2; sub.bone_indices[0] = 0; sub.bone_indices[1] = 1;
    for (unsigned b = 0; b < 2; ++b)
        for (unsigned r = 0; r < 3; ++r) bones[b].inverse[r * 5] = 1;
    bones[1].inverse[3] = -1; /* inverse bind: translate by -1 in x */
    m.materials = &material; m.material_count = 1;
    m.meshes = &mesh; m.mesh_count = 1;
    m.submeshes = &sub; m.submesh_count = 1;
    m.shaders = &shader; m.shader_count = 1;
    m.bones = bones; m.bone_count = 2;
    m.inputs = inputs; m.vertex_count = 1;
    const float v0[4] = {1, 2, 3, 1}, v4[4] = {4, 8, 0, 1}, v7[4] = {1, 0, 0, 1};
    memcpy(inputs + 0, v0, sizeof v0); memcpy(inputs + 16, v4, sizeof v4); memcpy(inputs + 28, v7, sizeof v7);

    static struct px_vertex_program program;
    assert(px_vertex_program_load(data, sizeof data, &m, 0, &program));
    assert(program.entry_index == 0 && program.world == 0 && program.normal == 3 && program.palette == 25 &&
           program.palette_rows == 60 && program.projection == 86 && program.view == 90);
    const double world[24] = {1,0,0,0, 0,1,0,0, 0,0,1,0,   1,0,0,5, 0,1,0,0, 0,0,1,0};
    struct px_vertex_frame frame = {{2,0,0,0, 0,2,0,0, 0,0,2,0}, {1,0,0,0, 0,1,0,0, 0,0,1,0},
                                    {2,0,0,0, 0,2,0,0, 0,0,2,0, 0,0,0,1}, world};
    static struct px_pica_state prepared, work;
    assert(px_vertex_program_prepare(&program, &m, 0, 0, &frame, &prepared));
    assert(prepared.c[93][3] == 3 && prepared.c[8][0] == .25f && prepared.b[1] == 1);
    assert(prepared.c[0][0] == 2 && prepared.c[3][0] == .5f && prepared.c[4][1] == .5f && prepared.c[5][3] == 0);
    assert(prepared.c[28][3] == 4 && prepared.c[25][3] == 0); /* palette 1 = world x inverse: x + 4 */
    assert(px_vertex_program_run(&program, &m, 0, &prepared, &work) == PX_PICA_OK);
    float o[4] = {0}, t[4] = {0}, c[4] = {0};
    assert(px_vertex_program_output(&program, &work, PX_VERTEX_OUTPUT_POSITION, o) == 15);
    assert(o[0] == 10 && o[1] == 4 && o[2] == 6 && o[3] == 1);  /* (1 + 4, 2, 3) x 2 */
    assert(px_vertex_program_output(&program, &work, PX_VERTEX_OUTPUT_TEXCOORD0, t) == 3 && t[0] == 1 && t[1] == 2);
    assert(px_vertex_program_output(&program, &work, PX_VERTEX_OUTPUT_COLOR, c) == 15 && c[0] == 1);
    sub.uniforms.bools = 0;
    assert(px_vertex_program_prepare(&program, &m, 0, 0, &frame, &prepared));
    assert(px_vertex_program_run(&program, &m, 0, &prepared, &work) == PX_PICA_OK);
    assert(px_vertex_program_output(&program, &work, PX_VERTEX_OUTPUT_COLOR, c) == 15 && c[0] == 0);

    /* References that name nothing, or a missing entry, do not load. */
    name(&material.shader, "1@Test");
    assert(!px_vertex_program_load(data, sizeof data, &m, 0, &program));
    name(&material.shader, "Test");
    assert(!px_vertex_program_load(data, sizeof data, &m, 0, &program));
    name(&material.shader, "0@Other");
    assert(!px_vertex_program_load(data, sizeof data, &m, 0, &program));
    name(&material.shader, "0@Test");
    shader.size = (uint32_t)sizeof data; /* past the end of the buffer */
    assert(!px_vertex_program_load(data, sizeof data, &m, 0, &program));
    shader.size = (uint32_t)size;
    sub.bone_indices[1] = 2; /* a palette entry naming a missing bone */
    assert(px_vertex_program_load(data, sizeof data, &m, 0, &program));
    assert(!px_vertex_program_prepare(&program, &m, 0, 0, &frame, &prepared));
    printf("vertex program tests passed\n");
    return 0;
}
