Battle item handlers, batch 6 reproduction, 2026-11-08

Implemented: 11 DllBattle item handlers. They are the accuracy items of event
0x39, the weather rocks of 0x85, Light Clay (0xab), and Grip Claw and Binding
Band (0x6b). See README.md for addresses.

  make -C reconstructed/battle_items_6 test

The build links battle_modifier (1084cbe4), battle_ability (1084ce84,
108658d4) and battle_event.

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-7; exporter outputs/rea-items/export_items_6.py):

  python3 -I reconstructed/battle_items_6/reproduction/verify_rea.py

Two handlers have no Ghidra function. Their listings come from
reconstructed/battle_power/reproduction/decode_arm.py:
- 108a5d60 must equal the REA listing of 108a6ce8 apart from the weather
  constant.
- 108a7e18's two forms not listed in this batch are checked against REA's
  listings at 108c43c0 and 10865d48.

The REA listings of 108a7598 and 108a7230 contain VFP instructions and smull,
which the decoder refuses, so those are kept as REA lists them.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_items_6/reproduction/check_bindings.py

Its output is record-comparison.json.

Mutation testing (32 mutations of battle_items_6.c, each must fail the suite):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_items_6 outputs/rea-items/mutations_6.json
