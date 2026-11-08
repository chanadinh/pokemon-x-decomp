Battle item handlers, batch 4 reproduction, 2026-11-08

Implemented: the 18 gems (items 548-564 and 715). That is 37 DllBattle item
handlers (18 on event 0x3d, 18 on 0x8a and one on 0x91) and the helpers
1084c284, 1084c700 and 10881638. See README.md for addresses.

  make -C reconstructed/battle_items_4 test

The build links battle_ability (1084ce84) and battle_event.

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-7; exporter outputs/rea-items/export_items_4.py):

  python3 -I reconstructed/battle_items_4/reproduction/verify_rea.py

Fourteen handlers have no Ghidra function. Their listings come from
reconstructed/battle_power/reproduction/decode_arm.py. The verifier requires
each to equal the REA listing of its same_shape_as sibling (108a8ec0 or
108aa510), with entry-relative branch and literal addresses, apart from the
one type constant.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_items_4/reproduction/check_bindings.py

Its output is record-comparison.json.

Mutation testing (35 mutations of battle_items_4.c, each must fail the suite):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_items_4 outputs/rea-items/mutations_4.json
