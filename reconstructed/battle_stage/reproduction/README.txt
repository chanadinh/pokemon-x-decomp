Battle stat-stage reproduction, 2026-11-01

Implemented: the 14 DllBattle handlers of events 0x63-0x66 (see README.md
for addresses).

  make -C reconstructed/battle_stage test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-4):

  python3 -I reconstructed/battle_stage/reproduction/verify_rea.py

108bef24, 108c1ea0, 108c2740, 108c2790, 108c2c3c and 108c48fc have no Ghidra
function; their listings come from
reconstructed/battle_power/reproduction/decode_arm.py, which the verifier
also checks against every supported REA-listed instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_stage/reproduction/check_bindings.py

Its output is record-comparison.json.
