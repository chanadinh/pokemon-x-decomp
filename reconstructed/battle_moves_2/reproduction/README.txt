Battle move handlers, damage-calculation inputs (batch 2) reproduction, 2026-11-07

Implemented: the 15 remaining DllBattle move handlers of events 0x3c-0x40
(see README.md for addresses).

  make -C reconstructed/battle_moves_2 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-6; exporter outputs/rea-moves/export_moves_2.py):

  python3 -I reconstructed/battle_moves_2/reproduction/verify_rea.py

Two handlers have no Ghidra function; their listings come from
decode_mul.py (reconstructed/battle_power/reproduction/decode_arm.py plus the
MUL form). The verifier checks the decoder against every supported
REA-listed instruction and the MUL form against the 20 MUL instructions of
twelve retained reference analyses.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_2/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_2.py.
