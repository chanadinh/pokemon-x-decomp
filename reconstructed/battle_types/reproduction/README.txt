Battle type-effectiveness reproduction, 2026-10-30

Implemented: DllBattle 10860c80, 10860b80 and 1089e7f0 (static analysis
addresses).

  make -C reconstructed/battle_types test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt):

  python3 -I reconstructed/battle_types/reproduction/verify_rea.py

It checks the instruction words, the three relocated literals (108fca00,
108d12a8, 108d2248), the fourteen relocated jump-table words, and the type
chart (324 bytes) and power table (56 bytes) against the module bytes.

Optional original-data comparison, from the repo root:

  cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror \
    reconstructed/battle_types/reproduction/check_chart.c \
    reconstructed/battle_types/battle_types.c native/assets/pokedata.c \
    -o reconstructed/battle_types/build/check_chart
  reconstructed/battle_types/build/check_chart \
    inputs/pokemon-x-modules-a035d9775a1f/modules/DllBattle.cro

The checker serves the module bytes at their static addresses, compares every
type pair in normal and inverse mode with the native px_type_chart_decode, and
every code combination with the dual-type rule. Its output is
record-comparison.json. No original data is retained here.
