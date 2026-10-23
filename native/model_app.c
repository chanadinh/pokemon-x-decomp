#include "assets/cgfx_model.h"
#include "assets/cgfx.h"
#include "assets/garc.h"
#include "assets/lz.h"
#include "render/raster.h"

#include <SDL.h>
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct options {
    const char *romfs, *archive, *capture;
    uint32_t entry, subentry, model, frames;
    int have_entry, have_subentry, have_model, demo, inspect, software, self_test, textured;
};
struct camera { double yaw, pitch, zoom; int running; };
struct model_asset {
    struct px_cgfx_model wire;
    struct px_cgfx_scene scene;
    struct px_texture_image *textures;
    uint32_t *draw_order;
    uint32_t texture_count;
};

static void asset_free(struct model_asset *asset)
{
    if (asset->textures)
        for (uint32_t i = 0; i < asset->texture_count; ++i) px_texture_free(&asset->textures[i]);
    free(asset->textures);
    free(asset->draw_order);
    px_cgfx_scene_free(&asset->scene);
    px_cgfx_model_free(&asset->wire);
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
        "  --model N          CGFX model index (default 0)\n"
        "  --textured         Preview supported material texture and vertex colors\n"
        "  --inspect          Print decoded mesh metadata without a window\n"
        "  --capture-frame F  Save first viewport to a new BMP file\n"
        "  --frames N         Exit after a positive number of frames\n"
        "  --software         Force software rendering\n"
        "  --self-test-input  Exercise orbit/zoom/reset/exit; default 3 frames\n"
        "Controls: arrows orbit, +/- zoom, R/Home reset, Escape exit.\n"
        "Orthographic wireframe by default; --textured uses a bounded software preview.\n"
        "Inspection camera; no lighting, animation or gameplay.\n", program);
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
        if (strcmp(arg, "--romfs") && strcmp(arg, "--archive") && strcmp(arg, "--entry") &&
            strcmp(arg, "--subentry") && strcmp(arg, "--model") && strcmp(arg, "--frames") &&
            strcmp(arg, "--capture-frame")) return 0;
        if (++i == argc || !argv[i][0]) return 0;
        const char *value = argv[i];
        if (!strcmp(arg, "--romfs")) o->romfs = value;
        else if (!strcmp(arg, "--archive")) o->archive = value;
        else if (!strcmp(arg, "--capture-frame")) o->capture = value;
        else {
            uint32_t n;
            if (!number(value, &n)) return 0;
            if (!strcmp(arg, "--entry")) { o->entry = n; o->have_entry = 1; }
            else if (!strcmp(arg, "--subentry")) { if (n > 31) return 0; o->subentry = n; o->have_subentry = 1; }
            else if (!strcmp(arg, "--model")) { o->model = n; o->have_model = 1; }
            else { if (!n) return 0; o->frames = n; }
        }
    }
    if (o->demo) {
        if (o->romfs || o->archive || o->have_entry || o->have_subentry || o->have_model) return 0;
    } else if (!o->romfs || !o->archive || !o->have_entry || !relative_path(o->archive)) return 0;
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
    size_t a = strlen(o->romfs), b = strlen(o->archive);
    if (a > SIZE_MAX - b - 2) return 0;
    char *path = malloc(a + b + 2);
    if (!path) return 0;
    memcpy(path, o->romfs, a); path[a] = '/'; memcpy(path + a + 1, o->archive, b + 1);
    char error[256]; struct px_garc *archive = NULL;
    int ok = px_garc_open(path, &archive, error, sizeof error);
    free(path);
    if (!ok) { fprintf(stderr, "[model-viewer] Archive: %s\n", error); return 0; }
    uint8_t *member = NULL, *decoded = NULL; size_t size = 0, decoded_size = 0;
    ok = px_garc_read(archive, o->entry, o->subentry, 64 * 1024 * 1024,
                      &member, &size, error, sizeof error);
    px_garc_close(archive);
    if (!ok) { fprintf(stderr, "[model-viewer] Member: %s\n", error); return 0; }
    const uint8_t *data = member;
    const char *compression = "none";
    if (size && (member[0] == 0x10 || member[0] == 0x11)) {
        compression = member[0] == 0x10 ? "LZ10" : "LZ11";
        if (!px_lz_decode(member, size, 64 * 1024 * 1024, &decoded, &decoded_size, error, sizeof error)) {
            fprintf(stderr, "[model-viewer] Compression: %s\n", error); free(member); return 0;
        }
        data = decoded;
    }
    size_t input_size = decoded ? decoded_size : size;
    enum px_cgfx_model_result result = o->textured ?
        px_cgfx_model_decode_textured(data, input_size, o->model, &asset->scene) :
        px_cgfx_model_decode(data, input_size, o->model, mesh);
    if (result == PX_CGFX_MODEL_OK && o->textured) ok = load_images(data, input_size, asset);
    free(decoded); free(member);
    if (result != PX_CGFX_MODEL_OK) {
        fprintf(stderr, "[model-viewer] CGFX model: %s\n", px_cgfx_model_result_string(result)); return 0;
    }
    if (!ok) return 0;
    printf("archive=%s entry=%" PRIu32 " subentry=%" PRIu32 " model=%" PRIu32 " compression=%s\n",
           o->archive, o->entry, o->subentry, o->model, compression);
    return 1;
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

