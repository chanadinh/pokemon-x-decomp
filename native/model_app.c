#include "assets/cgfx_model.h"
#include "assets/bch_model.h"
#include "assets/bch_material.h"
#include "assets/bch_scene.h"
#include "assets/bch_texture.h"
#include "assets/cgfx.h"
#include "assets/field.h"
#include "assets/garc.h"
#include "assets/lz.h"
#include "render/light.h"
#include "render/raster.h"

#include <SDL.h>
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct options {
    const char *romfs, *archive, *capture, *texture_archive, *environment_archive;
    uint32_t environment_entry, environment_subentry, environment_section, scene;
    int have_environment_entry, have_environment_subentry, have_environment_section, have_scene;
    int lighting; /* 0 none, 1 day, 2 night */
    uint32_t entry, subentry, model, frames, section;
    uint32_t texture_entry, texture_subentry, texture_section;
    int have_entry, have_subentry, have_model, demo, inspect, software, self_test, textured;
    int have_section, triangle_list, combiners;
    int have_texture_entry, have_texture_subentry, have_texture_section;
};
struct camera { double yaw, pitch, zoom; int running; };
struct model_asset {
    struct px_cgfx_model wire;
    struct px_cgfx_scene scene;
    struct px_bch_model bch;
    struct px_texture_image *textures;
    uint32_t *draw_order;
    uint32_t texture_count;
    /* BCH textured preview: per-material interpretation and the external
     * texture index (into textures) resolved by exact name bytes. */
    struct px_bch_preview_material *bch_preview;
    struct px_bch_combiner_material *bch_combiner; /* --combiners only */
    uint32_t *bch_texture; /* 3 per material: textures index or UINT32_MAX */
    /* --environment only: decoded lights and one light setup per material.
     * point_bound holds, per material and RGB channel, the summed register
     * ambient + diffuse of its unexecuted point/spot lights, in 1/255 units:
     * their contribution if every attenuation factor and dot lies in 0..1. */
    struct px_bch_environment environment;
    struct px_light_setup *bch_light;
    uint32_t *point_bound;
    uint32_t *light_index; /* 8 per material: environment light per setup slot */
    int lighting;
    int bch_textured;
};

static void asset_free(struct model_asset *asset)
{
    if (asset->textures)
        for (uint32_t i = 0; i < asset->texture_count; ++i) px_texture_free(&asset->textures[i]);
    free(asset->textures);
    free(asset->draw_order);
    free(asset->bch_preview);
    free(asset->bch_combiner);
    free(asset->bch_texture);
    free(asset->bch_light);
    free(asset->point_bound);
    free(asset->light_index);
    px_bch_environment_free(&asset->environment);
    px_cgfx_scene_free(&asset->scene);
    px_cgfx_model_free(&asset->wire);
    px_bch_model_free(&asset->bch);
    memset(asset, 0, sizeof *asset);
}

static int prepare_draw_order(struct model_asset *asset)
{
    const struct px_cgfx_scene *s = &asset->scene;
    asset->draw_order = malloc((size_t)s->draw_count * sizeof *asset->draw_order);
    if (!asset->draw_order) return 0;
    /* Preview policy: ascending material translucency layer, then priority;
     * equal keys retain resource object order. No distance-based sorting. */
    for (uint32_t i = 0; i < s->draw_count; ++i) {
        uint32_t j = i;
        const struct px_cgfx_draw *next = &s->draws[i];
        uint32_t layer = s->materials[next->material_index].translucency_kind;
        while (j) {
            const struct px_cgfx_draw *previous = &s->draws[asset->draw_order[j - 1]];
            uint32_t previous_layer = s->materials[previous->material_index].translucency_kind;
            if (previous_layer < layer ||
                (previous_layer == layer && previous->render_priority <= next->render_priority)) break;
            asset->draw_order[j] = asset->draw_order[j - 1]; --j;
        }
        asset->draw_order[j] = i;
    }
    return 1;
}

static void usage(FILE *stream, const char *program)
{
    fprintf(stream,
        "Usage: %s (--demo | --romfs PATH --archive RELATIVE/PATH --entry N) [options]\n"
        "  --subentry N       GARC bitmap subentry 0..31 (default 0)\n"
        "  --model N          CGFX/BCH model index (default 0)\n"
        "  --section N        Select an AD/GR/BM minipack section, 0..255\n"
        "  --triangle-list    Explicit BCH wireframe index-triple interpretation\n"
        "  --textured         Preview supported material texture and vertex colors\n"
        "                     (BCH: unlit TEV stage 0 only; requires --triangle-list\n"
        "                     and --texture-archive/--texture-entry)\n"
        "  --texture-archive RELATIVE/PATH  GARC holding the external BCH textures\n"
        "  --texture-entry N  Texture GARC entry; --texture-subentry N (default 0)\n"
        "  --texture-section N  Select an AD/GR/BM minipack section of that entry\n"
        "  --combiners        BCH: run all six TEV stages with declared unlit inputs\n"
        "  --environment-archive RELATIVE/PATH  GARC holding the scene environment BCH\n"
        "  --environment-entry N  Environment entry; --environment-subentry N (default 0)\n"
        "  --environment-section N  Its AD/GR/BM minipack section; --scene N (default 0)\n"
        "  --lighting day|night  With --combiners and an environment: execute the\n"
        "                     material light set's directional lights (default day;\n"
        "                     day omits light_night, night omits light_sun)\n"
        "  --inspect          Print decoded mesh metadata without a window\n"
        "  --capture-frame F  Save first viewport to a new BMP file\n"
        "  --frames N         Exit after a positive number of frames\n"
        "  --software         Force software rendering\n"
        "  --self-test-input  Exercise orbit/zoom/reset/exit; default 3 frames\n"
        "Controls: arrows orbit, +/- zoom, R/Home reset, Escape exit.\n"
        "Orthographic wireframe by default; --textured uses a bounded software preview.\n"
        "Inspection camera; lighting only with --environment-archive; no animation or gameplay.\n", program);
}

static int number(const char *text, uint32_t *out)
{
    if (!*text) return 0;
    for (const char *p = text; *p; ++p) if (*p < '0' || *p > '9') return 0;
    errno = 0;
    const uintmax_t value = strtoumax(text, NULL, 10);
    if (errno || value > UINT32_MAX) return 0;
    *out = (uint32_t)value;
    return 1;
}

static int relative_path(const char *path)
{
    const char *start = path;
    for (const char *p = path;; ++p) {
        if (*p == '\\' || *p == ':') return 0;
        if (*p != '/' && *p) continue;
        size_t length = (size_t)(p - start);
        if (!length || (length == 1 && *start == '.') ||
            (length == 2 && start[0] == '.' && start[1] == '.')) return 0;
        if (!*p) return 1;
        start = p + 1;
    }
}

static int parse(int argc, char **argv, struct options *o)
{
    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if (!strcmp(arg, "--help")) return 2;
        if (!strcmp(arg, "--demo")) { o->demo = 1; continue; }
        if (!strcmp(arg, "--inspect")) { o->inspect = 1; continue; }
        if (!strcmp(arg, "--software")) { o->software = 1; continue; }
        if (!strcmp(arg, "--self-test-input")) { o->self_test = 1; continue; }
        if (!strcmp(arg, "--textured")) { o->textured = 1; continue; }
        if (!strcmp(arg, "--triangle-list")) { o->triangle_list = 1; continue; }
        if (!strcmp(arg, "--combiners")) { o->combiners = 1; continue; }
        if (strcmp(arg, "--romfs") && strcmp(arg, "--archive") && strcmp(arg, "--entry") &&
            strcmp(arg, "--subentry") && strcmp(arg, "--model") && strcmp(arg, "--frames") &&
            strcmp(arg, "--capture-frame") && strcmp(arg, "--section") &&
            strcmp(arg, "--texture-archive") && strcmp(arg, "--texture-entry") &&
            strcmp(arg, "--texture-subentry") && strcmp(arg, "--texture-section") &&
            strcmp(arg, "--environment-archive") && strcmp(arg, "--environment-entry") &&
            strcmp(arg, "--environment-subentry") && strcmp(arg, "--environment-section") &&
            strcmp(arg, "--scene") && strcmp(arg, "--lighting")) return 0;
        if (++i == argc || !argv[i][0]) return 0;
        const char *value = argv[i];
        if (!strcmp(arg, "--romfs")) o->romfs = value;
        else if (!strcmp(arg, "--archive")) o->archive = value;
        else if (!strcmp(arg, "--capture-frame")) o->capture = value;
        else if (!strcmp(arg, "--texture-archive")) o->texture_archive = value;
        else if (!strcmp(arg, "--environment-archive")) o->environment_archive = value;
        else if (!strcmp(arg, "--lighting")) {
            if (o->lighting) return 0;
            if (!strcmp(value, "day")) o->lighting = 1;
            else if (!strcmp(value, "night")) o->lighting = 2;
            else return 0;
        }
        else {
            uint32_t n;
            if (!number(value, &n)) return 0;
            if (!strcmp(arg, "--entry")) { o->entry = n; o->have_entry = 1; }
            else if (!strcmp(arg, "--subentry")) { if (n > 31) return 0; o->subentry = n; o->have_subentry = 1; }
            else if (!strcmp(arg, "--model")) { o->model = n; o->have_model = 1; }
            else if (!strcmp(arg, "--section")) { if (n > 255) return 0; o->section = n; o->have_section = 1; }
            else if (!strcmp(arg, "--texture-entry")) { o->texture_entry = n; o->have_texture_entry = 1; }
            else if (!strcmp(arg, "--environment-entry")) { o->environment_entry = n; o->have_environment_entry = 1; }
            else if (!strcmp(arg, "--environment-subentry")) {
                if (n > 31) return 0;
                o->environment_subentry = n; o->have_environment_subentry = 1;
            } else if (!strcmp(arg, "--environment-section")) {
                if (n > 255) return 0;
                o->environment_section = n; o->have_environment_section = 1;
            } else if (!strcmp(arg, "--scene")) { o->scene = n; o->have_scene = 1; }
            else if (!strcmp(arg, "--texture-subentry")) {
                if (n > 31) return 0;
                o->texture_subentry = n; o->have_texture_subentry = 1;
            } else if (!strcmp(arg, "--texture-section")) {
                if (n > 255) return 0;
                o->texture_section = n; o->have_texture_section = 1;
            }
            else { if (!n) return 0; o->frames = n; }
        }
    }
    if (o->demo) {
        if (o->romfs || o->archive || o->have_entry || o->have_subentry || o->have_model ||
            o->have_section || o->triangle_list) return 0;
    } else if (!o->romfs || !o->archive || !o->have_entry || !relative_path(o->archive)) return 0;
    const int texture_options = o->texture_archive || o->have_texture_entry ||
        o->have_texture_subentry || o->have_texture_section;
    if (o->combiners && !texture_options) return 0;
    if (texture_options && (o->demo || !o->textured || !o->triangle_list ||
        !o->texture_archive || !o->have_texture_entry ||
        !relative_path(o->texture_archive))) return 0;
    const int environment_options = o->environment_archive || o->have_environment_entry ||
        o->have_environment_subentry || o->have_environment_section || o->have_scene || o->lighting;
    if (environment_options && (!o->combiners || !o->environment_archive ||
        !o->have_environment_entry || !relative_path(o->environment_archive))) return 0;
    if (o->environment_archive && !o->lighting) o->lighting = 1;
    if (o->inspect && (o->capture || o->frames || o->self_test)) return 0;
    if (o->self_test && !o->frames) o->frames = 3;
    return 1;
}

