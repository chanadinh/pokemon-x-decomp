Battle low-address state hooks, batch 3 (10802000-10840000): reproduction, 2026-11-22

Implemented: 15 DllBattle functions of the band (see README.md for addresses).

  make -C reconstructed/battle_low_3 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-10 for the analyses and the byte reads):

  python3 -I reconstructed/battle_low_3/reproduction/verify_rea.py

All 15 are Ghidra functions. Their pointer literals are ABS32 relocations, checked by the verifier.

Survey check (the family and the band survey) (needs outputs/rea-battle-10/267-list_procedures-response.json, which is
not committed):

  python3 -I reconstructed/battle_low_3/reproduction/check_bindings.py

Its output is record-comparison.json.

The evidence was exported by outputs/rea-abilities/export_batch.py low3; the
mutations are listed in outputs/rea-abilities/mutations_low3.py and run by
outputs/rea-abilities/mutate.py low3. The family came from
outputs/rea-abilities/find_family.py (plan_band.py lists the band from the original module).
