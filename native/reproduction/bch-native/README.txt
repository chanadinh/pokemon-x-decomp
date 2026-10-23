Native BCH geometry and original area-consumer continuation, 2026-10-23

This milestone adds a native C static-model decoder and an explicit triangle-list
wireframe path in the existing model viewer. Original executable review separately
corroborates the selected zone-to-area archive/member/section-consumer chain.
Tracked files contain independently authored source, synthetic tests and metadata.
No original executable, instruction listing, BCH, geometry arrays or capture is
included. Such inputs and derived outputs remain in ignored local directories.

Native preview

make -C native all test test-runtime
native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/0/4/1 --entry 165 --section 1 --triangle-list --inspect
native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/0/4/1 --entry 165 --section 1 --triangle-list

Arrows orbit, +/- zoom, R/Home reset, Escape exits. Add --software --frames 3
--self-test-input --capture-frame new.bmp for a bounded runtime check. Existing
files, source files and symlink targets are not overwritten. --inspect initializes
no video. Linux binaries under WSL and SDL's dummy backend were tested; a native
Windows build has not been verified.

The --triangle-list choice is mandatory for BCH and rejected for CGFX/demo.
It groups each submesh's raw indices into triples after checking divisibility
and any observed draw-time mode bits. There are 1,664 interpreted triples in the
selected sample. This does not establish the original GPU topology. The decoder
preserves arbitrary index counts independently of the viewer's interpretation.
BCH --textured is rejected; all meshes are shown regardless of visibility.
Materials, visibility, priority and layers are metadata rather than executed state.

Reader contract

Compatibility 7/7; checked nonoverlapping sections and relocation provenance;
identity model transform; no skinning, skeleton or submesh culling; one interleaved
vertex buffer; stored position/UV0; complete float32 uniforms 6..8; finite ordinary
attributes; expected byte masks and relocated raw addresses. Other fixed attributes
are identified but not executed. Material parameter/name references are checked,
and all three nullable external texture-unit names are preserved byte-for-byte.

All output geometry and names are owned independently of the unchanged input.
Per-mesh indices are rebased into the combined vertex array. Attribute arithmetic
uses binary64 intermediates and one final binary32 rounding. Draw-time primitive
state is preserved separately from subsequent cache-clear writes. At most one
full-mask value-1 indexed draw is accepted per submesh; address/count changes
after it, non-indexed draws and draws outside submesh streams are rejected.

Budgets: 64 MiB BCH input; 65,535 aggregate indexed resources; 262,144 relocations;
1,024 meshes/materials/nodes and submeshes per mesh; 262,144 vertices; 786,432
indices; 65,536 words per command stream and 262,144 aggregate command words;
1,024-byte names; absolute numeric magnitudes at most 1e12. When --section selects
an AD/GR minipack, owned member and optional outer/inner decompression buffers
share a 64 MiB budget, and the complete section-offset table is validated.

Independent original-input comparison

First extract the exact local inputs using ../bch-model/verify_area_textures.py.
Then build the small validation bridge and run:

cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror \
  native/assets/bch_model.c native/reproduction/bch-native/dump_model.c \
  -lm -o native/build/dump_bch_model
python3 native/reproduction/bch-native/verify_geometry.py \
  --file /path/to/field165.bch --bridge native/build/dump_bch_model \
  --viewer native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --output-dir outputs/new-native-geometry-check

The directory must be new. The bridge releases the input before reporting native
owned arrays through a pipe. The verifier checks the pinned BCH and reference
script hashes, then invokes the previously independent owned-copy relocation
oracle; it does not use the production Python parser. Every position and UV0
component is compared after explicitly rounding the reference to binary32;
every index is compared after undoing the native per-mesh rebase. All twelve
meshes, 2,356 vertices and 4,992 indices match exactly. All material names/configs
and sixteen nonnull texture names also match the earlier metadata record.

The same script loads the original archive through the native viewer, renders
three software frames, captures the first and exercises input. The 960x640 capture contains 30,551
wire pixels. This checks native display and the loading path, not running-game
camera/material/topology equivalence. The script writes only metadata to the
comparison report; the capture remains local. reference-comparison.json records
source/payload/pixel hashes and per-mesh comparison results without arrays.

Tests and original-code evidence

The C suite checks exact asymmetric geometry and UVs, both index widths, signed
integer attributes, immutable input, output ownership, relative-zero pointers,
nullable names, truncation, budgets, malformed relocation/packet/numeric inputs,
and draw-time versus trailing primitive state. It passes ASan and UBSan.
Nine black-box tests cover the actual viewer, both compression formats and both
AD/GR layers, whole-table rejection, distinct rendered meshes/index rebasing,
primitive state, exclusive output, window input and pre-video failure paths.
The complete native core and existing texture/model/field runtime targets pass;
validation.json and validation.txt record counts, commands and results.

primitive-area-notes.txt explains original area selection, section consumers,
material-coordinate observations and the remaining primitive boundary. The JSON
records retain exact input hashes, function/field offsets and verification counts.
The original area evidence was committed separately as 8b6dac5. All 539 earlier
reconstruction checkpoint files still match their existing SHA-256 records.

Remaining

Connect original primitive setup to the selected draw, establish downstream
material texture binding, implement projected coordinates/combiners/render state,
and integrate the environment with the field scene. Player entry, movement,
interaction, battle, return to field and save/load remain first-playable work.
