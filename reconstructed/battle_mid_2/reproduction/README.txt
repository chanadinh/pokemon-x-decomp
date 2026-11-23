Battle mid-band page objects (batch 2, band 10880000-108a6000) reproduction, 2026-11-21

Implemented: 22 DllBattle functions of the band (see README.md for addresses).

  make -C reconstructed/battle_mid_2 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-10; exporter outputs/rea-moves/export_mid_2.py):

  python3 -I reconstructed/battle_mid_2/reproduction/verify_rea.py

All 22 functions have Ghidra listings; the verifier also checks the
battle_power decoder against every REA-listed instruction it supports. The
functions have no literal pools or relocations.

Overlap check (needs the other milestones' evidence):

  python3 -I reconstructed/battle_mid_2/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_mid_2.py (mutations in mutations_mid_2.py).
