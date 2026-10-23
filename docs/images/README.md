# Native viewer captures

These documentation images are unmodified PNG copies of existing captures from
the local project workspace. They were selected for the main README at the
project owner's request. No generated artwork or mockups are used.

| Image | Local source | What it shows |
| --- | --- | --- |
| [BCH wireframe](native-bch-wireframe.png) | `outputs/native-bch-geometry-validated/field165-wireframe.png` | Region 165: 12 meshes, 2,356 vertices and 4,992 indices, displayed with an explicit triangle-list interpretation. All meshes are shown; materials are not executed. |
| [Textured BCH field](native-bch-textured.png) | `outputs/native-bch-textured/field-textured.bmp` (lossless PNG conversion) | Region 165 with area 75's textures: twelve material bindings chosen by each material's TEV stage 0, unlit, culling not executed, explicit triangle-list interpretation. Captured from single-member archives around the hash-pinned field and texture payloads. Not the game's final appearance. |
| [Textured CGFX model](native-textured-model.png) | `outputs/native-textured-model/model-frame.png` | Archive `a/1/0/2`, entry 0: six materials and textures in an unlit software preview. Lighting, complete shaders and original-game visual equivalence are not reproduced. |
| [Terrain/collision inspector](native-field-inspector.png) | `outputs/native-field-scene/field-collision-frame.png` | Zone 5 → matrix 12 → region 165, with collision edges and a restricted inspection marker. This earlier capture's footer predates the separate BCH wireframe milestone; it is not the current status of BCH decoding. |

The source captures are dated October 23, 2026. PNG dimensions are 960 × 640 for
the three model images and 1100 × 760 for the field inspector. They retain their
original pixels and dimensions; the README scales their display size only.

Evidence and reproduction instructions:

- [Native BCH geometry comparison](../../native/reproduction/bch-native/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-native/README.txt).
- [Textured BCH material comparison](../../native/reproduction/bch-textured/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-textured/README.txt).
- [Textured CGFX capture record](../../native/reproduction/textured-model/rendered-sample.json).
- [Field inspector scope and reproduction](../../native/reproduction/field-scene/README.txt).

SHA-256 hashes of the exact PNG files:

```text
0f002c357ea1eb4ed6970cbb3569f07b7c1de83352e22ddca87a274379f84963  native-bch-wireframe.png
c8c76b2e9f68859d2cc7af1ad2c7fb277d8d38496166ee36b99034890ed8b6ce  native-bch-textured.png
b987693a363ff84bf009c6d537b7e2937e9e8d4fdba4d4855bcb2f11ae0a0f45  native-textured-model.png
2335afaea29641d0c037bd082ec54963cdb2ea3099030a5d5a1e7a24862032de  native-field-inspector.png
```

These images document the native applications; they do not demonstrate a
playable port. Original game assets depicted in the captures belong to their
respective rights holders. ROMs, executable data and extracted asset payloads
are not included with these screenshots.
