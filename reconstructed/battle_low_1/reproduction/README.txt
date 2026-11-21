Battle low-address helpers, batch 1 (10802000-10840000): reproduction, 2026-11-22

Implemented: 15 DllBattle functions of the band (see README.md for addresses).

  make -C reconstructed/battle_low_1 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-10 for the analyses and the byte reads):

  python3 -I reconstructed/battle_low_1/reproduction/verify_rea.py

All 15 are Ghidra functions. REA's one-line "FAULT" is the four-instruction abort
call; the retained listings carry the four instructions, which the verifier checks
by bytes and with reconstructed/battle_power/reproduction/decode_arm.py.

Survey check (needs outputs/rea-battle-10/267-list_procedures-response.json, which is
not committed):

  python3 -I reconstructed/battle_low_1/reproduction/check_bindings.py

Its output is record-comparison.json.

The evidence was exported by outputs/rea-abilities/export_batch.py low1; the
mutations are listed in outputs/rea-abilities/mutations_low1.py and run by
outputs/rea-abilities/mutate.py low1. The frontier came from
outputs/rea-abilities/low_frontier.py.