static int load_images(const uint8_t *data, size_t size, struct model_asset *asset)
{
    struct px_cgfx *container = NULL;
    enum px_cgfx_result result = px_cgfx_open(data, size, &container);
    if (result != PX_CGFX_OK) goto failed;
    asset->texture_count = (uint32_t)px_cgfx_texture_count(container);
    asset->textures = calloc(asset->texture_count, sizeof *asset->textures);
    if (!asset->textures) { result = PX_CGFX_NO_MEMORY; goto failed; }
    uint64_t bytes = 0;
    for (uint32_t i = 0; i < asset->scene.material_count; ++i) {
        uint32_t selected = asset->scene.materials[i].texture_index;
        const struct px_cgfx_texture *texture = px_cgfx_texture(container, selected);
        if (!texture) { result = PX_CGFX_NOT_FOUND; goto failed; }
        if (asset->textures[selected].rgba) continue;
        result = texture->decode_status;
        if (result != PX_CGFX_OK) goto failed;
        bytes += (uint64_t)texture->width * texture->height * 4;
        if (bytes > 64u * 1024u * 1024u) { result = PX_CGFX_LIMIT_EXCEEDED; goto failed; }
        result = px_cgfx_decode_texture(container, selected, &asset->textures[selected]);
        if (result != PX_CGFX_OK) goto failed;
    }
    px_cgfx_close(container);
    return 1;
failed:
    fprintf(stderr, "[model-viewer] CGFX textures: %s\n", px_cgfx_result_string(result));
    px_cgfx_close(container);
    return 0;
}

static int demo_texture(struct model_asset *asset)
{
    struct px_cgfx_scene *s = &asset->scene;
    static const float uvs[] = {0,0, 1,0, 0.5f,1, 0.5f,0.5f};
    s->uvs = malloc(sizeof uvs);
    s->colors = malloc(16 * sizeof *s->colors);
    s->draws = calloc(1, sizeof *s->draws);
    s->materials = calloc(1, sizeof *s->materials);
    asset->textures = calloc(1, sizeof *asset->textures);
    if (!s->uvs || !s->colors || !s->draws || !s->materials || !asset->textures) return 0;
    s->draw_count = s->material_count = asset->texture_count = 1;
    memcpy(s->uvs, uvs, sizeof uvs);
    for (unsigned i = 0; i < 16; ++i) s->colors[i] = 1;
    s->draws[0].index_count = 12; s->draws[0].visible = 1;
    struct px_cgfx_material *m = &s->materials[0];
    strcpy(m->name, "synthetic checker");
    m->wrap_u = m->wrap_v = 2; m->depth_test = m->depth_write = 1;
    m->depth_compare = 4; m->alpha_compare = 1; m->cull_mode = 3;
    m->color_write_mask = 15;
    m->blend_src_rgb = m->blend_src_alpha = 1;
    m->uv_scale[0] = m->uv_scale[1] = 1;
    m->uv_transform[0] = m->uv_transform[5] = m->uv_transform[10] = 1;
    struct px_texture_image *image = asset->textures;
    image->width = image->height = 8; image->format = PX_TEXTURE_RGBA8;
    image->rgba = malloc(8 * 8 * 4);
    if (!image->rgba) return 0;
    for (unsigned y = 0; y < 8; ++y) for (unsigned x = 0; x < 8; ++x) {
        const uint8_t a[4] = {244, 173, 65, 255}, b[4] = {65, 164, 214, 255};
        memcpy(image->rgba + (y * 8 + x) * 4, ((x / 2 + y / 2) & 1) ? a : b, 4);
    }
    return 1;
}

/* One GARC member after optional outer LZ, optional AD/GR/BM section selection
 * and optional inner LZ. Each resource has its own 64 MiB aggregate budget. */
struct resource {
    uint8_t *member, *decoded, *section_decoded;
    const uint8_t *data;
    size_t size, member_size, decoded_size;
    const char *compression, *section_compression;
};

static void resource_free(struct resource *r)
{
    free(r->section_decoded); free(r->decoded); free(r->member);
    memset(r, 0, sizeof *r);
}

static int read_resource(const char *romfs, const char *archive_path, uint32_t entry,
                         uint32_t subentry, int have_section, uint32_t section,
                         const char *label, struct resource *out)
{
    memset(out, 0, sizeof *out);
    out->compression = out->section_compression = "none";
    size_t a = strlen(romfs), b = strlen(archive_path);
    if (a > SIZE_MAX - b - 2) return 0;
    char *path = malloc(a + b + 2);
    if (!path) return 0;
    memcpy(path, romfs, a); path[a] = '/'; memcpy(path + a + 1, archive_path, b + 1);
    char error[256]; struct px_garc *archive = NULL;
    int ok = px_garc_open(path, &archive, error, sizeof error);
    free(path);
    if (!ok) { fprintf(stderr, "[model-viewer] %s archive: %s\n", label, error); return 0; }
    ok = px_garc_read(archive, entry, subentry, 64 * 1024 * 1024,
                      &out->member, &out->member_size, error, sizeof error);
    px_garc_close(archive);
    if (!ok) { fprintf(stderr, "[model-viewer] %s member: %s\n", label, error); return 0; }
    const size_t size = out->member_size;
    out->data = out->member;
    if (size && (out->member[0] == 0x10 || out->member[0] == 0x11)) {
        out->compression = out->member[0] == 0x10 ? "LZ10" : "LZ11";
        const size_t limit = have_section ? 64 * 1024 * 1024 - size : 64 * 1024 * 1024;
        if (!limit || !px_lz_decode(out->member, size, limit, &out->decoded, &out->decoded_size,
                                    error, sizeof error)) {
            if (!limit) snprintf(error, sizeof error, "aggregate model input budget exceeded");
            goto failed;
        }
        out->data = out->decoded;
    }
    out->size = out->decoded ? out->decoded_size : size;
    if (have_section) {
        if (out->size < 2 || (memcmp(out->data, "AD", 2) && memcmp(out->data, "GR", 2) &&
                              memcmp(out->data, "BM", 2))) {
            snprintf(error, sizeof error, "--section requires an AD, GR or BM minipack");
            goto failed;
        }
        struct px_field_span selected;
        enum px_field_result status = px_field_section(out->data, out->size,
            (const char *)out->data, (uint16_t)section, &selected);
        if (status != PX_FIELD_OK) {
            snprintf(error, sizeof error, "minipack section: %s", px_field_result_string(status));
            goto failed;
        }
        out->data = selected.data;
        out->size = selected.size;
        if (out->size && (out->data[0] == 0x10 || out->data[0] == 0x11)) {
            out->section_compression = out->data[0] == 0x10 ? "LZ10" : "LZ11";
            size_t section_size = 0;
            const size_t used = size + out->decoded_size;
            if (used >= 64 * 1024 * 1024 ||
                !px_lz_decode(out->data, out->size, 64 * 1024 * 1024 - used,
                              &out->section_decoded, &section_size, error, sizeof error)) {
                if (used >= 64 * 1024 * 1024) snprintf(error, sizeof error, "aggregate model input budget exceeded");
                goto failed;
            }
            out->data = out->section_decoded;
            out->size = section_size;
        }
    }
    return 1;
failed:
    fprintf(stderr, "[model-viewer] %s %s\n", label, error);
    resource_free(out);
    return 0;
}

static void print_resource(const char *prefix, const char *archive, uint32_t entry,
                           uint32_t subentry, uint32_t model, int have_section,
                           uint32_t section, const struct resource *r)
{
    printf("%sarchive=%s %sentry=%" PRIu32 " %ssubentry=%" PRIu32, prefix, archive,
           prefix, entry, prefix, subentry);
    if (!*prefix) printf(" model=%" PRIu32, model);
    printf(" %scompression=%s", prefix, r->compression);
    if (have_section) printf(" %ssection=%" PRIu32 " %ssection_compression=%s",
                             prefix, section, prefix, r->section_compression);
    putchar('\n');
}

/* Units whose images a material needs: the stage-0 unit, or every sampled
 * unit of the full combiner chain. */
static uint32_t wanted_units(const struct model_asset *asset, uint32_t material)
{
    return asset->bch_combiner ? asset->bch_combiner[material].sampled_units
                               : UINT32_C(1) << asset->bch_preview[material].unit;
}

