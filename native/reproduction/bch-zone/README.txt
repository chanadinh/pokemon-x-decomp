Native zone scenes: the player's house with warps, an NPC and a walking controller, 2026-10-24

--zone N builds the scene from game data instead of explicit archive paths:
the zone's header, matrix, field region, area textures, entities and NPC
registry. The player walks in real time, talks to the NPC and takes the stairs
between the two floors of the house. Tracked files contain independently
authored source, synthetic tests and metadata only; the documentation PNGs are
recorded in docs/images/README.md.

Native preview

native/build/pokemon_model_viewer --romfs /path/to/romfs --zone 5

Zone 5 is the bedroom (region 165, the room of the earlier records); zone 4 is
the floor below (region 164). WASD walks (held keys keep walking), Shift runs,
Space or Return talks, arrows orbit, Escape exits. The window title shows the
zone and the latest event. --arrive N starts on warp N, --steps applies moves
before display or --inspect (warps included), --play UDLR scripts input during
display (u/d/l/r run, . talks), and --capture-at N picks the captured frame of
a --frames run.

What is resolved from game data

- Zone header (a/0/1/2 entry N, ZO section 0): area u16 at 2, matrix u16 at 4
  (as in ../field-scene, corroborated there by the executable).
- Matrix (a/0/4/2): one region; zones whose matrix is not 1 x 1 (towns and
  routes) are refused with a message, and a warp into one does not load.
- Region (a/0/4/1, GR): section 1 model, section 0 terrain grid, section 2
  collision, as before.
- Area textures: a/0/1/3 entry = area, AD section 11 (as before).
- Entities (ZO section 1) and NPC registry (a/1/4/9 entry = area): layout from
  pk3DS 6daaca9 (OWSEStructs.cs) and CTRMap 4b780a7 (ZoneEntities.java,
  NPCRegistry.java, NPCEditForm.java, WarpTool.java); see assets/zone.h. Not
  traced in the game's code here.

  Warp records of coordinate type 0 (674 of 871 warps, all indoor-style) give
  destination zone and warp, face direction, world x and y (tile =
  floor(v / 18)), width and height in tiles and directionality. The 197 type-1
  warps (outdoor zones) do not fit that layout: they are kept raw and never
  fire. NPCs give a registry model id, script, face direction and tile.
  Registry entries map that id to an a/0/2/1 entry (offset 0x14).

The last entry of a/0/1/2 (360) is not a zone: it equals the 360 zone headers
concatenated in zone order.

Inspection choices (not traced)

- Environment a/0/0/3 entry 137 for every zone (the only one with lights).
- Player model a/0/2/1 entry 2 with motion slots 1 (idle), 3 (walk), 4 (run);
  NPCs with their MM section 1 slot 1 when it poses, else the rest pose.
- Start: first exact standard cell (0x00000020) in row-major order that no NPC
  occupies.
- Movement: exact standard cells only; NPC tiles block. A step into a warp
  that is not receive-only loads its target zone; on failure the step falls
  back to a plain move and the title says why.
- Arrival: the target warp's first tile, facing its face direction, then one
  step that way when the cell is walkable and not itself a warp. This fits all
  three warps of the house: the stairs down arrive at (4,10) under the stairs,
  the stairs up at (4,15) beside them, the front door would step inside.
- Controller: a step lasts 11 walk-cycle frames, or 5 run-cycle frames with
  Shift, roughly the stride of the planted foot in each motion (measured from
  the posed feet: about 1.6 units per frame walking, 4 running); the cycle
  phase carries over consecutive steps; the player glides linearly between
  cell centers and the camera follows. Motions run at 30 frames per second of
  real time, or of a 60 Hz virtual clock when --frames bounds the run.
- Talking: the NPC ahead turns toward the player and its script number is
  reported; the zone script VM (and therefore the dialogue) is not executed.
- Rendering: 400 x 240 (the 3DS top screen) scaled to an 800 x 480 window, so
  the software renderer reaches interactive rates (about 26 ms per frame in
  this container); frames are paced to about 60 Hz.

Independent comparison (verify_zone.py)

cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror native/assets/zone.c \
  native/reproduction/bch-zone/dump_zone.c -o native/build/dump_zone
python3 -I -B native/reproduction/bch-zone/verify_zone.py --romfs /path/to/romfs \
  --bridge native/build/dump_zone --viewer native/build/pokemon_model_viewer \
  --output-dir outputs/new-zone-check

It parses every zone's entities and every registry itself and compares all
fields with the native decoder: 360 zones (733 furniture, 2,524 NPCs, 871
warps, 381 + 117 triggers) and 170 registries (1,421 entries), all identical.
It re-implements the walk policy and predicts four scripted walks through the
house (bedroom to the stairs and down; downstairs to the NPC; to the front
door, whose outdoor target is refused; down and back up), which the viewer's
--inspect lines match exactly. The display loop with --play
DDDRDDLRRRRUUUURRUU. over 520 frames takes the stairs and talks to the NPC
(id 0, script 1); capture hashes of frames 0, 60, 200 and 519 are in
reference-comparison.json.

Tests

assets/test_zone.c: hand-built entities (furniture, two NPCs, warps of both
coordinate types including negative coordinates and receive-only, triggers,
script bytes), warp rectangles, the registry, and rejections (length mismatch,
truncation, trigger cap, zero width, unknown directionality, NaN height).
tests/test_bch_zone_viewer.py: a synthetic RomFS at the real archive paths
(three zones, a 2 x 1 matrix, a lit skinned character with idle/walk/run
motions, an NPC registry): zone, NPC and warp lines, a blocked step into the
NPC, a warp with arrival, a refused outdoor warp, the runtime walk with talk
and warp lines, running versus walking, the input self-test, an NPC whose
model is missing (still blocks), option errors and help text.

Limits

Indoor single-region zones only: towns and routes (multi-region matrices,
building models, type-1 warps) are not loaded. Scripts, dialogue, triggers,
furniture interaction, NPC movement, transitions and the game's spawn,
controller and collision rules are not executed. Day/night selection and the
environment binding remain inferred.
