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
#include "assets/pokedata.h"
#include "game/battle.h"
#include "game/battle_ui.h"
#include "game/save.h"
#include "platform/audio.h"
#include "platform/audio_sdl.h"
#include "platform/clock.h"
#include "platform/fs.h"
#include "platform/hid.h"
#include "platform/thread.h"
#include "platform/guest.h"
#include "render/light.h"
#include "render/raster.h"
#include "render/skin.h"
#include "render/text.h"
#include "render/vertex_program.h"
#include "render/frag_light.h"

#include <SDL.h>
#include <ctype.h>
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
    uint32_t motion_entry; int have_motion_entry; /* the motion pack's entry, else the model's */
    uint32_t zone, arrive, run_motion_slot, capture_at, threads;
    int have_zone, have_arrive, silent, have_run_motion, have_capture_at;
    /* --zone wild encounters: the partner Pokémon, the random seed, the
     * percent chance per grass step and the code.bin holding the nature
     * table (natures are neutral without it). */
    uint32_t partner, partner_level, seed, encounter_rate, balls;
    int have_partner, have_partner_level, have_seed, have_encounter_rate, have_balls;
    const char *code_bin;
    const char *save_path, *load_path; /* --save / --load: the viewer save file (game/save.h) */
    int vertex_shader;    /* --vertex-shader: battle models' primary color from their own vertex programs */
    int bottom_screen;    /* --bottom-screen: the 3DS's second screen, the touch screen, below */
    const char *touch_play; /* --touch-play F:X,Y;...: scripted taps on the bottom screen */
    const char *sound;      /* --sound PATH[@OFFSET]: a BCWAV in the RomFS to play (looping as stored) */
    const char *list_sounds; /* --list-sounds PATH: the BCWAVs inside a RomFS file */
    const char *play; /* --zone: scripted input during display */
    int silly; /* --silly: 0 none, 1 giant, 2 moonwalk (for fun; not game behaviour) */
    uint32_t entry, subentry, model, frames, section;
    uint32_t texture_entry, texture_subentry, texture_section;
    int have_entry, have_subentry, have_model, demo, inspect, software, self_test, textured;
    int have_section, triangle_list, combiners;
    int allow_skeleton; /* --zone regions: retry a skeleton model as skinned */
    int hide_invisible; /* --zone scenes: meshes whose visible flag is 0 are not drawn */
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
    /* Wild Pokémon (ZO section 3), when the zone has them. */
    struct px_encounter_table wild;
    int has_wild;
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
    /* Placeholder regions (a single untextured flat plane over the cell): the
     * scene asset when hide_main, else region k when region_hidden[k]. */
    int hide_main;
    uint8_t *region_hidden;
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
    int save_request; /* F5 */
    int interact;
    const char *play; /* --play cursor */
    int internal_w, internal_h; /* --zone: fixed render size, scaled to the window */
    int in_battle;     /* keys drive the battle menu instead */
    char battle_key;   /* U, D, L, R, '.' or 'x', or 0 */
    /* The pad (platform/hid.h): host keys become 3DS buttons; the latches
     * above are taken from its presses each frame. */
    struct px_hid hid;
    const struct px_hid_state *pad;
    int bottom; /* --bottom-screen: taps reach the battle menu */
};
/* The bottom screen (320 x 240) below the top one in a 400 x 480 logical
 * window, centred as on the 3DS. */
enum { PX_BOTTOM_X = 40, PX_BOTTOM_Y = 240 };
static struct { int enabled; SDL_Texture *texture; uint8_t rgba[PX_TOUCH_WIDTH * PX_TOUCH_HEIGHT * 4]; } g_bottom;
/* The game's reading of the pad: a direction from the D-pad or the circle
 * pad (up, down, left, right first), and in battle A confirms, B cancels. */
static char pad_direction(uint32_t keys)
{
    return keys & (PX_KEY_DUP | PX_KEY_CPAD_UP) ? 'U' : keys & (PX_KEY_DDOWN | PX_KEY_CPAD_DOWN) ? 'D' :
           keys & (PX_KEY_DLEFT | PX_KEY_CPAD_LEFT) ? 'L' : keys & (PX_KEY_DRIGHT | PX_KEY_CPAD_RIGHT) ? 'R' : 0;
}
static char pad_battle_key(uint32_t keys)
{
    const char d = pad_direction(keys);
    return d ? d : keys & PX_KEY_A ? '.' : keys & PX_KEY_B ? 'x' : 0;
}
/* Host keyboard to the pad (an inspection mapping): WASD the D-pad (the
 * arrows too in battle; in the field they turn the camera), Space/Return A,
 * X/Backspace and Shift B (B held runs, as in the game). */
static uint32_t host_held(int in_battle)
{
    const Uint8 *k = SDL_GetKeyboardState(NULL);
    uint32_t keys = 0;
    if (k[SDL_SCANCODE_W] || (in_battle && k[SDL_SCANCODE_UP])) keys |= PX_KEY_DUP;
    if (k[SDL_SCANCODE_S] || (in_battle && k[SDL_SCANCODE_DOWN])) keys |= PX_KEY_DDOWN;
    if (k[SDL_SCANCODE_A] || (in_battle && k[SDL_SCANCODE_LEFT])) keys |= PX_KEY_DLEFT;
    if (k[SDL_SCANCODE_D] || (in_battle && k[SDL_SCANCODE_RIGHT])) keys |= PX_KEY_DRIGHT;
    if (k[SDL_SCANCODE_SPACE] || k[SDL_SCANCODE_RETURN]) keys |= PX_KEY_A;
    if (k[SDL_SCANCODE_X] || k[SDL_SCANCODE_BACKSPACE] || (SDL_GetModState() & KMOD_SHIFT)) keys |= PX_KEY_B;
    return keys;
}
/* End of the host's input for a frame: the pad state, then the latches. */
static void pad_frame(struct camera *c, uint32_t held)
{
    px_hid_set_held(&c->hid, held);
    c->pad = px_hid_update(&c->hid);
    const uint32_t down = c->pad->down;
    if (c->in_battle) { const char k = pad_battle_key(down); if (k) c->battle_key = k; return; }
    if (!c->zone) return;
    const char d = pad_direction(down);
    if (d) c->queued = d;
    if (down & PX_KEY_A) c->interact = 1;
}
static int npc_at(const struct zone_state *zone, int x, int z)
{
    for (uint32_t i = 0; zone && i < zone->npc_count; ++i)
        if (zone->npcs[i].x == x && zone->npcs[i].z == z) return (int)i;
    return -1;
}
/* --zone: tall grass, the tile family 0x2?002004 (Route 2's grass patches:
 * 0x20002004; 0x21..0x23 variants elsewhere), is walkable and starts wild
 * encounters. An inspection reading of the words, not the game's collision. */
static int grass_tile(uint32_t word) { return (word & UINT32_C(0xF0FFFFFF)) == UINT32_C(0x20002004); }
static int walkable(const struct walker *w, int x, int z)
{
    if (x < 0 || z < 0 || x >= w->grid->width || z >= w->grid->height) return 0;
    const uint32_t word = w->grid->tiles[(size_t)z * w->grid->width + (size_t)x];
    return (word == UINT32_C(0x00000020) || (w->zone && grass_tile(word))) && npc_at(w->zone, x, z) < 0;
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
    int hide_invisible; /* skip meshes whose visible flag is 0 */
    /* --vertex-shader: each material's vertex program (NULL when it has none
     * that loads) and two interpreter states. */
    struct px_vertex_program **programs;
    struct px_pica_state *program_state;
    /* Per material: the fragment lighting unit's configuration (lights in
     * view space, rebuilt each frame) and whether it executes; per index
     * the program's normal quaternion (o1) and view vector (o2). */
    struct px_frag_light_config *fragment_light;
    uint8_t *fragment_ok;
    double (*program_varyings)[8];
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
        free(asset->zone->cell_region); free(asset->zone->subcell_zone); free(asset->zone->region_hidden);
        for (uint32_t i = 0; i < asset->zone->prop_model_count; ++i) asset_free(&asset->zone->prop_models[i]);
        free(asset->zone->prop_models); free(asset->zone->prop_model_entry); free(asset->zone->props);
        free(asset->zone);
    }
    px_bch_model_free(&asset->skin_source);
    px_gf_motion_pack_free(&asset->motion_pack);
    free(asset->motion_locals); free(asset->motion_world); free(asset->motion_rest);
    for (uint32_t i = 0; asset->programs && i < asset->skin_source.material_count; ++i) free(asset->programs[i]);
    free(asset->programs); free(asset->program_state);
    free(asset->fragment_light); free(asset->fragment_ok); free(asset->program_varyings);
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
        "                     entry (the player's with --player-archive), or of\n"
        "                     --motion-entry N in the model's archive\n"
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
        , program);
    fputs("  --threads N        BCH scenes: draw in N row bands (1..8; default: CPUs)\n"
        "  --no-props         With --zone: do not load or draw region props\n"
        "  --partner N        With --zone: the partner Pokemon's species (default 656)\n"
        "  --partner-level N  Its level, 1..100 (default 5)\n"
        "  --seed N           Random seed for encounters and battles (default 1)\n"
        "  --encounter-rate N Percent chance per grass step, 0..100 (default 10)\n"
        "  --code-bin PATH    code.bin for the nature table (else natures are neutral)\n"
        "  --balls N          Poke Balls to start with, 0..999 (default 10)\n"
        "  --vertex-shader    With --zone: light the battle models per vertex with their\n"
        "                     own vertex programs (slower)\n"
        "  --bottom-screen    With --zone: show the 3DS bottom (touch) screen below;\n"
        "                     click it to touch (battle commands are there)\n"
        "  --touch-play F:X,Y;...  With --bottom-screen and --frames: tap (X, Y) of\n"
        "                     the bottom screen during rendered frame F\n"
        "  --sound PATH[@OFFSET]  Play the BCWAV at OFFSET of RomFS file PATH\n"
        "  --list-sounds PATH With --romfs alone: list the BCWAVs inside RomFS file PATH\n"
        "  --save PATH        With --zone and --code-bin: F5 (or s in --play) saves the\n"
        "                     zone, position, party and balls to PATH\n"
        "  --load PATH        With --code-bin, instead of --zone: start from that save\n"
        "                     (F5 then saves back to it unless --save is given)\n"
        "  --silly MODE       With --zone, for fun: giant (a three-times player) or\n"
        "                     moonwalk (walks facing backwards, cycle reversed)\n"
        "  --self-test-input  Exercise orbit/zoom/reset/exit; default 3 frames\n"
        "Controls: arrows orbit, +/- zoom, R/Home reset, Escape exit.\n"
        "Orthographic wireframe by default; --textured uses a bounded software preview.\n"
        "Inspection camera; lighting only with --environment-archive; animation only with --motion.\n", stream);
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

/* --touch-play: "F:X,Y" entries separated by ';', frames ascending,
 * coordinates on the 320 x 240 bottom screen. */
