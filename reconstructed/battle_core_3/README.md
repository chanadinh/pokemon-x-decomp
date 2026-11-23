# Battle core helpers, batch 3

This milestone reconstructs 19 small `DllBattle.cro` functions in
`10840000`–`10880000` and `108a6000`–`108ce000`. Ten are on the frontier
(committed milestones already call them). The other nine belong to the same
cluster of packed-word and record helpers at `10870000`–`10876000` and have no
committed caller yet. None has an uncovered callee. Each has a live REA/Ghidra
listing (session `outputs/rea-battle-10`).

The names are analyst interpretations of what each function does. Runtime
addresses are static − `0x1015b000`.

## Packed words and records

A packed word has a kind in its low 3 bits (1 to 4): kinds 1–3 keep a 16-bit
field in bits 9–24 and kind 4 in bits 15–30
([battle_core_2](../battle_core_2/README.md) rewrites the same words).

| Address | Function | Behavior |
| --- | --- | --- |
| `10873610` | `px_battle_pack_5` | Builds `1 \| (second << 4 & 0xf0) \| first << 8`, stores it, then sets or clears bit 28 from a flag |
| `108704ac` | `px_battle_pack_6` | Stores `3 \| (first << 4 & 0x70) \| (second << 7 & 0x380)` |
| `10871408` | `px_battle_unpack_field` | The 16-bit field of a kind 1–3 word (bits 9–24) or kind 4 word (bits 15–30), else 0 |
| `10873fd8` | `px_battle_push_record` | A record keeps a count nibble in the high half of byte `+3`; adds a word at `+4 + 4 count` and bumps the count, up to 8 entries |
| `10874010` | `px_battle_init_record` | Sets the halfword id at `+0`, zeroes the eight words `+4`–`+0x20`, stores the count nibble in byte `+3` and 0 in byte `+2` |
| `108747fc`, `1085fa8c` | `px_battle_copy_8`, `px_battle_copy_6` | Copy 8 bytes (two words) or 6 bytes (a word and a halfword) |

## Queries and accessors

| Address | Function | Behavior |
| --- | --- | --- |
| `10870368` | `px_battle_pair_search` | A value below 4 is among the ten bytes at context `+0x7ec` |
| `1087425c` | `px_battle_slot_group` | Slot to group: 0–5 give 0, 6–11 give 2, 12–17 give 1, 18–23 give 3; anything else calls the abort routine and gives 0 |
| `108717dc` | `px_battle_scaled_amount` | An amount at context `+0x804` doubled by each of two flags (`+0xa50`, `+0xa51`) and capped at 99,999, unless a flag in `+0x81c` bit 3 skips scaling; 0 when a byte behind the pointer at `+0x10` is set |
| `1087198c` | `px_battle_free_of_conditions_2_3` | The battler exists, has HP (pokemon value `0xd`) and neither condition word `+0x24` nor `+0x28` is active |
| `10873524` | `px_battle_usable_item` | The held item (halfword `+0x12`) unless context `+0x290` or battler `+0x68` has bits set, or the ability (value `0x11`) is `0x67` |
| `10870434` | `px_battle_party_has_hp` | Whether the first or second party member (if the party has more than 1 or 2) has HP: stores `0x16` or `0x26` and returns 4, else 0 |
| `1084bb2c`, `1084c374`, `108520e4`, `10855c68`, `1084af00` | `px_battle_byte_16d`, `px_battle_entry_274`, `px_battle_byte_11e`, `px_battle_half_11c`, `px_battle_is_side_0` | Bounds-checked accessors that call the abort routine (`10800394`) on a bad index and continue |
| `108ace94` | `px_battle_side_record_clear` | Removes the handler a side record holds and clears the record and its kind bits; 1 when it held one |

## Evidence

All 1,148 instruction bytes and 8 literal bytes match live REA bytes and the
original `DllBattle.cro`. One literal word (`108aced8`, the layer-record base)
is an ABS32 relocation whose original word and value are checked; the other
literal is the 99,999 cap. The decoder of the
[move-power milestone](../battle_power/README.md#decoder) reproduces every
listing it supports and REA's text stands where it refuses (`mul` and the
post-indexed `ldr`/`str` forms).

## Validation

`make -C reconstructed/battle_core_3 test` runs 2,929 cases: exhaustive
word and field tables for the packers, every record count and nibble, every
slot value and index for the accessors (counting abort calls), all mask
combinations for the scaled amount (with the cap), and every combination of
HP, condition and party size for the pokemon queries against a value
callback.

ASan/UBSan pass. 69 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) reads
every other milestone's `evidence/functions.json` (committed or on disk) and
confirms that none of these 19 functions is covered elsewhere, that all start
inside the batch ranges, and that the ten functions chosen as frontier are recorded callees of other
milestones ([record](reproduction/record-comparison.json)).

## Limits

The rest of the cluster at `10870000`–`10876000` is mostly UI code (objects
with virtual methods, a trace marker store at `+0x6c` of a global) and is not
part of this batch. When this batch was made, 825 functions remained uncovered in its ranges (706
in `10840000`–`10880000`, 119 from `108a6000` up), 125 of them called by
committed code. No running-game comparison has been made. Static evidence and
host comparisons do not establish a matching original build.
