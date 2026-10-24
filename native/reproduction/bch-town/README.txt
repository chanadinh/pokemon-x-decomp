Native town scenes: Vaniville Town, region props and banded drawing, 2026-10-24

--zone now loads zones that span several map regions. The player's front door
leads out into Vaniville Town (zone 0), the town's doors lead back in, and the
town's NPCs talk with the game's lines. Every region's props (doors, lamps,
fences, furniture) are drawn. Tracked files contain independently authored
source, synthetic tests and metadata only; the documentation PNGs are recorded
in docs/images/README.md.

Native preview

native/build/pokemon_model_viewer --romfs /path/to/romfs --zone 5

Walk downstairs and out of the front door (or start outside with --zone 0
--arrive 1). Controls as before. New options: --threads N draws BCH scenes in
N row bands (1..8; default one per CPU, at most 8), --no-props leaves region
props out (the scenes of the earlier records).

What is resolved from game data

- Zone cells (a/0/4/2, MM section 0): the matrix's sub-cell zone table
  ((width * 4) x (height * 4) zone ids, row-major, 10 x 10 tiles each, after
  the per-cell region ids; checked against zone 0, whose ids cover exactly its
  four cells) names the cells of the zone: those with a region and at least
  one sub-cell of the zone. A matrix without the table (has_lod 0; each such
  matrix is used by one zone) gives the zone every cell with a region. 1 x 1
  matrices load as before.
- Scene: the bounding box of those cells; its first cell must be the zone's
  own, and its region defines the scene frame. Each other region (model,
  textures, environment, terrain, collision) loads at its cell offset, 720
  units per cell. The terrain grids (40 x 40 tiles each) are joined into one;
  cells outside the zone are blocked.
- World tiles: zone NPC and warp tiles are world tiles (zone 0: NPC tiles
  around 450..471, 596..611); the scene subtracts 40 per origin cell. Indoor
  zones (origin cell 0,0) are unchanged.
- Props (CTRMap 4b780a7 GRProp.java, ADPropRegistry.java): GR section 3, a u32
  count of 0x2C-byte records: uid, scale, rotation (degrees: only yaw 0, 90,
  180, 270 occurs, and no other rotation axis), world position, an unknown
  word. The area's AD section 0 registry (80-byte entries, reference and
  model u16; reference equals model in all 3,063 entries) maps a uid to an
  a/0/2/4 BM entry, section 0 BCH; the uid itself when absent, as CTRMap.
  Textures from the area's AD section 1. Placement: scale, yaw, then the world
  position less 360 + 720 * origin cell. Rest pose.
- Indoor rooms also carry props: the house rooms now show their tables,
  cabinets, plants and rug.

Viewer policy (inspection choices)

- Default start: the first walkable tile of the zone's own sub-cells.
- Camera: multi-region scenes keep the house interior's scale (radius 185)
  instead of the region bounds.
- Terrain models with a skeleton (e.g. region 14) are decoded skinned and
  baked in their rest pose.
- Texture packs outside the texture-only subset (outdoor areas: raw formats,
  mipmaps) are read as mixed BCH; mipmapped textures decode their base level
  (descriptor flags (levels << 8) | format, 1..13 levels).
- Texture coordinate transforms (scale, rotation, translation; SPICA
  TextureTransform types Maya, SoftImage and 3dsMax) are applied per unit in
  the combiner path.
- Materials with a bump map or a Fresnel selector are lit without them; the
  zone_lighting line counts them (8 in zone 0).
- Region props with a model that fails to decode are listed and not drawn
  (zone 4: one TV; 7 models in the game, mostly TVs and PCs, use a fixed
  texture-coordinate attribute the decoder refuses).
- The 16- and 17-prop sections start with 0x10/0x11: prop and registry
  sections are read raw, never as LZ data.
- Banded drawing: every band draws every triangle in the same order into its
  rows only, so pixels equal a single pass (raster test; the town capture
  in 1 and 2 bands is identical). The counted first frame of a combiner run
  is drawn in one pass. A TEV stage that replaces the previous colour and
  alpha unscaled is skipped (randomized test against the general path).

Inspect lines (new)

zone_regions (cells, origin cell, scene region, each other region and its
offset, tile origin), prop (region, uid, model entry, scene position, yaw,
scale, drawn), zone_props, zone_lighting.

Independent comparison (verify_town.py)

cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror native/assets/zone.c \
  native/reproduction/bch-town/dump_town.c -lm -o native/build/dump_town
python3 -I -B native/reproduction/bch-town/verify_town.py --romfs /path/to/romfs \
  --bridge native/build/dump_town --viewer native/build/pokemon_model_viewer \
  --output-dir outputs/new-town-check

It parses the props of all 1,030 regions with a section 3 (1,459 records) and
all 170 registries (3,063 entries) itself: identical to the native decoders.
For every zone it derives the scene from the zone header and matrix and the
expected zone_regions line, props, NPC tiles and warp tiles, and compares
them with the viewer's --inspect: 287 of 360 zones load (68 of them
multi-region), every compared line identical (2,474 props, 2,313 drawn;
1,891 NPC tiles; 472 warp tiles). The 73 others fail with the reasons listed
in reference-comparison.json (mostly a region or model the decoders refuse;
4 zones whose bounding box starts on a cell that is not theirs). With
--no-props the viewer reproduces the captures of ../bch-zone (frames 0, 60,
200) and ../bch-talk exactly. Runtime: from the house (zone 4) through the
front door into the town, 43 steps across it to NPC 1 and a talk with both
pages of its line; capture hashes in reference-comparison.json.

Effect on earlier records: the ../bch-zone walk that left the house (DRRRRD
from zone 4) now loads the town instead of falling back
(result=loaded arrival=cell:35,37); its other walks and lines reproduce. The
house captures of ../bch-zone and ../bch-talk now include the props (they
reproduce exactly with --no-props).

Performance (this container, 2 CPUs): the town takes about 98 ms per frame in
one band and 61 ms in two; the room about 16 ms. Not measured on the local
host.

Tests

assets/test_zone.c: prop records and registry entries, padding, caps and
truncation. render/test_raster.c: banded drawing equals the whole target and
leaves other rows untouched. render/test_tev.c: the pass-through shortcut
against the general path (20,000 random chains). assets/test_bch_material.c:
coordinate transforms (Maya and SoftImage types) accepted with their
matrices, type 3 and non-finite values refused. assets/test_bch_texture.c: a
two-level texture decodes its base level; zero levels and unknown descriptor
bits are refused. tests/test_bch_town_viewer.py: a synthetic two-region town
(sub-cell zone table, world tiles, props with and without a registry, a
missing prop model, a walk across the region seam, a warp out, a matrix
without a zone table, captures in 1 and 4 bands, --no-props, refusals).
tests/test_bch_zone_viewer.py: its unsupported-matrix cases now use a matrix
without a region.

Limits

Walking into the sub-cells of another zone (north of Vaniville: zones 258
and 8) does not switch zone; its NPCs and warps are not loaded. Type-1 warps
never fire. All NPCs are shown regardless of story flags. No bump mapping,
Fresnel, fog, point lights, prop animation or day/night prop events. Camera
scale and spawn are choices. No running-game comparison.