static int prepare_bch_materials(struct model_asset *asset, int combiners)
{
    const struct px_bch_model *bch = &asset->bch;
    asset->bch_preview = calloc(bch->material_count, sizeof *asset->bch_preview);
    asset->bch_texture = malloc((size_t)bch->material_count * 3 * sizeof *asset->bch_texture);
    if (combiners) asset->bch_combiner = calloc(bch->material_count, sizeof *asset->bch_combiner);
    if (!asset->bch_preview || !asset->bch_texture || (combiners && !asset->bch_combiner)) return 0;
    for (size_t i = 0; i < (size_t)bch->material_count * 3; ++i) asset->bch_texture[i] = UINT32_MAX;
    for (uint32_t i = 0; i < bch->material_count; ++i) {
        const enum px_bch_material_result status = combiners ?
            px_bch_material_combiners(&bch->materials[i], &asset->bch_combiner[i]) :
            px_bch_material_preview(&bch->materials[i], &asset->bch_preview[i]);
        if (status != PX_BCH_MATERIAL_OK) {
            fprintf(stderr, "[model-viewer] BCH material %" PRIu32 ": %s\n", i,
                    px_bch_material_result_string(status));
            return 0;
        }
        if (!combiners && asset->bch_preview[i].mag_linear != asset->bch_preview[i].min_linear) {
            fprintf(stderr, "[model-viewer] BCH material %" PRIu32 ": distinct min/mag filters are unsupported\n", i);
            return 0;
        }
        for (unsigned unit = 0; combiners && unit < 3; ++unit)
            if (asset->bch_combiner[i].mag_linear[unit] != asset->bch_combiner[i].min_linear[unit]) {
                fprintf(stderr, "[model-viewer] BCH material %" PRIu32 ": distinct min/mag filters are unsupported\n", i);
                return 0;
            }
    }
    /* Preview policy: ascending mesh layer, then priority; equal keys keep
     * resource order. This is not recovered game draw sorting. */
    asset->draw_order = malloc((size_t)bch->mesh_count * sizeof *asset->draw_order);
    if (!asset->draw_order) return 0;
    for (uint32_t i = 0; i < bch->mesh_count; ++i) {
        uint32_t j = i;
        const struct px_bch_mesh *next = &bch->meshes[i];
        while (j) {
            const struct px_bch_mesh *previous = &bch->meshes[asset->draw_order[j - 1]];
            if (previous->layer < next->layer ||
                (previous->layer == next->layer && previous->priority <= next->priority)) break;
            asset->draw_order[j] = asset->draw_order[j - 1]; --j;
        }
        asset->draw_order[j] = i;
    }
    asset->bch_textured = 1;
    return 1;
}

/* Resolve each material's stage-0 texture name against the external BCH by
 * exact bytes. A name occurring more than once there is ambiguous and fails.
 * Only referenced textures are decoded; their RGBA total is capped at 64 MiB. */
static int bind_bch_textures(const uint8_t *data, size_t size, struct model_asset *asset)
{
    struct px_bch *pack = NULL;
    enum px_bch_result result = px_bch_open(data, size, &pack);
    if (result != PX_BCH_OK) goto failed;
    const size_t count = px_bch_texture_count(pack);
    if (count > UINT32_MAX) { result = PX_BCH_LIMIT_EXCEEDED; goto failed; }
    asset->texture_count = (uint32_t)count;
    asset->textures = calloc(count ? count : 1, sizeof *asset->textures);
    if (!asset->textures) { result = PX_BCH_NO_MEMORY; goto failed; }
    uint64_t bytes = 0;
    const struct px_bch_model *bch = &asset->bch;
    for (uint32_t i = 0; i < bch->material_count; ++i)
    for (unsigned unit = 0; unit < 3; ++unit) {
        if (!((wanted_units(asset, i) >> unit) & 1)) continue;
        const struct px_bch_name *wanted = &bch->materials[i].texture_names[unit];
        size_t found = SIZE_MAX;
        for (size_t j = 0; j < count; ++j) {
            const struct px_bch_texture *t = px_bch_texture(pack, j);
            if (t->name_size != wanted->size || memcmp(t->name, wanted->bytes, wanted->size)) continue;
            if (found != SIZE_MAX) {
                fprintf(stderr, "[model-viewer] ambiguous external texture name for material %" PRIu32 "\n", i);
                px_bch_close(pack); return 0;
            }
            found = j;
        }
        if (found == SIZE_MAX) { result = PX_BCH_NOT_FOUND; goto failed; }
        asset->bch_texture[(size_t)i * 3 + unit] = (uint32_t)found;
        if (asset->textures[found].rgba) continue;
        const struct px_bch_texture *t = px_bch_texture(pack, found);
        result = t->decode_status;
        if (result != PX_BCH_OK) goto failed;
        bytes += (uint64_t)t->width * t->height * 4;
        if (bytes > 64u * 1024u * 1024u) { result = PX_BCH_LIMIT_EXCEEDED; goto failed; }
        result = px_bch_decode_texture(pack, found, &asset->textures[found]);
        if (result != PX_BCH_OK) goto failed;
    }
    px_bch_close(pack);
    return 1;
failed:
    fprintf(stderr, "[model-viewer] BCH textures: %s\n", px_bch_result_string(result));
    px_bch_close(pack);
    return 0;
}

static int name_is(const struct px_bch_name *name, const char *text)
{
    return name->present && name->size == strlen(text) && !memcmp(name->bytes, text, name->size);
}

/* Declared --lighting policy, from the runtime light upload (code.bin
 * 0x2df3f0, 0x3030a8) and Azahar ComputeFragmentsColors: per material, the
 * enabled lights of its light set. Directional lights execute with light
 * vector -direction (the runtime negates the view-space direction; N.L is
 * evaluated in model space, identical up to the untraced float16 rounding of
 * the view-space vector) and register colors light x material per channel.
 * Global ambient is the material emission: no ambient light is bound. Point
 * and spot lights are not executed; their largest ambient+diffuse register
 * sum is reported as a bound. Day omits light_night, night omits light_sun:
 * the file enables both, and the runtime selection is not traced. */
static int prepare_bch_lighting(struct model_asset *asset, int lighting)
{
    const struct px_bch_model *bch = &asset->bch;
    const struct px_bch_environment *env = &asset->environment;
    asset->bch_light = calloc(bch->material_count, sizeof *asset->bch_light);
    asset->point_bound = calloc((size_t)bch->material_count * 3, sizeof *asset->point_bound);
    asset->light_index = calloc((size_t)bch->material_count * 8, sizeof *asset->light_index);
    if (!asset->bch_light || !asset->point_bound || !asset->light_index) return 0;
    asset->lighting = lighting;
    for (uint32_t i = 0; i < bch->material_count; ++i) {
        const struct px_bch_material *m = &bch->materials[i];
        if (!m->parameters_present || !(m->material_flags & 1) || m->fresnel_selector || m->bump_mode) {
            fprintf(stderr, "[model-viewer] Material %" PRIu32 " uses unsupported fragment lighting state.\n", i);
            return 0;
        }
        const struct px_bch_light_set *set = px_bch_environment_set(env, m->light_set_index);
        if (!set) {
            fprintf(stderr, "[model-viewer] Material %" PRIu32 " light set %" PRIu32 " is absent from the scene.\n",
                    i, m->light_set_index);
            return 0;
        }
        struct px_light_setup *setup = &asset->bch_light[i];
        for (unsigned c = 0; c < 3; ++c) setup->global_ambient[c] = m->colors[0][c];
        for (uint32_t k = 0; k < set->count; ++k) {
            const uint32_t index = env->set_lights[set->first + k];
            const struct px_bch_light *light = &env->lights[index];
            if (!light->enabled) continue;
            if (light->type == PX_BCH_LIGHT_FRAGMENT_POINT || light->type == PX_BCH_LIGHT_FRAGMENT_SPOT) {
                for (unsigned c = 0; c < 3; ++c)
                    asset->point_bound[(size_t)i * 3 + c] +=
                        px_light_register_channel(light->ambient[c], m->colors[1][c]) +
                        px_light_register_channel(light->diffuse[c], m->colors[2][c]);
                continue;
            }
            if (light->type != PX_BCH_LIGHT_FRAGMENT_DIRECTIONAL || (light->flags & 2)) {
                fprintf(stderr, "[model-viewer] Light set %" PRIu32 " uses an unsupported light.\n", set->index);
                return 0;
            }
            if ((lighting == 1 && name_is(&light->name, "light_night")) ||
                (lighting == 2 && name_is(&light->name, "light_sun"))) continue;
            if (setup->count == 8) return 0;
            struct px_light_source *source = &setup->lights[setup->count];
            asset->light_index[(size_t)i * 8 + setup->count++] = index;
            for (unsigned c = 0; c < 3; ++c) {
                source->ambient[c] = px_light_register_channel(light->ambient[c], m->colors[1][c]);
                source->diffuse[c] = px_light_register_channel(light->diffuse[c], m->colors[2][c]);
                source->vector[c] = -light->direction[c] + 0.0f; /* no negative zero */
            }
            source->two_sided = light->flags & 1;
            const float probe[3] = {0, 0, 1};
            uint8_t ignored[4];
            if (!px_light_primary(setup, probe, ignored)) return 0;
        }
    }
    for (uint32_t i = 0; i < bch->mesh_count; ++i)
        if (!bch->meshes[i].normal_source) {
            fprintf(stderr, "[model-viewer] Mesh %" PRIu32 " has no normal for fragment lighting.\n", i);
            return 0;
        }
    return 1;
}

