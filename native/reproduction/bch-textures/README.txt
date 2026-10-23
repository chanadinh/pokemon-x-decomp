Native BCH area texture continuation, 2026-10-23

This milestone connects checked BCH texture packs to the native asset viewer.
It contains independently authored C/Python source, synthetic tests and metadata
evidence. No game payload, bitmap, reference binary or external source is tracked.

The three preceding stages were pushed separately:
e9abe1f -- BCH index and reusable GR section extraction
ab4ea72 -- PICA packet reader with explicit known register bytes
285a6eb -- Static BCH model, external texture names and independent evidence

Native path

make -C native all test test-runtime
native/build/pokemon_asset_viewer --romfs /path/to/romfs \
  --archive a/0/1/3 --entry 75 --section 11 --list-textures
native/build/pokemon_asset_viewer --romfs /path/to/romfs \
  --archive a/0/1/3 --entry 75 --section 11 --texture 9

--section selects an AD/GR minipack after optional outer LZ10/11 decoding.
The complete offset table is checked, including unselected sections. Optional
inner LZ10/11 decoding is bounded; owned input/decompression buffers on this
section-selection path share 64 MiB. BCH metadata is indexed without allocating
pixels; selected images use the existing shared ETC1/ETC1A4 codec.

Accepted BCH: compatibility 7/7, texture-only resource dictionaries, checked
nonoverlapping sections, zero uninitialized-data lengths, relocation tags
0/1/2/0x24, bounded names with matching dictionary/object strings, all three
texture command streams agreeing on dimensions/format/payload. Overlapping
relocation source fields, out-of-range register IDs, incomplete packets and invalid
payload extents reject the pack. Other descriptors remain listable but cannot
decode. Validated images have exact flags 0x10c or 0x10d and power-of-two dimensions
8..4096. Cubemaps, model-bearing packs and mip chains remain unsupported.

Budgets: 64 MiB input; 65,535 total resources; 262,144 relocations; 1,024-byte names;
4,096 words per command stream; 4,194,304 aggregate command words. Output pixels
are owned separately from the borrowed index/input. Native Windows has not been
built; tests execute Linux binaries under WSL and SDL's dummy video backend.

Independent real-data check

The selected area contains 19 images and 100,608 pixels. The complete exported
RGBA data matches AOSP's independent ETC1 RGB decoder plus separately authored
A4-nibble and 8x8-tile reconstruction. All 19 matches and hashes are recorded in
reference-comparison.json. A three-frame software capture and input self-test
also pass on the selected original image, with the SDL dummy backend.

First produce local BCH inputs with ../bch-model/verify_area_textures.py.
Build the external AOSP reference exactly as described by
../texture-formats/reference-comparison.json, verifying its source/header hashes.
No third-party code is copied into this repository. Then run:

python3 native/reproduction/bch-textures/verify_pixels.py \
  --romfs /path/to/romfs --texture-bch /path/to/area75-world-textures.bch \
  --viewer native/build/pokemon_asset_viewer \
  --reference-library /path/to/libaosp_etc1.so \
  --output-dir outputs/new-bch-pixel-check

The output directory must be new. It contains local-only BMPs and a metadata
report. Keep all original-asset-derived files under ignored outputs/. The script
validates the selected BCH/payload hashes and refuses a different baseline.

Synthetic and regression checks

The C suite checks immutable input, owned output, exact pixels/alpha, both
formats, zero-relative pointers, truncation, section/relocation/command bounds,
unsupported descriptors and bad dimensions. It also passes ASan/UBSan.
Six black-box tests exercise the actual native binary across standalone BCH,
AD/GR wrappers, both compression layers, selection/listing, invalid inputs,
exclusive output and symlink preservation, dummy-video capture and injected input.
They also preserve raw BCLIM sections whose pixels resemble an LZ header.
The unchanged texture/model/field viewer tests run in the same target.
Full results and counts are recorded in validation.json and validation.txt.

Remaining: native environment mesh/material rendering, projected coordinates,
original area consumer, primitive state and player/gameplay integration. Nineteen
decoded images do not establish a playable map or running-game equivalence.
The primitive-state clarification is recorded in primitive-state-evidence.json;
mask-8 post-draw writes clear cache and must not be used to infer triangle mode.
