# Battle event handler registration

This twenty-fifth host-source milestone reconstructs how `DllBattle.cro`
attaches event handlers to a battle: the handler pool, sorted registration,
removal and search, and the four routines that register the handlers of an
ability, an item, a move or a field effect from their lookup tables. All ten
routines were reviewed through live REA MCP/Ghidra and ARM instructions; the
original-data check registers every row of the four original tables.

| Address (static) | Host routine | Behavior |
| --- | --- | --- |
| `1084cfc0` | `px_battle_handler_register` | Take a pool handler, fill it and insert it by sort key |
| `1084d1cc` | `px_battle_handler_remove` | Remove one handler (deferred while it runs) |
| `1088164c` | `px_battle_handler_remove_owner6` | Remove every handler of owner 6 |
| `1084d3c0` | `px_battle_handler_find` | First registered handler of an owner and value byte |
| `1084d370` | `px_battle_handler_find_next` | Next handler with the same owner and value byte |
| `1084cf8c` | `px_battle_handler_set_work` | Set work word 0–6 |
| `108678f8` | `px_battle_register_ability` | Ability handlers (owner 4) |
| `108abe30` | `px_battle_register_item` | Held-item handlers (owner 5) |
| `10869dd4` | `px_battle_register_move` | Move handlers (owner 0) |
| `108a5aac` | `px_battle_register_field` | Field-effect handlers (owner 1) |

Runtime addresses are static − `0x1015b000`. Names are analyst
interpretations; the handler layout is in the
[event-system milestone](../battle_event/README.md).

## Pool and registration

The pool array (`10b1196c`) holds 157 (`0x9d`) handlers, and battle state word
`+0x64` counts those in use: registration takes the handler at that index and
removal puts one back below it. (The event-system milestone first described
this word as a free count; its documentation is corrected here.)

Registration aborts (and continues) for a priority of `0xff` or more or a
sub-priority of `0xffffff` or more. A full pool aborts and returns 0; an
empty pool slot aborts and returns 0 after the count has advanced. The
handler gets its owner, id (`+0x38`), value byte (`+0x3a`), entry table and
count, a sort key `(7 − priority) << 24 | sub-priority` (`+0x14`), the
current dispatch depth as its registration depth and the registered bit; the
other state bits are cleared, the work area is zeroed, and byte `+0x3b`
copies the value for owners 0, 4 and 5 and is `0x1f` otherwise. It is
inserted after every handler whose key is at least as large, so larger keys
(lower priority numbers, then larger sub-priorities) run first and equal keys
keep their order.

Removal follows the event system's deferred rule. `find` requires the
registered bit; `find_next` does not. Both skip handlers with a removal
request. Work index 7 or more aborts but is still written, so index 7 lands
on the id and value bytes.

## Lookup tables

Each table row holds an id and a function that stores a count word and
returns the handler entries. The count word's low half is the number of
entries and its high half a priority:

- **Abilities** (`108d510c`, 182 rows, byte ids against the 16-bit ability
  from `10874420(pokemon, 0x10)`): priority from the high half, or 5 if it
  is zero, narrowed to a byte; sub-priority `108743a8(pokemon, 0xc)` (speed).
- **Items** (`108d382c`, 182 rows, word ids; the last row is an empty
  terminator): only a nonzero item at `+0x12`; priority 6; sub-priority speed.
- **Moves** (`108d3ddc`, 296 rows, 16-bit ids): not again for the same move,
  not when the battler already has nine other move handlers, not when the
  function returns no table; priority 0 with the caller's sub-priority;
  returns 1 on registration.
- **Field effects** (`108d6810`, 5 rows): kinds of 5 or more abort; not again
  for the same kind and value; priority 1; the caller's words fill work slots
  0.. and slot 6 takes the final argument.

The value byte is the battler id (`+0x19`) for abilities, items and moves.

## Validation

`make -C reconstructed/battle_handler test` checks every field written for
each owner over stale contents; both bounds and the pool limits; 300 random
sequences of 157 registrations against an independent sorted-list model; the
removal of the head, middle and tail and its reuse; deferred, unregistered
and pool-empty removals; every owner-6 mix including a running handler;
`find` and `find_next`; work indices; and the four registrars over their id
widths, priorities, duplicates and limits (1,204 cases). ASan/UBSan pass; 27
mutations are each detected. One further mutation (also clearing the
registered bit before it is set again) is equivalent and not counted.

[`reproduction/check_tables.py`](reproduction/check_tables.py) decodes every
row of the four original tables and its table function (601 of the form
`mov r1, #word` and 2 loading the word from a literal), checks every handler
entry against the relocation records, and registers all 664 rows through the
reconstruction built as a shared library: 603 table functions, 1,154 handler
entries, 926 distinct handler functions over 155 events, all matching
([record](reproduction/record-comparison.json)).

## Limits

The handler functions themselves, the value services `10874420` and
`108743a8`, the pool initialization (`1086e394`) and the abort helper remain
external. No running-game comparison of these routines has been made. Static
evidence and host comparisons do not establish a matching original build.
