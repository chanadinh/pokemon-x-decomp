/* Local validation bridge for verify_combiners.py.
 *
 *   dump_combiners field.bch    JSON: per-material combiner interpretation
 *                               and per-mesh decoded vertex colors.
 *   dump_combiners --evaluate   stdin lines of 32 hex words (6 stages x
 *                               source, operand, combine, constant, scale;
 *                               buffer color; buffer update) then 24 hex
 *                               bytes (primary, fragment primary, fragment
 *                               secondary, textures 0..2 as RGBA). Prints
 *                               "valid r g b a" per line using tev.c.
 * Output may contain local asset data; keep it in ignored outputs/. */
#include "../../assets/bch_material.h"
#include "../../assets/bch_model.h"
#include "../../render/tev.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int evaluate(void)
{
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        struct px_tev_config c;
        struct px_tev_inputs in;
        uint32_t words[32];
        unsigned bytes[24];
        char *p = line;
        memset(&c, 0, sizeof c);
        memset(&in, 0, sizeof in);
        for (unsigned i = 0; i < 32; ++i) {
            char *end;
            words[i] = (uint32_t)strtoul(p, &end, 16);
            if (end == p) return 1;
            p = end;
        }
        for (unsigned i = 0; i < 24; ++i) {
            char *end;
            bytes[i] = (unsigned)strtoul(p, &end, 16);
            if (end == p || bytes[i] > 255) return 1;
            p = end;
        }
        for (unsigned s = 0; s < 6; ++s) {
            c.stages[s].source = words[s * 5]; c.stages[s].operand = words[s * 5 + 1];
            c.stages[s].combine = words[s * 5 + 2]; c.stages[s].constant = words[s * 5 + 3];
            c.stages[s].scale = words[s * 5 + 4];
        }
        c.buffer_color = words[30]; c.buffer_update = words[31];
        for (unsigned j = 0; j < 4; ++j) {
            in.primary[j] = (uint8_t)bytes[j];
            in.fragment_primary[j] = (uint8_t)bytes[4 + j];
            in.fragment_secondary[j] = (uint8_t)bytes[8 + j];
            for (unsigned t = 0; t < 3; ++t) in.texture[t][j] = (uint8_t)bytes[12 + 4 * t + j];
        }
        const int valid = px_tev_valid(&c);
        uint8_t out[4] = {0, 0, 0, 0};
        if (valid) px_tev_evaluate(&c, &in, out);
        printf("%d %u %u %u %u\n", valid, out[0], out[1], out[2], out[3]);
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 2 && !strcmp(argv[1], "--evaluate")) return evaluate();
    if (argc != 2) { fprintf(stderr, "Usage: %s field.bch | --evaluate\n", argv[0]); return 1; }
    FILE *file = fopen(argv[1], "rb");
    if (!file) return 1;
    static uint8_t data[64 * 1024 * 1024];
    const size_t size = fread(data, 1, sizeof data, file);
    fclose(file);
    struct px_bch_model model = {0};
    if (px_bch_model_decode(data, size, 0, &model) != PX_BCH_MODEL_OK) return 1;
    printf("{\"materials\":[");
    for (uint32_t i = 0; i < model.material_count; ++i) {
        struct px_bch_combiner_material m;
        const enum px_bch_material_result result = px_bch_material_combiners(&model.materials[i], &m);
        printf("%s{\"result\":%d", i ? "," : "", (int)result);
        if (result == PX_BCH_MATERIAL_OK) {
            printf(",\"stages\":[");
            for (unsigned s = 0; s < 6; ++s) {
                const struct px_tev_stage *t = &m.tev.stages[s];
                printf("%s[%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 "]", s ? "," : "",
                       t->source, t->operand, t->combine, t->constant, t->scale);
            }
            printf("],\"buffer_color\":%" PRIu32 ",\"buffer_update\":%" PRIu32 ",\"sources\":%" PRIu32
                   ",\"sampled_units\":%" PRIu32 ",\"projected_units\":%" PRIu32 ",\"disabled_units\":%" PRIu32,
                   m.tev.buffer_color, m.tev.buffer_update, m.sources, m.sampled_units,
                   m.projected_units, m.disabled_units);
        }
        putchar('}');
    }
    printf("],\"meshes\":[");
    for (uint32_t i = 0; i < model.mesh_count; ++i) {
        const struct px_bch_mesh *mesh = &model.meshes[i];
        printf("%s{\"color_source\":%" PRIu32 ",\"colors\":[", i ? "," : "", mesh->color_source);
        for (uint32_t v = 0; v < mesh->vertex_count; ++v) {
            const float *c = model.colors + ((size_t)mesh->first_vertex + v) * 4;
            printf("%s[%.9g,%.9g,%.9g,%.9g]", v ? "," : "", c[0], c[1], c[2], c[3]);
        }
        printf("]}");
    }
    printf("]}\n");
    px_bch_model_free(&model);
    return 0;
}
