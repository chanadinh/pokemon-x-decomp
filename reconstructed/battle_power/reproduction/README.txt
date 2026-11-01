Battle move-power handler reproduction, 2026-11-01

Implemented: the nineteen DllBattle ability handlers of event 0x3d (see
README.md for addresses).

  make -C reconstructed/battle_power test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-4):

  python3 -I reconstructed/battle_power/reproduction/verify_rea.py

108be18c, 108bf060, 108c15a8 and 108c15f8 have no Ghidra function; their
listings come from decode_arm.py, which the verifier also checks against
every REA-listed instruction of the other handlers.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_power/reproduction/check_bindings.py

Its output is record-comparison.json.