static int decode_model(const struct options *o, const uint8_t *data, size_t size,
                        struct model_asset *asset)
{
    if (size >= 4 && !memcmp(data, "BCH\0", 4)) {
        if (!o->triangle_list || (o->textured && !o->texture_archive)) {
            fprintf(stderr, "[model-viewer] BCH requires --triangle-list for an explicit index-triple interpretation; "
                    "BCH --textured also requires --texture-archive and --texture-entry.\n");
            return 0;
        }
        struct px_bch_model *bch = &asset->bch;
        enum px_bch_model_result status = px_bch_model_decode(data, size, o->model, bch);
        if (status != PX_BCH_MODEL_OK) {
            fprintf(stderr, "[model-viewer] BCH model: %s\n", px_bch_model_result_string(status));
            return 0;
        }
        for (uint32_t i = 0; i < bch->submesh_count; ++i) {
            const struct px_bch_submesh *sub = &bch->submeshes[i];
            if (sub->index_count % 3 || sub->draw_count > 1 ||
                (sub->draw_primitive_value & sub->draw_primitive_known_mask & UINT32_C(0x300))) {
                fprintf(stderr, "[model-viewer] BCH submesh cannot use the restricted triangle-list interpretation.\n");
                return 0;
            }
        }
        /* Transfer the arrays to the existing wireframe renderer. BCH metadata
         * and UVs remain owned by bch; neither object borrows the archive. */
        struct px_cgfx_model *mesh = &asset->wire;
        mesh->positions = bch->positions; bch->positions = NULL;
        mesh->indices = bch->indices; bch->indices = NULL;
        mesh->vertex_count = bch->vertex_count;
        mesh->index_count = bch->index_count;
        mesh->shape_count = bch->mesh_count;
        memcpy(mesh->bounds_min, bch->bounds_min, sizeof mesh->bounds_min);
        memcpy(mesh->bounds_max, bch->bounds_max, sizeof mesh->bounds_max);
        memcpy(mesh->name, bch->name.bytes, bch->name.size + 1);
        return !o->textured || prepare_bch_materials(asset, o->combiners);
    }
    if (o->texture_archive) {
        fprintf(stderr, "[model-viewer] --texture-archive requires BCH model input.\n");
        return 0;
    }
    if (o->triangle_list) {
        fprintf(stderr, "[model-viewer] --triangle-list requires BCH input.\n");
        return 0;
    }
    enum px_cgfx_model_result result = o->textured ?
        px_cgfx_model_decode_textured(data, size, o->model, &asset->scene) :
        px_cgfx_model_decode(data, size, o->model, &asset->wire);
    if (result != PX_CGFX_MODEL_OK) {
        fprintf(stderr, "[model-viewer] CGFX model: %s\n", px_cgfx_model_result_string(result));
        return 0;
    }
    return !o->textured || load_images(data, size, asset);
}

static int load(const struct options *o, struct model_asset *asset)
{
    struct px_cgfx_model *mesh = o->textured ? &asset->scene.mesh : &asset->wire;
    if (o->demo) {
        static const float positions[] = {-1,-1,-1, 1,-1,-1, 0,1,-1, 0,0,1};
        static const uint32_t indices[] = {0,1,2, 0,3,1, 1,3,2, 2,3,0};
        mesh->positions = malloc(sizeof positions); mesh->indices = malloc(sizeof indices);
        if (!mesh->positions || !mesh->indices) { px_cgfx_model_free(mesh); return 0; }
        memcpy(mesh->positions, positions, sizeof positions); memcpy(mesh->indices, indices, sizeof indices);
        mesh->vertex_count = 4; mesh->index_count = 12; mesh->shape_count = 1;
        strcpy(mesh->name, "synthetic tetrahedron");
        for (unsigned i = 0; i < 3; ++i) { mesh->bounds_min[i] = -1; mesh->bounds_max[i] = 1; }
        return !o->textured || demo_texture(asset);
    }
    struct resource model = {0};
    if (!read_resource(o->romfs, o->archive, o->entry, o->subentry, o->have_section,
                       o->section, "model", &model)) return 0;
    int ok = decode_model(o, model.data, model.size, asset);
    if (ok) print_resource("", o->archive, o->entry, o->subentry, o->model,
                           o->have_section, o->section, &model);
    resource_free(&model);
    if (ok && asset->bch_textured) {
        struct resource textures = {0};
        ok = read_resource(o->romfs, o->texture_archive, o->texture_entry, o->texture_subentry,
                           o->have_texture_section, o->texture_section, "texture", &textures) &&
             bind_bch_textures(textures.data, textures.size, asset);
        if (ok) print_resource("texture_", o->texture_archive, o->texture_entry,
                               o->texture_subentry, 0, o->have_texture_section,
                               o->texture_section, &textures);
        resource_free(&textures);
    }
    if (ok && o->environment_archive) {
        struct resource environment = {0};
        ok = read_resource(o->romfs, o->environment_archive, o->environment_entry,
                           o->environment_subentry, o->have_environment_section,
                           o->environment_section, "environment", &environment);
        if (ok) {
            const enum px_bch_model_result status = px_bch_environment_decode(
                environment.data, environment.size, o->scene, &asset->environment);
            if (status != PX_BCH_MODEL_OK) {
                fprintf(stderr, "[model-viewer] BCH environment: %s\n", px_bch_model_result_string(status));
                ok = 0;
            }
        }
        if (ok) ok = prepare_bch_lighting(asset, o->lighting);
        if (ok) print_resource("environment_", o->environment_archive, o->environment_entry,
                               o->environment_subentry, o->scene, o->have_environment_section,
                               o->environment_section, &environment);
        resource_free(&environment);
    }
    return ok;
}

static void reset(struct camera *c) { c->yaw = 0.55; c->pitch = -0.35; c->zoom = 1; c->running = 1; }
static void event(struct camera *c, const SDL_Event *e)
{
    if (e->type == SDL_QUIT) c->running = 0;
    if (e->type != SDL_KEYDOWN) return;
    switch (e->key.keysym.sym) {
    case SDLK_ESCAPE: c->running = 0; break;
    case SDLK_LEFT: c->yaw -= 0.1; break;
    case SDLK_RIGHT: c->yaw += 0.1; break;
    case SDLK_UP: c->pitch -= 0.1; break;
    case SDLK_DOWN: c->pitch += 0.1; break;
    case SDLK_PLUS: case SDLK_EQUALS: case SDLK_KP_PLUS: c->zoom *= 1.25; break;
    case SDLK_MINUS: case SDLK_KP_MINUS: c->zoom /= 1.25; break;
    case SDLK_r: case SDLK_HOME: reset(c); break;
    default: break;
    }
    c->yaw = fmod(c->yaw, 6.283185307179586);
    if (c->pitch < -1.5) c->pitch = -1.5;
    if (c->pitch > 1.5) c->pitch = 1.5;
    if (c->zoom < 0.03125) c->zoom = 0.03125;
    if (c->zoom > 32) c->zoom = 32;
}
static int inject(struct camera *c, SDL_Keycode key)
{
    SDL_Event e; memset(&e, 0, sizeof e); e.type = SDL_KEYDOWN; e.key.keysym.sym = key;
    if (SDL_PushEvent(&e) != 1) return 0;
    while (SDL_PollEvent(&e)) event(c, &e);
    return 1;
}
static int input_test(struct camera *c)
{
    SDL_Event e; while (SDL_PollEvent(&e)) {}
    reset(c);
    if (!inject(c, SDLK_RIGHT) || c->yaw <= 0.55 || !inject(c, SDLK_UP) || c->pitch >= -0.35 ||
        !inject(c, SDLK_PLUS) || c->zoom <= 1 || !inject(c, SDLK_HOME) ||
        c->yaw != 0.55 || c->pitch != -0.35 || c->zoom != 1 ||
        !inject(c, SDLK_ESCAPE) || c->running) return 0;
    reset(c); fprintf(stderr, "[model-viewer] Input self-test passed: orbit, zoom, reset, Escape.\n");
    return 1;
}

static int capture(SDL_Renderer *renderer, const char *path, int width, int height)
{
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) return 0;
    int ok = SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch) == 0;
    if (ok) {
        FILE *file = fopen(path, "wbx");
        if (!file) { SDL_SetError("Cannot create new export: %s", strerror(errno)); ok = 0; }
        else {
            SDL_RWops *stream = SDL_RWFromFP(file, SDL_TRUE);
            if (!stream) { fclose(file); ok = 0; }
            else ok = SDL_SaveBMP_RW(surface, stream, 1) == 0;
        }
    }
    SDL_FreeSurface(surface); return ok;
}

static int draw(SDL_Renderer *renderer, const struct px_cgfx_model *mesh,
                const struct camera *c, SDL_FPoint *points, const char *path)
{
    int w, h;
    if (SDL_GetRendererOutputSize(renderer, &w, &h) != 0 || w <= 0 || h <= 0) return 0;
    if (SDL_SetRenderDrawColor(renderer, 22, 27, 36, 255) || SDL_RenderClear(renderer)) return 0;
    double center[3], radius = 0;
    for (unsigned j = 0; j < 3; ++j) {
        center[j] = ((double)mesh->bounds_min[j] + mesh->bounds_max[j]) / 2;
        double half = ((double)mesh->bounds_max[j] - mesh->bounds_min[j]) / 2;
        radius += half * half;
    }
    radius = sqrt(radius); if (radius < 1e-20) radius = 1;
    const double scale = (w < h ? w : h) * 0.42 * c->zoom;
    const double cy = cos(c->yaw), sy = sin(c->yaw), cp = cos(c->pitch), sp = sin(c->pitch);
    for (uint32_t i = 0; i < mesh->vertex_count; ++i) {
        double x = (mesh->positions[3 * (size_t)i] - center[0]) / radius;
        double y = (mesh->positions[3 * (size_t)i + 1] - center[1]) / radius;
        double z = (mesh->positions[3 * (size_t)i + 2] - center[2]) / radius;
        double rx = cy * x + sy * z, rz = -sy * x + cy * z;
        points[i].x = (float)(w * 0.5 + rx * scale);
        points[i].y = (float)(h * 0.5 - (cp * y - sp * rz) * scale);
    }
    if (SDL_SetRenderDrawColor(renderer, 126, 211, 230, 255)) return 0;
    for (uint32_t i = 0; i < mesh->index_count; i += 3) {
        SDL_FPoint triangle[4] = {points[mesh->indices[i]], points[mesh->indices[i + 1]],
                                 points[mesh->indices[i + 2]], points[mesh->indices[i]]};
        if (SDL_RenderDrawLinesF(renderer, triangle, 4)) return 0;
    }
    if (path && !capture(renderer, path, w, h)) return 0;
    SDL_RenderPresent(renderer); return 1;
}

