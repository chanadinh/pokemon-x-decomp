Battle item handlers, batch 8 reproduction, 2026-11-08

Implemented: 19 DllBattle item handlers for 28 items. They are the items set
off by a hit (events 0x53 and 0x54) and their event-0x7b uses: Rocky Helmet,
Air Balloon, Absorb Bulb, Cell Battery, Snowball, Luminous Moss, Weakness
Policy, Enigma, Jaboca and Rowap, and the resist berries' event 0x53. See
README.md for addresses.

  make -C reconstructed/battle_items_8 test

The build links battle_items_1 (1084c96c), battle_abilities_1 (1084d2f4,
1085f844), battle_ability (108658d4) and battle_event.

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-7; exporter outputs/rea-items/export_items_8.py):

  python3 -I reconstructed/battle_items_8/reproduction/verify_rea.py

Six handlers have no Ghidra function. Their listings come from
reconstructed/battle_power/reproduction/decode_arm.py. The exporter requires
every instruction shape they use to be REA-listed in this batch or at a
spelling reference (1084bcb0, 1084bd2c, 1084bd38, 108b1744, 108ba8f0). The
verifier checks each reference against REA's listing and the live word.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_items_8/reproduction/check_bindings.py

Its output is record-comparison.json.

Mutation testing (42 mutations of battle_items_8.c, each must fail the suite):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_items_8 outputs/rea-items/mutations_8.json
