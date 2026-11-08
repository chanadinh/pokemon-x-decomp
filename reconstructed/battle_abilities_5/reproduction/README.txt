Battle redirect-absorb-abilities reproduction, 2026-11-07

Implemented: 15 DllBattle redirect, absorb and blocking ability handlers (see
README.md for addresses).

  make -C reconstructed/battle_abilities_5 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-7 for the analyses and the byte reads):

  python3 -I reconstructed/battle_abilities_5/reproduction/verify_rea.py

Two handlers (108c5270, 108c4a88) have no Ghidra function; their
listings come from
reconstructed/battle_power/reproduction/decode_arm.py, which the verifier also
checks against every supported REA-listed instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_abilities_5/reproduction/check_bindings.py

Its output is record-comparison.json.

The evidence was exported by outputs/rea-abilities/export_batch.py 5; the
mutations are listed in outputs/rea-abilities/mutations_5.py and run by
outputs/rea-abilities/mutate.py 5.
