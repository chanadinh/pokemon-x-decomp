#include "assets/cgfx_model.h"
#include "assets/bch_model.h"
#include "assets/bch_material.h"
#include "assets/bch_scene.h"
#include "assets/bch_texture.h"
#include "assets/cgfx.h"
#include "assets/field.h"
#include "assets/garc.h"
#include "assets/lz.h"
#include "assets/zone.h"
#include "assets/gf_script.h"
#include "assets/gf_text.h"
#include "assets/zone_talk.h"
#include "render/light.h"
#include "render/raster.h"
#include "render/skin.h"
#include "render/text.h"

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
    int walk, skinned, quiet;
    const char *steps, *player_archive;
    uint32_t player_entry, player_section;
    int have_player_entry, have_player_section;
    uint32_t motion_section, motion_slot, motion_frame, walk_motion_slot;
    int have_motion_section, have_motion_slot, have_motion_frame, have_walk_motion;
    uint32_t zone, arrive, run_motion_slot, capture_at, threads;
    int have_zone, have_arrive, silent, have_run_motion, have_capture_at;
    const char *play; /* --zone: scripted input during display */
    int silly; /* --silly: 0 none, 1 giant, 2 moonwalk (for fun; not game behaviour) */
    uint32_t entry, subentry, model, frames, section;
    uint32_t texture_entry, texture_subentry, texture_section;
    int have_entry, have_subentry, have_model, demo, inspect, software, self_test, textured;
    int have_section, triangle_list, combiners;
    int allow_skeleton; /* --zone regions: retry a skeleton model as skinned */
    int no_props;       /* --zone: leave region props out (the earlier scenes) */
    int have_texture_entry, have_texture_subentry, have_texture_section;
};
/* --walk: an inspection marker on the GR terrain grid. Policy as in the
 * field viewer: only the exact word 0x00000020 admits the marker, which starts
 * at the first such cell in row-major order and moves one cardinal cell per
 * key. This is not reconstructed player movement or game collision. */
struct model_asset;
/* --zone: one indoor zone resolved from game data (see load_zone). NPCs stand
 * on their stored tiles and block them; warps fire on an attempted step into
 * their rectangle. */
struct zone_npc {
    struct model_asset *model; /* NULL when its model could not be loaded */
    int x, z, facing;
    uint16_t id, script, registry_model, model_entry;
};
enum { ZONE_MAX_PAGES = 64 };
/* A region prop (GR section 3) in the scene frame. */
struct zone_prop {
    uint32_t model; /* index into prop_models, UINT32_MAX when not drawn */
    uint32_t region, uid, entry;
    double x, y, z, yaw, scale[3];
};
struct zone_state {
    uint32_t zone, area, matrix, region, text_id;
    struct px_zone_entities entities;
    struct zone_npc *npcs;
    uint32_t npc_count;
    char message[192];
    /* Dialogue: the zone script (ZO section 2) and the zone's story text
     * (a/0/8/2, English, entry = header u16 at 6); either may be absent. */
    struct px_gf_script script;
    struct px_gf_text text;
    int has_script, has_text;
    char *pages[ZONE_MAX_PAGES]; /* open message window pages, NULL-free when closed */
    uint32_t page_count, page;
    /* Multi-region matrices (towns, routes): the zone's cells span cols x rows
     * matrix cells from cell (cell_x0, cell_z0), whose region is the scene
     * asset itself and defines the scene frame. cell_region holds per cell -1
     * (not the zone's), 0 (the scene asset) or k (regions[k - 1], drawn and
     * queried at region_offset[k - 1]). Single-region zones have none. */
    uint32_t cols, rows, cell_x0, cell_z0;
    double main_offset[2]; /* the scene asset's cell, from the origin cell */
    int32_t refused; /* a neighbouring zone that failed to load, or -1 */
    int32_t *cell_region;
    int16_t *subcell_zone; /* (cols * 4) x (rows * 4) matrix zone ids, 10 x 10 tiles each */
    struct model_asset *regions;
    double (*region_offset)[2];
    uint32_t *region_entry, region_count;
    struct zone_prop *props;
    uint32_t prop_count, prop_model_count, prop_failed;
    struct model_asset *prop_models;
    uint32_t *prop_model_entry;
};
struct walker {
    const struct px_field_grid *grid;
    const struct zone_state *zone; /* --zone only */
    int active, x, z;
    int facing; /* quarter turns about +Y: 0 +Z (D), 1 +X (R), 2 -Z (U), 3 -X (L) */
    uint32_t moves, blocked, warps;
    int last_moved; /* the latest attempted step moved */
    int32_t warp_hit; /* --zone: warp index the latest step attempted, or -1 */
};
struct camera {
    double yaw, pitch, zoom; int running; struct walker *walk;
    int zone;       /* --zone: WASD queue a step for the controller, Space/Return talk */
    char queued;    /* U, D, L, R or 0 */
    int interact;
    const char *play; /* --play cursor */
    int internal_w, internal_h; /* --zone: fixed render size, scaled to the window */
};
static int npc_at(const struct zone_state *zone, int x, int z)
{
    for (uint32_t i = 0; zone && i < zone->npc_count; ++i)
        if (zone->npcs[i].x == x && zone->npcs[i].z == z) return (int)i;
    return -1;
}
static int walkable(const struct walker *w, int x, int z)
{
    return x >= 0 && z >= 0 && x < w->grid->width && z < w->grid->height &&
           w->grid->tiles[(size_t)z * w->grid->width + (size_t)x] == UINT32_C(0x00000020) &&
           npc_at(w->zone, x, z) < 0;
}
static void walk_start(struct walker *w, const struct px_field_grid *grid)
{
    memset(w, 0, sizeof *w);
    w->grid = grid;
    w->warp_hit = -1;
    for (int z = 0; z < grid->height && !w->active; ++z)
        for (int x = 0; x < grid->width && !w->active; ++x)
            if (walkable(w, x, z)) { w->active = 1; w->x = x; w->z = z; }
}
/* One cardinal step: U = -Z, D = +Z, L = -X, R = +X in region-local space. */
static void walk_step(struct walker *w, char direction)
{
    if (!w->active) return;
    const int dx = direction == 'L' ? -1 : direction == 'R' ? 1 : 0;
    const int dz = direction == 'U' ? -1 : direction == 'D' ? 1 : 0;
    /* The marker turns toward every attempted step, blocked or not. */
    w->facing = direction == 'D' ? 0 : direction == 'R' ? 1 : direction == 'U' ? 2 : 3;
    w->warp_hit = -1;
    if (w->zone) {
        /* --zone: a step into a warp that is not receive-only asks for it;
         * the caller loads the target (and falls back to walk_move). */
        const int32_t k = px_zone_warp_at(&w->zone->entities, w->x + dx, w->z + dz);
        if (k >= 0 && w->zone->entities.warps[k].directionality != 2) {
            w->warp_hit = k; w->last_moved = 0; ++w->warps;
            return;
        }
    }
    if (walkable(w, w->x + dx, w->z + dz)) { w->x += dx; w->z += dz; ++w->moves; w->last_moved = 1; }
    else { ++w->blocked; w->last_moved = 0; }
}
/* The plain move of walk_step, for a warp whose target did not load. */
static void walk_move(struct walker *w)
{
    const int dx = w->facing == 1 ? 1 : w->facing == 3 ? -1 : 0, dz = w->facing == 0 ? 1 : w->facing == 2 ? -1 : 0;
    w->warp_hit = -1;
    if (walkable(w, w->x + dx, w->z + dz)) { w->x += dx; w->z += dz; ++w->moves; w->last_moved = 1; }
    else { ++w->blocked; w->last_moved = 0; }
}
/* Zone face direction (0 up, 1 down, 2 left, 3 right) as a walker facing. */
static int zone_facing(unsigned face) { static const int map[4] = {2, 0, 3, 1}; return map[face & 3]; }
/* Region-local cell center: (cell + 0.5) * 18 - 360, as the field viewer. */
static double cell_center(int cell) { return (cell + 0.5) * 18 - 360; }

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
    /* Per material and RGB channel: summed specular0 + specular1 registers
     * of unexecuted point/spot lights (an upper bound on fragment secondary
     * RGB if LUT, attenuation and dot factors stay within 0..1). */
    uint32_t *secondary_bound;
    uint32_t *light_index; /* 8 per material: environment light per setup slot */
    int lighting;
    uint32_t approximated; /* --environment: materials whose bump map or Fresnel is not executed */
    /* --walk only: the GR member's terrain grid and collision triangles. */
    struct px_field_grid grid;
    struct px_field_collision collision;
    struct walker walker;
    int walk;
    struct model_asset *player; /* --player-archive: drawn at the marker */
    struct zone_state *zone;    /* --zone */
    /* --motion: an unbaked copy of the skinned model, the GF1 pack and the
     * per-frame buffers that pose the baked arrays in place. */
    struct px_bch_model skin_source;
    struct px_gf_motion_pack motion_pack;
    const struct px_gf_motion *motion, *walk_motion, *run_motion, *posed_motion;
    double glide[2]; /* player offset from its cell center while a step plays */
    int silly, moonwalking; /* --silly mode; a moonwalk step is playing */
    float (*motion_locals)[9];
    double *motion_world, *motion_rest;
    int32_t attach_child, attach_target; /* -1 when no attachment applies */
    double attach_residual;
    uint32_t motion_start, motion_posed;
    int motion_fixed, motion_ready;
    int bch_textured;
};

