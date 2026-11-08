Battle item handlers, batch 9 reproduction, 2026-11-08

Implemented: 12 DllBattle item handlers for eight items: Custap Berry, Quick
Claw, Lagging Tail, Full Incense, King's Rock, Razor Fang, Focus Band and
Focus Sash. See README.md for addresses.

  make -C reconstructed/battle_items_9 test

The build links battle_items_1 (1084c730, 1084c96c), battle_ability
(108658d4) and battle_event.

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-7; exporter outputs/rea-items/export_items_9.py):

  python3 -I reconstructed/battle_items_9/reproduction/verify_rea.py

Two handlers (108aba18, 108ab354) have no Ghidra function. Their listings
come from reconstructed/battle_power/reproduction/decode_arm.py. The exporter
requires every instruction shape they use to be REA-listed in this batch.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_items_9/reproduction/check_bindings.py

Its output is record-comparison.json.

Mutation testing (31 mutations of battle_items_9.c, each must fail the suite):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_items_9 outputs/rea-items/mutations_9.json
