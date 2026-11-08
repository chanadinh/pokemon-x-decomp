Battle item handlers, batch 2 reproduction, 2026-11-07

Implemented: 23 DllBattle item handlers (the Figy family 159-163 and the stat
berries 201-207) and the Figy body 1084c5e0 (see README.md for addresses).

  make -C reconstructed/battle_items_2 test

The build links battle_items_1 (1084c198, 1084c730, 1084c96c),
battle_abilities_1 (1084d2f4), battle_modifier (1084ca4c), battle_ability and
battle_event.

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-6; exporter outputs/rea-items/export_items_2.py):

  python3 -I reconstructed/battle_items_2/reproduction/verify_rea.py

Eight handlers have no Ghidra function. Their listings come from
reconstructed/battle_power/reproduction/decode_arm.py, plus a bx<cond> rule in
verify_rea.py that must reproduce REA's listing of 108695c4 (bxeq lr). The
verifier also checks decode_arm against every supported REA-listed
instruction, and checks Starf's five table bytes against the read-only
segment.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_items_2/reproduction/check_bindings.py

Its output is record-comparison.json.

Mutation testing (45 mutations of battle_items_2.c, each must fail the suite):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_items_2 outputs/rea-items/mutations_2.json
