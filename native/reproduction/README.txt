Native archive and texture path: reproduction notes

Scope

The C asset readers are new host tools. They are not reconstructions of the
original game's resource manager and do not establish gameplay equivalence.
They read local user-provided assets; no game payload is included in Git.

GARC v4 allocation layouts were checked against the original pk3DS source and
the local archive bytes. LZ10/LZ11 token layouts were independently implemented
and checked against original nlzss and pk3DS implementations. References:

https://github.com/kwsch/pk3DS/blob/6daaca934ca2284a73ab743bf89c848c57cd9de1/pk3DS.Core/CTR/GARC.cs
https://github.com/kwsch/pk3DS/blob/6daaca934ca2284a73ab743bf89c848c57cd9de1/pk3DS.Core/CTR/LZSS.cs
https://github.com/magical/nlzss/blob/f27414f373eab53bfe3c1a819c40eb800323e690/lzss3.py

No code from those tools is required at runtime. The local discovery report
records checksums of the fetched reference files. The texture decoder has its
own documented format scope and references in the native README.

Reproduce the bounded archive survey

Run from the repository root with a C11 compiler and Python 3.9 or newer.
On this local workstation, tools/native-build/activate.sh selects the installed
ignored compiler. Other machines may use their normal system compiler.

  mkdir -p native/build
  cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror \
    native/assets/garc.c native/tools/check_garc.c -o native/build/check_garc

Set ROMFS_ROOT to the local extracted RomFS directory. The audit used:
  /mnt/d/Downloads/PokemonX-modules-20261023-104349-3eee2a4b/work/romfs

Use a fresh output directory outside that input tree. The script refuses an
existing output directory and any output path inside the original RomFS.

  python3 native/tools/survey_assets.py \
    --romfs "$ROMFS_ROOT" \
    --output outputs/native-foundation/reproduction-new \
    --validate-native native/build/check_garc \
    --sample a/1/1/8:1:0 \
    --sample a/1/3/8:10:0 \
    --sample a/1/6/0:0:0 \
    --sample a/0/0/5:35:0

The script inventories allocation indexes, four-byte member prefixes, and
forty-byte member suffixes. It copies only the explicitly selected members
and hashes their archives only when those archives are at most 64 MiB.
The optional check_garc executable uses the production C parser to validate
every surveyed archive index without reading its payload members.

Observed local survey:
  271 archives; 44,512 members; zero Python index-validation errors.
  Independent C index validation: 271 archives; 44,512 members; zero failures.
  174 members end with CLIM at the expected footer position.
  Only 32 of those declare a BCLIM file size equal to the complete member.

A CLIM suffix alone does not identify a standalone image. For example,
a/0/0/5 entry 0 has a 386,856-byte member ending with a footer that declares
a 2,088-byte image. Its enclosing container has not been implemented here;
the reader rejects it as a standalone BCLIM instead of guessing an offset.

Selected directly accessible members:
  a/1/1/8 entry 1 subentry 0: raw BCLIM, RGBA8 format 9, 36 x 30.
  a/1/3/8 entry 10 subentry 0: raw BCLIM, RGBA4 format 8, 512 x 128.
  a/1/6/0 entry 0 subentry 0: LZ11, 987 bytes -> 8,232 bytes;
    decoded BCLIM RGBA4 format 8, 40 x 40.
  a/0/0/5 entry 35 subentry 0: raw BCLIM, ETC1 format 10, 320 x 240;
    this is an explicitly unsupported texture sample, not a display target.

Use native/build/pokemon_asset_viewer --romfs "$ROMFS_ROOT" --archive a/1/6/0
--entry 0 --subentry 0 --inspect to exercise GARC -> LZ11 -> BCLIM. Add the
viewer display/export options documented in native/README.md for inspection.
Do not infer a map, battle, or UI role solely from a numbered path or appearance.

Limits and failure behavior

GARC accepts little-endian version 0x0400, four sections, and packed FATO/FATB
allocation records. Version 6, big-endian data, record aliases/gaps/overlaps,
inconsistent file/section sizes, and out-of-range member extents are rejected.
Indexes are limited to 8 MiB and 262,144 members. The file is seeked rather than
loaded whole. On hosts with a limited C long seek range, offsets outside that
range are rejected. The library borrows FILE streams only when open_file is
used; open(path) owns its stream. The caller must not mutate a stream/archive
concurrently with reads. Truncated reads and allocation failures return errors.

Each selected member read has a caller-provided byte budget. LZ decode has a
separate caller-provided output budget checked before allocation. LZ10 and LZ11
are supported, including extended LZ11 sizes and overlapping backreferences.
Empty streams, truncated tokens, backward distances before output, tokens
exceeding the declared size, and unsupported compression types are rejected.
At most three trailing zero bytes are permitted. Other wrappers and executable
overlay compression are not implemented. These bounds do not decode every
asset or establish complete container integrity.

Validation

The GARC tests build a synthetic archive with sparse subentry bits, assert
exact selected bytes and ownership, and reject malformed tables, aliasing,
out-of-range lengths, wrong counts, and every truncated prefix. LZ tests use
fixed byte streams and known outputs for literal, short/medium/long references,
overlap, maximum token length, extended headers, size limits, and corruption.
They do not generate expected output with a matching encoder.

  make -C native test-core

Selected local archive/member/decompressed hashes and complete footer fields
are recorded in the milestone's metadata evidence. Screenshots and decoded
game payloads remain in ignored outputs directories.

SDL platform references

The window, software-renderer fallback, borrowed RGBA surface, monotonic
counter, injected events, and framebuffer capture use SDL2's documented APIs:
https://wiki.libsdl.org/SDL2/SDL_CreateRenderer
https://wiki.libsdl.org/SDL2/SDL_CreateRGBSurfaceWithFormatFrom
https://wiki.libsdl.org/SDL2/SDL_GetPerformanceCounter
https://wiki.libsdl.org/SDL2/SDL_PushEvent
https://wiki.libsdl.org/SDL2/SDL_RenderReadPixels

The local Windows launcher invokes the Linux executable through WSL; it is
not a native Windows ABI build. Its bounded launch was exercised as part of
the recorded validation. Source/build and asset locations remain explicit.
