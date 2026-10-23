# Static CGFX geometry preview

The native model viewer can extract and display a bounded subset of static
CGFX meshes as wireframes. The verified local sample is `a/1/0/2`, entry `0`,
subentry `0`, model `0`. Its stored name is `collision02`; that label does not
establish a collision, map, or gameplay role.

This page describes the positions-only API and wireframe mode. The additive
`--textured` mode and its separate material/UV contract are documented in
[the material preview notes](native-materials.md).

```sh
native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/1/0/2 --entry 0 --model 0 --inspect
native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/1/0/2 --entry 0 --model 0
```

The viewer shows geometry and provides orbit/zoom controls. It does not apply
materials, texture coordinates, lighting, animation, draw visibility, or the
game's camera. See the [native runtime instructions](../native/README.md) for
controls, frame capture, build, and test commands; see the
[playable-slice plan](PLAYABLE_SLICE.md) for the broader objective.

## Verified sample and evidence

The selected 442,880-byte CGFX contains six standard shapes, six object
references, six materials, and six textures. The model has no skeleton flag,
its submeshes have no bone references, and its scale/rotation/translation and
both matrices are identity transforms. The six vertex buffers contain float
positions, normals, and UV coordinates. Five also contain byte colors; the
remaining shape has a separate fixed float color attribute.

| Shape | Vertex count | Byte stride | Index count | Index element |
| --- | ---: | ---: | ---: | --- |
| 0 | 91 | 36 | 450 | unsigned byte |
| 1 | 108 | 36 | 360 | unsigned byte |
| 2 | 8 | 32 | 18 | unsigned byte |
| 3 | 310 | 36 | 900 | unsigned short |
| 4 | 64 | 36 | 180 | unsigned byte |
| 5 | 186 | 36 | 900 | unsigned byte |

Together these produce 767 vertices and 2,808 indices, or 936 triangles. Their
position bounds are approximately `(-499.450012, -0.0000305, -496.713989)` to
`(499.450012, 1637.369995, 496.713989)`. A separate sample-specific Python check
unpacked the float triples and byte/short indices from independently specified
stream offsets, then compared every position and rebased index byte with the
C extractor. Both hashes matched.

The tracked [geometry sample metadata](../native/reproduction/cgfx-textures/geometry-sample.json)
records those hashes, input/archive-index/archive hashes, exact stream and
index offsets, per-shape bounds, and pinned source references. It contains no
vertex arrays, index arrays, textures, or game payloads. The selected archive
is 29,057,724 bytes; only this bounded archive was read in full for its hash.
The discovery scan used archive indexes and bounded member metadata, reading
1,491,888 member bytes across 2,860 CGFX headers including the selected copy.
Its detailed local report is ignored under `outputs/native-cgfx`.

The earlier small `ball_in_1` and `asobitai` samples use SOBJ type `0x10000002`,
whose ordinary mesh fields are empty. They are explicitly unsupported here.
The `encount_panelB_05` sample has skeleton/node references and is also
unsupported. No geometry is guessed for these variants.

## Source basis and binary layout

