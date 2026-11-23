# Battle core helpers, batch 5

This milestone reconstructs 22 `DllBattle.cro` functions in `10840000`–`10880000`
and `108a6000`–`108ce000`: the random and item-data wrappers, a family of
bounds-checked battle-state accessors around `108c8400`–`108c99fc`, struct
initializers, list inserts, a scaled-HP event builder and the handler-blocking
test `108c59b4` that the event milestone takes as a callback. Eleven are
recorded callees of committed milestones; the others are the same accessor
family. None has an uncovered callee other than import thunks (calls out of the
module, which are callbacks here). Each has a live REA/Ghidra listing (session
`outputs/rea-battle-11`).

The names are analyst interpretations of what each function does. Runtime
addresses are static − `0x1015b000`. The battle state object (`108fca00`)
holds the random number object at `+0x54`, the item data object at `+0x5c` and
the first handler at `+0x60`. A bad index in the accessors calls the abort
routine `10800394` and the code goes on.

| Address | Function | Behavior |
| --- | --- | --- |
| `1084c7bc` | `px_battle_random` | A random value below a limit from the random object (import `108003dc`) |
| `1084cf74` | `px_battle_item_value` | An item data value (import `108003ac`) for an item and a byte parameter |
| `1084c0f8` | `px_battle_chance` | A random value below 100 is below a percentage |
| `108c86d4` | `px_battle_ring_length` | The length byte of the three-slot ring `back` entries behind its cursor (`+0x205`, lengths `+0x202`) |
| `108c99fc` | `px_battle_group_count` | Count byte of the 49-byte group `group` (below 4) |
| `108c8714` | `px_battle_entry_flag_121` | The low nibble of byte `+0x121` of the 14-byte entry `index` is set (count byte `+0x152`) |
| `108c8400` | `px_battle_entry_83c` | Word `+0x83c` of the 56-byte entry `index` (below 4) of the object at `+4` is set |
| `108c87a0` | `px_battle_byte_or_6` | Byte `index` of an object whose byte `+0x19` is its length, else 6 |
| `108c84e0` | `px_battle_condition_word` | The condition word `index` (below `0x26`) of a battler |
| `108c78ec` | `px_battle_byte_7ec` | Byte `+0x7ec + index` (index below 10) |
| `108c8510` | `px_battle_status_is_0x78` | The word at `+0x30` is a kind 1 word with field `0x78` |
| `108c79b0` | `px_battle_slot_half` | Halfword `+0x84c` of the 56-byte slot unless word `+0x838` is set |
| `108c82f4` | `px_battle_next_pair` | `(byte +0x815 + 2) & 3` if it is among the ten pair bytes at `+0x7ec`, else 4 |
| `108c5c18`, `108c9e30` | `px_battle_clear_18`, `px_battle_clear_10` | Zero 26 bytes (two bytes at `+0x18` and six words) or ten bytes |
| `108c9e84`, `108c9e68` | `px_battle_fill_10`, `px_battle_copy_7` | Fill ten bytes with one byte; copy seven bytes |
| `10872ff8` | `px_battle_find_matching_entry` | For an entry with flag `0x40`: the entry in its slot group's list (count `+0x18` of the 28-byte group at `+4`) whose first word equals the entry's key (`+4` if set, else `+0`); the entry itself otherwise |
| `108696b0` | `px_battle_add_unique` | Appends a word to a list of up to 10 (count `+0x6e`, high-water mark `+0x6f`) unless present, clearing its per-entry bytes and halfwords |
| `1086f878` | `px_battle_add_unique_slot` | The same for slot lists (slot below 11, enabled by word `+0x274`, count `+0x248`); an index of 11 or more aborts twice |
| `1084b9f0` | `px_battle_scaled_hp` | Runs event `0x84` with a multiplier (key `0x36`, 1.0 limited to `0x19a`–`0x20000`) and scales a battler's halfword `+0x150`, rounding half down, never below 1 |
| `108c59b4` | `px_battle_event_blocked` | Whether a handler is blocked: an ability handler's battler has a condition in `+0x5c`, an item handler is blocked by a context flag, a condition in `+0x68` or bit 1 of byte `+0x167`, or any other handler in the list with its own blocking function (word `+0xc`) reports it |

## Evidence

All 1,896 instruction bytes and 20 literal bytes match live REA bytes and the
original `DllBattle.cro`. Four literal words (the battle state object in
`1084c7bc`, `1084cf74`, `1084c0f8` and `108c59b4`) are ABS32 relocations whose
original words and values are checked; the other literals are the multiplier
limit `0x19a`. The decoder of the
[move-power milestone](../battle_power/README.md#decoder) reproduces every
listing it supports and REA's text stands where it refuses (`blx`, `mul` and a
`ldrh` form).

## Validation

`make -C reconstructed/battle_core_5 test` runs 6,124 cases: the random and
item callbacks with their objects, every index of the accessors counting abort
calls, all struct initializer bytes against a poison pattern, seeded searches and
inserts against independent models, the scaled HP through the real event stack
with handlers that multiply the key, and 3,000 random handler lists for the
blocked test (owners, flags, conditions and checker results; the checker's
arguments are compared).

ASan/UBSan pass. 79 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) reads
every other milestone's `evidence/functions.json` (committed or on disk) and
confirms that none of these 22 functions is covered elsewhere, that all start
inside the batch ranges and that the eleven chosen as frontier are recorded
callees of other milestones ([record](reproduction/record-comparison.json)).

## Limits

The remaining functions of the ranges (those with uncovered callees, jump
tables or VFP code, and the UI objects) are not part of this batch. No
running-game comparison has been made. Static evidence and host comparisons
do not establish a matching original build.
