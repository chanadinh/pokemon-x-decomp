#include "bch_model.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Independently authored geometry, deliberately asymmetric in sign, component
 * order and UVs. The first relocated texture name and vertex address are zero
 * relative pointers; the two absent texture units are genuinely unrelocated. */
enum { CONTENT = 56, STRING = 2104, COMMAND = 2360, RAW = 3384, RELOC = 3640,
       MODEL = 248, MATERIAL = 400, MESH = 488, SUB = 544, PARAMS = 596,
       VISIBILITY = 600, MODEL_TREE = 604, PARAM_TREE = 628,
       MATERIAL_TREE = 652, NODE_TREE = 676, MODEL_TABLE = 700, PARAM_TABLE = 704 };
struct fixture {
    uint8_t data[4096];
    size_t size, cursor, enable, face, vertex_relocation, index_relocation;
};
static void word(uint8_t *p, uint32_t value)
{
    for (unsigned i = 0; i < 4; ++i) p[i] = (uint8_t)(value >> (8 * i));
}
static uint32_t float_bits(float value)
{
    uint32_t bits;
    memcpy(&bits, &value, sizeof bits);
    return bits;
}
static void pointer(struct fixture *f, size_t field, size_t target, unsigned tag)
{
    const size_t base = tag == 1 ? STRING : (tag == 2 || tag == 3) ? COMMAND : tag >= 0x24 ? RAW : CONTENT;
    const size_t source = tag >= 0x24 ? COMMAND : CONTENT;
    word(f->data + field, (uint32_t)(target - base));
    word(f->data + f->size, (uint32_t)(tag << 25) |
         (uint32_t)((field - source) / (tag == 1 ? 1 : 4)));
    f->size += 4;
}
static size_t packet(struct fixture *f, unsigned reg, unsigned mask,
                     int consecutive, unsigned count, const uint32_t *values)
{
    const size_t at = f->cursor;
    word(f->data + at, values[0]);
    word(f->data + at + 4, reg | mask << 16 | (count - 1) << 20 | (uint32_t)consecutive << 31);
    for (unsigned i = 1; i < count; ++i) word(f->data + at + 4 + i * 4, values[i]);
    f->cursor += ((size_t)(count + 1) * 4 + 7) & ~(size_t)7;
    return at;
}
static size_t single(struct fixture *f, unsigned reg, unsigned mask, uint32_t value)
{
    return packet(f, reg, mask, 0, 1, &value);
}
static void stream(struct fixture *f, size_t field, size_t start)
{
    pointer(f, field, start, 2);
    word(f->data + field + 4, (uint32_t)((f->cursor - start) / 4));
}
static void dictionary(struct fixture *f, unsigned category, size_t table,
                       size_t tree, size_t object, size_t name)
{
    const size_t at = CONTENT + category * 12;
    pointer(f, at, table, 0);
    word(f->data + at + 4, 1);
    pointer(f, at + 8, tree, 0);
    pointer(f, table, object, 0);
    pointer(f, tree + 20, name, 1);
}
static void finish(struct fixture *f)
{
    word(f->data + 44, (uint32_t)(f->size - RELOC));
}
static struct fixture fixture(unsigned width)
{
    struct fixture f = {{0}, RELOC, COMMAND, 0, 0, 0, 0};
    memcpy(f.data, "BCH\0\7\7", 6);
    const uint32_t starts[] = {CONTENT, STRING, COMMAND, RAW, RELOC};
    const uint32_t lengths[] = {2048, 256, 1024, 256};
    for (unsigned i = 0; i < 5; ++i) word(f.data + 8 + i * 4, starts[i]);
    for (unsigned i = 0; i < 4; ++i) word(f.data + 28 + i * 4, lengths[i]);
    memcpy(f.data + STRING, "pattern\0triangle\0surface\0node\xff\0", 30);
    dictionary(&f, 0, MODEL_TABLE, MODEL_TREE, MODEL, STRING + 8);
    dictionary(&f, 1, PARAM_TABLE, PARAM_TREE, PARAMS, STRING + 17);
    for (unsigned i = 0; i < 12; ++i) word(f.data + MODEL + 4 + i * 4,
                                          float_bits(i == 0 || i == 5 || i == 10 ? 1.f : 0.f));
    pointer(&f, MODEL + 0x34, MATERIAL, 0);
    pointer(&f, MODEL + 0x3c, MATERIAL_TREE, 0);
    pointer(&f, MODEL + 0x40, MESH, 0);
    pointer(&f, MODEL + 0x7c, VISIBILITY, 0);
    pointer(&f, MODEL + 0x84, STRING + 8, 1);
    pointer(&f, MODEL + 0x8c, NODE_TREE, 0);
    const unsigned counts[] = {0x38, 0x44, 0x80, 0x88};
    for (unsigned i = 0; i < 4; ++i) word(f.data + MODEL + counts[i], 1);
    pointer(&f, MATERIAL_TREE + 20, STRING + 17, 1);
    pointer(&f, NODE_TREE + 20, STRING + 25, 1);
    pointer(&f, MATERIAL, PARAMS, 0);
    pointer(&f, MATERIAL + 0x48, STRING, 1);
    pointer(&f, MATERIAL + 0x54, STRING + 17, 1);
    f.data[VISIBILITY] = 1;
    size_t start = f.cursor;
    single(&f, 0x80, 15, 1);
    stream(&f, MATERIAL + 0x10, start);
    start = f.cursor;
    single(&f, 0x203, 15, 0);
    stream(&f, MESH + 24, start);
    pointer(&f, MESH + 16, SUB, 0);
    word(f.data + MESH + 20, 1);
    f.enable = f.cursor;
    const uint32_t config[] = {0x03000000, 0x7b, 0x10000000, 0, 0x10, 0x20140000};
    packet(&f, 0x200, 15, 1, 6, config);
    const uint32_t permutation[] = {0x40, 0};
    packet(&f, 0x2bb, 15, 1, 2, permutation);
    single(&f, 0x242, 1, 1);
    const uint32_t offset[] = {0x80000006, 0, float_bits(30), float_bits(-20), float_bits(10)};
    packet(&f, 0x2c0, 15, 1, 5, offset);
    single(&f, 0x2c0, 15, 0x80000007);
    const uint32_t scales[] = {float_bits(1), float_bits(1), float_bits(1), float_bits(2),
                              float_bits(1), float_bits(1), float_bits(1), float_bits(.25f)};
    packet(&f, 0x2c1, 15, 0, 8, scales);
    stream(&f, MESH + 8, f.enable);
    f.vertex_relocation = f.size;
    pointer(&f, f.enable + 16, RAW, 0x26);
    f.face = f.cursor;
    single(&f, 0x227, 15, 0);
    single(&f, 0x228, 15, 3);
    single(&f, 0x25e, 8, 0);
    stream(&f, SUB + 44, f.face);
    f.index_relocation = f.size;
    pointer(&f, f.face, RAW + 60, width == 2 ? 0x27 : 0x28);
    const float positions_uv[] = {-2,3,5,.5f,-1, 7,-4,2,2,3, -1,9,-6,-2,4};
    for (unsigned i = 0; i < 15; ++i) word(f.data + RAW + i * 4, float_bits(positions_uv[i]));
    f.data[RAW + 60] = 2;
    f.data[RAW + 60 + 2 * width] = 1;
    finish(&f);
    return f;
}
static void reject(struct fixture f, size_t at, uint32_t value)
{
    struct px_bch_model model;
    memset(&model, 0xff, sizeof model);
    word(f.data + at, value);
    assert(px_bch_model_decode(f.data, f.size, 0, &model) != PX_BCH_MODEL_OK);
    const struct px_bch_model empty = {0};
    assert(memcmp(&model, &empty, sizeof model) == 0);
}
static void test_geometry_and_ownership(void)
{
    const float expected_positions[] = {6,-14,40, 24,-28,34, 8,-2,18};
    const float expected_uvs[] = {.125f,-.25f, .5f,.75f, -.5f,1};
    const uint32_t expected_indices[] = {2,0,1};
    for (unsigned width = 1; width <= 2; ++width) {
        struct fixture f = fixture(width), before = f;
        struct px_bch_model model;
        assert(px_bch_model_decode(f.data, f.size, 0, &model) == PX_BCH_MODEL_OK);
        assert(memcmp(&f, &before, sizeof f) == 0);
        memset(f.data, 0, sizeof f.data);
        assert(model.vertex_count == 3 && model.index_count == 3 && model.mesh_count == 1);
        assert(model.material_count == 1 && model.submesh_count == 1);
        assert(memcmp(model.positions, expected_positions, sizeof expected_positions) == 0);
        assert(memcmp(model.uvs, expected_uvs, sizeof expected_uvs) == 0);
        assert(memcmp(model.indices, expected_indices, sizeof expected_indices) == 0);
        assert(model.bounds_min[0] == 6 && model.bounds_min[1] == -28 && model.bounds_min[2] == 18);
        assert(model.bounds_max[0] == 24 && model.bounds_max[1] == -2 && model.bounds_max[2] == 40);
        assert(model.meshes[0].visible == 1 && model.meshes[0].name.size == 5);
        assert((uint8_t)model.meshes[0].name.bytes[4] == 255);
        assert(model.materials[0].texture_names[0].present == 1);
        assert(strcmp(model.materials[0].texture_names[0].bytes, "pattern") == 0);
        assert(!model.materials[0].texture_names[1].present && !model.materials[0].texture_names[2].present);
        assert(model.submeshes[0].index_width == width);
        assert(model.submeshes[0].primitive_known_mask == 0xff000000);
        assert(model.submeshes[0].draw_count == 0 && model.submeshes[0].draw_primitive_known_mask == 0);
        px_bch_model_free(&model);
        px_bch_model_free(&model);
        assert(model.positions == NULL && model.mesh_count == 0);
    }
}
static void test_integer_attributes_and_no_topology_guess(void)
{
    const int values[] = {-2,3,5,2,-4, 7,-4,2,8,12, -1,9,-6,-8,16};
    for (unsigned width = 1; width <= 2; ++width) {
        struct fixture f = fixture(1);
        word(f.data + f.enable + 8, width == 1 ? 0x48 : 0x6a);
        word(f.data + f.enable + 24, 0x20000000 | (5 * width) << 16);
        for (unsigned i = 0; i < 15; ++i) {
            f.data[RAW + i * width] = (uint8_t)values[i];
            if (width == 2) f.data[RAW + i * width + 1] = values[i] < 0 ? 255 : 0;
        }
        /* Four indices are valid extraction data. Triangles are caller policy. */
        word(f.data + f.face + 8, 4);
        struct px_bch_model model;
        assert(px_bch_model_decode(f.data, f.size, 0, &model) == PX_BCH_MODEL_OK);
        assert(model.positions[0] == 6 && model.positions[7] == -2);
        assert(model.uvs[0] == .5f && model.uvs[1] == -1 && model.uvs[5] == 4);
        assert(model.index_count == 4 && model.indices[3] == 0);
        px_bch_model_free(&model);
    }
}
static void test_draw_snapshot(void)
{
    struct fixture f = fixture(1);
    single(&f, 0x25e, 2, 0x100);
    const size_t draw = single(&f, 0x22f, 15, 1);
    single(&f, 0x25e, 2, 0);
    word(f.data + SUB + 48, (uint32_t)((f.cursor - f.face) / 4));
    struct px_bch_model model;
    assert(px_bch_model_decode(f.data, f.size, 0, &model) == PX_BCH_MODEL_OK);
    const struct px_bch_submesh *sub = model.submeshes;
    assert(sub->draw_count == 1 && sub->draw_source_offset == draw);
    assert(sub->draw_primitive_known_mask == 0xff00ff00 && sub->draw_primitive_value == 0x100);
    assert(sub->primitive_value == 0 && sub->primitive_write_count == 3);
    px_bch_model_free(&model);
    reject(f, draw, 0);
    reject(f, draw + 4, 0x1022f);
    struct fixture changed = f;
    single(&changed, 0x228, 15, 3);
    word(changed.data + SUB + 48, (uint32_t)((changed.cursor - changed.face) / 4));
    assert(px_bch_model_decode(changed.data, changed.size, 0, &model) == PX_BCH_MODEL_UNSUPPORTED);
    single(&f, 0x22f, 15, 1);
    word(f.data + SUB + 48, (uint32_t)((f.cursor - f.face) / 4));
    assert(px_bch_model_decode(f.data, f.size, 0, &model) == PX_BCH_MODEL_UNSUPPORTED);
}
static void test_malformed_and_limits(void)
{
    struct fixture f = fixture(1);
    struct px_bch_model model;
    assert(px_bch_model_decode(NULL, 0, 0, &model) == PX_BCH_MODEL_INVALID_ARGUMENT);
    assert(px_bch_model_decode(f.data, f.size, 0, NULL) == PX_BCH_MODEL_INVALID_ARGUMENT);
    assert(px_bch_model_decode(f.data, 64u * 1024 * 1024 + 1, 0, &model) == PX_BCH_MODEL_LIMIT_EXCEEDED);
    assert(px_bch_model_decode(f.data, f.size, 1, &model) != PX_BCH_MODEL_OK);
    for (size_t size = 0; size < f.size; ++size) {
        assert(px_bch_model_decode(f.data, size, 0, &model) != PX_BCH_MODEL_OK);
        assert(model.positions == NULL);
    }
    reject(f, 8, 0); /* Header overlap. */
    reject(f, 12, CONTENT); /* Overlapping sections. */
    reject(f, 48, 1); /* Unsupported address extension. */
    reject(f, MODEL, 1);
    reject(f, MODEL + 4, float_bits(2));
    reject(f, MODEL + 0x38, 1025);
    reject(f, MODEL + 0x80, 2);
    reject(f, MODEL + 0x74, 1);
    reject(f, MATERIAL + 0x4c, 1); /* Non-null unrelocated name. */
    reject(f, MESH + 20, 1025);
    reject(f, SUB, 1); /* Skinning. */
    reject(f, f.face + 8, 786433);
    reject(f, f.face + 4, 0x0f0400); /* Register out of range. */
    reject(f, f.enable + 4, 0xfff00200); /* Packet extent. */
    reject(f, MESH + 12, 65537); /* Command budget. */
    reject(f, f.enable + 16, 250); /* Vertex span beyond raw section. */
    reject(f, f.face, 256); /* Index span beyond raw section. */
    reject(f, f.enable + 32 + 16 + 4, 0x00000242); /* Unknown required byte. */
    reject(f, f.enable + 56, 6); /* Float24 uniforms unsupported. */
    reject(f, RAW, 0x7fc00000); /* Nonfinite source. */
    reject(f, f.vertex_relocation, 0x7f000000); /* Unknown relocation. */
    /* A byte-displaced string relocation cannot overlap a four-byte field. */
    word(f.data + f.size, 1u << 25 | (MODEL + 0x84 + 1 - CONTENT));
    f.size += 4;
    finish(&f);
    assert(px_bch_model_decode(f.data, f.size, 0, &model) != PX_BCH_MODEL_OK);
}
int main(void)
{
    test_geometry_and_ownership();
    test_integer_attributes_and_no_topology_guess();
    test_draw_snapshot();
    test_malformed_and_limits();
    puts("BCH static geometry tests passed");
    return 0;
}
