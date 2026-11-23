Battle mid-band teardowns and accessors (batch 3, band 10880000-108a6000) reproduction, 2026-11-21

Implemented: 19 DllBattle functions of the band (see README.md for addresses).

  make -C reconstructed/battle_mid_3 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-10; exporter outputs/rea-moves/export_mid_3.py):

  python3 -I reconstructed/battle_mid_3/reproduction/verify_rea.py

All 19 functions have Ghidra listings; the verifier also checks the
battle_power decoder against every REA-listed instruction. The functions have
no literal pools or relocations.

Overlap check (needs the other milestones' evidence):

  python3 -I reconstructed/battle_mid_3/reproduction/check_bindings.py

Its output is record-comparison.json. Mutation testing:
outputs/rea-moves/mutate_mid_3.py (mutations in mutations_mid_3.py).
