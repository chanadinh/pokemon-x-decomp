# Battle core helpers, batch 1

This milestone reconstructs 28 small `DllBattle.cro` functions that earlier
milestones call but never reconstructed: battler and context accessors,
bit-field helpers, a type query, and two builders that run an event and push
an action record. Several were named as external callbacks in earlier
headers (the battler's current status, the airborne and type queries' inputs,
the record/message/commit callers), so the batch narrows what the handler
milestones still take on trust. Each function has a live REA/Ghidra listing
(session `outputs/rea-battle-10`).

The names are analyst interpretations of what each function does. Runtime
addresses are static − `0x1015b000`.

## Functions

| Address | Function | Behavior |
| --- | --- | --- |
| `1084c5a8` | `px_battle_same_side` | Two slots are on the same side (slot 12 and up is side 1); false when either is `0x1f` |
| `1086f960` | `px_battle_has_type` | Does a battler count as a given type: type bytes `+0x100`/`+0x101` (a type 2 is dropped while the low bits of `+0x7c` are set; a missing second type copies the first; two missing types read as type 0) or the third type byte `+0x102`; the "no type" value `0x12` never matches |
| `1086d8ac` | `px_battle_pokemon_status` | The first of the five condition words at `+0x20`–`+0x30` with a nonzero low 3 bits: 1 to 5, else 0 |
| `1086c2cc` | `px_battle_find_entry` | The index of the first of ten 3-byte entries with a nonzero first byte and a third byte equal to the value, else 10 |
| `10874348` | `px_battle_first_set_id` | The first id of a four-byte table whose bit is set in the bit array at context `+0x169`, else `0x12` |
| `1085daf8` | `px_battle_array_word` | Word `index` of an array whose byte `+0x18` is its length, else 0 |
| `1084b8d4`, `10851c28` | `px_battle_byte_9`, `px_battle_bit_5_of_1b` | A byte reached through the pointer at `+0x1c`; bit 5 of byte `+0x1b` |
| `1084b648` | `px_battle_field_16` | A word whose low nibble is 1 yields its bits 8–23, else 0 |
| `1084d364` | `px_battle_low_half` | The low halfword of the word at `+0x14` |
| `108c9ac8` | `px_battle_count_matches` | Walks a 400-entry history ring backwards from its head index and counts the entries with a given id and value, stopping at the head or an empty entry |
| `1084c818` | `px_battle_bit_set` | A bit of the context's bit array at `+0x166` |
| `1084c7fc`, `108c86c8`, `1084c70c` | `px_battle_half_160`, `px_battle_half_15c`, `px_battle_word_1080_set` | Context halfwords `+0x160` and `+0x15c`; word `+0x1080` is nonzero |
| `1084b784` | `px_battle_first_kind_word` | Of the context's battler entries (count byte `+0x108c`, 16 bytes each from `+0x10bc`), the first for a given slot whose word's low nibble is not 6 or 8: stores the word through a pointer and returns 1 |
| `1084b8e0` | `px_battle_others_flagged` | Every entry for another slot has its flag byte (`+0xd`) set |
| `1086b284` | `px_battle_normalize_word` | Clears the word's low 10 bits, moves bits 10–18 to bits 19–27 and clears bits 26–27 and 28–30 |
| `10876ca8` | `px_battle_set_kind_bit` | Sets bit 25 (kinds 1–3) or bit 31 (kind 4) of a word from a flag; other kinds leave it |
| `10876c28` | `px_battle_mark_if_small` | When bits 7–13 of the halfword at `+2` are below 8, sets its bit 14 and stores a value at `+0x20` |
| `1084bef8` | `px_battle_store_byte_db4` | Stores a byte at `+4` of the object the context holds at `+0xdb4` |
| `1084c3b0`, `10876e20` | `px_battle_pack_1`, `px_battle_pack_3` | `value << 9 \| 1` and `(value << 3 & 0x1f8) \| 3` |
| `1084be58` | `px_battle_event_0x83` | Pushes an event frame, clears key `0x42`, dispatches event `0x83` and pops; 0 when a handler set the key, else the byte at `+1` of the word at `+0x1c` of the context |
| `1084a944` | `px_battle_record_change` | Records a change of a battler's byte `+0x153`: nothing when equal; otherwise a kind `0x3a` record (id at `+4`, value at `+5`, a flag into bit 23 of its header) with message `0xde`, committed |
| `10865c48`, `1084d1c8`, `108676cc` | `px_battle_nop` | Single `cpy r0, r0` stubs |

## Evidence

All 1,436 instruction bytes and 16 literal bytes match live REA bytes and
the original `DllBattle.cro`. One literal word (`108743a4`, the id table of
`10874348`) is an ABS32 relocation whose original word and value are
checked; the other literals are the ring's last index (399), and the
entry-count offset `0x108c` twice. The decoder of the
[move-power milestone](../battle_power/README.md#decoder) reproduces every
listing it supports; it refuses the post-indexed forms of `10874348`, whose
REA text is kept.

## Validation

`make -C reconstructed/battle_core_1 test` runs 77,424 cases: all slot
pairs, every type-byte pair with the flag, third type and asked type, all
mask and value combinations of the condition words, ten-entry arrays and
bit tables from many seeds, history rings with random heads and gaps,
battler entries with random slots, kinds and flags, random words for the
three bit-field helpers, and the event and record builders against the real
event stack and a handler that does and does not set the key.

ASan/UBSan pass. 82 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) reads
every other milestone's `evidence/functions.json` and confirms that none of
these 28 functions is covered elsewhere and that each of the 25 that do real
work is a recorded callee of at least one other milestone's function
([record](reproduction/record-comparison.json)).

## Limits

These remain external:

- the handler-side callbacks of the builders (battler lookup, record stack,
  messages and commit) and the real handlers an event runs
- the other functions that committed milestones call but no milestone covers
  (about 200, many of them 8-byte import thunks), including the VFP
  percentage `108713a0` and the jump-table function `108743a8`

No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
