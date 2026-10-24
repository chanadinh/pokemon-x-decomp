/* Local validation bridge for verify_motion.py.
 *
 *   dump_motion model.bch pack.bin SLOT...
 *
 * JSON from px_gf_motion_pack_decode (skeleton, every motion's frame count,
 * consumed byte range, channel kinds and keys as float bits), then for each
 * listed slot and every integer frame 0..frames: the channel values of
 * px_gf_channel_value at that frame (present channels only), the bone local
 * transforms of px_skin_motion_locals, and every index posed by
 * px_skin_pose_indices after px_skin_world_from_locals and, as the viewer
 * does, px_skin_attach of "glass" onto "loc_acchat".
 * Output may contain local asset data; keep it in ignored outputs/. */
#include "../../assets/bch_model.h"
#include "../../assets/gf_motion.h"
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
static uint8_t *read_file(const char *path, size_t *size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    uint8_t *data = NULL;
    if (!fseek(f, 0, SEEK_END)) {
        const long n = ftell(f);
        if (n >= 0 && !fseek(f, 0, SEEK_SET) && (data = malloc(n ? (size_t)n : 1)) &&
            fread(data, 1, (size_t)n, f) != (size_t)n) { free(data); data = NULL; }
        *size = (size_t)n;
    }
    fclose(f);
    return data;
}
static int32_t bone_named(const struct px_bch_model *m, const char *name)
{
    for (uint32_t i = 0; i < m->bone_count; ++i)
        if (m->bones[i].name.present && m->bones[i].name.size == strlen(name) &&
            !memcmp(m->bones[i].name.bytes, name, m->bones[i].name.size)) return (int32_t)i;
    return -1;
}
int main(int argc, char **argv)
{
    if (argc < 3) { fprintf(stderr, "usage: %s model.bch pack.bin SLOT...\n", argv[0]); return 2; }
    size_t model_size = 0, pack_size = 0;
    uint8_t *model_data = read_file(argv[1], &model_size), *pack_data = read_file(argv[2], &pack_size);
    if (!model_data || !pack_data) return 1;
    struct px_bch_model m;
    struct px_gf_motion_pack p;
    if (px_bch_model_decode_skinned(model_data, model_size, 0, &m) != PX_BCH_MODEL_OK) return 1;
    if (px_gf_motion_pack_decode(pack_data, pack_size, &p) != PX_GF_MOTION_OK) return 1;
    printf("{\"bone_count\":%" PRIu32 ",\"first_bone\":%" PRIu32 ",\"slot_count\":%" PRIu32 ",\"bones\":[",
           p.bone_count, p.first_bone, p.slot_count);
    for (uint32_t i = 0; i < p.bone_count; ++i) {
        const struct px_gf_motion_bone *b = &p.bones[i];
        printf("%s{\"name\":\"%s\",\"parent\":%" PRId32 ",\"flags\":%u,\"children\":%u,\"translation\":",
               i ? "," : "", b->name, b->parent, b->flags, b->children);
        floats(b->translation, 3); printf(",\"rotation\":"); floats(b->rotation, 4); putchar('}');
    }
    printf("],\"motions\":[");
    for (uint32_t i = 0; i < p.motion_count; ++i) {
        const struct px_gf_motion *mo = &p.motions[i];
        printf("%s{\"slot\":%" PRIu32 ",\"frames\":%" PRIu32 ",\"byte_offset\":%" PRIu32 ",\"byte_length\":%" PRIu32
               ",\"channels\":[", i ? "," : "", mo->slot, mo->frames, mo->byte_offset, mo->byte_length);
        for (uint32_t c = 0; c < p.bone_count * 9; ++c) {
            const struct px_gf_channel *ch = &mo->channels[c];
            printf("%s[%" PRIu32 ",[", c ? "," : "", ch->kind);
            for (uint32_t k = 0; k < ch->key_count; ++k) {
                const struct px_gf_key *key = &p.keys[ch->first_key + k];
                printf("%s[%" PRIu32 ",%" PRIu32 ",%" PRIu32 "]", k ? "," : "", key->frame, bits(key->value),
                       bits(key->slope));
            }
            printf("]]");
        }
        printf("]}");
    }
    printf("],\"poses\":[");
    float (*locals)[9] = calloc(m.bone_count, sizeof *locals);
    double *world = calloc(m.bone_count, 12 * sizeof *world), *rest = calloc(m.bone_count, 12 * sizeof *rest);
    float *positions = malloc((size_t)m.index_count * 3 * sizeof *positions);
    float *normals = malloc((size_t)m.index_count * 3 * sizeof *normals);
    if (!locals || !world || !rest || !positions || !normals || !px_skin_rest_world(m.bones, m.bone_count, rest))
        return 1;
    const int32_t glass = bone_named(&m, "glass"), acchat = bone_named(&m, "loc_acchat");
    for (int a = 3; a < argc; ++a) {
        const uint32_t slot = (uint32_t)strtoul(argv[a], NULL, 10);
        const struct px_gf_motion *mo = px_gf_motion_find(&p, slot);
        if (!mo) return 1;
        printf("%s{\"slot\":%" PRIu32 ",\"frames\":[", a > 3 ? "," : "", slot);
        for (uint32_t f = 0; f <= mo->frames; ++f) {
            printf("%s{\"values\":[", f ? "," : "");
            int first = 1;
            for (uint32_t c = 0; c < p.bone_count * 9; ++c) {
                if (!mo->channels[c].kind) continue;
                printf("%s[%" PRIu32 ",%" PRIu32 "]", first ? "" : ",", c,
                       bits(px_gf_channel_value(&p, &mo->channels[c], (float)f, 0)));
                first = 0;
            }
            if (!px_skin_motion_locals(&m, &p, mo, f, locals) ||
                !px_skin_world_from_locals(m.bones, m.bone_count, (const float (*)[9])locals, world) ||
                (glass >= 0 && acchat >= 0 && !px_skin_attach(rest, m.bone_count, (uint32_t)glass,
                                                              (uint32_t)acchat, world)) ||
                !px_skin_pose_indices(&m, world, positions, normals)) return 1;
            printf("],\"locals\":"); floats(&locals[0][0], (size_t)m.bone_count * 9);
            printf(",\"positions\":"); floats(positions, (size_t)m.index_count * 3);
            printf(",\"normals\":"); floats(normals, (size_t)m.index_count * 3);
            putchar('}');
        }
        printf("]}");
    }
    printf("]}\n");
    free(locals); free(world); free(rest); free(positions); free(normals);
    px_gf_motion_pack_free(&p); px_bch_model_free(&m); free(model_data); free(pack_data);
    return 0;
}
