Battle move handlers, protection breakers and event 0x4d (batch 13) reproduction, 2026-11-15

Implemented: 10 DllBattle move handlers of events 0x2c, 0x32, 0x33, 0x4d and
0x8d (see README.md for addresses).

  make -C reconstructed/battle_moves_13 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-8; exporter outputs/rea-moves/export_moves_13.py):

  python3 -I reconstructed/battle_moves_13/reproduction/verify_rea.py

Feint (108b8a70), Hyperspace Fury (108bb43c), the bypass handler (108b8b58)
and Uproar (108aee4c) have no Ghidra function and are listed by the
battle_power decoder; the verifier also checks that decoder against every
supported REA-listed instruction (it refuses the nine STRB writeback forms,
which are checked against the REA text).

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_13/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_13.py (mutations in mutations_13.py).
