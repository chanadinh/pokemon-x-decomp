# Native texture, model, and field viewers

This native C11/SDL2 application opens selected local Pokémon X archive entries
and displays supported textures. It establishes an executable, window and event
loop, monotonic timing, keyboard controls, image decoding, and bounded resource
access. A separate executable previews a restricted subset of static CGFX mesh
geometry as a wireframe or an opt-in unlit textured preview, and the selected
BCH field as a wireframe or a TEV-stage-0 textured preview. The eight reconstructed game-logic modules are
not connected to gameplay in this application yet. A third executable loads
checked XY zone, matrix and region records for terrain/collision inspection.

A first playable still needs environment rendering, player entry/movement and collision,
interactions, an interactive battle, return to the map, and save/load. See the
[first-playable acceptance criteria](../docs/PLAYABLE_SLICE.md). This directory
contains independently authored host/platform code, rather than an additional
ARM game-function reconstruction milestone. All eight earlier checkpoints stay
unchanged.

## Build and run

Install a C11 compiler, Make, pkg-config, and SDL2 development files. On Ubuntu,
the packages are `build-essential`, `pkg-config`, and `libsdl2-dev`.

```sh
make -C native
native/build/pokemon_asset_viewer --demo
```

The demo is a generated test image and requires no game data. Arrow keys pan,
`+`/`-` zoom, `R` or Home resets the view, and Escape closes the window. Small
images initially enlarge up to 8× with nearest-neighbor sampling; large images
fit the window. Window resizing is supported.

For the checked local game-data baseline, this entry is a standalone LZ11
compressed RGBA4 texture:

```sh
native/build/pokemon_asset_viewer \
  --romfs /path/to/extracted/romfs --archive a/1/6/0 --entry 0
```

A raw RGBA8 sample is `a/1/1/8`, entry `1`. An ETC1 texture is `a/0/0/5`,
entry `35`; an LZ11-compressed ETC1A4 texture is `a/1/5/9`, entry `0`.
These indices describe the checked
baseline, not a portable mapping for every game revision. No assets ship with
the viewer. The asset root and member selection are always explicit; no local
private paths are compiled into it.

On the current WSL workspace, repository-local verified dependencies can be
activated with `source tools/native-sdl/activate.sh` before the build/run commands.
The ignored setup uses the compiler already documented by the state milestone,
plus the packages recorded in `reproduction/local-sdl-toolchain.json`. A standard
SDL2 installation needs no such activation. `SDL_CFLAGS` and `SDL_LIBS` can
select an alternative installation. Native Windows execution has not been
validated in this milestone.

## Inspection and image exports

```sh
# Decode without creating a window; export original-size pixels.
native/build/pokemon_asset_viewer --romfs /path/to/romfs \
  --archive a/1/6/0 --entry 0 --inspect --screenshot decoded.bmp

# Exercise the actual renderer, capture its first frame, then exit.
native/build/pokemon_asset_viewer --romfs /path/to/romfs \
  --archive a/1/6/0 --entry 0 --frames 3 --capture-frame viewport.bmp
```

`--screenshot` exports the decoded image, including alpha, at its original
size. `--capture-frame` captures the displayed viewport before presentation.
Both exports refuse to overwrite existing files, including input archives.
Use a fresh output filename for each export. `--inspect` does not initialize
the video driver and cannot be combined with window/input options.

`--subentry` selects the original GARC bitmap bit, defaulting to zero.
For CGFX or supported BCH members, `--texture N` selects an embedded image, defaulting to zero.
`--list-textures` prints names, dimensions, hardware format IDs, and supported
decode status without opening a window or decoding pixels. It cannot be combined
with texture selection, exports, or window options. Both options require CGFX or supported BCH;
ordinary BCLIM selections retain their existing behavior. For example:

```sh
native/build/pokemon_asset_viewer --romfs /path/to/romfs \
  --archive a/0/3/7 --entry 0 --list-textures
native/build/pokemon_asset_viewer --romfs /path/to/romfs \
  --archive a/0/3/7 --entry 0 --texture 0
```

