#include "assets/cgfx_model.h"
#include "assets/garc.h"
#include "assets/lz.h"

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
    int have_entry, have_subentry, have_model, demo, inspect, software, self_test;
};
struct camera { double yaw, pitch, zoom; int running; };

static void usage(FILE *stream, const char *program)
{
    fprintf(stream,
        "Usage: %s (--demo | --romfs PATH --archive RELATIVE/PATH --entry N) [options]\n"
        "  --subentry N       GARC bitmap subentry 0..31 (default 0)\n"
        "  --model N          CGFX model index (default 0)\n"
        "  --inspect          Print decoded mesh metadata without a window\n"
        "  --capture-frame F  Save first wireframe viewport to a new BMP file\n"
        "  --frames N         Exit after a positive number of frames\n"
        "  --software         Force software rendering\n"
        "  --self-test-input  Exercise orbit/zoom/reset/exit; default 3 frames\n"
        "Controls: arrows orbit, +/- zoom, R/Home reset, Escape exit.\n"
        "Orthographic wireframe inspection; no materials, animation or gameplay.\n", program);
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

static int load(const struct options *o, struct px_cgfx_model *mesh)
{
    if (o->demo) {
        static const float positions[] = {-1,-1,-1, 1,-1,-1, 0,1,-1, 0,0,1};
        static const uint32_t indices[] = {0,1,2, 0,3,1, 1,3,2, 2,3,0};
        mesh->positions = malloc(sizeof positions); mesh->indices = malloc(sizeof indices);
        if (!mesh->positions || !mesh->indices) { px_cgfx_model_free(mesh); return 0; }
        memcpy(mesh->positions, positions, sizeof positions); memcpy(mesh->indices, indices, sizeof indices);
        mesh->vertex_count = 4; mesh->index_count = 12; mesh->shape_count = 1;
        strcpy(mesh->name, "synthetic tetrahedron");
        for (unsigned i = 0; i < 3; ++i) { mesh->bounds_min[i] = -1; mesh->bounds_max[i] = 1; }
        return 1;
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
    enum px_cgfx_model_result result = px_cgfx_model_decode(data, decoded ? decoded_size : size, o->model, mesh);
    free(decoded); free(member);
    if (result != PX_CGFX_MODEL_OK) {
        fprintf(stderr, "[model-viewer] CGFX model: %s\n", px_cgfx_model_result_string(result)); return 0;
    }
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

static int window(const struct options *o, const struct px_cgfx_model *mesh)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) return 0;
    SDL_Window *win = SDL_CreateWindow("Pokemon X wireframe — arrows: orbit | +/-: zoom | R: reset | Esc: exit",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 640, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = NULL; SDL_FPoint *points = NULL; int ok = 0;
    if (!win) goto done;
    if (!o->software) renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer) goto done;
    points = malloc((size_t)mesh->vertex_count * sizeof *points); if (!points) goto done;
    struct camera c; reset(&c);
    if (o->self_test && !input_test(&c)) { SDL_SetError("Input self-test failed"); goto done; }
    SDL_RendererInfo info; if (SDL_GetRendererInfo(renderer, &info)) goto done;
    fprintf(stderr, "[model-viewer] Video=%s renderer=%s; orthographic wireframe.\n", SDL_GetCurrentVideoDriver(), info.name);
    uint32_t frames = 0;
    while (c.running) {
        SDL_Event e; while (SDL_PollEvent(&e)) event(&c, &e);
        if (!c.running) break;
        if (!draw(renderer, mesh, &c, points, frames == 0 ? o->capture : NULL)) goto done;
        if (++frames == o->frames) break;
        SDL_Delay(16);
    }
    fprintf(stderr, "[model-viewer] Exit after %" PRIu32 " frames.\n", frames); ok = 1;
done:
    if (!ok) fprintf(stderr, "[model-viewer] Runtime: %s\n", SDL_GetError());
    free(points); if (renderer) SDL_DestroyRenderer(renderer); if (win) SDL_DestroyWindow(win); SDL_Quit();
    return ok;
}

int main(int argc, char **argv)
{
    struct options o = {0}; int parsed = parse(argc, argv, &o);
    if (parsed != 1) { usage(parsed == 2 ? stdout : stderr, argv[0]); return parsed == 2 ? 0 : 1; }
    struct px_cgfx_model mesh = {0}; if (!load(&o, &mesh)) return 1;
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
    int ok = o.inspect || window(&o, &mesh); px_cgfx_model_free(&mesh); return ok ? 0 : 1;
}
