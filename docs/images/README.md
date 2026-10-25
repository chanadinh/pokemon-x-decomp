# Native viewer captures

These documentation images are unmodified PNG copies of existing captures from
the local project workspace. They were selected for the main README at the
project owner's request. No generated artwork or mockups are used.

The showcase video is an unmodified capture from the local project workspace.

| Video | Local source | What it shows |
| --- | --- | --- |
| [Pokémon X zone 5 showcase](pokemon-x-zone5-showcase.mp4) | Native viewer capture from Ubuntu, October 24, 2026 | Walking and running through zone 5, talking to the NPC, and orbiting the camera with the controls listed in the main README. |

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
| [Walking player](native-bch-motion-walk.png) | `outputs/native-bch-motion/walk-step-7.bmp` (lossless PNG conversion) | As the player capture with `--motion-section 1 --motion 1 --walk-motion 3 --motion-frame 7`: frame 7 of the walk cycle (slot 3) halfway through the last step, gliding between cells. Motion choice and glide are inspection choices, not the game's controller. Real archive paths. |
| [Animated player model](native-bch-motion.png) | `outputs/native-bch-motion/motion-3-7.bmp` (lossless PNG conversion) | `tr0002_00_fi` alone posed by frame 7 of motion slot 3, unlit inputs declared (no environment). |
| [Bedroom walk](native-bch-zone-walk.png) | `outputs/native-bch-zone/play-60.bmp` (lossless PNG conversion) | `--zone 5 --play DDDRDDLRRRRUUUURRUU. --frames 520 --capture-at 60`: the player walking in the bedroom at the 3DS top-screen resolution 400 × 240, scaled to 800 × 480. Real archive paths. |
| [Downstairs, talking](native-bch-zone.png) | `outputs/native-bch-zone/play-519.bmp` (lossless PNG conversion) | The same run's last frame: after the stairs warp to zone 4, the player faces the NPC, who has turned toward the player. Dialogue is not executed. |
| [Talking downstairs](native-bch-talk.png) | `outputs/native-bch-talk/talk.bmp` (lossless PNG conversion) | `--zone 4 --arrive 1 --play RRRRUUUURRUU. --frames 330 --capture-at 329`: the first page of the NPC's line, chosen by the zone script, in the viewer's own message window and font. Real archive paths. |
| [Vaniville Town](native-bch-town.png) | `outputs/native-bch-town/town.bmp` (lossless PNG conversion) | `--zone 0 --arrive 1 --play DDDDDDDDDDD --frames 330 --capture-at 329`: the town's regions, props (lamps, doors) and NPCs around the player. Real archive paths. |
| [Talking in town](native-bch-town-talk.png) | `outputs/native-bch-town/town-talk.bmp` (lossless PNG conversion) | `--zone 0 --arrive 2 --play DDDLL. --frames 150 --capture-at 149`: the first page of an NPC's line in Vaniville Town. Real archive paths. |
| [Route 1](native-bch-route1.png) | `outputs/new-route-check/route-route1.bmp` (lossless PNG conversion) | `--zone 0 --play ruuu…u --frames 760 --capture-at 300` (one `r`, then 70 `u`): after the crossing from Vaniville Town into zone 258, walking north toward Aquacorde. Real archive paths. |
| [Aquacorde Town](native-bch-aquacorde.png) | `outputs/new-route-check/route-aquacorde.bmp` (lossless PNG conversion) | The same run's frame 759: after the second crossing, in zone 8 at the fountain. |
| [Giant](native-bch-giant.png) | `outputs/new-route-check/route-giant.bmp` (lossless PNG conversion) | The same run with `--silly giant` (a joke mode, not game behaviour). |
| [Moonwalk](native-bch-moonwalk.png) | `outputs/new-route-check/route-moonwalk.bmp` (lossless PNG conversion) | The same run's frame 300 with `--silly moonwalk` (a joke mode, not game behaviour). |
| [Route 2](native-bch-route2.png) | `outputs/new-battle-check` run, frame 200 (lossless PNG conversion) | `--zone 259 --play RDDDDDDDDD…. --encounter-rate 100 --code-bin … --frames 520 --capture-at 200`: walking into Route 2's tall grass. Real archive paths. |
| [Wild battle](native-bch-battle.png) | The same run, frame 228 | The battle screen with the move menu: a wild Scatterbug against Froakie. Placeholders, not battle models. |
| [Catch](native-bch-catch.png) | `outputs/new-catch-check` run: the catch play of `../native/reproduction/bch-catch/verify_catch.py` with three rounds, frame 247 (lossless PNG conversion) | The second Poké Ball holds: "Gotcha! Scatterbug was caught!". Placeholders, not battle models. |
| [Battle models](native-bch-pokemon.png) | `--zone 259 --seed 4 --play RDDDDDDDDD` + 40 `.` `--encounter-rate 100 --code-bin … --software --frames 229 --capture-at 228` (lossless PNG conversion) | Froakie and a wild Zigzagoon as their own a/0/0/7 models, rest pose. The stage and HUD are the viewer's own. |
| [Attack](native-bch-attack.gif) | `--zone 259 --seed 4 --play RDDDDDDDDD.... --encounter-rate 100 --code-bin … --software`, frames 224..290 every second frame (one run each, `--capture-at`), 400 x 240, 15 frames per second | Froakie's Pound motion, then the wild Zigzagoon's damage reaction. |
| [Switching](native-bch-switch.png) | `--load` of the `../native/reproduction/bch-party` party file, `--play D..RR.D` frame 30 and `D..RR.D..` frame 60 (two runs, each 400 x 240, side by side) | The party list over the battle, then Scatterbug sent out against a wild Bunnelby. |
| [Two screens](native-bch-two-screens.png) | `--zone 259 --seed 4 --bottom-screen --play RDDDDDDDDD.. --touch-play 235:160,60 --encounter-rate 100 --code-bin … --software --frames 240 --capture-at 238` (lossless PNG conversion) | Both screens in one window: the battle above, the move buttons on the touch screen after a tap on FIGHT. The button layout is the viewer's own. |
| [Vertex lighting](native-bch-vertex-lighting.png) | Without and with `--vertex-shader`: `--zone 259 --seed 4 --play RDDDDDDDDD.. --encounter-rate 100 --code-bin … --software --frames 240 --capture-at 238 [--vertex-shader]`, the 360 x 260 region at (60, 120) of each capture side by side (lossless PNG) | Froakie with its stored vertex colors (left) and with the primary color its own vertex program computes (right): per-vertex diffuse, Phong and rim terms instead of blown-out white. |
| [Fragment lighting](native-bch-fragment-lighting.png) | `--zone 259 --seed 4 --play RDDDDDDDDD.. --encounter-rate 100 --code-bin … --software --frames 240 --capture-at 238 --vertex-shader`, the 760 x 280 region at (40, 80): above with the viewer of the vertex lighting record, below with the fragment lighting unit (lossless PNG) | The battle with vertex lighting only (above) and with the PICA's fragment lighting unit, the materials' LUTs and the light_poke_1 light (below). |
| [Textured CGFX model](native-textured-model.png) | `outputs/native-textured-model/model-frame.png` | Archive `a/1/0/2`, entry 0: six materials and textures in an unlit software preview. Lighting, complete shaders and original-game visual equivalence are not reproduced. |
| [Terrain/collision inspector](native-field-inspector.png) | `outputs/native-field-scene/field-collision-frame.png` | Zone 5 → matrix 12 → region 165, with collision edges and a restricted inspection marker. This earlier capture's footer predates the separate BCH wireframe milestone; it is not the current status of BCH decoding. |