static int material_state(const struct px_cgfx_material *m, struct px_raster_sampler *sampler,
                          struct px_raster_state *state)
{
    static const enum px_raster_compare compare[] = {
        PX_RASTER_NEVER, PX_RASTER_ALWAYS, PX_RASTER_EQUAL, PX_RASTER_NOTEQUAL,
        PX_RASTER_LESS, PX_RASTER_LEQUAL, PX_RASTER_GREATER, PX_RASTER_GEQUAL
    };
    static const enum px_raster_wrap wrap[] = {
        PX_RASTER_CLAMP, PX_RASTER_CLAMP, PX_RASTER_REPEAT, PX_RASTER_MIRROR
    };
    if (m->wrap_u > 3 || m->wrap_v > 3 || m->wrap_u == 1 || m->wrap_v == 1 ||
        m->min_filter != m->mag_filter || m->min_filter > 1 ||
        m->alpha_compare > 7 || m->depth_compare > 7 || m->cull_mode > 3 ||
        m->color_write_mask > 15) return 0;
    sampler->wrap_u = wrap[m->wrap_u]; sampler->wrap_v = wrap[m->wrap_v];
    sampler->filter = m->min_filter ? PX_RASTER_LINEAR : PX_RASTER_NEAREST;
    *state = px_raster_default_state();
    state->depth_test = (int)m->depth_test; state->depth_write = (int)m->depth_write;
    state->depth_compare = compare[m->depth_compare];
    state->alpha_compare = m->alpha_test ? compare[m->alpha_compare] : PX_RASTER_ALWAYS;
    state->alpha_reference = (uint8_t)m->alpha_reference;
    state->front = PX_RASTER_CCW;
    state->color_write_mask = (uint8_t)m->color_write_mask;
    state->cull = m->cull_mode == 0 ? PX_RASTER_CULL_FRONT :
                  m->cull_mode == 1 ? PX_RASTER_CULL_BACK : PX_RASTER_CULL_NONE;
    state->blend = PX_RASTER_REPLACE;
    if (m->blend_mode) {
        if (m->blend_equation_rgb || m->blend_equation_alpha) return 0;
        if (m->blend_src_rgb == 6 && m->blend_dst_rgb == 7 &&
            m->blend_src_alpha == 6 && m->blend_dst_alpha == 7)
            state->blend = PX_RASTER_SOURCE_ALPHA;
        else if (m->blend_src_rgb != 1 || m->blend_dst_rgb != 0 ||
                 m->blend_src_alpha != 1 || m->blend_dst_alpha != 0) return 0;
    }
    return 1;
}

static int draw_textured(SDL_Renderer *renderer, const struct model_asset *asset,
                         const struct camera *camera, struct px_raster_vertex *points,
                         struct px_raster_target *target, SDL_Texture **frame,
                         const char *path)
{
    const struct px_cgfx_scene *scene = &asset->scene;
    const struct px_cgfx_model *mesh = &scene->mesh;
    int w, h;
    if (SDL_GetRendererOutputSize(renderer, &w, &h) != 0 || w <= 0 || h <= 0) return 0;
    enum px_raster_result result;
    if (target->width != (uint32_t)w || target->height != (uint32_t)h) {
        px_raster_free(target);
        if (*frame) { SDL_DestroyTexture(*frame); *frame = NULL; }
        result = px_raster_create((uint32_t)w, (uint32_t)h, target);
        if (result != PX_RASTER_OK) goto raster_error;
        *frame = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                   SDL_TEXTUREACCESS_STREAMING, w, h);
        if (!*frame || SDL_SetTextureBlendMode(*frame, SDL_BLENDMODE_NONE)) return 0;
    }
    const uint8_t background[4] = {22, 27, 36, 255};
    result = px_raster_clear(target, background, INFINITY);
    if (result != PX_RASTER_OK) goto raster_error;
    double center[3], radius = 0;
    for (unsigned j = 0; j < 3; ++j) {
        center[j] = ((double)mesh->bounds_min[j] + mesh->bounds_max[j]) / 2;
        double half = ((double)mesh->bounds_max[j] - mesh->bounds_min[j]) / 2;
        radius += half * half;
    }
    radius = sqrt(radius); if (radius < 1e-20) radius = 1;
    double scale = (w < h ? w : h) * 0.42 * camera->zoom;
    double cy = cos(camera->yaw), sy = sin(camera->yaw);
    double cp = cos(camera->pitch), sp = sin(camera->pitch);
    for (uint32_t i = 0; i < mesh->vertex_count; ++i) {
        double x = (mesh->positions[3 * (size_t)i] - center[0]) / radius;
        double y = (mesh->positions[3 * (size_t)i + 1] - center[1]) / radius;
        double z = (mesh->positions[3 * (size_t)i + 2] - center[2]) / radius;
        double rx = cy * x + sy * z, rz = -sy * x + cy * z;
        points[i].x = w * 0.5 + rx * scale;
        points[i].y = h * 0.5 - (cp * y - sp * rz) * scale;
        points[i].z = -(sp * y + cp * rz);
        points[i].u = scene->uvs[2 * (size_t)i];
        points[i].v = 1.0 - scene->uvs[2 * (size_t)i + 1];
        for (unsigned j = 0; j < 4; ++j) points[i].rgba[j] = scene->colors[4 * (size_t)i + j];
    }
    /* Preserve triangle order within each draw; material-layer ordering is
     * an explicit preview policy, not recovered game transparency sorting. */
    for (uint32_t i = 0; i < scene->draw_count; ++i) {
        const struct px_cgfx_draw *draw_call = &scene->draws[asset->draw_order[i]];
        const struct px_cgfx_material *material = &scene->materials[draw_call->material_index];
        if (!draw_call->visible || material->cull_mode == 2) continue;
        struct px_raster_sampler sampler; struct px_raster_state state;
        if (!material_state(material, &sampler, &state)) {
            SDL_SetError("Material uses unsupported preview state"); return 0;
        }
        const struct px_texture_image *image = &asset->textures[material->texture_index];
        const struct px_raster_image texture = {
            image->width, image->height, image->rgba, (size_t)image->width * image->height * 4
        };
        for (uint32_t j = 0; j < draw_call->index_count; j += 3) {
            size_t first = (size_t)draw_call->first_index + j;
            struct px_raster_vertex triangle[3] = {
                points[mesh->indices[first]], points[mesh->indices[first + 1]], points[mesh->indices[first + 2]]
            };
            result = px_raster_draw(target, triangle, &texture, &sampler, &state);
            if (result != PX_RASTER_OK) goto raster_error;
        }
    }
    if (SDL_UpdateTexture(*frame, NULL, target->rgba, w * 4) ||
        SDL_RenderClear(renderer) || SDL_RenderCopy(renderer, *frame, NULL, NULL)) return 0;
    if (path && !capture(renderer, path, w, h)) return 0;
    SDL_RenderPresent(renderer); return 1;
raster_error:
    SDL_SetError("Rasterizer: %s", px_raster_result_string(result)); return 0;
}

static int bch_state(const struct px_bch_preview_material *m, struct px_raster_sampler *sampler,
                     struct px_raster_state *state)
{
    static const enum px_raster_compare compare[] = {
        PX_RASTER_NEVER, PX_RASTER_ALWAYS, PX_RASTER_EQUAL, PX_RASTER_NOTEQUAL,
        PX_RASTER_LESS, PX_RASTER_LEQUAL, PX_RASTER_GREATER, PX_RASTER_GEQUAL
    };
    static const enum px_raster_wrap wrap[] = {
        PX_RASTER_CLAMP, PX_RASTER_CLAMP, PX_RASTER_REPEAT, PX_RASTER_MIRROR
    };
    static const enum px_raster_blend blend[] = {
        PX_RASTER_REPLACE, PX_RASTER_SOURCE_ALPHA, PX_RASTER_ADD_SOURCE_ALPHA
    };
    if (m->wrap_u > 3 || m->wrap_v > 3 || m->alpha_compare > 7 || m->depth_compare > 7 ||
        (unsigned)m->blend > 2 || m->color_write_mask > 15) return 0;
    sampler->wrap_u = wrap[m->wrap_u]; sampler->wrap_v = wrap[m->wrap_v];
    sampler->filter = m->mag_linear ? PX_RASTER_LINEAR : PX_RASTER_NEAREST;
    *state = px_raster_default_state();
    /* The material's depth comparison is applied in this preview's own
     * lower-z-is-nearer convention; PICA depth-map registers are not run. */
    state->depth_test = (int)m->depth_test;
    state->depth_compare = m->depth_test ? compare[m->depth_compare] : PX_RASTER_ALWAYS;
    state->depth_write = (int)m->depth_write;
    state->alpha_compare = m->alpha_test ? compare[m->alpha_compare] : PX_RASTER_ALWAYS;
    state->alpha_reference = (uint8_t)m->alpha_reference;
    state->cull = PX_RASTER_CULL_NONE; /* register 0x40 is reported, not executed */
    state->front = PX_RASTER_CCW;
    state->blend = blend[m->blend];
    state->color_write_mask = (uint8_t)m->color_write_mask;
    return 1;
}

/* Declared --combiners input policy. Primary color: the decoded vertex
 * color (stored or fixed attribute, clamped), because the vertex shader is
 * not executed. Fragment lighting is not executed: fragment primary is white
 * and fragment secondary black. Projection-mapped units are not executed: a
 * white texel substitutes. Ordinary units sample UV0 with their sampler. */
