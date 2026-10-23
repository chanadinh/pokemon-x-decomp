#include "cgfx_model.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define SIZE 16384u
#define MAT 0x400u

static void w32(uint8_t *p, size_t offset, uint32_t value) {
    unsigned i;
    for (i = 0; i < 4; ++i) p[offset + i] = (uint8_t)(value >> (8 * i));
}
static void wf(uint8_t *p, size_t offset, float value) {
    uint32_t raw;
    memcpy(&raw, &value, 4); w32(p, offset, raw);
}
static void ptr(uint8_t *p, size_t field, size_t target) {
    w32(p, field, (uint32_t)(target - field));
}
static void dict(uint8_t *p, size_t where, size_t object, size_t key) {
    memcpy(p + where, "DICT", 4); w32(p, where + 4, 44); w32(p, where + 8, 1);
    ptr(p, where + 36, key); ptr(p, where + 40, object);
}
static void identity(uint8_t *p, size_t where) {
    unsigned i;
    for (i = 0; i < 12; ++i) wf(p, where + i * 4, i == 0 || i == 5 || i == 10 ? 1 : 0);
}
static void fixture(uint8_t *p) {
    const float coordinates[3][5] = {{1, 2, 3, 0, 0}, {4, 5, 6, 2, 0}, {7, 8, 9, 0, 4}};
    const uint8_t colors[3][4] = {{255, 128, 0, 64}, {0, 255, 128, 255}, {64, 0, 255, 0}};
    unsigned i, j;
    memset(p, 0, SIZE);
    memcpy(p, "CGFX\xff\xfe\x14\0", 8); w32(p, 8, 0x05000000);
    w32(p, 12, SIZE); w32(p, 16, 2);
    memcpy(p + 20, "DATA", 4); w32(p, 24, 0x3000 - 20);
    memcpy(p + 0x3000, "IMAG", 4); w32(p, 0x3004, 0x1000);
    w32(p, 28, 1); ptr(p, 32, 0x9c); dict(p, 0x9c, 0x100, 0xf0);
    memcpy(p + 0xf0, "fixture", 8);
    w32(p, 0x100, 0x40000012); memcpy(p + 0x104, "CMDL", 4);
    w32(p, 0x108, 0x09000000); ptr(p, 0x10c, 0xf0);
    for (i = 0; i < 3; ++i) wf(p, 0x130 + i * 4, 1);
    identity(p, 0x154); identity(p, 0x184);
    w32(p, 0x1b4, 1); ptr(p, 0x1b8, 0x200); ptr(p, 0x200, 0x1100);
    w32(p, 0x1bc, 1); ptr(p, 0x1c0, 0x300); dict(p, 0x300, MAT, 0xba0);
    w32(p, 0x1c4, 1); ptr(p, 0x1c8, 0x204); ptr(p, 0x204, 0x1000);
    w32(p, 0x1d4, 1); w32(p, 0x1dc, 2);
    w32(p, 0x1100, 0x01000000); memcpy(p + 0x1104, "SOBJ", 4);
    ptr(p, 0x1120, 0x100); p[0x1124] = 1; p[0x1125] = 7;
    w32(p, 0x1000, 0x10000001); memcpy(p + 0x1004, "SOBJ", 4);
    w32(p, 0x102c, 1); ptr(p, 0x1030, 0x1340); ptr(p, 0x1340, 0x1350);
    w32(p, 0x1038, 1); ptr(p, 0x103c, 0x1180); ptr(p, 0x1180, 0x1200);
    w32(p, 0x1200, 0x40000002); w32(p, 0x1204, 21); w32(p, 0x1208, 2);
    w32(p, 0x1214, 72); ptr(p, 0x1218, 0x3008); w32(p, 0x1224, 24);
    w32(p, 0x1228, 3); ptr(p, 0x122c, 0x1240);
    for (i = 0; i < 3; ++i) {
        size_t a = 0x1250 + i * 0x40;
        ptr(p, 0x1240 + i * 4, a); w32(p, a, 0x40000001);
        w32(p, a + 4, i == 0 ? 0 : i == 1 ? 4 : 3);
        w32(p, a + 36, i == 2 ? 0x1401 : 0x1406);
        w32(p, a + 40, i == 0 ? 3 : i == 1 ? 2 : 4);
        wf(p, a + 44, i == 0 ? 1 : i == 1 ? 0.5f : 1.0f / 255.0f);
        w32(p, a + 48, i == 0 ? 0 : i == 1 ? 12 : 20);
    }
    w32(p, 0x135c, 1); ptr(p, 0x1360, 0x1370); ptr(p, 0x1370, 0x1380);
    w32(p, 0x1380, 1); ptr(p, 0x1384, 0x13a0); ptr(p, 0x13a0, 0x13c0);
    w32(p, 0x13c0, 0x1401); p[0x13c5] = 1; w32(p, 0x13c8, 3); ptr(p, 0x13cc, 0x3080);
    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 5; ++j) wf(p, 0x3008 + i * 24 + j * 4, coordinates[i][j]);
        memcpy(p + 0x3008 + i * 24 + 20, colors[i], 4);
        p[0x3080 + i] = (uint8_t)i;
    }
    w32(p, MAT, 0x08000000); memcpy(p + MAT + 4, "MTOB", 4);
    w32(p, MAT + 8, 0x06000003); ptr(p, MAT + 12, 0xb80);
    memcpy(p + 0xb80, "material", 9); memcpy(p + 0xba0, "material-key", 13);
    w32(p, MAT + 0x18, 0x11); w32(p, MAT + 0x20, 2);
    w32(p, MAT + 0x108, 3); w32(p, MAT + 0x114, 0x00010040);
    w32(p, MAT + 0x118, 3); w32(p, MAT + 0x11c, 0x41);
    w32(p, MAT + 0x120, 0x00010107); w32(p, MAT + 0x124, 0x03000000);
    w32(p, MAT + 0x128, 0x00080126);
    w32(p, MAT + 0x12c, 1); w32(p, MAT + 0x140, 0x00e40100);
    w32(p, MAT + 0x144, 0x803f0100); w32(p, MAT + 0x148, 0x76760000);
    w32(p, MAT + 0x158, 0x40); w32(p, MAT + 0x15c, 0x000d0105);
    w32(p, MAT + 0x164, 0x000f0106);
    w32(p, MAT + 0x168, 1); w32(p, MAT + 0x174, 0xffffffff);
    wf(p, MAT + 0x17c, 1); wf(p, MAT + 0x180, 1); identity(p, MAT + 0x194);
    ptr(p, MAT + 0x274, 0x800); ptr(p, MAT + 0x288, 0xa40);
    w32(p, 0x800, 0x80000000); ptr(p, 0x808, 0x900); ptr(p, 0x80c, 0xa00);
    w32(p, 0x814, 0x0001008e); w32(p, 0x81c, 0x809f0081);
    w32(p, 0x824, 0x01003200); w32(p, 0x848, 56);
    w32(p, 0x900, 0x20000004); memcpy(p + 0x904, "TXOB", 4);
    w32(p, 0x908, 0x05000000); ptr(p, 0x90c, 0xb90); memcpy(p + 0xb90, "reference", 10);
    ptr(p, 0x918, 0xcd0);
    w32(p, 0xa00, 0x80000000); ptr(p, 0xa04, 0x800); w32(p, 0xa08, 3);
    w32(p, 0xb14, 0x8061); w32(p, 0xb18, 0x000f0104);
    w32(p, 36, 1); ptr(p, 40, 0xc00); dict(p, 0xc00, 0xc40, 0xce0);
    w32(p, 0xc40, 0x20000011); memcpy(p + 0xc44, "TXOB", 4); w32(p, 0xc48, 0x05000000);
    ptr(p, 0xc4c, 0xcd0); memcpy(p + 0xcd0, "texture", 8); memcpy(p + 0xce0, "texture-key", 12);
    w32(p, 0xc58, 8); w32(p, 0xc5c, 8); w32(p, 0xc68, 1);
    ptr(p, 0xc78, 0xc7c); w32(p, 0xc7c, 8); w32(p, 0xc80, 8);
    w32(p, 0xc84, 256); ptr(p, 0xc88, 0x3100); w32(p, 0xc90, 32);
}

