Battle side-effect handlers reproduction, 2026-11-20

Implemented: the 20 handlers of the 17-row side-effect table of DllBattle.cro
(108d4fe8) and the helpers 1084bfc0, 1084bf04, 1084bf14 and 108802a8. See
README.md for addresses.

  make -C reconstructed/battle_fields_1 test

The build links battle_items_1 (the record, message and commit callbacks'
shape), battle_abilities_1 (1084d2f4, the max-HP share), battle_ability
(1084ce84, 108658d4), and battle_event.

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-9; exporter outputs/rea-items/export_fields_1.py):

  python3 -I reconstructed/battle_fields_1/reproduction/verify_rea.py

Thirteen handlers have no Ghidra function. Their listings come from
reconstructed/battle_power/reproduction/decode_arm.py, a bx<cond> rule and a
pre-indexed str/strb rule (both in verify_rea.py). The exporter and the
verifier require every instruction shape they use to be REA-listed in this
evidence or at one of the 18 spelling references, each checked against REA's
listing and the live word. The 14 words 108ac950-108ac984 are the relocated
jump table of 108ac8fc, kept as .word and checked as ABS32 relocations into
their own handler.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_fields_1/reproduction/check_bindings.py

Its output is record-comparison.json.

Mutation testing (90 mutations of battle_fields_1.c, each must fail the suite):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_fields_1 outputs/rea-items/mutations_fields_1.json
