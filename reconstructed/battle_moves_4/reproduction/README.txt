Battle move handlers, accuracy, added effects and cleanup (batch 4) reproduction, 2026-11-07

Implemented: the 23 DllBattle move handlers of events 0x37, 0x62, 0x7f and
0xa5 (see README.md for addresses).

  make -C reconstructed/battle_moves_4 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-6; exporter outputs/rea-moves/export_moves_4.py):

  python3 -I reconstructed/battle_moves_4/reproduction/verify_rea.py

Four handlers have no Ghidra function; their listings come from
reconstructed/battle_power/reproduction/decode_arm.py, which the verifier also
checks against every supported REA-listed instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_4/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_4.py (mutations in mutations_4.py).
