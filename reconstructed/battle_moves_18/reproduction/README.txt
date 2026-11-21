Battle move handlers of events 0x7d-0x9f (batch 18) reproduction, 2026-11-21

Implemented: 13 DllBattle move handlers of events 0x7d, 0x80, 0x8a, 0x8e,
0x8f, 0x92, 0x99, 0x9e and 0x9f (see README.md for addresses).

  make -C reconstructed/battle_moves_18 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-10; exporter outputs/rea-moves/export_moves_18.py):

  python3 -I reconstructed/battle_moves_18/reproduction/verify_rea.py

Three handlers (108af958, Uproar 108b8590 and the pledges 108b9244) have no
Ghidra function and are listed by the battle_power decoder; the verifier also
checks that decoder against every REA-listed instruction (it refuses none
here).

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_18/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_18.py (mutations in mutations_18.py).
