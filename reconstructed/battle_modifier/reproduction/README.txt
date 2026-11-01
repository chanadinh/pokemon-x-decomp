Battle damage-modifier reproduction, 2026-11-01

Implemented: the 30 DllBattle handlers of event 0x4f and four helpers (see
README.md for addresses).

  make -C reconstructed/battle_modifier test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-4):

  python3 -I reconstructed/battle_modifier/reproduction/verify_rea.py

108a68e4 and 108bf300 have no Ghidra function; their listings come from
reconstructed/battle_power/reproduction/decode_arm.py, which the verifier
also checks against every supported REA-listed instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_modifier/reproduction/check_bindings.py

Its output is record-comparison.json.
