# Native field terrain inspector

The native field viewer follows a checked XY resource chain and displays its
terrain grid, with optional collision triangle edges and an inspection marker.
Original zone `5` selects area `75` and matrix `12`; that matrix contains one
region, `165`. These are numeric identities for the checked local baseline.
No location name or player spawn is inferred.

```sh
make -C native
native/build/pokemon_field_viewer --demo
native/build/pokemon_field_viewer --romfs /path/to/romfs --zone 5 --inspect
native/build/pokemon_field_viewer --romfs /path/to/romfs --zone 5 --collision
```

Arrows or WASD move the inspection marker. `+`/`-` zoom, `F` toggles between
the standard-cell bounds and the full matrix, `C` toggles collision edges,
`R`/Home resets, and Escape exits. The top view places increasing X to the right
and increasing Z downward. The camera, colors, focus bounds and initial marker
are inspection choices. The demo uses a generated grid without game files.

`--steps RDLU` applies a repeatable sequence before inspection or display.
`--capture-frame new.bmp` exclusively creates a viewport capture; existing
files and symlink targets are preserved. `--frames N`, `--software` and
`--self-test-input` support bounded runtime checks. `--inspect` initializes no
video backend and rejects window-only options.

## Verified selection and original-code boundary

| Resource | XY archive | Checked selection |
| --- | --- | --- |
| Zone, ZO | `a/0/1/2` | Entry 5, header area 75 and matrix 12 |
| Area | `a/0/1/3` | Association recorded; graphics not loaded by this viewer |
| Matrix, MM | `a/0/4/2` | Entry 12, 1 × 1 region table containing 165 |
| Field region, GR | `a/0/4/1` | Entry 165, 40 × 40 raw cells and collision triangles |

All selected entries use GARC subentry zero. Existing bounded GARC and LZ10/11
readers supply the resource bytes. Original executable analysis independently
corroborates the archive table and zone-to-matrix selection: `0x00107c94`
installs the table, `0x00112b78` selects archive index 12, and the transition
path through `0x003b843c` reads the zone header's unsigned 16-bit matrix ID at
offset 4 via `0x004a311c`. It passes that ID to the archive-42 matrix object.
The matrix parser at `0x003e3660` reads dimensions at offsets 4/6 and the
16-bit region table at offset 8. DllField's setup path at `0x15ca4cc0` constructs
`field::MapFileSimple` with archive 41; its loader separately consumes GR sections.

DllField addresses are synthetic saved-project addresses, not observed runtime
addresses. The original-code review and reproducible exporter are preserved in
[`native/reproduction/field-scene/`](../native/reproduction/field-scene/README.txt).
This evidence supports resource selection. It does not yet reconstruct player
entry, transitions, physics, terrain flag consumers, or the script VM.

