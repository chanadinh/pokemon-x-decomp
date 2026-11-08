Battle item handlers, batch 7 reproduction, 2026-11-08

Implemented: 12 DllBattle item handlers for Toxic Orb, Flame Orb, Sticky
Barb, Leftovers, Black Sludge, Shell Bell, Life Orb (recoil) and Big Root.
See README.md for addresses.

  make -C reconstructed/battle_items_7 test

The build links battle_items_1 (1084c96c), battle_abilities_1 (1084d2f4),
battle_modifier (1084cbe4), battle_ability (1084ce84) and battle_event.

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-7; exporter outputs/rea-items/export_items_7.py):

  python3 -I reconstructed/battle_items_7/reproduction/verify_rea.py

Two handlers (108aa8e8, 108ab2cc) have no Ghidra function. Their listings come
from reconstructed/battle_power/reproduction/decode_arm.py. The exporter
requires every instruction form they use to appear in this batch's REA
listings, and the verifier checks the decoder against all of those listings.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_items_7/reproduction/check_bindings.py

Its output is record-comparison.json.

Mutation testing (39 mutations of battle_items_7.c, each must fail the suite):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_items_7 outputs/rea-items/mutations_7.json
