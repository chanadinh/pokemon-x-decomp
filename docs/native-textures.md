# Native BCLIM decoder

`native/assets/bclim.c` independently implements a bounded subset of the 3DS
BCLIM texture format. It accepts a complete, decompressed file and returns owned,
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
| 12, L4 | Low nibble first; opaque alpha |
| 13, A4 | Low nibble first; zero RGB |

ETC1/ETC1A4 (10/11), unknown format flags, alternate footer versions, and the
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

## Validation

`make -C native test-core` runs fixed color vectors for all supported formats,
the asymmetric orientation matrix, cropping/stride cases, and malformed or
unsupported input cases. Tests include truncated buffers, mismatched lengths,
dimension limits, unknown flags, and output cleanup. The suite also passes a
strict C11 build with AddressSanitizer and UndefinedBehaviorSanitizer.

Local validation used user-provided RomFS archives. These selections are useful
for reproducing the read-only asset path, but their bytes and generated previews
remain under ignored `outputs/native-foundation/`:

| Archive | Entry / subentry | Compression | Texture | Observed preview |
| --- | --- | --- | --- | --- |
| `a/1/1/8` | 1 / 0 | None | 36×30 RGBA8 | Black and light-blue checkerboard |
| `a/1/3/8` | 10 / 0 | None | 512×128 RGBA4 | Green rounded interface bar |
| `a/1/6/0` | 0 / 0 | LZ11 | 40×40 RGBA4 | Pink cream-filled pastry icon |

The compressed example expands from 987 bytes to an 8232-byte BCLIM file. Its
decoded RGBA bytes have SHA-256
`481ae7a97121b365040f61a0eb2b12d13fc50439ddd6cd8fee251ca72b0a71d0`.
This is an asset inspection foundation; it does not reconstruct the game's
texture selection, layout, animation, or rendering behavior.
