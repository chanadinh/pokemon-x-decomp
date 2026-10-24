Native player character in the walkable lit field, 2026-10-24

The walk marker of ../bch-walk becomes the game's player-character model:
skinned, textured from its own BCH, lit by the environment's character light
set and placed on the collision floor. Tracked files contain independently
authored source, synthetic tests and metadata only; the documentation PNGs are
recorded in docs/images/README.md.

Native preview

native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/0/4/1 --entry 165 --section 1 --triangle-list --textured --combiners \
  --texture-archive a/0/1/3 --texture-entry 75 --texture-section 11 \
  --environment-archive a/0/0/3 --environment-entry 137 --environment-section 0 \
  --walk --player-archive a/0/2/1 --player-entry 2 --player-section 0

The model alone: --archive a/0/2/1 --entry 2 --section 0 --skinned with the same
archive as --texture-archive (and optionally the environment). WASD walks; the
camera follows the marker from above (pitch +0.9, zoom 2.5) and the model turns
toward each attempted step. --section and --texture-section now also accept MM
packs.

Which model

a/0/2/1 holds field character models (trNNNN_00_fi, NPCs, Pokemon, objects),
each an MM pack: section 0 a BCH with model and textures, section 1 a 173-item
pack (not decoded; probably animations), section 2 eye material animations.
Entry 1, tr0001_00_fi, has one material and no textures: it is assembled at run
time from the part archives a/0/8/9 (b1_*, b2_* models and textures) and a/0/8/8
(f1_*), which also hold the customization tables (not decoded). Entry 2,
tr0002_00_fi, is complete: nine parts (bag, bangle, bottoms, face, glasses,
hair, hat, shoes, tops) whose materials and textures are named after the f2_*
part set. It is used here as the player-character model. Which entry the game
loads for the player, and with which outfit, is not traced.

Skinning (assets/bch_model.h px_bch_model_decode_skinned, render/skin.c)

The skinned decoder accepts model flags and bone scaling, a skeleton of
100-byte H3DBone records (SPICA bd29a78: flags, parent, scale, rotation,
translation, 3x4 inverse bind matrix, name), each submesh's skinning mode (0
none, 1 smooth, 2 rigid) and palette of up to 20 bones, and bone index (stored
u8 or fixed) and weight attributes (any stored format, scaled by uniform 8.w
as SPICA's H3DMesh reader does, or fixed), filling missing components with the
PICA default (0, 0, 0, 1). px_bch_model_decode is unchanged and still refuses
skinned models. The male tr0001 is refused by both decoders; why was not
investigated.

Rest pose follows SPICA's DefaultVertexShader translation and palette setup:
bone world = parent world x (T Rz Ry Rx S); palette entry = world (rigid,
none) or world x inverse bind (smooth); smooth vertices blend x and y always, z
when its weight is nonzero, w only with four stored weight elements; rigid uses
index.x, none uses palette 0. tr0002: 45 bones; world x inverse bind is the
identity within 5.8e-6 for every bone, so the stored matrices and this
composition agree. Smooth vertices stay within 3.8e-6 of their stored
positions; the rigid glasses move from the origin to the head. The rest pose is
a T-pose: animations are not decoded.

Textures and shading

px_bch_open_mixed indexes the textures of a BCH that also holds other
resources (tags 03 and 25..28 bounded, not interpreted) and decodes the raw
formats under the same descriptor rule flags == 0x100 | format; px_bch_open is
unchanged. All nine player materials bind (three RGB565 and six ETC1 textures)
and their six-stage chains are accepted. Each uses light set 2, whose
executed light is light_character: ambient white, diffuse black.

The player's chains read the fragment secondary color in alpha. With lighting
executed it is now supplied as Azahar computes it for this subset: RGB 0 and
alpha 255 (specular_sum starts at (0, 0, 0, 1); Fresnel excluded). RGB is 0
exactly because every specular register of the executed lights (light x
material) is zero; materials where that fails are rejected, and point/spot
specular registers are reported as a bound (light_secondary lines). The room's
chains do not read the secondary (../bch-combiners), and its lit captures are
byte-identical.

Camera correction

The inspection camera's depth is -(sin(pitch) y + cos(pitch) z'): a negative
pitch puts the eye below the model. The earlier default views (pitch -0.35,
and -1.2 in ../bch-walk) therefore looked up through the floor; the walk
marker was visible only because its bottom face lay on the floor. Walk mode
now looks from above. The default view is unchanged, so the earlier records'
captures reproduce, except ../bch-walk's walk capture (55f1b9b2...), which
reproduces only at commit 622102d; its verifier report reproduces exactly.

Also fixed: the first walk commit printed a stray CGFX summary line in every
textured BCH --inspect run (the walk block detached an else-if); a test now
checks for exactly one summary line.

Independent comparison (verify_player.py)

cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror native/assets/bch_model.c \
  native/render/skin.c native/reproduction/bch-player/dump_player.c -lm \
  -o native/build/dump_player
python3 -I -B native/reproduction/bch-player/verify_player.py --romfs /path/to/romfs \
  --bridge native/build/dump_player --viewer native/build/pokemon_model_viewer \
  --output-dir outputs/new-player-check

It extracts the model itself (GARC, LZ11, MM), parses skeleton, palettes and
bone attributes itself and poses every index in Python. Recorded result: model
flags, 45 bones (all fields), 9 meshes' bone index/weight sources, element
counts, 1,990 vertices' indices and weights, and every submesh's mode and
palette match; all 6,099 posed positions and normals match the native bake
exactly (5,463 smooth, 636 rigid). Viewer: walk capture sha256 460e940f...4b8e;
standalone capture (3 frames, input self-test) 1bb52546...5cef.

Tests

render/test_skin.c: hand-computed rest worlds, the affine inverse, the three
skinning modes, index clamping, the fourth-weight rule, palette bounds and the
bake (expansion, renormalized normals, bounds, unchanged model on failure).
assets/test_bch_model.c: a two-bone skinned fixture through both entry points
and its rejection cases. assets/test_bch_texture.c: the mixed reader.
tests/test_bch_player_viewer.py: a synthetic player BCH with its own texture
replaces the box, follows steps, fails before video without textures, and the
option contract.

Limits

Rest pose only (T-pose): no skeletal animation, so no walking motion. The male
player's run-time part assembly, the outfit tables and the game's choice of
player model are not decoded. The vertex shader's normal quaternion,
projection, culling, the field camera and the game's movement rules remain
unexecuted. The model's front is taken to be +Z.
