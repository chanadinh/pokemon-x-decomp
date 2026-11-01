Battle action-processor reproduction, 2026-11-01

Implemented: the DllBattle action processor 10819600 (see README.md).

  make -C reconstructed/battle_process test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-4):

  python3 -I reconstructed/battle_process/reproduction/verify_rea.py

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_process/reproduction/check_cases.py

Its output is record-comparison.json.
