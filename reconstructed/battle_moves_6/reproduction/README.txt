Battle move handlers, two-turn moves (batch 6) reproduction, 2026-11-08

Implemented: 16 DllBattle move handlers of events 0xa0, 0xa1 and 0xa3 (see
README.md for addresses); Sky Drop's two handlers of these events are deferred.

  make -C reconstructed/battle_moves_6 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-7; exporter outputs/rea-moves/export_moves_6.py):

  python3 -I reconstructed/battle_moves_6/reproduction/verify_rea.py

Seven handlers are listed by reconstructed/battle_moves_3/reproduction/decode_mult.py.
The verifier checks the decoder against every supported REA-listed instruction
and its BX form against the bx lr of four retained reference analyses.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_6/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_6.py (mutations in mutations_6.py).
