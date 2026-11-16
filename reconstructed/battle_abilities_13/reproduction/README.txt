Battle last-ability-handlers reproduction, 2026-11-15

Implemented: the last three DllBattle ability handlers (Magician event 0x8c,
Scrappy event 0x04, Aerilate/Pixilate/Refrigerate event 0x91) and the work
reset 1087a19c (see README.md for addresses).

  make -C reconstructed/battle_abilities_13 test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt;
REA session outputs/rea-battle-8 for the byte reads and the spelling
reference):

  python3 -I reconstructed/battle_abilities_13/reproduction/verify_rea.py

None of the four functions has a Ghidra function; their listings come from
reconstructed/battle_power/reproduction/decode_arm.py, which the verifier also
checks against every supported REA-listed instruction. The bx lr word the
decoder refuses is accepted only as REA spells it in the retained analysis of
1084aeec.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_abilities_13/reproduction/check_bindings.py

Its output is record-comparison.json.

The evidence was exported by outputs/rea-abilities/export_batch.py 13; the
verifier was written by outputs/rea-abilities/make_verify_13.py; the mutations
are listed in outputs/rea-abilities/mutations_13.py and run by
outputs/rea-abilities/mutate.py 13.