The [BCH continuation](native-bch.md#external-textures-and-next-work) also follows
the original zone-area accessor through the archive-13 member request and the
AD section-11 resource consumer. Its thirteen reviewed functions have 1,348
instructions checked against the original bytes. This corroborates the selected
area-75 texture-container path; later field-material binding remains unverified.

Format and placement facts were also checked against the original
[CTRMap source at 4b780a7](https://github.com/Rynbo/CTRMap/tree/4b780a7bc32a75a7f31c162829b086355caf32f2),
especially `Workspace.java`, `AbstractGamefreakContainer.java`, `MapMatrix.java`,
`Tilemap.java`, `GRCollisionFile.java`, and `GRCollisionMesh.java`. Exact terrain
labels come from that source's `DefaultTileset.mets`, not a guessed flag mask.
The viewer is independently authored C; no external implementation or original
game payload is included.

## Bounded parsing and geometry

`native/assets/field.h` exposes owned ZO header, MM table, GR tile and collision
records. It also provides a borrowed minipack section API. Section offsets are
validated across the entire offset table, including unselected sections.
Unsupported variants fail explicitly. The accepted subset is:

- Four-section XY ZO with an exact 56-byte header. Camera and coordinate bytes
  remain uninterpreted. In particular, the header tail is not an XY spawn.
- Two-section MM with positive dimensions, row-major signed region IDs (`-1`
  denotes an empty slot), optional finer zone and LOD tables, and bounded camera
  records. Camera metadata is preserved, not applied to the inspection view.
- Six-section GR with raw four-byte cells in section 0. Section 1 contains BCH
  graphics in the selected original sample, section 2 contains `coll` geometry.
  Models, props, encounters and extended layers are not rendered or executed.
- Collision groups with checked offsets/counts, triangle alignment, finite
  binary32 records, checked length/terminator and bounded zero alignment padding.
  Stored order and duplicates are retained. XYZ magnitude is capped at `1e12`.

The parser caps input at 64 MiB, section count at 256, grid dimensions at
256 per side/65,536 cells, camera records at 4,096, and collision vertices at
1,048,576. The app further limits the matrix to 64 cells, requires 40 × 40
regions, caps aggregate decoded member bytes at 64 MiB and collision vertices
at 262,144 across all matrix instances. Repeated regions share allocations.
Missing references or unsupported grids reject the scene; no substitute region
is drawn. Empty slots and scenes without standard cells remain inspectable.

Region placement uses 720 world units per matrix cell and 18 units per terrain
cell. A local collision point becomes world X/Z by adding
`360 + 720 * matrix_coordinate` on each axis. The marker samples cell centers.
The geometric query returns the first stored nonvertical triangle intersecting
that X/Z and interpolates its height. It does not select the highest floor,
infer walls, or resolve overlapping surfaces. Overlay edges are clipped in
double precision before reaching SDL's integer software rasterizer.

## Explicit inspection movement policy

Only the exact byte sequence `20 00 00 00` admits the marker; its little-endian
word is `0x00000020`. The exact standard-blocked sequence `21 00 00 01` is gray.
All other words are amber and excluded by this deliberately narrow policy.
The viewer never tests only one flag or generalizes another terrain variant.
It starts at the first eligible cell in row-major order and moves one cell per
cardinal key event. If there is no such cell, no marker is created.

This is a testable inspection policy using verified resource data. It is not
reconstructed player movement or a game collision algorithm. The geometric
height query is a readout; it does not authorize a step or change game state.
Warp/entity records, running, ledges, water, directional rules, triggers and
transitions remain unimplemented.

For region 165, independent raw reads match all 1,600 terrain cells, 1,728
stored XYZ vertices (576 triangles), six section records and sixteen collision
groups. There are 52 exact standard cells, 1,542 exact standard-blocked cells,
and six other cells. All 52 standard-cell centers intersect collision geometry
at height zero. Duplicate stored triangles are counted, not deduplicated.

## Remaining first-playable work

The selected field uses BCH graphics, so the existing CGFX textured preview
does not render its environment. Next decode this field's BCH models/textures
and checked area/prop associations, then establish the actual player entry and
movement/collision contract. A captured reference scenario is still needed.
Interaction, interactive battle, return to field, and save/load remain required
by the [first-playable acceptance criteria](PLAYABLE_SLICE.md).

### BCH index continuation

The new [BCH index tool](../native/tools/inspect_bch.py) reads region 165's GR
section 1: compatibility 7/7, one model named `t01r0202_00_00`, twelve material
records, and no embedded textures. The index itself does not establish
material-to-texture bindings or render geometry.
Its bounds and synthetic checks are recorded separately in
[the new evidence directory](../native/reproduction/bch-index/README.txt).

The subsequent [BCH model inspector](native-bch.md) now decodes positions, UV0
and indices for all twelve meshes, with an independent numeric comparison.
Every nonnull texture-unit name resolves in area 75's world-texture BCH.
The native field viewer still shows terrain/collision only. Native BCH rendering,
projected texture/shader behavior, the original area consumer and actual player
entry remain the next work; an optional OBJ uses an explicitly stated triangle-list
interpretation because primitive GPU state is not fully determined here.
