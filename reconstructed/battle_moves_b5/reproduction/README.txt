Battle move handlers, event 0xac (fourth group): reproduction, 2026-11-21

Implemented: 14 DllBattle move handlers of event 0xac (see README.md for
addresses).

  make -C reconstructed/battle_moves_b5 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-10 (byte reads)):

  python3 -I reconstructed/battle_moves_b5/reproduction/verify_rea.py

None of the 14 handlers has a Ghidra function; their listings come from reconstructed/battle_power/reproduction/decode_arm.py, which the verifier also checks against every supported REA-listed instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_b5/reproduction/check_bindings.py

Its output is record-comparison.json.

The evidence was exported by outputs/rea-abilities/export_batch.py b5; the
mutations are listed in outputs/rea-abilities/mutations_b5.py and run by
outputs/rea-abilities/mutate.py b5.