`--frames` requires a positive count. `--software` forces the SDL software
renderer; otherwise the application tries accelerated rendering and falls back
to software. `--self-test-input` injects pan, zoom, reset, resize, and Escape
through the same event handler, checks their effects, restores the view, and
renders three frames unless another count was supplied.

## Supported asset path

1. Read the GARC v4 allocation tables with an 8 MiB index budget and at most
   262,144 members. The archive stays on disk; the application reads only the
   selected member, capped at 64 MiB. Original entry/subentry identities are
   retained. Unsupported layouts and invalid bounds fail with diagnostics.
2. Decode a standalone BCLIM or CGFX directly, or decode one LZ10/LZ11 stream when
   indicated by its header. Decompressed output has a 64 MiB budget. Streams
   with invalid references, truncated tokens, or unexplained trailing data fail.
3. Decode supported little-endian BCLIM version `0x02020000` raw, ETC1, and
   ETC1A4 formats into
   owned, tightly packed RGBA8 pixels. Dimensions are capped at 4096 per side.
   Morton tile order, channel widths, cropping, and supported storage strides
   are documented in [the texture notes](../docs/native-textures.md).
4. For CGFX revision `0x05000000`, select a bounded image TXOB, resolve
   its image descriptor, validate format/bpp/storage and single-mip payload,
   then decode through the shared PICA payload API. CGFX format IDs differ
   from BCLIM IDs; no synthetic container header is created. Unsupported mip
   chains and texture variants remain listable but cannot be displayed.
5. Upload decoded pixels into an SDL texture and display them over a checker
   background with nearest-neighbor sampling and alpha blending.

XY palette extensions, model materials, maps, animation, shaders, audio, and
gameplay are outside the texture path. Finding
`CLIM` near a member's end does not establish that the entire member is a
standalone texture. The decoder requires its complete size/layout contract.
Selected-file hashes and format references are recorded with the reproduction
materials. The visible sample's role in game scenes is not established.

## Inspect model containers

The Python 3 tool indexes bounded, uncompressed CGFX containers independently
of the viewer. It reports resource names and offsets, model counts, and supported
image texture metadata without reading a whole enclosing GARC archive:

```sh
python3 native/tools/inspect_container.py \
  --archive /path/to/romfs/a/0/0/3 --entry 134
# Or inspect an already selected file:
python3 native/tools/inspect_container.py --file selected.cgfx
```

JSON is written to standard output. The checked sample contains the model name
`ball_in_1` and two texture records. Those names do not establish a gameplay or
map role. This metadata inspector does not decode material or scene bindings.
See [the container evidence and limits](../docs/native-containers.md).

## Preview static geometry

```sh
native/build/pokemon_model_viewer --demo
native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/1/0/2 --entry 0 --model 0
```

Arrows orbit; `+`/`-` zoom; `R`/Home resets; Escape exits. The preview centers
and fits all decoded triangles using an orthographic camera. It draws all edges,
including hidden edges, and all accepted shapes regardless of visibility.
Materials, texture bindings, lighting, culling, and animation are not reproduced.
The default camera is an inspection choice, not a recovered game camera.

`--inspect` prints mesh counts and bounds without a window. `--capture-frame`
exports a new BMP viewport file; existing files are preserved. `--frames`,
`--software`, and `--self-test-input` support bounded smoke tests. Geometry
loading accepts a restricted unskinned SOBJ subset with identity model
transforms and triangle lists; unsupported layouts fail explicitly. See
[the geometry contract and evidence](../docs/native-geometry.md).

## Preview a textured model

```sh
native/build/pokemon_model_viewer --demo --textured
native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/1/0/2 --entry 0 --textured
native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/1/0/2 --entry 0 --textured --inspect
```

