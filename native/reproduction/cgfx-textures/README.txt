CGFX image textures and static geometry preview
==============================================

native/assets/texture.h/.c expose a container-independent, exact-length payload
decoder. Formats use PICA/CGFX numbering. BCLIM retains its public API and format
IDs, translating them explicitly before calling this decoder. CGFX passes the
resolved image descriptor's payload directly; no BCLIM footer is synthesized.

The shared decoder preserves the previous raw/ETC arithmetic, channels, Morton
and block order, and top-down RGBA convention. Its stored dimensions must be
multiples of eight, cover the positive logical dimensions, and be at most 4096.
It checks exact payload length and allocates only the cropped logical output.
BCLIM alone retains its RGBA8 tile-rounded alternative and format-7 palette
marker rejection. CGFX RGBA5551 has no BCLIM palette-marker interpretation.

Bounded C CGFX adapter
---------------------
px_cgfx_open borrows an immutable input buffer and allocates an index. The
caller keeps the input alive until px_cgfx_close. Texture names are borrowed
NUL-terminated byte strings with explicit lengths, not guaranteed UTF-8. The
DICT key and TXOB name are separately exposed and need not match. Decoded RGBA
has independent ownership via px_texture_free and survives index closure.

The reader accepts little-endian CGFX revision 0x05000000, a 20-byte header,
DATA followed by optional IMAG, and exact section/file extents. Input is capped
at 64 MiB and total dictionary entries at 65,536. All sixteen dictionary slots,
their contiguous records, names (at most 1024 bytes), and object prefixes are
validated inside DATA. Signed 32-bit relative pointers are resolved using a
wider signed intermediate. Geometry and other resource bodies are not parsed.

An image TXOB has type 0x20000011 and revision 0x05000000. Its field at +0x38
is a self-relative pointer to a separate 32-byte image descriptor; adjacency is
not required. The descriptor contains stored height/width at +0/+4, payload
length at +8, self-relative payload pointer at +12, and bits-per-pixel at +20.
Every selected or indexed image payload must be nonempty and wholly inside the
IMAG payload, excluding its block header. Structural pointer/extent errors
reject opening the container.

Pixel interpretation supports one mip, known hardware formats 0..13, matching
bits-per-pixel and exact stored-surface length. Logical dimensions must fit the
stored dimensions, and stored dimensions must be powers of two from 8 to 4096.
The stored width determines the tile-row stride; output crops the logical
rectangle. Nonzero runtime texture/location/allocator/address fields are outside
this static-file subset. GL format/type fields are not used to interpret pixels;
the explicit PICA hardware format determines the decoder.

Unknown texture types/revisions, mip chains, runtime state, and unsupported
storage layouts remain listable with a per-entry unsupported decode status.
Dimension-limit and invalid bpp/length metadata are also reported per entry.
No image descriptor is followed for unknown TXOB types/revisions. Selecting an
unsupported entry leaves the output zeroed. Compressed member handling belongs
to the existing bounded archive/LZ layer, not this container reader.

Primary sources and evidence
----------------------------
1. EveryFileExplorer, pinned commit f9f00d193c9608d71c9a23d9f3ab7e752f4ada2a:
   https://github.com/Gericom/EveryFileExplorer/blob/f9f00d193c9608d71c9a23d9f3ab7e752f4ada2a/3DS/NintendoWare/GFX/TXOB.cs
   SHA-256: 6fed4ca803cb4234d3093baa074f9408c7853d3f779edc5288c6b1a7aecf4561
   ImageTextureCtr and PixelBasedImageCtr explicitly resolve the descriptor and
   payload relative pointers. This provides evidence beyond the prior importer,
   which skips the descriptor pointer and reads adjacent descriptor fields.

2. The same commit's format enum, bpp table, and row/tile conversion:
   https://github.com/Gericom/EveryFileExplorer/blob/f9f00d193c9608d71c9a23d9f3ab7e752f4ada2a/3DS/GPU/Textures.cs
   SHA-256: 8f4f044a6c6034a56ef7f470f1b5dd3bace853317ebd0a8cabb6d586ad9b3960
   Format numbers differ from BCLIM. Raw row placement does not add a vertical
   flip. Existing Khronos/Azahar ETC evidence remains in texture-formats/README.txt.

3. Ohana3DS-Rebirth CGFX importer, pinned commit:
   https://github.com/gdkchan/Ohana3DS-Rebirth/blob/c50635e82763fad9d1bcc30a905523a1b92d7d1f/Ohana3DS%20Rebirth/Ohana/Models/CGFX.cs
   Confirms DATA category and DICT record structure and the TXOB base fields.

The C implementation is independently written from these format facts. The
known local TXOBs have adjacent descriptors, so synthetic forward/backward and
relocated descriptor fixtures exercise the pointer rule independently. Tests
also compare direct payload colors, selection, named entries, crops, format
mapping, unsupported metadata, malformed offsets, and allocation boundaries.
Independent real-image comparison covers raw and ETC embedded textures using
a separate parser and scalar/raw or pinned AOSP reference decoder.

Static geometry and validation
------------------------------
The separate cgfx_model API extracts bounded, unskinned, identity-transform
triangle meshes. The model viewer displays positions/indices as an orthographic
wireframe with orbit, zoom, and reset controls. It does not apply materials,
UVs, visibility, animation, or the game's camera. See docs/native-geometry.md
for the supported structures and pinned Ohana/SPICA primary references.

geometry-sample.json records the six-shape local example: 767 vertices and
936 triangles, with independently matched position/index bytes. The stored
name does not establish a map, collision, or gameplay role.
reference-comparison.json records 11 embedded textures, 2,088,960 RGBA bytes
and 23,616 ETC blocks compared without mismatches. rendered-samples.json
records native window captures and original-size pixel comparisons.

validation.json and validation.txt record the completed build, regression,
runtime, and address/undefined-behavior sanitizer checks. Leak detection was
disabled because this environment restricts LeakSanitizer's ptrace operation;
these results do not assert a LeakSanitizer pass. baseline.json pins the prior
32 native files and all 539 immutable reconstruction manifest entries. The
12 earlier native reproduction files remain byte-for-byte unchanged.

Downloaded reference implementations, extracted game bytes, and previews remain
in ignored outputs directories. This milestone establishes asset inspection and
static wireframe display, not a playable scene or original rendering equivalence.
