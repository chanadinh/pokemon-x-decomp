Battle move handlers, after the move and Secret Power (batch 9) reproduction, 2026-11-08

Implemented: the 13 DllBattle move handlers of event 0x24, Secret Power's
handler of event 0x8c and its environment routine 108bb8ac (see README.md for
addresses).

  make -C reconstructed/battle_moves_9 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-7; exporter outputs/rea-moves/export_moves_9.py):

  python3 -I reconstructed/battle_moves_9/reproduction/verify_rea.py

Three routines have no Ghidra function; their listings come from
reconstructed/battle_power/reproduction/decode_arm.py (108bb8ac in two segments
around its jump table), which the verifier also checks against every supported
REA-listed instruction. The Magnitude table is verified as a data block.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_9/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_9.py (mutations in mutations_9.py).