struct combiner_counts {
    uint64_t fragments, projected_sensitive, secondary_sensitive, point_sensitive, degenerate_normals;
    uint32_t point_max_delta; /* largest final-channel change at the bound */
};
struct combiner_shader {
    const struct px_bch_combiner_material *material;
    const struct px_light_setup *light; /* NULL: fragment primary white */
    const uint32_t *point_bound; /* 3 values, or NULL */
    struct px_raster_image images[3];
    struct px_raster_sampler samplers[3];
    struct combiner_counts *counts; /* NULL except on a counted frame */
};

static uint8_t quantize(double value)
{
    return (uint8_t)floor((value < 0 ? 0 : value > 1 ? 1 : value) * 255.0 + 0.5);
}

static void combiner_fragment(void *context, double u, double v, const double rgba[4],
                              const double varying[4], double out[4])
{
    const struct combiner_shader *s = context;
    const struct px_bch_combiner_material *m = s->material;
    struct px_tev_inputs in;
    int degenerate = 0;
    memset(&in, 0, sizeof in);
    for (unsigned c = 0; c < 4; ++c) {
        in.primary[c] = quantize(rgba[c]);
        in.fragment_primary[c] = 255;
    }
    if (s->light) {
        /* Interpolated model-space normal, renormalized by px_light_primary.
         * A zero interpolated normal gives a black fragment primary. */
        const float normal[3] = {(float)varying[0], (float)varying[1], (float)varying[2]};
        if (!px_light_primary(s->light, normal, in.fragment_primary)) {
            memset(in.fragment_primary, 0, 3);
            in.fragment_primary[3] = 255;
            degenerate = 1;
        }
    }
    for (unsigned unit = 0; unit < 3; ++unit) {
        if ((m->sampled_units >> unit) & 1) {
            double texel[4];
            px_raster_sample(&s->images[unit], &s->samplers[unit], u, v, texel);
            for (unsigned c = 0; c < 4; ++c) in.texture[unit][c] = quantize(texel[c]);
        } else if ((m->projected_units >> unit) & 1) {
            memset(in.texture[unit], 255, 4);
        }
    }
    uint8_t result[4];
    px_tev_evaluate(&m->tev, &in, result);
    for (unsigned c = 0; c < 4; ++c) out[c] = result[c] / 255.0;
    if (!s->counts) return;
    /* Sensitivity: does the final RGBA change when an unexecuted input takes
     * its other extreme? Counted over shaded fragments of one frame. */
    struct px_tev_inputs alternative = in;
    uint8_t other[4];
    ++s->counts->fragments;
    s->counts->degenerate_normals += degenerate;
    if (s->point_bound && (s->point_bound[0] || s->point_bound[1] || s->point_bound[2])) {
        /* Unexecuted point/spot lights add at most point_bound[c]/255 to
         * channel c before the clamp. Fragment primary is the sum truncated
         * once, so channel c rises by 0..point_bound[c]+1 levels. Every
         * combination is evaluated (TEV subtraction is not monotonic). */
        if (!s->light) return;
        uint32_t span[3], total = 1;
        for (unsigned c = 0; c < 3; ++c) {
            span[c] = s->point_bound[c] ? s->point_bound[c] + 2 : 1;
            total *= span[c];
            if (total > 4096) { s->counts->point_max_delta = UINT32_MAX; return; }
        }
        int changed = 0;
        for (uint32_t k = 0; k < total; ++k) {
            uint32_t rest = k;
            for (unsigned c = 0; c < 3; ++c) {
                const uint32_t raised = in.fragment_primary[c] + rest % span[c];
                rest /= span[c];
                alternative.fragment_primary[c] = (uint8_t)(raised > 255 ? 255 : raised);
            }
            px_tev_evaluate(&m->tev, &alternative, other);
            changed |= memcmp(result, other, 4) != 0;
            for (unsigned c = 0; c < 4; ++c) {
                const uint32_t delta = other[c] > result[c] ? other[c] - result[c] : result[c] - other[c];
                if (delta > s->counts->point_max_delta) s->counts->point_max_delta = delta;
            }
        }
        s->counts->point_sensitive += changed;
        alternative = in;
    }
    for (unsigned unit = 0; unit < 3; ++unit)
        if ((m->projected_units >> unit) & 1) memset(alternative.texture[unit], 0, 4);
    if (m->projected_units) {
        px_tev_evaluate(&m->tev, &alternative, other);
        if (memcmp(result, other, 4)) ++s->counts->projected_sensitive;
    }
    alternative = in;
    memset(alternative.fragment_secondary, 255, 4);
    px_tev_evaluate(&m->tev, &alternative, other);
    if (memcmp(result, other, 4)) ++s->counts->secondary_sensitive;
}

static int fragment_state(const struct px_bch_fragment_preview *m, struct px_raster_state *state)
{
    struct px_bch_preview_material flat;
    struct px_raster_sampler ignored;
    memset(&flat, 0, sizeof flat);
    flat.wrap_u = flat.wrap_v = 2;
    flat.alpha_test = m->alpha_test; flat.alpha_compare = m->alpha_compare;
    flat.alpha_reference = m->alpha_reference; flat.depth_test = m->depth_test;
    flat.depth_compare = m->depth_compare; flat.depth_write = m->depth_write;
    flat.color_write_mask = m->color_write_mask; flat.cull = m->cull; flat.blend = m->blend;
    return bch_state(&flat, &ignored, state);
}

static int draw_bch_textured(SDL_Renderer *renderer, const struct model_asset *asset,
                             const struct camera *camera, struct px_raster_vertex *points,
                             double (*normals)[4],
                             struct px_raster_target *target, SDL_Texture **frame,
                             const char *path, struct combiner_counts *counts)
{
    const struct px_cgfx_model *mesh = &asset->wire;
    const struct px_bch_model *bch = &asset->bch;
    int w, h;
    if (SDL_GetRendererOutputSize(renderer, &w, &h) != 0 || w <= 0 || h <= 0) return 0;
    enum px_raster_result result;
    if (target->width != (uint32_t)w || target->height != (uint32_t)h) {
        px_raster_free(target);
        if (*frame) { SDL_DestroyTexture(*frame); *frame = NULL; }
        result = px_raster_create((uint32_t)w, (uint32_t)h, target);
        if (result != PX_RASTER_OK) goto raster_error;
        *frame = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                   SDL_TEXTUREACCESS_STREAMING, w, h);
        if (!*frame || SDL_SetTextureBlendMode(*frame, SDL_BLENDMODE_NONE)) return 0;
    }
    const uint8_t background[4] = {22, 27, 36, 255};
    result = px_raster_clear(target, background, INFINITY);
    if (result != PX_RASTER_OK) goto raster_error;
    double center[3], radius = 0;
    for (unsigned j = 0; j < 3; ++j) {
        center[j] = ((double)mesh->bounds_min[j] + mesh->bounds_max[j]) / 2;
        double half = ((double)mesh->bounds_max[j] - mesh->bounds_min[j]) / 2;
        radius += half * half;
    }
    radius = sqrt(radius); if (radius < 1e-20) radius = 1;
    double scale = (w < h ? w : h) * 0.42 * camera->zoom;
    double cy = cos(camera->yaw), sy = sin(camera->yaw);
    double cp = cos(camera->pitch), sp = sin(camera->pitch);
    for (uint32_t i = 0; i < mesh->vertex_count; ++i) {
        double x = (mesh->positions[3 * (size_t)i] - center[0]) / radius;
        double y = (mesh->positions[3 * (size_t)i + 1] - center[1]) / radius;
        double z = (mesh->positions[3 * (size_t)i + 2] - center[2]) / radius;
        double rx = cy * x + sy * z, rz = -sy * x + cy * z;
        points[i].x = w * 0.5 + rx * scale;
        points[i].y = h * 0.5 - (cp * y - sp * rz) * scale;
        points[i].z = -(sp * y + cp * rz);
        /* Stored BCH UV0 has V up; decoded images are top-down. */
        points[i].u = bch->uvs[2 * (size_t)i];
        points[i].v = 1.0 - bch->uvs[2 * (size_t)i + 1];
        for (unsigned j = 0; j < 4; ++j)
            points[i].rgba[j] = asset->bch_combiner ? bch->colors[4 * (size_t)i + j] : 1;
        for (unsigned j = 0; normals && j < 4; ++j)
            normals[i][j] = j < 3 ? bch->normals[3 * (size_t)i + j] : 0;
    }
    for (uint32_t i = 0; i < bch->mesh_count; ++i) {
        const struct px_bch_mesh *m = &bch->meshes[asset->draw_order[i]];
        if (asset->bch_combiner) {
            struct combiner_shader shader;
            struct px_raster_state state;
            memset(&shader, 0, sizeof shader);
            shader.material = &asset->bch_combiner[m->material_index];
            shader.counts = counts ? &counts[m->material_index] : NULL;
            if (asset->bch_light) {
                shader.light = &asset->bch_light[m->material_index];
                shader.point_bound = &asset->point_bound[(size_t)m->material_index * 3];
            }
            static const enum px_raster_wrap wrap[] = {
                PX_RASTER_CLAMP, PX_RASTER_CLAMP, PX_RASTER_REPEAT, PX_RASTER_MIRROR
            };
            for (unsigned unit = 0; unit < 3; ++unit) {
                if (!((shader.material->sampled_units >> unit) & 1)) continue;
                const struct px_texture_image *image =
                    &asset->textures[asset->bch_texture[(size_t)m->material_index * 3 + unit]];
                shader.images[unit] = (struct px_raster_image){
                    image->width, image->height, image->rgba, (size_t)image->width * image->height * 4};
                shader.samplers[unit].wrap_u = wrap[shader.material->wrap_u[unit] & 3];
                shader.samplers[unit].wrap_v = wrap[shader.material->wrap_v[unit] & 3];
                shader.samplers[unit].filter = shader.material->mag_linear[unit] ?
                    PX_RASTER_LINEAR : PX_RASTER_NEAREST;
                if (!px_raster_image_valid(&shader.images[unit], &shader.samplers[unit])) {
                    SDL_SetError("Invalid bound texture"); return 0;
                }
            }
            if (!fragment_state(&shader.material->fragment, &state)) {
                SDL_SetError("Material uses unsupported preview state"); return 0;
            }
            for (uint32_t j = 0; j < m->index_count; j += 3) {
                size_t first = (size_t)m->first_index + j;
                struct px_raster_vertex triangle[3] = {
                    points[mesh->indices[first]], points[mesh->indices[first + 1]], points[mesh->indices[first + 2]]
                };
                double varyings[12];
                for (unsigned k = 0; k < 3; ++k)
                    memcpy(varyings + 4 * k, normals[mesh->indices[first + k]], 4 * sizeof *varyings);
                result = px_raster_draw_varying(target, triangle, varyings, combiner_fragment, &shader, &state);
                if (result != PX_RASTER_OK) goto raster_error;
            }
            continue;
        }
        const struct px_bch_preview_material *material = &asset->bch_preview[m->material_index];
        struct px_raster_sampler sampler; struct px_raster_state state;
        if (!bch_state(material, &sampler, &state)) {
            SDL_SetError("Material uses unsupported preview state"); return 0;
        }
        const struct px_texture_image *image =
            &asset->textures[asset->bch_texture[(size_t)m->material_index * 3 + material->unit]];
        const struct px_raster_image texture = {
            image->width, image->height, image->rgba, (size_t)image->width * image->height * 4
        };
        for (uint32_t j = 0; j < m->index_count; j += 3) {
            size_t first = (size_t)m->first_index + j;
            struct px_raster_vertex triangle[3] = {
                points[mesh->indices[first]], points[mesh->indices[first + 1]], points[mesh->indices[first + 2]]
            };
            result = px_raster_draw(target, triangle, &texture, &sampler, &state);
            if (result != PX_RASTER_OK) goto raster_error;
        }
    }
    if (SDL_UpdateTexture(*frame, NULL, target->rgba, w * 4) ||
        SDL_RenderClear(renderer) || SDL_RenderCopy(renderer, *frame, NULL, NULL)) return 0;
    if (path && !capture(renderer, path, w, h)) return 0;
    SDL_RenderPresent(renderer); return 1;
