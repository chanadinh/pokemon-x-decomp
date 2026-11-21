Battle move-table functions reproduction, 2026-11-21

Implemented: the 248 distinct table functions of the 296-row move table of
DllBattle.cro (108d3ddc), regular four-instruction stubs. The move registrar
10869dd4 is in battle_handler. See README.md.

  make -C reconstructed/battle_tables_2 test

The build links battle_tables_1 and battle_handler.

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-10; exporter outputs/rea-items/export_tables_2.py):

  python3 -I reconstructed/battle_tables_2/reproduction/verify_rea.py

The stubs have no Ghidra function. Their listings come from
reconstructed/battle_power/reproduction/decode_arm.py and are kept in
evidence/stubs.asm; every instruction shape is REA-listed through the stub
108a53cc.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_tables_2/reproduction/check_bindings.py

Its output is record-comparison.json.

Mutation testing (8 mutations of battle_tables_2.c, each must fail the suite):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_tables_2 outputs/rea-items/mutations_tables_2.json
