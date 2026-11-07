Battle status-applier reproduction, 2026-11-07

Implemented: the status applier 108629b8
(see README.md for addresses).

  make -C reconstructed/battle_conditions test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA sessions outputs/rea-battle-4 for the analyses and outputs/rea-battle-6
for the byte reads, over the same carrier):

  python3 -I reconstructed/battle_conditions/reproduction/verify_rea.py

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_conditions/reproduction/check_conditions.py

Its output is record-comparison.json.
