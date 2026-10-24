/* Local validation bridge for verify_player.py.
 *
 *   dump_player model.bch    JSON from px_bch_model_decode_skinned: bones,
 *                            meshes (skinning sources), submeshes (mode,
 *                            palette, index range), per-vertex bone indices
 *                            and weights (weights as float bits), then the
 *                            rest-pose bake (positions and normals as float
 *                            bits, one vertex per index).
 * Output may contain local asset data; keep it in ignored outputs/. */
#include "../../assets/bch_model.h"
#include "../../render/skin.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t bits(float value)
{
    uint32_t out;
    memcpy(&out, &value, sizeof out);
    return out;
}
static void floats(const float *v, size_t n)
{
    putchar('[');
    for (size_t i = 0; i < n; ++i) printf("%s%" PRIu32, i ? "," : "", bits(v[i]));
    putchar(']');
}
static void name(const struct px_bch_name *n)
{
    putchar('"');
    for (uint32_t i = 0; i < n->size; ++i) {
        const unsigned char c = (unsigned char)n->bytes[i];
        if (c >= 32 && c < 127 && c != '"' && c != '\\') putchar(c); else printf("\\u%04x", c);
    }
    putchar('"');
}
int main(int argc, char **argv)
{
    if (argc != 2) { fprintf(stderr, "usage: %s model.bch\n", argv[0]); return 2; }
    FILE *f = fopen(argv[1], "rb");
    if (!f) return 1;
    static uint8_t data[64u << 20];
    const size_t size = fread(data, 1, sizeof data, f);
    fclose(f);
    struct px_bch_model m;
    if (px_bch_model_decode_skinned(data, size, 0, &m) != PX_BCH_MODEL_OK) return 1;
    printf("{\"model_flags\":%" PRIu32 ",\"bone_scaling\":%" PRIu32 ",\"bones\":[", m.model_flags, m.bone_scaling);
    for (uint32_t i = 0; i < m.bone_count; ++i) {
        const struct px_bch_bone *b = &m.bones[i];
        printf("%s{\"name\":", i ? "," : ""); name(&b->name);
        printf(",\"flags\":%" PRIu32 ",\"parent\":%" PRId32 ",\"scale\":", b->flags, b->parent);
        floats(b->scale, 3); printf(",\"rotation\":"); floats(b->rotation, 3);
        printf(",\"translation\":"); floats(b->translation, 3); printf(",\"inverse\":"); floats(b->inverse, 12);
        putchar('}');
    }
    printf("],\"meshes\":[");
    for (uint32_t i = 0; i < m.mesh_count; ++i) {
        const struct px_bch_mesh *me = &m.meshes[i];
        printf("%s{\"bone_index_source\":%" PRIu32 ",\"bone_weight_source\":%" PRIu32
               ",\"bone_weight_elements\":%" PRIu32 ",\"indices\":[", i ? "," : "",
               me->bone_index_source, me->bone_weight_source, me->bone_weight_elements);
        for (uint32_t v = 0; v < me->vertex_count * 4; ++v)
            printf("%s%u", v ? "," : "", m.bone_indices[(size_t)me->first_vertex * 4 + v]);
        printf("],\"weights\":");
        floats(m.bone_weights + (size_t)me->first_vertex * 4, (size_t)me->vertex_count * 4);
        printf(",\"submeshes\":[");
        for (uint32_t s = 0; s < me->submesh_count; ++s) {
            const struct px_bch_submesh *sub = &m.submeshes[me->first_submesh + s];
            printf("%s{\"skinning\":%" PRIu32 ",\"palette\":[", s ? "," : "", sub->skinning);
            for (uint32_t k = 0; k < sub->bone_count; ++k) printf("%s%u", k ? "," : "", sub->bone_indices[k]);
            printf("],\"first_index\":%" PRIu32 ",\"index_count\":%" PRIu32 "}", sub->first_index, sub->index_count);
        }
        printf("]}");
    }
    if (!px_skin_bake_rest(&m)) { px_bch_model_free(&m); return 1; }
    printf("],\"baked_positions\":"); floats(m.positions, (size_t)m.vertex_count * 3);
    printf(",\"baked_normals\":"); floats(m.normals, (size_t)m.vertex_count * 3);
    printf("}\n");
    px_bch_model_free(&m);
    return 0;
}