static void reject(uint8_t *data) {
    struct px_cgfx_scene scene, zero;
    uint8_t before[SIZE];
    memcpy(before, data, SIZE); memset(&scene, 0xa5, sizeof(scene)); memset(&zero, 0, sizeof(zero));
    assert(px_cgfx_model_decode_textured(data, SIZE, 0, &scene) != PX_CGFX_MODEL_OK);
    assert(!memcmp(&scene, &zero, sizeof(scene))); assert(!memcmp(data, before, SIZE));
    px_cgfx_scene_free(&scene);
}

int main(void) {
    uint8_t original[SIZE], data[SIZE];
    struct px_cgfx_scene s;
    struct px_cgfx_model wire;
    const float uv[6] = {0, 0, 1, 0, 0, 2};
    const struct {size_t offset; uint32_t value;} mutations[] = {
        {0x1bc, 129}, {0x1c0, 0}, {0x304, 43}, {MAT + 8, 0},
        {MAT + 0x1c, 1}, {MAT + 0x104, 1}, {MAT + 0x108, 2}, {MAT + 0x110, 2},
        {MAT + 0x118, 8}, {MAT + 0x11c, 0x40}, {MAT + 0x120, 0x00000107},
        {MAT + 0x12c, 2}, {MAT + 0x144, 0x87ff0100}, {MAT + 0x148, 0x76760001},
        {MAT + 0x148, 0x76160000}, {MAT + 0x158, 0x41},
        {MAT + 0x168, 2}, {MAT + 0x16c, 1}, {MAT + 0x170, 1},
        {MAT + 0x17c, 0}, {MAT + 0x184, 0x7fc00000}, {MAT + 0x194, 0},
        {MAT + 0x278, 4}, {MAT + 0x288, 0xffffffff},
        {0x800, 0}, {0x808, 0x7fffffff}, {0x810, 1}, {0x824, 0x01001200}, {0x824, 0x01003202},
        {0x828, 1}, {0x848, 52}, {0x900, 0x20000011}, {0x918, 0}, {0xa04, 4},
        {0xa1c, 0x7f800000}, {0xb18, 0x00000104},
        {0xc4c, 0x7fffffff}, {0x1294, 5}, {0x12bc, 0x7fc00000},
        {0x3014, 0x7fc00000}, {0x12fc, 0x3f800000}, {0x1128, 1}
    };
    size_t i;
    fixture(original);
    assert(px_cgfx_model_decode_textured(original, SIZE, 0, &s) == PX_CGFX_MODEL_OK);
    assert(s.mesh.vertex_count == 3 && s.mesh.index_count == 3 && s.draw_count == 1 && s.material_count == 1);
    assert(!memcmp(s.uvs, uv, sizeof(uv)));
    assert(s.colors[0] == 1 && fabsf(s.colors[1] - 0.501960814f) < 0.0000001f);
    assert(s.colors[2] == 0 && fabsf(s.colors[3] - 0.250980407f) < 0.0000001f);
    assert(!strcmp(s.materials[0].name, "material") && s.materials[0].texture_index == 0);
    assert(s.materials[0].wrap_u == 3 && s.materials[0].wrap_v == 2 && s.materials[0].mip_filter == 1);
    assert(s.materials[0].min_filter == 0 && s.materials[0].mag_filter == 0);
    assert(s.materials[0].alpha_test == 1 && s.materials[0].alpha_compare == 6 && s.materials[0].alpha_reference == 128);
    assert(s.materials[0].depth_test == 1 && s.materials[0].depth_write == 1 && s.materials[0].depth_compare == 4);
    assert(s.materials[0].color_write_mask == 15 && !s.materials[0].color_write_mask_declared);
    assert(s.materials[0].cull_mode == 3 && s.materials[0].blend_mode == 1);
    assert(s.materials[0].blend_src_rgb == 6 && s.materials[0].blend_dst_rgb == 7 &&
           s.materials[0].blend_src_alpha == 6 && s.materials[0].blend_dst_alpha == 7);
    assert(s.draws[0].first_index == 0 && s.draws[0].index_count == 3 && s.draws[0].visible == 1);
    assert(s.draws[0].render_priority == 7 && s.draws[0].layer_id == 2);
    px_cgfx_scene_free(&s); px_cgfx_scene_free(&s);
    for (i = 0; i < sizeof(mutations) / sizeof(mutations[0]); ++i) {
        memcpy(data, original, SIZE); w32(data, mutations[i].offset, mutations[i].value); reject(data);
    }
    memcpy(data, original, SIZE); data[0x13c5] = 0; reject(data);
    /* Old geometry decoder still accepts unknown preview material states. */
    memcpy(data, original, SIZE); w32(data, MAT + 0x184, 0x3f800000); reject(data);
    assert(px_cgfx_model_decode(data, SIZE, 0, &wire) == PX_CGFX_MODEL_OK); px_cgfx_model_free(&wire);
    /* A full register-byte write declares RGBA masks; incomplete channel masks reject. */
    memcpy(data, original, SIZE); w32(data, MAT + 0x120, 0x00030107); w32(data, MAT + 0x11c, 0x0f41);
    assert(px_cgfx_model_decode_textured(data, SIZE, 0, &s) == PX_CGFX_MODEL_OK);
    assert(s.materials[0].color_write_mask_declared == 1 && s.materials[0].color_write_mask == 15);
    px_cgfx_scene_free(&s); w32(data, MAT + 0x11c, 0x0e41); reject(data);
    /* Fixed color values are already final and do not use the zero scale field. */
    memcpy(data, original, SIZE); w32(data, 0x1228, 2); w32(data, 0x1038, 2); ptr(data, 0x1184, 0x1400);
    w32(data, 0x1400, 0x80000000); w32(data, 0x1404, 3); w32(data, 0x1408, 1);
    w32(data, 0x140c, 0x1406); w32(data, 0x1410, 4); w32(data, 0x1418, 4); ptr(data, 0x141c, 0x1420);
    wf(data, 0x1420, 1); wf(data, 0x1424, 0.25f); wf(data, 0x1428, 0.5f); wf(data, 0x142c, 0.75f);
    assert(px_cgfx_model_decode_textured(data, SIZE, 0, &s) == PX_CGFX_MODEL_OK);
    for (i = 0; i < 3; ++i) assert(s.colors[i * 4] == 1 && s.colors[i * 4 + 1] == 0.25f &&
                                  s.colors[i * 4 + 2] == 0.5f && s.colors[i * 4 + 3] == 0.75f);
    px_cgfx_scene_free(&s);
    /* Object-node visibility is a separately referenced dictionary record. */
    memcpy(data, original, SIZE); w32(data, 0x1cc, 1); ptr(data, 0x1d0, 0x1500);
    dict(data, 0x1500, 0x1540, 0xf0); ptr(data, 0x1540, 0xf0);
    assert(px_cgfx_model_decode_textured(data, SIZE, 0, &s) == PX_CGFX_MODEL_OK);
    assert(!s.draws[0].visible); px_cgfx_scene_free(&s);
    w32(data, 0x1544, 2); reject(data);
    assert(px_cgfx_model_decode_textured(NULL, SIZE, 0, &s) == PX_CGFX_MODEL_INVALID_ARGUMENT);
    assert(px_cgfx_model_decode_textured(original, SIZE, 1, &s) == PX_CGFX_MODEL_INVALID_ARGUMENT);
    puts("CGFX textured scene: UV/color vectors, named material binding, command masks/state, visibility and rejection passed");
    return 0;
}
