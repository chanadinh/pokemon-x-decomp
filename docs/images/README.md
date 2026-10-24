# Native viewer captures

These documentation images are unmodified PNG copies of existing captures from
the local project workspace. They were selected for the main README at the
project owner's request. No generated artwork or mockups are used.

| Image | Local source | What it shows |
| --- | --- | --- |
| [BCH wireframe](native-bch-wireframe.png) | `outputs/native-bch-geometry-validated/field165-wireframe.png` | Region 165: 12 meshes, 2,356 vertices and 4,992 indices, displayed with an explicit triangle-list interpretation. All meshes are shown; materials are not executed. |
| [Textured BCH field](native-bch-textured.png) | `outputs/native-bch-textured/field-textured.bmp` (lossless PNG conversion) | Region 165 with area 75's textures: twelve material bindings chosen by each material's TEV stage 0, unlit, culling not executed, explicit triangle-list interpretation. Captured from single-member archives around the hash-pinned field and texture payloads. Not the game's final appearance. |
| [BCH combiner chain](native-bch-combiners.png) | `outputs/native-bch-combiners/field-combiners.bmp` (lossless PNG conversion) | Region 165 with all six combiner stages and decoded vertex colors; lighting, projection and culling not executed (declared substitutes). Same wrapper-archive inputs as the textured capture. Not the game's final appearance. |
| [Lit BCH field, day](native-bch-lighting-day.png) | `outputs/native-bch-lighting/lighting-day.bmp` (lossless PNG conversion) | Region 165 with the six-stage combiners and directional fragment lighting from `a/0/0/3` entry 137 (`light_sun`); point/spot lights bounded, vertex shader, projection and culling not executed. Captured from the real archive paths. Not the game's final appearance. |
| [Lit BCH field, night](native-bch-lighting-night.png) | `outputs/native-bch-lighting/lighting-night.bmp` (lossless PNG conversion) | As the day capture with `--lighting night` (`light_night`). The game's day/night selection is not traced. |
| [Walkable lit field](native-bch-walk.png) | `outputs/native-bch-walk/walk.bmp` (lossless PNG conversion) | The lit day capture's room seen from above (`--walk --steps DDDRRR`), with the inspection marker on the GR collision floor. Exact-cell inspection policy, not game movement. Real archive paths. |
| [Player in the lit field](native-bch-player-walk.png) | `outputs/native-bch-player/player-walk.bmp` (lossless PNG conversion) | `--walk --player-archive a/0/2/1 --player-entry 2 --player-section 0 --steps DDDRRR`: the player-character model (rest pose) on the rug of the lit room, camera above and following. Not the game's camera or animation. Real archive paths. |
| [Player model](native-bch-player.png) | `outputs/native-bch-player/player-standalone.bmp` (lossless PNG conversion) | `tr0002_00_fi` alone with `--skinned`, its own textures and the character light set, in its rest (T) pose. |
| [Textured CGFX model](native-textured-model.png) | `outputs/native-textured-model/model-frame.png` | Archive `a/1/0/2`, entry 0: six materials and textures in an unlit software preview. Lighting, complete shaders and original-game visual equivalence are not reproduced. |
| [Terrain/collision inspector](native-field-inspector.png) | `outputs/native-field-scene/field-collision-frame.png` | Zone 5 → matrix 12 → region 165, with collision edges and a restricted inspection marker. This earlier capture's footer predates the separate BCH wireframe milestone; it is not the current status of BCH decoding. |

The source captures are dated October 23, 2026, except the lit-field and walk captures
(October 24, 2026), and the player captures (also October 24). PNG dimensions are 960 × 640 for
the nine model images and 1100 × 760 for the field inspector. They retain their
original pixels and dimensions; the README scales their display size only.

Evidence and reproduction instructions:

- [Native BCH geometry comparison](../../native/reproduction/bch-native/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-native/README.txt).
- [Textured BCH material comparison](../../native/reproduction/bch-textured/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-textured/README.txt).
- [Combiner oracle comparison](../../native/reproduction/bch-combiners/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-combiners/README.txt).
- [Lighting oracle comparison](../../native/reproduction/bch-lighting/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-lighting/README.txt).
- [Walk alignment comparison](../../native/reproduction/bch-walk/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-walk/README.txt).
- [Player skinning comparison](../../native/reproduction/bch-player/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-player/README.txt).
- [Textured CGFX capture record](../../native/reproduction/textured-model/rendered-sample.json).
- [Field inspector scope and reproduction](../../native/reproduction/field-scene/README.txt).

SHA-256 hashes of the exact PNG files:

```text
0f002c357ea1eb4ed6970cbb3569f07b7c1de83352e22ddca87a274379f84963  native-bch-wireframe.png
c8c76b2e9f68859d2cc7af1ad2c7fb277d8d38496166ee36b99034890ed8b6ce  native-bch-textured.png
2d21e67b7e40d59da0fcf4caf231bdb71aed0f962a34371d7201302f5435d65a  native-bch-combiners.png
1597a107c95612b5bf0ee56aec11e2483e8cb1f8990756866fc610527d90de8c  native-bch-lighting-day.png
537790b82613c62a1e50c6c9f644f66d5a5f7766cbab324399334909c8e8bbb6  native-bch-lighting-night.png
965c990180858d7baff9f9e88b383bf28ae457d3741d0371d63c3a63b54770f5  native-bch-walk.png
37972f5a9a7aec3a88c25a7a6f7fbbd03f340dd43ff5a906c7e56e05899ea928  native-bch-player-walk.png
5f2ceb3c1a96b738dacca747f6e27a888e0ecb2dd492d9a38a50a3c89e1fdb9e  native-bch-player.png
b987693a363ff84bf009c6d537b7e2937e9e8d4fdba4d4855bcb2f11ae0a0f45  native-textured-model.png
2335afaea29641d0c037bd082ec54963cdb2ea3099030a5d5a1e7a24862032de  native-field-inspector.png
```

These images document the native applications; they do not demonstrate a
playable port. Original game assets depicted in the captures belong to their
respective rights holders. ROMs, executable data and extracted asset payloads
are not included with these screenshots.
