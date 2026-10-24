Native open-world walking: Vaniville Town to Route 1 and Aquacorde Town, 2026-10-24

Walking out of a zone's own map cells now switches to the zone that owns the
next cells, without a load screen: north from Vaniville Town (zone 0) the
player walks up Route 1 (zone 258) into Aquacorde Town (zone 8), with its
fountain and townspeople. Scenes now include a ring of neighbouring cells,
so the next area is already visible at the edge. For fun, --silly giant and
--silly moonwalk change the player. Tracked files contain independently
authored source, synthetic tests and metadata only; the documentation PNGs
are recorded in docs/images/README.md.

Native preview

native/build/pokemon_model_viewer --romfs /path/to/romfs --zone 0

Walk up the path north of the town square. --silly giant draws the player
three times larger; --silly moonwalk makes the player face away from each
step and play the walk cycle backwards. Both are jokes, not game behaviour,
and change nothing else.

Scene rule (changed from ../bch-town)

- The zone's own cells are as before (sub-cell zone table, or every cell
  with a region when the matrix has none). The scene is their bounding box
  grown by one cell on each side within the matrix, with every cell of it
  that has a region, whichever zone owns it.
- The scene region is the zone's first own cell in row-major order, drawn and
  queried at its offset from the box's first cell (zone_regions now prints
  scene=ENTRY@X,Z). This also loads the 4 zones whose bounding box began on a
  cell not their own.
- A neighbouring region that fails to load is left out (blocked, not drawn,
  reported); a failure in the zone's own cells still refuses the zone.

Zone crossing (inspection policy)

After every step, the sub-cell under the player names its zone. When that is
another zone of the same matrix, the viewer loads it and keeps the player's
world tile, facing and step (zone_cross line; the step and its walk cycle
carry on). A zone that fails to load is reported once, and the player stays
in the current zone. Not traced in the game's code: the game streams regions
and switches zones by its own rules, and may run zone scripts on entry.

Blending

Materials with blend factors other than the three earlier pairs now draw
with the general PICA factors 0..9 (zero, one, source and destination colour
and alpha, and their complements) and the add equation, RGB and alpha each
with their own pair. Aquacorde needs two: 0x01760000 (source alpha on RGB,
alpha replaced) and 0x98980000 (destination alpha). Constant-colour factors
and other equations are still refused.

Independent comparison (verify_route.py)

python3 -I -B native/reproduction/bch-route/verify_route.py --romfs /path/to/romfs \
  --viewer native/build/pokemon_model_viewer --output-dir outputs/new-route-check

It reuses the readers and parsers of ../bch-town/verify_town.py and derives
every zone's scene with the rule above: 291 of 360 zones now load (72
multi-region; 4 neighbouring regions left out), every zone_regions, prop
(2,669, 2,511 drawn), NPC tile (1,947) and warp tile (526) line identical.
With --no-props the captures of ../bch-zone and ../bch-talk still reproduce.
An independent walk over the joined region grids of the whole matrix 0
predicts the two crossings of the walk north from the town's first tile
(0 -> 258 at world tile 460,589; 258 -> 8 at 460,559) and its end; the
viewer's --steps and display runs match. Capture hashes, including the two
silly modes, are in reference-comparison.json.

Effect on earlier records: ../bch-town's zone_regions lines (format and
cells) and its town captures change with the neighbouring cells; its walks,
talks and --no-props captures still hold. The "first cell is not its own"
refusal no longer exists.

Tests

tests/test_bch_town_viewer.py (rewritten for the new rule): a synthetic town
of two cells next to a third zone's cell; the ring of neighbours, world
tiles and props in the grown frame, the NPC blocking below the path, crossing
into the third zone and back with world tiles kept, a crossing during
display (also moonwalking), a neighbour that fails to load, an undefined
neighbouring zone (reported once, walking goes on), the giant capture, and
refusals of --silly values. render/test_raster.c: general factors including
destination alpha; factor 10 refused. assets/test_bch_material.c: the two new
factor pairs accepted, a constant-colour factor and a non-add equation
refused.

Limits

As ../bch-town, and: crossing reloads the whole scene (a short pause); no
zone-entry scripts, no story-flag NPC visibility, type-1 warps never fire.
Tiles 0x00000028 (path edges) are not walkable under the exact-cell policy.
No running-game comparison.