`--textured` resolves supported material references, UV0 and vertex colors,
then decodes the referenced embedded images. The software rasterizer applies
checked wrapping/filtering, alpha test, depth test/write, culling, and supported
blend factors. The same orbit/zoom/reset controls and exclusive frame captures
work in both modes. `--inspect --textured` validates and reports the bindings
without opening a window.

The preview uses texture multiplied by vertex color and an orthographic
inspection camera. Lighting, fog, texture combiners, shader programs, material
animation, and the game's draw ordering are not reproduced. The preview orders
material translucency layers and priorities ascending, retaining resource order
for ties; it does not sort triangles by distance. Supported material
UV transforms are currently identity only. This is a bounded visual inspection
tool, not a complete CGFX or PICA renderer. See the
[material contract and evidence](../docs/native-materials.md) for the accepted
states and the preview's ordering and coordinate conventions.

## Inspect a field layout

```sh
native/build/pokemon_field_viewer --demo
native/build/pokemon_field_viewer --romfs /path/to/romfs --zone 5 --inspect
native/build/pokemon_field_viewer --romfs /path/to/romfs --zone 5 --collision
```

The checked chain is zone 5 → matrix 12 → region 165. The viewer displays raw
terrain cells and optional collision edges, with a geometric height readout.
Arrows/WASD move an inspection marker only onto exact standard cells; `F`
switches focus/full layout, `C` toggles edges, `+`/`-` zoom, `R`/Home resets,
and Escape exits. Initial marker and camera placement are inspector choices.
`--steps RDLU` makes inspection movement reproducible. Exclusive captures and
bounded frame/input checks work as in the other viewers.

This does not implement player movement, transitions or full collision rules.
The field's BCH environment model is previewed separately in the model viewer
(wireframe or textured, below); it is not yet placed in this inspector. The parser,
placement evidence, limits and policy are documented in [the field notes](../docs/native-field.md).

## Tests and evidence

```sh
make -C native test          # C asset + Python container tests; no SDL/game files.
make -C native test-runtime  # Viewer CLI, pixel exports, and dummy-video input.
make -C reconstructed test  # The eight immutable game-logic suites.
python3 -m unittest discover -s tests -v
```

The tests include fixed color and asymmetric tile vectors, ETC individual and
differential modes, transparency and cropping, malformed/truncated
archives and streams, subentry handling, allocation budgets, and an independent
synthetic GARC → LZ11 → BCLIM → BMP check. Export tests verify pixel positions
and refusal to overwrite inputs. Synthetic inputs are generated during tests;
no game asset bytes are committed. The repository CI runs these checks using
SDL's dummy video backend; actual desktop rendering is a separate local check.

`reproduction/validation.json` preserves the foundation's commands and results.
The ETC milestone's additional validation and pinned references are in
[`reproduction/texture-formats/`](reproduction/texture-formats/README.txt).
Embedded-texture and wireframe validation is recorded separately in
[`reproduction/cgfx-textures/`](reproduction/cgfx-textures/README.txt).
Material, UV, and software-renderer validation is in
[`reproduction/textured-model/`](reproduction/textured-model/README.txt).
Field resource selection, terrain/collision decoding and marker-policy validation
are recorded in [`reproduction/field-scene/`](reproduction/field-scene/README.txt).
Static format references, synthetic tests, and local rendering do not establish
visual equivalence with a running original game. Reference captures and a
concrete map/interaction/battle scenario remain the next integration gate.

## Inspect the selected field BCH index

```sh
python3 native/tools/inspect_bch.py --file selected.gr --gr-section 1
# A standalone BCH can be inspected without --gr-section.
```

This read-only Python tool accepts compatibility 7/7 and reports section bounds,
resource names and section-relative object addresses as file offsets. It checks
all fifteen dictionary extents, child indices, name termination and aggregate
resource budgets. It does not validate relocation commands, object bodies,
GPU commands, texture bindings or geometry. It does not render BCH.
The checked region 165 contains model `t01r0202_00_00`, twelve materials and
zero embedded textures. The model continuation below checks external names
against area 75; resource names do not establish a location or player spawn.
See [BCH index evidence](reproduction/bch-index/README.txt).

