Battle position-effect handlers reproduction, 2026-11-21

Implemented: the 17 handlers of the 11-row position-effect table of
DllBattle.cro (108d6768). See README.md for addresses.

  make -C reconstructed/battle_fields_2 test

The build links battle_items_1 (the record, message and commit callbacks'
shape), battle_fields_1 (1084bf04), battle_modifier (1084ca4c),
battle_abilities_1 (1084d2f4), battle_ability (1084ce84, 108658d4) and
battle_event.

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-10; exporter outputs/rea-items/export_fields_2.py):

  python3 -I reconstructed/battle_fields_2/reproduction/verify_rea.py

Eleven handlers have no Ghidra function. Their listings come from
reconstructed/battle_power/reproduction/decode_arm.py and a bx<cond> rule
(in verify_rea.py). The exporter and the verifier require every instruction
shape they use to be REA-listed in this evidence or at one of four spelling
references, each checked against REA's listing and the live word.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_fields_2/reproduction/check_bindings.py

Its output is record-comparison.json. It also reads row 10's irregular table
function (108bbf98) and its four entry tables.

Mutation testing (60 mutations of battle_fields_2.c, each must fail the suite):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_fields_2 outputs/rea-items/mutations_fields_2.json
