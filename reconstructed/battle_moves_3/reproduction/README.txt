Battle move handlers, fixed damage and type matchups (batch 3) reproduction, 2026-11-07

Implemented: the 20 DllBattle move handlers of events 0x38, 0x43-0x46, 0x4e
and 0x50, the Counter routine 1084b028 and the handler-word setters 1084aef8
and 1084aeec (see README.md for addresses).

  make -C reconstructed/battle_moves_3 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-6; exporter outputs/rea-moves/export_moves_3.py):

  python3 -I reconstructed/battle_moves_3/reproduction/verify_rea.py

Six routines are listed by decode_mult.py (the battle_moves_2 decoder plus
UMULL and BX). The verifier checks the decoder against every supported
REA-listed instruction and its multiply forms against the 30 MUL and UMULL
instructions of sixteen retained reference analyses.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_3/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_3.py (mutations in mutations_3.py).
