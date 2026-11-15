Battle item handlers, batch 10 reproduction, 2026-11-15

Implemented: 12 DllBattle item handlers (every entry of White Herb 214,
Mental Herb 219 and Power Herb 271) and the helper 1084c140. See README.md
for addresses.

  make -C reconstructed/battle_items_10 test

The build links battle_items_1 (1084c96c, 1084c844, 1084c4c0), battle_ability
(108658d4) and battle_event.

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-7; exporter outputs/rea-items/export_items_10.py):

  python3 -I reconstructed/battle_items_10/reproduction/verify_rea.py

Two handlers (108a8714, 108aa05c) have no Ghidra function. Their listings
come from reconstructed/battle_power/reproduction/decode_arm.py. The final
bx lr of 108aa05c is spelled by a bx<cond> rule in verify_rea.py. That rule
must reproduce REA's listing of every REA-listed bx word, including the bx lr
in 1084c140. The exporter requires every instruction shape of the two
decoded handlers to be REA-listed in this batch.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_items_10/reproduction/check_bindings.py

Its output is record-comparison.json.

Mutation testing (26 mutations of battle_items_10.c, each must fail the suite):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_items_10 outputs/rea-items/mutations_10.json
