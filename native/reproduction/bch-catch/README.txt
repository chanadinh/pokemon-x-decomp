Experience, level-ups and catching: Route 2, 2026-10-24

Battles now pay out. A win gives the partner experience from the game's
growth tables, it levels up through the reconstructed level-from-experience
routine and learns the moves its learnset lists at the new level; the BALL
command throws a Poké Ball, and a caught Pokémon joins a party of up to six.
Tracked files contain independently authored source, synthetic tests and
metadata only; the documentation PNG is recorded in docs/images/README.md.

Native preview

native/build/pokemon_model_viewer --romfs /path/to/romfs --zone 259 \
  --code-bin /path/to/code.bin

The command menu is FIGHT / BALL xN / RUN. New option: --balls N (0..999,
default 10).

What comes from game data

- Growth tables: a/2/1/7, eight members of 101 u32 (total experience at
  levels 0..100); the group is personal byte 0x15 (pk3DS PersonalInfoXY.cs).
  Groups 0..5 are medium fast, erratic, fluctuating, medium slow, fast and
  slow; members 6 and 7 hold copies of group 0.
- Base experience (personal 0x22) and catch rate (personal 0x08).
- The level for an amount of experience: px_pokemon_level_from_growth
  (reconstructed/pokemon_stats), now linked into the viewer and reading the
  loaded tables through its data interface.
- Learnsets a/2/1/4 for every species, loaded once: moves listed at a new
  level are learned into an empty slot, else not learned (no forgetting
  prompt yet).

Policy (inspection choices, stated in game/battle.h)

- Experience for a defeat: floor(base_exp x level / 7), the published
  Generation VI wild formula without trainer, item or Exp. Share factors.
- Catching: a = floor((3M - 2H) x rate x ball / 3M) with the Poké Ball's
  multiplier 1 (ball10 = 10); a >= 255 catches, else four shake checks
  against floor(65536 / (255 / a)^0.1875) with the battle's own generator;
  broke-free messages by the number of shakes. No status bonus, no critical
  capture. A failed throw uses the turn: the wild Pokémon moves.
- Party: party[0] fights; a catch joins while fewer than six are held
  (no PC boxes yet). Losing still heals the lead where it stands.

Independent comparison (verify_catch.py)

cc -std=c11 -O2 native/reproduction/bch-catch/dump_catch.c native/assets/garc.c \
  native/assets/lz.c native/assets/pokedata.c native/game/battle.c \
  reconstructed/pokemon_stats/stats_math.c reconstructed/pokemon_data/*.c \
  reconstructed/pokemon_constructor/*.c -lm -o native/build/dump_catch
  (the pokemon_data and pokemon_constructor sources other than their tests)
python3 -I -B native/reproduction/bch-catch/verify_catch.py --romfs /path/to/romfs \
  --code-bin /path/to/code.bin --bridge native/build/dump_catch \
  --viewer native/build/pokemon_model_viewer --out outputs/new-catch-check

All eight growth tables read independently are identical to the loaded ones
and to the published formulas at every level. 266,514 cases agree: 239,400
level lookups through the reconstructed routine (every species, every level
boundary, one below and one above, past level 100), 3,192 starting
experience values, 4,788 experience awards and 19,134 catch thresholds
(1,061 certain catches). On Route 2 the viewer's won battle prints the
experience and level that follow from the tables (Froakie 135 + 17 = 152,
still level 5); the catch run throws two balls, catches the level 3
Scatterbug and shows it as the second party member, with eight balls left
(reference-comparison.json).

Effect on earlier records: ../bch-battle/verify_battle.py steps 1-3 pass
with this code (its unchanged dump_battle.c now links with -lm, as
game/battle.c uses pow); its step 4 now differs by design: the Route 2 battle adds
the line "Froakie gained 17 Exp. Points!" and battle_end carries level, exp
and balls. Every other line of that battle is unchanged. The field is
unchanged.

Tests

game/test_battle.c (growth lookups, level boundaries, experience, a level-up
that learns a move, catch thresholds, a battle ending in a catch, the
no-ball page), tests/test_bch_battle_viewer.py (a synthetic growth archive:
experience and a level-up in the display loop, a catch joining the party,
--balls 0 and its message, --balls range).

Limits

No item bag, other balls, switching, PC boxes, evolution, move-forgetting,
Exp. Share or EVs; the experience and catch formulas are published ones, not
yet traced in DllBattle.cro. No running-game comparison.
