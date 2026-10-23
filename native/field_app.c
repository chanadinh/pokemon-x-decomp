#include "assets/field.h"
#include "assets/garc.h"
#include "assets/lz.h"

#include <SDL.h>
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Inspector choices, not reconstructed player rules. Raw words are LE. */
#define STANDARD UINT32_C(0x00000020)
#define BLOCKED UINT32_C(0x01000021)
#define REGION_SIDE 40
#define SCENE_CELLS 64
#define SCENE_VERTICES 262144u

struct options {
    const char *romfs, *capture, *steps;
    uint32_t zone, frames;
    int have_zone, demo, inspect, software, self_test, collision;
};
struct region {
    int16_t id;
    struct px_field_grid grid;
    struct px_field_collision collision;
};
struct scene {
    struct px_field_zone zone;
    struct px_field_matrix matrix;
    struct region regions[SCENE_CELLS];
    unsigned count, width, height;
    int slots[SCENE_CELLS];
    int start_x, start_z, min_x, max_x, min_z, max_z;
    size_t decoded_bytes;
};
struct view {
    int x, z, running, focus, collision;
    unsigned accepted, rejected;
    double zoom;
};

static void usage(FILE *f, const char *p)
{
    fprintf(f, "Usage: %s (--demo | --romfs PATH --zone N) [options]\n"
        "  --inspect          Report the zone/matrix/region chain without video\n"
        "  --steps RDLU       Apply up to 4096 inspection-marker steps\n"
        "  --capture-frame F  Save first viewport to a new BMP file\n"
        "  --frames N         Exit after a positive number of frames\n"
        "  --software         Force software rendering\n"
        "  --collision        Initially show collision triangle edges\n"
        "  --self-test-input  Check controls; default 3 frames\n"
        "Arrows/WASD: marker, +/-: zoom, F: focus/full, C: collision edges,\n"
        "R/Home: reset, Escape: exit. Only exact raw 20 00 00 00 cells\n"
        "admit the inspection marker. This is not player movement or gameplay.\n", p);
}

static int number(const char *s, uint32_t *out)
{
    if (!*s) return 0;
    for (const char *p = s; *p; ++p) if (*p < '0' || *p > '9') return 0;
    errno = 0;
    uintmax_t n = strtoumax(s, NULL, 10);
    if (errno || n > UINT32_MAX) return 0;
    *out = (uint32_t)n; return 1;
}

static int parse(int argc, char **argv, struct options *o)
{
    for (int i = 1; i < argc; ++i) {
        const char *a = argv[i];
        if (!strcmp(a, "--help")) return 2;
        if (!strcmp(a, "--demo")) { o->demo = 1; continue; }
        if (!strcmp(a, "--inspect")) { o->inspect = 1; continue; }
        if (!strcmp(a, "--software")) { o->software = 1; continue; }
        if (!strcmp(a, "--collision")) { o->collision = 1; continue; }
        if (!strcmp(a, "--self-test-input")) { o->self_test = 1; continue; }
        if (strcmp(a, "--romfs") && strcmp(a, "--zone") && strcmp(a, "--steps") &&
            strcmp(a, "--capture-frame") && strcmp(a, "--frames")) return 0;
        if (++i == argc || !argv[i][0]) return 0;
        if (!strcmp(a, "--romfs")) o->romfs = argv[i];
        else if (!strcmp(a, "--capture-frame")) o->capture = argv[i];
        else if (!strcmp(a, "--steps")) {
            o->steps = argv[i];
            if (strlen(o->steps) > 4096 || strspn(o->steps, "RDLU") != strlen(o->steps)) return 0;
        } else {
            uint32_t n;
            if (!number(argv[i], &n)) return 0;
            if (!strcmp(a, "--zone")) { o->zone = n; o->have_zone = 1; }
            else { if (!n) return 0; o->frames = n; }
        }
    }
    if (o->demo ? (o->romfs || o->have_zone) : (!o->romfs || !o->have_zone)) return 0;
    if (o->inspect && (o->capture || o->frames || o->self_test || o->software || o->collision)) return 0;
    if (o->self_test && !o->frames) o->frames = 3;
    return 1;
}

static void scene_free(struct scene *s)
{
    for (unsigned i = 0; i < s->count; ++i) {
        px_field_grid_free(&s->regions[i].grid);
        px_field_collision_free(&s->regions[i].collision);
    }
    px_field_matrix_free(&s->matrix); memset(s, 0, sizeof *s);
}

