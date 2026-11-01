Battle restore-routine reproduction, 2026-11-01

Implemented: the action-processor routines for record kinds 4, 8, 0x23 and
0x34 (see README.md for addresses).

  make -C reconstructed/battle_restores test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-4):

  python3 -I reconstructed/battle_restores/reproduction/verify_rea.py

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_restores/reproduction/check_restores.py

Its output is record-comparison.json.
