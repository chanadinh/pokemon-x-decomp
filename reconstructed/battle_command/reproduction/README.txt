Battle command-writer reproduction, 2026-11-07

Implemented: the command writer 1086c22c, the queue encoder 1084dbf4 and its
writers and packers (see README.md for addresses).

  make -C reconstructed/battle_command test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA sessions outputs/rea-battle-4 for the analyses and outputs/rea-battle-6
for the byte reads, over the same carrier):

  python3 -I reconstructed/battle_command/reproduction/verify_rea.py

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_command/reproduction/check_commands.py

Its output is record-comparison.json.
