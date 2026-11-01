Battle ability-handler reproduction, 2026-11-01

Implemented: DllBattle handlers 108bf010, 108c0258, 108bca9c, 108bd838,
108bea50, 108c1030, 108bd110, 108c1648, 108c3a64, 1084cd44, 108bd618,
108bd7fc and 108c0f10, and helpers 1084ce84, 108658d4, 108544a0, 1084c124
and 1089e708 (static analysis addresses).

  make -C reconstructed/battle_ability test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-3):

  python3 -I reconstructed/battle_ability/reproduction/verify_rea.py

108c1648 has no Ghidra function: its bytes are checked and the branch targets
of its analyst-decoded listing are recomputed from the instruction words.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_ability/reproduction/check_bindings.py

It confirms the abilities and events bound to each handler in the original
lookup tables. Its output is record-comparison.json.