static int member(const struct options *o, const char *archive, uint32_t id,
                  struct scene *s, uint8_t **data, size_t *size)
{
    char error[256] = "allocation failure";
    *data = NULL; *size = 0;
    size_t a = strlen(o->romfs), b = strlen(archive);
    if (a > SIZE_MAX - b - 2) return 0;
    char *path = malloc(a + b + 2);
    if (!path) return 0;
    memcpy(path, o->romfs, a); path[a] = '/'; memcpy(path + a + 1, archive, b + 1);
    struct px_garc *g = NULL;
    int ok = px_garc_open(path, &g, error, sizeof error);
    free(path);
    if (ok) ok = px_garc_read(g, id, 0, PX_FIELD_INPUT_LIMIT, data, size, error, sizeof error);
    px_garc_close(g);
    if (ok && *size && ((*data)[0] == 0x10 || (*data)[0] == 0x11)) {
        uint8_t *decoded = NULL; size_t n = 0;
        ok = px_lz_decode(*data, *size, PX_FIELD_INPUT_LIMIT - s->decoded_bytes,
                         &decoded, &n, error, sizeof error);
        free(*data); *data = decoded; *size = n;
    }
    if (ok && *size > PX_FIELD_INPUT_LIMIT - s->decoded_bytes) {
        snprintf(error, sizeof error, "aggregate decoded input limit exceeded"); ok = 0;
    }
    if (!ok) {
        fprintf(stderr, "[field-viewer] %s entry=%" PRIu32 ": %s\n", archive, id, error);
        free(*data); *data = NULL; *size = 0;
    } else s->decoded_bytes += *size;
    return ok;
}

static const struct region *cell(const struct scene *s, int x, int z, uint32_t *word)
{
    if (x < 0 || z < 0 || (unsigned)x >= s->width || (unsigned)z >= s->height) return NULL;
    unsigned slot = (unsigned)(z / REGION_SIDE) * s->matrix.width + (unsigned)(x / REGION_SIDE);
    int index = s->slots[slot];
    if (index < 0) return NULL;
    const struct region *r = &s->regions[index];
    *word = r->grid.tiles[(z % REGION_SIDE) * REGION_SIDE + x % REGION_SIDE];
    return r;
}

static int eligible(const struct scene *s, int x, int z)
{
    uint32_t word;
    return cell(s, x, z, &word) && word == STANDARD;
}

static int finish_scene(struct scene *s)
{
    s->width = s->matrix.width * REGION_SIDE; s->height = s->matrix.height * REGION_SIDE;
    s->start_x = s->start_z = -1;
    s->min_x = (int)s->width; s->min_z = (int)s->height; s->max_x = s->max_z = 0;
    for (unsigned z = 0; z < s->height; ++z) for (unsigned x = 0; x < s->width; ++x) {
        if (!eligible(s, (int)x, (int)z)) continue;
        if (s->start_x < 0) { s->start_x = (int)x; s->start_z = (int)z; }
        if ((int)x < s->min_x) s->min_x = (int)x;
        if ((int)z < s->min_z) s->min_z = (int)z;
        if ((int)x > s->max_x) s->max_x = (int)x;
        if ((int)z > s->max_z) s->max_z = (int)z;
    }
    /* Scenes without this exact type remain inspectable, without a marker. */
    return 1;
}

static int demo(struct scene *s)
{
    s->matrix.width = s->matrix.height = 1;
    s->matrix.field_ids = malloc(sizeof *s->matrix.field_ids);
    if (!s->matrix.field_ids) return 0;
    s->matrix.field_ids[0] = 0; s->count = 1; s->slots[0] = 0;
    struct region *r = s->regions; r->grid.width = r->grid.height = REGION_SIDE;
    r->grid.tiles = malloc(REGION_SIDE * REGION_SIDE * sizeof *r->grid.tiles);
    if (!r->grid.tiles) return 0;
    for (int z = 0; z < REGION_SIDE; ++z) for (int x = 0; x < REGION_SIDE; ++x)
        r->grid.tiles[z * REGION_SIDE + x] =
            x >= 10 && x <= 24 && z >= 12 && z <= 25 ? STANDARD : BLOCKED;
    r->grid.tiles[18 * REGION_SIDE + 17] = 0x12345678;
    return finish_scene(s);
}

