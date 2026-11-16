Battle item handlers, batch 12 reproduction, 2026-11-15

Implemented: 13 DllBattle item handlers (every entry of Leppa Berry 154 and
Micle Berry 209) and the helpers 108a9e1c, 1084c278 and 1084c224. See
README.md for addresses.

  make -C reconstructed/battle_items_12 test

The build links battle_items_1 (1084c730, 1084c96c), battle_ability
(108658d4) and battle_event.

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-8; exporter outputs/rea-items/export_items_12.py):

  python3 -I reconstructed/battle_items_12/reproduction/verify_rea.py

Two handlers (108a9dfc, 108a871c) have no Ghidra function. Their listings
come from reconstructed/battle_power/reproduction/decode_arm.py, with a
bx<cond> rule for 108a9dfc's bxeq lr. The exporter requires every
instruction shape they use to be REA-listed in this batch or at a spelling
reference (108695c4, 108a7584, 108a7594). The verifier checks each reference
against REA's listing and the live word.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_items_12/reproduction/check_bindings.py

Its output is record-comparison.json.

Mutation testing (35 mutations of battle_items_12.c, each must fail the suite):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_items_12 outputs/rea-items/mutations_12.json
