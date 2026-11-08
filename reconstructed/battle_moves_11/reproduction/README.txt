Battle move handlers, moves that call other moves (batch 11) reproduction, 2026-11-08

Implemented: the 10 DllBattle move handlers of events 0x18 and 0x19 (see
README.md for addresses).

  make -C reconstructed/battle_moves_11 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-7; exporter outputs/rea-moves/export_moves_11.py):

  python3 -I reconstructed/battle_moves_11/reproduction/verify_rea.py

Two handlers have no Ghidra function; their listings come from
reconstructed/battle_power/reproduction/decode_arm.py, which the verifier also
checks against every supported REA-listed instruction. Nature Power's two jump
tables are verified as relocated literal words.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_11/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_11.py (mutations in mutations_11.py).
