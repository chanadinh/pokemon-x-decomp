Battle move handlers of events 0x2a-0x53 (batch 16) reproduction, 2026-11-21

Implemented: 19 DllBattle move handlers of events 0x2a, 0x2b, 0x35, 0x36,
0x39, 0x3a, 0x42, 0x48 and 0x53 (see README.md for addresses).

  make -C reconstructed/battle_moves_16 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA sessions outputs/rea-battle-8 and -9; exporter outputs/rea-moves/export_moves_16.py):

  python3 -I reconstructed/battle_moves_16/reproduction/verify_rea.py

The three event 0x2a thunks (1084b458, 108b8404, 108b8f78; the shared body
1084b46c is retained with the first) and 108ba004, 108ba3cc and 108b847c
have no Ghidra function and are listed by the battle_power decoder; the
verifier also checks that decoder against every REA-listed instruction (it
refuses none here).

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_16/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_16.py (mutations in mutations_16.py).
