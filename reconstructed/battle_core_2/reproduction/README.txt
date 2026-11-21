Battle core helpers (batch 2) reproduction, 2026-11-21

Implemented: 21 small DllBattle.cro functions in 10840000-10880000 and
108a6000-108ce000 that committed milestones already call. See README.md.

  make -C reconstructed/battle_core_2 test

The build links battle_core_1, battle_tables_1, battle_handler and their
event and ability dependencies.

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-10; exporter outputs/rea-items/export_core_2.py):

  python3 -I reconstructed/battle_core_2/reproduction/verify_rea.py

All 21 functions are REA-listed with Ghidra pseudocode. The decoder of
reconstructed/battle_power/reproduction/decode_arm.py reproduces every
listing it supports; its refusals keep REA's text.

Overlap and connection check (reads every other milestone's evidence, on disk):

  python3 -I reconstructed/battle_core_2/reproduction/check_bindings.py

Its output is record-comparison.json.

Frontier listing used to choose the batch (outputs/rea-items/frontier.py):

  python3 -I outputs/rea-items/frontier.py --range 10840000-10880000 --range 108a6000-108ce000

Mutation testing (85 mutations of battle_core_2.c, each must fail the suite; a
hanging mutant counts as detected):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_core_2 outputs/rea-items/mutations_core_2.json
