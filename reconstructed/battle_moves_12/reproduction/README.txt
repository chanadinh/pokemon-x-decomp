Battle move handlers, Sky Drop and lock releases (batch 12) reproduction, 2026-11-15

Implemented: 11 DllBattle move handlers of events 0x21, 0x23, 0x26, 0x91, 0xa0,
0xa3 and 0xb6 (see README.md for addresses).

  make -C reconstructed/battle_moves_12 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-7; exporter outputs/rea-moves/export_moves_12.py):

  python3 -I reconstructed/battle_moves_12/reproduction/verify_rea.py

All handlers have Ghidra functions; the verifier also checks the battle_power
decoder against every supported REA-listed instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_12/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_12.py (mutations in mutations_12.py).
