Battle move handlers, event 0xac (fifth group, upper half): reproduction, 2026-11-21

Implemented: 8 DllBattle move handlers of event 0xac (see README.md for
addresses).

  make -C reconstructed/battle_moves_b6 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA sessions outputs/rea-battle-9 (analyses) and rea-battle-10 (byte reads)):

  python3 -I reconstructed/battle_moves_b6/reproduction/verify_rea.py

Four handlers (108b2b2c, 108b3458, 108b40fc, 108b50c8) have no Ghidra function;
their listings come from reconstructed/battle_power/reproduction/decode_arm.py,
which the verifier also checks against every supported REA-listed instruction.
The literal at 108b4930 is an ABS32 relocation to the Acupressure stat table at
108d2c46, whose seven bytes the original-data check compares.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_b6/reproduction/check_bindings.py

Its output is record-comparison.json.

The evidence was exported by outputs/rea-abilities/export_batch.py b6; the
mutations are listed in outputs/rea-abilities/mutations_b6.py and run by
outputs/rea-abilities/mutate.py b6. The sorted list of unreconstructed
event-0xac handlers and its split point come from
outputs/rea-abilities/split_list.py.
