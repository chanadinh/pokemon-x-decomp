# Battle move-table functions

This milestone covers the move table's own plumbing in `DllBattle.cro`: the
table functions of its rows. The move registrar (`10869dd4`,
`px_battle_register_move`) is already in the battle_handler milestone, and
the move handlers (the event entries) belong to the move-batch milestones;
neither is touched here.

## Table functions

The move table (`108d3ddc`) has 296 rows of 8 bytes: a 16-bit move id and a
table function. All 296 ids are distinct and all rows name a function; the
rows share them, so there are 248 distinct functions. Every one is a
regular four-instruction stub of the form reconstructed in
[battle_tables_1](../battle_tables_1/README.md): `mov r1, #count`, `str r1,
[r0]`, `ldr r0, [entries literal]` and `bx lr`. The count constants run
from 1 to 9 and every priority is 0, so the stored count word is the count
itself. `px_battle_move_table_function` is the stub: it stores the count word
through its first argument and returns the entries table.

Runtime addresses are static − `0x1015b000`.

## Evidence

All 3,968 instruction bytes and 992 literal bytes match live REA bytes
(session `outputs/rea-battle-10`) and the original `DllBattle.cro`. The 248
entries literals are ABS32 relocations
whose original words and symbol-plus-addend values are checked.

The 248 stubs have no Ghidra function; their listings come from the
[move-power milestone's decoder](../battle_power/README.md#decoder) and are
kept in `evidence/stubs.asm`. Their instruction shapes (registers,
immediates and literal addresses abstracted) are all REA-listed in this
batch through four spelling references: the REA listing of the field-table
stub `108a53cc`, a function of the same four-instruction form. That stub is
owned by battle_tables_1 and is not part of this milestone's function list.

## Validation

`make -C reconstructed/battle_tables_2 test` runs 392 cases: eight count
words (including `0xffffffff`) against several entries values with the
neighbouring words checked untouched, and all 296 moves registered through
`px_battle_register_move` with the stub as the table function (handler
entries, count, owner, id, value and sort key checked for every row, with
shared functions).

ASan/UBSan pass. 8 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) reads the
296 rows of the original table and confirms that every row names a function,
that the 248 distinct functions are regular stubs with a count from 1 to
255 and a priority below 8, that they are exactly the functions of this
milestone's `evidence/functions.json`, that none is covered by an earlier
milestone, that `108a53cc` (the shape reference) is a field table function
owned by battle_tables_1 and not listed here, and that the move registrar is
covered elsewhere
([record](reproduction/record-comparison.json)).

## Limits

These remain external:

- the indirect call of a table function and the pool and handler state
  (the battle_handler milestone's callbacks)
- the move handlers themselves, which the move-batch milestones cover

No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
