# Native asset viewer foundation

This native C11/SDL2 application opens selected local Pokémon X archive entries
and displays supported textures. It establishes an executable, window and event
loop, monotonic timing, keyboard controls, image decoding, and bounded resource
access. It is an asset viewer; the eight reconstructed game-logic modules are
not connected to gameplay in this application yet.

A first playable still needs map/model loading, player movement and collision,
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
2. Decode a standalone BCLIM directly, or decode one LZ10/LZ11 stream when
   indicated by its header. Decompressed output has a 64 MiB budget. Streams
   with invalid references, truncated tokens, or unexplained trailing data fail.
3. Decode supported little-endian BCLIM version `0x02020000` raw, ETC1, and
   ETC1A4 formats into
   owned, tightly packed RGBA8 pixels. Dimensions are capped at 4096 per side.
   Morton tile order, channel widths, cropping, and supported storage strides
   are documented in [the texture notes](../docs/native-textures.md).
4. Upload decoded pixels into an SDL texture and display them over a checker
   background with nearest-neighbor sampling and alpha blending.

XY palette extensions, model rendering, embedded images, maps,
animation, shaders, audio, and gameplay are outside the supported path. Finding
`CLIM` near a member's end does not establish that the entire member is a
standalone texture. The decoder requires its complete size/layout contract.
Selected-file hashes and format references are recorded with the reproduction
materials. The visible sample's role in game scenes is not established.

## Inspect model containers

The Python 3 tool indexes bounded, uncompressed CGFX containers independently
of the viewer. It reports resource names and offsets, model counts, and supported
inline texture metadata without reading a whole enclosing GARC archive:

```sh
python3 native/tools/inspect_container.py \
  --archive /path/to/romfs/a/0/0/3 --entry 134
# Or inspect an already selected file:
python3 native/tools/inspect_container.py --file selected.cgfx
```

JSON is written to standard output. The checked sample contains the model name
`ball_in_1` and two texture records. Those names do not establish a gameplay or
map role. Geometry, materials, animation, and scene bindings remain to be decoded.
See [the container evidence and limits](../docs/native-containers.md).

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
Static format references, synthetic tests, and local rendering do not establish
visual equivalence with a running original game. Reference captures and a
concrete map/interaction/battle scenario remain the next integration gate.
