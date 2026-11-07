Battle message-routine reproduction, 2026-11-07

Implemented: the message routines 1086b5e0, 1086bbe8 and 10867bbc
(see README.md for addresses).

  make -C reconstructed/battle_message test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA sessions outputs/rea-battle-4 for the analyses and outputs/rea-battle-6
for the byte reads, over the same carrier):

  python3 -I reconstructed/battle_message/reproduction/verify_rea.py

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_message/reproduction/check_messages.py

Its output is record-comparison.json.