The original [Ohana CGFX importer at c50635e](https://github.com/gdkchan/Ohana3DS-Rebirth/blob/c50635e82763fad9d1bcc30a905523a1b92d7d1f/Ohana3DS%20Rebirth/Ohana/Models/CGFX.cs)
provides the model, shape, stream, index, and position conversion layout. The
independent original [SPICA mesh classes at bd29a78](https://github.com/gdkchan/SPICA/tree/bd29a7828595d7839cda2ac61c76bb63f9071250/SPICA/Formats/CtrGfx/Model/Mesh)
confirm type discriminators, fixed attributes, submesh/face lists, and the
descriptor's primitive byte. Its
[primitive-mode enum](https://github.com/gdkchan/SPICA/blob/bd29a7828595d7839cda2ac61c76bb63f9071250/SPICA/PICA/Commands/PICAPrimitiveMode.cs)
identifies mode zero as triangle lists. The code in this repository is an
independent bounded implementation; no external implementation is embedded.

| Structure | Fields followed by this implementation |
| --- | --- |
| CGFX | Revision `0x05000000`, little-endian 20-byte header, exact size, DATA then IMAG |
| Model DICT | Count and self-relative pointer at DATA slot 0; 28-byte header/root and 16-byte entries |
| CMDL | Revision `0x09000000`; object count/table at `+0xb4/+0xb8`, material count at `+0xbc`, shape count/table at `+0xc4/+0xc8` |
| Object SOBJ | Type `0x01000000`, revision 0; shape index `+24`, material index `+28`, self-relative owner pointer `+32` |
| Shape SOBJ | Type `0x10000001`, revision 0; position offset `+32`, submesh count/table `+44/+48`, vertex-buffer count/table `+56/+60`, optional blend descriptor `+64` |
| Interleaved buffer | Type `0x40000002`; payload size/pointer `+20/+24`, stride `+36`, attribute count/table `+40/+44` |
| Attribute | Type `0x40000001`; semantic `+4`, GL type `+36`, components `+40`, float scale `+44`, byte offset within stride `+48` |
| Fixed buffer | Type `0x80000000`; semantic `+4`, GL type `+12`, components `+16`, scale `+20`, vector count/pointer `+24/+28` |
| Submesh | Bone count/table `+0/+4`, skinning mode `+8`, face count/table `+12/+16` |
| Face | Descriptor count/table `+0/+4` |
| Index descriptor | GL type `+0`, primitive mode byte `+4`, payload byte count/pointer `+8/+12` |

Every pointer is relative to its own field, including backwards references.
Metadata must fit within DATA and vertex/index payloads within IMAG. Position
conversion is `raw_component * attribute_scale + shape_position_offset`.
Shape-local indices are rebased by that shape's vertex offset when flattening
the result. The extractor preserves stored winding and triangle order.

## Public contract and deliberate boundaries

`px_cgfx_model_decode(data, size, model_index, out)` returns owned XYZ float
triples, owned `uint32_t` triangle indices, counts, bounds, and the stored model
name. `px_cgfx_model_free` releases and clears the result. Failures clear the
output and return an explicit result code; the caller must pass a fresh or
previously freed output object. Source bytes are borrowed and must stay
immutable during the call.

The supported subset requires exactly one object reference per shape, no
skeletons or submesh bone references, identity model transforms, standard
shapes, one interleaved position stream per shape, and triangle lists. Extra
fixed nonposition float attributes are checked and ignored. Empty default
blend descriptors are accepted; active blend data is unsupported. Position
components may be signed byte, unsigned byte, signed short, or binary32 float.
Index elements may be unsigned byte or unsigned short. All shape geometry is
included regardless of object/descriptor visibility.

Unsupported SOBJ, buffer, and attribute types, revisions, transforms, topology,
bone data, fixed positions,
separate attribute streams, duplicate object-to-shape mappings, unsupported
attribute semantics, and invalid attribute widths/offsets are rejected. This
is not a full renderer: material bodies and render state are not parsed.
Unused interleaved attribute descriptors are validated, but only position
components are decoded; no claim is made about the validity of unused normal,
UV, or color values.

Limits are 64 MiB input, 1,024 shapes, 65,536 traversed table records,
1,048,576 vertices, and 3,145,728 indices. Names are bounded to 1,024 bytes.
Position values, scales, translations, and supported fixed vectors must be
finite; output coordinates are capped at absolute `1e12` to bound preview
arithmetic. No index may reference outside its own shape's vertex buffer.
The implementation validates once before allocation and again while filling
the bounded output, with cleanup on every failure path.

The independent C fixture uses two shapes, signed-short scaling/translation,
float positions, byte and short indices, backwards pointers, and a fixed color
attribute. It checks complete expected XYZ/index arrays and bounds, index
rebasing, all truncated input lengths, malformed offsets/counts/revisions,
unsupported types, skinning/topology, nonfinite numbers, limits, and both byte
component signedness paths. Separate runtime tests use another layout to
exercise raw/LZ11 GARC loading and the model-viewer CLI.

The additive textured-model preview connects this geometry to a bounded
subset of material texture references, UVs, sampler state, and transparency.
Full material behavior and game draw ordering remain unverified.
Skeletons and nonidentity transforms require their own
evidence and tests. Scene placement, collision behavior, and gameplay remain
separate boundaries; a successful wireframe does not establish any of them.
