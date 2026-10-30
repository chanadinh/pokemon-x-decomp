Battle type-matchup reproduction, 2026-10-30

Implemented: DllBattle 10870994, 1086094c, 10826264 and 10863d44 (static
analysis addresses). The chart lookup and combination come from
reconstructed/battle_types.

  make -C reconstructed/battle_matchup test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt):

  python3 -I reconstructed/battle_matchup/reproduction/verify_rea.py

It checks every instruction word and the two literals (0xffff0000 and
0x437c) against the module bytes; no relocations apply in these ranges.
