Battle handler-table functions and registrars reproduction, 2026-11-21

Implemented: the table functions of the ability, item, field, side-effect and
position-effect tables of DllBattle.cro (382 regular stubs, the irregular
terrain function 108bbf98), the registrars 108acd0c (side), 108bc690
(position), 108a5bd8 and 108abf28 (item), and the helpers 1087c024 and
10882674. See README.md for details.

  make -C reconstructed/battle_tables_1 test

The build links battle_handler (1084cfc0, 1084cf8c, the pool and the list).

Evidence verification against live REA observations and the original module
(REA session outputs/rea-battle-10; exporter outputs/rea-items/export_tables_1.py):

  python3 -I reconstructed/battle_tables_1/reproduction/verify_rea.py

The registrars, the helpers and the stub 108a53cc are REA-listed functions.
The other 381 stubs and 108bbf98 have no Ghidra function; their listings come
from reconstructed/battle_power/reproduction/decode_arm.py and a bx<cond> rule
(in verify_rea.py), kept in evidence/stubs.asm and evidence/108bbf98.asm. The
exporter and the verifier require every instruction shape they use to be
REA-listed in this evidence or at one of five spelling references, each
checked against REA's listing and the live word.

Original-data check (needs outputs/cro-prepared):

  python3 -I reconstructed/battle_tables_1/reproduction/check_bindings.py

Its output is record-comparison.json.

Mutation testing (58 mutations of battle_tables_1.c, each must fail the suite):

  python3 -I outputs/rea-items/mutate.py reconstructed/battle_tables_1 outputs/rea-items/mutations_tables_1.json
