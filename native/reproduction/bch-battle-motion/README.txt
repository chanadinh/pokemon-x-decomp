Battle motions: Route 2, 2026-10-24

The Pokémon move. Each battle model now plays its own motions from the
game: an idle loop, an attack when it uses a move, a flinch when it is hit,
and a fall when it faints. Tracked files contain independently authored
source, synthetic tests and metadata only; the documentation GIF is
recorded in docs/images/README.md.

Native preview

native/build/pokemon_model_viewer --romfs /path/to/romfs --zone 259
A single motion: the model command of ../bch-pokemon plus
  --motion-entry 9+8m --motion-section 0 --motion SLOT [--motion-frame N]
  (new option: the motion pack's entry in the model's archive).

What comes from game data

- a/0/0/7 entry 9 + 8m (model m, +5) is a PB pack whose section 0 is a GF1
  motion pack, the format of the player's motions (../bch-motion), decoded
  by the existing assets/gf_motion.c: 29 slots, slot 0 the motion skeleton.
  972 of the 981 models have one; nine are empty (models 313, 898..901: a
  Mega Houndoom and Flabébé's colour forms, and the four empty models of
  ../bch-pokemon). Every species' first model has motions.
- Slot occupancy over the 972 packs: 1: 972, 2: 490, 4/5/6: 661 each,
  7: 309, 9: 901, 13: 951, 17: 971, 18: 921, 26: 466, others rarer
  (reference-comparison.json).
- The other PB sections (BCH material and visibility animations, SESD
  sound records) and the PF and PK packs are not read.

Policy (inspection choices, stated in model_app.c and game/battle.h)

- Slot roles, inferred from viewing Froakie's motions and from the
  occupancy (not traced in the game's battle code): 1 idle (looping),
  4 physical move, 5 special move, 6 status move, 17 damage reaction,
  18 fainting. One motion frame per two rendered frames, as in the field.
- Triggers: the battle log now marks each message (battle.h log_event):
  "X used M!" starts X's move motion; when the move damaged, the target's
  reaction starts when that motion ends; "X fainted!" plays the faint motion
  and holds its last frame. A fainted Pokémon stays on the stage until its
  fainting page has passed (before, it vanished at once). Motions do not
  delay the messages.
- Models are placed by their bounds in the first idle frame.
- A model whose pack does not decode, or lacks slot 1, stands in the rest
  pose (pose=rest); a missing slot leaves the idle loop playing.

Independent comparison (verify_battle_motion.py)

cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror -D_DEFAULT_SOURCE \
  native/assets/bch_model.c native/assets/gf_motion.c native/render/skin.c \
  native/reproduction/bch-motion/dump_motion.c -lm -o native/build/dump_motion
python3 -I -B native/reproduction/bch-battle-motion/verify_battle_motion.py \
  --romfs /path/to/romfs --bridge native/build/dump_motion \
  --viewer native/build/pokemon_model_viewer --out outputs/new-battle-motion-check

All 972 motion packs parsed by the independent GF1 reader of ../bch-motion
are identical to the native decoder (skeletons, frame counts, byte ranges,
channel kinds, 8,899,892 keys). For Froakie and the six Route 2 species, in
the six played slots, every channel value at every frame is bit-identical,
the bone locals agree exactly, and every vertex of the first, middle and
last frames (126 poses) agrees exactly. The viewer's battle_model lines on
Route 2 list the derived slots and frame counts, and the scripted battle
still ends as recorded (won, three turns).

Effect on earlier records: ../bch-pokemon/verify_pokemon.py step 3 now
differs by design (battle_model lines end with the motion fields and
pose=idle instead of pose=rest); its steps 1 and 2 are unaffected.
../bch-catch/verify_catch.py passes.

Tests

game/test_battle.c (log events: a status move, a damaging move flagged as
hitting, the faint of the loser, the UI's page events),
tests/test_bch_battle_viewer.py (a synthetic PB pack with the zone
fixture's GF1 motions: pose=idle, the slot list, a battle that still ends).

Limits

The slot roles and triggers are inferred; the game's battle sequencing,
camera moves, effects, the appearance motion (perhaps slot 2 or 9) and the
material animations (blinking) are not reproduced. Messages do not wait for
motions. No running-game comparison.
