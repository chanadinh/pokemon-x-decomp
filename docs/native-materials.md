# Unlit textured CGFX preview

The native model viewer now has an explicit `--textured` mode for the verified
static CGFX sample `a/1/0/2`, entry `0`, model `0`. It connects model objects to
materials and textures, decodes UV0 and vertex colors, and rasterizes an unlit
texture-times-vertex-color preview. The default remains wireframe.

```sh
native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/1/0/2 --entry 0 --model 0 --textured --inspect
native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/1/0/2 --entry 0 --model 0 --textured
```

This is a reconstruction checkpoint for asset display, not a reproduction of
the game's shader or scene. Lighting, fog, PICA texture combiners, material
animation, game cameras, and gameplay remain outside the preview. The stored
model name `collision02` does not establish a map or collision-system role.
The earlier [static geometry contract](native-geometry.md) still applies.

## Verified binding and state

The selected resource has 767 vertices, 936 triangles, six object draws, six
materials, and six embedded textures. Object order does not match material
order:

| Object / shape | First index | Index count | Material index / stored name | Texture index / stored name |
| --- | ---: | ---: | --- | --- |
| 0 | 0 | 450 | 0 / `collision02_floor` | 0 / `collision02_floor` |
| 1 | 450 | 360 | 4 / `collision02_shabon` | 4 / `collision02_shabon` |
| 2 | 810 | 18 | 3 / `collision02_rainbow` | 3 / `collision02_rainbow` |
| 3 | 828 | 900 | 1 / `collision02_kumo3` | 1 / `collision02_kumo01` |
| 4 | 1728 | 180 | 2 / `collision02_kumo` | 2 / `collision02_kumo00` |
| 5 | 1908 | 900 | 5 / `collision02_skyw` | 5 / `collision02_wall` |

Material texture references resolve by the reference's stored path and the
TXOB object's stored name. Dictionary keys and material names are not assumed
to match those names. Missing or ambiguous texture matches fail explicitly.

| Material | Translucency kind | Wrap U / V | Alpha test | Depth test / write | Blend |
| --- | ---: | --- | --- | --- | --- |
| 0 | 0 | repeat / repeat | greater than 0 | disabled / disabled | replace |
| 1 | 2 | repeat / repeat | disabled | less / disabled | source-alpha factors |
| 2 | 2 | repeat / repeat | disabled | less / disabled | source-alpha factors |
| 3 | 1 | repeat / mirror | greater than 128 | less / enabled | replace |
| 4 | 3 | repeat / repeat | disabled | less / disabled | source-alpha factors |
| 5 | 1 | mirror / mirror | disabled | less / disabled | source-alpha factors |

All six have nearest minification/magnification, a serialized linear mip-filter
bit, zero LOD bias/minimum level, back-face culling, UV source 0, and identity
material UV transforms. The preview uses the decoded base image; mip selection
is not reproduced. Enabled blending uses add equations with source-alpha and
one-minus-source-alpha factors for **both** RGB and alpha. Its alpha result is
`srcA*srcA + dstA*(1-srcA)`, rather than the separate source-over alpha formula.

All source object priorities are zero, their model layer is zero, and the
observed model/object/node/descriptor visibility values are enabled. The parser
returns resource order and raw ordering metadata. The viewer applies its
documented preview policy: ascending material translucency kind, then render
priority, retaining object order for ties. It does not infer the game's
distance-dependent transparency order.

## Primary source checks and important distinctions

