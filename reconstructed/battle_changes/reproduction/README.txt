Battle change-routine reproduction, 2026-11-01

Implemented: the action-processor routines for record kinds 6, 7, 0xb and
0x13 (see README.md for addresses).

  make -C reconstructed/battle_changes test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-4):

  python3 -I reconstructed/battle_changes/reproduction/verify_rea.py

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_changes/reproduction/check_changes.py

Its output is record-comparison.json.
