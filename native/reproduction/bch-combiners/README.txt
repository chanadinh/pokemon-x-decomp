Native BCH six-stage combiner preview, 2026-10-23

This milestone runs each field material's complete texture-environment (TEV)
combiner chain, all six stages, instead of only stage 0, and feeds it the
model's decoded vertex colors. The inputs that still depend on unexecuted
GPU work are replaced by declared values, and their influence is measured.
Tracked files contain independently authored source, synthetic tests and
metadata only. No payload, pixel array or capture is included.

Native preview

make -C native all test test-runtime
native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/0/4/1 --entry 165 --section 1 --triangle-list --textured --combiners \
  --texture-archive a/0/1/3 --texture-entry 75 --texture-section 11

--combiners requires the BCH --textured options. Without it the stage-0 preview
of ../bch-textured is unchanged (its recorded capture still reproduces
byte-for-byte). --inspect prints the per-material chain, units and every stage
register. A rendered run prints one combiner_sensitivity line per material
after the first frame.

What is new

render/tev.c evaluates six stages on 8-bit RGBA: sources, RGB/alpha operands,
the ten operations (replace, modulate, add, add-signed, lerp, subtract, dot3
RGB/RGBA, multiply-add, add-multiply), 1x/2x/4x scale with saturation, the
combiner buffer with its one-stage delay and per-stage RGB/alpha update bits,
and the stage-0 rule that replaces a "previous" first/second RGB source by the
third. Register layouts follow the pinned CTRMap reader; arithmetic follows the
documented behavior of the Azahar software rasterizer (commit 1b5fd009; file
hashes in the report). That is an emulator, not a hardware measurement; its
own comments note dot3 may be off by up to 3. No emulator code was copied.

The BCH reader now returns per-vertex colors: the stored color attribute after
its uniform scale, or a fixed attribute uploaded through registers 0x232..0x235
(three words per attribute, float24 components x,y,z,w = R,G,B,A, index
advancing after each attribute, as in the reference). It also returns the
combiner buffer color 0xfd. In field 165, eight meshes store colors and four
use fixed colors (white for the window chip, gray for the mirror, a green tint
for the grayscale curtain and a brown tint for the gray shadow texture).

The rasterizer gained a per-fragment shader hook (px_raster_draw_shaded) with
unchanged coverage, depth, alpha-test and blend rules, and exposes its sampler.

Declared input policy (printed by --inspect)
- Primary color = the decoded vertex color, interpolated. The vertex shader is
  not executed, so this is an assumption about what the shader outputs.
- Fragment primary = white, fragment secondary = black: fragment lighting
  (enabled in every field material) is not executed.
- Projection-mapped units (unit 0 projection_dummy in four materials) are not
  executed; a white texel substitutes.
- A unit the chain reads while disabled in 0x80 reads (0,0,0,0), as in the
  reference (material t01r02_stairs reads disabled unit 1).
- Ordinary units sample UV0 with their sampler; only UV0 is stored.
- Culling is reported, not executed; draw order is layer, priority, resource.

Field 165 result

All twelve chains are accepted. Under the field's own constants, the projected
texture and fragment secondary cannot change any output: an independent oracle
tried 3,000 random input sets per material with each substitute at both
extremes and saw no change, and the viewer counted zero sensitive fragments
among those it shaded. The projected term is multiplied by a zero stage-4
constant; whether the game sets that constant at run time is not known. Every
chain does multiply by fragment primary (lighting), so lighting is now the main
unexecuted input, together with the vertex shader's primary color. Baked
vertex colors visibly darken corners; the stair mesh has vertices with zero
color and draws black under this policy.

Independent comparison

cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror native/assets/bch_model.c \
  native/assets/bch_material.c native/render/tev.c \
  native/reproduction/bch-combiners/dump_combiners.c -lm -o native/build/dump_combiners
python3 -I native/reproduction/bch-combiners/verify_combiners.py \
  --field /path/to/field165.bch --bridge native/build/dump_combiners \
  --viewer native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --azahar /path/to/azahar-checkout --output-dir outputs/new-combiner-check

The verifier's TEV oracle is table-driven Python written separately from
tev.c. It reuses only the section/relocation resolver of ../bch-textured's
verifier and otherwise parses command packets, uniforms, fixed attributes and
vertex buffers itself.

Recorded result (reference-comparison.json):
- 20,000 random configurations x 4 inputs = 80,000 evaluations (57,724 of
  valid configurations; invalid ones exercise reserved sources, operands,
  operations, scales and update bits): validity and every RGBA byte match.
- 12 field chains, buffer colors, update bits, source sets and unit classes
  match; 36,000 field evaluations match.
- 2,356 vertex colors in 12 meshes match bit-for-bit, including all four fixed
  colors.
- Runtime: three-frame software capture and input self-test; capture sha256
  bb9e433d...f0c4; sensitivity lines recorded.
As in ../bch-textured, the capture used single-member archives around the
hash-pinned field and texture payloads; the real RomFS path was not readable
from this session. The default verifier arguments use the real archive paths.

Tests

render/test_tev.c: hand-computed results for every operation, operand and
scale, constant byte order, previous/buffer delay, the stage-0 substitution,
reserved encodings and a chain modeled on t01r02_floor whose output ignores
texture 0. assets/test_bch_model.c: a fixed float24 color with a prior slot
upload, absent colors, and rejection of exponent 127, slot 12, an unuploaded
slot and partial masks. assets/test_bch_material.c: sampled/projected/disabled
classification and each rejection class of the full-chain interpreter.
render/test_raster.c: shader inputs, clamping, alpha test after shading, no
shading of depth-failed fragments, and the exposed sampler.
tests/test_bch_combiner_viewer.py: exact predicted colors through constants,
scale, buffer delay and fragment secondary; projected and disabled units with
sensitivity counts; failures before video; the unchanged stage-0 default.
All pass under ASan/UBSan with LeakSanitizer, including the sanitized viewer
suites, a real-input combiner run and the bridge.

Remaining

Fragment lighting (light sets live outside this model), the vertex shader and
its primary color, projected textures with a camera, fog, culling, primitive
state, the game camera and depth mapping; then scene integration with the
terrain inspector. Player entry, movement, interaction, battle, return to field
and save/load remain first-playable work.
