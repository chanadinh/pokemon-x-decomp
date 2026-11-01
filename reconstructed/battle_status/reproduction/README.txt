Battle status-prevention reproduction, 2026-11-01

Implemented: the 17 DllBattle handlers of events 0x6e and 0x70 (see
README.md for addresses).

  make -C reconstructed/battle_status test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-4):

  python3 -I reconstructed/battle_status/reproduction/verify_rea.py

108c1cb8, 108c35f8 and 108c51c8 have no Ghidra function; their listings come
from reconstructed/battle_power/reproduction/decode_arm.py, which the
verifier also checks against every supported REA-listed instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_status/reproduction/check_bindings.py

Its output is record-comparison.json.
