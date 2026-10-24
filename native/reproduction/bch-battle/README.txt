Native wild encounters and a first battle: Route 2, 2026-10-24

The field slice reaches its first battle. On Route 2 (zone 259), north of
Aquacorde Town, tall grass starts wild encounters drawn from the zone's own
encounter table; the battle runs on the game's species, move, learnset and
type data, with HP and stats from the reconstructed routines of
reconstructed/pokemon_stats, and returns to the field with the partner's HP
kept. Tracked files contain independently authored source, synthetic tests
and metadata only; the documentation PNGs are recorded in
docs/images/README.md.

Native preview

native/build/pokemon_model_viewer --romfs /path/to/romfs --zone 259 \
  --code-bin /path/to/code.bin

Walk into the grass south of the forest gate. In battle: WASD or arrows move
the cursor, Space/Return confirm, X/Backspace go back. New options:
--partner N and --partner-level N (default Froakie, 656, level 5), --seed N
(default 1), --encounter-rate N (percent per grass step, default 10),
--code-bin PATH (the nature table; without it natures are neutral). --play
also takes x (cancel).

What comes from game data

- Wild Pokémon: ZO section 3 of the zone, after pk3DS XYWE.cs: a 0x10-byte
  header and 94 slots (u16 species | form << 11, min and max level) in the
  order grass 12, three flower kinds 12 each, rough terrain 12, surf 5,
  rock smash 5, three rods 3 each, hordes 15. 53 zones have them; Route 2's
  grass holds Bunnelby, Zigzagoon, Fletchling, Pidgey, Scatterbug and Weedle
  at levels 2-4 (species names from the game's text).
- Personal data a/2/1/8 (0x40 bytes; base stats, types, catch rate, growth,
  abilities, base experience), moves a/2/1/2 (0x22 bytes; type, category,
  power, accuracy, PP, priority, critical stage, target, three stat changes
  with stage and chance), learnsets a/2/1/4 (move, level pairs), after
  pk3DS PersonalInfoXY.cs, Move6.cs and Learnset6.cs.
- The type chart: DllBattle.cro at 0xD12A8 (pk3DS TypeChart6.cs), 18 x 18
  bytes 0/2/4/8; the battle module of the game itself.
- Names: a/0/7/4 files 80 (species), 14 (moves), 17 (types).
- HP and stats: px_pokemon_hp_value (0x00168b84) and px_pokemon_stat_value
  with the nature adjustment (0x001689fc, table at code.bin 0x00570f6c), the
  reconstructed host routines, now linked into the viewer.

Policy (inspection choices, stated in model_app.c and game/battle.h)

- Tall grass: tile words 0x2?002004 are walkable in --zone and start
  encounters (Route 2's grass patches are 0x20002004; the 0x21..0x23
  variants occur in other areas). Not the game's collision code.
- An encounter after a completed grass step with the --encounter-rate
  chance; the grass slot by the weights 20, 20, 10, 10, 10, 10, 5, 5, 4, 4,
  1, 1 (community-documented Generation VI odds), the level uniformly in the
  slot's range, IVs 0..31 and the nature uniformly; moves as pk3DS
  GetEncounterMoves.
- Battle rules (game/battle.h): the published Generation VI damage formula,
  critical odds, accuracy and stat stages, turn order, escape odds; status
  moves change stats from their own move data; ailments, multi-hit, recoil,
  healing and other effect sequences are not executed. Not yet reconstructed
  from DllBattle.cro.
- No experience or level-up yet; losing heals the partner where it stands.
- The battle screen is the viewer's own: placeholders coloured by type
  stand where the Pokémon's battle models go. Those are in a/0/0/7 (1.04
  GB), not read yet.

Also in this milestone

- Meshes without texture coordinates (the dummy plane regions around Route
  2) decode with UV (0, 0) (bch_model uv_source).
- Placeholder regions (one untextured flat plane over the cell) are not
  drawn (zone_placeholders line).
- In --zone scenes, meshes whose visible flag is 0 are not drawn (the
  Santalune Forest light shafts' bounding cubes); earlier non-zone views keep
  drawing every mesh.

Independent comparison (verify_battle.py)

cc -std=c11 -O2 native/reproduction/bch-battle/dump_battle.c native/assets/garc.c \
  native/assets/lz.c native/assets/pokedata.c native/game/battle.c \
  reconstructed/pokemon_stats/stats_math.c reconstructed/pokemon_data/*.c \
  reconstructed/pokemon_constructor/*.c -o native/build/dump_battle
  (the pokemon_data and pokemon_constructor sources other than their tests)
python3 -I -B native/reproduction/bch-battle/verify_battle.py --romfs /path/to/romfs \
  --code-bin /path/to/code.bin --bridge native/build/dump_battle \
  --viewer native/build/pokemon_model_viewer --output-dir outputs/new-battle-check

It parses all 799 personal entries, 618 moves, 799 learnsets (12,531 pairs),
every zone's wild section (53 with Pokémon) and the type chart itself:
identical to the native decoders, including the moves known at levels 5 and
50. The reconstructed HP and stat routines agree with the published
formulas on all 9,576 cases (every species, four levels, three IV patterns,
all natures, the nature table read from code.bin). 20,000 random damage
cases agree with an independent implementation of the stated formula. An
independent model of the viewer's random numbers and battle rules predicts
the Route 2 battle of the record exactly: the encounter (Scatterbug, level
3, slot 7), both Pokémon's IVs, natures, stats and moves, all ten battle
messages over three turns and the result (won, partner HP 16/19).

Effect on earlier records: ../bch-route/verify_route.py passes with this
viewer (route-recheck.json): 328 of 360 zones now load (109 multi-region),
up from 291, as meshes without texture coordinates decode; every scene line
still matches its independent derivation, both crossings and the --no-props
captures of ../bch-zone and ../bch-talk reproduce. Its Aquacorde and giant
captures change (hidden meshes and placeholder planes are no longer drawn);
its Route 1 and moonwalk captures are unchanged.

Tests

assets/test_pokedata.c (layouts, the encounter-move rule, rejections, the
chart inside a CRO image), game/test_battle.c (reconstructed stats with and
without a nature table, damage vectors for STAB, critical hits, x4, x0.5 and
immunity, stages, a deterministic battle, PP, running, the menu flow with
cancel), tests/test_bch_battle_viewer.py (a synthetic RomFS with grass, an
encounter table and battle archives: encounters only on grass, seeds,
rate 0, a zone without wild Pokémon, a won battle, running, cancel and the
battle screen), assets/test_bch_model.c (a mesh without texture
coordinate 0).

Limits

One-on-one wild battles only; no items, catching, switching, experience,
ailments or move effects beyond stat changes; the damage, accuracy and
escape formulas are published ones, not yet traced in DllBattle.cro; no
battle models, animations, audio or the game's battle UI. No running-game
comparison.
