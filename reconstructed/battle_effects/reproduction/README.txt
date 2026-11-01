Battle effect-routine reproduction, 2026-11-01

Implemented: the action-processor routines for record kinds 5, 0xe, 0x11, 0x1a, 0x1c and
0x2b (see README.md for addresses).

  make -C reconstructed/battle_effects test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-4):

  python3 -I reconstructed/battle_effects/reproduction/verify_rea.py

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_effects/reproduction/check_effects.py

Its output is record-comparison.json.
