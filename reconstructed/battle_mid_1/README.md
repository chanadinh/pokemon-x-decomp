# Battle mid-band helpers, batch 1

This milestone reconstructs 18 `DllBattle.cro` functions in the static band
`10880000`–`108a6000` that committed milestones call but never reconstructed
(the frontier of the band: its uncovered functions that have a recorded
caller). Eleven test a value against a halfword list through `1086fe6c`, one
returns a list, and six do small computations or build records. Each has a
live REA/Ghidra listing (session `outputs/rea-battle-10`). The band holds
443 procedures.

The names are analyst interpretations. Runtime addresses are static −
`0x1015b000`.

## Functions

| Address | Function | Behavior |
| --- | --- | --- |
| `10882188` | `px_battle_mid_effect_record` | Looks up a kind in a table of 17 six-byte entries (id, two signed halfwords; literal `108822fc`) and, when an entry has a non-negative value (the second one when a value is given, else the first), builds an action-argument record: halfword id at +0, header at +2 (kind 2, count of words in bits 7–13, bits 14–15 kept), then up to eight words: the battler's byte `+0x19`, the given value, and for kind 8 the owner of a word (`10871408`) or for kind `0x12` the constant `0x49` |
| `10888a88` | `px_battle_mid_pending_flags` | Three condition words of an object (`+0x60`, `+0x70`, `+0x98`) with a nonzero kind in their low 3 bits set keys `0x4c` or `0x4d` (key `0x15` against the first word's owner; kinds 1–3 test bit 25, kind 4 bit 31; key `0x16` equal to 4 for the others) |
| `1089e5ac` | `px_battle_mid_free_slots` | The slots 0–17 whose status (`10860c80`) is 0 or 6, stored as bytes; returns how many |
| `1089e5f8` | `px_battle_mid_scale_12` | `a * b / 4096`, rounded half down, at least 1 |
| `1089e724` | `px_battle_mid_random_unused` | Collects the values 1–`0x269` that are not among a halfword list into a buffer (a global's `+0x58`), then picks one by `108003dc` of the global's random word (`+0x54`); 0 when none |
| `108a46c4`, `108a4714`, `108a47f4`, `108a4804`, `108a4814`, `108a4824`, `108a4844` | `px_battle_listed_NNNN` | Tail calls of `1086fe6c` on one list each (21, 7, 6, 6, 7, 11 and 6 halfwords) |
| `108a4724`, `108a47b4` | `px_battle_listed_4724`, `_47b4` | Two lists in turn (14 then 25; 14 then 32 halfwords); 1 when either holds the value |
| `108a4764` | `px_battle_listed_4764` | Six halfwords read directly, in pairs |
| `108a4854`, `108a48e4` | `px_battle_listed_4854`, `_48e4` | Lists of 2 then 10 halfwords; `4854` returns 0 for `0x19` |
| `108a48a0` | `px_battle_mid_list_1476` | Stores `0x36` through its argument and returns the address of a list of `0x36` entries |

## Evidence

All 1,304 instruction bytes, 80 literal bytes and 408 data bytes match live
REA bytes and the original `DllBattle.cro`. Nineteen literal pool words are
ABS32 relocations (pointers to the halfword lists and to the global at
`108fca00`) whose original words and values are checked; the fourteen
halfword lists are retained as data blocks and sit side by side in the
module (`108d25a0`–`108d2664`), which confirms their sizes. The list returned
by `108a48a0` (entry size unknown) and the global are not retained. The
decoder of the [move-power milestone](../battle_power/README.md#decoder)
reproduces every listing it supports and refuses six forms (a multiply and
`bx lr`), whose REA text is kept.

## Validation

`make -C reconstructed/battle_mid_1 test` runs 5,411 cases: every rounding
case of the scaling, 4,096 status patterns of the slot collector, the list
tests with every service result, the unused-value picker over many lists
and random words (including a full list), the record builder over every
table entry, header, value and special kind (and a negative-value entry),
and the pending flags over every combination of kinds, bits, owners and key
values. ASan/UBSan pass. 80 mutations are each detected (the record
builder's eight-word limit is unreachable, since the header is rebuilt with
a count of 0 and at most three words follow).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) reads
every other milestone's `evidence/functions.json` and confirms that none of
these 18 functions is covered elsewhere, that each starts in the band and
that each is a recorded callee of another milestone's function
([record](reproduction/record-comparison.json)); the record holds only facts
that later milestones cannot change (callers only ever increase).

## Limits

The callbacks (`1086fe6c`, `10860c80`, `10871408`, `108003dc`), the lists'
contents, the global's random word and buffer, and the returned list of
`108a48a0` remain external. The other 424 − 18 uncovered functions of the
band are not covered. No running-game comparison has been made. Static
evidence and host comparisons do not establish a matching original build.