static int window(const struct options *o, const struct model_asset *asset)
{
    const struct px_cgfx_model *mesh = o->textured ? &asset->scene.mesh : &asset->wire;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) return 0;
    SDL_Window *win = SDL_CreateWindow(o->textured ?
        "Pokemon X unlit textured preview — arrows: orbit | +/-: zoom | R: reset | Esc: exit" :
        "Pokemon X wireframe — arrows: orbit | +/-: zoom | R: reset | Esc: exit",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 640, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = NULL; SDL_FPoint *points = NULL; int ok = 0;
    SDL_Texture *frame = NULL; struct px_raster_target target = {0};
    struct px_raster_vertex *vertices = NULL;
    if (!win) goto done;
    if (!o->software) renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer) goto done;
    if (o->textured) vertices = malloc((size_t)mesh->vertex_count * sizeof *vertices);
    else points = malloc((size_t)mesh->vertex_count * sizeof *points);
    if (o->textured ? !vertices : !points) goto done;
    struct camera c; reset(&c);
    if (o->self_test && !input_test(&c)) { SDL_SetError("Input self-test failed"); goto done; }
    SDL_RendererInfo info; if (SDL_GetRendererInfo(renderer, &info)) goto done;
    fprintf(stderr, "[model-viewer] Video=%s renderer=%s; orthographic %s.\n",
            SDL_GetCurrentVideoDriver(), info.name, o->textured ?
            "unlit textured software preview; translucency/priority/resource draw order" : "wireframe");
    uint32_t frames = 0;
    while (c.running) {
        SDL_Event e; while (SDL_PollEvent(&e)) event(&c, &e);
        if (!c.running) break;
        const char *path = frames == 0 ? o->capture : NULL;
        if (o->textured ? !draw_textured(renderer, asset, &c, vertices, &target, &frame, path) :
                         !draw(renderer, mesh, &c, points, path)) goto done;
        if (++frames == o->frames) break;
        SDL_Delay(16);
    }
    fprintf(stderr, "[model-viewer] Exit after %" PRIu32 " frames.\n", frames); ok = 1;
done:
    if (!ok) fprintf(stderr, "[model-viewer] Runtime: %s\n", SDL_GetError());
    free(vertices); px_raster_free(&target); if (frame) SDL_DestroyTexture(frame);
    free(points); if (renderer) SDL_DestroyRenderer(renderer); if (win) SDL_DestroyWindow(win); SDL_Quit();
    return ok;
}

int main(int argc, char **argv)
{
    struct options o = {0}; int parsed = parse(argc, argv, &o);
    if (parsed != 1) { usage(parsed == 2 ? stdout : stderr, argv[0]); return parsed == 2 ? 0 : 1; }
    struct model_asset asset = {0};
    if (!load(&o, &asset)) { asset_free(&asset); return 1; }
    if (o.textured && !prepare_draw_order(&asset)) { asset_free(&asset); return 1; }
    const struct px_cgfx_model mesh = o.textured ? asset.scene.mesh : asset.wire;
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
    if (o.textured) {
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
