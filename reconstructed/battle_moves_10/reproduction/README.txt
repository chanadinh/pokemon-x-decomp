Battle move handlers, the check before a move (batch 10) reproduction, 2026-11-08

Implemented: the 20 DllBattle move handlers of event 0x1f and the shared
Counter check 1084b2d8 (see README.md for addresses).

  make -C reconstructed/battle_moves_10 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-7; exporter outputs/rea-moves/export_moves_10.py):

  python3 -I reconstructed/battle_moves_10/reproduction/verify_rea.py

Four routines have no Ghidra function; their listings come from
reconstructed/battle_power/reproduction/decode_arm.py, which the verifier also
checks against every supported REA-listed instruction. The Protect odds table
is verified as a data block.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_10/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_10.py (mutations in mutations_10.py).
