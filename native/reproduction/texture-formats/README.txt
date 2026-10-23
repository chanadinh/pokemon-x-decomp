Native texture-format milestone: ETC1 and ETC1A4
================================================

This milestone extends native/assets/bclim.c with BCLIM formats 10 and 11.
The original foundation records describe the earlier raw-format-only decoder;
their unsupported-ETC limitation does not describe this milestone's decoder.
No source-game texture payloads or external decoder implementations are tracked.

Implementation contract
-----------------------
native/assets/etc1.h exposes px_etc1_decode_block. It consumes one PICA block:
8 bytes for ETC1, or 16 for ETC1A4. It writes exactly 64 tightly packed RGBA8
bytes in four rows, starting at the top left. Larger source/output buffers are
allowed; extra bytes are untouched. Null pointers, alpha values other than 0/1,
short input, and short output fail before any output write.

ETC1 supports individual and differential base colors, both half-block splits,
all modifier tables/selectors, and saturated RGB channels. Differential values
outside the five-bit range fail explicitly. ETC1 defines those combinations as
undefined; ETC2 assigns additional meanings which this decoder does not support.

PICA stores each ETC1 color word little-endian, unlike the standard's serialized
big-endian ETC1 word. An ETC1A4 block has a separate little-endian alpha word
before its color word. Alpha nibble index is 4*x+y within a 4x4 block; each
nibble expands by multiplication by 17. Pixel selector bits use the same
column-first index, while decoded output is row-major.

The BCLIM adapter uses row-major 8x8 tiles, each containing four 4x4 blocks in
top-left, top-right, bottom-left, bottom-right order. Stored dimensions remain
power-of-two with a minimum of eight; logical output crops the padded surface.
It validates every stored block, including padding. On any block error, it
frees the partial image and returns PX_BCLIM_UNSUPPORTED with zeroed output.
Existing footer, length, dimension, and raw-format checks remain in force.

Primary references
------------------
1. Khronos OES_compressed_ETC1_RGB8_texture specification (Ericsson format).
   Last-modified date in the document: April 24, 2008.
   Registry repository commit: 6af574a14089ccfee87efe230ebcdd8742859813
   https://github.com/KhronosGroup/OpenGL-Registry/blob/6af574a14089ccfee87efe230ebcdd8742859813/extensions/OES/OES_compressed_ETC1_RGB8_texture.txt
   Downloaded file SHA-256:
   113eabf43f24c99ab2dd821113f04b25ea77ee2159bbc3776c93728348458284
   Specifies base-color fields, signed differences, selector bit positions,
   modifier tables, channel expansion/clamping, and invalid differential ranges.

2. Azahar PICA texture implementation, texture_decode.cpp lines 161-187.
   Repository commit: 93f8939a10b0f6dd740683d3201c7ff69250580d
   https://github.com/azahar-emu/azahar/blob/93f8939a10b0f6dd740683d3201c7ff69250580d/src/video_core/texture/texture_decode.cpp
   Downloaded file SHA-256:
   0d39eb0000e916958e8c9c83799161604d4595b4784128fc9cdfb26c9de94006
   Confirms PICA word byte order, four-block tile order, and alpha placement.
   The companion ETC1 sampler was also inspected:
   https://github.com/azahar-emu/azahar/blob/93f8939a10b0f6dd740683d3201c7ff69250580d/src/video_core/texture/etc1.cpp

These references establish format facts; the C decoder is independently written.
The low-level fixed-vector tests and separate AOSP comparison are independently
maintained. Visual appearance alone is not proof of image orientation; fixed
asymmetric positions test the documented top-down output convention.

Local evidence
--------------
The existing metadata-only sample at a/0/0/5, entry 35/subentry 0, decodes as
320x240 ETC1. Its output is a gray triangular gradient background. Local preview
and decoded hash are recorded under ignored outputs/native-textures/; those
bytes are not part of this repository. asset-samples.json records this selection
and the LZ11-compressed ETC1A4 sample at a/1/5/9, entry 0/subentry 0.

reference-comparison.json records 40,000 generated decoder comparisons, all
8,448 stored blocks from the two original samples, and 192 invalid-endpoint
rejections. The independently assembled reference RGBA images match both
complete visible images. This is a format comparison, not a running-game test.

container-samples.json records four bounded CGFX selections with archive,
index, and selected-member hashes. The new Python inspector reads resource
indices and basic CMDL/TXOB metadata, without rendering a model. Its contract
and pinned field references are in docs/native-containers.md.

validation.json preserves 21 successful command records: eight reconstruction
suites, existing analysis tests, native decoder and CLI tests, seven CGFX
tests, four real container inspections, actual desktop rendering, Windows-to-
WSL launcher execution, and AddressSanitizer/UndefinedBehaviorSanitizer runs.
validation.txt summarizes the limits and portable reproduction commands.

baseline.json verifies all 539 earlier game-reconstruction manifest entries
and the original 20-file native foundation manifest against its pinned commit.
The native application is extended in place; all eight reconstruction
directories and the foundation's earlier reproduction records stay unchanged.
