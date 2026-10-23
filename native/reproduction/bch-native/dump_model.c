/* Local validation bridge: expose the native API's owned arrays to the
 * independent Python reference comparison. Output contains local asset data;
 * capture it only under ignored outputs/ or consume it through a pipe. */
#include "../../assets/bch_model.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

static void name_hex(const struct px_bch_name *name)
{
    putchar('"');
    for (uint32_t i = 0; i < name->size; ++i) printf("%02x", (unsigned char)name->bytes[i]);
    putchar('"');
}

static void vectors(const float *data, uint32_t count, unsigned components)
{
    putchar('[');
    for (uint32_t i = 0; i < count; ++i) {
        if (i) putchar(',');
        putchar('[');
        for (unsigned j = 0; j < components; ++j)
            printf("%s%.9g", j ? "," : "", data[(size_t)i * components + j]);
        putchar(']');
    }
    putchar(']');
}

int main(int argc, char **argv)
{
    if (argc != 2) { fprintf(stderr, "Usage: %s selected.bch\n", argv[0]); return 1; }
    FILE *file = fopen(argv[1], "rb");
    if (!file) return 1;
    if (fseek(file, 0, SEEK_END)) { fclose(file); return 1; }
    long length = ftell(file);
    if (length <= 0 || length > 64 * 1024 * 1024 || fseek(file, 0, SEEK_SET)) {
        fclose(file); return 1;
    }
    uint8_t *data = malloc((size_t)length);
    if (!data) { fclose(file); return 1; }
    int ok = fread(data, 1, (size_t)length, file) == (size_t)length;
    fclose(file);
    struct px_bch_model model = {0};
    enum px_bch_model_result result = ok ? px_bch_model_decode(data, (size_t)length, 0, &model) : PX_BCH_MODEL_TRUNCATED;
    free(data); /* Successful output must outlive the input allocation. */
    if (result != PX_BCH_MODEL_OK) {
        fprintf(stderr, "%s\n", px_bch_model_result_string(result)); return 1;
    }
    printf("{\"name_hex\":"); name_hex(&model.name);
    printf(",\"vertex_count\":%" PRIu32 ",\"index_count\":%" PRIu32 ",\"positions\":",
           model.vertex_count, model.index_count);
    vectors(model.positions, model.vertex_count, 3);
    printf(",\"uv0\":"); vectors(model.uvs, model.vertex_count, 2);
    printf(",\"indices\":[");
    for (uint32_t i = 0; i < model.index_count; ++i) printf("%s%" PRIu32, i ? "," : "", model.indices[i]);
    printf("],\"meshes\":[");
    for (uint32_t i = 0; i < model.mesh_count; ++i) {
        const struct px_bch_mesh *mesh = &model.meshes[i];
        printf("%s{\"name_hex\":", i ? "," : ""); name_hex(&mesh->name);
        printf(",\"first_vertex\":%" PRIu32 ",\"vertex_count\":%" PRIu32
               ",\"first_index\":%" PRIu32 ",\"index_count\":%" PRIu32
               ",\"material_index\":%" PRIu32 ",\"first_submesh\":%" PRIu32
               ",\"submesh_count\":%" PRIu32 ",\"visible\":%" PRIu32
               ",\"layer\":%" PRIu32 ",\"priority\":%" PRIu32 "}",
               mesh->first_vertex, mesh->vertex_count, mesh->first_index, mesh->index_count,
               mesh->material_index, mesh->first_submesh, mesh->submesh_count,
               mesh->visible, mesh->layer, mesh->priority);
    }
    printf("],\"submeshes\":[");
    for (uint32_t i = 0; i < model.submesh_count; ++i) {
        const struct px_bch_submesh *sub = &model.submeshes[i];
        printf("%s{\"first_index\":%" PRIu32 ",\"index_count\":%" PRIu32
               ",\"index_width\":%" PRIu32 ",\"draw_count\":%" PRIu32
               ",\"primitive_value\":%" PRIu32 ",\"primitive_known_mask\":%" PRIu32 "}",
               i ? "," : "", sub->first_index, sub->index_count, sub->index_width,
               sub->draw_count, sub->draw_primitive_value, sub->draw_primitive_known_mask);
    }
    printf("],\"materials\":[");
    for (uint32_t i = 0; i < model.material_count; ++i) {
        const struct px_bch_material *material = &model.materials[i];
        printf("%s{\"name_hex\":", i ? "," : ""); name_hex(&material->name);
        printf(",\"texture_unit_config\":%" PRIu32 ",\"texture_names\":[", material->texture_unit_config);
        for (unsigned unit = 0; unit < 3; ++unit) {
            if (unit) putchar(',');
            if (material->texture_names[unit].present) name_hex(&material->texture_names[unit]);
            else printf("null");
        }
        printf("]}");
    }
    printf("]}\n");
    px_bch_model_free(&model);
    return fflush(stdout) || ferror(stdout) ? 1 : 0;
}
