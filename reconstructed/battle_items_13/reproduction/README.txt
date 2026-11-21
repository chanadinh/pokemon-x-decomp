Battle item handlers, batch 13 reproduction, 2026-11-15

Implemented: the last 8 DllBattle item-table handlers. They are Amulet Coin
and Luck Incense 223/319, Smoke Ball 228, Destiny Knot 280, Float Stone 539,
Ring Target 543 and Safety Goggles 650. See README.md for addresses.

  make -C reconstructed/battle_items_13 test

The build links battle_items_1, battle_ability (1084ce84, 108658d4) and
battle_event.

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-8; exporter outputs/rea-items/export_items_13.py):

  python3 -I reconstructed/battle_items_13/reproduction/verify_rea.py

Two handlers (1084c3bc, 108ab6f8) have no Ghidra function. Their listings
come from reconstructed/battle_power/reproduction/decode_arm.py. The exporter
requires every instruction shape they use to be REA-listed in this batch or
at the spelling reference 108b1080 (ble). The verifier checks that reference
against REA's listing and the live word.

Original-data and coverage check (needs outputs/cro-prepared and the other
milestones' evidence/functions.json):

  python3 -I reconstructed/battle_items_13/reproduction/check_bindings.py

Its output is record-comparison.json. Besides this batch's bindings, it
confirms that all 247 distinct item-table handlers appear in some milestone's
evidence/functions.json.

Mutation testing (27 mutations of battle_items_13.c, each must fail the suite):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_items_13 outputs/rea-items/mutations_13.json