## View BCH area textures

```sh
native/build/pokemon_asset_viewer --romfs /path/to/romfs \
  --archive a/0/1/3 --entry 75 --section 11 --list-textures
native/build/pokemon_asset_viewer --romfs /path/to/romfs \
  --archive a/0/1/3 --entry 75 --section 11 --texture 9
```

`--section N` selects an AD or GR minipack section after optional archive-member
decompression. The entire section table is checked, and one additional LZ10/11
stream may wrap the selected section. All owned input/decompression buffers in
this path share a 64 MiB budget. Selection is explicit; it does not execute the
game's area loader. Omit `--section` for a standalone BCH archive member.

The native BCH texture reader accepts a bounded compatibility 7/7 texture-only
pack with checked resource names, relocations and all three unit command streams.
The validated images are single-level ETC1/ETC1A4, power-of-two dimensions from
8 through 4096, with descriptor flags `0x10c`/`0x10d`. Unsupported descriptors
remain listable; decoding refuses them. Model-bearing BCH files, mip chains,
cubemaps and other versions are outside this native texture reader's subset.

All nineteen textures from the selected area now decode and export through the
native application. Their complete RGBA images match an independent ETC1 RGB
reference plus separately implemented alpha/tile checks. Existing pan/zoom,
exclusive BMP exports, frame captures and input smoke tests apply. This displays
individual textures; the environment geometry and projected material effects
are still not rendered. See [the reproduction record](reproduction/bch-textures/README.txt).

## Decode the selected BCH model

```sh
python3 native/tools/inspect_bch_model.py --file selected.gr --gr-section 1
python3 native/tools/inspect_bch_model.py --file selected.gr --gr-section 1 \
  --texture-bch area75-world-textures.bch --triangle-list-obj new-model.obj
```

This Python tool decodes a restricted static model's positions, UV0 and indices,
reports per-mesh bounds/hashes and material texture names, and optionally checks
those names against an external BCH resource index. The checked field has 12
meshes, 2,356 stored vertices and 4,992 indices. All sixteen nonnull material
texture references match area 75's nineteen textures. Four materials also
reference a projected texture; their effects remain unimplemented.

The optional OBJ is an explicit triangle-list interpretation (1,664 triples in
this sample), with UV0 and all accepted meshes. Export refuses existing files
and symlinks. The recorded primitive writes do not establish the required GPU
state, so this is not proof of the game's draw topology or appearance. Material
shaders, projected texture coordinates, lighting and visibility execution remain
pending. See [the geometry contract](../docs/native-bch.md)
and [reproducible checks](reproduction/bch-model/README.txt).

## View BCH geometry in the native application

```sh
native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/0/4/1 --entry 165 --section 1 --triangle-list --inspect
native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/0/4/1 --entry 165 --section 1 --triangle-list
```

This loads the selected field directly from GARC through optional LZ decoding
and GR section 1 into the native C geometry reader. It displays all twelve meshes
as a wireframe with orbit/zoom/reset controls. `--capture-frame new.bmp` creates
an exclusive viewport capture; `--software --frames 3 --self-test-input` provides
a bounded runtime check. `--inspect` opens no window and reports mesh/material
associations and observed primitive state. For a standalone BCH archive member,
omit `--section`.

The mandatory `--triangle-list` flag makes the index interpretation explicit.
It is rejected for CGFX and demo input. Original primitive mode is still unknown
for this sample. All meshes are displayed regardless of stored visibility.
Native Windows has not been tested; validation uses
Linux under WSL and SDL's dummy video backend. See the [reproduction record](reproduction/bch-native/README.txt).

## Texture the BCH field

```sh
native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/0/4/1 --entry 165 --section 1 --triangle-list --textured \
  --texture-archive a/0/1/3 --texture-entry 75 --texture-section 11
```

