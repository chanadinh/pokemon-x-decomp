Battle kind-routine reproduction, 2026-11-01

Implemented: the action-processor routines for record kinds 0xd, 0x1e and
0x20 (see README.md for addresses).

  make -C reconstructed/battle_kinds test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-4):

  python3 -I reconstructed/battle_kinds/reproduction/verify_rea.py

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_kinds/reproduction/check_routes.py

Its output is record-comparison.json.
