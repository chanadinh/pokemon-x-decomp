# Battle handler-table functions and registrars

This milestone reconstructs the pieces that connect a battle's handler
tables to the handler pool: the table functions of the five non-move
tables, and the registrars that read those tables. The handlers
themselves are in the earlier milestones; the move table's handlers and
table functions belong to the move-batch milestones and are not touched here.

## Table functions

Each row of an ability, item, field, side-effect or position-effect table
holds an id and a table function (`108d510c`, `108d382c`, `108d6810`,
`108d4fe8` and `108d6768`). The table functions are called with a pointer for
a count word and return the address of an entries table of (event byte,
handler) pairs. In total the five tables name 383 distinct functions:

- **382 regular stubs** (`px_battle_table_stub`) of four instructions: `mov
  r1, #count` (or `ldr r1, [literal]` in two cases, `108c0c68` and
  `108c0c94`, whose count words carry a priority of 4 and 7), `str r1,
  [r0]`, `ldr r0, [entries literal]` and `bx lr`. The 20-byte stride of
  the stubs is the 16 bytes of code plus the entries literal. Their count
  constants run from 1 to 11.
- **One irregular function** (`108bbf98`, `px_battle_terrain_table`):
  position row 10, the terrains. It takes a parameter in `r1`: 1 returns
  the Grassy entries (`108d67d0`, count 2), 2 the Misty entries
  (`108d67e0`, count 3), 3 the Electric entries (`108d67f8`, count 3) and
  anything else the default entries (`108d380c`, count 1). The parameter 1
  case lives in a separate four-instruction block at `108bbfd0`.

One item row (181) holds no function.

## Registrars

The names are analyst interpretations of what each function does.

| Address (static) | Function | Behavior |
| --- | --- | --- |
| `108acd0c` | `px_battle_register_side` | Registers a side effect (owner 2) for a side (0 or 1) and an id below 17, or adds a layer. A side of 2 or more, or an id of 17 or more, calls the abort routine first. The layer record is at `10b11c00` + (side × 17 + id) × 16: handler `+0`, value `+4`, 0 `+8`, layer count `+0xc`. A first registration stores the handler there and sets the handler's work word 6 to the value; with layers present it returns the stored handler and adds a layer while the row's maximum (`+8` of the 12-byte row) is above the count, and otherwise continues with later rows. A failed registration calls the overflow routine (`108003a4`) and still stores the work word and the record |
| `108bc690` | `px_battle_register_position` | Looks the id up in the 11-row position table and registers an owner-3 handler (priority 3, value as given). The value is also passed to the table function as its parameter, which is what picks a terrain; 0 when the id has no row |
| `108a5bd8` | `px_battle_register_item_checked` | Registers the item held by a battler's pokemon, but not when `1087c024` finds handlers of owner 5 for the battler's slot |
| `108abf28` | `px_battle_register_item_given` | Registers a given item (a parameter) for a pokemon, marks the handler (flag bit 26) and sets its work word 6 to 1; returns the handler, or 0 |
| `1087c024` | `px_battle_handlers_unmark` | Walks the handler list; for each of an owner whose byte `+0x3b` is the slot, clears flag bit 30; returns whether any matched |
| `10882674` | `px_battle_handler_mark_26` | Sets flag bit 26 of a handler |

The item registrars scan the item table's 182 rows as `px_battle_register_item`
in the handler milestone does, and register with owner 5, priority 6, the
pokemon's speed as the sub-priority and its slot as the value.

Runtime addresses are static − `0x1015b000`.

## Evidence

All 7,104 instruction bytes and 1,576 literal bytes match live REA bytes and
the original `DllBattle.cro`. 392 literal words are ABS32 relocations
(entries pointers and the registrars' table literals) whose original words
and symbol-plus-addend values are checked.

Six registrars and helpers and the stub `108a53cc` are REA-listed
functions with Ghidra pseudocode. The other 381 stubs and the terrain
function have no Ghidra function; their listings come from the
[move-power milestone's decoder](../battle_power/README.md#decoder) plus a
`bx<cond>` rule and are kept in `evidence/stubs.asm` and
`evidence/108bbf98.asm`. Every instruction shape they use (registers,
immediates, literal addresses and branch targets abstracted) is REA-listed
in this batch or at one of five spelling references for conditional forms
(`streq`, `ldreq`, `movne`, `strne` and `ldrne`), each checked against
REA's listing and the live word. The decoder refuses only `blx`, in the
registrars, whose REA text is kept.

## Validation

`make -C reconstructed/battle_tables_1 test` runs 3,041 cases:

- **The stubs:** nine count words (including `0xffffffff`) against several
  entries values, with the neighbouring words checked untouched; the
  terrain function for 32 parameters and `0xffffffff`.
- **`1087c024` and `10882674`:** every slot and owner pair over five-handler
  lists in all 32 owner patterns, with flags checked exactly.
- **Side effects:** all three sides against 19 ids with the exact handler,
  record and work-word fields, layers up to every row's maximum, a stored
  layer count, duplicate rows, and an exhausted pool.
- **Position effects:** 14 ids against the table order with the parameter
  seen by the table function, and a pool that is full.
- **Items:** all 256 item values with and without handlers to unmark, for
  both item registrars.

ASan/UBSan pass. 58 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) reads all
five tables and confirms that 396 of their 397 rows name a table function
(item row 181 has none), that the 383 distinct functions are exactly the
ones in `evidence/functions.json`, that every regular stub has the four
expected instructions with a count in range, that the terrain function's 18
words and four literals are as above, and that the registrars' retained
listings and literals carry the loop bounds, record layout, priorities and
table addresses used here ([record](reproduction/record-comparison.json)).

## Limits

These remain external:

- the abort and overflow routines, the speed and pokemon queries, the pool
  and handler state (provided by the handler milestone's callbacks) and the
  indirect call of a table function
- the move table's table functions and handlers, which other milestones
  cover
- the callers of these registrars and the table functions' use by the
  event dispatcher

No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
