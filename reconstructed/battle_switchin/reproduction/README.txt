Battle switch-in reproduction, 2026-11-01

Implemented: 23 of the 28 DllBattle handlers of event 0x94 and the entry
weather setter (see README.md for addresses).

  make -C reconstructed/battle_switchin test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-4):

  python3 -I reconstructed/battle_switchin/reproduction/verify_rea.py

Nine handlers have no Ghidra function; their listings come from
reconstructed/battle_power/reproduction/decode_arm.py, which the verifier also
checks against every supported REA-listed instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_switchin/reproduction/check_bindings.py

Its output is record-comparison.json.
