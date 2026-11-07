Battle item handlers, batch 1 reproduction, 2026-11-07

Implemented: 19 DllBattle item handlers (the cure berries 149-153, 156 and
157; Berry Juice 43, Oran 155 and Sitrus 158) and seven shared routines
(see README.md for addresses).

  make -C reconstructed/battle_items_1 test

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-6; exporter outputs/rea-items/export_items_1.py):

  python3 -I reconstructed/battle_items_1/reproduction/verify_rea.py

Fourteen handlers have no Ghidra function; their listings come from
reconstructed/battle_power/reproduction/decode_arm.py, which the verifier also
checks against every supported REA-listed instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_items_1/reproduction/check_bindings.py

Its output is record-comparison.json.

Mutation testing (43 mutations of battle_items_1.c, each must fail the suite):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_items_1 outputs/rea-items/mutations_1.json
