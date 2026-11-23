Battle low-address functions, batch 4 (10802000-10840000): reproduction, 2026-11-22

Implemented: 7 DllBattle functions of the band (see README.md for addresses).

  make -C reconstructed/battle_low_4 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-11 for the analyses and the byte reads):

  python3 -I reconstructed/battle_low_4/reproduction/verify_rea.py

All 7 are Ghidra functions. Their pointer literals (and the jump table words of 1083c0b8) are ABS32 relocations, checked by the verifier.

Survey check (needs outputs/rea-battle-10/267-list_procedures-response.json, which is
not committed):

  python3 -I reconstructed/battle_low_4/reproduction/check_bindings.py

Its output is record-comparison.json.

The evidence was exported by outputs/rea-abilities/export_batch.py low4; the
mutations are listed in outputs/rea-abilities/mutations_low4.py and run by
outputs/rea-abilities/mutate.py low4. The frontier and the planning listings came from
outputs/rea-abilities/low_frontier.py and plan_band.py.