static int load(const struct options *o, struct scene *s)
{
    if (o->demo) return demo(s);
    uint8_t *data = NULL; size_t size = 0; enum px_field_result result;
    if (!member(o, "a/0/1/2", o->zone, s, &data, &size)) return 0;
    result = px_field_zone_decode(data, size, &s->zone); free(data);
    if (result != PX_FIELD_OK) goto failed;
    if (!member(o, "a/0/4/2", (uint32_t)s->zone.matrix_id, s, &data, &size)) return 0;
    result = px_field_matrix_decode(data, size, &s->matrix); free(data);
    if (result != PX_FIELD_OK) goto failed;
    size_t cells = (size_t)s->matrix.width * s->matrix.height;
    if (cells > SCENE_CELLS) { result = PX_FIELD_LIMIT_EXCEEDED; goto failed; }
    uint32_t vertices = 0;
    for (size_t i = 0; i < cells; ++i) {
        int16_t id = s->matrix.field_ids[i]; s->slots[i] = -1;
        if (id < 0) continue;
        unsigned index;
        for (index = 0; index < s->count; ++index) if (s->regions[index].id == id) break;
        s->slots[i] = (int)index;
        if (index < s->count) continue;
        struct region *r = &s->regions[s->count++]; r->id = id;
        if (!member(o, "a/0/4/1", (uint32_t)id, s, &data, &size)) return 0;
        result = px_field_grid_decode(data, size, &r->grid);
        if (result == PX_FIELD_OK && (r->grid.width != REGION_SIDE || r->grid.height != REGION_SIDE))
            result = PX_FIELD_UNSUPPORTED;
        if (result == PX_FIELD_OK && r->grid.sections[2].size)
            result = px_field_collision_decode(data, size, &r->collision);
        free(data);
        if (result != PX_FIELD_OK) goto failed;
        if (r->collision.vertex_count > SCENE_VERTICES - vertices) {
            result = PX_FIELD_LIMIT_EXCEEDED; goto failed;
        }
        vertices += r->collision.vertex_count;
    }
    /* Rendering cost counts every instance, even when storage is shared. */
    vertices = 0;
    for (size_t i = 0; i < cells; ++i) {
        if (s->slots[i] < 0) continue;
        uint32_t count = s->regions[s->slots[i]].collision.vertex_count;
        if (count > SCENE_VERTICES - vertices) { result = PX_FIELD_LIMIT_EXCEEDED; goto failed; }
        vertices += count;
    }
    return finish_scene(s);
failed:
    fprintf(stderr, "[field-viewer] Field decode: %s\n", px_field_result_string(result)); return 0;
}

static void reset(struct view *v, const struct scene *s)
{
    *v = (struct view){s->start_x, s->start_z, 1, 1, 0, 0, 0, 1};
}

static void step(struct view *v, const struct scene *s, int dx, int dz)
{
    if (v->x >= 0 && eligible(s, v->x + dx, v->z + dz)) {
        v->x += dx; v->z += dz; ++v->accepted;
    } else ++v->rejected;
}

static void event(struct view *v, const struct scene *s, const SDL_Event *e)
{
    if (e->type == SDL_QUIT) { v->running = 0; return; }
    if (e->type != SDL_KEYDOWN) return;
    switch (e->key.keysym.sym) {
    case SDLK_ESCAPE: v->running = 0; break;
    case SDLK_RIGHT: case SDLK_d: step(v,s,1,0); break;
    case SDLK_LEFT: case SDLK_a: step(v,s,-1,0); break;
    case SDLK_UP: case SDLK_w: step(v,s,0,-1); break;
    case SDLK_DOWN: case SDLK_s: step(v,s,0,1); break;
    case SDLK_PLUS: case SDLK_EQUALS: case SDLK_KP_PLUS: v->zoom *= 1.25; break;
    case SDLK_MINUS: case SDLK_KP_MINUS: v->zoom /= 1.25; break;
    case SDLK_f: v->focus = !v->focus; break;
    case SDLK_c: v->collision = !v->collision; break;
    case SDLK_r: case SDLK_HOME: reset(v,s); break;
    default: break;
    }
    if (v->zoom < 0.125) v->zoom = 0.125;
    if (v->zoom > 16) v->zoom = 16;
}