BCH `--textured` needs an explicit external texture source: the
`--texture-archive`/`--texture-entry` pair, with optional `--texture-subentry`
and `--texture-section`, loads a BCH texture pack through the same bounded
GARC/LZ/AD-GR path (its own 64 MiB budget). Each material's texture unit is the
one its TEV stage 0 names as source; that unit's name is then matched by exact
bytes, and a missing or duplicated name fails. For field 165 all twelve
materials resolve; the four projection-mapped materials select their unit-1
base texture, and their unit-0 `projection_dummy` is reported as unexecuted.

The preview is unlit and runs stage 0 only, with white primary/lighting colors.
It applies each material's sampler wrap/filter, alpha test, depth test/write,
color mask and blend (replace, source-alpha or additive source-alpha) from its
command registers. Culling is reported but not executed; meshes draw by layer,
priority and resource order. Combiner stages 1–5, the projected texture,
fragment lighting, fog, vertex colors and the original primitive state remain
unimplemented, so this is not the game's final appearance. `--inspect` prints
every binding without a window. See the
[textured-preview record](reproduction/bch-textured/README.txt).

## Run the full BCH combiner chain

Add `--combiners` to the textured command to evaluate all six TEV stages of each
material (constants, scales, the combiner buffer and every texture unit the chain
reads) instead of stage 0 only. Vertex colors become the primary color: stored
color attributes after their scale, or fixed float24 attributes. Inputs that need
unexecuted GPU work use declared values that `--inspect` prints: fragment lighting
is white/black, projection-mapped units read a white texel, and disabled units read
zero. After the first rendered frame the viewer prints, per material, how many
shaded fragments would change if the projected substitute or the fragment
secondary color took its other extreme. For field 165 both counts are zero; every
chain still multiplies by the unexecuted lighting color. See the
[combiner record](reproduction/bch-combiners/README.txt).

## Light the BCH field

Add the environment options to the `--combiners` command to execute directional
fragment lighting:

```sh
native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/0/4/1 --entry 165 --section 1 --triangle-list --textured --combiners \
  --texture-archive a/0/1/3 --texture-entry 75 --texture-section 11 \
  --environment-archive a/0/0/3 --environment-entry 137 --environment-section 0 \
  --lighting day
```

`--environment-archive`/`--environment-entry` (with optional
`--environment-subentry`, `--environment-section` and `--scene`) load a BCH
environment through the same bounded path; `--section` options also accept BM
minipacks. Each material's light set is resolved by index. Its enabled
directional lights are evaluated per fragment, with the model's normals
interpolated and renormalized; the light vector is the negated direction and
each light color is multiplied by the material color, both as the original code
does. Point and spot lights are not executed: `--inspect` prints their per-channel
bound, and the sensitivity line reports how much the final color could change.
`--lighting day` (the default) omits `light_night`; `night` omits `light_sun`.
Materials without fragment lighting, Fresnel/bump use, ambient lights, missing
normals or light sets fail before a window opens. See the
[lighting record](reproduction/bch-lighting/README.txt).

## Walk the lit field

`--walk` (BCH from a GR `--section`, textured) also decodes that GR's terrain grid
and collision and draws an inspection marker on the collision floor. WASD moves it
one cell, and `--steps UDLR...` applies moves before display or `--inspect`. Only
exact standard cells (`0x00000020`) admit it, as in the field inspector. The camera
starts looking down, because culling is not executed. `--inspect` prints the
marker's cell, local position and collision height. See the
[walk record](reproduction/bch-walk/README.txt).

## Draw the player character

`--player-archive`/`--player-entry` (and optional `--player-section`) add a skinned
BCH at the `--walk` marker in place of the box. The model uses its own textures,
the room's environment and the same shading mode, and turns toward each step.
`--skinned` decodes a model with a skeleton by itself and draws its rest pose:

```sh
native/build/pokemon_model_viewer --romfs /path/to/romfs --archive a/0/2/1 --entry 2 --section 0 \
  --skinned --triangle-list --textured --combiners \
  --texture-archive a/0/2/1 --texture-entry 2 --texture-section 0
```

