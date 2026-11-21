Battle move handlers of event 0xac, lower half (batch 19) reproduction, 2026-11-21

Implemented: 11 DllBattle move handlers of event 0xac (the lower half of the
sorted list of the last 21; see README.md for the list and the split).

  make -C reconstructed/battle_moves_19 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-10; exporter outputs/rea-moves/export_moves_19.py):

  python3 -I reconstructed/battle_moves_19/reproduction/verify_rea.py

Ten handlers have no Ghidra function and are listed by the battle_power
decoder (only Psych Up, 108b0468, has one); the verifier also checks that
decoder against every REA-listed instruction. Camouflage's two jump tables
(14 words) are ABS32 relocations, verified word by word.

Original-data check and split check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_19/reproduction/check_bindings.py

Its output is record-comparison.json; it recomputes the sorted list without the three split participants (this
milestone, battle_moves_b6, battle_moves_b7), asserts the upper half is exactly
b6's and b7's move-table handlers, and that with all three every move-table
handler is covered. Mutation testing:
outputs/rea-moves/mutate_19.py (mutations in mutations_19.py).
