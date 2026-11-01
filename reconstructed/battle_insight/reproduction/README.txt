Battle insight reproduction, 2026-11-01

Implemented: the last five DllBattle handlers of event 0x94 (Trace, Download,
Forewarn, Anticipation, Frisk; see README.md for addresses). Together with
battle_switchin every entry of event 0x94 is now reconstructed.

  make -C reconstructed/battle_insight test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-4):

  python3 -I reconstructed/battle_insight/reproduction/verify_rea.py

108be1dc (Download) and 108be6bc (Frisk) have no Ghidra function; their
listings come from reconstructed/battle_power/reproduction/decode_arm.py,
which the verifier also checks against every supported REA-listed instruction.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_insight/reproduction/check_bindings.py

Its output is record-comparison.json.
