/* Local validation bridge for verify_lighting.py.
 *
 *   dump_lighting --environment env.bch   JSON: lights and scene 0 light sets
 *                                         from px_bch_environment_decode.
 *   dump_lighting --model field.bch       JSON: per-material lighting params
 *                                         and per-mesh normals (as float bits).
 *   dump_lighting --registers             65,536 lines "light material value"
 *                                         from px_light_register_channel.
 *   dump_lighting --evaluate              stdin lines: count g0 g1 g2, then per
 *                                         light a0 a1 a2 d0 d1 d2 x y z two_sided
 *                                         (x/y/z as hex float bits), then the
 *                                         normal's three hex float bits. Prints
 *                                         "r g b a" or "invalid" (light.c).
 * Output may contain local asset data; keep it in ignored outputs/. */
#include "../../assets/bch_model.h"
#include "../../assets/bch_scene.h"
#include "../../render/light.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t *read_file(const char *path, size_t *size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    uint8_t *data = NULL;
    size_t used = 0, capacity = 0;
    for (;;) {
        if (used == capacity) {
            capacity = capacity ? capacity * 2 : 65536;
            uint8_t *grown = realloc(data, capacity);
            if (!grown) { free(data); fclose(f); return NULL; }
            data = grown;
        }
        const size_t n = fread(data + used, 1, capacity - used, f);
        used += n;
        if (!n) break;
    }
    fclose(f);
    *size = used;
    return data;
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
static uint32_t bits(float value)
{
    uint32_t out;
    memcpy(&out, &value, sizeof out);
    return out;
}
static void floats(const float *v, unsigned n)
{
    putchar('[');
    for (unsigned i = 0; i < n; ++i) printf("%s%" PRIu32, i ? "," : "", bits(v[i]));
    putchar(']');
}
static void bytes(const uint8_t *v, unsigned n)
{
    putchar('[');
    for (unsigned i = 0; i < n; ++i) printf("%s%u", i ? "," : "", v[i]);
    putchar(']');
}
static int environment(const uint8_t *data, size_t size)
{
    struct px_bch_environment e;
    if (px_bch_environment_decode(data, size, 0, &e) != PX_BCH_MODEL_OK) return 1;
    printf("{\"scene\":"); name(&e.scene_name);
    printf(",\"cameras\":%" PRIu32 ",\"fogs\":%" PRIu32 ",\"lights\":[", e.camera_count, e.fog_count);
    for (uint32_t i = 0; i < e.light_count; ++i) {
        const struct px_bch_light *l = &e.lights[i];
        printf("%s{\"name\":", i ? "," : ""); name(&l->name);
        printf(",\"type\":%" PRIu32 ",\"enabled\":%" PRIu32 ",\"flags\":%" PRIu32 ",\"lut\":%" PRIu32,
               l->type, l->enabled, l->flags, l->lut_config);
        printf(",\"scale\":"); floats(l->scale, 3);
        printf(",\"rotation\":"); floats(l->rotation, 3);
        printf(",\"translation\":"); floats(l->translation, 3);
        printf(",\"direction\":"); floats(l->direction, 3);
        printf(",\"attenuation\":"); floats(&l->attenuation_start, 1);
        printf(",\"attenuation_end\":"); floats(&l->attenuation_end, 1);
        printf(",\"ambient\":"); bytes(l->ambient, 4);
        printf(",\"diffuse\":"); bytes(l->diffuse, 4);
        printf(",\"specular0\":"); bytes(l->specular0, 4);
        printf(",\"specular1\":"); bytes(l->specular1, 4);
        printf(",\"samplers\":[%" PRIu32 ",%" PRIu32 "]}", l->has_distance_sampler, l->has_angle_sampler);
    }
    printf("],\"sets\":[");
    for (uint32_t i = 0; i < e.set_count; ++i) {
        const struct px_bch_light_set *s = &e.sets[i];
        printf("%s{\"index\":%" PRIu32 ",\"lights\":[", i ? "," : "", s->index);
        for (uint32_t k = 0; k < s->count; ++k) printf("%s%" PRIu32, k ? "," : "", e.set_lights[s->first + k]);
        printf("]}");
    }
    printf("]}\n");
    px_bch_environment_free(&e);
    return 0;
}
static int model(const uint8_t *data, size_t size)
{
    struct px_bch_model m;
    if (px_bch_model_decode(data, size, 0, &m) != PX_BCH_MODEL_OK) return 1;
    printf("{\"materials\":[");
    for (uint32_t i = 0; i < m.material_count; ++i) {
        const struct px_bch_material *x = &m.materials[i];
        printf("%s{\"present\":%" PRIu32 ",\"flags\":%" PRIu32 ",\"fragment_flags\":%" PRIu32
               ",\"light_set\":%" PRIu32 ",\"fog\":%" PRIu32 ",\"lut_samplers\":%" PRIu32
               ",\"fresnel\":%" PRIu32 ",\"bump\":%" PRIu32 ",\"color_scale\":%" PRIu32 ",\"colors\":[",
               i ? "," : "", x->parameters_present, x->material_flags, x->fragment_flags,
               x->light_set_index, x->fog_index, x->lut_sampler_mask, x->fresnel_selector, x->bump_mode,
               bits(x->color_scale));
        for (unsigned c = 0; c < 5; ++c) { if (c) putchar(','); bytes(x->colors[c], 4); }
        printf("]}");
    }
    printf("],\"meshes\":[");
    for (uint32_t i = 0; i < m.mesh_count; ++i) {
        const struct px_bch_mesh *mesh = &m.meshes[i];
        printf("%s{\"normal_source\":%" PRIu32 ",\"material\":%" PRIu32 ",\"normals\":",
               i ? "," : "", mesh->normal_source, mesh->material_index);
        floats(m.normals + 3 * (size_t)mesh->first_vertex, 3 * mesh->vertex_count);
        putchar('}');
    }
    printf("]}\n");
    px_bch_model_free(&m);
    return 0;
}
static int evaluate(void)
{
    char line[4096];
    while (fgets(line, sizeof line, stdin)) {
        struct px_light_setup setup;
        memset(&setup, 0, sizeof setup);
        char *p = line, *end;
        unsigned long count = strtoul(p, &end, 10);
        if (end == p || count > 8) return 1;
        p = end;
        setup.count = (uint32_t)count;
        for (unsigned c = 0; c < 3; ++c) { setup.global_ambient[c] = (uint16_t)strtoul(p, &end, 10); p = end; }
        for (unsigned long i = 0; i < count; ++i) {
            struct px_light_source *l = &setup.lights[i];
            for (unsigned c = 0; c < 3; ++c) { l->ambient[c] = (uint16_t)strtoul(p, &end, 10); p = end; }
            for (unsigned c = 0; c < 3; ++c) { l->diffuse[c] = (uint16_t)strtoul(p, &end, 10); p = end; }
            for (unsigned c = 0; c < 3; ++c) {
                const uint32_t b = (uint32_t)strtoul(p, &end, 16);
                p = end;
                memcpy(&l->vector[c], &b, sizeof b);
            }
            l->two_sided = (uint32_t)strtoul(p, &end, 10);
            p = end;
        }
        float normal[3];
        for (unsigned c = 0; c < 3; ++c) {
            const uint32_t b = (uint32_t)strtoul(p, &end, 16);
            if (end == p) return 1;
            p = end;
            memcpy(&normal[c], &b, sizeof b);
        }
        uint8_t out[4];
        if (px_light_primary(&setup, normal, out)) printf("%u %u %u %u\n", out[0], out[1], out[2], out[3]);
        else puts("invalid");
    }
    return 0;
}
int main(int argc, char **argv)
{
    if (argc == 2 && !strcmp(argv[1], "--evaluate")) return evaluate();
    if (argc == 2 && !strcmp(argv[1], "--registers")) {
        for (unsigned l = 0; l < 256; ++l)
            for (unsigned m = 0; m < 256; ++m)
                printf("%u %u %u\n", l, m, px_light_register_channel((uint8_t)l, (uint8_t)m));
        return 0;
    }
    if (argc != 3 || (strcmp(argv[1], "--environment") && strcmp(argv[1], "--model"))) {
        fprintf(stderr, "usage: %s --environment FILE | --model FILE | --registers | --evaluate\n", argv[0]);
        return 2;
    }
    size_t size = 0;
    uint8_t *data = read_file(argv[2], &size);
    if (!data) return 1;
    const int status = argv[1][2] == 'e' ? environment(data, size) : model(data, size);
    free(data);
    return status;
}
