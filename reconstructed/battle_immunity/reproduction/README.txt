Battle immunity reproduction, 2026-11-01

Implemented: the 17 DllBattle handlers of event 0x31 and the absorb test
(see README.md for addresses).

  make -C reconstructed/battle_immunity test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-4):

  python3 -I reconstructed/battle_immunity/reproduction/verify_rea.py

108be0c8, 108c1d5c, 108c3888, 108c47b0 and 108c54dc have no Ghidra function;
their listings come from reconstructed/battle_power/reproduction/decode_arm.py,
which the verifier also checks against every supported REA-listed
instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_immunity/reproduction/check_bindings.py

Its output is record-comparison.json.