Offsets were checked against original local bytes and
[SPICA pinned at bd29a78](https://github.com/gdkchan/SPICA/tree/bd29a7828595d7839cda2ac61c76bb63f9071250/SPICA/Formats/CtrGfx/Model/Material).
The original
[Ohana CGFX importer](https://github.com/gdkchan/Ohana3DS-Rebirth/blob/c50635e82763fad9d1bcc30a905523a1b92d7d1f/Ohana3DS%20Rebirth/Ohana/Models/CGFX.cs)
provides useful independent context, but its material and sampler reads omit
fields present in SPICA and in the selected binary. The implementation follows
the corroborated layout:

| Record | Fields used |
| --- | --- |
| CMDL | Material dictionary count/pointer `+0xbc/+0xc0`; object table links each object to a shape and material |
| MTOB | Type `0x08000000`, revision `0x06000003`; name `+12`; coordinate config `+0x1c`; translucency kind `+0x20` |
| Rasterization | `GfxMaterialColor.CommandCache` occupies `+0x100`; polygon-offset flag `+0x104`, cull enum `+0x108`, offset units `+0x10c`, cull command `+0x110` |
| Depth / blend / stencil | Depth flags `+0x118`, commands `+0x11c`; blend mode `+0x12c`, commands `+0x140`; stencil commands `+0x158` |
| UV coordinator | Used-coordinate count `+0x168`; UV0 block at `+0x16c`, with source/mapping/camera/transform type, scale, rotation, translation, and stored 3×4 matrix |
| Mapper / fragment | Mapper-0 relative pointer `+0x274`; fragment-shader relative pointer `+0x288` |
| Texture mapper | Type `0x80000000`; texture reference `+8`, sampler `+12`, fourteen command words `+16`, command-byte length `+72` |
| Texture reference | Type `0x20000004`, TXOB magic, revision `0x05000000`; referenced texture path at `+24` |
| Standard sampler | Type, parent pointer, min-filter field, border color, LOD bias; LOD bias is `+28`, not the start of the record |
| Fragment shader | Alpha-test command pair `+0xd4` |

Selected inline PICA commands are parsed with their consecutive-register flag,
parameter count, padding, and byte write masks. They are not treated as raw
register snapshots. The source's depth-color-mask command writes only its low
byte, which specifies test enable and comparison; it does **not** declare that
all color channels should be disabled. The preview uses all-channel writes
when these serialized bytes are absent and reports
`color_write_mask=15, color_write_mask_declared=0`. A fully declared mask is
accepted only when all four channels are enabled. Depth test/write come from
the separately stored depth flags, while comparison comes from the command.
These are explicit preview rules, not a recovered complete GPU state history.

The [SPICA fixed-attribute conversion](https://github.com/gdkchan/SPICA/blob/bd29a7828595d7839cda2ac61c76bb63f9071250/SPICA/Formats/CtrGfx/Gfx.cs)
uses fixed vector values directly. The sample's fixed white color has a zero
scale metadata field, so multiplying that field would incorrectly turn the
shape black. Interleaved colors instead use their attribute scale: byte RGBA
times the stored float32 approximation to `1/255`. Missing vertex color uses
the preview's white default.

The same conversion maps material translucency kind to rendering layer.
[Model.cs](https://github.com/gdkchan/SPICA/blob/bd29a7828595d7839cda2ac61c76bb63f9071250/SPICA.Rendering/Model.cs)
sorts meshes by priority and submits layers 0 through 3. SPICA also reverses one layer as an explicit
workaround. These facts support a practical preview policy without proving
the original game's ordering.

The scene API retains the CGFX UV convention. The viewer converts `V` to
`1-V` for its decoded top-down images. This mapping was cross-checked against
SPICA's
[texture conversion](https://github.com/gdkchan/SPICA/blob/bd29a7828595d7839cda2ac61c76bb63f9071250/SPICA/PICA/Converters/TextureConverter.cs),
which reverses decoded rows, followed by its
[direct OpenGL upload](https://github.com/gdkchan/SPICA/blob/bd29a7828595d7839cda2ac61c76bb63f9071250/SPICA.Rendering/Texture.cs).
The independent reference report records both raw and ETC conversion paths.

## Additive API and limits

`px_cgfx_model_decode_textured` returns an owned `px_cgfx_scene` containing the
existing static mesh, UV0 float pairs, normalized RGBA float quads, per-object
draw ranges, and parsed material state. Texture indices select images through
the existing `px_cgfx` API; the caller owns those decoded images separately.
`px_cgfx_scene_free` releases and clears all scene allocations. No input bytes
are borrowed after a successful return. Failure clears the entire output.
The original wireframe decoder remains independent of material support and
continues to accept its earlier fixtures.

The textured subset adds these boundaries to the static geometry contract:

- At most 128 materials and 1,024 indexed textures; unique referenced images
  are loaded with an aggregate 64 MiB RGBA budget in the viewer.
- A single ordinary 2D texture mapper, UV0, coordinate configuration 0,
  and identity material transforms. Other texture stages and procedural
  mappers are rejected.
- Clamp-to-edge, repeat, or mirror wrapping; equal nearest or linear min/mag
  filters, zero LOD bias/minimum level. Border wrapping is unsupported.
- No stencil testing or polygon offset; supported cull modes exclude
  always-cull. Blending is replace or add with `(one, zero)` or
  `(source-alpha, one-minus-source-alpha)` factors in both channel groups.
- Object primitive index zero and visible index descriptors. Object, model,
  and node visibility are combined; mixed descriptor visibility is rejected.
- Finite scaled UVs bounded to absolute `1e6` and finite normalized colors in
  `[0,1]`. Fixed float colors are supported; unsupported component counts or
  absent UV0 fail explicitly.

The software preview uses orthographic projection, affine interpolation,
back-face handling in its documented screen-coordinate convention, a depth
buffer, alpha testing, and the supported blend equations. It does not replay
PICA shaders, lighting, fog, combiners, mip choice, or material animation.

## Verification and next boundary

The independent synthetic scene fixture verifies scaled UVs, byte and fixed
colors, name-based texture binding with different dictionary labels, material
state, PICA command masks and consecutive packets, draw ranges, node
visibility, cleanup, and rejection of unsupported or malformed fields. It also
checks that an unsupported textured transform leaves the wireframe API usable.
Separate raster and runtime fixtures exercise the actual rendering and archive
paths.

An independent direct-byte reference matched every UV/color output byte, 162
material fields, and 48 draw fields for the original local sample. The portable
[reference comparison](../native/reproduction/textured-model/reference-comparison.json)
records input hashes, exact observations, pinned references, and the comparison
method without embedding game arrays or images. All earlier reconstruction
directories and reproduction records remain unchanged.

A later fidelity milestone should choose a specific material and recover its
combiner/shader output, animation, and runtime draw state against a known
reference frame. Adding map traversal still requires separate placement,
collision, spawn, and transition evidence. The textured preview alone does
not establish those behaviors.
