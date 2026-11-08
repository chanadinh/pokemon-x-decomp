Battle move-calculation-abilities reproduction, 2026-11-07

Implemented: 27 DllBattle move-calculation ability handlers (see
README.md for addresses).

  make -C reconstructed/battle_abilities_4 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-6 for the analyses and the byte reads):

  python3 -I reconstructed/battle_abilities_4/reproduction/verify_rea.py

Three handlers (108bf910, 108c3cd0, 108c551c) have no Ghidra function; their
listings come from
reconstructed/battle_power/reproduction/decode_arm.py, which the verifier also
checks against every supported REA-listed instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_abilities_4/reproduction/check_bindings.py

Its output is record-comparison.json.

The evidence was exported by outputs/rea-abilities/export_batch.py 4; the
mutations are listed in outputs/rea-abilities/mutations_4.py and run by
outputs/rea-abilities/mutate.py 4.
