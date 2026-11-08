Battle move handlers, move failure (batch 8) reproduction, 2026-11-08

Implemented: the 11 DllBattle move handlers of event 0x2e (see README.md for
addresses).

  make -C reconstructed/battle_moves_8 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-7; exporter outputs/rea-moves/export_moves_8.py):

  python3 -I reconstructed/battle_moves_8/reproduction/verify_rea.py

Two handlers have no Ghidra function; their listings come from
reconstructed/battle_power/reproduction/decode_arm.py, which the verifier also
checks against every supported REA-listed instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_8/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_8.py (mutations in mutations_8.py).