raster_error:
    SDL_SetError("Rasterizer: %s", px_raster_result_string(result)); return 0;
}

static int window(const struct options *o, const struct model_asset *asset)
{
    const struct px_cgfx_model *mesh = o->textured && !asset->bch_textured ?
        &asset->scene.mesh : &asset->wire;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) return 0;
    SDL_Window *win = SDL_CreateWindow(o->textured ?
        "Pokemon X unlit textured preview — arrows: orbit | +/-: zoom | R: reset | Esc: exit" :
        "Pokemon X wireframe — arrows: orbit | +/-: zoom | R: reset | Esc: exit",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 640, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = NULL; SDL_FPoint *points = NULL; int ok = 0;
    SDL_Texture *frame = NULL; struct px_raster_target target = {0};
    struct px_raster_vertex *vertices = NULL;
    double (*normals)[4] = NULL;
    struct combiner_counts *counts = NULL;
    if (!win) goto done;
    if (!o->software) renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer) goto done;
    if (o->textured) vertices = malloc((size_t)mesh->vertex_count * sizeof *vertices);
    else points = malloc((size_t)mesh->vertex_count * sizeof *points);
    if (o->textured ? !vertices : !points) goto done;
    if (asset->bch_combiner && !(normals = calloc(mesh->vertex_count, sizeof *normals))) goto done;
    struct camera c; reset(&c);
    if (o->self_test && !input_test(&c)) { SDL_SetError("Input self-test failed"); goto done; }
    SDL_RendererInfo info; if (SDL_GetRendererInfo(renderer, &info)) goto done;
    fprintf(stderr, "[model-viewer] Video=%s renderer=%s; orthographic %s.\n",
            SDL_GetCurrentVideoDriver(), info.name, o->textured ?
            asset->bch_light ? "BCH six-stage combiner software preview; directional fragment lighting; layer/priority/resource draw order" :
            asset->bch_combiner ? "BCH six-stage combiner software preview; declared unlit inputs; layer/priority/resource draw order" :
            asset->bch_textured ? "unlit BCH TEV-stage-0 software preview; layer/priority/resource draw order" :
            "unlit textured software preview; translucency/priority/resource draw order" : "wireframe");
    uint32_t frames = 0;
    if (asset->bch_combiner) {
        counts = calloc(asset->bch.material_count, sizeof *counts);
        if (!counts) goto done;
    }
    while (c.running) {
        SDL_Event e; while (SDL_PollEvent(&e)) event(&c, &e);
        if (!c.running) break;
        const char *path = frames == 0 ? o->capture : NULL;
        if (asset->bch_textured ? !draw_bch_textured(renderer, asset, &c, vertices, normals, &target, &frame, path,
                                                     frames == 0 ? counts : NULL) :
            o->textured ? !draw_textured(renderer, asset, &c, vertices, &target, &frame, path) :
                          !draw(renderer, mesh, &c, points, path)) goto done;
        if (frames == 0 && counts)
            for (uint32_t i = 0; i < asset->bch.material_count; ++i)
            {
                printf("combiner_sensitivity material=%" PRIu32 " frame=1 fragments=%" PRIu64
                       " projected_substitute_sensitive=%" PRIu64 " secondary_sensitive=%" PRIu64,
                       i, counts[i].fragments, counts[i].projected_sensitive, counts[i].secondary_sensitive);
                if (asset->bch_light)
                    printf(" point_bound_sensitive=%" PRIu64 " point_bound_max_delta=%" PRIu32
                           " degenerate_normals=%" PRIu64, counts[i].point_sensitive,
                           counts[i].point_max_delta, counts[i].degenerate_normals);
                putchar('\n');
            }
        if (++frames == o->frames) break;
        SDL_Delay(16);
    }
    fprintf(stderr, "[model-viewer] Exit after %" PRIu32 " frames.\n", frames); ok = 1;
done:
    if (!ok) fprintf(stderr, "[model-viewer] Runtime: %s\n", SDL_GetError());
    free(counts);
    free(normals);
    free(vertices); px_raster_free(&target); if (frame) SDL_DestroyTexture(frame);
    free(points); if (renderer) SDL_DestroyRenderer(renderer); if (win) SDL_DestroyWindow(win); SDL_Quit();
    return ok;
}

static void print_bch_name(const struct px_bch_name *name)
{
    if (!name->present) { printf("null"); return; }
    putchar('"');
    for (uint32_t i = 0; i < name->size; ++i) {
        const unsigned char byte = (unsigned char)name->bytes[i];
        if (byte >= 32 && byte <= 126 && byte != '"' && byte != '\\') putchar(byte);
        else printf("\\x%02x", byte);
    }
    putchar('"');
}

static void print_bch_metadata(const struct px_bch_model *bch, int textured)
{
    printf("container=BCH topology=assumed-triangle-list mode=%s visibility=all-meshes "
           "materials=%s meshes=%" PRIu32 " submeshes=%" PRIu32 " material_count=%" PRIu32 "\n",
           textured ? "textured" : "wireframe", textured ? "tev-stage0-unlit" : "metadata-only",
           bch->mesh_count, bch->submesh_count, bch->material_count);
    for (uint32_t i = 0; i < bch->mesh_count; ++i) {
        const struct px_bch_mesh *mesh = &bch->meshes[i];
        printf("mesh=%" PRIu32 " name=", i); print_bch_name(&mesh->name);
        printf(" first_vertex=%" PRIu32 " vertices=%" PRIu32 " first_index=%" PRIu32
               " indices=%" PRIu32 " material=%" PRIu32 " visible=%" PRIu32 " layer=%" PRIu32 " priority=%" PRIu32 "\n",
               mesh->first_vertex, mesh->vertex_count, mesh->first_index, mesh->index_count,
               mesh->material_index, mesh->visible, mesh->layer, mesh->priority);
    }
    for (uint32_t i = 0; i < bch->submesh_count; ++i) {
        const struct px_bch_submesh *sub = &bch->submeshes[i];
        printf("submesh=%" PRIu32 " first_index=%" PRIu32 " indices=%" PRIu32
               " index_width=%" PRIu32 " draw_count=%" PRIu32
               " primitive_at_draw=0x%08" PRIx32 " primitive_known_mask=0x%08" PRIx32 "\n",
               i, sub->first_index, sub->index_count, sub->index_width, sub->draw_count,
               sub->draw_primitive_value, sub->draw_primitive_known_mask);
    }
    for (uint32_t i = 0; i < bch->material_count; ++i) {
        const struct px_bch_material *material = &bch->materials[i];
        printf("material=%" PRIu32 " name=", i); print_bch_name(&material->name);
        printf(" texture_unit_config=0x%08" PRIx32, material->texture_unit_config);
        for (unsigned unit = 0; unit < 3; ++unit) {
            printf(" texture%u=", unit); print_bch_name(&material->texture_names[unit]);
        }
        putchar('\n');
    }
}

