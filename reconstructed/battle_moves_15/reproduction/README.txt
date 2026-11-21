Battle move handlers of events 0x15-0x27 (batch 15) reproduction, 2026-11-20

Implemented: 11 DllBattle move handlers of events 0x15, 0x1a, 0x1c, 0x1d,
0x22, 0x25 and 0x27 (see README.md for addresses).

  make -C reconstructed/battle_moves_15 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-8; exporter outputs/rea-moves/export_moves_15.py):

  python3 -I reconstructed/battle_moves_15/reproduction/verify_rea.py

Spit Up/Swallow's handler (108b6af0) and Fling's (108b7ebc) have no Ghidra
function and are listed by the battle_power decoder; the verifier also checks
that decoder against every REA-listed instruction (it refuses none here).

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_15/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_15.py (mutations in mutations_15.py).