static int inject(struct view *v, const struct scene *s, SDL_Keycode key)
{
    SDL_Event e; memset(&e,0,sizeof e); e.type = SDL_KEYDOWN; e.key.keysym.sym = key;
    if (SDL_PushEvent(&e) != 1) return 0;
    while (SDL_PollEvent(&e)) event(v,s,&e);
    return 1;
}

static int input_test(struct view *v, const struct scene *s)
{
    struct view saved = *v; SDL_Event e; while (SDL_PollEvent(&e)) {}
    reset(v,s);
    int can_step = eligible(s,v->x+1,v->z), x = v->x;
    int ok = inject(v,s,SDLK_RIGHT) && v->x == x + (v->x >= 0 && can_step) &&
        v->accepted == (unsigned)(x >= 0 && can_step) &&
        inject(v,s,SDLK_PLUS) && v->zoom > 1 && inject(v,s,SDLK_f) && !v->focus &&
        inject(v,s,SDLK_c) && v->collision && inject(v,s,SDLK_HOME) &&
        v->x == s->start_x && v->z == s->start_z && v->zoom == 1 &&
        !v->accepted && !v->rejected && inject(v,s,SDLK_ESCAPE) && !v->running;
    *v = saved;
    if (ok) fprintf(stderr,"[field-viewer] Input self-test passed: marker, zoom, focus, overlay, reset, Escape.\n");
    return ok;
}

/* Small independently authored 5x7 UI alphabet; no game font is used. */
static const uint8_t glyphs[][7] = {
    {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},
    {30,1,1,14,1,1,30},{2,6,10,18,31,2,2},{31,16,16,30,1,1,30},
    {14,16,16,30,17,17,14},{31,1,2,4,8,8,8},{14,17,17,14,17,17,14},
    {14,17,17,15,1,1,14},
    {14,17,17,31,17,17,17},{30,17,17,30,17,17,30},{15,16,16,16,16,16,15},
    {30,17,17,17,17,17,30},{31,16,16,30,16,16,31},{31,16,16,30,16,16,16},
    {15,16,16,23,17,17,15},{17,17,17,31,17,17,17},{14,4,4,4,4,4,14},
    {7,2,2,2,18,18,12},{17,18,20,24,20,18,17},{16,16,16,16,16,16,31},
    {17,27,21,21,17,17,17},{17,25,21,19,17,17,17},{14,17,17,17,17,17,14},
    {30,17,17,30,16,16,16},{14,17,17,17,21,18,13},{30,17,17,30,20,18,17},
    {15,16,16,14,1,1,30},{31,4,4,4,4,4,4},{17,17,17,17,17,17,14},
    {17,17,17,17,17,10,4},{17,17,17,21,21,21,10},{17,17,10,4,10,17,17},
    {17,17,10,4,4,4,4},{31,1,2,4,8,16,31}
};
static void label(SDL_Renderer *r, int x, int y, const char *s, int scale)
{
    for (; *s; ++s, x += scale * 6) {
        int c = *s, index = c >= '0' && c <= '9' ? c - '0' :
            c >= 'A' && c <= 'Z' ? c - 'A' + 10 : -1;
        for (int row = 0; row < 7; ++row) for (int col = 0; col < 5; ++col) {
            int set = index >= 0 ? (glyphs[index][row] >> (4-col)) & 1 :
                (c == '-' && row == 3) || (c == '.' && row == 6 && col == 2) ||
                (c == ':' && (row == 2 || row == 5) && col == 2) ||
                (c == '/' && row + col == 5) ||
                (c == '+' && (row == 3 || (col == 2 && row >= 1 && row <= 5)));
            if (set) { SDL_Rect p = {x+col*scale,y+row*scale,scale,scale}; SDL_RenderFillRect(r,&p); }
        }
    }
}

static int capture(SDL_Renderer *r, const char *path)
{
    int w,h;
    if (SDL_GetRendererOutputSize(r,&w,&h) || w <= 0 || h <= 0 || w > 4096 || h > 4096 ||
        (uint64_t)w*h > 4194304) { SDL_SetError("Capture viewport limit exceeded"); return 0; }
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0,w,h,32,SDL_PIXELFORMAT_RGBA32);
    if (!surface) return 0;
    int ok = !SDL_RenderReadPixels(r,NULL,surface->format->format,surface->pixels,surface->pitch);
    if (ok) {
        FILE *file = fopen(path,"wbx");
        if (!file) { SDL_SetError("Cannot create new export: %s",strerror(errno)); ok = 0; }
        else {
            SDL_RWops *stream = SDL_RWFromFP(file,SDL_TRUE);
            if (!stream) { fclose(file); ok = 0; }
            else ok = !SDL_SaveBMP_RW(surface,stream,1);
        }
    }
    SDL_FreeSurface(surface); return ok;
}

