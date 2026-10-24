Native lit field with a walkable inspection marker, 2026-10-24

This milestone joins the two halves of field 165 that existed separately: the
rendered room (BCH model with textures, six combiner stages and directional
lighting) and the terrain grid and collision of the same GR member. A marker
can now be walked around the lit room. Tracked files contain independently
authored source, synthetic tests and metadata only; the documentation PNG is
recorded in docs/images/README.md.

Native preview

native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/0/4/1 --entry 165 --section 1 --triangle-list --textured --combiners \
  --texture-archive a/0/1/3 --texture-entry 75 --texture-section 11 \
  --environment-archive a/0/0/3 --environment-entry 137 --environment-section 0 \
  --walk                      # WASD moves the marker; --steps UDLR... scripts it

--walk requires the BCH to come from a GR section (--section) and the
textured preview. It decodes the GR's section 0 grid and section 2 collision
with the existing field decoders, draws a 14 x 28 x 14 box on the first stored
collision triangle under the marker's cell center, and starts the camera
looking steeply down (yaw 0, pitch -1.2) because culling is not executed and
the room's walls would otherwise hide the floor. --inspect prints one walk
line: policy, grid, standard-cell count, collision vertices, marker cell,
local center, collision height, moves and blocked moves. --self-test-input
also checks that D then A returns the marker to its cell.

Movement policy (unchanged from the field inspector)

Only the exact terrain word 0x00000020 admits the marker; it starts at the
first such cell in row-major order and moves one cardinal cell per key (W/U =
-Z, S/D = +Z, A/L = -X, D/R = +X in region-local space). Blocked steps are
counted. This is an inspection policy over verified resource data, not the
game's player movement, collision response, running, ledges or triggers.

Shared frame (verify_walk.py, recorded in reference-comparison.json)

Cell centers are (cell + 0.5) * 18 - 360 in region-local units, as in
docs/native-field.md. The room model is decoded by the independent Python
decoder of ../bch-model; the verifier finds a path to each exact standard cell
by breadth-first search and drives the viewer there with --steps. Result for
field 165 (GR member sha256 f4df7947...dae9):
- 52 exact standard cells, all reachable from the start cell (3,10).
- At every one the viewer's reported cell, local center and move count match,
  with no blocked step, and its collision height (0 at all 52) equals a flat
  model surface under the center. The model, grid and collision therefore
  share one frame at all walkable cells; no offset is applied.
- 25 cells (a 5 x 5 block) also have a flat model surface at 0.5: a carpet
  above the floor. Collision follows the floor.
- Three U steps from the start are all blocked.

Runtime capture: real archive paths, --steps DDDRRR, 3 frames, input
self-test, software: sha256 55f1b9b2...db5e (sanitized build identical).

Tests

tests/test_bch_walk_viewer.py: a hand-built GR pack (grid, BCH, collision
triangle): start cell, local center and height, a nine-step script with
blocked moves, a cell outside the collision (height none), marker pixels only
with --walk, no marker without standard cells, option errors, a non-GR pack
failing before video, and help text.

Limits

No player model, animation or camera of the game; the marker is a box. Game
movement and collision rules, entry/spawn, warps, interaction, encounters,
culling and the field camera are not executed. The earlier captures of
../bch-textured, ../bch-combiners and ../bch-lighting are unchanged.
