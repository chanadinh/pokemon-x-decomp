Battle contact-abilities reproduction, 2026-11-07

Implemented: 14 DllBattle ability handlers of event 0x53 and three helpers
they share (see README.md for addresses).

  make -C reconstructed/battle_abilities_1 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-6 for the analyses and the byte reads):

  python3 -I reconstructed/battle_abilities_1/reproduction/verify_rea.py

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_abilities_1/reproduction/check_bindings.py

Its output is record-comparison.json.

The evidence was exported by outputs/rea-abilities/export_batch.py 1; the
mutations are listed in outputs/rea-abilities/mutations_1.py and run by
outputs/rea-abilities/mutate.py 1.