static void report_marker(const struct scene *s, const struct view *v)
{
    printf("marker_start=(%d,%d) marker=(%d,%d) accepted=%u rejected=%u policy=exact-standard-cell\n",
           s->start_x,s->start_z,v->x,v->z,v->accepted,v->rejected);
    uint32_t word; const struct region *r = cell(s,v->x,v->z,&word);
    if (r) {
        double y;
        enum px_field_result hit = px_field_collision_first_height(&r->collision,
            (v->x % REGION_SIDE + 0.5)*18-360, (v->z % REGION_SIDE + 0.5)*18-360,&y);
        printf("marker_world_xz=(%.9g,%.9g) raw_le=%08" PRIx32 " first_triangle_height=",
               (v->x+0.5)*18,(v->z+0.5)*18,word);
        if (hit == PX_FIELD_OK) printf("%.9g\n",y); else puts("none");
    }
}

/* Clip in double before SDL's software renderer converts endpoints to ints. */
static void clipped_edge(SDL_Renderer *r, const SDL_Rect *box,
                         double x0, double z0, double x1, double z1)
{
    double dx = x1-x0, dz = z1-z0, low = 0, high = 1;
    const double p[4] = {-dx,dx,-dz,dz};
    const double q[4] = {x0-box->x,box->x+box->w-1-x0,z0-box->y,box->y+box->h-1-z0};
    for (unsigned i = 0; i < 4; ++i) {
        if (p[i] == 0) { if (q[i] < 0) return; continue; }
        double t = q[i]/p[i];
        if (p[i] < 0) { if (t > low) low = t; }
        else if (t < high) high = t;
        if (low > high) return;
    }
    /* Final clamps remove cancellation at large but accepted coordinates. */
    double ax = fmax(box->x,fmin(box->x+box->w-1,x0+low*dx));
    double az = fmax(box->y,fmin(box->y+box->h-1,z0+low*dz));
    double bx = fmax(box->x,fmin(box->x+box->w-1,x0+high*dx));
    double bz = fmax(box->y,fmin(box->y+box->h-1,z0+high*dz));
    SDL_RenderDrawLineF(r,(float)ax,(float)az,(float)bx,(float)bz);
}

