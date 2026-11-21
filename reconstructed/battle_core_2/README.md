# Battle core helpers, batch 2

This milestone reconstructs 21 small `DllBattle.cro` functions in the
address ranges `10840000`–`10880000` and `108a6000`–`108ce000`, taken from
the frontier: every one is already a recorded callee of some committed
milestone and none has an uncovered callee of its own. They are accessors,
table and array searches, bit-field rewrites, bulk clears and a handler
cleanup. Each has a live REA/Ghidra listing (session `outputs/rea-battle-10`).

The names are analyst interpretations of what each function does. Runtime
addresses are static − `0x1015b000`. Layouts are those of
[battle_core_1](../battle_core_1/README.md) plus the ones named below.

| Address | Function | Behavior |
| --- | --- | --- |
| `1087a1c0` | `px_battle_is_kind1_0x78` | Low 3 bits are 1 and bits 3–8 are `0x78` |
| `108797b0` | `px_battle_pack_4` | `(high << 9 & 0x7e00) \| (low << 3 & 0x1f8) \| 4` |
| `1084b664` | `px_battle_double_plus` | `(2 value + (flag & 1)) & 0xff` |
| `108c8548` | `px_battle_four_halves_match` | Any of the halfwords at `+0x11c`, `+0x12a`, `+0x138`, `+0x146` equals a value |
| `108c9a30` | `px_battle_group_slot` | Byte `+0x19 + index` of the 49-byte group `group` (count byte at its start), else `0x1f` for a group of 4 or more or an index at or above the count |
| `1084b590` | `px_battle_default_byte` | Context `+0x1244`: a pointer, a length and an inline default byte at `+8`; the indexed byte when the pointer is set and the index is below the length, else the default |
| `1084bfa0` | `px_battle_has_entry_120c` | `px_battle_find_entry` over the ten 3-byte entries at context `+0x120c` found something |
| `1084b804` | `px_battle_has_set_id` | `px_battle_first_set_id` returned something other than `0x12` |
| `1086fe6c` | `px_battle_half_in_array` | A halfword is among the first `count` halfwords of an array |
| `1086fc8c` | `px_battle_find_word_index` | The index of a word among the first `count` (byte `+0x18`) words of an array, or −1 |
| `108c9a64` | `px_battle_log_contains` | Walks the 400-entry history ring backwards (see battle_core_1's `108c9ac8`) and finds an entry with a given id and value |
| `108acedc` | `px_battle_side_record_set` | A side's layer record (`10b11c00`, 17 records of 16 bytes per side) holds a handler |
| `108ace4c`, `108ace70` | `px_battle_side_flag_clear`, `px_battle_side_flag_set` | Clear or set flag bit 25 of that handler when it exists; 1 when the bit changed. Their bodies include the shared tails `10876e04` and `10877490` |
| `10876cf4` | `px_battle_set_field_3` | For kind 2 or 4 words whose bits 3–8 are below 8, replaces bits 3–8 |
| `1084b990` | `px_battle_set_field_9` | Kinds 1–3 replace bits 9–24 (mask `0x1fffe00`); kind 4 replaces bits 15–30 |
| `1086fd30` | `px_battle_clear_slot_tables` | Zeroes five words of a slot (`+0xc`, `+0x38`, `+0x64`, `+0x248`, `+0x274` plus 4 × slot) and sets ten words from `+0x90` of the 40-byte slot block to `0x1f` |
| `1086fd8c` | `px_battle_clear_entries` | Clears entries 1–5 (word `+0x1c + 4 i`, byte `+0xb4 + i`, and word `+0x40` and byte `+0xbd` with entry 2) for an index below 6, else just that entry |
| `10869870` | `px_battle_clear_tables_10` | Clears ten words, three halfword tables, a byte table and four bytes at `+0x6e`–`+0x71` |
| `1084b3f8` | `px_battle_push_log` | Appends to the ten-entry log at the pointer in context `+0xdc0` (count byte, then bytes at `+1`, `+0xb` and `+0x15`) the two values and the index `px_battle_find_entry` returned; nothing when full |
| `108ae5e0` | `px_battle_remove_move_handlers` | Removes every owner-0 handler registered for a battler's slot, one at a time |

## Evidence

All 1,408 instruction bytes and 24 literal bytes match live REA bytes and the
original `DllBattle.cro`. Three literal words (the layer-record base in
`108acedc`, `108ace4c` and `108ace70`) are ABS32 relocations whose
original words and values are checked; the other literals are the ring's
last index (399) and two masks. `108ace4c` and `108ace70` each have a second
body range (their tail), read separately. The decoder of the
[move-power milestone](../battle_power/README.md#decoder) reproduces every
listing it supports and REA's text stands where it refuses (the
post-indexed `ldr`/`str` forms).

## Validation

`make -C reconstructed/battle_core_2 test` runs 5,720 cases: exhaustive
value tables for the pure helpers, every group, index and array length for
the searches, random words for the bit-field rewrites, every slot for the
clears (with a poison pattern checked untouched), rings with random heads
and gaps, every count and entry position for the log push, and all 64
combinations of registered and unregistered handlers for the cleanup.

ASan/UBSan pass. 85 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) reads
every other milestone's `evidence/functions.json` (committed or on disk,
including battle_low and battle_mid) and confirms that none of these 21
functions is covered elsewhere, that all start inside the batch ranges and
that each is a recorded callee of another milestone's function
([record](reproduction/record-comparison.json)).

## Limits

The remaining frontier functions of these ranges (116 are called by committed
code, 844 are uncovered in all) are not part of this batch. No running-game
comparison has been made. Static evidence and host comparisons do not
establish a matching original build.
