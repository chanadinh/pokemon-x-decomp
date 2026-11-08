Battle move handlers, effects after a hit (batch 7) reproduction, 2026-11-08

Implemented: 11 DllBattle move handlers of event 0x8c and the Smack Down
routine 108b65f8 (see README.md for addresses); Secret Power's handler of the
event is deferred.

  make -C reconstructed/battle_moves_7 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-7; exporter outputs/rea-moves/export_moves_7.py):

  python3 -I reconstructed/battle_moves_7/reproduction/verify_rea.py

Three routines have no Ghidra function; their listings come from
reconstructed/battle_power/reproduction/decode_arm.py, which the verifier also
checks against every supported REA-listed instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_7/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_7.py (mutations in mutations_7.py).
