Battle action-service reproduction, 2026-11-01

Implemented: the DllBattle action stack, commit, ability popup, message
builders and temporary buffer (see README.md for addresses).

  make -C reconstructed/battle_action test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-4):

  python3 -I reconstructed/battle_action/reproduction/verify_rea.py

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_action/reproduction/check_sizes.py

Its output is record-comparison.json.
