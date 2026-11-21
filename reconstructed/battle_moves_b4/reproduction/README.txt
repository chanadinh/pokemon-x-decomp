Battle move handlers, event 0xac (third group): reproduction, 2026-11-21

Implemented: 10 DllBattle move handlers of event 0xac (see README.md for
addresses).

  make -C reconstructed/battle_moves_b4 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA sessions outputs/rea-battle-9 (analyses) and rea-battle-10 (byte reads)):

  python3 -I reconstructed/battle_moves_b4/reproduction/verify_rea.py

All 10 handlers are Ghidra functions.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_b4/reproduction/check_bindings.py

Its output is record-comparison.json.

The evidence was exported by outputs/rea-abilities/export_batch.py b4; the
mutations are listed in outputs/rea-abilities/mutations_b4.py and run by
outputs/rea-abilities/mutate.py b4.
