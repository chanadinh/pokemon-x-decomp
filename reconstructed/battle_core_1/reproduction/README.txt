Battle core helpers (batch 1) reproduction, 2026-11-21

Implemented: 28 small DllBattle.cro functions (accessors, bit-field helpers,
a type query, an event runner and an action-record builder) that earlier
milestones call but never reconstructed. See README.md for the list.

  make -C reconstructed/battle_core_1 test

The build links battle_items_1, battle_modifier, battle_ability, battle_event
and battle_abilities_1 (the event stack and the record callbacks' shape).

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-10; exporter outputs/rea-items/export_core_1.py):

  python3 -I reconstructed/battle_core_1/reproduction/verify_rea.py

All 28 functions are REA-listed with Ghidra pseudocode. The decoder of
reconstructed/battle_power/reproduction/decode_arm.py reproduces every
listing it supports; its refusals keep REA's text.

Connection check (reads the other milestones' evidence):

  python3 -I reconstructed/battle_core_1/reproduction/check_bindings.py

Its output is record-comparison.json.

Mutation testing (82 mutations of battle_core_1.c, each must fail the suite):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_core_1 outputs/rea-items/mutations_core_1.json
