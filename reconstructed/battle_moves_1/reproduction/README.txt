Battle move handlers, base power (batch 1) reproduction, 2026-11-07

Implemented: 33 of the 34 DllBattle move handlers of event 0x3c (see
README.md for addresses). Eruption/Water Spout (108adae8) remains.

  make -C reconstructed/battle_moves_1 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-6; exporter outputs/rea-moves/export_moves_1.py):

  python3 -I reconstructed/battle_moves_1/reproduction/verify_rea.py

Four handlers have no Ghidra function; their listings come from
reconstructed/battle_power/reproduction/decode_arm.py, which the verifier also
checks against every supported REA-listed instruction. The four read-only
tables the literals point at are verified as data blocks.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_1/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_1.py.