static void print_bch_bindings(const struct model_asset *asset)
{
    static const char *blend[] = {"replace", "source-alpha", "add-source-alpha"};
    const struct px_bch_model *bch = &asset->bch;
    uint32_t decoded = 0;
    for (uint32_t i = 0; i < asset->texture_count; ++i) decoded += asset->textures[i].rgba != NULL;
    if (asset->bch_combiner) {
        if (asset->bch_light)
            printf("mode=textured shading=tev-six-stage primary=vertex-color fragment_primary=directional-lit "
                   "fragment_secondary=black lighting=%s point_spot=bounded normals=interpolated-renormalized "
                   "vertex_shader=not-executed ", asset->lighting == 2 ? "night" : "day");
        else
            printf("mode=textured shading=tev-six-stage primary=vertex-color fragment_primary=white "
                   "fragment_secondary=black lighting=not-executed vertex_shader=not-executed ");
        printf(
               "projected_units=white-substitute disabled_units=zero culling=not-executed draw_order=layer-priority-resource "
               "external_textures=%" PRIu32 " decoded_textures=%" PRIu32 "\n", asset->texture_count, decoded);
        for (uint32_t i = 0; i < bch->mesh_count; ++i)
            printf("mesh_color mesh=%" PRIu32 " source=%s\n", i,
                   bch->meshes[i].color_source == 1 ? "stored" :
                   bch->meshes[i].color_source == 2 ? "fixed" : "absent-white");
        if (asset->bch_light) {
            const struct px_bch_environment *env = &asset->environment;
            printf("environment scene="); print_bch_name(&env->scene_name);
            printf(" lights=%" PRIu32 " light_sets=%" PRIu32 " cameras=%" PRIu32 " fogs=%" PRIu32 "\n",
                   env->light_count, env->set_count, env->camera_count, env->fog_count);
            for (uint32_t i = 0; i < bch->mesh_count; ++i)
                printf("mesh_normal mesh=%" PRIu32 " source=%s\n", i,
                       bch->meshes[i].normal_source == 1 ? "stored" : "fixed");
            for (uint32_t i = 0; i < bch->material_count; ++i) {
                const struct px_light_setup *setup = &asset->bch_light[i];
                const uint32_t *bound = &asset->point_bound[(size_t)i * 3];
                printf("light_setup material=%" PRIu32 " set=%" PRIu32 " global_ambient=%u,%u,%u"
                       " point_bound=%" PRIu32 ",%" PRIu32 ",%" PRIu32,
                       i, bch->materials[i].light_set_index, setup->global_ambient[0], setup->global_ambient[1],
                       setup->global_ambient[2], bound[0], bound[1], bound[2]);
                for (uint32_t k = 0; k < setup->count; ++k) {
                    const struct px_light_source *l = &setup->lights[k];
                    printf(" light="); print_bch_name(&env->lights[asset->light_index[(size_t)i * 8 + k]].name);
                    printf(":ambient=%u,%u,%u:diffuse=%u,%u,%u:vector=%.9g,%.9g,%.9g",
                           l->ambient[0], l->ambient[1], l->ambient[2], l->diffuse[0], l->diffuse[1],
                           l->diffuse[2], l->vector[0], l->vector[1], l->vector[2]);
                }
                putchar('\n');
            }
        }
        for (uint32_t i = 0; i < bch->material_count; ++i) {
            const struct px_bch_combiner_material *m = &asset->bch_combiner[i];
            printf("combiner material=%" PRIu32 " sources=0x%04" PRIx32 " sampled_units=0x%" PRIx32
                   " projected_units=0x%" PRIx32 " disabled_units=0x%" PRIx32
                   " buffer_update=0x%04" PRIx32 " buffer_color=0x%08" PRIx32,
                   i, m->sources, m->sampled_units, m->projected_units, m->disabled_units, m->tev.buffer_update,
                   m->tev.buffer_color);
            for (unsigned unit = 0; unit < 3; ++unit) {
                if (!((m->sampled_units >> unit) & 1)) continue;
                printf(" unit%u=", unit); print_bch_name(&bch->materials[i].texture_names[unit]);
                printf(":%" PRIu32 ":wrap%" PRIu32 ",%" PRIu32 ":%s", asset->bch_texture[(size_t)i * 3 + unit],
                       m->wrap_u[unit], m->wrap_v[unit], m->mag_linear[unit] ? "linear" : "nearest");
            }
            const struct px_bch_fragment_preview *f = &m->fragment;
            printf(" alpha_test=%" PRIu32 " alpha_compare=%" PRIu32 " alpha_reference=%" PRIu32
                   " depth_test=%" PRIu32 " depth_compare=%" PRIu32 " depth_write=%" PRIu32
                   " color_mask=0x%" PRIx32 " blend=%s cull_register=%" PRIu32 "\n",
                   f->alpha_test, f->alpha_compare, f->alpha_reference, f->depth_test, f->depth_compare,
                   f->depth_write, f->color_write_mask, blend[f->blend], f->cull);
            for (unsigned stage = 0; stage < 6; ++stage) {
                const struct px_tev_stage *t = &m->tev.stages[stage];
                printf("tev material=%" PRIu32 " stage=%u source=0x%08" PRIx32 " operand=0x%08" PRIx32
                       " combine=0x%08" PRIx32 " constant=0x%08" PRIx32 " scale=0x%08" PRIx32 "\n",
                       i, stage, t->source, t->operand, t->combine, t->constant, t->scale);
            }
        }
        printf("submission_order=");
        for (uint32_t i = 0; i < bch->mesh_count; ++i)
            printf("%s%" PRIu32, i ? "," : "", asset->draw_order[i]);
        putchar('\n');
        return;
    }
    printf("mode=textured shading=unlit-tev-stage0 primary=white lighting=not-executed "
           "stages_executed=1 culling=not-executed draw_order=layer-priority-resource "
           "external_textures=%" PRIu32 " decoded_textures=%" PRIu32 "\n", asset->texture_count, decoded);
    for (uint32_t i = 0; i < bch->material_count; ++i) {
        const struct px_bch_preview_material *m = &asset->bch_preview[i];
        const uint32_t bound = asset->bch_texture[(size_t)i * 3 + m->unit];
        const struct px_texture_image *image = &asset->textures[bound];
        printf("binding material=%" PRIu32 " unit=%" PRIu32 " texture=", i, m->unit);
        print_bch_name(&bch->materials[i].texture_names[m->unit]);
        printf(" external_index=%" PRIu32 " size=%" PRIu32 "x%" PRIu32
               " wrap=%" PRIu32 ",%" PRIu32 " filter=%s alpha_test=%" PRIu32 " alpha_compare=%" PRIu32
               " alpha_reference=%" PRIu32 " depth_test=%" PRIu32 " depth_compare=%" PRIu32
               " depth_write=%" PRIu32 " color_mask=0x%" PRIx32 " blend=%s cull_register=%" PRIu32
               " unexecuted_units=0x%" PRIx32 "\n",
               bound, image->width, image->height, m->wrap_u, m->wrap_v,
               m->mag_linear ? "linear" : "nearest", m->alpha_test, m->alpha_compare,
               m->alpha_reference, m->depth_test, m->depth_compare, m->depth_write,
               m->color_write_mask, blend[m->blend], m->cull, m->unexecuted_units);
    }
    printf("submission_order=");
    for (uint32_t i = 0; i < bch->mesh_count; ++i)
        printf("%s%" PRIu32, i ? "," : "", asset->draw_order[i]);
    putchar('\n');
}

int main(int argc, char **argv)
{
    struct options o = {0}; int parsed = parse(argc, argv, &o);
    if (parsed != 1) { usage(parsed == 2 ? stdout : stderr, argv[0]); return parsed == 2 ? 0 : 1; }
    struct model_asset asset = {0};
    if (!load(&o, &asset)) { asset_free(&asset); return 1; }
    if (o.textured && !asset.bch_textured && !prepare_draw_order(&asset)) { asset_free(&asset); return 1; }
    const struct px_cgfx_model mesh = o.textured && !asset.bch_textured ? asset.scene.mesh : asset.wire;
    printf("name=\"");
    for (const unsigned char *p = (const unsigned char *)mesh.name; *p; ++p) {
        if (*p >= 32 && *p <= 126 && *p != '"' && *p != '\\') putchar(*p);
        else printf("\\x%02x", *p);
    }
    printf("\" vertices=%" PRIu32 " triangles=%" PRIu32 " shapes=%" PRIu32
           " bounds_min=(%.9g,%.9g,%.9g) bounds_max=(%.9g,%.9g,%.9g)\n",
           mesh.vertex_count, mesh.index_count / 3, mesh.shape_count,
           mesh.bounds_min[0], mesh.bounds_min[1], mesh.bounds_min[2],
           mesh.bounds_max[0], mesh.bounds_max[1], mesh.bounds_max[2]);
    if (asset.bch.mesh_count) print_bch_metadata(&asset.bch, asset.bch_textured);
    if (asset.bch_textured) print_bch_bindings(&asset);
    else if (o.textured) {
        uint32_t decoded = 0;
        for (uint32_t i = 0; i < asset.texture_count; ++i) decoded += asset.textures[i].rgba != NULL;
        printf("mode=textured shading=unlit-texture-times-vertex-color draws=%" PRIu32
               " materials=%" PRIu32 " textures=%" PRIu32 " draw_order=translucency-priority-resource\n",
               asset.scene.draw_count, asset.scene.material_count, decoded);
        for (uint32_t i = 0; i < asset.scene.draw_count; ++i) {
            const struct px_cgfx_draw *d = &asset.scene.draws[i];
            const struct px_cgfx_material *m = &asset.scene.materials[d->material_index];
            struct px_raster_sampler sampler; struct px_raster_state state;
            if (!material_state(m, &sampler, &state)) {
                fprintf(stderr, "[model-viewer] Unsupported material preview state.\n");
                asset_free(&asset); return 1;
            }
            printf("draw=%" PRIu32 " shape=%" PRIu32 " material=%" PRIu32 " texture=%" PRIu32
                   " indices=%" PRIu32 " visible=%" PRIu32 " priority=%" PRIu32
                   " depth_write=%" PRIu32 " cull=%" PRIu32 " blend=%" PRIu32 "\n",
                   i, d->shape_index, d->material_index, m->texture_index, d->index_count,
                   d->visible, d->render_priority, m->depth_write, m->cull_mode, m->blend_mode);
        }
        printf("submission_order=");
        for (uint32_t i = 0; i < asset.scene.draw_count; ++i)
            printf("%s%" PRIu32, i ? "," : "", asset.draw_order[i]);
        putchar('\n');
    }
    int ok = o.inspect || window(&o, &asset); asset_free(&asset); return ok ? 0 : 1;
}
