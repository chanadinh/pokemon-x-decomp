# Bounded CGFX resource inspection

`native/tools/inspect_container.py` indexes the resources in a little-endian
CGFX file, either standalone or selected from a GARC v4 archive. It reports
stored names, member-relative offsets, resource categories, basic CMDL counts,
and image TXOB texture metadata. It does not render models or establish which
resources form a playable map. See [the native playable-slice plan](PLAYABLE_SLICE.md)
for the larger reconstruction sequence.

```sh
python3 native/tools/inspect_container.py --file /path/to/selected.bcres
python3 native/tools/inspect_container.py \
  --archive "$ROMFS/a/0/0/3" --entry 134 --subentry 0
python3 -m unittest discover -s native/tests -p test_container.py -v
```

The tool writes JSON to standard output and errors to standard error. It does
not write to either input or export any payload. Archive input uses the existing
bounded GARC index reader, then reads only the selected member. The archive path
is explicit; no local private path is embedded in the tool. Compressed members,
BCH, and other containers return an unsupported-input error.

## Format evidence and supported layout

The implementation was checked against the original
[Ohana3DS CGFX importer, pinned at c50635e](https://github.com/gdkchan/Ohana3DS-Rebirth/blob/c50635e82763fad9d1bcc30a905523a1b92d7d1f/Ohana3DS%20Rebirth/Ohana/Models/CGFX.cs)
and local bytes. Its separate
[texture-format enum](https://github.com/gdkchan/Ohana3DS-Rebirth/blob/c50635e82763fad9d1bcc30a905523a1b92d7d1f/Ohana3DS%20Rebirth/Ohana/RenderBase.cs)
supplies CGFX texture names. This is an independently written metadata reader;
the original importer is evidence for offsets and meanings, not a claim that
all of its broader format support has been reproduced.

| Structure | Fields used |
| --- | --- |
| CGFX | Magic, little-endian marker, 20-byte header, revision, exact file length, section count |
| Sections | DATA first, optional IMAG second; each length includes its eight-byte block header |
| DATA | Sixteen pairs of count and relative DICT pointer starting after the block header |
| DICT | Magic, length, count, sixteen-byte root node, then sixteen-byte entry records |
| DICT entry | Name pointer at `+8`, object pointer at `+12`; both relative to their own pointer field |
| CMDL object | Flags, magic at `+4`, name pointer at `+12`; object/material/shape/object-node counts at `+0xb4/+0xbc/+0xc4/+0xcc` |
| Inline TXOB object | Type flags `0x20000011`, magic at `+4`, name pointer at `+12`; height/width at `+0x18/+0x1c`, mip levels at `+0x28`, format at `+0x34` |
| TXOB image descriptor pointer | Relative pointer at TXOB `+0x38`; the 32-byte descriptor may be elsewhere in DATA |
| Image descriptor | Stored height/width at `+0/+4`, byte count at `+8`, relative data pointer at `+12`, bits per pixel at `+20` |

The initial inspector assumed the descriptor immediately followed the pointer,
which holds for its four recorded samples. The active reader now follows the
pointer, as documented by
[EveryFileExplorer's ImageTextureCtr](https://github.com/Gericom/EveryFileExplorer/blob/f9f00d193c9608d71c9a23d9f3ab7e752f4ada2a/3DS/NintendoWare/GFX/TXOB.cs).
A relocated, backwards descriptor fixture exercises this correction. Earlier
sample/evidence records remain unchanged.

Relative offsets use signed 32-bit displacements, including valid backwards
pointers. The dictionary root and radix-tree links are not traversed: entries
are enumerated from their bounded contiguous records. Names include both a
UTF-8 display string and exact hexadecimal bytes, preserving non-UTF-8 input in
the report. Category labels follow the importer; slot 15 remains explicitly
`slot_15`. Local slot-15 objects have `PEMT` magic, but their bodies are not parsed.

CGFX format numbers differ from BCLIM's numbering. For example, CGFX `4` means
RGBA4, `12` means ETC1, and `13` means ETC1A4. A CGFX TXOB is not a standalone
BCLIM image. Passing its payload to the BCLIM footer parser is not a supported
way to display it. Width, height, format, and mip count are reported as declared
metadata; this tool does not validate their pixel-storage arithmetic or decode
the texture. The native C adapter now separately validates a bounded single-mip
subset and decodes its payload directly; see
[the implementation contract](../native/reproduction/cgfx-textures/README.txt).
Other TXOB type flags are reported as unsupported variants without
following their image fields.

## Limits and validation

Input or selected member size is capped at 64 MiB; the reused GARC reader caps
its allocation index at 8 MiB. Resource count is capped at 65,536 and each name
at 1,024 bytes. Section extents must account for the exact selected input.
Dictionary extents, counts, names, and object headers must fit inside DATA.
Parsed image payload spans must fit wholly inside IMAG. Truncation, null required
pointers, invalid relative offsets, count/length disagreement, unsupported block
layouts, and invalid archive selections produce errors without tracebacks.

The synthetic tests construct an independent fixed layout with a backwards
name pointer, CMDL counts, an ETC1 image TXOB, and GARC selection. They exercise
field values, exact offsets, malformed sizes/counts/pointers/magic, truncated
input, unterminated names, explicit unknown texture variants, and clean CLI
failures. They require Python only, with no game data or SDL.

Four bounded local selections successfully exercised the reader:

| Archive / entry / subentry | CGFX bytes | Indexed observations |
| --- | ---: | --- |
| `a/0/0/1 / 0 / 0` | 1,829 | Two light entries and one scene entry |
| `a/0/0/3 / 134 / 0` | 53,248 | CMDL `ball_in_1`; RGBA4 TXOBs `ball_2l` (64×64) and `ball_1` (32×32) |
| `a/0/3/7 / 0 / 0` | 41,216 | CMDL `asobitai`; RGBA8 `k_heart_o` and RGBA4 `pop`, both 64×64 |
| `a/1/1/6 / 1 / 0` | 381,824 | CMDL `encount_panelB_05`; RGB565 `tile_test` (512×256) |

All four have CGFX revision `0x05000000`. These names are stored labels, not
proof of a location or gameplay role. The portable
[container sample metadata](../native/reproduction/texture-formats/container-samples.json)
records their selected-member, archive-index, and archive SHA-256 hashes.
Ignored local `container-*.json` reports under `outputs/native-textures` record
all parsed offsets. Each selected archive hashed in full is under 1 MiB.
No full read of the large model archive was needed.

The same milestone found a standalone ETC1A4 BCLIM at `a/1/5/9 / 0 / 0`:
LZ11 expands 704 bytes into a 4,136-byte, 40×40 BCLIM. Its compressed and decoded
hashes, selected archive hash, footer, and bounded-search accounting are in the
ignored `sample-etc1a4.json` and `etc1a4-probe.json` under that output directory.
This is a separate texture-decoder sample, not a CGFX resource. See
[native texture notes](native-textures.md) for decoding support.

## Next evidence boundary

The native texture viewer now exposes selected image TXOB images using their
hardware format numbers and validated storage dimensions. A separate
[wireframe model viewer](native-geometry.md) decodes a restricted static mesh
subset. The next rendering boundary is material/texture bindings, UVs, PICA
state, and scene transforms with a known camera convention. Map selection and traversal need
additional evidence connecting those resources to scene placement, collision,
spawn, and transition data. Current resource names and archive numbering do not
establish those connections. All earlier reconstruction checkpoints remain
unchanged.
