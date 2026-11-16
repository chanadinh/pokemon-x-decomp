Battle move handlers of events 0x01-0x0e (batch 14) reproduction, 2026-11-15

Implemented: 16 DllBattle move handlers of events 0x01, 0x03, 0x04, 0x06,
0x07, 0x08, 0x09, 0x0d and 0x0e (see README.md for addresses).

  make -C reconstructed/battle_moves_14 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-8; exporter outputs/rea-moves/export_moves_14.py):

  python3 -I reconstructed/battle_moves_14/reproduction/verify_rea.py

Doom Desire's scheduler (108b6ca4), Future Sight's message (108b8418) and
Uproar's failure check (108ba778) have no Ghidra function and are listed by
battle_moves_3's decode_mult.py; the verifier checks that decoder against
every supported REA-listed instruction and its BX form against four retained
references. The protect-move table (108d4b28) is a data block.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_14/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_14.py (mutations in mutations_14.py).
