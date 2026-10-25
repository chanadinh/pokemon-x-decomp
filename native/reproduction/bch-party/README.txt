The whole party in battle: Route 2, 2026-10-25

The party now fights together. The command menu gains POKéMON: the party
list switches the active Pokémon (it costs the turn, and the wild Pokémon
then moves). When the active one faints and another can still fight, the
list opens and one must be sent out; the battle is lost only when none can
fight ("You have no more Pokémon that can fight!"). Battles start with the
first party member that can fight. Tracked files contain independently
authored source, synthetic tests and metadata only; the documentation PNG is
recorded in docs/images/README.md.

In the viewer

FIGHT / BALL / POKéMON / RUN (RUN moved one place right). In the list,
up/down choose, Space confirms, X goes back (not after a faint). The list
refuses the Pokémon already in battle and fainted ones, with a note. The
sent-out Pokémon's battle model is loaded when "Go, Y!" is shown.

Policy (game/battle.h; the published Generation VI rules in outline, not
traced in DllBattle.cro)

- A switch is the player's action for the turn: "Come back, X!", "Go, Y!",
  then the wild Pokémon's move. After a faint the replacement comes in
  without a turn.
- Experience goes to the Pokémon in battle when the wild one faints (no
  sharing among participants).
- Stat stages belong to the battle and are dropped on switching out.
- After the battle every member keeps its HP, PP, experience and level;
  losing heals the whole party where the player stands (no Pokémon Center).

Independent comparison (verify_party.py)

python3 -I -B native/reproduction/bch-party/verify_party.py --romfs /path/to/romfs \
  --code-bin /path/to/code.bin --viewer native/build/pokemon_model_viewer \
  --out outputs/new-party-check

A catch and a save make a party of Froakie and Scatterbug (read back with
../bch-save's independent reader). Loaded, one step into the grass meets a
level 2 Bunnelby; the party list sends out Scatterbug. From the saved random
state alone, the independent model of ../bch-battle predicts the encounter,
the battle seed and every message of the switch turn ("Come back, Froakie!",
"Go, Scatterbug!", "The wild Bunnelby used Tackle!"; Scatterbug 15 -> 12 HP,
as the battle screen shows); the viewer prints exactly those and loads
Scatterbug's model at "Go, Scatterbug!" (reference-comparison.json).

Effect on earlier records: none of their scripted inputs reach RUN; the
synthetic RUN tests now press right three times. ../bch-catch and ../bch-save
pass with this viewer.

Tests

game/test_battle.c (party setup and refusals, a switch through the UI with
its pages and the send-out event, cancel, a forced replacement without a
turn, losing with the last one), tests/test_bch_save_viewer.py (a loaded
two-Pokémon party switches in the display loop),
tests/test_bch_battle_viewer.py (RUN in its new place).

Limits

No items in battle, no trainer battles, no shift prompt, no experience
sharing, no PC when the party is full. No running-game comparison.
