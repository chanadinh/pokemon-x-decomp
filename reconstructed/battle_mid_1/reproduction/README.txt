Battle mid-band helpers (batch 1, band 10880000-108a6000) reproduction, 2026-11-21

Implemented: 18 DllBattle functions of the band that committed milestones call
(see README.md for addresses).

  make -C reconstructed/battle_mid_1 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-10; exporter outputs/rea-moves/export_mid_1.py):

  python3 -I reconstructed/battle_mid_1/reproduction/verify_rea.py

All 18 functions have Ghidra listings; the verifier also checks the battle_power
decoder against every REA-listed instruction it supports (it refuses six).
The 14 halfword lists the literals point at are data blocks; 19 literal words
are ABS32 relocations, verified word by word.

Overlap and connectivity check (needs the other milestones' evidence):

  python3 -I reconstructed/battle_mid_1/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_mid_1.py (mutations in mutations_mid_1.py).
