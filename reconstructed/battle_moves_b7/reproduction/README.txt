Battle move handlers, event 0xac (last group): reproduction, 2026-11-21

Implemented: Heal Bell (108b87e0) and Aromatherapy (108b5620) with their shared
body 108b87e4 (see README.md).

  make -C reconstructed/battle_moves_b7 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-10 for the analysis, the byte reads and the
inspect_native_instruction record of 108b8950):

  python3 -I reconstructed/battle_moves_b7/reproduction/verify_rea.py

108b87e4 is a Ghidra function. 108b87e0 and 108b5620 have none; their listings
come from decode_arm.py in this directory.

decode_arm.py here is a COPY of reconstructed/battle_power/reproduction/decode_arm.py
(SHA-256 7b8658ce88254bf87eece1a767521fba45b277301e56f1984eca5ed22624fcd2, left
untouched), extended with one form only: the post-indexed immediate byte load
`ldrb<cond> rd, [rn],#imm` (add, no write-back flag, rn not pc), which REA lists
for word e4d03001 at 108b8950 as `ldrb r3,[r0],#0x1`. decode_arm.diff is the
unified diff (one added branch of four lines). The copy's SHA-256 is printed by
check_decoder.py. verify_rea.py imports this copy.

  python3 -I reconstructed/battle_moves_b7/reproduction/check_decoder.py

It checks the original's hash, that the copy and the original decode every
instruction word of every evidence listing under reconstructed/*/evidence
identically (14 words of the extended form, this milestone's own included, which the original refuses, equal
their listings), and that they agree on 4,000,000 pseudo-random words plus the
neighbourhood of the extended form except on that form itself.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_moves_b7/reproduction/check_bindings.py

Its output is record-comparison.json; it also confirms that every move-table
handler is covered by some evidence/functions.json (battle_moves_19 included).

The evidence was exported by outputs/rea-abilities/export_batch.py b7; the
mutations are listed in outputs/rea-abilities/mutations_b7.py and run by
outputs/rea-abilities/mutate.py b7.
