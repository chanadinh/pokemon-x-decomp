Pokémon battle models: Route 2, 2026-10-24

The coloured circles of the battle screen give way to the Pokémon's own 3D
models from a/0/0/7 (1.04 GB; moved here in four parts and rejoined, GARC
header size 1,041,219,296 bytes matched). Both sides are drawn with the
game's textures through the six-stage combiner and field lighting the zone
scenes use, in the rest pose. Tracked files contain independently authored
source, synthetic tests and metadata only; the documentation PNG is
recorded in docs/images/README.md.

Native preview

native/build/pokemon_model_viewer --romfs /path/to/romfs --zone 259
  (a/0/0/7 must be in the RomFS; without it the placeholders stay.)
A single model: --archive a/0/0/7 --entry 4+8m --section 0 --skinned
  --triangle-list --textured --combiners --texture-archive a/0/0/7
  --texture-entry 6+8m --texture-section 0 (m from the index below).

What comes from game data (layout observed in the file; the game's own
reading code is not traced)

- a/0/0/7 entry 0: four bytes per species from 1 (u16 first model, u8
  model count, u8 flags 1/3/5/7); the 722 records (to the egg) tile models
  0..980; 147 species have more than one model (genders, forms, Megas).
- Model m owns entries 4 + 8m .. 11 + 8m, always of kinds PC PF PT PT PT PB
  PK PC. PC section 0 is the skinned model BCH (e.g. pm0723_00, Froakie:
  87 bones, 10 meshes, 8 materials), +2 a PT pack of its textures, +3 the
  shiny textures. PF (31 BCH, presumably motions) and the rest are not read.
- Four models (963, 964, 967, 968: Pumpkaboo's and Gourgeist's later size
  forms) have empty PC packs.

Decoder changes

- BCH header words 48 and 52 (the sizes of the uninitialized data and
  description areas the game reserves at load; 9,912 and 29,648 bytes in
  Froakie) are accepted when nonzero; nothing is stored there and no
  relocation targets them. Field and character BCH have them 0.
- Camera cube (1) and sphere (2) environment-mapped texture units join the
  unexecuted (projected) units of the combiner path, substituted white as
  projection units are (139 models used them).
- --section also selects PC, PT and PF packs.

Policy (viewer choices, stated in model_app.c and game/battle_ui.h)

- Each species shows its first model (no gender, form or shiny choice) in
  the rest pose, with the field environment a/0/0/3 entry 137 for light.
- The battle view: orthographic, pitched 0.25 rad, one unit per pixel; each
  model scaled into a box (wild 64 x 110, partner 96 x 150 pixels) with its
  lowest point on its platform; the wild one turned 0.4 rad, the partner
  seen from behind. Not the game's battle camera or scale.
- A species whose model does not load keeps its type-coloured placeholder
  (battle_model ... failed / model=none).

Independent comparison (verify_pokemon.py)

cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror native/assets/bch_model.c \
  native/render/skin.c native/assets/gf_motion.c \
  native/reproduction/bch-player/dump_player.c -lm -o native/build/dump_player
  (gf_motion.c is needed since the motion milestone; the bch-player
  README's command predates it)
python3 -I -B native/reproduction/bch-pokemon/verify_pokemon.py --romfs /path/to/romfs \
  --bridge native/build/dump_player --viewer native/build/pokemon_model_viewer \
  --out outputs/new-pokemon-check

The index tiles all 981 models, every model has the eight-file pattern, and
all 977 non-empty models decode identically in the independent skinned BCH
parser of ../bch-player (skeleton, meshes, skinning sources, palettes, bone
indices and weights); 57 of them (every 20th and the Route 2 species with
Froakie) also match the native rest-pose bake at every index exactly (largest
difference 0). Every species' first model is among them. On Route 2 the
viewer names Froakie's model 864 (entry 6916, 10 meshes, 25,536 vertices)
and Scatterbug's 872 (entry 6980, 9 meshes, 12,156 vertices), as derived
(reference-comparison.json). Separately, the viewer loads all 977 with their
textures through the combiner path (--inspect, validation.txt).

Tests

assets/test_bch_model.c (nonzero reserved sizes decode; another
compatibility byte rejects), assets/test_bch_material.c (cube and sphere
maps become unexecuted units; mapping 4 rejects),
tests/test_bch_combiner_viewer.py (the failure case is now mapping mode 4),
tests/test_bch_battle_viewer.py (a synthetic a/0/0/7: index, a skinned model
with its own textures for the partner, an empty pack for the wild species;
the model replaces only the partner's placeholder; model=none without the
archive).

Limits

Rest pose only: the battle motions (PF) are not decoded, so arms stand out
and some Pokémon lie oddly (Scatterbug). No vertex shader, bump or
environment maps, fresnel or outline passes; Froakie in particular comes out
pale. No form, gender or shiny selection, no game battle camera, scale or
backgrounds. No running-game comparison.