Battle captures made before the lighting default changed reproduce with `--no-vertex-shader` added; the two lighting comparisons name the options they used (their lit halves were made with earlier steps of the same lighting).

The source captures are dated October 23, 2026, except the lit-field and walk captures
(October 24, 2026), and the player, motion and zone captures (also October 24). PNG dimensions are 960 × 640 for
the eleven model images, 800 × 480 for the three zone images and 1100 × 760 for the field inspector. They retain their
original pixels and dimensions; the README scales their display size only.

Evidence and reproduction instructions:

- [Native BCH geometry comparison](../../native/reproduction/bch-native/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-native/README.txt).
- [Textured BCH material comparison](../../native/reproduction/bch-textured/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-textured/README.txt).
- [Combiner oracle comparison](../../native/reproduction/bch-combiners/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-combiners/README.txt).
- [Lighting oracle comparison](../../native/reproduction/bch-lighting/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-lighting/README.txt).
- [Walk alignment comparison](../../native/reproduction/bch-walk/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-walk/README.txt).
- [Player skinning comparison](../../native/reproduction/bch-player/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-player/README.txt).
- [Motion comparison](../../native/reproduction/bch-motion/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-motion/README.txt).
- [Zone comparison](../../native/reproduction/bch-zone/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-zone/README.txt).
- [Dialogue comparison](../../native/reproduction/bch-talk/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-talk/README.txt).
- [Town comparison](../../native/reproduction/bch-town/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-town/README.txt).
- [Route comparison](../../native/reproduction/bch-route/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-route/README.txt).
- [Battle comparison](../../native/reproduction/bch-battle/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-battle/README.txt).
- [Catch comparison](../../native/reproduction/bch-catch/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-catch/README.txt).
- [Battle model comparison](../../native/reproduction/bch-pokemon/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-pokemon/README.txt).
- [Battle motion comparison](../../native/reproduction/bch-battle-motion/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-battle-motion/README.txt).
- [Save comparison](../../native/reproduction/bch-save/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-save/README.txt).
- [Party comparison](../../native/reproduction/bch-party/reference-comparison.json) and [viewer commands](../../native/reproduction/bch-party/README.txt).
- [Platform layer record](../../native/reproduction/platform/README.txt).
- [Vertex lighting record](../../native/reproduction/vertex-lighting/README.txt).
- [Fragment lighting record](../../native/reproduction/fragment-lighting/README.txt).
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
bf53473c851c4ed09be977ce5580123fce5b8b170d82d3bfe523a23a2fb49a71  native-bch-motion-walk.png
115541aa251477e563f8e461fe81040eb1c384e65e9641c0f5cd89c39e197834  native-bch-motion.png
e617380d1a84d6d24b0240d5b94607ef908e3e1073865b0c3fe58143b0e73f8f  native-bch-zone-walk.png
002c3f1869e9b9bd0cecb2ae9dcf14f04972bf4cecf44f0122c3a768370eb9d2  native-bch-zone.png
592bf5b471d150352230972ff78bc87da4f144dc497d8c68e131d1e51eb16bf8  native-bch-talk.png
e529a030786531ad91bd0e07a726ec4f876d3c1eab33bafd65b9a64310b420f6  native-bch-town.png
91bc3ac2e2bae07ef8bfaacac8fed06094caa65c38a7850a0e5fe5fbda504440  native-bch-town-talk.png
8241ece2d898c4ae126939ad1a5a255d72bba3e43fa4a5ba0202820ceeb88f52  native-bch-route1.png
ee43af0523a4aec799c6f1cb9fa5034e9f8f0e829837ba3af349a7eca81b6ee8  native-bch-aquacorde.png
8d9f412a898c7752346890c564d02eefb5ab9c296dd1aecb0278b6a6800cd0e3  native-bch-giant.png
a6d50709637b89aba1d671d5e1735c7fc18f0a6f82395dfabd6d55a09fd91acf  native-bch-moonwalk.png
9c090058933ff09515ef3a29e2bd25c29269387c6db9f417c5bcdb6962c13835  native-bch-route2.png
454c5c3e336f947d2ce1c0954ec78e00a7a4d7ae01464eedde7153501f212085  native-bch-battle.png
e92763a8b528b0845707e9eed493ddd828636a680e129fe2ca99c005d433808c  native-bch-catch.png
e25fcd837276a38d0ba41d20cc8647f20b3edc4a206db8b70e8d1dd87f9922ea  native-bch-pokemon.png
94946f6bc576ff5625f98fb5daef8f7b401106341f79c7764b03fa38a0b883d1  native-bch-attack.gif
7182313967435a8d3130d266fac7965a50210b1384c414095ef771c6ea5cef7a  native-bch-switch.png
d3af5f4eac291cc528fe47ed3bfaf16cefd853aa33e088a766a1924a025ef76a  native-bch-two-screens.png
7f2bac63592fa20a7f40ded1f9d2c925b76bf6c0246c8d5a1e0cf97e1c2b37a5  native-bch-vertex-lighting.png
95cddc6661ed04dabbe07864c1c4b22d032574e670cb2fb44ad2243cb962b011  native-bch-fragment-lighting.png
b987693a363ff84bf009c6d537b7e2937e9e8d4fdba4d4855bcb2f11ae0a0f45  native-textured-model.png
2335afaea29641d0c037bd082ec54963cdb2ea3099030a5d5a1e7a24862032de  native-field-inspector.png
```

These images document the native applications; they do not demonstrate a
playable port. Original game assets depicted in the captures belong to their
respective rights holders. ROMs, executable data and extracted asset payloads
are not included with these screenshots.
