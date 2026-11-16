Battle move handlers, events 0xae-0xb4: reproduction, 2026-11-15

Implemented: 5 DllBattle move handlers of events 0xb4, 0xb3, 0xb1 and 0xae
(see README.md for addresses).

  make -C reconstructed/battle_moves_b1 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-8 for the analyses and the byte reads):

  python3 -I reconstructed/battle_moves_b1/reproduction/verify_rea.py

Three handlers (108adce8, 108baa30, 108b93a4) have no Ghidra function; their
listings come from reconstructed/battle_power/reproduction/decode_arm.py,
which the verifier also checks against every supported REA-listed
instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_b1/reproduction/check_bindings.py

Its output is record-comparison.json.

The evidence was exported by outputs/rea-abilities/export_batch.py b1; the
mutations are listed in outputs/rea-abilities/mutations_b1.py and run by
outputs/rea-abilities/mutate.py b1.