Skeletons, bone palettes and bone attributes are decoded. The rest pose follows the
pinned vertex-shader translation. A BCH that also holds models can serve as its own
texture pack. Without a motion the model holds its T-pose. In walk
mode the camera looks from above and follows the marker. See the
[player record](reproduction/bch-player/README.txt).

## Animate the player character

`--motion-section N --motion N` pose a skinned model (with `--skinned`, or the
player with `--player-archive`) from the GF1 motion pack in that MM section of its
entry. The motion loops at one frame per two rendered frames; `--motion-frame N`
picks the start frame. In walk mode, `--walk-motion N` plays that slot once per
step while the player glides between cells:

```sh
native/build/pokemon_model_viewer --romfs /path/to/romfs --archive a/0/2/1 --entry 2 --section 0 \
  --skinned --triangle-list --textured --combiners \
  --texture-archive a/0/2/1 --texture-entry 2 --texture-section 0 --motion-section 1 --motion 4
```

For the player, slot 1 is a standing pose, slot 3 a walk cycle and slot 4 a run,
identified by rendering. World-space feet use SPICA's conversion, and the glasses
follow a head locator by an explicit rule. See the
[motion record](reproduction/bch-motion/README.txt).

## Play a zone

`--zone N` builds the scene from game data: the zone header's area and matrix, the
matrix's regions for the zone, the area's textures, the regions' props, the zone's
warps and NPCs and the NPC registry. The player walks in real time (WASD, Shift runs), Space talks to the NPC
ahead, and warps load their target zone:

```sh
native/build/pokemon_model_viewer --romfs /path/to/romfs --zone 5
```

Zone 5 is the bedroom; its stairs lead to zone 4, whose front door leads out into
Vaniville Town (zone 0). The controller timing and arrival rule are inspection
choices. `--arrive`, `--steps`,
`--play` and `--capture-at` support repeatable runs. See the
[zone record](reproduction/bch-zone/README.txt).

Space in front of an NPC runs the zone's own script (a Pawn 3.3 image) for that
NPC and shows the story-text lines it picks in a message window; Space turns the
pages. Five native calls are given inferred meanings (message, player gender,
work values, two waits) for a new game; see the
[dialogue record](reproduction/bch-talk/README.txt).

Towns and routes span several map regions: the matrix's sub-cell zone table names
the zone's cells, whose regions load at their offsets with one joined terrain grid;
NPC and warp tiles are world tiles. Region props (doors, lamps, furniture) come
from the area's prop registry. BCH scenes draw in row bands on several cores
(`--threads N`; same pixels as one pass); `--no-props` gives the earlier scenes.
See the [town record](reproduction/bch-town/README.txt).

Walking into a map cell of another zone switches to that zone and keeps the
player's world position (Vaniville Town → Route 1 → Aquacorde Town); scenes
include a ring of neighbouring cells. `--silly giant` and `--silly moonwalk` are
for fun. See the [route record](reproduction/bch-route/README.txt).

Tall grass starts wild encounters from the zone's table (Route 2, `--zone 259`);
the battle uses the game's species, move, learnset and type data, stats from the
reconstructed routines, and published damage formulas (`--partner`,
`--partner-level`, `--seed`, `--encounter-rate`, `--code-bin`). See the
[battle record](reproduction/bch-battle/README.txt).

Wins give experience from the game's growth tables (`a/2/1/7`); the level comes
from the reconstructed level-from-experience routine and new moves from the
learnset. BALL throws a Poké Ball (`--balls N`, default 10) and a catch joins a
party of up to six. See the [catch record](reproduction/bch-catch/README.txt).

With `a/0/0/7` in the RomFS the battle draws both Pokémon's own skinned models
(game textures, the field's lighting) instead of placeholders, playing their own
idle, attack, damage and faint motions; see the
[battle model record](reproduction/bch-pokemon/README.txt) and the
[battle motion record](reproduction/bch-battle-motion/README.txt).

