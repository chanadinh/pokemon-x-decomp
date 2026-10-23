Native BCH textured field preview, 2026-10-23

This milestone renders the selected field model (a/0/4/1 entry 165, GR section 1)
with area 75's world textures (a/0/1/3 entry 75, AD section 11) in the existing
model viewer. Each material's texture is chosen by that material's own TEV
stage-0 combiner commands, then resolved by exact name bytes in the area pack.
Tracked files contain independently authored source, synthetic tests and
metadata only. No BCH payload, texture, pixel array or capture is included.

Native preview

make -C native all test test-runtime
native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/0/4/1 --entry 165 --section 1 --triangle-list --textured \
  --texture-archive a/0/1/3 --texture-entry 75 --texture-section 11

Add --inspect to print every binding without video, or --software --frames 3
--self-test-input --capture-frame new.bmp for a bounded runtime check. Arrows
orbit, +/- zoom, R/Home reset, Escape exits. --triangle-list stays mandatory:
the original draw topology is still not connected to this field.

What the preview executes

New native metadata (assets/bch_model.h) returns, per material, the texture
unit parameter registers 0x83/0x93/0x9b, the three texture coordinators at
parameter+12+24*unit, and the fragment command stream at parameter+0xc8:
registers 0x40, 0x100, 0x101, 0x104, 0x107, 0xe0 and all six TEV stages, each
with a bit-level known mask. The stream is read only when +0xc8 carries a
command-section relocation (tag 2); the older four-byte synthetic parameter
objects remain valid geometry inputs without this metadata.

assets/bch_material.c interprets a deliberately small subset:
- TEV stage 0 only. Its RGB and alpha source 0 must name the same texture unit;
  replace or modulate by primary/fragment-primary color, plain operands, 1x
  scale. Primary and lighting colors are white: the preview is unlit.
- The named unit must be enabled in 0x80, have a texture name, a 2D sampler
  type, ordinary UV mapping (mode 0) and an identity coordinator [1,1,0,0,0].
  Only UV0 is stored in these meshes, so every sampled unit reads UV0.
- Sampler wrap/filter from the unit parameter register; clamp-to-border is
  rejected; the viewer additionally rejects distinct min/mag filters.
- Blend mode (not logic op) with the add equation and matching RGB/alpha
  factors: one/zero, source-alpha/one-minus-source-alpha, or source-alpha/one.
  The software rasterizer gained the last mode (PX_RASTER_ADD_SOURCE_ALPHA).
- Alpha test, depth test/function/write and color mask from 0x104/0x107.
  Depth functions are applied in the preview's lower-z-is-nearer convention;
  PICA depth-map registers are not executed.
- Culling register 0x40 is reported (all twelve materials hold 2) but not
  executed; every triangle is drawn regardless of winding.
- Draw order: ascending mesh layer, then priority, then resource order. This
  is a preview policy, not recovered game sorting.
Anything outside the subset fails before video. Stages 1..5, the projected
unit-0 texture, fragment lighting, fog and vertex colors are not executed.

Field 165 result

All twelve materials fit the subset. Eight sample unit 0. The four materials
with projection mapping on unit 0 (t01r02_carpet2, t01r02_floor, t01r02_wall,
t01r02_woody) name texture 1 as the stage-0 source; their projection_dummy
unit is reported as unexecuted_units=0x1. This replaces the earlier open
question about unit selection with the material's own combiner evidence; it
does not establish what the original projected term contributes (stage 4
modulates by texture 0 in those materials). Two layer-1 materials blend:
gym06_kage01 uses source alpha and t01r02_mirror adds source alpha; both
disable depth writes. Twelve of the area's nineteen textures are bound.

Independent comparison

Build the bridge, then run the verifier with the exact extracted payloads
(see ../bch-model/verify_area_textures.py for extraction):

cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror native/assets/bch_model.c \
  native/assets/bch_material.c native/assets/bch_texture.c native/assets/texture.c \
  native/assets/etc1.c native/reproduction/bch-textured/dump_materials.c \
  -lm -o native/build/dump_bch_materials
python3 -I native/reproduction/bch-textured/verify_materials.py \
  --field /path/to/field165.bch --textures /path/to/area75-world-textures.bch \
  --bridge native/build/dump_bch_materials \
  --viewer native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --ctrmap /path/to/CTRMap-checkout --output-dir outputs/new-textured-check

The verifier pins both input hashes and does not import native/tools. It
parses sections, relocations and command packets itself and re-derives every
register observation and preview field from the pinned CTRMap layouts
(H3DMaterial, PICACommandReader, PICACommand; hashes in the report). It then
checks each bound texture's exact name, its earlier external index in
../bch-model/field165-model.json, and its decoded RGBA hash against the 19
images already verified pixel-for-pixel in ../bch-textures. Finally it runs the
viewer, captures a software frame and counts covered pixels whose RGB is an
exact texel of a bound image (expected under unlit replace/nearest sampling).

Recorded result (reference-comparison.json): 12 materials, 432 register
observations, 12 coordinator triples and samplers, 12 bindings and pixel hashes
all match. The 960x640 capture has 75,538 covered pixels; 75,422 (99.85%) are
exact texels, the remainder being blended shadow/mirror pixels and similar.

Input path caveat: this session could read the two extracted BCH payloads but
not the user's RomFS directory. The recorded runtime capture therefore used
single-member GARC wrappers around the exact hash-pinned payloads, passed via
--model-args/--texture-args. The archive, LZ and AD/GR section paths are
covered by the synthetic runtime tests and by the earlier geometry/texture
milestones, not by this capture. Running the command above with the default
arguments against the real RomFS exercises the complete original path.

Tests

assets/test_bch_material.c covers ordinary and projected-unit materials,
every blend mode and each rejection class with asymmetric register values.
assets/test_bch_model.c adds a full parameter object (coordinators, partial
sampler masks, fragment stream) plus nonfinite, draw-in-stream, tag-3 and
empty-stream cases; the legacy fixture still decodes without metadata.
render/test_raster.c checks additive source-alpha saturation exactly.
tests/test_bch_textured_viewer.py runs the actual viewer: exact-texel capture,
stage-0 unit 1 selection with an unresolvable unit-0 name, LZ11 + AD/GR texture
sections, and pre-video failures for missing, case-different and ambiguous
names, unsupported combiners, mapping, samplers, filters, logic ops and blends.
All of these also pass under ASan/UBSan; the C fixtures and the real-input
viewer run additionally pass with LeakSanitizer enabled.

Remaining

Execute the full combiner chain (stages 1..5, constants, buffer), fragment
lighting and the projected texture; connect the original primitive state and
culling convention; establish camera/projection and depth-map conventions;
integrate the environment into the field scene alongside the terrain
inspector. Player entry, movement, interaction, battle, return to field and
save/load remain first-playable work.
