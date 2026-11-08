Battle weather-abilities reproduction, 2026-11-07

Implemented: 17 DllBattle weather ability handlers (events 0x88, 0x13,
0x39, 0x40, 0x7f, 0x0e, 0x5e and 0x83; see README.md for addresses).

  make -C reconstructed/battle_abilities_2 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-6 for the analyses and the byte reads):

  python3 -I reconstructed/battle_abilities_2/reproduction/verify_rea.py

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_abilities_2/reproduction/check_bindings.py

Its output is record-comparison.json.

The evidence was exported by outputs/rea-abilities/export_batch.py 2; the
mutations are listed in outputs/rea-abilities/mutations_2.py and run by
outputs/rea-abilities/mutate.py 2.

Four handlers have no Ghidra function (108bfb58, 108c02f4, 108c2ee0,
108c51bc); their listings come from
reconstructed/battle_power/reproduction/decode_arm.py, which the verifier also
checks against every supported REA-listed instruction.
