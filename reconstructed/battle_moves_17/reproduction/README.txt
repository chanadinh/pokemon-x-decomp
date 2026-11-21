Battle move handlers of events 0x54-0x6d (batch 17) reproduction, 2026-11-21

Implemented: 13 DllBattle move handlers of events 0x54, 0x55, 0x58, 0x5b,
0x67, 0x69, 0x6a, 0x6b and 0x6d (see README.md for addresses).

  make -C reconstructed/battle_moves_17 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-10; exporter outputs/rea-moves/export_moves_17.py):

  python3 -I reconstructed/battle_moves_17/reproduction/verify_rea.py

Tri Attack's handler (108b30fc) has no Ghidra function and is listed by the
battle_power decoder; the verifier also checks that decoder against every
supported REA-listed instruction. Tri Attack's table (108d2c34) is a data
block, and its literal 108b3160 an ABS32 relocation.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_17/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_17.py (mutations in mutations_17.py).
