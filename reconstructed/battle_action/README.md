# Battle action services

This thirty-fifth host-source milestone reconstructs the `DllBattle.cro`
services that every handler milestone so far left external. They are the
action stack that handlers push records onto, the commit that executes and
pops a record, the message builders, the ability popup and the temporary
buffer. All seven functions were reviewed through live REA MCP/Ghidra
listings. With them, a handler's output is a real record in battle memory
rather than a call to a stand-in. The tests run Download, Frisk and Forewarn
end to end on these services.

| Function | Address (static) | Role |
| --- | --- | --- |
| `px_battle_action_record` | `10869f7c` | Push onto the context's stack (context + `0x437c`); falls through into the push |
| `px_battle_action_push` | `10869f88` | Look up the kind's size, zero the record, write its header, count the bytes |
| `px_battle_action_commit` | `10867b48` | Execute the record (`10819600`), then pop it; it must be the top record |
| `px_battle_ability_popup` | `1084c948` | Push and commit a header-only record of kind 1 (show) or 2 (hide) |
| `px_battle_message` | `1084d2d4` | Start a message: id, kind, no arguments |
| `px_battle_message_arg` | `1084d29c` | Append a word argument; at most eight |
| `px_battle_temp_buffer` | `1084c7d0` | The context's buffer at `+0x467a`; 0x140 bytes or more faults |

Runtime addresses are static − `0x1015b000`.

## Layout

- **Stack word** (context + `0x437c`): bits 10–18 count the bytes in use.
  Records start at context + `0x4380` and may fill up to `0x1f4` bytes. The
  push and pop change only that field.
- **Record header**: kind in bits 0–7, battler id in bits 8–12, size in
  bytes in bits 13–22. Bit 23 starts cleared (handlers set it as a flag),
  and bit 26 is always set. The rest of the record is zeroed.
- **Size table**: 61 `(kind, size)` byte pairs at `108d1ed8`, reached
  through the relocated literal `1086a0cc`. Sizes are rounded up to a
  multiple of 4. An unknown kind, a size of 0 or a full stack calls the
  fault routine `10800394(0, 0)`, and the push returns 0.
- **Message**: a u16 id, then a u16 holding the kind (bits 0–6) and the
  argument count (bits 7–13). Starting a message keeps bits 14–15. Up to
  eight word arguments follow from `+4`. Further arguments are dropped.
- **Commit** reads the header after executing the record. It faults
  (without popping) unless the record's size is at most the bytes in use
  and the record ends exactly at the top of the stack.

The action processor `10819600` (5,696 bytes) and the fault routine stay
external.

## Validation

`make -C reconstructed/battle_action test` runs 378 cases:

- **Push:** every kind of a synthetic size table, with and without ids,
  including the rounding, the zeroing, the header bits and preserved stack
  bits. Also first-match lookup, unknown kinds, the last table entry, and
  the capacity edge.
- **Commit:** execution before the pop, refusing a record that is not on
  top, a size changed by execution, a size above the bytes in use, and pops
  across bit 8 of the count.
- **Popup:** both phases.
- **Message:** kind and flag masking, eight arguments and a dropped ninth.
- **Temporary buffer:** its limit.
- **End to end:** Download, Frisk with every combination of three held
  items, and Forewarn through the real temporary buffer, each leaving the
  stack empty.

ASan and UBSan pass, and each of 24 mutations is detected. One further
mutation was equivalent and replaced.

[`reproduction/check_sizes.py`](reproduction/check_sizes.py) reads the
original table. It confirms that the literal points at the table, that its
61 kinds are distinct with nonzero sizes, and that every kind the
reconstructed handlers allocate is large enough for the fields they write.
Kinds 3, `0x20` and `0x3a` are exactly long enough for a message with eight
arguments at their offsets ([record](reproduction/record-comparison.json)).
The tests use a synthetic table; no original table data is retained.

## Limits

The action processor, the fault routine and random numbers remain external.
Earlier handler milestones still take these services as callbacks. The
tests here connect them to the real services. No running-game comparison
has been made. Static evidence and host comparisons do not establish a
matching original build.
