Battle move handlers, field and side conditions (batch 5) reproduction, 2026-11-08

Implemented: the 27 DllBattle move handlers of events 0xaa and 0xad, the
side-condition setter 1084bcb0 and the turn-count encoder 1084be3c (see
README.md for addresses).

  make -C reconstructed/battle_moves_5 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-7; exporter outputs/rea-moves/export_moves_5.py):

  python3 -I reconstructed/battle_moves_5/reproduction/verify_rea.py

Six handlers are listed by decode_store.py (the battle_moves_3 decoder plus
STR pre-indexed with writeback). The verifier checks the decoder against
every supported REA-listed instruction and that form against the six
instructions of the retained reference analyses.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_5/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_5.py (mutations in mutations_5.py).
