Battle item handlers, batch 3 reproduction, 2026-11-08

Implemented: 29 DllBattle item handlers. They are the 28 power boosts of
event 0x3d (the type items and plates, the incenses, the three orbs, Muscle
Band and Wise Glasses) and Poison Barb's event-0x7c handler. See README.md
for addresses.

  make -C reconstructed/battle_items_3 test

The build links battle_modifier (1084cbe4), battle_ability (1084ce84) and
battle_event.

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-6; exporter outputs/rea-items/export_items_3.py):

  python3 -I reconstructed/battle_items_3/reproduction/verify_rea.py

Five handlers have no Ghidra function. Their listings come from
reconstructed/battle_power/reproduction/decode_arm.py. The verifier checks
the decoder against every REA-listed instruction of the other 24, and against
REA's listing of 108bc9ec (uxth r0, r0), the one further form the five use.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_items_3/reproduction/check_bindings.py

Its output is record-comparison.json.

Mutation testing (35 mutations of battle_items_3.c, each must fail the suite):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_items_3 outputs/rea-items/mutations_3.json
