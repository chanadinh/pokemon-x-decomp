# Native BCLIM decoder

`native/assets/bclim.c` independently implements a bounded subset of the 3DS
BCLIM texture format and maps its format IDs to the shared PICA payload decoder
in `native/assets/texture.c`. It accepts a complete, decompressed file and returns owned,
tightly packed RGBA8 pixels. `px_bclim_free` releases that allocation. No game
textures are included in the repository.

The decoder requires the 40-byte little-endian `CLIM`/`imag` footer, version
`0x02020000`, one image section, and exact declared file and payload lengths.
Logical dimensions must be between 1 and 4096. Storage normally rounds each
dimension up to a power of two, with a minimum of eight pixels. RGBA8 also accepts
an exact payload matching dimensions rounded up to complete 8×8 tiles.

| Format | Input interpretation |
| --- | --- |
| 0, L8 | Luminance byte; opaque alpha |
| 1, A8 | Alpha byte; zero RGB |
| 2, LA4 | High nibble luminance, low nibble alpha |
| 3, LA8 | Bytes alpha, luminance |
| 4, HILO8 / RG8 | Bytes green, red; zero blue, opaque alpha |
| 5, RGB565 | Little-endian 5/6/5-bit RGB word |
| 6, RGB8 | Bytes blue, green, red |
| 7, RGBA5551 | Little-endian 5/5/5/1-bit RGBA word |
| 8, RGBA4 | Little-endian word, most significant nibble red, then green, blue, alpha |
| 9, RGBA8 | Bytes alpha, blue, green, red |
| 10, ETC1 | Little-endian ETC1 blocks, opaque alpha |
| 11, ETC1A4 | Four-bit alpha word followed by an ETC1 color word |
| 12, L4 | Low nibble first; opaque alpha |
| 13, A4 | Low nibble first; zero RGB |

Unknown format flags, alternate footer versions, and the
Pokémon X/Y format-7 palette extension return an explicit unsupported result.
The palette marker is a leading little-endian word of 2. Because that also could
be a raw RGBA5551 first pixel, this implementation conservatively rejects either
interpretation when the marker is present.

## Layout and orientation evidence

Tiles are ordered left to right and top to bottom. Within each 8×8 tile, source
indices interleave the three low x and y bits, starting with x. Output row zero
is the top row; the decoder neither flips nor transposes the result.

This convention agrees with the Morton lookup and successive PNG row pointers
in [dnasdw/bclimtool, pinned source](https://github.com/dnasdw/bclimtool/blob/c7f99bd9e307974a539932286fee6473ba1f1536/src/bclim.cpp).
Its [header declarations](https://github.com/dnasdw/bclimtool/blob/c7f99bd9e307974a539932286fee6473ba1f1536/src/bclim.h)
also describe the footer fields. The independent test stores source luminances
0 through 63 and compares every output position against a literal asymmetric
matrix. Separate tests check tile order, logical cropping, and both RGBA8 storage
strides. These establish the implemented convention; a recognizable local
preview alone would not establish the source image's orientation.

The RGBA8 tile-rounded variant and X/Y palette marker are documented by
[kwsch/png2bclim, pinned implementation](https://github.com/kwsch/png2bclim/blob/1c7c6b59bd8d5a4cd36f9e083f9e25afb606feb4/png2bclim/BCLIM.cs).
Packed channel ordering, missing-channel values, and bit replication for 5/6-bit
channels were checked against
[Azahar's color definitions, pinned source](https://github.com/azahar-emu/azahar/blob/93f8939a10b0f6dd740683d3201c7ff69250580d/src/common/color.h).
The C decoder was written independently from these format facts.

## ETC1 and ETC1A4

The separate `native/assets/etc1.c` decoder handles one 4×4 block. BCLIM stores
four such blocks per 8×8 tile, ordered top-left, top-right, bottom-left, then
bottom-right. Tiles run left to right and top to bottom. Color words are
little-endian; ETC1A4 prefixes each color word with a little-endian alpha word.
Alpha nibble index `4*x+y` starts at the least significant nibble and expands
to eight bits by multiplication by 17.

Individual and differential modes, both split directions, the eight intensity
tables, selector sign, and channel clamping follow the
[Khronos ETC1 specification](https://registry.khronos.org/OpenGL/extensions/OES/OES_compressed_ETC1_RGB8_texture.txt).
Differential endpoints outside the 5-bit range are rejected, including in
padding blocks; ETC2 modes are not supported. BCLIM validates the exact padded
payload size and crops only after decoding. On failure it frees temporary
storage and leaves the caller's output empty.

[The milestone evidence](../native/reproduction/texture-formats/README.txt)
pins the PICA storage references and records an independent decoder comparison.
Synthetic fixtures check all selectors and intensity tables, signed deltas,
split direction, tile order, alpha placement, cropping, and malformed input.
Black-box viewer tests also verify original-size BMP pixels and alpha through
both direct and LZ11-compressed archive paths.

## Validation

`make -C native test-core` runs fixed color vectors for all supported formats,
the asymmetric orientation matrix, cropping/stride cases, and malformed or
unsupported input cases. Tests include truncated buffers, mismatched lengths,
dimension limits, unknown flags, and output cleanup. The suite also passes a
strict C11 build with AddressSanitizer and UndefinedBehaviorSanitizer.

Local validation used user-provided RomFS archives. These selections are useful
for reproducing the read-only asset path, but their bytes and generated previews
remain under ignored `outputs/native-foundation/` and `outputs/native-textures/`:

| Archive | Entry / subentry | Compression | Texture | Observed preview |
| --- | --- | --- | --- | --- |
| `a/1/1/8` | 1 / 0 | None | 36×30 RGBA8 | Black and light-blue checkerboard |
| `a/1/3/8` | 10 / 0 | None | 512×128 RGBA4 | Green rounded interface bar |
| `a/1/6/0` | 0 / 0 | LZ11 | 40×40 RGBA4 | Pink cream-filled pastry icon |
| `a/0/0/5` | 35 / 0 | None | 320×240 ETC1 | Gray triangular gradient background |
| `a/1/5/9` | 0 / 0 | LZ11 | 40×40 ETC1A4 | Pale peach decorative panel with transparency |

The `a/1/6/0` example expands from 987 bytes to an 8232-byte BCLIM file. Its
decoded RGBA bytes have SHA-256
`481ae7a97121b365040f61a0eb2b12d13fc50439ddd6cd8fee251ca72b0a71d0`.
This is an asset inspection foundation; it does not reconstruct the game's
texture selection, layout, animation, or rendering behavior.

ETC inputs and previews stay in ignored `outputs/native-textures/`. Their
selection metadata and hashes are recorded in the new milestone evidence;
the earlier foundation's reproduction records remain unchanged.