static int draw(SDL_Renderer *r, const struct options *o, const struct scene *s,
                const struct view *v, const char *path)
{
    SDL_SetRenderDrawColor(r,15,22,32,255); if (SDL_RenderClear(r)) return 0;
    SDL_SetRenderDrawColor(r,225,235,245,255);
    label(r,24,24,"FIELD TERRAIN INSPECTOR",3);
    SDL_SetRenderDrawColor(r,126,156,183,255);
    label(r,24,61,o->demo ? "GENERATED GRID / INSPECTION MARKER / NO GAMEPLAY" :
          "ORIGINAL LAYOUT / INSPECTION MARKER / NO GAMEPLAY",2);
    SDL_Rect viewport = {24,104,720,600};
    SDL_SetRenderDrawColor(r,21,32,45,255); SDL_RenderFillRect(r,&viewport);
    if (SDL_RenderSetClipRect(r,&viewport)) return 0;
    double cx = s->width/2.0, cz = s->height/2.0, spanx = s->width+2, spanz = s->height+2;
    if (v->focus && s->start_x >= 0) {
        cx = (s->min_x+s->max_x+1)/2.0; cz = (s->min_z+s->max_z+1)/2.0;
        spanx = s->max_x-s->min_x+5; spanz = s->max_z-s->min_z+5;
    }
    double scale = fmin(viewport.w/spanx,viewport.h/spanz)*v->zoom;
    double ox = viewport.x+viewport.w/2.0-cx*scale, oz = viewport.y+viewport.h/2.0-cz*scale;
    for (unsigned z = 0; z < s->height; ++z) for (unsigned x = 0; x < s->width; ++x) {
        uint32_t word;
        if (!cell(s,(int)x,(int)z,&word)) continue;
        double px = ox+x*scale, pz = oz+z*scale;
        if (px+scale < viewport.x || pz+scale < viewport.y || px > viewport.x+viewport.w || pz > viewport.y+viewport.h) continue;
        if (word == STANDARD) SDL_SetRenderDrawColor(r,58,143,116,255);
        else if (word == BLOCKED) SDL_SetRenderDrawColor(r,39,53,70,255);
        else SDL_SetRenderDrawColor(r,191,128,65,255);
        SDL_FRect rect = {(float)px,(float)pz,(float)scale,(float)scale}; SDL_RenderFillRectF(r,&rect);
        if (scale >= 5) { SDL_SetRenderDrawColor(r,24,36,49,255); SDL_RenderDrawRectF(r,&rect); }
    }
    if (v->collision) {
        SDL_SetRenderDrawColor(r,144,152,213,255);
        for (unsigned j = 0; j < (unsigned)s->matrix.width*s->matrix.height; ++j) {
            if (s->slots[j] < 0) continue;
            const struct px_field_collision *c = &s->regions[s->slots[j]].collision;
            for (uint32_t i = 0; i < c->vertex_count; i += 3) {
                double points[3][2];
                for (unsigned k = 0; k < 3; ++k) {
                    const float *p = &c->positions[(i+k)*3];
                    points[k][0] = ox+(j%s->matrix.width*40+((double)p[0]+360)/18)*scale;
                    points[k][1] = oz+(j/s->matrix.width*40+((double)p[2]+360)/18)*scale;
                }
                for (unsigned k = 0; k < 3; ++k)
                    clipped_edge(r,&viewport,points[k][0],points[k][1],points[(k+1)%3][0],points[(k+1)%3][1]);
            }
        }
    }
    if (v->x >= 0) {
        SDL_FRect mark = {(float)(ox+(v->x+0.22)*scale),(float)(oz+(v->z+0.22)*scale),
                          (float)(scale*0.56),(float)(scale*0.56)};
        SDL_SetRenderDrawColor(r,255,225,125,255); SDL_RenderFillRectF(r,&mark);
        SDL_SetRenderDrawColor(r,255,250,223,255); SDL_RenderDrawRectF(r,&mark);
    }
    if (SDL_RenderSetClipRect(r,NULL)) return 0;
    char text[96]; int x = 776;
    SDL_SetRenderDrawColor(r,225,235,245,255);
    label(r,x,110,o->demo ? "SYNTHETIC DEMO" : "XY RESOURCE CHAIN",2);
    snprintf(text,sizeof text,"ZONE %" PRIu32 " / AREA %d",o->zone,s->zone.area_id); label(r,x,148,text,2);
    snprintf(text,sizeof text,"MATRIX %d / %u X %u",s->zone.matrix_id,s->matrix.width,s->matrix.height); label(r,x,176,text,2);
    snprintf(text,sizeof text,"REGIONS %u",s->count); label(r,x,204,text,2);
    uint32_t word = 0; const struct region *region = cell(s,v->x,v->z,&word);
    snprintf(text,sizeof text,"REGION %d",region ? region->id : -1); label(r,x,258,text,2);
    snprintf(text,sizeof text,"CELL X %d / Z %d",v->x,v->z); label(r,x,286,text,2);
    snprintf(text,sizeof text,"RAW %02X %02X %02X %02X",word&255,(word>>8)&255,(word>>16)&255,word>>24); label(r,x,314,text,2);
    double y;
    if (region && px_field_collision_first_height(&region->collision,(v->x%40+0.5)*18-360,
        (v->z%40+0.5)*18-360,&y) == PX_FIELD_OK) snprintf(text,sizeof text,"TRIANGLE Y %.2f",y);
    else snprintf(text,sizeof text,"TRIANGLE Y NONE");
    label(r,x,342,text,2);
    SDL_SetRenderDrawColor(r,108,207,167,255); label(r,x,400,"GREEN / STANDARD CELL",2);
    SDL_SetRenderDrawColor(r,146,165,185,255); label(r,x,428,"GRAY / STANDARD BLOCK",2);
    SDL_SetRenderDrawColor(r,226,166,101,255); label(r,x,456,"AMBER / OTHER RAW TYPE",2);
    SDL_SetRenderDrawColor(r,255,225,125,255); label(r,x,484,"YELLOW / MARKER",2);
    SDL_SetRenderDrawColor(r,157,179,199,255);
    label(r,x,546,"ARROWS/WASD MOVE",2); label(r,x,574,"+/- ZOOM  F FOCUS",2);
    label(r,x,602,"C COLLISION EDGES",2); label(r,x,630,"R RESET  ESC EXIT",2);
    SDL_SetRenderDrawColor(r,126,156,183,255);
    label(r,24,726,"TOP VIEW X RIGHT / Z DOWN. BCH ENVIRONMENT GRAPHICS ARE NOT DECODED.",2);
    if (path && !capture(r,path)) return 0;
    SDL_RenderPresent(r); return 1;
}

