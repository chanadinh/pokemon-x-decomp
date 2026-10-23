/* Local validation bridge: expose the native material metadata, preview
 * interpretation and bound texture pixels to verify_materials.py. Output
 * contains local asset data; consume it through a pipe or ignored outputs/. */
#include "../../assets/bch_material.h"
#include "../../assets/bch_model.h"
#include "../../assets/bch_texture.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t *read_file(const char *path, size_t *size)
{
    FILE *file = fopen(path, "rb");
    if (!file) return NULL;
    uint8_t *data = NULL;
    long length;
    if (!fseek(file, 0, SEEK_END) && (length = ftell(file)) > 0 &&
        length <= 64 * 1024 * 1024 && !fseek(file, 0, SEEK_SET) &&
        (data = malloc((size_t)length)) && fread(data, 1, (size_t)length, file) == (size_t)length) {
        *size = (size_t)length;
    } else {
        free(data); data = NULL;
    }
    fclose(file);
    return data;
}

static void hex(const void *bytes, size_t size)
{
    putchar('"');
    for (size_t i = 0; i < size; ++i) printf("%02x", ((const unsigned char *)bytes)[i]);
    putchar('"');
}

static void reg(const char *key, const struct px_bch_register *r, int comma)
{
    printf("%s\"%s\":[%" PRIu32 ",%" PRIu32 "]", comma ? "," : "", key, r->value, r->known_mask);
}

int main(int argc, char **argv)
{
    if (argc != 3) { fprintf(stderr, "Usage: %s field.bch textures.bch\n", argv[0]); return 1; }
    size_t model_size = 0, texture_size = 0;
    uint8_t *model_data = read_file(argv[1], &model_size);
    uint8_t *texture_data = read_file(argv[2], &texture_size);
    if (!model_data || !texture_data) { free(model_data); free(texture_data); return 1; }
    struct px_bch_model model = {0};
    enum px_bch_model_result status = px_bch_model_decode(model_data, model_size, 0, &model);
    free(model_data); /* Material metadata must outlive the input allocation. */
    struct px_bch *pack = NULL;
    if (status != PX_BCH_MODEL_OK || px_bch_open(texture_data, texture_size, &pack) != PX_BCH_OK) {
        fprintf(stderr, "decode failed\n"); px_bch_model_free(&model); free(texture_data); return 1;
    }
    printf("{\"materials\":[");
    for (uint32_t i = 0; i < model.material_count; ++i) {
        const struct px_bch_material *m = &model.materials[i];
        printf("%s{\"name_hex\":", i ? "," : ""); hex(m->name.bytes, m->name.size);
        printf(",\"texture_unit_config\":%" PRIu32 ",\"parameters_present\":%" PRIu32,
               m->texture_unit_config, m->parameters_present);
        printf(",\"coordinators\":[");
        for (unsigned u = 0; u < 3; ++u) {
            const struct px_bch_coordinator *c = &m->coordinators[u];
            printf("%s[%" PRIu32 ",%.9g,%.9g,%.9g,%.9g,%.9g]", u ? "," : "", c->packed,
                   c->scale[0], c->scale[1], c->rotation, c->translation[0], c->translation[1]);
        }
        printf("],\"unit_parameters\":[");
        for (unsigned u = 0; u < 3; ++u)
            printf("%s[%" PRIu32 ",%" PRIu32 "]", u ? "," : "",
                   m->unit_parameters[u].value, m->unit_parameters[u].known_mask);
        printf("],\"fragment\":{");
        const struct px_bch_fragment_state *f = &m->fragment;
        reg("0x40", &f->cull, 0); reg("0x100", &f->color_operation, 1);
        reg("0x101", &f->blend, 1); reg("0x104", &f->alpha_test, 1);
        reg("0x107", &f->depth_color, 1); reg("0xe0", &f->tev_buffer, 1);
        static const unsigned stages[6] = {0xc0, 0xc8, 0xd0, 0xd8, 0xf0, 0xf8};
        for (unsigned s = 0; s < 6; ++s)
            for (unsigned j = 0; j < 5; ++j) {
                char key[8]; snprintf(key, sizeof key, "0x%x", stages[s] + j);
                reg(key, &f->tev[s][j], 1);
            }
        printf("}");
        struct px_bch_preview_material p;
        const enum px_bch_material_result result = px_bch_material_preview(m, &p);
        printf(",\"preview_result\":%d", (int)result);
        if (result == PX_BCH_MATERIAL_OK) {
            printf(",\"preview\":{\"unit\":%" PRIu32 ",\"combine_rgb\":%" PRIu32
                   ",\"combine_alpha\":%" PRIu32 ",\"wrap_u\":%" PRIu32 ",\"wrap_v\":%" PRIu32
                   ",\"mag_linear\":%" PRIu32 ",\"min_linear\":%" PRIu32
                   ",\"alpha_test\":%" PRIu32 ",\"alpha_compare\":%" PRIu32
                   ",\"alpha_reference\":%" PRIu32 ",\"depth_test\":%" PRIu32
                   ",\"depth_compare\":%" PRIu32 ",\"depth_write\":%" PRIu32
                   ",\"color_write_mask\":%" PRIu32 ",\"cull\":%" PRIu32
                   ",\"blend\":%d,\"unexecuted_units\":%" PRIu32 "}",
                   p.unit, p.combine_rgb, p.combine_alpha, p.wrap_u, p.wrap_v, p.mag_linear,
                   p.min_linear, p.alpha_test, p.alpha_compare, p.alpha_reference, p.depth_test,
                   p.depth_compare, p.depth_write, p.color_write_mask, p.cull, (int)p.blend,
                   p.unexecuted_units);
            const struct px_bch_name *wanted = &m->texture_names[p.unit];
            long found = -1; int matches = 0;
            for (size_t j = 0; j < px_bch_texture_count(pack); ++j) {
                const struct px_bch_texture *t = px_bch_texture(pack, j);
                if (t->name_size == wanted->size && !memcmp(t->name, wanted->bytes, wanted->size)) {
                    found = (long)j; ++matches;
                }
            }
            printf(",\"texture_name_hex\":"); hex(wanted->bytes, wanted->size);
            printf(",\"name_matches\":%d,\"external_index\":%ld", matches, found);
            struct px_texture_image image = {0};
            if (matches == 1 && px_bch_decode_texture(pack, (size_t)found, &image) == PX_BCH_OK) {
                printf(",\"width\":%" PRIu32 ",\"height\":%" PRIu32 ",\"rgba\":",
                       image.width, image.height);
                hex(image.rgba, (size_t)image.width * image.height * 4);
                px_texture_free(&image);
            }
        }
        putchar('}');
    }
    printf("]}\n");
    px_bch_close(pack); free(texture_data); px_bch_model_free(&model);
    return 0;
}
