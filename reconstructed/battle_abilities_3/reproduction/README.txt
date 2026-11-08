Battle end-of-turn-abilities reproduction, 2026-11-07

Implemented: 9 DllBattle ability handlers of events 0x80 and 0x7f (see
README.md for addresses).

  make -C reconstructed/battle_abilities_3 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-6 for the analyses and the byte reads):

  python3 -I reconstructed/battle_abilities_3/reproduction/verify_rea.py

Moody (108bdb4c) has no Ghidra function; its listing comes from
reconstructed/battle_power/reproduction/decode_arm.py, which the verifier also
checks against every supported REA-listed instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_abilities_3/reproduction/check_bindings.py

Its output is record-comparison.json.

The evidence was exported by outputs/rea-abilities/export_batch.py 3; the
mutations are listed in outputs/rea-abilities/mutations_3.py and run by
outputs/rea-abilities/mutate.py 3.
