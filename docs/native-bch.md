# Static BCH field model inspection

The selected field's BCH now has a bounded geometry decoder and an optional OBJ
export. This is a Python inspection tool; the SDL field viewer still displays
terrain and collision. No game assets are included in the repository.

```sh
python3 native/tools/inspect_bch_model.py --file selected.gr --gr-section 1
python3 native/tools/inspect_bch_model.py --file field165.bch \
  --texture-bch area75-world-textures.bch --triangle-list-obj new-model.obj
```

Both commands print metadata JSON. Offsets are relative to the selected BCH,
including when it came from a GR section. `--model` selects a resource index
(default zero). The optional OBJ exclusively creates a new file, includes all
accepted meshes with UV0, and uses numeric object names to keep stored resource
strings out of OBJ syntax. It does not apply visibility, materials or lighting.

## Checked sample and comparison

Zone 5 selects matrix 12/region 165 and area 75 in the existing field evidence.
The region's GR section 1 contains model `t01r0202_00_00`, with twelve meshes,
2,356 stored vertices and 4,992 raw indices. Vertices are read in buffer order
through the largest referenced index; this count is not a count of unique
positions. Repeated positions and indices are retained.

An independent, separately authored reference relocates an owned copy before
decoding. Its positions, UV0 and indices match the production resolver on all
twelve meshes, as do bounds and strides. Canonical hashes use little-endian
float64 triples/pairs and uint32 indices. Arithmetic uses Python binary64 after
reading the stored float32 values. This checks parsing against another
implementation; it does not establish original shader arithmetic or a running
game comparison. Reproduction scripts, input hashes and metadata are in
[`native/reproduction/bch-model/`](../native/reproduction/bch-model/README.txt).

## Accepted format and bounds

The decoder accepts BCH compatibility 7/7 with nonoverlapping section bounds
checked by the existing resource inspector. It keeps input bytes unchanged.
Relocations are indexed by source field, with duplicate/overlapping sources,
unsupported tags and out-of-section offsets rejected. Every dereferenced model,
material, string, command and raw-buffer pointer must carry the expected
relocation. A zero field without relocation can be null; a relocated zero
offset can refer to the first byte of a section.

The supported model has an identity transform, zero model flags, no skeleton
or submesh culling, normal unskinned meshes, and valid node/material references.
Material records provide all three nullable texture-unit names and enable bits.
Parameter records, layer ranges, metadata and shader code are not executed.
Only their consumed pointers/extents are checked; this is not full BCH validation.

PICA packets have bounded lengths, register numbers, consecutive writes and
eight-byte packet padding. Register masks affect bytes, and unwritten bits
remain unknown. Complete float32 vertex uniforms 6–8 supply position offsets
and attribute scales. Float24 uniform uploads and partial uniform writes reject.
Raw buffer addresses require a complete command write and the corresponding
vertex or 8/16-bit index relocation.

Geometry uses one interleaved buffer with checked attribute permutations,
strides and naturally aligned i8/u8/i16/f32 values. Position must have three
stored components and UV0 two. Other ordinary attributes are bounded and checked
for finite values but not exported. Fixed position/UV0, skinning attributes,
additional vertex buffers and ambiguous alignment reject. Other fixed attributes
are only identified; their float24 values are not decoded or used by the export.
All decoded numeric components must be finite and at most `1e12` in magnitude.

Budgets are 64 MiB per input, 1,024 meshes/materials/nodes, 262,144 vertices,
786,432 indices, 262,144 aggregate command words and relocation entries,
65,536 words per command stream, and 1,024 bytes per resource name. Indices and
vertex extents are checked inside raw data. Unsupported inputs fail with an error.

## Topology boundary

The selected submesh streams write primitive register `0x25e` with parameter
zero and byte mask 8, after their draw command. Those writes do not establish
bits 8–9, which encode the primitive mode. The inspector reports the encoded
parameter and masks; it does not supply invented inherited GPU state.

The continuation checked the same trailing sequence against
[Citro3D's indexed draw implementation](https://github.com/devkitPro/citro3d/blob/9f21cf7b380ce6f9e01a0420f19f0763e5443ca7/source/drawElements.c).
It identifies the two mask-8 writes as post-vertex-cache clearing. Primitive
selection is a separate mask-2 write before the draw; indexed triangles can also
use geometry-primitive mode with geometry-stage configuration. This supports
keeping topology unknown here, and prevents treating the post-draw zero as
triangle-mode evidence. The original game's corresponding setup is still needed.

`--triangle-list-obj` explicitly interprets consecutive index triples as faces,
matching the inspection convention used by the reviewed format implementations.
The selected sample yields 1,664 triples. This is a useful inspection export,
with topology still awaiting confirmation from the original draw setup or a
reference capture. The tool rejects nonzero primitive hints or counts that
cannot form triples within this restricted export path.

## External textures and next work

Area 75 comes from `a/0/1/3`, entry 75/subentry 0. Its decompressed AD minipack
has twelve sections; section 11 is a BCH with nineteen textures. All sixteen
nonnull references across the model's twelve materials match exact raw names in
that dictionary. Material names are not used as texture guesses. Zero pointers
are distinguished from valid string offset zero through relocation records.

Four materials reference `projection_dummy` in unit 0 and their ordinary image
in unit 1. The image payloads use ETC1/ETC1A4 already supported by the native
decoder, but projected coordinates, samplers, combiners, lights and draw ordering
still require implementation. `--texture-bch` verifies dictionary name matches
only, not texture pixels or the correctness of a chosen area's runtime binding.

The original-code continuation now corroborates the area selection and section
consumer. Zone accessor `0x004a36cc` reads section 0's unsigned 16-bit area ID at
offset 2. Transition `0x003b843c` narrows it to a byte, which is stored in the area
state. Setup `0x003e1680` creates archive index 13 (`a/0/1/3`), and continuation
`0x003e1dd4` requests the member selected by that area byte. Consumer `0x003e0530`
decompresses it and passes AD sections 11 and 1 into separate BCH resource holders.
The selected value 75 survives the narrowing; this is not a claim about all u16 IDs.

All 1,348 exported instructions across thirteen functions were checked against
the pinned executable bytes. The [area-consumer evidence](../native/reproduction/bch-native/primitive-area-notes.txt)
records addresses, hashes, dataflow and reproduction instructions. Later exact
texture-name binding to field materials, primitive setup and material execution
remain open. Player entry, movement, interaction, battle, return to field and
save/load remain first-playable requirements.

Format references are the reviewed
[SPICA revision bd29a78](https://github.com/gdkchan/SPICA/tree/bd29a7828595d7839cda2ac61c76bb63f9071250)
and [CTRMap revision 4b780a7](https://github.com/Rynbo/CTRMap/tree/4b780a7bc32a75a7f31c162829b086355caf32f2).
The new tools are independently authored; neither external source nor original
game payload bytes are included.

## Native area texture continuation

The native asset viewer now loads the selected area directly through
GARC → LZ11 → AD section 11 → BCH → ETC1/ETC1A4. Use
`--archive a/0/1/3 --entry 75 --section 11 --list-textures` or select an image with
`--texture N`. All nineteen images, totaling 100,608 pixels, match the independent
RGB reference and separate alpha/tile reconstruction. The texture path does not
depend on the unresolved primitive mode.

The C reader validates the texture-only BCH subset, all three unit command
streams, expected relocation kinds and complete payload extents. It shares the
existing texture codec. Native environment geometry, projected coordinates and
material execution remain pending. See [the viewer instructions](../native/README.md#view-bch-area-textures)
and [new validation](../native/reproduction/bch-textures/README.txt).