struct touch_tap { uint32_t frame; int x, y; };
static int touch_script_next(const char **p, struct touch_tap *t)
{
    unsigned long f; int x, y, n = 0;
    if (!**p || sscanf(*p, "%lu:%d,%d%n", &f, &x, &y, &n) != 3 || x < 0 || y < 0 || x >= PX_TOUCH_WIDTH ||
        y >= PX_TOUCH_HEIGHT || f > UINT32_MAX) return 0;
    t->frame = (uint32_t)f; t->x = x; t->y = y;
    *p += n;
    if (**p == ';') ++*p;
    return 1;
}
static int touch_script_valid(const char *s)
{
    struct touch_tap t; int64_t last = -1;
    const char *p = s;
    if (!*p) return 0;
    while (*p) {
        if (!touch_script_next(&p, &t) || (int64_t)t.frame <= last) return 0;
        last = t.frame;
    }
    return 1;
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
        if (!strcmp(arg, "--bottom-screen")) { o->bottom_screen = 1; continue; }
        if (!strcmp(arg, "--vertex-shader")) { o->vertex_shader = 1; continue; }
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
            strcmp(arg, "--player-section") && strcmp(arg, "--motion-section") && strcmp(arg, "--motion-entry") &&
            strcmp(arg, "--motion") && strcmp(arg, "--motion-frame") && strcmp(arg, "--walk-motion") &&
            strcmp(arg, "--zone") && strcmp(arg, "--arrive") && strcmp(arg, "--play") &&
            strcmp(arg, "--capture-at") && strcmp(arg, "--threads") && strcmp(arg, "--silly") &&
            strcmp(arg, "--partner") && strcmp(arg, "--partner-level") && strcmp(arg, "--seed") &&
            strcmp(arg, "--encounter-rate") && strcmp(arg, "--code-bin") && strcmp(arg, "--balls") &&
            strcmp(arg, "--save") && strcmp(arg, "--load") && strcmp(arg, "--touch-play") &&
            strcmp(arg, "--sound") && strcmp(arg, "--list-sounds")) return 0;
        if (++i == argc || !argv[i][0]) return 0;
        const char *value = argv[i];
        if (!strcmp(arg, "--romfs")) o->romfs = value;
        else if (!strcmp(arg, "--archive")) o->archive = value;
        else if (!strcmp(arg, "--capture-frame")) o->capture = value;
        else if (!strcmp(arg, "--texture-archive")) o->texture_archive = value;
        else if (!strcmp(arg, "--environment-archive")) o->environment_archive = value;
        else if (!strcmp(arg, "--player-archive")) o->player_archive = value;
        else if (!strcmp(arg, "--code-bin")) { if (o->code_bin) return 0; o->code_bin = value; }
        else if (!strcmp(arg, "--save")) { if (o->save_path) return 0; o->save_path = value; }
        else if (!strcmp(arg, "--sound")) { if (o->sound) return 0; o->sound = value; }
        else if (!strcmp(arg, "--list-sounds")) { if (o->list_sounds) return 0; o->list_sounds = value; }
        else if (!strcmp(arg, "--touch-play")) { if (o->touch_play || !touch_script_valid(value)) return 0; o->touch_play = value; }
        else if (!strcmp(arg, "--load")) {
            /* The zone comes from the file (read in main); --zone must not be given too. */
            if (o->load_path || o->have_zone) return 0;
            o->load_path = value; o->have_zone = 1;
        }
        else if (!strcmp(arg, "--play")) {
            if (o->play || !px_hid_script_valid(value) || strlen(value) > 4096) return 0;
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
            else if (!strcmp(arg, "--motion-entry")) {
                if (o->have_motion_entry) return 0;
                o->motion_entry = n; o->have_motion_entry = 1;
            } else if (!strcmp(arg, "--motion-section")) {
                if (n > 255 || o->have_motion_section) return 0;
                o->motion_section = n; o->have_motion_section = 1;
            } else if (!strcmp(arg, "--motion")) {
                if (!n || o->have_motion_slot) return 0;
                o->motion_slot = n; o->have_motion_slot = 1;
            } else if (!strcmp(arg, "--zone")) {
                if (o->have_zone || n > 65535) return 0;
                o->zone = n; o->have_zone = 1;
            } else if (!strcmp(arg, "--partner")) {
                if (o->have_partner || !n || n > 798) return 0;
                o->partner = n; o->have_partner = 1;
            } else if (!strcmp(arg, "--partner-level")) {
                if (o->have_partner_level || !n || n > 100) return 0;
                o->partner_level = n; o->have_partner_level = 1;
            } else if (!strcmp(arg, "--seed")) {
                if (o->have_seed) return 0;
                o->seed = n; o->have_seed = 1;
            } else if (!strcmp(arg, "--balls")) {
                if (o->have_balls || n > 999) return 0;
                o->balls = n; o->have_balls = 1;
            } else if (!strcmp(arg, "--encounter-rate")) {
                if (o->have_encounter_rate || n > 100) return 0;
                o->encounter_rate = n; o->have_encounter_rate = 1;
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
    if (o->list_sounds) {
        /* A listing on its own: only --romfs. */
        return o->romfs && !o->have_zone && !o->archive && !o->demo && !o->sound && px_fs_path_valid(o->list_sounds);
    }
    if (o->sound) {
        const char *at = strchr(o->sound, '@');
        char path[256];
        const size_t n = at ? (size_t)(at - o->sound) : strlen(o->sound);
        if (!o->romfs || n >= sizeof path) return 0;
        memcpy(path, o->sound, n); path[n] = 0;
        if (!px_fs_path_valid(path) || (at && !at[1])) return 0;
        for (const char *d = at ? at + 1 : ""; *d; ++d) if (!isxdigit((unsigned char)*d) && *d != 'x') return 0;
    }
    if ((o->have_arrive || o->play) && !o->have_zone) return 0;
    if ((o->bottom_screen && !o->have_zone) || (o->touch_play && (!o->bottom_screen || !o->frames))) return 0;
    /* Saving and loading need the zone scene and code.bin (the record block order). */
    if ((o->save_path || o->load_path) && (!o->have_zone || !o->code_bin)) return 0;
    if (o->load_path && (o->have_arrive || o->have_partner || o->have_partner_level || o->have_balls || o->have_seed))
        return 0;
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
            o->have_motion_frame || o->have_walk_motion || o->have_motion_entry) return 0;
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
    if (o->have_motion_entry && (!o->have_motion_section || !o->skinned || o->player_archive)) return 0;
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

/* A RomFS GARC opened through the filesystem service; the stream stays
 * open (borrowed by the archive) until romfs_garc_close. */
struct romfs_garc { FILE *file; struct px_garc *garc; };
static int romfs_garc_open(const char *path, struct romfs_garc *out, char *error, size_t error_size)
{
    memset(out, 0, sizeof *out);
    const enum px_fs_result r = px_fs_open(PX_FS_ROMFS, path, &out->file);
    if (r != PX_FS_OK) { snprintf(error, error_size, "%s: %s", path, px_fs_result_string(r)); return 0; }
    if (!px_garc_open_file(out->file, &out->garc, error, error_size)) { fclose(out->file); out->file = NULL; return 0; }
    return 1;
}
static void romfs_garc_close(struct romfs_garc *g)
{
    if (g->garc) px_garc_close(g->garc);
    if (g->file) fclose(g->file);
    memset(g, 0, sizeof *g);
}

static int read_resource(const char *romfs, const char *archive_path, uint32_t entry,
                         uint32_t subentry, int have_section, uint32_t section,
                         const char *label, struct resource *out)
{
    (void)romfs; /* mounted as the RomFS archive in main */
    memset(out, 0, sizeof *out);
    out->compression = out->section_compression = "none";
    char error[300]; struct romfs_garc archive;
    int ok = romfs_garc_open(archive_path, &archive, error, sizeof error);
    if (!ok) { fprintf(stderr, "[model-viewer] %s archive: %s\n", label, error); return 0; }
    ok = px_garc_read(archive.garc, entry, subentry, 64 * 1024 * 1024,
                      &out->member, &out->member_size, error, sizeof error);
    romfs_garc_close(&archive);
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
                              memcmp(out->data, "BM", 2) && memcmp(out->data, "MM", 2) &&
                              memcmp(out->data, "PC", 2) && memcmp(out->data, "PT", 2) &&
                              memcmp(out->data, "PF", 2) && memcmp(out->data, "PB", 2) &&
                              memcmp(out->data, "PK", 2))) {
            snprintf(error, sizeof error, "--section requires an AD, GR, BM, MM, PC, PT, PF, PB or PK minipack");
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

static int same_name_bytes(const struct px_bch_name *a, const struct px_bch_name *b)
{
    return a->present && b->present && a->size == b->size && !memcmp(a->bytes, b->bytes, a->size);
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
    if (!read_resource(o->romfs, o->archive, o->have_motion_entry ? o->motion_entry : o->entry,
                       o->have_motion_entry ? 0 : o->subentry, 1, o->motion_section, "motion", &pack))
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
    if (o->vertex_shader && asset->skin_source.inputs && asset->skin_source.material_count) {
        /* A material whose program does not load keeps the stored colors. */
        const struct px_bch_model *m = &asset->skin_source;
        asset->programs = calloc(m->material_count, sizeof *asset->programs);
        asset->program_state = malloc(2 * sizeof *asset->program_state);
        if (!asset->programs || !asset->program_state) return 0;
        for (uint32_t i = 0; i < m->material_count; ++i) {
            asset->programs[i] = malloc(sizeof *asset->programs[i]);
            if (asset->programs[i] && !px_vertex_program_load(data, size, m, i, asset->programs[i])) {
                free(asset->programs[i]);
                asset->programs[i] = NULL;
            }
        }
    }
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
    asset->hide_invisible = o->hide_invisible;
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

/* A placeholder region model: one mesh of at most 6 vertices, flat at y=0
 * across exactly the cell, whose materials name no texture. Such cells fill
 * the matrix around a route (region 0 of matrix 0 is one); the viewer does
 * not draw them (an inspection choice: in the game they stay below the
 * terrain, out of the field camera's view). */
static int placeholder_region(const struct model_asset *r)
{
    const struct px_bch_model *b = &r->bch;
    if (b->mesh_count != 1 || r->wire.vertex_count > 6) return 0;
    for (unsigned c = 0; c < 3; c += 2)
        if (r->wire.bounds_min[c] != -360 || r->wire.bounds_max[c] != 360) return 0;
    if (r->wire.bounds_min[1] != 0 || r->wire.bounds_max[1] != 0) return 0;
    for (uint32_t i = 0; i < b->material_count; ++i)
        for (unsigned u = 0; u < 3; ++u)
            if (b->materials[i].texture_names[u].present) return 0;
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
    zs->region_hidden = calloc(zs->region_count ? zs->region_count : 1, 1);
    if (!zs->regions || !zs->region_offset || !zs->region_entry || !zs->region_hidden) return 0;
    zs->hide_main = placeholder_region(asset);
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
            if (k > 0) zs->region_hidden[k - 1] = (uint8_t)placeholder_region(r);
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
            struct px_field_span wild;
            if (px_field_section(r.data, r.size, "ZO", 3, &wild) == PX_FIELD_OK &&
                px_encounter_table_decode(wild.data, wild.size, &zs->wild) == PX_POKEDATA_OK) zs->has_wild = 1;
            resource_free(&r);
        }
    }
    {
        /* Optional: without the story text archive NPCs still face the player. */
        if (px_fs_exists(PX_FS_ROMFS, "a/0/8/2")) {
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
    zo.walk = 1; zo.steps = NULL; zo.allow_skeleton = 1; zo.hide_invisible = 1;
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
            uint32_t hidden = zs->hide_main;
            for (uint32_t i = 0; i < zs->region_count; ++i) hidden += zs->region_hidden[i];
            if (hidden) {
                printf("zone_placeholders zone=%" PRIu32 " regions=", zone);
                const char *sep = "";
                if (zs->hide_main) { printf("%" PRIu32, zs->region); sep = ","; }
                for (uint32_t i = 0; i < zs->region_count; ++i)
                    if (zs->region_hidden[i]) { printf("%s%" PRIu32, sep, zs->region_entry[i]); sep = ","; }
                printf(" drawn=no\n");
            }
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

/* --zone wild encounters and battles. Game tables load once, on the first
 * encounter: personal (a/2/1/8), moves (a/2/1/2), learnsets (a/2/1/4, per
 * species), the type chart (DllBattle.cro in the RomFS root), English names
 * (a/0/7/4 files 80, 14, 17) and, with --code-bin, the nature table. The
 * partner Pokémon and its HP persist across battles and zones.
 * Policy (inspection choices, not traced in the game's code): a completed
 * step onto tall grass starts an encounter with --encounter-rate percent
 * (default 10); the grass slot is chosen with the weights 20, 20, 10, 10,
 * 10, 10, 5, 5, 4, 4, 1, 1 (community-documented Generation VI odds), the
 * level uniformly in the slot's range, IVs 0..31 and the nature uniformly;
 * moves are the learnset's at that level. Losing heals the partner. */
struct game_session {
    int loaded, failed;
    struct px_personal *personal; uint32_t personal_count;
    struct px_move *moves; uint32_t move_count;
    uint8_t chart[PX_TYPE_COUNT][PX_TYPE_COUNT];
    uint8_t natures[125];
    int have_natures;
    uint8_t block_table[128]; /* code.bin 0x00570eec: the party record block order (game/save.h) */
    int have_block_table;
    struct px_guest image; int have_image, image_flat; /* the game's process image (platform/guest.h) */
    char **species_names, **move_names, **type_names;
    uint32_t species_name_count, move_name_count, type_name_count;
    uint32_t growth[8][101]; uint32_t growth_count;
    struct px_learnset *learnsets; uint32_t learnset_count;
    struct px_battle_data data;
    struct px_rng rng;
    struct px_battler partner;      /* the lead: party[0] while a battle is not running */
    struct px_battler party[6]; uint32_t party_count, balls;
    int have_partner, pending, active;
    struct px_battler wild;
    struct px_battle battle;
    struct px_battle_ui ui;
    uint32_t encounters, battles;
    /* Battle models (a/0/0/7): the species index (entry 0) and the loaded
     * model of each side (NULL: the placeholder), with its vertex buffers. */
    uint8_t *model_index; size_t model_index_size; int model_index_tried;
    struct model_asset *models[2];
    uint16_t model_species[2];
    struct px_raster_vertex *model_points[2];
    double (*model_normals[2])[4];
    float model_lo[2][3], model_hi[2][3]; /* bounds in the idle pose's first frame (else the rest pose) */
    /* Battle motions: per side the motion playing (NULL: the idle loop), its
     * start on the battle clock (rendered frames; it may lie ahead: the
     * damage reaction waits for the attack), and whether its last frame holds. */
    struct { const struct px_gf_motion *motion; uint64_t start; int hold; } anim[2];
    uint64_t clock, anim_key;
    struct options opts; int have_opts; /* the options battle models load with mid-battle (a switch) */
};
static struct game_session g_game;
static char g_code_name[256] = "code.bin", g_save_name[256], g_load_name[256]; /* names inside their archives */

static char **text_lines(const struct options *o, uint32_t file, uint32_t *count)
{
    struct resource r = {0};
    struct px_gf_text text;
    *count = 0;
    if (!read_resource(o->romfs, "a/0/7/4", file, 0, 0, 0, "game text", &r)) return NULL;
    const int ok = px_gf_text_decode(r.data, r.size, &text) == PX_GF_TEXT_OK;
    resource_free(&r);
    if (!ok) return NULL;
    char **lines = calloc(text.line_count ? text.line_count : 1, sizeof *lines);
    static char buffer[1024];
    for (uint32_t i = 0; lines && i < text.line_count; ++i) {
        if (px_gf_text_render(&text, i, "Player", buffer, sizeof buffer) < 0) buffer[0] = 0;
        lines[i] = malloc(strlen(buffer) + 1);
        if (lines[i]) strcpy(lines[i], buffer);
        else buffer[0] = 0;
    }
    if (lines) *count = text.line_count;
    px_gf_text_free(&text);
    return lines;
}

static int game_load(const struct options *o)
{
    struct game_session *g = &g_game;
    if (g->loaded || g->failed) return g->loaded;
    g->failed = 1;
    char error[300];
    const char *path;
    for (unsigned table = 0; table < 2; ++table) {
        path = table ? "a/2/1/2" : "a/2/1/8";
        struct romfs_garc garc;
        if (!romfs_garc_open(path, &garc, error, sizeof error)) {
            fprintf(stderr, "[model-viewer] battle data: %s\n", error); return 0;
        }
        struct px_garc *archive = garc.garc;
        /* The personal archive's last entry is every entry concatenated. */
        const uint32_t count = (uint32_t)px_garc_count(archive) - (table ? 0 : 1);
        if (table) g->moves = calloc(count, sizeof *g->moves); else g->personal = calloc(count, sizeof *g->personal);
        int ok = table ? g->moves != NULL : g->personal != NULL;
        for (uint32_t i = 0; ok && i < count; ++i) {
            uint8_t *member = NULL; size_t size = 0;
            ok = px_garc_read(archive, i, 0, 1u << 20, &member, &size, error, sizeof error) &&
                 (table ? px_move_decode(member, size, &g->moves[i]) : px_personal_decode(member, size, &g->personal[i]))
                 == PX_POKEDATA_OK;
            free(member);
        }
        romfs_garc_close(&garc);
        if (!ok) { fprintf(stderr, "[model-viewer] battle data: %s entries do not decode\n", path); return 0; }
        if (table) g->move_count = count; else g->personal_count = count;
    }
    uint8_t *cro = NULL;
    size_t cro_size = 0;
    if (px_fs_read_all(PX_FS_ROMFS, "DllBattle.cro", 16u << 20, &cro, &cro_size) != PX_FS_OK) cro_size = 0;
    const enum px_pokedata_result chart = px_type_chart_decode(cro, cro_size, g->chart);
    free(cro);
    if (chart != PX_POKEDATA_OK) {
        fprintf(stderr, "[model-viewer] battle data: DllBattle.cro type chart: %s\n", px_pokedata_result_string(chart));
        return 0;
    }
    if (o->code_bin) {
        /* The process image: code.bin mapped as the exheader places its
         * segments (exheader.bin beside it), else at code.bin offset =
         * address - 0x00100000 (how its page-aligned segments lie). The game
         * tables are then read at their own addresses. */
        uint8_t *code = NULL, *ex = NULL;
        size_t code_size = 0, ex_size = 0;
        if (px_fs_read_all(PX_FS_EXEFS, g_code_name, 64u << 20, &code, &code_size) == PX_FS_OK) {
            const int have_ex = px_fs_read_all(PX_FS_EXEFS, "exheader.bin", 4096, &ex, &ex_size) == PX_FS_OK;
            px_guest_init(&g->image);
            g->have_image = have_ex && px_guest_map_image(&g->image, code, code_size, ex, ex_size);
            if (!g->have_image) {
                px_guest_free(&g->image);
                g->have_image = px_guest_map(&g->image, PX_GUEST_CODE, (uint32_t)code_size,
                                             PX_GUEST_READ | PX_GUEST_EXECUTE, code, code_size, "code.bin");
                g->image_flat = 1;
            }
            if (g->have_image) {
                for (uint32_t k = 0; k < 125; ++k) g->natures[k] = px_guest_load8(&g->image, 0x00570f6c + k);
                for (uint32_t k = 0; k < 128; ++k) g->block_table[k] = px_guest_load8(&g->image, 0x00570eec + k);
                g->have_natures = g->have_block_table = g->image.fault == PX_FAULT_NONE;
                if (!o->silent)
                    printf("process_image source=%s regions=%u fault=%s\n",
                           g->image_flat ? "code.bin-flat" : "code.bin+exheader", g->image.region_count,
                           px_guest_fault_string(g->image.fault));
            }
        }
        free(code); free(ex);
    }
    /* Growth tables (a/2/1/7, 101 u32 per group) and every learnset (a/2/1/4). */
    for (unsigned table = 0; table < 2; ++table) {
        path = table ? "a/2/1/4" : "a/2/1/7";
        struct romfs_garc garc;
        if (!romfs_garc_open(path, &garc, error, sizeof error)) {
            fprintf(stderr, "[model-viewer] battle data: %s\n", error); return 0;
        }
        struct px_garc *archive = garc.garc;
        uint32_t count = (uint32_t)px_garc_count(archive);
        if (!table && count > 8) count = 8;
        if (table && !(g->learnsets = calloc(count ? count : 1, sizeof *g->learnsets))) { romfs_garc_close(&garc); return 0; }
        int ok = 1;
        for (uint32_t i = 0; ok && i < count; ++i) {
            uint8_t *member = NULL; size_t size = 0;
            ok = px_garc_read(archive, i, 0, 1u << 20, &member, &size, error, sizeof error);
            if (ok && table) ok = px_learnset_decode(member, size, &g->learnsets[i]) == PX_POKEDATA_OK;
            else if (ok) {
                ok = size >= 404;
                for (unsigned l = 0; ok && l <= 100; ++l)
                    g->growth[i][l] = (uint32_t)member[4 * l] | (uint32_t)member[4 * l + 1] << 8 |
                                      (uint32_t)member[4 * l + 2] << 16 | (uint32_t)member[4 * l + 3] << 24;
            }
            free(member);
        }
        romfs_garc_close(&garc);
        if (!ok) { fprintf(stderr, "[model-viewer] battle data: %s entries do not decode\n", path); return 0; }
        if (table) g->learnset_count = count; else g->growth_count = count;
    }
    g->balls = o->have_balls ? o->balls : 10;
    g->species_names = text_lines(o, 80, &g->species_name_count);
    g->move_names = text_lines(o, 14, &g->move_name_count);
    g->type_names = text_lines(o, 17, &g->type_name_count);
    g->data = (struct px_battle_data){g->personal, g->personal_count, g->moves, g->move_count,
                                      (const uint8_t (*)[PX_TYPE_COUNT])g->chart,
                                      g->have_natures ? g->natures : NULL, (const uint32_t (*)[101])g->growth,
                                      g->growth_count, g->learnsets, g->learnset_count};
    g->rng.state = o->have_seed ? (o->seed ? o->seed : 1) : 1;
    g->loaded = 1; g->failed = 0;
    if (!o->silent)
        printf("battle_data personal=%" PRIu32 " moves=%" PRIu32 " chart=DllBattle.cro:0xD12A8 natures=%s "
               "names=%" PRIu32 ",%" PRIu32 ",%" PRIu32 " growth=a/2/1/7:%" PRIu32 " learnsets=%" PRIu32 " balls=%" PRIu32
               " formulas=reconstructed-stats-and-levels,community-damage-and-catch\n",
               g->personal_count, g->move_count, g->have_natures ? "code.bin:0x00570f6c" : "neutral",
               g->species_name_count, g->move_name_count, g->type_name_count, g->growth_count, g->learnset_count,
               g->balls);
    return 1;
}

/* A Pokémon of the species at the level: random IVs and nature, its
 * learnset's moves at that level. */
static int game_pokemon(const struct options *o, uint16_t species, uint8_t level, struct px_battler *out)
{
    /* Read raw: learnset members are not compressed, and a first move id of
     * 16 or 17 would look like an LZ header. */
    static struct px_learnset learnset;
    (void)o;
    char error[300];
    struct romfs_garc garc;
    uint8_t *member = NULL; size_t size = 0;
    if (!romfs_garc_open("a/2/1/4", &garc, error, sizeof error)) { fprintf(stderr, "[model-viewer] learnsets: %s\n", error); return 0; }
    const int read = px_garc_read(garc.garc, species, 0, 1u << 20, &member, &size, error, sizeof error);
    romfs_garc_close(&garc);
    const enum px_pokedata_result lr = read ? px_learnset_decode(member, size, &learnset) : PX_POKEDATA_TRUNCATED;
    free(member);
    if (lr != PX_POKEDATA_OK) return 0;
    uint16_t moves[4];
    px_learnset_moves_at(&learnset, level, moves);
    uint8_t iv[6];
    for (unsigned k = 0; k < 6; ++k) iv[k] = (uint8_t)px_rng_below(&g_game.rng, 32);
    const uint8_t nature = (uint8_t)px_rng_below(&g_game.rng, 25);
    return px_battler_make(&g_game.data, species, level, iv, nature, moves, out);
}

static void print_battler(const char *what, const struct px_battler *b)
{
    printf("%s species=%u name=%s level=%u nature=%u iv=%u,%u,%u,%u,%u,%u stats=%u,%u,%u,%u,%u,%u moves=%u,%u,%u,%u\n",
           what, b->species, b->species < g_game.species_name_count ? g_game.species_names[b->species] : "?", b->level,
           b->nature, b->iv[0], b->iv[1], b->iv[2], b->iv[3], b->iv[4], b->iv[5], b->stats[0], b->stats[1],
           b->stats[2], b->stats[3], b->stats[4], b->stats[5], b->moves[0], b->moves[1], b->moves[2], b->moves[3]);
}

/* After a completed step onto grass: roll, and on an encounter prepare the
 * wild Pokémon (g_game.pending). Returns 1 on an encounter. */
static void battle_model_load(const struct options *o, unsigned side, uint16_t species);
/* The party's first Pokémon that can fight leads (0 if none can). */
static uint32_t first_able(void)
{
    for (uint32_t i = 0; i < g_game.party_count; ++i) if (g_game.party[i].hp) return i;
    return 0;
}
/* The partner is made on first need (the first encounter, or a save). */
static int ensure_partner(const struct options *o)
{
    if (g_game.have_partner) return 1;
    if (!game_load(o) ||
        !game_pokemon(o, (uint16_t)(o->have_partner ? o->partner : 656),
                      (uint8_t)(o->have_partner_level ? o->partner_level : 5), &g_game.partner)) return 0;
    g_game.have_partner = 1;
    g_game.party[0] = g_game.partner; g_game.party_count = 1;
    print_battler("partner", &g_game.partner);
    return 1;
}
/* --save / F5: the zone, the walker's world tile and facing, the party, the
 * balls and the random state (game/save.h); written to a temporary file and
 * renamed over the target. */
static int game_save(const struct options *o, const struct model_asset *asset)
{
    struct game_session *g = &g_game;
    const char *path = o->save_path ? o->save_path : o->load_path;
    if (!path || !asset->zone || !ensure_partner(o)) return 0;
    if (!g->have_block_table) { fprintf(stderr, "[model-viewer] save: code.bin block table not read\n"); return 0; }
    struct px_save save;
    memset(&save, 0, sizeof save);
    save.zone = asset->zone->zone;
    save.world_x = asset->walker.x + 40 * (int32_t)asset->zone->cell_x0;
    save.world_z = asset->walker.z + 40 * (int32_t)asset->zone->cell_z0;
    save.facing = (uint8_t)(asset->walker.facing & 3);
    save.balls = g->balls; save.rng = g->rng.state; save.trainer = 0;
    save.party_count = g->party_count;
    memcpy(save.party, g->party, sizeof save.party);
    uint8_t file[PX_SAVE_SIZE];
    char error[160], temporary[1100];
    if (!px_save_encode(&save, &g->data, g->block_table, file, error, sizeof error)) {
        fprintf(stderr, "[model-viewer] save: %s\n", error); return 0;
    }
    memcpy(g->party, save.party, sizeof g->party); /* keys assigned on first save stay */
    g->partner.key = g->party[0].key;
    (void)temporary;
    const enum px_fs_result written = px_fs_write_all(PX_FS_SAVE, o->save_path ? g_save_name : g_load_name, file,
                                                      sizeof file);
    if (written != PX_FS_OK) {
        fprintf(stderr, "[model-viewer] save: %s: %s\n", path, px_fs_result_string(written)); return 0;
    }
    printf("save path=%s zone=%" PRIu32 " world_tile=%" PRId32 ",%" PRId32 " facing=%u party=%" PRIu32
           " balls=%" PRIu32 " bytes=%u\n", path, save.zone, save.world_x, save.world_z, save.facing,
           save.party_count, save.balls, (unsigned)sizeof file);
    snprintf(asset->zone->message, sizeof asset->zone->message, "saved");
    return 1;
}
/* --load: the file's party, balls and random state; the zone is loaded by
 * main, which then places the walker with load_place. */
static struct px_save g_loaded;
static int game_restore(const struct options *o)
{
    struct game_session *g = &g_game;
    uint8_t *file = NULL;
    size_t size = 0;
    char error[160];
    const enum px_fs_result read = px_fs_read_all(PX_FS_SAVE, g_load_name, PX_SAVE_SIZE + 1, &file, &size);
    if (read != PX_FS_OK) {
        fprintf(stderr, "[model-viewer] --load: cannot read %s: %s\n", o->load_path, px_fs_result_string(read));
        return 0;
    }
    if (!game_load(o)) { free(file); return 0; }
    if (!g->have_block_table) {
        free(file); fprintf(stderr, "[model-viewer] --load: code.bin block table not read\n"); return 0;
    }
    const int decoded = px_save_decode(file, size, &g->data, g->block_table, &g_loaded, error, sizeof error);
    free(file);
    if (!decoded) { fprintf(stderr, "[model-viewer] --load: %s\n", error); return 0; }
    g->party_count = g_loaded.party_count;
    memcpy(g->party, g_loaded.party, sizeof g->party);
    g->partner = g->party[0]; g->have_partner = 1;
    g->balls = g_loaded.balls; g->rng.state = g_loaded.rng ? g_loaded.rng : 1;
    return 1;
}
static int load_place(struct model_asset *asset)
{
    struct walker *w = &asset->walker;
    const int32_t x = g_loaded.world_x - 40 * (int32_t)asset->zone->cell_x0;
    const int32_t z = g_loaded.world_z - 40 * (int32_t)asset->zone->cell_z0;
    if (x < 0 || z < 0 || x >= (int32_t)asset->grid.width || z >= (int32_t)asset->grid.height) {
        fprintf(stderr, "[model-viewer] --load: tile %" PRId32 ",%" PRId32 " is outside zone %" PRIu32 ".\n",
                g_loaded.world_x, g_loaded.world_z, g_loaded.zone);
        return 0;
    }
    w->x = x; w->z = z; w->facing = g_loaded.facing; w->active = 1;
    printf("load zone=%" PRIu32 " world_tile=%" PRId32 ",%" PRId32 " cell=%" PRId32 ",%" PRId32
           " facing=%u party=%" PRIu32 " balls=%" PRIu32 "\n", g_loaded.zone, g_loaded.world_x, g_loaded.world_z, x, z,
           g_loaded.facing, g_loaded.party_count, g_loaded.balls);
    for (uint32_t i = 0; i < g_game.party_count; ++i) {
        const struct px_battler *b = &g_game.party[i];
        printf("party_member slot=%" PRIu32 " species=%u level=%u exp=%u hp=%u/%u moves=%u,%u,%u,%u pp=%u,%u,%u,%u\n", i,
               b->species, b->level, (unsigned)b->exp, b->hp, b->stats[0], b->moves[0], b->moves[1], b->moves[2],
               b->moves[3], b->pp[0], b->pp[1], b->pp[2], b->pp[3]);
    }
    return 1;
}
static int maybe_encounter(const struct options *o, struct model_asset *asset)
{
    const struct zone_state *zs = asset->zone;
    const struct walker *w = &asset->walker;
    if (!zs || !zs->has_wild || !w->active || !w->last_moved) return 0;
    if (!grass_tile(asset->grid.tiles[(size_t)w->z * asset->grid.width + (size_t)w->x])) return 0;
    unsigned first, count;
    px_slot_range(PX_SLOTS_GRASS, &first, &count);
    if (!zs->wild.slots[first].species || !game_load(o)) return 0;
    const uint32_t rate = o->have_encounter_rate ? o->encounter_rate : 10;
    if (px_rng_below(&g_game.rng, 100) >= rate) return 0;
    static const unsigned weights[12] = {20, 20, 10, 10, 10, 10, 5, 5, 4, 4, 1, 1};
    unsigned roll = px_rng_below(&g_game.rng, 100), slot = 0;
    while (slot < 11 && roll >= weights[slot]) roll -= weights[slot++];
    const struct px_encounter_slot *s = &zs->wild.slots[first + slot];
    if (!s->species) return 0;
    const uint8_t level = (uint8_t)(s->min_level + px_rng_below(&g_game.rng, s->max_level - s->min_level + 1u));
    if (!game_pokemon(o, s->species, level, &g_game.wild)) return 0;
    if (!ensure_partner(o)) return 0;
    ++g_game.encounters;
    printf("encounter zone=%" PRIu32 " cell=%d,%d slot=%u species=%u level=%u\n", zs->zone, w->x, w->z, slot,
           s->species, level);
    print_battler("wild", &g_game.wild);
    if (!g_game.have_opts) { g_game.opts = *o; g_game.have_opts = 1; }
    battle_model_load(o, 0, g_game.party[first_able()].species);
    battle_model_load(o, 1, g_game.wild.species);
    g_game.pending = 1;
    return 1;
}

/* Pokémon battle models, a/0/0/7. Entry 0 indexes it by species: four bytes
 * per species from 1 (u16 first model, u8 model count, u8 flags). Model m
 * owns entries 4 + 8m .. 11 + 8m: a PC pack whose section 0 is the model BCH,
 * a PF pack of 31 BCH (not read), PT packs holding the textures (+2) and the
 * shiny textures (+3), a PT pack (+4, not read), a PB pack whose section 0
 * is a GF1 motion pack (+5; 29 slots in 972 of 981 models, empty in the
 * rest), then a PK and a PC pack (not read). Layout observed in the
 * file; the reading code of the game is not traced. The viewer draws the
 * species' first model (no gender or form selection; Pumpkaboo's and
 * Gourgeist's later size forms have empty PC packs), animated by its PB
 * pack's motions (below). */
enum { PX_POKEMON_MODEL_FIRST_ENTRY = 4, PX_POKEMON_MODEL_FILES = 8 };
static void battle_model_free(unsigned side)
{
    struct game_session *g = &g_game;
    if (g->models[side]) { asset_free(g->models[side]); free(g->models[side]); }
    free(g->model_points[side]); free(g->model_normals[side]);
    g->models[side] = NULL; g->model_points[side] = NULL; g->model_normals[side] = NULL; g->model_species[side] = 0;
}
static void battle_model_load(const struct options *o, unsigned side, uint16_t species)
{
    struct game_session *g = &g_game;
    static const char *const names[] = {"partner", "wild"};
    if (g->models[side] && g->model_species[side] == species) return;
    battle_model_free(side);
    if (!g->model_index_tried) {
        g->model_index_tried = 1;
        struct resource r = {0};
        if (px_fs_exists(PX_FS_ROMFS, "a/0/0/7")) {
            if (read_resource(o->romfs, "a/0/0/7", 0, 0, 0, 0, "battle model index", &r) &&
                (g->model_index = malloc(r.size ? r.size : 1))) {
                memcpy(g->model_index, r.data, r.size); g->model_index_size = r.size;
            }
            resource_free(&r);
        }
    }
    if (!g->model_index || !species || (size_t)species * 4 > g->model_index_size) {
        if (!o->silent) printf("battle_model side=%s species=%u model=none\n", names[side], species);
        return;
    }
    const uint8_t *rec = g->model_index + ((size_t)species - 1) * 4;
    const uint32_t model = (uint32_t)rec[0] | (uint32_t)rec[1] << 8;
    struct options po = *o;
    po.have_zone = 0; po.have_arrive = 0; po.archive = "a/0/0/7"; po.have_entry = 1; po.subentry = 0; po.model = 0;
    po.entry = PX_POKEMON_MODEL_FIRST_ENTRY + PX_POKEMON_MODEL_FILES * model;
    po.have_section = 1; po.section = 0;
    po.triangle_list = po.textured = po.combiners = 1;
    po.texture_archive = "a/0/0/7"; po.texture_entry = po.entry + 2; po.have_texture_entry = 1;
    po.texture_subentry = 0; po.have_texture_section = 1; po.texture_section = 0;
    po.environment_archive = "a/0/0/3"; po.environment_entry = 137; po.have_environment_entry = 1;
    po.environment_subentry = 0; po.have_environment_section = 1; po.environment_section = 0; po.scene = 0;
    po.skinned = 1; po.allow_skeleton = 1; po.hide_invisible = 1;
    po.walk = 0; po.steps = NULL; po.player_archive = NULL; po.quiet = 1; po.silent = 1;
    po.have_motion_frame = 0; po.have_walk_motion = po.have_run_motion = 0;
    /* The battle motions: section 0 of the model's PB pack (+5), a GF1 pack;
     * slot 1 loops as the idle motion. A model whose pack does not decode or
     * lacks slot 1 stands in the rest pose. */
    po.have_motion_entry = po.have_motion_section = po.have_motion_slot = 1;
    po.motion_entry = po.entry + 5; po.motion_section = 0; po.motion_slot = 1;
    struct model_asset *a = calloc(1, sizeof *a);
    int ok = a && load(&po, a) && a->bch.mesh_count && a->bch_combiner;
    if (!ok && a) {
        asset_free(a); memset(a, 0, sizeof *a);
        po.have_motion_entry = po.have_motion_section = po.have_motion_slot = 0;
        ok = load(&po, a) && a->bch.mesh_count && a->bch_combiner;
    }
    if (ok) {
        const size_t n = a->wire.vertex_count ? a->wire.vertex_count : 1;
        g->model_points[side] = malloc(n * sizeof *g->model_points[side]);
        g->model_normals[side] = malloc(n * sizeof *g->model_normals[side]);
        ok = g->model_points[side] && g->model_normals[side];
    }
    if (!ok) {
        if (a) { asset_free(a); free(a); }
        free(g->model_points[side]); free(g->model_normals[side]);
        g->model_points[side] = NULL; g->model_normals[side] = NULL;
        if (!o->silent) printf("battle_model side=%s species=%u model=%" PRIu32 " entry=%" PRIu32 " failed\n",
                               names[side], species, model, po.entry);
        return;
    }
    g->models[side] = a; g->model_species[side] = species;
    for (uint32_t k = 0; k < a->wire.vertex_count; ++k)
        for (unsigned c = 0; c < 3; ++c) {
            const float v = a->wire.positions[(size_t)k * 3 + c];
            if (!k || v < g->model_lo[side][c]) g->model_lo[side][c] = v;
            if (!k || v > g->model_hi[side][c]) g->model_hi[side][c] = v;
        }
    if (!o->silent) {
        printf("battle_model side=%s species=%u model=%" PRIu32 " entry=%" PRIu32 " texture_entry=%" PRIu32
               " meshes=%" PRIu32 " vertices=%" PRIu32, names[side], species, model, po.entry,
               po.texture_entry, a->bch.mesh_count, a->wire.vertex_count);
        if (a->motion_ready) {
            printf(" motion_entry=%" PRIu32 " motions=%" PRIu32 " slots=", po.motion_entry, a->motion_pack.motion_count);
            for (uint32_t k = 0; k < a->motion_pack.motion_count; ++k)
                printf("%s%" PRIu32 ":%" PRIu32, k ? "," : "", a->motion_pack.motions[k].slot,
                       a->motion_pack.motions[k].frames);
            printf(" pose=idle\n");
        } else printf(" pose=rest\n");
    }
}

static void battle_begin(void)
{
    struct game_session *g = &g_game;
    g->pending = 0; g->active = 1; ++g->battles;
    memset(g->anim, 0, sizeof g->anim); g->anim_key = 0;
    const uint32_t lead = first_able();
    px_battle_start(&g->battle, &g->data, &g->party[lead], &g->wild, px_rng_next(&g->rng));
    px_battle_set_party(&g->battle, g->party, g->party_count, lead);
    g->battle.balls = g->balls;
    px_battle_ui_start(&g->ui, &g->battle, (const char *const *)g->species_names, g->species_name_count,
                       (const char *const *)g->move_names, g->move_name_count, (const char *const *)g->type_names);
}

/* One battle input; ends the battle after its last page. */
static void battle_after(uint32_t before);
static void battle_input(char key)
{
    struct game_session *g = &g_game;
    const uint32_t before = g->ui.phase == PX_UI_MESSAGES ? g->ui.page_count : 0;
    px_battle_ui_input(&g->ui, &g->battle, key);
    battle_after(before);
}
/* A tap on the bottom screen in battle. */
static void battle_touch(int x, int y)
{
    struct game_session *g = &g_game;
    const uint32_t before = g->ui.phase == PX_UI_MESSAGES ? g->ui.page_count : 0;
    printf("touch x=%d y=%d phase=%d\n", x, y, (int)g->ui.phase);
    px_battle_ui_touch(&g->ui, &g->battle, x, y);
    battle_after(before);
}
static void battle_after(uint32_t before)
{
    struct game_session *g = &g_game;
    if (g->ui.phase == PX_UI_MESSAGES && g->ui.page_count > before)
        for (uint32_t i = before; i < g->ui.page_count; ++i)
            printf("battle turn=%" PRIu32 " message=\"%s\"\n", g->battle.turn, g->ui.pages[i]);
    if (g->ui.phase != PX_UI_DONE) return;
    static const char *const result[] = {"choosing", "won", "lost", "escaped", "caught", "switching"};
    /* The party comes back as the battle left it; losing heals every member
     * where the player stands (no Pokémon Center yet). */
    memcpy(g->party, g->battle.party, g->battle.party_count * sizeof *g->party);
    for (uint32_t i = 0; i < g->party_count; ++i) {
        memset(g->party[i].stages, 0, sizeof g->party[i].stages);
        if (g->battle.state == PX_BATTLE_LOST) g->party[i].hp = g->party[i].stats[0];
    }
    g->partner = g->party[0];
    g->balls = g->battle.balls;
    battle_model_free(1);
    const struct px_battler *active = &g->party[g->battle.active];
    printf("battle_end result=%s turns=%" PRIu32 " partner_hp=%u/%u level=%u exp=%u wild_hp=%u/%u balls=%u",
           result[g->battle.state], g->battle.turn, active->hp, active->stats[0], active->level,
           (unsigned)active->exp, g->battle.side[1].hp, g->battle.side[1].stats[0], (unsigned)g->balls);
    if (g->battle.active) printf(" active=%" PRIu32, g->battle.active);
    putchar('\n');
    if (g->battle.state == PX_BATTLE_CAUGHT) {
        /* The catch joins the party (a full party keeps it out: no PC boxes yet). */
        if (g->party_count < 6) {
            g->party[g->party_count] = g->battle.side[1];
            memset(g->party[g->party_count].stages, 0, sizeof g->party[0].stages);
            ++g->party_count;
        }
        printf("party count=%u", (unsigned)g->party_count);
        for (uint32_t i = 0; i < g->party_count; ++i)
            printf(" %u:%s:Lv%u:%u/%u", (unsigned)i, g->party[i].species < g->species_name_count ?
                   g->species_names[g->party[i].species] : "?", g->party[i].level, g->party[i].hp, g->party[i].stats[0]);
        putchar('\n');
    }
    g->active = 0;
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
        else if (asset->walker.last_moved && !zone_cross(o, asset) && maybe_encounter(o, asset))
            g_game.pending = 0; /* --steps reports encounters; battles need the display loop */
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
    if (c->in_battle) {
        /* Battle menu: the pad (WASD or arrows, Space/Return A, X/Backspace
         * B); Escape still exits. */
        switch (e->key.keysym.sym) {
        case SDLK_ESCAPE: c->running = 0; break;
        case SDLK_w: case SDLK_UP: px_hid_press(&c->hid, PX_KEY_DUP); break;
        case SDLK_s: case SDLK_DOWN: px_hid_press(&c->hid, PX_KEY_DDOWN); break;
        case SDLK_a: case SDLK_LEFT: px_hid_press(&c->hid, PX_KEY_DLEFT); break;
        case SDLK_d: case SDLK_RIGHT: px_hid_press(&c->hid, PX_KEY_DRIGHT); break;
        case SDLK_SPACE: case SDLK_RETURN: px_hid_press(&c->hid, PX_KEY_A); break;
        case SDLK_x: case SDLK_BACKSPACE: px_hid_press(&c->hid, PX_KEY_B); break;
        case SDLK_F5: c->save_request = 1; break; /* saved once back on the field */
        default: break;
        }
        return;
    }
    switch (e->key.keysym.sym) {
    case SDLK_ESCAPE: c->running = 0; break;
    case SDLK_LEFT: c->yaw -= 0.1; break;
    case SDLK_RIGHT: c->yaw += 0.1; break;
    case SDLK_UP: c->pitch -= 0.1; break;
    case SDLK_DOWN: c->pitch += 0.1; break;
    case SDLK_PLUS: case SDLK_EQUALS: case SDLK_KP_PLUS: c->zoom *= 1.25; break;
    case SDLK_MINUS: case SDLK_KP_MINUS: c->zoom /= 1.25; break;
    case SDLK_r: case SDLK_HOME: reset(c); break;
    case SDLK_w: if (c->zone) px_hid_press(&c->hid, PX_KEY_DUP); else if (c->walk) walk_step(c->walk, 'U'); break;
    case SDLK_s: if (c->zone) px_hid_press(&c->hid, PX_KEY_DDOWN); else if (c->walk) walk_step(c->walk, 'D'); break;
    case SDLK_a: if (c->zone) px_hid_press(&c->hid, PX_KEY_DLEFT); else if (c->walk) walk_step(c->walk, 'L'); break;
    case SDLK_d: if (c->zone) px_hid_press(&c->hid, PX_KEY_DRIGHT); else if (c->walk) walk_step(c->walk, 'R'); break;
    case SDLK_SPACE: case SDLK_RETURN: if (c->zone) px_hid_press(&c->hid, PX_KEY_A); break;
    case SDLK_F5: if (c->zone) c->save_request = 1; break;
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
        if (!inject(c, SDLK_d)) return 0;
        pad_frame(c, 0);
        if (c->queued != 'R' || !inject(c, SDLK_w)) return 0;
        pad_frame(c, 0);
        if (c->queued != 'U' || !inject(c, SDLK_SPACE)) return 0;
        pad_frame(c, 0);
        if (!c->interact) return 0;
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
    /* --vertex-shader: the fragment lighting unit (render/frag_light.h)
     * replaces the directional approximation; varyings 0..3 are the
     * program's quaternion, 4..6 its view vector. */
    const struct px_frag_light_config *fragment_light;
};

static uint8_t quantize(double value)
{
    return (uint8_t)floor((value < 0 ? 0 : value > 1 ? 1 : value) * 255.0 + 0.5);
}

static void combiner_fragment(void *context, double u, double v, const double rgba[4],
                              const double varying[8], double out[4])
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
    if (s->fragment_light) {
        const struct px_frag_light_config *f = s->fragment_light;
        const float q[4] = {(float)varying[0], (float)varying[1], (float)varying[2], (float)varying[3]};
        const float view[3] = {(float)varying[4], (float)varying[5], (float)varying[6]};
        if (!px_frag_light(f, q, view, f->bump ? in.texture[f->bump_unit] : NULL, in.fragment_primary,
                           in.fragment_secondary)) {
            memset(in.fragment_primary, 0, 3); memset(in.fragment_secondary, 0, 3);
            in.fragment_primary[3] = in.fragment_secondary[3] = 255;
            degenerate = 1;
        }
    } else if (s->light) {
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
        if (asset->hide_invisible && !m->visible) continue;
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
            const int executed = asset->fragment_light && asset->fragment_ok[m->material_index] && asset->program_varyings;
            if (executed) shader.fragment_light = &asset->fragment_light[m->material_index];
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
                double varyings[24];
                if (executed) {
                    for (unsigned k = 0; k < 3; ++k)
                        memcpy(varyings + 8 * k, asset->program_varyings[mesh->indices[first + k]], 8 * sizeof *varyings);
                    result = px_raster_draw_varying8(target, triangle, varyings, combiner_fragment, &shader, &state);
                } else {
                    for (unsigned k = 0; k < 3; ++k)
                        memcpy(varyings + 4 * k, normals[mesh->indices[first + k]], 4 * sizeof *varyings);
                    result = px_raster_draw_varying(target, triangle, varyings, combiner_fragment, &shader, &state);
                }
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
    if (!asset->zone || !asset->zone->hide_main) {
        project_bch(asset, v, main_at.x || main_at.z ? &main_at : NULL, points, normals);
        if (!draw_bch_meshes(asset, points, normals, target, counts)) return 0;
    }
    for (uint32_t i = 0; asset->zone && i < asset->zone->region_count; ++i) {
        /* The zone's other regions, at their cell offsets (same buffers). */
        const struct placement at = {asset->zone->region_offset[i][0], 0, asset->zone->region_offset[i][1], 0, 0, {0}};
        if (asset->zone->region_hidden && asset->zone->region_hidden[i]) continue;
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

/* The battle models on the stage of game/battle_ui.h. Viewer policy, not the
 * game's battle camera: an orthographic view pitched 0.25 rad down, one
 * world unit per pixel; each model scaled to fit a box (wild 64 x 110
 * pixels, partner 96 x 150) with its lowest point on its platform's centre,
 * the wild one turned toward the player's side, the partner seen from behind.
 * Caught Pokémon, and fainted ones once their page has passed, are not drawn. */
/* Battle motion slots, inferred by viewing Froakie's motions and from how
 * many of the 972 packs hold each slot (not traced in the game's battle
 * code): 1 the idle loop (972 packs), 4 / 5 / 6 a physical, special and
 * status move (661 each), 17 the damage reaction (971), 18 fainting (921).
 * A missing slot plays the idle loop instead (fainting: the model is hidden
 * when its page passes, as without motions). One motion frame per two
 * rendered frames, as in the field. */
enum { PX_MOTION_IDLE = 1, PX_MOTION_PHYSICAL = 4, PX_MOTION_SPECIAL = 5, PX_MOTION_STATUS = 6,
       PX_MOTION_DAMAGE = 17, PX_MOTION_FAINT = 18 };
static void battle_play(unsigned side, uint32_t slot, uint64_t start, int hold)
{
    struct game_session *g = &g_game;
    if (!g->models[side] || !g->models[side]->motion_ready) return;
    const struct px_gf_motion *m = px_gf_motion_find(&g->models[side]->motion_pack, slot);
    if (!m || !m->frames) return;
    g->anim[side].motion = m; g->anim[side].start = start; g->anim[side].hold = hold;
}
/* Advance the battle clock; a page that becomes current starts its motions. */
static void battle_animate(void)
{
    struct game_session *g = &g_game;
    const struct px_battle_ui *ui = &g->ui;
    const uint64_t now = ++g->clock;
    const uint64_t key = ui->phase == PX_UI_MESSAGES && ui->page < ui->page_count ?
        ((uint64_t)g->battle.turn << 32 | (uint64_t)ui->page << 1 | 1) : 0;
    if (key && key != g->anim_key) {
        const uint8_t e = ui->page_event[ui->page];
        const unsigned side = (e & PX_LOG_SIDE1) ? 1 : 0, kind = e & PX_LOG_KIND;
        if (kind >= PX_LOG_PHYSICAL && kind <= PX_LOG_STATUS) {
            static const uint32_t slot[] = {0, PX_MOTION_PHYSICAL, PX_MOTION_SPECIAL, PX_MOTION_STATUS};
            battle_play(side, slot[kind], now, 0);
            const struct px_gf_motion *attack = g->anim[side].start == now ? g->anim[side].motion : NULL;
            if (e & PX_LOG_HIT) battle_play(!side, PX_MOTION_DAMAGE, now + (attack ? 2u * attack->frames : 0), 0);
        } else if (kind == PX_LOG_FAINT) battle_play(side, PX_MOTION_FAINT, now, 1);
        else if (kind == PX_LOG_SEND && g->have_opts) {
            /* "Go, Y!": the player's side now shows the Pokémon sent out. */
            battle_model_load(&g->opts, 0, g->battle.side[0].species);
            memset(&g->anim[0], 0, sizeof g->anim[0]);
        }
    }
    g->anim_key = key;
    for (unsigned s = 0; s < 2; ++s) {
        struct model_asset *m = g->models[s];
        if (!m || !m->motion_ready || !m->motion) continue;
        const struct px_gf_motion *motion = g->anim[s].motion;
        uint32_t frame = 0;
        if (motion && now >= g->anim[s].start) {
            const uint64_t t = (now - g->anim[s].start) / 2;
            if (t < motion->frames || g->anim[s].hold) frame = (uint32_t)(t < motion->frames ? t : motion->frames);
            else motion = g->anim[s].motion = NULL;
        } else motion = NULL;
        if (!motion) {
            motion = m->motion;
            frame = motion->frames ? (uint32_t)((now / 2) % motion->frames) : 0;
        }
        pose_motion(m, motion, frame);
    }
}
/* A fainted Pokémon stays on the stage until its fainting page has passed. */
static int battle_shown(unsigned s)
{
    struct game_session *g = &g_game;
    if (s && g->battle.state == PX_BATTLE_CAUGHT) return 0;
    if (g->battle.side[s].hp) return 1;
    if (g->ui.phase != PX_UI_MESSAGES) return 0;
    for (uint32_t i = g->ui.page; i < g->ui.page_count; ++i)
        if ((g->ui.page_event[i] & PX_LOG_KIND) == PX_LOG_FAINT && ((g->ui.page_event[i] & PX_LOG_SIDE1) ? 1u : 0u) == s)
            return 1;
    return 0;
}
/* --vertex-shader: run each mesh's own vertex program for every drawn index
 * and take its color output (o3) as the primary color, as the PICA feeds the
 * combiners. The Pokémon programs write per-vertex lighting there (diffuse,
 * Phong and rim terms in separate channels; see reproduction/vertex-shader).
 * Positions and UVs stay the CPU path's (they agree). The engine's matrices
 * are not traced: WrldMtx is the battle placement, ViewMtx the viewer's
 * camera rotation with the camera 1000 units in front, so the programs'
 * view-dependent terms see a nearby perspective eye. */
/* --vertex-shader: the fragment lighting unit's state per material, as the
 * game's runtime sets it from the material and the scene (render/
 * frag_light.h). From the material: the fragment flags (+6: bit 0 bump
 * renormalization, 1 clamp highlights, 2 Distribution0, 3 Distribution1,
 * 4 reflection), the LUT input registers, the LUT table and sampler names
 * (matched against the model's LUT sets), the Fresnel selector and bump
 * mode and texture. Lights (reproduction/battle-lights): the battle looks
 * up light_poke_1 .. light_poke_6 by name in the scene (DllBattle.cro
 * 0x38f54..0x38fb4), one per battle position; the partner takes
 * light_poke_1 and the wild Pokémon light_poke_2 here (the position-to-
 * light mapping is not traced; the six lights are identical in the field
 * scene). Their vectors are rotated into view space (view: the camera
 * rows). Light colors are the registers the traced color loop (code.bin
 * 0x3030a8, reproduction/bch-lighting) writes; for specular1 that loop
 * writes the light's own color when its fourth argument is set
 * (0x3031d8). The caller passes bit 5 of the material's lighting flags,
 * the same bit that leaves the reflection LUTs enabled in the lighting
 * config register (0x2e9030..0x2e9074), so it is set exactly for
 * materials with reflection. A material reading no LUT that is present
 * keeps the directional approximation. */
static int build_fragment_light(struct model_asset *a, const float view[12], unsigned side)
{
    const struct px_bch_model *m = &a->skin_source;
    if (!a->bch_light || !a->light_index || m->material_count != a->bch.material_count) return 0;
    if (!a->fragment_light) {
        a->fragment_light = calloc(m->material_count, sizeof *a->fragment_light);
        a->fragment_ok = calloc(m->material_count, 1);
        a->program_varyings = calloc(a->wire.vertex_count ? a->wire.vertex_count : 1, sizeof *a->program_varyings);
        if (!a->fragment_light || !a->fragment_ok || !a->program_varyings) return 0;
    }
    const struct px_bch_environment *env = &a->environment;
    for (uint32_t i = 0; i < m->material_count; ++i) {
        const struct px_bch_material *mat = &m->materials[i];
        struct px_frag_light_config *c = &a->fragment_light[i];
        memset(c, 0, sizeof *c);
        a->fragment_ok[i] = 0;
        if (!a->programs[i] || !(mat->material_flags & 1) || (mat->lut_known & 7) != 7 || mat->bump_mode > 1) continue;
        px_frag_light_inputs(c, mat->lut_registers[0], mat->lut_registers[1], mat->lut_registers[2]);
        /* Material LUT slots 0 D0, 1 D1, 2 Fresnel, 3..5 reflection R, G, B. */
        static const unsigned slot[6] = {PX_LUT_D0, PX_LUT_D1, PX_LUT_FR, PX_LUT_RR, PX_LUT_RG, PX_LUT_RB};
        const uint32_t flags = mat->fragment_flags;
        const int wanted[6] = {(flags >> 2) & 1, (flags >> 3) & 1, mat->fresnel_selector != 0,
                               (flags >> 4) & 1, (flags >> 4) & 1, (flags >> 4) & 1};
        unsigned found = 0;
        for (unsigned k = 0; k < 6; ++k) {
            if (!wanted[k] || !mat->lut_table[k].present || !mat->lut_sampler[k].present) continue;
            for (uint32_t l = 0; l < m->lut_count; ++l)
                if (same_name_bytes(&m->luts[l].table, &mat->lut_table[k]) &&
                    same_name_bytes(&m->luts[l].sampler, &mat->lut_sampler[k])) {
                    c->lut[slot[k]] = m->luts[l].entries;
                    ++found;
                    break;
                }
        }
        if (!found) continue;
        c->clamp_highlights = (flags >> 1) & 1;
        c->bump = mat->bump_mode == 1 && mat->bump_texture < 3 &&
                  ((a->bch_combiner[i].sampled_units >> mat->bump_texture) & 1);
        c->bump_unit = c->bump ? (int)mat->bump_texture : 0;
        c->bump_renormalize = flags & 1;
        c->fresnel_primary = mat->fresnel_selector & 1;
        c->fresnel_secondary = (mat->fresnel_selector >> 1) & 1;
        /* The lights: the set holding this position's light_poke_N when the
         * scene has it, else the material's own set as prepare_bch_lighting
         * selected it. */
        const char *wanted_light = side ? "light_poke_2" : "light_poke_1";
        const struct px_bch_light_set *set = NULL;
        for (uint32_t k = 0; k < env->set_count && !set; ++k)
            if (env->sets[k].count == 1 && name_is(&env->lights[env->set_lights[env->sets[k].first]].name, wanted_light))
                set = &env->sets[k];
        const struct px_light_setup *setup = &a->bch_light[i];
        memcpy(c->global_ambient, setup->global_ambient, sizeof c->global_ambient);
        c->count = set ? set->count : setup->count;
        for (uint32_t l = 0; l < c->count; ++l) {
            const struct px_bch_light *light = &env->lights[set ? env->set_lights[set->first + l] :
                                                            a->light_index[(size_t)i * 8 + l]];
            struct px_frag_light_source *to = &c->lights[l];
            float vector[3];
            for (unsigned k = 0; k < 3; ++k) vector[k] = -light->direction[k] + 0.0f;
            for (unsigned k = 0; k < 3; ++k) {
                to->ambient[k] = px_light_register_channel(light->ambient[k], mat->colors[1][k]);
                to->diffuse[k] = px_light_register_channel(light->diffuse[k], mat->colors[2][k]);
                to->specular0[k] = px_light_register_channel(light->specular0[k], mat->colors[3][k]);
                to->specular1[k] = (flags >> 4) & 1 ? light->specular1[k] :
                                   px_light_register_channel(light->specular1[k], mat->colors[4][k]);
                to->vector[k] = view[k * 4] * vector[0] + view[k * 4 + 1] * vector[1] + view[k * 4 + 2] * vector[2];
            }
            to->two_sided = (int)(light->flags & 1);
        }
        a->fragment_ok[i] = 1;
    }
    return 1;
}

static void shade_battle_model(struct model_asset *a, const struct placement *at, const struct view *v,
                               struct px_raster_vertex *points, unsigned side)
{
    const struct px_bch_model *m = &a->skin_source;
    if (!a->programs || !a->program_state) return;
    const double c = cos(at->yaw), s = sin(at->yaw), k = at->scale[0];
    struct px_vertex_frame frame = {
        {(float)(k * c), 0, (float)(k * s), (float)at->x, 0, (float)k, 0, (float)at->y,
         (float)(-k * s), 0, (float)(k * c), (float)at->z},
        {(float)v->cy, 0, (float)v->sy, 0, (float)(v->sp * v->sy), (float)v->cp, (float)(-v->sp * v->cy), 0,
         (float)(-v->cp * v->sy), (float)v->sp, (float)(v->cp * v->cy), -1000},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        a->motion_fixed ? a->motion_world : a->motion_rest};
    struct px_pica_state *prepared = &a->program_state[0], *work = &a->program_state[1];
    if (!build_fragment_light(a, frame.view, side)) return;
    for (uint32_t i = 0; i < m->mesh_count; ++i) {
        const struct px_bch_mesh *mesh = m->meshes + i;
        const struct px_vertex_program *program = a->programs[mesh->material_index];
        if (!program) continue;
        for (uint32_t sub = 0; sub < mesh->submesh_count; ++sub) {
            const struct px_bch_submesh *sm = m->submeshes + mesh->first_submesh + sub;
            if (!px_vertex_program_prepare(program, m, i, sub, &frame, prepared)) continue;
            for (uint32_t x = sm->first_index; x < sm->first_index + sm->index_count && x < m->index_count; ++x) {
                float color[4], quaternion[4] = {0, 0, 0, 1};
                if (px_vertex_program_run(program, m, m->indices[x], prepared, work) != PX_PICA_OK ||
                    px_vertex_program_output(program, work, PX_VERTEX_OUTPUT_COLOR, color) != 15) continue;
                for (unsigned j = 0; j < 4; ++j)
                    points[x].rgba[j] = !(color[j] > 0) ? 0 : color[j] > 1 ? 1 : color[j];
                float view[4] = {0, 0, 0, 0};
                px_vertex_program_output(program, work, PX_VERTEX_OUTPUT_QUATERNION, quaternion);
                px_vertex_program_output(program, work, PX_VERTEX_OUTPUT_VIEW, view);
                for (unsigned j = 0; j < 8; ++j) {
                    const float value = j < 4 ? quaternion[j] : j < 7 ? view[j - 4] : 0;
                    a->program_varyings[x][j] = isfinite(value) && fabsf(value) <= 1e6f ? value : 0;
                }
            }
        }
    }
}

static int draw_battle_models(struct px_raster_target *target)
{
    struct game_session *g = &g_game;
    for (unsigned s = 0; s < 2; ++s) {
        struct model_asset *m = g->models[s];
        if (!m || !battle_shown(s)) continue;
        const float *lo = g->model_lo[s], *hi = g->model_hi[s];
        const double height = hi[1] - lo[1] > 1e-6 ? hi[1] - lo[1] : 1;
        const double width = hi[0] - lo[0] > hi[2] - lo[2] ? hi[0] - lo[0] : hi[2] - lo[2];
        double k = (s ? 64.0 : 96.0) / height;
        if (width * k > (s ? 110.0 : 150.0)) k = (s ? 110.0 : 150.0) / width;
        struct view v = {{0, 0, 0}, 1, 1, 1, 0, cos(0.25), sin(0.25), (int)target->width, (int)target->height};
        const double w = target->width, h = target->height;
        const double z = s ? -300 : 300, feet = s ? h * 9 / 20 : h * 3 / 4 - 8;
        struct placement at = {(s ? w * 3 / 4 : w / 4) - w / 2 - k * (lo[0] + hi[0]) / 2, 0, z,
                               s ? 0.4 : 3.14159265358979323846 - 0.6, 1, {k, k, k}};
        at.y = (h / 2 + v.sp * z - feet) / v.cp - k * lo[1];
        project_bch(m, &v, &at, g->model_points[s], g->model_normals[s]);
        shade_battle_model(m, &at, &v, g->model_points[s], s);
        if (!draw_bch_meshes(m, g->model_points[s], g->model_normals[s], target, NULL)) return 0;
    }
    return 1;
}

/* The bottom screen: the battle's touch menu, or in the field the party. */
static void draw_bottom_screen(void)
{
    uint8_t *rgba = g_bottom.rgba;
    const uint32_t w = PX_TOUCH_WIDTH, h = PX_TOUCH_HEIGHT;
    if (g_game.active) { px_battle_ui_draw_touch(rgba, w, h, &g_game.ui, &g_game.battle); return; }
    static const uint8_t back[4] = {214, 228, 214, 255}, ink[4] = {40, 48, 40, 255};
    px_text_rect(rgba, w, h, 0, 0, (int)w, (int)h, back);
    char line[96];
    px_text_draw(rgba, w, h, 10, 8, "PARTY", ink, 1);
    for (uint32_t i = 0; i < g_game.party_count; ++i) {
        const struct px_battler *b = &g_game.party[i];
        snprintf(line, sizeof line, "%s  Lv%u  HP %u/%u", b->species < g_game.species_name_count ?
                 g_game.species_names[b->species] : "?", b->level, b->hp, b->stats[0]);
        px_text_draw(rgba, w, h, 16, 28 + 18 * (int)i, line, ink, 1);
    }
    if (!g_game.party_count) px_text_draw(rgba, w, h, 16, 28, "(your partner joins at the first encounter)", ink, 1);
    snprintf(line, sizeof line, "Poke Balls: %u", (unsigned)(g_game.loaded ? g_game.balls : 0));
    px_text_draw(rgba, w, h, 10, 220, line, ink, 1);
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
    const unsigned bands = g_game.active ? 0 : counts ? 1 : b->bands;
    struct band_job jobs[PX_MAX_BANDS];
    struct px_thread *threads[PX_MAX_BANDS] = {0};
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
        if (!(threads[k] = px_thread_create(band_thread, &jobs[k], "band", 0x30, -2))) band_thread(&jobs[k]);
    int all = 1;
    if (bands) {
        jobs[0].ok = draw_scene(asset, &v, jobs[0].points, jobs[0].normals, jobs[0].player_points,
                                jobs[0].player_normals, &jobs[0].target, counts);
        all = jobs[0].ok;
    } else {
        /* The battle: stage, models (depth-tested among themselves), HUD. */
        const unsigned modeled = (g_game.models[0] ? 1u : 0) | (g_game.models[1] ? 2u : 0);
        px_battle_ui_draw_stage(target->rgba, target->width, target->height, &g_game.battle, modeled);
        battle_animate();
        all = draw_battle_models(target);
        px_battle_ui_draw_hud(target->rgba, target->width, target->height, &g_game.ui, &g_game.battle);
    }
    for (unsigned k = 1; k < bands; ++k) {
        if (threads[k]) px_thread_join(threads[k]);
        all &= jobs[k].ok;
    }
    if (!all) return 0;
    if (asset->zone && asset->zone->page_count && !g_game.active)
        px_text_window(target->rgba, target->width, target->height, asset->zone->pages[asset->zone->page],
                       asset->zone->page + 1 < asset->zone->page_count);
    const SDL_Rect top = {0, 0, w, h}, bottom = {PX_BOTTOM_X, PX_BOTTOM_Y, PX_TOUCH_WIDTH, PX_TOUCH_HEIGHT};
    if (SDL_UpdateTexture(*frame, NULL, target->rgba, w * 4) || SDL_RenderClear(renderer) ||
        SDL_RenderCopy(renderer, *frame, NULL, g_bottom.enabled ? &top : NULL)) return 0;
    if (g_bottom.enabled) {
        draw_bottom_screen();
        if (SDL_UpdateTexture(g_bottom.texture, NULL, g_bottom.rgba, PX_TOUCH_WIDTH * 4) ||
            SDL_RenderCopy(renderer, g_bottom.texture, NULL, &bottom)) return 0;
    }
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
    if (!zm->stepping && g_game.pending) { battle_begin(); zm->phase = 0; }
    c->in_battle = g_game.active;
    if (g_game.active) {
        /* Battle: one key (or one --play input) per frame drives the menu. */
        char key = c->battle_key;
        c->battle_key = 0; c->queued = 0; c->interact = 0;
        if (!key && c->play && *c->play) {
            const char next = *c->play++;
            if (next == 's') c->save_request = 1; /* saved once back on the field */
            else key = pad_battle_key(px_hid_script_buttons(next));
        }
        if (key) battle_input(key);
        else if (c->bottom && c->pad && (c->pad->down & PX_KEY_TOUCH)) battle_touch(c->pad->touch_x, c->pad->touch_y);
        c->in_battle = g_game.active;
        return 1;
    }
    if (!zm->stepping && !asset->zone->page_count) {
        const uint32_t held = c->pad ? c->pad->held : 0;
        char direction = pad_direction(held & (PX_KEY_DPAD | PX_KEY_CPAD));
        if (!direction) direction = c->queued;
        c->queued = 0;
        int run = (held & PX_KEY_B) != 0;
        if (!direction && c->play && *c->play) {
            /* --play: one scripted pad input per free controller frame. */
            const char next = *c->play++;
            const uint32_t buttons = px_hid_script_buttons(next);
            if (next == 's') c->save_request = 1;
            else if (buttons & PX_KEY_A) c->interact = 1;
            else {
                run = (buttons & PX_KEY_B) != 0;
                direction = pad_direction(buttons);
            }
        }
        if (c->save_request) { c->save_request = 0; game_save(o, asset); }
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
        if (moved && !*swapped) maybe_encounter(o, asset); /* the battle starts once the step ends */
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

/* --sound / --list-sounds: BCWAVs read through the filesystem service. */
static const char *const encoding_names[] = {"pcm8", "pcm16", "dsp-adpcm"};
static int sound_split(const char *spec, char *path, size_t size, uint64_t *offset)
{
    const char *at = strchr(spec, '@');
    const size_t n = at ? (size_t)(at - spec) : strlen(spec);
    if (n >= size) return 0;
    memcpy(path, spec, n); path[n] = 0;
    *offset = at ? strtoull(at + 1, NULL, 0) : 0;
    return 1;
}
static int list_sounds(const char *path)
{
    uint8_t *data = NULL; size_t size = 0;
    const enum px_fs_result r = px_fs_read_all(PX_FS_ROMFS, path, (size_t)1 << 31, &data, &size);
    if (r != PX_FS_OK) { fprintf(stderr, "[model-viewer] --list-sounds: %s: %s\n", path, px_fs_result_string(r)); return 0; }
    uint32_t found = 0, valid = 0;
    for (size_t at = 0; at + 4 <= size; ++at) {
        if (memcmp(data + at, "CWAV", 4)) continue;
        struct px_audio_wave waves[2]; uint32_t channels;
        const int result = px_cwav_decode(data + at, size - at, waves, &channels);
        ++found;
        if (result != PX_CWAV_OK) { printf("sound offset=0x%zx result=\"%s\"\n", at, px_cwav_result_string(result)); continue; }
        ++valid;
        printf("sound offset=0x%zx channels=%" PRIu32 " encoding=%s rate=%" PRIu32 " samples=%" PRIu32 " loop=%d loop_start=%" PRIu32 "\n",
               at, channels, encoding_names[waves[0].encoding], waves[0].rate, waves[0].samples, waves[0].loop, waves[0].loop_start);
    }
    printf("sounds path=%s bytes=%zu cwav_magics=%" PRIu32 " decoded=%" PRIu32 "\n", path, size, found, valid);
    free(data);
    return 1;
}
/* The --sound voice(s) and the device; silent (with a note) when the host
 * has no audio. */
static struct px_audio g_audio;
static struct px_audio_device *g_audio_device;
static uint8_t *g_sound_file;
static void sound_start(const struct options *o)
{
    char path[256], error[256];
    uint64_t offset = 0;
    if (!o->sound || !sound_split(o->sound, path, sizeof path, &offset)) return;
    size_t size = 0;
    const enum px_fs_result r = px_fs_read_all(PX_FS_ROMFS, path, (size_t)1 << 31, &g_sound_file, &size);
    if (r != PX_FS_OK || offset >= size) {
        fprintf(stderr, "[model-viewer] --sound: %s: %s\n", path, r != PX_FS_OK ? px_fs_result_string(r) : "offset beyond the file");
        return;
    }
    struct px_audio_wave waves[2]; uint32_t channels;
    const int result = px_cwav_decode(g_sound_file + offset, size - (size_t)offset, waves, &channels);
    if (result != PX_CWAV_OK) { fprintf(stderr, "[model-viewer] --sound: %s\n", px_cwav_result_string(result)); return; }
    px_audio_init(&g_audio, PX_DSP_RATE);
    int ok = channels == 1 ? px_audio_play(&g_audio, 0, &waves[0], 1, 1) :
             px_audio_play(&g_audio, 0, &waves[0], 1, 0) && px_audio_play(&g_audio, 1, &waves[1], 0, 1);
    if (!ok) { fprintf(stderr, "[model-viewer] --sound: the wave does not decode\n"); px_audio_free(&g_audio); return; }
    g_audio_device = px_audio_device_open(&g_audio, error, sizeof error);
    printf("audio driver=%s mix_rate=%u device_rate=%" PRIu32 " voices=%d sound=%s@0x%" PRIx64 " channels=%" PRIu32
           " encoding=%s rate=%" PRIu32 " samples=%" PRIu32 " loop=%d\n", g_audio_device ? px_audio_device_driver() : "none",
           (unsigned)PX_DSP_RATE, px_audio_device_rate(g_audio_device), PX_AUDIO_VOICES, path, offset, channels,
           encoding_names[waves[0].encoding], waves[0].rate, waves[0].samples, waves[0].loop);
    if (!g_audio_device) fprintf(stderr, "[model-viewer] audio device: %s (continuing silently)\n", error);
}
static void sound_stop(void)
{
    px_audio_device_close(g_audio_device); g_audio_device = NULL;
    px_audio_free(&g_audio);
    free(g_sound_file); g_sound_file = NULL;
}

static uint64_t host_now_ns(void *user)
{
    (void)user;
    const Uint64 count = SDL_GetPerformanceCounter(), rate = SDL_GetPerformanceFrequency();
    return (uint64_t)(count / rate) * 1000000000u + (uint64_t)(count % rate) * 1000000000u / rate;
}

static int window(const struct options *o, struct model_asset *asset)
{
    const struct px_cgfx_model *mesh = o->textured && !asset->bch_textured ?
        &asset->scene.mesh : &asset->wire;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) return 0;
    SDL_Window *win = SDL_CreateWindow(o->textured ?
        "Pokemon X unlit textured preview — arrows: orbit | +/-: zoom | R: reset | Esc: exit" :
        "Pokemon X wireframe — arrows: orbit | +/-: zoom | R: reset | Esc: exit",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, asset->zone ? 800 : 960,
        asset->zone ? (o->bottom_screen ? 960 : 480) : 640, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = NULL; int ok = 0;
    SDL_Texture *frame = NULL; struct px_raster_target target = {0};
    struct frame_buffers b;
    memset(&b, 0, sizeof b);
    if (!win) goto done;
    if (!o->software) renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer) goto done;
    if (!buffers_alloc(o, asset, &b)) goto done;
    if (o->bottom_screen && asset->zone) {
        /* Two screens in a 400 x 480 logical window: SDL scales it and maps
         * the mouse into it. */
        g_bottom.enabled = 1;
        g_bottom.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING,
                                             PX_TOUCH_WIDTH, PX_TOUCH_HEIGHT);
        if (!g_bottom.texture || SDL_RenderSetLogicalSize(renderer, 400, 480)) goto done;
    }
    sound_start(o);
    struct camera c;
    memset(&c, 0, sizeof c);
    c.bottom = g_bottom.enabled;
    const char *taps = o->touch_play;
    struct touch_tap tap = {0};
    int have_tap = taps && touch_script_next(&taps, &tap);
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
    /* The frame clock (platform/clock.h): fixed for runs with --frames, so
     * they repeat; live otherwise, paced to 60 Hz. */
    struct px_clock clock;
    px_clock_init(&clock, o->frames != 0, host_now_ns, NULL);
    px_hid_init(&c.hid);
    if (asset->zone) zone_title(win, asset);
    while (c.running) {
        SDL_Event e; while (SDL_PollEvent(&e)) event(&c, &e);
        if (!c.running) break;
        /* The touch screen: the left mouse button on the bottom screen, or a
         * scripted tap for this frame. */
        if (c.bottom) {
            int touching = 0, tx = 0, ty = 0;
            if (have_tap && tap.frame == frames) {
                touching = 1; tx = tap.x; ty = tap.y;
                have_tap = touch_script_next(&taps, &tap);
            } else if (!o->frames) {
                int mx, my; float lx, ly;
                if (SDL_GetMouseState(&mx, &my) & SDL_BUTTON_LMASK) {
                    SDL_RenderWindowToLogical(renderer, mx, my, &lx, &ly);
                    tx = (int)lx - PX_BOTTOM_X; ty = (int)ly - PX_BOTTOM_Y;
                    touching = tx >= 0 && ty >= 0 && tx < PX_TOUCH_WIDTH && ty < PX_TOUCH_HEIGHT;
                }
            }
            px_hid_touch(&c.hid, touching, tx, ty);
        }
        pad_frame(&c, o->frames ? 0 : host_held(c.in_battle));
        const char *path = frames == o->capture_at ? o->capture : NULL;
        if (asset->zone) {
            int swapped = 0;
            const char before = asset->zone->message[0];
            const uint64_t now = px_clock_ms(&clock);
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
        const uint64_t wait = px_clock_end_frame(&clock);
        if (wait) SDL_Delay((Uint32)((wait + 999999) / 1000000));
    }
    fprintf(stderr, "[model-viewer] Exit after %" PRIu32 " frames.\n", frames); ok = 1;
done:
    if (!ok) fprintf(stderr, "[model-viewer] Runtime: %s\n", SDL_GetError());
    buffers_free(&b);
    px_raster_free(&target); if (frame) SDL_DestroyTexture(frame);
    sound_stop();
    if (g_bottom.texture) SDL_DestroyTexture(g_bottom.texture);
    g_bottom.texture = NULL; g_bottom.enabled = 0;
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
    /* The filesystem service: the RomFS, the ExeFS inputs and the save
     * archive are mounted on the host paths the options name. */
    px_fs_reset();
    if (o.romfs) px_fs_mount(PX_FS_ROMFS, o.romfs, 0);
    if (o.code_bin && px_fs_mount_file(PX_FS_EXEFS, o.code_bin, 0, g_code_name, sizeof g_code_name) != PX_FS_OK) {
        fprintf(stderr, "[model-viewer] --code-bin: invalid path\n"); return 1;
    }
    if (o.load_path) {
        if (px_fs_mount_file(PX_FS_SAVE, o.load_path, 1, g_load_name, sizeof g_load_name) != PX_FS_OK) {
            fprintf(stderr, "[model-viewer] --load: invalid path\n"); return 1;
        }
        if (!game_restore(&o)) return 1;
        o.zone = g_loaded.zone;
    }
    if (o.list_sounds) return list_sounds(o.list_sounds) ? 0 : 1;
    if (o.save_path && px_fs_mount_file(PX_FS_SAVE, o.save_path, 1, g_save_name, sizeof g_save_name) != PX_FS_OK) {
        fprintf(stderr, "[model-viewer] --save: invalid path\n"); return 1;
    }
    if (o.have_zone) {
        if (!load_zone(&o, o.zone, &asset)) { asset_free(&asset); return 1; }
        if (o.load_path && !load_place(&asset)) { asset_free(&asset); return 1; }
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
