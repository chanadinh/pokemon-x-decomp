Battle item handlers, batch 5 reproduction, 2026-11-08

Implemented: 23 DllBattle item handlers for 24 items. They are the stat
multipliers of events 0x13, 0x40 and 0x41, the critical-hit items of 0x3b,
the Choice lock and release, Iron Ball's grounding and Light Ball's use. See
README.md for addresses.

  make -C reconstructed/battle_items_5 test

The build links battle_ability (1084ce84, 108658d4) and battle_event.

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-7; exporter outputs/rea-items/export_items_5.py):

  python3 -I reconstructed/battle_items_5/reproduction/verify_rea.py

All 23 handlers are REA-listed. The verifier also checks
reconstructed/battle_power/reproduction/decode_arm.py against every listed
instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_items_5/reproduction/check_bindings.py

Its output is record-comparison.json.

Mutation testing (40 mutations of battle_items_5.c, each must fail the suite):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_items_5 outputs/rea-items/mutations_5.json
