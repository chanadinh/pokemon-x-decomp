Battle battler-service reproduction, 2026-11-07

Implemented: nine battler services the kind routines call
(see README.md for addresses).

  make -C reconstructed/battle_services test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA sessions outputs/rea-battle-4 for the analyses and outputs/rea-battle-6
for the byte reads, over the same carrier):

  python3 -I reconstructed/battle_services/reproduction/verify_rea.py

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_services/reproduction/check_services.py

Its output is record-comparison.json.
