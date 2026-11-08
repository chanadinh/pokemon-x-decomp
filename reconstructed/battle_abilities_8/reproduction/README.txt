Battle item-abilities reproduction, 2026-11-08

Implemented: 9 DllBattle item and after-move ability handlers (see
README.md for addresses).

  make -C reconstructed/battle_abilities_8 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-7 for the analyses and the byte reads):

  python3 -I reconstructed/battle_abilities_8/reproduction/verify_rea.py

The Sticky Hold message handler (108c3bb8) has no Ghidra function; its
listing comes from
reconstructed/battle_power/reproduction/decode_arm.py, which the verifier also
checks against every supported REA-listed instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_abilities_8/reproduction/check_bindings.py

Its output is record-comparison.json.

The evidence was exported by outputs/rea-abilities/export_batch.py 8; the
mutations are listed in outputs/rea-abilities/mutations_8.py and run by
outputs/rea-abilities/mutate.py 8.
