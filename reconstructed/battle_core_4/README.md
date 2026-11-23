# Battle core helpers, batch 4

This milestone reconstructs 15 `DllBattle.cro` functions in `10840000`–`10880000`,
mostly the battler-condition queries around `1084a9d4`–`1084d184`: the
grounded, airborne and exempt checks, item-use and condition tests, and two
record builders. All 15 are recorded callees of committed milestones and none
has an uncovered callee. Each has a live REA/Ghidra listing (session
`outputs/rea-battle-10`).

The names are analyst interpretations of what each function does. Runtime
addresses are static − `0x1015b000`. The battle context holds the battler
table pointer at `+8`: battlers are the pointers at `+0x84 + 4 id` (an id
of `0x18` or more, or a missing battler, calls the abort routine `10800394`
and the code goes on).

| Address | Function | Behavior |
| --- | --- | --- |
| `108651d8` | `px_battle_grounded_check` | Runs event `0x12` for a battler: key 2 the battler's slot, key `0x53` 1 when the battler has the Flying type (type 2, when asked), key `0x42` 0. Handlers may rewrite them; then a condition word at `+0x70` or `+0x98` forces key `0x42` to 1 and one at `+0x9c` or `+0x94` forces key `0x53` to 1. True when key `0x42` ends 0 and key `0x53` nonzero |
| `1084bf24` | `px_battle_is_airborne` | The grounded check (Flying asked) for a battler id, false while the context object's flag at `+0x27c` is set |
| `1084af30` | `px_battle_is_exempt` | `px_battle_first_set_id` over the battler's bit array finds one of the four table ids |
| `1084b5b8` | `px_battle_item_usable` | The battler's ability is not `0x67`, no condition at `+0x68` and the context object's word at `+0x290` is 0 |
| `1084c030` | `px_battle_has_flagged_condition` | Byte `+0x171` is 1 to `0x18`, or the word at `+0xa0` is active |
| `1084b950` | `px_battle_group_table` | The address `+4` of the 28-byte group entry for a slot's group (0 to 3) in the context table |
| `1084c3cc` | `px_battle_mark_group_flag` | When a mode byte is 0 or 1 and a byte at `+0x815` equals the slot's group, sets a flag byte `+0xa50 + index` (index 0 or 1; otherwise abort) |
| `1084bac4` | `px_battle_ring_copy` | Of a three-slot ring of entries (cursor byte `+0x205`, lengths `+0x202`, 6 entries of 8 bytes per 48 bytes from `+0x172`), copies the entry `back` slots behind the cursor and `skip` from its end; 0 when out of range |
| `10856a88` | `px_battle_has_condition` | The condition word `index` (below `0x26`) of a battler has a low-3-bit value |
| `1084d184` | `px_battle_range_width` | Byte `+0x11f` minus byte `+0x11e` of the 14-byte entry `index` |
| `10876384` | `px_battle_array_word_b` | Word `index` of an array whose byte `+0x18` is its length, else 0 (a copy of `1085daf8`) |
| `1086fd18` | `px_battle_fill_6` | Fills six bytes with one byte value |
| `1084beac` | `px_battle_pack_7` | `(first << 3 & 0x1f8 \| 2)` with bits 9–24 replaced by `second` |
| `1084bd48` | `px_battle_report_slot` | When event key 3 is the id: records kind `0x15` (id, 7), kind `0x27` (id, 3 and a count from `px_battle_byte_16d` plus 1), and kind 3 with message `0x205` |
| `1084a9d4` | `px_battle_popup_message` | When event key 2 is the id and a state word is set: unless the same key `0x1a` value was already shown, an ability popup, kind 3 message with the message flag set and a closing popup; then clears the state word |

## Evidence

All 1,704 instruction bytes and 8 literal bytes match live REA bytes and the
original `DllBattle.cro`; no relocation is involved (the literals are the
message id `0x205` and the mask `0x1fffe00`). The decoder of the
[move-power milestone](../battle_power/README.md#decoder) reproduces every
listing it supports.

## Validation

`make -C reconstructed/battle_core_4 test` runs 4,665 cases:

- **The grounded check** against an independent model of the event keys
  (including handlers that rewrite a key first, after which the function's own
  rewrite does not take) over 3,000 random battler and condition combinations.
- **Airborne, exempt and item checks** for every battler id including the
  aborting ones, and every condition combination.
- **The ring copy, group map and flag marker** for every cursor, offset, mode
  and slot.
- **The record builders** against the real event stack with exact record,
  message and popup sequences.

ASan/UBSan pass. 77 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) reads
every other milestone's `evidence/functions.json` (committed or on disk) and
confirms that none of these 15 functions is covered elsewhere, that all start
inside the batch ranges and that each is a recorded callee of another
milestone's function ([record](reproduction/record-comparison.json)).

## Limits

The rest of this cluster (functions with uncovered callees, the jump-table
function `1084b82c` and the UI objects) is not part of this batch. No
running-game comparison has been made. Static evidence and host comparisons
do not establish a matching original build.
