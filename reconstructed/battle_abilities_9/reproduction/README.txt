Battle hook-abilities reproduction, 2026-11-08

Implemented: 20 DllBattle hook ability handlers (see README.md for
addresses). The hook-word helpers come from battle_moves_3 and the
owner-byte helper from battle_items_4.

  make -C reconstructed/battle_abilities_9 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-7 for the analyses and the byte reads):

  python3 -I reconstructed/battle_abilities_9/reproduction/verify_rea.py

Six handlers have no Ghidra function (1084aee8 falls through into
1084aeec, reconstructed in battle_moves_3); their listings come from
reconstructed/battle_power/reproduction/decode_arm.py, which the verifier also
checks against every supported REA-listed instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_abilities_9/reproduction/check_bindings.py

Its output is record-comparison.json.

The evidence was exported by outputs/rea-abilities/export_batch.py 9; the
mutations are listed in outputs/rea-abilities/mutations_9.py and run by
outputs/rea-abilities/mutate.py 9.
