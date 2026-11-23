Battle core helpers (batch 5) reproduction, 2026-11-23

Implemented: 22 DllBattle.cro functions in 10840000-10880000 and
108a6000-108ce000: random and item-data wrappers, battle-state accessors,
struct initializers, list inserts, a scaled-HP event builder and the
handler-blocking test 108c59b4. See README.md.

  make -C reconstructed/battle_core_5 test

The build links battle_core_4, battle_core_3, battle_core_2, battle_core_1,
battle_tables_1, battle_handler and their event and ability dependencies.

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-11; exporter outputs/rea-items/export_core_5.py):

  python3 -I reconstructed/battle_core_5/reproduction/verify_rea.py

All 22 functions are REA-listed with Ghidra pseudocode.

Overlap and connection check (reads every other milestone's evidence, on disk):

  python3 -I reconstructed/battle_core_5/reproduction/check_bindings.py

Its output is record-comparison.json. Frontier listing used to choose the batch:

  python3 -I outputs/rea-items/frontier.py --range 10840000-10880000 --range 108a6000-108ce000

Mutation testing (79 mutations of battle_core_5.c, each must fail the suite; a
hanging mutant counts as detected):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_core_5 outputs/rea-items/mutations_core_5.json
