Battle cure-routine reproduction, 2026-11-01

Implemented: the action-processor routines for record kinds 0xa, 0x21, 0x25, 0x28 and
0x36 (see README.md for addresses).

  make -C reconstructed/battle_cures test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-4):

  python3 -I reconstructed/battle_cures/reproduction/verify_rea.py

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_cures/reproduction/check_cures.py

Its output is record-comparison.json.