static int window(const struct options *o, const struct scene *s, struct view *v)
{
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)) return 0;
    SDL_Window *win = SDL_CreateWindow("Pokemon X field inspector - terrain and collision evidence",
        SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1100,760,SDL_WINDOW_RESIZABLE);
    SDL_Renderer *r = NULL; int ok = 0;
    if (!win) goto done;
    if (!o->software) r = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if (!r) r = SDL_CreateRenderer(win,-1,SDL_RENDERER_SOFTWARE);
    if (!r || SDL_RenderSetLogicalSize(r,1100,760)) goto done;
    if (o->self_test && !input_test(v,s)) { SDL_SetError("Input self-test failed"); goto done; }
    SDL_RendererInfo info; if (SDL_GetRendererInfo(r,&info)) goto done;
    fprintf(stderr,"[field-viewer] Video=%s renderer=%s; top-down inspection.\n",SDL_GetCurrentVideoDriver(),info.name);
    uint32_t frames = 0;
    while (v->running) {
        SDL_Event e; while (SDL_PollEvent(&e)) event(v,s,&e);
        if (!v->running) break;
        if (!draw(r,o,s,v,frames == 0 ? o->capture : NULL)) goto done;
        if (++frames == o->frames) break;
        SDL_Delay(16);
    }
    fprintf(stderr,"[field-viewer] Exit after %" PRIu32 " frames.\n",frames); ok = 1;
done:
    if (!ok) fprintf(stderr,"[field-viewer] Runtime: %s\n",SDL_GetError());
    if (r) SDL_DestroyRenderer(r);
    if (win) SDL_DestroyWindow(win);
    SDL_Quit(); return ok;
}

int main(int argc, char **argv)
{
    struct options o = {0}; int parsed = parse(argc,argv,&o);
    if (parsed != 1) { usage(parsed == 2 ? stdout : stderr,argv[0]); return parsed == 2 ? 0 : 1; }
    struct scene s = {0};
    if (!load(&o,&s)) { scene_free(&s); return 1; }
    struct view v; reset(&v,&s); v.collision = o.collision;
    printf("zone=%" PRIu32 " area=%d matrix=%d matrix_size=%ux%u unique_regions=%u decoded_bytes=%zu\n",
           o.zone,s.zone.area_id,s.zone.matrix_id,s.matrix.width,s.matrix.height,s.count,s.decoded_bytes);
    for (unsigned i = 0; i < (unsigned)s.matrix.width*s.matrix.height; ++i)
        printf("matrix_cell=(%u,%u) field=%d\n",i%s.matrix.width,i/s.matrix.width,s.matrix.field_ids[i]);
    for (unsigned i = 0; i < s.count; ++i) {
        const struct region *r = &s.regions[i]; unsigned standard = 0, blocked = 0;
        for (unsigned j = 0; j < REGION_SIDE*REGION_SIDE; ++j) {
            standard += r->grid.tiles[j] == STANDARD; blocked += r->grid.tiles[j] == BLOCKED;
        }
        printf("field=%d grid=%ux%u standard=%u blocked=%u other=%u collision_triangles=%" PRIu32 "\n",
               r->id,r->grid.width,r->grid.height,standard,blocked,REGION_SIDE*REGION_SIDE-standard-blocked,r->collision.vertex_count/3);
    }
    for (const char *p = o.steps; p && *p; ++p)
        step(&v,&s,*p == 'R' ? 1 : *p == 'L' ? -1 : 0,*p == 'D' ? 1 : *p == 'U' ? -1 : 0);
    int ok = o.inspect || window(&o,&s,&v);
    report_marker(&s,&v); scene_free(&s); return ok ? 0 : 1;
}
