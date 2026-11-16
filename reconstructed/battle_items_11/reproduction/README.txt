Battle item handlers, batch 11 reproduction, 2026-11-15

Implemented: 7 DllBattle item handlers (every entry of Red Card 542, Eject
Button 547, Kee Berry 687 and Maranga Berry 688) and the helpers 1084c000 and
1084c020. See README.md for addresses.

  make -C reconstructed/battle_items_11 test

The build links battle_items_1 (1084c96c), battle_ability and battle_event.

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-8; exporter outputs/rea-items/export_items_11.py):

  python3 -I reconstructed/battle_items_11/reproduction/verify_rea.py

Three handlers (108a5e78, 108aaec4, 108a7478) have no Ghidra function. Their
listings come from reconstructed/battle_power/reproduction/decode_arm.py. The
exporter requires every instruction shape they use to be REA-listed in this
batch or at a spelling reference (108af7a4, 108af7a8, 108bbaf0). The verifier
checks each reference against REA's listing and the live word. It also checks
the bx<cond> rule against the bx lr of the two helpers.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_items_11/reproduction/check_bindings.py

Its output is record-comparison.json.

Mutation testing (35 mutations of battle_items_11.c, each must fail the suite):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_items_11 outputs/rea-items/mutations_11.json
