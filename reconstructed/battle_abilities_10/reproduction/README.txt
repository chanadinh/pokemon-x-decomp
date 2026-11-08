Battle form-abilities reproduction, 2026-11-08

Implemented: 14 DllBattle form ability handlers (see
README.md for addresses).

  make -C reconstructed/battle_abilities_10 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-7 for the analyses and the byte reads):

  python3 -I reconstructed/battle_abilities_10/reproduction/verify_rea.py

Two handlers (108c40b0, 108c4cac) have no Ghidra function; their
listings come from
reconstructed/battle_power/reproduction/decode_arm.py, which the verifier also
checks against every supported REA-listed instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_abilities_10/reproduction/check_bindings.py

Its output is record-comparison.json.

The evidence was exported by outputs/rea-abilities/export_batch.py 10; the
mutations are listed in outputs/rea-abilities/mutations_10.py and run by
outputs/rea-abilities/mutate.py 10.