static void asset_free(struct model_asset *asset)
{
    if (asset->player) { asset_free(asset->player); free(asset->player); }
    if (asset->zone) {
        for (uint32_t i = 0; i < asset->zone->npc_count; ++i)
            if (asset->zone->npcs[i].model) { asset_free(asset->zone->npcs[i].model); free(asset->zone->npcs[i].model); }
        free(asset->zone->npcs);
        px_zone_entities_free(&asset->zone->entities);
        px_gf_script_free(&asset->zone->script);
        px_gf_text_free(&asset->zone->text);
        for (uint32_t i = 0; i < asset->zone->page_count; ++i) free(asset->zone->pages[i]);
        for (uint32_t i = 0; asset->zone->regions && i < asset->zone->region_count; ++i) asset_free(&asset->zone->regions[i]);
        free(asset->zone->regions); free(asset->zone->region_offset); free(asset->zone->region_entry);
        free(asset->zone->cell_region); free(asset->zone->subcell_zone);
        for (uint32_t i = 0; i < asset->zone->prop_model_count; ++i) asset_free(&asset->zone->prop_models[i]);
        free(asset->zone->prop_models); free(asset->zone->prop_model_entry); free(asset->zone->props);
        free(asset->zone);
    }
    px_bch_model_free(&asset->skin_source);
    px_gf_motion_pack_free(&asset->motion_pack);
    free(asset->motion_locals); free(asset->motion_world); free(asset->motion_rest);
    if (asset->textures)
        for (uint32_t i = 0; i < asset->texture_count; ++i) px_texture_free(&asset->textures[i]);
    free(asset->textures);
    free(asset->draw_order);
    free(asset->bch_preview);
    free(asset->bch_combiner);
    free(asset->bch_texture);
    free(asset->bch_light);
    free(asset->point_bound);
    free(asset->secondary_bound);
    free(asset->light_index);
    px_bch_environment_free(&asset->environment);
    px_field_grid_free(&asset->grid);
    px_field_collision_free(&asset->collision);
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
        "  --skinned          BCH: accept a skeleton and draw its rest (bind) pose\n"
        "  --walk             BCH from a GR section: load that GR's terrain grid and\n"
        "                     collision and add an inspection marker (WASD moves it\n"
        "                     one cell; only exact standard cells 0x00000020 admit it)\n"
        "  --steps UDLR...    Apply marker moves before inspection or display (--walk)\n"
        "  --player-archive RELATIVE/PATH  With --walk: draw this skinned BCH (rest\n"
        "                     pose, its own textures) at the marker instead of a box;\n"
        "                     --player-entry N, --player-section N select it\n"
        "  --motion-section N  GF1 motion pack: this section of the skinned model's\n"
        "                     entry (the player's with --player-archive)\n"
        "  --motion N         Pose with motion slot N (1..), one motion frame per two\n"
        "                     rendered frames, looping; --motion-frame N starts (and\n"
        "                     with --inspect reports) frame N\n"
        "  --walk-motion N    With --player-archive and --motion: play slot N once\n"
        "                     per marker step while the player glides between cells;\n"
        "                     --motion loops otherwise\n"
        "  --zone N           Game-data scene: zone N of a/0/1/2 with its area\n"
        "                     textures, single-region matrix, GR terrain, collision,\n"
        "                     warps and NPCs, the player and its motions; WASD walk,\n"
        "                     Shift runs, Space talks; warps load their target zone.\n"
        "                     Combines only with --romfs, --lighting, --steps,\n"
        "                     --arrive N (start at warp N), --play UDLR (scripted\n"
        "                     input during display; u/d/l/r run, . talks) and\n"
        "                     run-time options\n"
        "  --inspect          Print decoded mesh metadata without a window\n"
        "  --capture-frame F  Save first viewport to a new BMP file\n"
        "  --frames N         Exit after a positive number of frames\n"
        "  --capture-at N     With --frames and --capture-frame: capture rendered\n"
        "                     frame N (0-based, default 0) instead of the first\n"
        "  --software         Force software rendering\n"
        "  --threads N        BCH scenes: draw in N row bands (1..8; default: CPUs)\n"
        "  --no-props         With --zone: do not load or draw region props\n"
        "  --silly MODE       With --zone, for fun: giant (a three-times player) or\n"
        "                     moonwalk (walks facing backwards, cycle reversed)\n"
        "  --self-test-input  Exercise orbit/zoom/reset/exit; default 3 frames\n"
        "Controls: arrows orbit, +/- zoom, R/Home reset, Escape exit.\n"
        "Orthographic wireframe by default; --textured uses a bounded software preview.\n"
        "Inspection camera; lighting only with --environment-archive; animation only with --motion.\n", program);
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
        if (!strcmp(arg, "--no-props")) { o->no_props = 1; continue; }
        if (!strcmp(arg, "--walk")) { o->walk = 1; continue; }
        if (!strcmp(arg, "--skinned")) { o->skinned = 1; continue; }
        if (strcmp(arg, "--romfs") && strcmp(arg, "--archive") && strcmp(arg, "--entry") &&
            strcmp(arg, "--subentry") && strcmp(arg, "--model") && strcmp(arg, "--frames") &&
            strcmp(arg, "--capture-frame") && strcmp(arg, "--section") &&
            strcmp(arg, "--texture-archive") && strcmp(arg, "--texture-entry") &&
            strcmp(arg, "--texture-subentry") && strcmp(arg, "--texture-section") &&
            strcmp(arg, "--environment-archive") && strcmp(arg, "--environment-entry") &&
            strcmp(arg, "--environment-subentry") && strcmp(arg, "--environment-section") &&
            strcmp(arg, "--scene") && strcmp(arg, "--lighting") && strcmp(arg, "--steps") &&
            strcmp(arg, "--player-archive") && strcmp(arg, "--player-entry") &&
            strcmp(arg, "--player-section") && strcmp(arg, "--motion-section") &&
            strcmp(arg, "--motion") && strcmp(arg, "--motion-frame") && strcmp(arg, "--walk-motion") &&
            strcmp(arg, "--zone") && strcmp(arg, "--arrive") && strcmp(arg, "--play") &&
            strcmp(arg, "--capture-at") && strcmp(arg, "--threads") && strcmp(arg, "--silly")) return 0;
        if (++i == argc || !argv[i][0]) return 0;
        const char *value = argv[i];
        if (!strcmp(arg, "--romfs")) o->romfs = value;
        else if (!strcmp(arg, "--archive")) o->archive = value;
        else if (!strcmp(arg, "--capture-frame")) o->capture = value;
        else if (!strcmp(arg, "--texture-archive")) o->texture_archive = value;
        else if (!strcmp(arg, "--environment-archive")) o->environment_archive = value;
        else if (!strcmp(arg, "--player-archive")) o->player_archive = value;
        else if (!strcmp(arg, "--play")) {
            if (o->play || strspn(value, "UDLRudlr.") != strlen(value) || strlen(value) > 4096) return 0;
            o->play = value;
        }
        else if (!strcmp(arg, "--steps")) {
            if (o->steps || strspn(value, "UDLR") != strlen(value) || strlen(value) > 4096) return 0;
            o->steps = value;
        }
        else if (!strcmp(arg, "--silly")) {
            if (o->silly) return 0;
            if (!strcmp(value, "giant")) o->silly = 1;
            else if (!strcmp(value, "moonwalk")) o->silly = 2;
            else return 0;
        }
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
            else if (!strcmp(arg, "--player-entry")) { o->player_entry = n; o->have_player_entry = 1; }
            else if (!strcmp(arg, "--motion-section")) {
                if (n > 255 || o->have_motion_section) return 0;
                o->motion_section = n; o->have_motion_section = 1;
            } else if (!strcmp(arg, "--motion")) {
                if (!n || o->have_motion_slot) return 0;
                o->motion_slot = n; o->have_motion_slot = 1;
            } else if (!strcmp(arg, "--zone")) {
                if (o->have_zone || n > 65535) return 0;
                o->zone = n; o->have_zone = 1;
            } else if (!strcmp(arg, "--threads")) {
                if (o->threads || !n || n > 8) return 0;
                o->threads = n;
            } else if (!strcmp(arg, "--capture-at")) {
                if (o->have_capture_at) return 0;
                o->capture_at = n; o->have_capture_at = 1;
            } else if (!strcmp(arg, "--arrive")) {
                if (o->have_arrive || n > 255) return 0;
                o->arrive = n; o->have_arrive = 1;
            } else if (!strcmp(arg, "--walk-motion")) {
                if (!n || o->have_walk_motion) return 0;
                o->walk_motion_slot = n; o->have_walk_motion = 1;
            } else if (!strcmp(arg, "--motion-frame")) {
                if (o->have_motion_frame) return 0;
                o->motion_frame = n; o->have_motion_frame = 1;
            }
            else if (!strcmp(arg, "--player-section")) {
                if (n > 255) return 0;
                o->player_section = n; o->have_player_section = 1;
            }
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
    if ((o->have_arrive || o->play) && !o->have_zone) return 0;
    if (o->have_capture_at && (!o->capture || !o->frames || o->capture_at >= o->frames)) return 0;
    if (o->have_zone) {
        /* --zone resolves the whole scene itself; only run-time and lighting
         * options combine with it. */
        if (o->demo || !o->romfs || o->archive || o->have_entry || o->have_subentry || o->have_model ||
            o->have_section || o->triangle_list || o->textured || o->combiners || o->texture_archive ||
            o->have_texture_entry || o->have_texture_subentry || o->have_texture_section ||
            o->environment_archive || o->have_environment_entry || o->have_environment_subentry ||
            o->have_environment_section || o->have_scene || o->walk || o->skinned || o->player_archive ||
            o->have_player_entry || o->have_player_section || o->have_motion_section || o->have_motion_slot ||
            o->have_motion_frame || o->have_walk_motion) return 0;
        if (o->inspect && (o->capture || o->frames || o->self_test || o->play)) return 0;
        if (o->self_test && !o->frames) o->frames = 3;
        if (!o->lighting) o->lighting = 1;
        return 1;
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
    if ((o->steps && !o->walk) || (o->walk && (o->demo || !o->have_section || !o->triangle_list || !o->textured)))
        return 0;
    if ((o->player_archive || o->have_player_entry || o->have_player_section) &&
        (!o->walk || !o->player_archive || !o->have_player_entry || !relative_path(o->player_archive)))
        return 0;
    if ((o->have_motion_section || o->have_motion_slot || o->have_motion_frame) &&
        (!o->have_motion_section || !o->have_motion_slot || (!o->skinned && !o->player_archive)))
        return 0;
    if (o->have_walk_motion && (!o->player_archive || !o->have_motion_slot)) return 0;
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
                              memcmp(out->data, "BM", 2) && memcmp(out->data, "MM", 2))) {
            snprintf(error, sizeof error, "--section requires an AD, GR, BM or MM minipack");
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
static int bind_bch_textures(const uint8_t *data, size_t size, struct model_asset *asset, int mixed)
{
    struct px_bch *pack = NULL;
    enum px_bch_result result = mixed ? px_bch_open_mixed(data, size, &pack) : px_bch_open(data, size, &pack);
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
    px_bch_close(pack);
    if (result == PX_BCH_UNSUPPORTED && !mixed) {
        /* A pack outside the texture-only subset (outdoor areas: raw formats,
         * extra relocation tags) is read again as a mixed BCH. */
        if (asset->textures)
            for (uint32_t i = 0; i < asset->texture_count; ++i) px_texture_free(&asset->textures[i]);
        free(asset->textures); asset->textures = NULL; asset->texture_count = 0;
        for (size_t i = 0; i < (size_t)asset->bch.material_count * 3; ++i) asset->bch_texture[i] = UINT32_MAX;
        return bind_bch_textures(data, size, asset, 1);
    }
    fprintf(stderr, "[model-viewer] BCH textures: %s\n", px_bch_result_string(result));
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
    asset->secondary_bound = calloc((size_t)bch->material_count * 3, sizeof *asset->secondary_bound);
    if (!asset->bch_light || !asset->point_bound || !asset->light_index || !asset->secondary_bound) return 0;
    asset->lighting = lighting;
    for (uint32_t i = 0; i < bch->material_count; ++i) {
        const struct px_bch_material *m = &bch->materials[i];
        /* Bump mapping and Fresnel (outdoor materials) are not executed: such
         * a material is lit with its interpolated normal and an unmodulated
         * alpha, an approximation counted in asset->approximated and
         * reported. */
        if (m->bump_mode == 1 || m->fresnel_selector) ++asset->approximated;
        if (!m->parameters_present || !(m->material_flags & 1) || m->bump_mode > 1) {
            fprintf(stderr, "[model-viewer] Material %" PRIu32 " uses unsupported fragment lighting state.\n", i); fprintf(stderr,"DBG p=%d f=%x fr=%d bump=%d\n",m->parameters_present,m->material_flags,m->fresnel_selector,m->bump_mode);
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
                for (unsigned c = 0; c < 3; ++c) {
                    asset->point_bound[(size_t)i * 3 + c] +=
                        px_light_register_channel(light->ambient[c], m->colors[1][c]) +
                        px_light_register_channel(light->diffuse[c], m->colors[2][c]);
                    asset->secondary_bound[(size_t)i * 3 + c] +=
                        px_light_register_channel(light->specular0[c], m->colors[3][c]) +
                        px_light_register_channel(light->specular1[c], m->colors[4][c]);
                }
                continue;
            }
            if (light->type != PX_BCH_LIGHT_FRAGMENT_DIRECTIONAL || (light->flags & 2)) {
                fprintf(stderr, "[model-viewer] Light set %" PRIu32 " uses an unsupported light.\n", set->index);
                return 0;
            }
            if ((lighting == 1 && name_is(&light->name, "light_night")) ||
                (lighting == 2 && name_is(&light->name, "light_sun"))) continue;
            /* Specular is not executed: an executed light must contribute a
             * zero specular register, so fragment secondary RGB is exactly 0
             * whatever the LUTs do; its alpha is 1 (Azahar specular_sum
             * starts at (0, 0, 0, 1); Fresnel is excluded above). */
            for (unsigned c = 0; c < 3; ++c)
                if (px_light_register_channel(light->specular0[c], m->colors[3][c]) ||
                    px_light_register_channel(light->specular1[c], m->colors[4][c])) {
                    fprintf(stderr, "[model-viewer] Material %" PRIu32 " needs specular lighting (not executed).\n", i);
                    return 0;
                }
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
        enum px_bch_model_result status = o->skinned ?
            px_bch_model_decode_skinned(data, size, o->model, bch) :
            px_bch_model_decode(data, size, o->model, bch);
        int skeleton = o->skinned;
        if (status == PX_BCH_MODEL_UNSUPPORTED && o->allow_skeleton) {
            /* --zone regions: a terrain model with a skeleton (rigid to its
             * bones) is decoded skinned and baked in its rest pose. */
            px_bch_model_free(bch);
            status = px_bch_model_decode_skinned(data, size, o->model, bch);
            skeleton = 1;
        }
        if (status != PX_BCH_MODEL_OK) {
            fprintf(stderr, "[model-viewer] BCH model: %s\n", px_bch_model_result_string(status));
            return 0;
        }
        if (skeleton && bch->bones && !px_skin_bake_rest(bch)) {
            fprintf(stderr, "[model-viewer] BCH skinning: rest pose failed\n");
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

/* Pose the baked (index-expanded) arrays at an integer motion frame; the
 * baked order is the source index order, so index k writes vertex k. */
static int pose_motion(struct model_asset *asset, const struct px_gf_motion *motion, uint32_t frame)
{
    if (!asset->motion_ready || !motion) return 0;
    if (frame == asset->motion_posed && motion == asset->posed_motion && asset->motion_fixed) return 1;
    const struct px_bch_model *m = &asset->skin_source;
    if (!px_skin_motion_locals(m, &asset->motion_pack, motion, frame, asset->motion_locals) ||
        !px_skin_world_from_locals(m->bones, m->bone_count, (const float (*)[9])asset->motion_locals,
                                   asset->motion_world) ||
        (asset->attach_child >= 0 && !px_skin_attach(asset->motion_rest, m->bone_count, (uint32_t)asset->attach_child,
                                                      (uint32_t)asset->attach_target, asset->motion_world)) ||
        !px_skin_pose_indices(m, asset->motion_world, asset->wire.positions, asset->bch.normals)) return 0;
    asset->motion_posed = frame; asset->posed_motion = motion; asset->motion_fixed = 1;
    return 1;
}

/* --motion: decode the GF1 pack, keep an unbaked copy of the skinned model
 * and check that the requested frame poses. */
static int load_motion(const struct options *o, const uint8_t *data, size_t size, struct model_asset *asset)
{
    struct resource pack = {0};
    if (!read_resource(o->romfs, o->archive, o->entry, o->subentry, 1, o->motion_section, "motion", &pack))
        return 0;
    enum px_gf_motion_result status = px_gf_motion_pack_decode(pack.data, pack.size, &asset->motion_pack);
    resource_free(&pack);
    if (status != PX_GF_MOTION_OK) {
        fprintf(stderr, "[model-viewer] GF1 motion pack: %s\n", px_gf_motion_result_string(status));
        return 0;
    }
    asset->motion = px_gf_motion_find(&asset->motion_pack, o->motion_slot);
    if (!asset->motion) {
        fprintf(stderr, "[model-viewer] GF1 motion slot %" PRIu32 " is absent or empty.\n", o->motion_slot);
        return 0;
    }
    if (px_bch_model_decode_skinned(data, size, o->model, &asset->skin_source) != PX_BCH_MODEL_OK ||
        asset->skin_source.index_count != asset->wire.index_count) return 0;
    const uint32_t bones = asset->skin_source.bone_count;
    asset->motion_locals = calloc(bones ? bones : 1, sizeof *asset->motion_locals);
    asset->motion_world = calloc(bones ? bones : 1, 12 * sizeof *asset->motion_world);
    asset->motion_rest = calloc(bones ? bones : 1, 12 * sizeof *asset->motion_rest);
    if (!asset->motion_locals || !asset->motion_world || !asset->motion_rest ||
        !px_skin_rest_world(asset->skin_source.bones, bones, asset->motion_rest)) return 0;
    /* Attachment policy: tr0002's rigid glasses are skinned to "glass", a
     * child of the root "model" that no motion animates, whose rest world
     * equals that of the motion bone "loc_acchat" (a head locator). The game's
     * accessory attachment is not traced; the viewer re-parents glass onto
     * loc_acchat only when both exist, glass is unanimated and their rest
     * worlds agree within 1e-4 in every element (residual printed). */
    asset->attach_child = asset->attach_target = -1;
    for (uint32_t i = 0; i < bones; ++i) {
        if (name_is(&asset->skin_source.bones[i].name, "glass")) asset->attach_child = (int32_t)i;
        if (name_is(&asset->skin_source.bones[i].name, "loc_acchat")) asset->attach_target = (int32_t)i;
    }
    asset->attach_residual = 0;
    if (asset->attach_child >= 0 && asset->attach_target >= 0) {
        for (unsigned j = 0; j < 12; ++j) {
            const double d = fabs(asset->motion_rest[(size_t)asset->attach_child * 12 + j] -
                                  asset->motion_rest[(size_t)asset->attach_target * 12 + j]);
            if (d > asset->attach_residual) asset->attach_residual = d;
        }
        for (uint32_t k = 0; k < asset->motion_pack.bone_count; ++k)
            if (!strcmp(asset->motion_pack.bones[k].name, "glass")) asset->attach_residual = INFINITY;
    }
    if (asset->attach_child < 0 || asset->attach_target < 0 || !(asset->attach_residual <= 1e-4))
        asset->attach_child = asset->attach_target = -1;
    asset->motion_start = o->have_motion_frame ? o->motion_frame : 0;
    asset->motion_fixed = 0;
    asset->motion_ready = 1;
    if (o->have_walk_motion && !(asset->walk_motion = px_gf_motion_find(&asset->motion_pack, o->walk_motion_slot))) {
        fprintf(stderr, "[model-viewer] GF1 walk motion slot %" PRIu32 " is absent or empty.\n", o->walk_motion_slot);
        return 0;
    }
    /* --motion-frame indexes the walk cycle when one is given (it starts the
     * last --steps move), otherwise the looping motion. */
    const uint32_t limit = asset->walk_motion ? asset->walk_motion->frames : asset->motion->frames;
    if (asset->motion_start > limit) {
        fprintf(stderr, "[model-viewer] --motion-frame exceeds the motion's %" PRIu32 " frames.\n", limit);
        return 0;
    }
    /* Motions loop with period frames (frame N repeats frame 0), as playback does. */
    const uint32_t idle_start = asset->walk_motion || !asset->motion->frames ? 0 :
                                asset->motion_start % asset->motion->frames;
    if (asset->walk_motion && !pose_motion(asset, asset->walk_motion, 0)) {
        fprintf(stderr, "[model-viewer] GF1 walk motion: pose failed\n");
        return 0;
    }
    if (!pose_motion(asset, asset->motion, idle_start)) {
        fprintf(stderr, "[model-viewer] GF1 motion: pose failed\n");
        return 0;
    }
    float low[3] = {0}, high[3] = {0};
    for (uint32_t k = 0; k < asset->wire.index_count; ++k)
        for (unsigned c = 0; c < 3; ++c) {
            const float v = asset->wire.positions[(size_t)k * 3 + c];
            if (!k || v < low[c]) low[c] = v;
            if (!k || v > high[c]) high[c] = v;
        }
    uint32_t matched = 0, world_space = 0;
    for (uint32_t i = 0; i < bones; ++i)
        for (uint32_t k = 0; k < asset->motion_pack.bone_count; ++k)
            if (name_is(&asset->skin_source.bones[i].name, asset->motion_pack.bones[k].name)) {
                ++matched; world_space += (asset->motion_pack.bones[k].flags & 8) != 0; break;
            }
    if (!o->silent) printf("motion_section=%" PRIu32 " motion=%" PRIu32 " frames=%" PRIu32 " pack_slots=%" PRIu32
           " pack_motions=%" PRIu32 " motion_bones=%" PRIu32 " model_bones=%" PRIu32 " matched=%" PRIu32
           " world_space=%" PRIu32 " start_frame=%" PRIu32 " bytes=%" PRIu32
           " posed_min=%.4f,%.4f,%.4f posed_max=%.4f,%.4f,%.4f attach=%s residual=%.3g\n",
           o->motion_section, o->motion_slot, asset->motion->frames, asset->motion_pack.slot_count,
           asset->motion_pack.motion_count, asset->motion_pack.bone_count, bones, matched, world_space,
           asset->motion_start, asset->motion->byte_length, low[0], low[1], low[2], high[0], high[1], high[2],
           asset->attach_child >= 0 ? "glass:loc_acchat" : "none", asset->attach_child >= 0 ? asset->attach_residual : 0.0);
    if (o->have_run_motion &&
        !(asset->run_motion = px_gf_motion_find(&asset->motion_pack, o->run_motion_slot))) {
        fprintf(stderr, "[model-viewer] GF1 run motion slot %" PRIu32 " is absent or empty.\n", o->run_motion_slot);
        return 0;
    }
    if (asset->walk_motion && !o->silent)
        printf("walk_motion=%" PRIu32 " frames=%" PRIu32 " playback=one-cycle-per-step glide=linear-over-cycle\n",
               o->walk_motion_slot, asset->walk_motion->frames);
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
    struct resource model = {0};
    if (!read_resource(o->romfs, o->archive, o->entry, o->subentry, o->have_section,
                       o->section, "model", &model)) return 0;
    int ok = decode_model(o, model.data, model.size, asset);
    if (ok && o->have_motion_slot && o->skinned && !o->player_archive)
        ok = load_motion(o, model.data, model.size, asset);
    if (ok && o->walk) {
        const uint8_t *gr = model.decoded ? model.decoded : model.member;
        const size_t gr_size = model.decoded ? model.decoded_size : model.member_size;
        enum px_field_result status = gr_size >= 2 && !memcmp(gr, "GR", 2) ?
            px_field_grid_decode(gr, gr_size, &asset->grid) : PX_FIELD_UNSUPPORTED;
        if (status == PX_FIELD_OK) status = px_field_collision_decode(gr, gr_size, &asset->collision);
        if (status != PX_FIELD_OK) {
            fprintf(stderr, "[model-viewer] --walk GR terrain: %s\n", px_field_result_string(status));
            ok = 0;
        } else {
            asset->walk = 1;
            walk_start(&asset->walker, &asset->grid);
            for (const char *p = o->steps; p && *p; ++p) walk_step(&asset->walker, *p);
        }
    }
    if (ok && !o->quiet) print_resource("", o->archive, o->entry, o->subentry, o->model,
                                        o->have_section, o->section, &model);
    resource_free(&model);
    if (ok && asset->bch_textured) {
        struct resource textures = {0};
        ok = read_resource(o->romfs, o->texture_archive, o->texture_entry, o->texture_subentry,
                           o->have_texture_section, o->texture_section, "texture", &textures) &&
             bind_bch_textures(textures.data, textures.size, asset, o->skinned);
        if (ok && !o->quiet) print_resource("texture_", o->texture_archive, o->texture_entry,
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
        if (ok && !o->quiet) print_resource("environment_", o->environment_archive, o->environment_entry,
                               o->environment_subentry, o->scene, o->have_environment_section,
                               o->environment_section, &environment);
        resource_free(&environment);
    }
    if (ok && o->player_archive) {
        /* The player: skinned, rest pose, textures from its own BCH, the
         * room's environment for lighting, the same shading mode. */
        struct options po = *o;
        po.archive = o->player_archive; po.entry = o->player_entry; po.subentry = 0; po.model = 0;
        po.have_section = o->have_player_section; po.section = o->player_section;
        po.texture_archive = po.archive; po.texture_entry = po.entry; po.texture_subentry = 0;
        po.have_texture_section = po.have_section; po.texture_section = po.section;
        po.skinned = 1; po.walk = 0; po.steps = NULL; po.player_archive = NULL; po.quiet = 1;
        asset->player = calloc(1, sizeof *asset->player);
        ok = asset->player && load(&po, asset->player);
        if (ok && !o->silent) printf("player_archive=%s player_entry=%" PRIu32 " player_section=%s%" PRIu32 "\n",
                       po.archive, po.entry, po.have_section ? "" : "none:", po.section);
    }
    return ok;
}

/* --zone scene policy. Resolved from game data: zone header (a/0/1/2) area
 * and matrix, the matrix's single region (a/0/4/2 -> a/0/4/1 GR section 1
 * model, section 0 grid, section 2 collision), the area's world textures
 * (a/0/1/3 AD section 11), zone entities (ZO section 1) and the area's NPC
 * registry (a/1/4/9). Inspection choices, not traced: environment a/0/0/3
 * entry 137 for every zone (the only one with lights), the player model
 * a/0/2/1 entry 2 with motion slots 1 (idle), 3 (walk) and 4 (run), NPCs
 * with their MM section 1 slot 1 when it poses, and the arrival rule. */
static int load_character(const struct options *o, uint32_t entry, int motion, struct model_asset **out)
{
    struct options po = *o;
    po.archive = "a/0/2/1"; po.entry = entry; po.subentry = 0; po.model = 0;
    po.have_section = 1; po.section = 0;
    po.texture_archive = po.archive; po.texture_entry = entry; po.texture_subentry = 0;
    po.have_texture_section = 1; po.texture_section = 0;
    po.skinned = 1; po.walk = 0; po.steps = NULL; po.player_archive = NULL; po.quiet = 1; po.silent = 1;
    po.have_motion_section = po.have_motion_slot = motion; po.motion_section = 1; po.motion_slot = 1;
    po.have_motion_frame = 0; po.have_walk_motion = 0; po.have_run_motion = 0;
    *out = calloc(1, sizeof **out);
    if (!*out) return 0;
    if (load(&po, *out)) return 1;
    asset_free(*out); free(*out); *out = NULL;
    return 0;
}

/* Multi-region matrix: the zone's cells are those with a region and at
 * least one of their 16 sub-cell zone ids equal to the zone; a matrix without
 * a zone table (each such matrix is used by one zone) gives the zone every
 * cell with a region. The scene frame is the region at the bounding box's
 * first cell, which must be one of them. */
static int cell_member(const struct px_field_matrix *m, uint32_t cx, uint32_t cz, uint32_t zone)
{
    if (m->field_ids[(size_t)cz * m->width + cx] < 0) return 0;
    if (!m->zone_ids) return 1;
    for (unsigned k = 0; k < 16; ++k)
        if (m->zone_ids[((size_t)cz * 4 + k / 4) * m->width * 4 + (size_t)cx * 4 + k % 4] == (int32_t)zone) return 1;
    return 0;
}
static int zone_cells(struct zone_state *zs, const struct px_field_matrix *m)
{
    uint32_t x0 = UINT32_MAX, z0 = UINT32_MAX, x1 = 0, z1 = 0;
    for (uint32_t cz = 0; cz < m->height; ++cz)
        for (uint32_t cx = 0; cx < m->width; ++cx) {
            if (!cell_member(m, cx, cz, zs->zone)) continue;
            if (cx < x0) x0 = cx;
            if (cz < z0) z0 = cz;
            if (cx > x1) x1 = cx;
            if (cz > z1) z1 = cz;
        }
    if (x0 == UINT32_MAX) {
        fprintf(stderr, "[model-viewer] zone %" PRIu32 ": no cell of matrix %" PRIu32 " belongs to it.\n", zs->zone, zs->matrix);
        return 0;
    }
    /* The scene adds a ring of neighbouring cells (any zone's), so that the
     * view and the walk continue past the zone's edge. */
    uint32_t mx = UINT32_MAX, mz = 0;
    for (uint32_t cz = z0; cz <= z1 && mx == UINT32_MAX; ++cz)
        for (uint32_t cx = x0; cx <= x1 && mx == UINT32_MAX; ++cx)
            if (cell_member(m, cx, cz, zs->zone)) { mx = cx; mz = cz; }
    if (x0) --x0;
    if (z0) --z0;
    if (x1 + 1 < m->width) ++x1;
    if (z1 + 1 < m->height) ++z1;
    zs->cell_x0 = x0; zs->cell_z0 = z0; zs->cols = x1 - x0 + 1; zs->rows = z1 - z0 + 1;
    zs->main_offset[0] = 720.0 * (mx - x0); zs->main_offset[1] = 720.0 * (mz - z0);
    if (zs->cols * zs->rows > 64) {
        fprintf(stderr, "[model-viewer] zone %" PRIu32 " spans %ux%u cells; at most 64 are supported.\n", zs->zone,
                zs->cols, zs->rows);
        return 0;
    }
    zs->cell_region = malloc((size_t)zs->cols * zs->rows * sizeof *zs->cell_region);
    if (!zs->cell_region) return 0;
    for (uint32_t cz = 0; cz < zs->rows; ++cz)
        for (uint32_t cx = 0; cx < zs->cols; ++cx) {
            const int loaded = m->field_ids[(size_t)(cz + z0) * m->width + cx + x0] >= 0;
            zs->cell_region[(size_t)cz * zs->cols + cx] =
                !loaded ? -1 : cx + x0 == mx && cz + z0 == mz ? 0 : (int32_t)++zs->region_count;
        }
    zs->subcell_zone = malloc((size_t)zs->cols * zs->rows * 16 * sizeof *zs->subcell_zone);
    if (!zs->subcell_zone) return 0;
    for (uint32_t z = 0; z < zs->rows * 4; ++z)
        for (uint32_t x = 0; x < zs->cols * 4; ++x)
            zs->subcell_zone[(size_t)z * zs->cols * 4 + x] = m->zone_ids ?
                m->zone_ids[((size_t)z0 * 4 + z) * m->width * 4 + (size_t)x0 * 4 + x] : (int16_t)zs->zone;
    zs->region = (uint32_t)m->field_ids[(size_t)mz * m->width + mx];
    return 1;
}

/* Load the zone's other regions (model, textures, environment, terrain) and
 * replace the scene grid by the composite of all its cells (40 x 40 tiles
 * each; cells not in the zone are blocked). */
static int load_regions(const struct options *zo, const struct px_field_matrix *m, struct model_asset *asset)
{
    struct zone_state *zs = asset->zone;
    zs->regions = calloc(zs->region_count, sizeof *zs->regions);
    zs->region_offset = calloc(zs->region_count, sizeof *zs->region_offset);
    zs->region_entry = calloc(zs->region_count, sizeof *zs->region_entry);
    if (!zs->regions || !zs->region_offset || !zs->region_entry) return 0;
    struct px_field_grid grid = {0};
    grid.width = (uint16_t)(zs->cols * 40); grid.height = (uint16_t)(zs->rows * 40);
    memcpy(grid.sections, asset->grid.sections, sizeof grid.sections);
    grid.tiles = malloc((size_t)grid.width * grid.height * sizeof *grid.tiles);
    if (!grid.tiles) return 0;
    for (size_t i = 0; i < (size_t)grid.width * grid.height; ++i) grid.tiles[i] = UINT32_C(0x01000021);
    for (uint32_t cz = 0; cz < zs->rows; ++cz)
        for (uint32_t cx = 0; cx < zs->cols; ++cx) {
            const int32_t k = zs->cell_region[(size_t)cz * zs->cols + cx];
            if (k < 0) continue;
            struct model_asset *r = asset;
            if (k > 0) {
                struct options ro = *zo;
                ro.entry = (uint32_t)m->field_ids[(size_t)(cz + zs->cell_z0) * m->width + cx + zs->cell_x0];
                ro.player_archive = NULL; ro.have_motion_slot = ro.have_motion_section = 0;
                ro.have_walk_motion = ro.have_run_motion = 0; ro.quiet = 1;
                r = &zs->regions[k - 1];
                zs->region_entry[k - 1] = ro.entry;
                zs->region_offset[k - 1][0] = 720.0 * cx; zs->region_offset[k - 1][1] = 720.0 * cz;
                if (!load(&ro, r)) {
                    int own = 0;
                    for (unsigned s16 = 0; s16 < 16; ++s16)
                        own |= zs->subcell_zone[((size_t)cz * 4 + s16 / 4) * zs->cols * 4 + (size_t)cx * 4 + s16 % 4] ==
                               (int32_t)zs->zone;
                    fprintf(stderr, "[model-viewer] zone %" PRIu32 ": region %" PRIu32 " failed to load%s.\n", zs->zone,
                            ro.entry, own ? "" : " (a neighbouring cell; left out)");
                    if (own) { free(grid.tiles); return 0; }
                    /* A neighbour outside the zone is left out: blocked, not drawn. */
                    asset_free(r);
                    zs->cell_region[(size_t)cz * zs->cols + cx] = -1;
                    continue;
                }
            }
            if (r->grid.width != 40 || r->grid.height != 40) {
                fprintf(stderr, "[model-viewer] zone %" PRIu32 ": a region grid is %ux%u, not 40x40.\n", zs->zone,
                        r->grid.width, r->grid.height);
                free(grid.tiles); return 0;
            }
            for (uint32_t z = 0; z < 40; ++z)
                memcpy(grid.tiles + ((size_t)cz * 40 + z) * grid.width + (size_t)cx * 40, r->grid.tiles + (size_t)z * 40,
                       40 * sizeof *grid.tiles);
        }
    px_field_grid_free(&asset->grid);
    asset->grid = grid;
    /* Start on the first walkable tile of the zone's own sub-cells. */
    walk_start(&asset->walker, &asset->grid);
    asset->walker.active = 0;
    for (int z = 0; z < asset->grid.height && !asset->walker.active; ++z)
        for (int x = 0; x < asset->grid.width && !asset->walker.active; ++x)
            if (walkable(&asset->walker, x, z) &&
                zs->subcell_zone[(size_t)(z / 10) * zs->cols * 4 + (size_t)(x / 10)] == (int32_t)zs->zone) {
                asset->walker.active = 1; asset->walker.x = x; asset->walker.z = z;
            }
    return 1;
}

/* Region props: every region's GR section 3, each prop's model the a/0/2/4
 * BM entry its uid names in the area registry (AD section 0; the uid itself
 * when absent, as CTRMap), section 0, textured from the area's AD section 1,
 * lit like the region, rest pose. Placement: scale, then rotation about +Y
 * by the stored degrees, then the position less the scene origin. A prop
 * whose model fails to load is listed and not drawn. */
static int load_props(const struct options *zo, struct model_asset *asset)
{
    struct zone_state *zs = asset->zone;
    struct resource r = {0};
    struct px_prop_registry registry = {0};
    int have_registry = 0;
    /* Sections are read raw: a count of 16 or 17 would look like an LZ header. */
    struct px_field_span span;
    if (read_resource(zo->romfs, "a/0/1/3", zs->area, 0, 0, 0, "prop registry", &r)) {
        have_registry = px_field_section(r.data, r.size, "AD", 0, &span) == PX_FIELD_OK &&
                        px_prop_registry_decode(span.data, span.size, &registry) == PX_ZONE_OK;
        resource_free(&r);
    }
    const double origin_x = 360.0 + 720.0 * zs->cell_x0, origin_z = 360.0 + 720.0 * zs->cell_z0;
    int ok = 1;
    for (uint32_t k = 0; k <= zs->region_count && ok; ++k) {
        const uint32_t region = k ? zs->region_entry[k - 1] : zs->region;
        struct px_gr_props props = {0};
        if (!read_resource(zo->romfs, "a/0/4/1", region, 0, 0, 0, "region props", &r)) continue;
        const enum px_zone_result pr = px_field_section(r.data, r.size, "GR", 3, &span) == PX_FIELD_OK ?
            px_gr_props_decode(span.data, span.size, &props) : PX_ZONE_TRUNCATED;
        resource_free(&r);
        if (pr != PX_ZONE_OK) {
            fprintf(stderr, "[model-viewer] region %" PRIu32 " props: %s\n", region, px_zone_result_string(pr));
            continue;
        }
        if (props.count) {
            struct zone_prop *grown = realloc(zs->props, (size_t)(zs->prop_count + props.count) * sizeof *grown);
            if (!grown) { px_gr_props_free(&props); ok = 0; break; }
            zs->props = grown;
        }
        for (uint32_t i = 0; i < props.count; ++i) {
            const struct px_gr_prop *g = &props.props[i];
            const struct px_prop_registry_entry *e = have_registry ? px_prop_registry_find(&registry, g->uid) : NULL;
            struct zone_prop *z = &zs->props[zs->prop_count++];
            memset(z, 0, sizeof *z);
            z->region = region; z->uid = g->uid; z->entry = e ? e->model : g->uid;
            z->x = g->position[0] - origin_x; z->y = g->position[1]; z->z = g->position[2] - origin_z;
            z->yaw = g->rotation[1] * (3.14159265358979323846 / 180.0);
            for (unsigned c = 0; c < 3; ++c) z->scale[c] = g->scale[c];
            z->model = UINT32_MAX;
            if (g->rotation[0] != 0 || g->rotation[2] != 0) { ++zs->prop_failed; continue; } /* only yaw */
            for (uint32_t m = 0; m < zs->prop_model_count; ++m)
                if (zs->prop_model_entry[m] == z->entry) z->model = m;
            if (z->model != UINT32_MAX) {
                if (!zs->prop_models[z->model].bch.mesh_count) z->model = UINT32_MAX, ++zs->prop_failed;
                continue;
            }
            struct model_asset *models = realloc(zs->prop_models, (size_t)(zs->prop_model_count + 1) * sizeof *models);
            uint32_t *entries = models ? realloc(zs->prop_model_entry, (size_t)(zs->prop_model_count + 1) * sizeof *entries) : NULL;
            if (models) zs->prop_models = models;
            if (!entries) { ok = 0; break; }
            zs->prop_model_entry = entries;
            struct model_asset *model = &zs->prop_models[zs->prop_model_count];
            memset(model, 0, sizeof *model);
            zs->prop_model_entry[zs->prop_model_count] = z->entry;
            struct options po = *zo;
            po.archive = "a/0/2/4"; po.entry = z->entry; po.subentry = 0; po.model = 0;
            po.have_section = 1; po.section = 0;
            po.texture_section = 1;
            po.walk = 0; po.player_archive = NULL; po.quiet = 1;
            po.have_motion_slot = po.have_motion_section = po.have_walk_motion = po.have_run_motion = 0;
            if (load(&po, model) && model->bch.mesh_count) z->model = zs->prop_model_count;
            else { asset_free(model); ++zs->prop_failed; }
            ++zs->prop_model_count;
        }
        px_gr_props_free(&props);
    }
    px_prop_registry_free(&registry);
    return ok;
}

static int load_zone(const struct options *o, uint32_t zone, struct model_asset *asset)
{
    struct zone_state *zs = calloc(1, sizeof *zs);
    if (!zs) return 0;
    zs->zone = zone;
    zs->refused = -1;
    asset->zone = zs; /* owned by asset from here; asset_free releases it */
    struct resource r = {0};
    struct px_field_zone header;
    struct px_field_span span;
    if (!read_resource(o->romfs, "a/0/1/2", zone, 0, 0, 0, "zone", &r)) return 0;
    enum px_field_result fs = px_field_zone_decode(r.data, r.size, &header);
    if (fs == PX_FIELD_OK) fs = px_field_section(r.data, r.size, "ZO", 1, &span);
    if (fs != PX_FIELD_OK) {
        fprintf(stderr, "[model-viewer] zone %" PRIu32 ": %s\n", zone, px_field_result_string(fs));
        resource_free(&r); return 0;
    }
    const enum px_zone_result zr = px_zone_entities_decode(span.data, span.size, &zs->entities);
    resource_free(&r);
    if (zr != PX_ZONE_OK) {
        fprintf(stderr, "[model-viewer] zone %" PRIu32 " entities: %s\n", zone, px_zone_result_string(zr));
        return 0;
    }
    zs->area = header.area_id; zs->matrix = header.matrix_id;
    zs->text_id = (uint32_t)header.raw_header[6] | (uint32_t)header.raw_header[7] << 8;
    {
        struct px_field_span script;
        if (read_resource(o->romfs, "a/0/1/2", zone, 0, 0, 0, "zone", &r)) {
            if (px_field_section(r.data, r.size, "ZO", 2, &script) == PX_FIELD_OK &&
                px_gf_script_decode(script.data, script.size, &zs->script) == PX_GF_SCRIPT_OK) zs->has_script = 1;
            resource_free(&r);
        }
    }
    {
        /* Optional: without the story text archive NPCs still face the player. */
        char path[512];
        snprintf(path, sizeof path, "%s/a/0/8/2", o->romfs);
        FILE *probe = fopen(path, "rb");
        if (probe) {
            fclose(probe);
            if (read_resource(o->romfs, "a/0/8/2", zs->text_id, 0, 0, 0, "story text", &r)) {
                zs->has_text = px_gf_text_decode(r.data, r.size, &zs->text) == PX_GF_TEXT_OK;
                resource_free(&r);
            }
        }
    }
    struct px_field_matrix matrix = {0};
    if (!read_resource(o->romfs, "a/0/4/2", zs->matrix, 0, 0, 0, "matrix", &r)) return 0;
    fs = px_field_matrix_decode(r.data, r.size, &matrix);
    resource_free(&r);
    int ok = fs == PX_FIELD_OK;
    if (!ok) fprintf(stderr, "[model-viewer] matrix %" PRIu32 ": %s\n", zs->matrix, px_field_result_string(fs));
    else if (matrix.width == 1 && matrix.height == 1) {
        if (matrix.field_ids[0] < 0) {
            fprintf(stderr, "[model-viewer] zone %" PRIu32 ": matrix %" PRIu32 " has no region.\n", zone, zs->matrix);
            ok = 0;
        } else zs->region = (uint32_t)matrix.field_ids[0];
    } else ok = zone_cells(zs, &matrix);
    if (!ok) { px_field_matrix_free(&matrix); return 0; }
    struct options zo = *o;
    zo.have_zone = 0; zo.have_arrive = 0;
    zo.archive = "a/0/4/1"; zo.entry = zs->region; zo.have_entry = 1; zo.subentry = 0; zo.model = 0;
    zo.have_section = 1; zo.section = 1;
    zo.triangle_list = zo.textured = zo.combiners = 1;
    zo.texture_archive = "a/0/1/3"; zo.texture_entry = zs->area; zo.have_texture_entry = 1;
    zo.texture_subentry = 0; zo.have_texture_section = 1; zo.texture_section = 11;
    zo.environment_archive = "a/0/0/3"; zo.environment_entry = 137; zo.have_environment_entry = 1;
    zo.environment_subentry = 0; zo.have_environment_section = 1; zo.environment_section = 0; zo.scene = 0;
    zo.walk = 1; zo.steps = NULL; zo.allow_skeleton = 1;
    zo.player_archive = "a/0/2/1"; zo.player_entry = 2; zo.have_player_entry = 1;
    zo.player_section = 0; zo.have_player_section = 1;
    zo.have_motion_section = zo.have_motion_slot = 1; zo.motion_section = 1; zo.motion_slot = 1;
    zo.have_motion_frame = 0; zo.have_walk_motion = 1; zo.walk_motion_slot = 3;
    zo.have_run_motion = 1; zo.run_motion_slot = 4;
    if (!load(&zo, asset) || (zs->cell_region && !load_regions(&zo, &matrix, asset))) {
        px_field_matrix_free(&matrix); return 0;
    }
    px_field_matrix_free(&matrix);
    if (!o->no_props && !load_props(&zo, asset)) return 0;
    /* Entities store world tiles (40 per cell): move them into the scene frame. */
    const int32_t tile_dx = 40 * (int32_t)zs->cell_x0, tile_dz = 40 * (int32_t)zs->cell_z0;
    for (uint32_t i = 0; i < zs->entities.warp_count; ++i) {
        zs->entities.warps[i].tile_x -= tile_dx; zs->entities.warps[i].tile_y -= tile_dz;
    }
    struct walker *w = &asset->walker;
    w->zone = zs;
    if (npc_at(zs, w->x, w->z) >= 0) { /* never start inside an NPC */
        w->active = 0;
        for (int z = 0; z < asset->grid.height && !w->active; ++z)
            for (int x = 0; x < asset->grid.width && !w->active; ++x)
                if (walkable(w, x, z)) { w->active = 1; w->x = x; w->z = z; }
    }
    /* NPCs: registry of the zone's area maps each NPC's model id to an
     * a/0/2/1 entry. An NPC whose model is missing or fails to load still
     * blocks its tile. */
    struct px_npc_registry registry = {0};
    if (zs->entities.npc_count) {
        if (!read_resource(o->romfs, "a/1/4/9", zs->area, 0, 0, 0, "NPC registry", &r)) return 0;
        const enum px_zone_result rr = px_npc_registry_decode(r.data, r.size, &registry);
        resource_free(&r);
        if (rr != PX_ZONE_OK) {
            fprintf(stderr, "[model-viewer] NPC registry %" PRIu32 ": %s\n", zs->area, px_zone_result_string(rr));
            return 0;
        }
        zs->npcs = calloc(zs->entities.npc_count, sizeof *zs->npcs);
        if (!zs->npcs) { px_npc_registry_free(&registry); return 0; }
    }
    for (uint32_t i = 0; i < zs->entities.npc_count; ++i) {
        const struct px_zone_npc *n = &zs->entities.npcs[i];
        struct zone_npc *z = &zs->npcs[zs->npc_count++];
        z->x = n->tile_x - tile_dx; z->z = n->tile_y - tile_dz; z->facing = zone_facing(n->face);
        z->id = n->id; z->script = n->script; z->registry_model = n->model; z->model_entry = UINT16_MAX;
        const struct px_npc_registry_entry *e = px_npc_registry_find(&registry, n->model);
        if (e) {
            z->model_entry = e->model;
            if (!load_character(&zo, e->model, 1, &z->model)) load_character(&zo, e->model, 0, &z->model);
        }
        if (!o->silent)
            printf("npc index=%" PRIu32 " id=%u registry_model=%u model_entry=%s%u tile=%d,%d face=%u script=%u "
                   "stored_height=%.9g drawn=%s posed=%s\n", i, z->id, z->registry_model, e ? "" : "none:",
                   e ? e->model : 0, z->x, z->z, n->face, z->script, n->height, z->model ? "yes" : "no",
                   z->model && z->model->motion_ready ? "slot1" : "rest");
    }
    px_npc_registry_free(&registry);
    if (!o->silent) {
        printf("zone=%" PRIu32 " area=%" PRIu32 " matrix=%" PRIu32 " region=%" PRIu32 " furniture=%" PRIu32
               " npcs=%" PRIu32 " warps=%" PRIu32 " triggers=%" PRIu32 ",%" PRIu32 " environment=a/0/0/3:137 "
               "player=a/0/2/1:2 motions=idle:1,walk:3,run:4\n", zone, zs->area, zs->matrix, zs->region,
               zs->entities.furniture_count, zs->entities.npc_count, zs->entities.warp_count,
               zs->entities.trigger1_count, zs->entities.trigger2_count);
        if (zs->cell_region) {
            printf("zone_regions zone=%" PRIu32 " matrix=%" PRIu32 " cells=%ux%u origin_cell=%u,%u scene=%" PRIu32
                   "@%.0f,%.0f", zone, zs->matrix, zs->cols, zs->rows, zs->cell_x0, zs->cell_z0, zs->region,
                   zs->main_offset[0], zs->main_offset[1]);
            for (uint32_t i = 0; i < zs->region_count; ++i)
                printf(" region=%" PRIu32 "@%.0f,%.0f", zs->region_entry[i], zs->region_offset[i][0], zs->region_offset[i][1]);
            printf(" tile_origin=%u,%u\n", 40 * zs->cell_x0, 40 * zs->cell_z0);
        }
        uint32_t drawn = 0;
        for (uint32_t i = 0; i < zs->prop_count; ++i) {
            const struct zone_prop *pr = &zs->props[i];
            drawn += pr->model != UINT32_MAX;
            printf("prop index=%" PRIu32 " region=%" PRIu32 " uid=%" PRIu32 " model_entry=a/0/2/4:%" PRIu32
                   " position=%.9g,%.9g,%.9g yaw_degrees=%.9g scale=%.9g,%.9g,%.9g drawn=%s\n", i, pr->region, pr->uid,
                   pr->entry, pr->x, pr->y, pr->z, pr->yaw * (180.0 / 3.14159265358979323846), pr->scale[0],
                   pr->scale[1], pr->scale[2], pr->model != UINT32_MAX ? "yes" : "no");
        }
        printf("zone_props zone=%" PRIu32 " props=%" PRIu32 " models=%" PRIu32 " drawn=%" PRIu32 " not_drawn=%" PRIu32
               " registry=a/0/1/3:%" PRIu32 ":0 textures=a/0/1/3:%" PRIu32 ":1 pose=rest\n", zone, zs->prop_count,
               zs->prop_model_count, drawn, zs->prop_count - drawn, zs->area, zs->area);
        uint32_t approximated = asset->approximated;
        for (uint32_t i = 0; i < zs->region_count; ++i) approximated += zs->regions[i].approximated;
        for (uint32_t i = 0; i < zs->prop_model_count; ++i) approximated += zs->prop_models[i].approximated;
        printf("zone_lighting zone=%" PRIu32 " unexecuted_bump_or_fresnel_materials=%" PRIu32 "\n", zone, approximated);
        printf("dialogue zone=%" PRIu32 " script=%s text=a/0/8/2:%" PRIu32 ":%s lines=%" PRIu32
               " natives=policy-inferred female=1\n", zone, zs->has_script ? "yes" : "no", zs->text_id,
               zs->has_text ? "yes" : "no", zs->has_text ? zs->text.line_count : 0);
        for (uint32_t i = 0; zs->has_script && i < zs->npc_count; ++i) {
            struct px_zone_talk talk;
            px_zone_talk(&zs->script, zs->npcs[i].script, 1, &talk);
            printf("npc_dialogue index=%" PRIu32 " script=%u result=%s steps=%" PRIu32 " lines=", i,
                   zs->npcs[i].script, px_gf_script_result_string(talk.run.result), talk.run.steps);
            for (uint32_t k = 0; k < talk.line_count; ++k) printf("%s%" PRId32, k ? "," : "", talk.lines[k]);
            if (!talk.line_count) printf("none");
            putchar('\n');
        }
        for (uint32_t i = 0; i < zs->entities.warp_count; ++i) {
            const struct px_zone_warp *wp = &zs->entities.warps[i];
            if (!wp->supported) {
                printf("warp index=%" PRIu32 " target_zone=%u target_warp=%u coordinate_type=%u supported=no\n",
                       i, wp->target_zone, wp->target_warp, wp->coordinate_type);
                continue;
            }
            printf("warp index=%" PRIu32 " target_zone=%u target_warp=%u tiles=%d,%d+%dx%d face=%u transition=%u "
                   "directionality=%u height=%d supported=yes\n", i, wp->target_zone, wp->target_warp,
                   wp->tile_x, wp->tile_y, wp->width, wp->length, wp->face, wp->transition, wp->directionality,
                   wp->height);
        }
    }
    return 1;
}

/* Arrival policy (inspection, consistent with the three house warps): stand
 * on the target warp's first tile facing its face direction, then step once
 * that way when the cell is walkable and not itself a warp. */
static int zone_arrive(struct model_asset *asset, uint32_t index)
{
    const struct zone_state *zs = asset->zone;
    if (!zs || index >= zs->entities.warp_count || !zs->entities.warps[index].supported) return 0;
    const struct px_zone_warp *t = &zs->entities.warps[index];
    if (t->tile_x < 0 || t->tile_y < 0 || t->tile_x >= asset->grid.width || t->tile_y >= asset->grid.height) return 0;
    struct walker *w = &asset->walker;
    w->active = 1; w->x = t->tile_x; w->z = t->tile_y; w->facing = zone_facing(t->face); w->warp_hit = -1;
    const int dx = w->facing == 1 ? 1 : w->facing == 3 ? -1 : 0, dz = w->facing == 0 ? 1 : w->facing == 2 ? -1 : 0;
    if (walkable(w, w->x + dx, w->z + dz) && px_zone_warp_at(&zs->entities, w->x + dx, w->z + dz) < 0) {
        w->x += dx; w->z += dz; ++w->moves; w->last_moved = 1;
    } else w->last_moved = 0;
    return 1;
}

/* Follow the warp the latest step asked for. On success the scene is
 * replaced and 1 returned; otherwise the scene stays, the message says why
 * and the step falls back to a plain move. */
static int zone_warp(const struct options *o, struct model_asset *asset)
{
    struct zone_state *from = asset->zone;
    const int32_t index = asset->walker.warp_hit;
    if (!from || index < 0) return 0;
    const struct px_zone_warp warp = from->entities.warps[index];
    struct options so = *o;
    so.silent = 1; so.quiet = 1;
    struct model_asset next;
    memset(&next, 0, sizeof next);
    int ok = load_zone(&so, warp.target_zone, &next) && zone_arrive(&next, warp.target_warp);
    printf("warp from_zone=%" PRIu32 " warp=%" PRId32 " target_zone=%u target_warp=%u result=%s",
           from->zone, index, warp.target_zone, warp.target_warp, ok ? "loaded" : "not-loaded");
    if (!ok) {
        putchar('\n');
        asset_free(&next);
        snprintf(from->message, sizeof from->message, "warp to zone %u not supported here", warp.target_zone);
        walk_move(&asset->walker);
        return 0;
    }
    printf(" arrival=cell:%d,%d stepped=%d\n", next.walker.x, next.walker.z, next.walker.last_moved);
    next.walker.moves += asset->walker.moves; next.walker.blocked += asset->walker.blocked;
    next.walker.warps = asset->walker.warps;
    snprintf(next.zone->message, sizeof next.zone->message, "entered zone %u", warp.target_zone);
    asset_free(asset);
    *asset = next;
    asset->walker.grid = &asset->grid; /* re-point into the moved struct */
    asset->walker.zone = asset->zone;
    return 1;
}

/* After a step: when the sub-cell under the player belongs to another zone
 * of the same matrix (a town's edge onto its route), switch to that zone,
 * keeping the player's world tile and facing. Returns 1 when switched. */
static int zone_cross(const struct options *o, struct model_asset *asset)
{
    struct zone_state *from = asset->zone;
    const struct walker *w = &asset->walker;
    if (!from || !from->subcell_zone || !w->active || w->x < 0 || w->z < 0) return 0;
    const int32_t to = from->subcell_zone[(size_t)(w->z / 10) * from->cols * 4 + (size_t)(w->x / 10)];
    if (to < 0 || to == (int32_t)from->zone || to == from->refused) return 0;
    const int32_t wx = w->x + 40 * (int32_t)from->cell_x0, wz = w->z + 40 * (int32_t)from->cell_z0;
    struct options so = *o;
    so.silent = 1; so.quiet = 1;
    struct model_asset next;
    memset(&next, 0, sizeof next);
    int ok = load_zone(&so, (uint32_t)to, &next) && next.zone->cell_region && next.zone->matrix == from->matrix;
    int32_t nx = 0, nz = 0;
    if (ok) {
        nx = wx - 40 * (int32_t)next.zone->cell_x0; nz = wz - 40 * (int32_t)next.zone->cell_z0;
        ok = nx >= 0 && nz >= 0 && nx < next.grid.width && nz < next.grid.height;
    }
    printf("zone_cross from_zone=%" PRIu32 " to_zone=%" PRId32 " world_tile=%" PRId32 ",%" PRId32 " result=%s",
           from->zone, to, wx, wz, ok ? "loaded" : "not-loaded");
    if (!ok) {
        putchar('\n');
        asset_free(&next);
        from->refused = to;
        snprintf(from->message, sizeof from->message, "zone %" PRId32 " not supported here", to);
        return 0;
    }
    printf(" cell=%" PRId32 ",%" PRId32 "\n", nx, nz);
    struct walker *nw = &next.walker;
    nw->active = 1; nw->x = nx; nw->z = nz; nw->facing = w->facing; nw->warp_hit = -1;
    nw->last_moved = w->last_moved; nw->moves = w->moves; nw->blocked = w->blocked; nw->warps = w->warps;
    snprintf(next.zone->message, sizeof next.zone->message, "entered zone %" PRId32, to);
    asset_free(asset);
    *asset = next;
    asset->walker.grid = &asset->grid;
    asset->walker.zone = asset->zone;
    return 1;
}

static void zone_steps(const struct options *o, struct model_asset *asset, const char *steps)
{
    for (const char *p = steps; p && *p; ++p) {
        walk_step(&asset->walker, *p);
        if (asset->walker.warp_hit >= 0) zone_warp(o, asset);
        else if (asset->walker.last_moved) zone_cross(o, asset);
    }
}

/* Camera convention: depth is -(sin(pitch) y + cos(pitch) z'), so a negative
 * pitch places the eye below the model (the default inspection view) and a
 * positive one above it. Walk mode starts above, looking down at 0.9 rad and
 * zoomed 2.5x, and follows the marker; all are inspection choices. */
static void reset(struct camera *c)
{
    c->yaw = c->walk ? 0 : 0.55; c->pitch = c->walk ? 0.9 : -0.35;
    c->zoom = c->walk ? 2.5 : 1; c->running = 1;
}
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
    case SDLK_w: if (c->zone) c->queued = 'U'; else if (c->walk) walk_step(c->walk, 'U'); break;
    case SDLK_s: if (c->zone) c->queued = 'D'; else if (c->walk) walk_step(c->walk, 'D'); break;
    case SDLK_a: if (c->zone) c->queued = 'L'; else if (c->walk) walk_step(c->walk, 'L'); break;
    case SDLK_d: if (c->zone) c->queued = 'R'; else if (c->walk) walk_step(c->walk, 'R'); break;
    case SDLK_SPACE: case SDLK_RETURN: if (c->zone) c->interact = 1; break;
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
    const struct camera initial = *c;
    if (!inject(c, SDLK_RIGHT) || c->yaw <= initial.yaw || !inject(c, SDLK_UP) || c->pitch >= initial.pitch ||
        !inject(c, SDLK_PLUS) || c->zoom <= initial.zoom || !inject(c, SDLK_HOME) ||
        c->yaw != initial.yaw || c->pitch != initial.pitch || c->zoom != initial.zoom ||
        !inject(c, SDLK_ESCAPE) || c->running) return 0;
    struct walker *w = c->walk;
    reset(c);
    if (c->zone) {
        /* Zone mode queues steps for the controller instead of moving. */
        if (!inject(c, SDLK_d) || c->queued != 'R' || !inject(c, SDLK_w) || c->queued != 'U' ||
            !inject(c, SDLK_SPACE) || !c->interact) return 0;
        c->queued = 0; c->interact = 0;
        fprintf(stderr, "[model-viewer] Input self-test passed: orbit, zoom, reset, Escape, WASD queue, talk.\n");
        return 1;
    }
    if (w && w->active) {
        /* A step and its reverse return to the start, or both are blocked. */
        const struct walker before = *w;
        if (!inject(c, SDLK_d) || !inject(c, SDLK_a) || w->x != before.x || w->z != before.z ||
            w->moves + w->blocked != before.moves + before.blocked + 2) return 0;
        *w = before;
        fprintf(stderr, "[model-viewer] Input self-test passed: orbit, zoom, reset, Escape, WASD.\n");
        return 1;
    }
    fprintf(stderr, "[model-viewer] Input self-test passed: orbit, zoom, reset, Escape.\n");
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
        PX_RASTER_REPLACE, PX_RASTER_SOURCE_ALPHA, PX_RASTER_ADD_SOURCE_ALPHA, PX_RASTER_FACTORS
    };
    if (m->wrap_u > 3 || m->wrap_v > 3 || m->alpha_compare > 7 || m->depth_compare > 7 ||
        (unsigned)m->blend > 3 || m->color_write_mask > 15) return 0;
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
    memcpy(state->blend_factors, m->blend_factors, sizeof state->blend_factors);
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
        /* Executed lighting: secondary RGB is exactly zero (see
         * prepare_bch_lighting) and its alpha is 255. */
        in.fragment_secondary[3] = 255;
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
            double su = u, sv = v;
            if ((m->transformed_units >> unit) & 1) {
                /* The matrix acts on stored UV (V up); v is the flipped image V. */
                const float *mt = m->uv_matrix[unit];
                const double stored = 1.0 - v;
                su = mt[0] * u + mt[1] * stored + mt[2];
                sv = 1.0 - (mt[3] * u + mt[4] * stored + mt[5]);
            }
            px_raster_sample(&s->images[unit], &s->samplers[unit], su, sv, texel);
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
    memcpy(flat.blend_factors, m->blend_factors, sizeof flat.blend_factors);
    return bch_state(&flat, &ignored, state);
}

/* Shared inspection camera: orthographic, fitted to the room's bounds. */
struct view { double center[3], radius, scale, cy, sy, cp, sp; int w, h; };
/* Floor height at a scene point: the first stored collision triangle of the
 * region whose cell holds it (multi-region zones), else the scene's own. */
static int scene_height(const struct model_asset *asset, double x, double z, double *height)
{
    const struct zone_state *zs = asset->zone;
    if (!zs || !zs->cell_region) return px_field_collision_first_height(&asset->collision, x, z, height) == PX_FIELD_OK;
    const double fx = floor((x + 360) / 720), fz = floor((z + 360) / 720);
    if (!(fx >= 0 && fz >= 0 && fx < zs->cols && fz < zs->rows)) return 0;
    const int32_t k = zs->cell_region[(size_t)fz * zs->cols + (size_t)fx];
    if (k < 0) return 0;
    if (k == 0) return px_field_collision_first_height(&asset->collision, x - zs->main_offset[0],
                                                       z - zs->main_offset[1], height) == PX_FIELD_OK;
    return px_field_collision_first_height(&zs->regions[k - 1].collision, x - zs->region_offset[k - 1][0],
                                           z - zs->region_offset[k - 1][1], height) == PX_FIELD_OK;
}
/* Placement of a model in the room's frame: rotation about +Y by yaw (radians),
 * then translation. The room itself uses the identity placement. */
struct placement { double x, y, z, yaw; int scaled; double scale[3]; /* scale applies when scaled */ };

static void project_bch(const struct model_asset *asset, const struct view *v, const struct placement *at,
                        struct px_raster_vertex *points, double (*normals)[4])
{
    const struct px_cgfx_model *mesh = &asset->wire;
    const struct px_bch_model *bch = &asset->bch;
    const double c = at ? cos(at->yaw) : 1, s = at ? sin(at->yaw) : 0;
    const double sx = at && at->scaled ? at->scale[0] : 1, sy = at && at->scaled ? at->scale[1] : 1;
    const double sz = at && at->scaled ? at->scale[2] : 1;
    for (uint32_t i = 0; i < mesh->vertex_count; ++i) {
        const double px = sx * mesh->positions[3 * (size_t)i], py = sy * mesh->positions[3 * (size_t)i + 1];
        const double pz = sz * mesh->positions[3 * (size_t)i + 2];
        const double wx = c * px + s * pz + (at ? at->x : 0), wy = py + (at ? at->y : 0);
        const double wz = -s * px + c * pz + (at ? at->z : 0);
        double x = (wx - v->center[0]) / v->radius;
        double y = (wy - v->center[1]) / v->radius;
        double z = (wz - v->center[2]) / v->radius;
        double rx = v->cy * x + v->sy * z, rz = -v->sy * x + v->cy * z;
        points[i].x = v->w * 0.5 + rx * v->scale;
        points[i].y = v->h * 0.5 - (v->cp * y - v->sp * rz) * v->scale;
        points[i].z = -(v->sp * y + v->cp * rz);
        /* Stored BCH UV0 has V up; decoded images are top-down. */
        points[i].u = bch->uvs[2 * (size_t)i];
        points[i].v = 1.0 - bch->uvs[2 * (size_t)i + 1];
        for (unsigned j = 0; j < 4; ++j)
            points[i].rgba[j] = asset->bch_combiner ? bch->colors[4 * (size_t)i + j] : 1;
        if (normals) {
            /* Inverse-transpose of the scale; lighting renormalizes. */
            const double nx = bch->normals[3 * (size_t)i] / sx, ny = bch->normals[3 * (size_t)i + 1] / sy;
            const double nz = bch->normals[3 * (size_t)i + 2] / sz;
            normals[i][0] = c * nx + s * nz; normals[i][1] = ny; normals[i][2] = -s * nx + c * nz; normals[i][3] = 0;
        }
    }
}

static int draw_bch_meshes(const struct model_asset *asset, const struct px_raster_vertex *points,
                           double (*normals)[4], struct px_raster_target *target,
                           struct combiner_counts *counts)
{
    const struct px_cgfx_model *mesh = &asset->wire;
    const struct px_bch_model *bch = &asset->bch;
    enum px_raster_result result;
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
                if (result != PX_RASTER_OK) { SDL_SetError("Rasterizer: %s", px_raster_result_string(result)); return 0; }
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
            if (result != PX_RASTER_OK) { SDL_SetError("Rasterizer: %s", px_raster_result_string(result)); return 0; }
        }
    }
    return 1;
}

enum { PX_MAX_BANDS = 8 };
/* Per-scene buffers: the scene's vertices, and one shared buffer for the
 * player and NPCs (each is projected and drawn in turn); one set per band. */
struct frame_buffers {
    SDL_FPoint *points;
    struct px_raster_vertex *vertices, *character_points;
    double (*normals)[4], (*character_normals)[4];
    struct combiner_counts *counts;
    size_t vertex_stride, character_stride;
    unsigned bands;
};
/* One pass over the scene into target (or its band): the scene asset, the
 * zone's other regions and props, then the player and NPCs (or the marker). */
static int draw_scene(const struct model_asset *asset, const struct view *v, struct px_raster_vertex *points,
                      double (*normals)[4], struct px_raster_vertex *player_points, double (*player_normals)[4],
                      struct px_raster_target *target, struct combiner_counts *counts)
{
    const struct placement main_at = {asset->zone ? asset->zone->main_offset[0] : 0, 0,
                                      asset->zone ? asset->zone->main_offset[1] : 0, 0, 0, {0}};
    project_bch(asset, v, main_at.x || main_at.z ? &main_at : NULL, points, normals);
    if (!draw_bch_meshes(asset, points, normals, target, counts)) return 0;
    for (uint32_t i = 0; asset->zone && i < asset->zone->region_count; ++i) {
        /* The zone's other regions, at their cell offsets (same buffers). */
        const struct placement at = {asset->zone->region_offset[i][0], 0, asset->zone->region_offset[i][1], 0, 0, {0}};
        project_bch(&asset->zone->regions[i], v, &at, points, normals);
        if (!draw_bch_meshes(&asset->zone->regions[i], points, normals, target, NULL)) return 0;
    }
    for (uint32_t i = 0; asset->zone && i < asset->zone->prop_count; ++i) {
        const struct zone_prop *pr = &asset->zone->props[i];
        if (pr->model == UINT32_MAX) continue;
        struct placement at = {pr->x, pr->y, pr->z, pr->yaw, 1, {pr->scale[0], pr->scale[1], pr->scale[2]}};
        project_bch(&asset->zone->prop_models[pr->model], v, &at, points, normals);
        if (!draw_bch_meshes(&asset->zone->prop_models[pr->model], points, normals, target, NULL)) return 0;
    }
    const double center[3] = {v->center[0], v->center[1], v->center[2]}, radius = v->radius, scale = v->scale;
    const double cy = v->cy, sy = v->sy, cp = v->cp, sp = v->sp;
    const int w = v->w, h = v->h;
    enum px_raster_result result;
    if (asset->walk && asset->walker.active && asset->player) {
        /* Player: rest pose standing on the first stored collision triangle
         * under the marker's cell center, facing its last step direction. */
        const int turned = asset->player->moonwalking ? 2 : 0;
        const double size = asset->player->silly == 1 ? 3 : 1;
        struct placement at = {cell_center(asset->walker.x) + asset->player->glide[0], 0,
                               cell_center(asset->walker.z) + asset->player->glide[1],
                               ((asset->walker.facing + turned) & 3) * 1.5707963267948966, size != 1, {size, size, size}};
        if (!scene_height(asset, at.x, at.z, &at.y) &&
            !scene_height(asset, cell_center(asset->walker.x),
                                            cell_center(asset->walker.z), &at.y)) at.y = 0;
        project_bch(asset->player, v, &at, player_points, player_normals);
        if (!draw_bch_meshes(asset->player, player_points, player_normals, target, NULL)) return 0;
        /* --zone NPCs on their tiles, same buffers (projected and drawn in turn). */
        for (uint32_t i = 0; asset->zone && i < asset->zone->npc_count; ++i) {
            const struct zone_npc *n = &asset->zone->npcs[i];
            if (!n->model) continue;
            struct placement np = {cell_center(n->x), 0, cell_center(n->z), n->facing * 1.5707963267948966, 0, {0}};
            if (!scene_height(asset, np.x, np.z, &np.y)) np.y = 0;
            project_bch(n->model, v, &np, player_points, player_normals);
            if (!draw_bch_meshes(n->model, player_points, player_normals, target, NULL)) return 0;
        }
    } else if (asset->walk && asset->walker.active) {
        /* Marker: a 14 x 28 x 14 box standing on the first stored collision
         * triangle under the cell center (height 0 if none). Depth tested. */
        const double mx = cell_center(asset->walker.x), mz = cell_center(asset->walker.z);
        double my = 0;
        if (!scene_height(asset, mx, mz, &my)) my = 0;
        struct px_raster_vertex corner[8];
        for (unsigned k = 0; k < 8; ++k) {
            const double px = mx + ((k & 1) ? 7 : -7), py = my + ((k & 2) ? 28 : 0), pz = mz + ((k & 4) ? 7 : -7);
            const double x = (px - center[0]) / radius, y = (py - center[1]) / radius, z = (pz - center[2]) / radius;
            const double rx = cy * x + sy * z, rz = -sy * x + cy * z;
            corner[k] = (struct px_raster_vertex){w * 0.5 + rx * scale, h * 0.5 - (cp * y - sp * rz) * scale,
                                                  -(sp * y + cp * rz), 0, 0, {1, 1, 1, 1}};
        }
        static const unsigned char faces[6][4] = {
            {2, 3, 7, 6}, {0, 1, 5, 4}, {0, 2, 6, 4}, {1, 3, 7, 5}, {0, 1, 3, 2}, {4, 5, 7, 6}};
        static const double shade[6] = {1.0, 0.45, 0.7, 0.7, 0.85, 0.85};
        const struct px_raster_state marker_state = px_raster_default_state();
        for (unsigned f = 0; f < 6; ++f) {
            struct px_raster_vertex quad[4];
            for (unsigned k = 0; k < 4; ++k) {
                quad[k] = corner[faces[f][k]];
                quad[k].rgba[0] = 0.96 * shade[f]; quad[k].rgba[1] = 0.33 * shade[f];
                quad[k].rgba[2] = 0.25 * shade[f]; quad[k].rgba[3] = 1;
            }
            const struct px_raster_vertex t0[3] = {quad[0], quad[1], quad[2]}, t1[3] = {quad[0], quad[2], quad[3]};
            result = px_raster_draw(target, t0, NULL, NULL, &marker_state);
            if (result == PX_RASTER_OK) result = px_raster_draw(target, t1, NULL, NULL, &marker_state);
            if (result != PX_RASTER_OK) { SDL_SetError("Rasterizer: %s", px_raster_result_string(result)); return 0; }
        }
    }
    return 1;
}

struct band_job {
    const struct model_asset *asset;
    const struct view *view;
    struct px_raster_vertex *points, *player_points;
    double (*normals)[4], (*player_normals)[4];
    struct px_raster_target target;
    int ok;
};
static int band_thread(void *data)
{
    struct band_job *j = data;
    j->ok = draw_scene(j->asset, j->view, j->points, j->normals, j->player_points, j->player_normals, &j->target, NULL);
    return 0;
}

static int draw_bch_textured(SDL_Renderer *renderer, const struct model_asset *asset,
                             const struct camera *camera, const struct frame_buffers *b,
                             struct px_raster_target *target, SDL_Texture **frame,
                             const char *path, struct combiner_counts *counts)
{
    const struct px_cgfx_model *mesh = &asset->wire;
    int out_w, out_h, w, h;
    if (SDL_GetRendererOutputSize(renderer, &out_w, &out_h) != 0 || out_w <= 0 || out_h <= 0) return 0;
    /* --zone renders at the 3DS top screen's 400 x 240 and scales to the window. */
    w = camera->internal_w ? camera->internal_w : out_w; h = camera->internal_h ? camera->internal_h : out_h;
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
    struct view v;
    v.radius = 0;
    for (unsigned j = 0; j < 3; ++j) {
        v.center[j] = ((double)mesh->bounds_min[j] + mesh->bounds_max[j]) / 2;
        double half = ((double)mesh->bounds_max[j] - mesh->bounds_min[j]) / 2;
        v.radius += half * half;
    }
    v.radius = sqrt(v.radius); if (v.radius < 1e-20) v.radius = 1;
    /* Multi-region zones: the camera keeps the house interior's scale (zone 4
     * room bounds radius, about 185) rather than the region's; a choice. */
    if (asset->zone && asset->zone->cell_region) v.radius = 185;
    if (asset->walk && asset->walker.active) {
        /* Follow the marker: center on its cell, 18 units above the floor. */
        double floor_y = 0;
        if (!scene_height(asset, cell_center(asset->walker.x),
                                            cell_center(asset->walker.z), &floor_y)) floor_y = 0;
        v.center[0] = cell_center(asset->walker.x);
        v.center[1] = floor_y + 18;
        v.center[2] = cell_center(asset->walker.z);
        if (asset->zone && asset->player) { /* --zone: follow the gliding player */
            v.center[0] += asset->player->glide[0]; v.center[2] += asset->player->glide[1];
        }
    }
    v.w = w; v.h = h;
    v.scale = (w < h ? w : h) * 0.42 * camera->zoom;
    v.cy = cos(camera->yaw); v.sy = sin(camera->yaw);
    v.cp = cos(camera->pitch); v.sp = sin(camera->pitch);
    /* The frame is split into row bands drawn concurrently (each band sees
     * every triangle in the same order, so pixels match a single pass); the
     * counted first frame is drawn in one pass. */
    const unsigned bands = counts ? 1 : b->bands;
    struct band_job jobs[PX_MAX_BANDS];
    SDL_Thread *threads[PX_MAX_BANDS] = {0};
    for (unsigned k = 0; k < bands; ++k) {
        struct band_job *j = &jobs[k];
        j->asset = asset; j->view = &v;
        j->points = b->vertices + k * b->vertex_stride; j->normals = b->normals ? b->normals + k * b->vertex_stride : NULL;
        j->player_points = b->character_points ? b->character_points + k * b->character_stride : NULL;
        j->player_normals = b->character_normals ? b->character_normals + k * b->character_stride : NULL;
        j->target = *target;
        if (bands > 1) {
            j->target.band_y0 = (uint32_t)((uint64_t)h * k / bands);
            j->target.band_y1 = (uint32_t)((uint64_t)h * (k + 1) / bands);
        }
        j->ok = 0;
    }
    for (unsigned k = 1; k < bands; ++k)
        if (!(threads[k] = SDL_CreateThread(band_thread, "band", &jobs[k]))) band_thread(&jobs[k]);
    jobs[0].ok = draw_scene(asset, &v, jobs[0].points, jobs[0].normals, jobs[0].player_points, jobs[0].player_normals,
                            &jobs[0].target, counts);
    int all = jobs[0].ok;
    for (unsigned k = 1; k < bands; ++k) {
        if (threads[k]) SDL_WaitThread(threads[k], NULL);
        all &= jobs[k].ok;
    }
    if (!all) return 0;
    if (asset->zone && asset->zone->page_count)
        px_text_window(target->rgba, target->width, target->height, asset->zone->pages[asset->zone->page],
                       asset->zone->page + 1 < asset->zone->page_count);
    if (SDL_UpdateTexture(*frame, NULL, target->rgba, w * 4) ||
        SDL_RenderClear(renderer) || SDL_RenderCopy(renderer, *frame, NULL, NULL)) return 0;
    if (path && !capture(renderer, path, out_w, out_h)) return 0;
    SDL_RenderPresent(renderer); return 1;
raster_error:
    SDL_SetError("Rasterizer: %s", px_raster_result_string(result)); return 0;
}

static void buffers_free(struct frame_buffers *b)
{
    free(b->points); free(b->vertices); free(b->character_points);
    free(b->normals); free(b->character_normals); free(b->counts);
    memset(b, 0, sizeof *b);
}
/* Per-scene buffers: the scene's vertices, and one shared buffer for the
 * player and NPCs (each is projected and drawn in turn). */
static int buffers_alloc(const struct options *o, const struct model_asset *asset, struct frame_buffers *b)
{
    buffers_free(b);
    const struct px_cgfx_model *mesh = o->textured && !asset->bch_textured ? &asset->scene.mesh : &asset->wire;
    size_t n = mesh->vertex_count ? mesh->vertex_count : 1;
    for (uint32_t i = 0; asset->zone && i < asset->zone->region_count; ++i)
        if (asset->zone->regions[i].wire.vertex_count > n) n = asset->zone->regions[i].wire.vertex_count;
    for (uint32_t i = 0; asset->zone && i < asset->zone->prop_model_count; ++i)
        if (asset->zone->prop_models[i].wire.vertex_count > n) n = asset->zone->prop_models[i].wire.vertex_count;
    /* BCH scenes draw in row bands, one per CPU (at most PX_MAX_BANDS) or
     * --threads. */
    int cpus = o->threads ? (int)o->threads : SDL_GetCPUCount();
    b->bands = asset->bch_textured ? (unsigned)(cpus < 1 ? 1 : cpus > PX_MAX_BANDS ? PX_MAX_BANDS : cpus) : 1;
    b->vertex_stride = n;
    if (o->textured) b->vertices = malloc(n * b->bands * sizeof *b->vertices);
    else b->points = malloc(n * sizeof *b->points);
    if (o->textured ? !b->vertices : !b->points) return 0;
    if (asset->bch_combiner && !(b->normals = calloc(n * b->bands, sizeof *b->normals))) return 0;
    uint32_t characters = asset->player ? asset->player->wire.vertex_count : 0;
    for (uint32_t i = 0; asset->zone && i < asset->zone->npc_count; ++i)
        if (asset->zone->npcs[i].model && asset->zone->npcs[i].model->wire.vertex_count > characters)
            characters = asset->zone->npcs[i].model->wire.vertex_count;
    b->character_stride = characters ? characters : 1;
    if (asset->player) {
        b->character_points = calloc(b->character_stride * b->bands, sizeof *b->character_points);
        b->character_normals = calloc(b->character_stride * b->bands, sizeof *b->character_normals);
        if (!b->character_points || !b->character_normals) return 0;
    }
    if (asset->bch_combiner && !(b->counts = calloc(asset->bch.material_count ? asset->bch.material_count : 1,
                                                    sizeof *b->counts))) return 0;
    return 1;
}

/* --zone controller (inspection choices, not the game's player controller):
 * a held or queued WASD direction starts a step when none is playing; a step
 * lasts 11 walk-cycle frames (slot 3) or, with Shift, 5 run-cycle frames
 * (slot 4), about the stride of the planted foot in each motion, and the
 * cycle phase carries over consecutive steps. Motions run at 30 frames per
 * second of a clock that is real time, or 60 rendered frames per second when
 * --frames bounds the run (deterministic captures). Idle plays slot 1. Space
 * or Return faces the NPC ahead toward the player and reports its script;
 * the zone script VM is not executed. */
/* Run the NPC's zone-script event and open its lines as message pages: a
 * page ends at the text's scroll/clear codes or after two lines. */
static void zone_dialogue(struct zone_state *zs, const struct zone_npc *n)
{
    for (uint32_t i = 0; i < zs->page_count; ++i) free(zs->pages[i]);
    zs->page_count = zs->page = 0;
    if (!zs->has_script || !zs->has_text) return;
    struct px_zone_talk talk;
    px_zone_talk(&zs->script, n->script, 1, &talk);
    printf("dialogue zone=%" PRIu32 " script=%u result=%s lines=%" PRIu32 "\n", zs->zone, n->script,
           px_gf_script_result_string(talk.run.result), talk.line_count);
    static char buffer[1 << 14];
    for (uint32_t k = 0; k < talk.line_count; ++k) {
        if (talk.lines[k] < 0 || px_gf_text_render(&zs->text, (uint32_t)talk.lines[k], "Player", buffer,
                                                   sizeof buffer) < 0) continue;
        char *at = buffer;
        while (*at && zs->page_count < ZONE_MAX_PAGES) {
            /* Take up to two lines, stopping at a page break. */
            char *end = at;
            int lines = 0;
            while (*end && *end != '\f') {
                if (*end == '\n' && ++lines == 2) break;
                ++end;
            }
            const size_t length = (size_t)(end - at);
            char *page = malloc(length + 1);
            if (!page) return;
            memcpy(page, at, length); page[length] = 0;
            if (length) {
                zs->pages[zs->page_count++] = page;
                printf("message zone=%" PRIu32 " line=%" PRId32 " page=%" PRIu32 " text=\"", zs->zone, talk.lines[k],
                       zs->page_count);
                for (const char *q = page; *q; ++q) {
                    if (*q == '\n') fputs("\\n", stdout);
                    else if (*q == '"' || *q == '\\') printf("\\%c", *q);
                    else putchar(*q);
                }
                puts("\"");
            } else free(page);
            at = *end ? end + 1 : end;
        }
    }
}

struct zone_motion {
    int stepping, run, dx, dz;
    uint64_t start_ms;
    uint32_t length, phase;
};
enum { WALK_STEP_FRAMES = 11, RUN_STEP_FRAMES = 5 };
static void zone_title(SDL_Window *win, const struct model_asset *asset)
{
    char title[320];
    snprintf(title, sizeof title, "Pokemon X — zone %" PRIu32 " | WASD walk, Shift run, Space talk, arrows orbit, "
             "Esc exit%s%s", asset->zone->zone, asset->zone->message[0] ? " | " : "", asset->zone->message);
    if (strcmp(title, SDL_GetWindowTitle(win))) SDL_SetWindowTitle(win, title);
}
static int zone_frame(const struct options *o, struct model_asset *asset, struct camera *c,
                      struct zone_motion *zm, uint64_t now_ms, int *swapped)
{
    struct walker *w = &asset->walker;
    const uint64_t motion_now = now_ms * 30 / 1000;
    if (zm->stepping && motion_now - zm->start_ms * 30 / 1000 >= zm->length) {
        zm->stepping = 0; zm->phase += zm->length;
    }
    if (!zm->stepping && !asset->zone->page_count) {
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        char direction = keys[SDL_SCANCODE_W] ? 'U' : keys[SDL_SCANCODE_S] ? 'D' : keys[SDL_SCANCODE_A] ? 'L' :
                         keys[SDL_SCANCODE_D] ? 'R' : c->queued;
        c->queued = 0;
        int run = (SDL_GetModState() & KMOD_SHIFT) != 0;
        if (!direction && c->play && *c->play) {
            /* --play: one scripted input per free controller frame. */
            const char next = *c->play++;
            if (next == '.') c->interact = 1;
            else {
                run = next >= 'a';
                direction = (char)(next >= 'a' ? next - 'a' + 'A' : next);
            }
        }
        int moved = 0;
        if (direction && w->active) {
            walk_step(w, direction);
            if (w->warp_hit >= 0) {
                if (zone_warp(o, asset)) { *swapped = 1; zm->phase = 0; }
                w = &asset->walker;
            } else if (w->last_moved && zone_cross(o, asset)) {
                *swapped = 1; /* the step and its cycle carry on in the next zone */
                w = &asset->walker;
            }
            moved = w->last_moved;
            if (w->zone && asset->zone->message[0] && !*swapped && w->warp_hit < 0 && moved)
                asset->zone->message[0] = 0;
        }
        if (moved) {
            zm->stepping = 1; zm->run = run; zm->start_ms = now_ms;
            zm->length = run ? RUN_STEP_FRAMES : WALK_STEP_FRAMES;
            zm->dx = w->facing == 1 ? 1 : w->facing == 3 ? -1 : 0;
            zm->dz = w->facing == 0 ? 1 : w->facing == 2 ? -1 : 0;
        } else zm->phase = 0;
    }
    if (asset->zone->page_count) {
        /* A message window is open: Space/Return turns the page; no walking.
         * --play: a '.' turns it too; other scripted input waits. */
        if (!c->interact && c->play && *c->play == '.') { ++c->play; c->interact = 1; }
        if (c->interact) {
            c->interact = 0;
            if (++asset->zone->page >= asset->zone->page_count) {
                for (uint32_t i = 0; i < asset->zone->page_count; ++i) free(asset->zone->pages[i]);
                asset->zone->page_count = asset->zone->page = 0;
            }
        }
        c->queued = 0;
    } else if (c->interact) {
        c->interact = 0;
        const int dx = w->facing == 1 ? 1 : w->facing == 3 ? -1 : 0, dz = w->facing == 0 ? 1 : w->facing == 2 ? -1 : 0;
        const int k = npc_at(asset->zone, w->x + dx, w->z + dz);
        if (k >= 0) {
            struct zone_npc *n = &asset->zone->npcs[k];
            n->facing = (w->facing + 2) & 3;
            snprintf(asset->zone->message, sizeof asset->zone->message,
                     "NPC %u: script %u (zone script VM not implemented)", n->id, n->script);
            printf("talk zone=%" PRIu32 " npc=%d id=%u script=%u\n", asset->zone->zone, k, n->id, n->script);
            zone_dialogue(asset->zone, n);
        } else snprintf(asset->zone->message, sizeof asset->zone->message, "nothing to talk to");
    }
    struct model_asset *p = asset->player;
    p->glide[0] = p->glide[1] = 0;
    const struct px_gf_motion *motion = p->motion;
    uint64_t at = motion_now;
    if (zm->stepping) {
        const uint64_t t = motion_now - zm->start_ms * 30 / 1000;
        motion = zm->run ? p->run_motion : p->walk_motion;
        at = zm->phase + t;
        const double remaining = 18.0 * (1.0 - (double)t / zm->length);
        p->glide[0] = -zm->dx * remaining; p->glide[1] = -zm->dz * remaining;
    }
    /* --silly moonwalk: face away from the step and play its cycle backwards. */
    p->silly = o->silly;
    p->moonwalking = o->silly == 2 && zm->stepping;
    uint32_t pose = motion->frames ? (uint32_t)(at % motion->frames) : 0;
    if (p->moonwalking && motion->frames) pose = motion->frames - 1 - pose;
    if (!pose_motion(p, motion, pose)) return 0;
    for (uint32_t i = 0; i < asset->zone->npc_count; ++i) {
        struct model_asset *n = asset->zone->npcs[i].model;
        if (n && n->motion_ready &&
            !pose_motion(n, n->motion, n->motion->frames ? (uint32_t)(motion_now % n->motion->frames) : 0)) return 0;
    }
    return 1;
}

static int window(const struct options *o, struct model_asset *asset)
{
    const struct px_cgfx_model *mesh = o->textured && !asset->bch_textured ?
        &asset->scene.mesh : &asset->wire;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) return 0;
    SDL_Window *win = SDL_CreateWindow(o->textured ?
        "Pokemon X unlit textured preview — arrows: orbit | +/-: zoom | R: reset | Esc: exit" :
        "Pokemon X wireframe — arrows: orbit | +/-: zoom | R: reset | Esc: exit",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, asset->zone ? 800 : 960, asset->zone ? 480 : 640,
        SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = NULL; int ok = 0;
    SDL_Texture *frame = NULL; struct px_raster_target target = {0};
    struct frame_buffers b;
    memset(&b, 0, sizeof b);
    if (!win) goto done;
    if (!o->software) renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer) goto done;
    if (!buffers_alloc(o, asset, &b)) goto done;
    struct camera c;
    memset(&c, 0, sizeof c);
    c.walk = asset->walk ? &asset->walker : NULL;
    c.zone = asset->zone != NULL;
    c.play = o->play;
    if (asset->zone) { c.internal_w = 400; c.internal_h = 240; }
    reset(&c);
    if (o->self_test && !input_test(&c)) { SDL_SetError("Input self-test failed"); goto done; }
    SDL_RendererInfo info; if (SDL_GetRendererInfo(renderer, &info)) goto done;
    fprintf(stderr, "[model-viewer] Video=%s renderer=%s; orthographic %s.\n",
            SDL_GetCurrentVideoDriver(), info.name, o->textured ?
            asset->bch_light ? "BCH six-stage combiner software preview; directional fragment lighting; layer/priority/resource draw order" :
            asset->bch_combiner ? "BCH six-stage combiner software preview; declared unlit inputs; layer/priority/resource draw order" :
            asset->bch_textured ? "unlit BCH TEV-stage-0 software preview; layer/priority/resource draw order" :
            "unlit textured software preview; translucency/priority/resource draw order" : "wireframe");
    uint32_t frames = 0, last_moves = asset->walker.moves;
    int64_t step_frame = INT64_MIN; /* rendered frame where the playing step began */
    int step_dx = 0, step_dz = 0;
    if (!asset->zone && asset->walk && asset->player && asset->player->walk_motion && asset->walker.last_moved) {
        /* The last --steps move plays its cycle as display starts, from
         * --motion-frame, so captures can show any phase of a step. */
        const struct walker *w = &asset->walker;
        step_frame = -2 * (int64_t)asset->player->motion_start;
        step_dx = w->facing == 1 ? 1 : w->facing == 3 ? -1 : 0;
        step_dz = w->facing == 0 ? 1 : w->facing == 2 ? -1 : 0;
    }
    struct zone_motion zm;
    memset(&zm, 0, sizeof zm);
    const uint64_t start_ticks = SDL_GetTicks64();
    uint64_t frame_ticks = start_ticks;
    if (asset->zone) zone_title(win, asset);
    while (c.running) {
        SDL_Event e; while (SDL_PollEvent(&e)) event(&c, &e);
        if (!c.running) break;
        const char *path = frames == o->capture_at ? o->capture : NULL;
        if (asset->zone) {
            int swapped = 0;
            const char before = asset->zone->message[0];
            const uint64_t now = o->frames ? (uint64_t)frames * 1000 / 60 : SDL_GetTicks64() - start_ticks;
            if (!zone_frame(o, asset, &c, &zm, now, &swapped)) { SDL_SetError("zone frame failed"); goto done; }
            if (swapped && !buffers_alloc(o, asset, &b)) goto done;
            (void)before;
            zone_title(win, asset);
        } else {
            /* One motion frame per two rendered frames (30 of 60 Hz), looping. */
            struct model_asset *animated = asset->motion_ready ? asset : asset->player && asset->player->motion_ready ?
                                           asset->player : NULL;
            if (animated) {
                const struct px_gf_motion *motion = animated->motion;
                uint64_t at = (animated->walk_motion ? 0 : (uint64_t)animated->motion_start) + frames / 2;
                animated->glide[0] = animated->glide[1] = 0;
                if (asset->walk && asset->walker.moves != last_moves) {
                    /* A step happened: start the walk cycle from the previous cell. */
                    last_moves = asset->walker.moves; step_frame = frames;
                    step_dx = asset->walker.facing == 1 ? 1 : asset->walker.facing == 3 ? -1 : 0;
                    step_dz = asset->walker.facing == 0 ? 1 : asset->walker.facing == 2 ? -1 : 0;
                }
                if (animated->walk_motion && step_frame != INT64_MIN &&
                    ((int64_t)frames - step_frame) / 2 < (int64_t)animated->walk_motion->frames) {
                    motion = animated->walk_motion;
                    at = (uint64_t)(((int64_t)frames - step_frame) / 2);
                    /* Glide: the player covers the 18-unit cell linearly over the
                     * cycle, by motion frame (an inspection choice). */
                    const double remaining = 18.0 * (1.0 - (double)at / motion->frames);
                    animated->glide[0] = -step_dx * remaining; animated->glide[1] = -step_dz * remaining;
                } else step_frame = INT64_MIN;
                const uint32_t length = motion->frames;
                if (!pose_motion(animated, motion, length ? (uint32_t)(at % length) : 0)) {
                    SDL_SetError("GF1 motion pose failed"); goto done;
                }
            }
        }
        if (asset->bch_textured ? !draw_bch_textured(renderer, asset, &c, &b, &target, &frame, path,
                                                     frames == 0 ? b.counts : NULL) :
            o->textured ? !draw_textured(renderer, asset, &c, b.vertices, &target, &frame, path) :
                          !draw(renderer, mesh, &c, b.points, path)) goto done;
        if (frames == 0 && b.counts)
            for (uint32_t i = 0; i < asset->bch.material_count; ++i)
            {
                printf("combiner_sensitivity material=%" PRIu32 " frame=1 fragments=%" PRIu64
                       " projected_substitute_sensitive=%" PRIu64 " secondary_sensitive=%" PRIu64,
                       i, b.counts[i].fragments, b.counts[i].projected_sensitive, b.counts[i].secondary_sensitive);
                if (asset->bch_light)
                    printf(" point_bound_sensitive=%" PRIu64 " point_bound_max_delta=%" PRIu32
                           " degenerate_normals=%" PRIu64, b.counts[i].point_sensitive,
                           b.counts[i].point_max_delta, b.counts[i].degenerate_normals);
                putchar('\n');
            }
        if (++frames == o->frames) break;
        if (asset->zone) {
            /* --zone paces to about 60 Hz: wait only for what rendering left. */
            const uint64_t spent = SDL_GetTicks64() - frame_ticks;
            if (spent < 16) SDL_Delay((Uint32)(16 - spent));
            frame_ticks = SDL_GetTicks64();
        } else SDL_Delay(16);
    }
    fprintf(stderr, "[model-viewer] Exit after %" PRIu32 " frames.\n", frames); ok = 1;
done:
    if (!ok) fprintf(stderr, "[model-viewer] Runtime: %s\n", SDL_GetError());
    buffers_free(&b);
    px_raster_free(&target); if (frame) SDL_DestroyTexture(frame);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (win) SDL_DestroyWindow(win);
    SDL_Quit();
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
    static const char *blend[] = {"replace", "source-alpha", "add-source-alpha", "factors"};
    const struct px_bch_model *bch = &asset->bch;
    uint32_t decoded = 0;
    for (uint32_t i = 0; i < asset->texture_count; ++i) decoded += asset->textures[i].rgba != NULL;
    if (asset->bch_combiner) {
        if (asset->bch_light)
            printf("mode=textured shading=tev-six-stage primary=vertex-color fragment_primary=directional-lit "
                   "fragment_secondary=zero-rgb-alpha-255 lighting=%s point_spot=bounded normals=interpolated-renormalized "
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
            for (uint32_t i = 0; i < bch->material_count; ++i) {
                const uint32_t *secondary = &asset->secondary_bound[(size_t)i * 3];
                printf("light_secondary material=%" PRIu32 " value=0,0,0,255 point_specular_bound=%" PRIu32
                       ",%" PRIu32 ",%" PRIu32 "\n", i, secondary[0], secondary[1], secondary[2]);
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
    if (o.have_zone) {
        if (!load_zone(&o, o.zone, &asset)) { asset_free(&asset); return 1; }
        if (o.have_arrive && !zone_arrive(&asset, o.arrive)) {
            fprintf(stderr, "[model-viewer] --arrive: zone %" PRIu32 " has no supported warp %" PRIu32 ".\n",
                    o.zone, o.arrive);
            asset_free(&asset); return 1;
        }
        zone_steps(&o, &asset, o.steps);
        o.textured = 1; /* the window draws the zone's textured BCH */
    } else if (!load(&o, &asset)) { asset_free(&asset); return 1; }
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
    if (asset.walk) {
        const struct walker *w = &asset.walker;
        uint32_t standard = 0;
        for (size_t i = 0; i < (size_t)asset.grid.width * asset.grid.height; ++i)
            standard += asset.grid.tiles[i] == UINT32_C(0x00000020);
        printf("walk policy=exact-cell-0x00000020 movement=inspection-not-game-rules grid=%ux%u "
               "standard_cells=%" PRIu32 " collision_vertices=%" PRIu32,
               asset.grid.width, asset.grid.height, standard, asset.collision.vertex_count);
        if (!w->active) printf(" marker=none\n");
        else {
            double y = 0;
            const int hit = scene_height(&asset, cell_center(w->x), cell_center(w->z), &y);
            printf(" marker=cell:%d,%d local=%.9g,%.9g height=%s%.9g moves=%" PRIu32 " blocked=%" PRIu32 "\n",
                   w->x, w->z, cell_center(w->x), cell_center(w->z), hit ? "" : "none:", y, w->moves, w->blocked);
        }
        if (asset.zone)
            printf("zone_walk zone=%" PRIu32 " cell=%d,%d facing=%d moves=%" PRIu32 " blocked=%" PRIu32
                   " warps=%" PRIu32 " policy=standard-cells-npcs-block-warps-on-step\n", asset.zone->zone,
                   w->x, w->z, w->facing, w->moves, w->blocked, w->warps);
    }
    int ok = o.inspect || window(&o, &asset); asset_free(&asset); return ok ? 0 : 1;
}
