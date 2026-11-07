# Battle command writer

This forty-third host-source milestone reconstructs `1086c22c`, the
`DllBattle.cro` routine every reconstructed battle routine uses to report an
effect to the display side, together with the queue encoder `1084dbf4` and
its five writers and packers. All seven were reviewed through live REA
MCP/Ghidra listings. A command sent by the action processor or a kind
routine now becomes the bytes the original queues.

| Function | Address (static) | Role |
| --- | --- | --- |
| `px_battle_command` | `1086c22c` | Command id below `0x6d`. Looks up the info byte (`108d13fc`), copies its low nibble of word arguments into the buffer (`10b0f460`), encodes |
| `px_battle_command_encode` | `1084dbf4` | Writes the id as a big-endian u16, then the arguments in one of 32 formats chosen by the info byte; format 0 and any other value fault |
| `px_battle_queue_u16` / `_u24` / `_u32` | `10862078`, `1084d930`, `1084db70` | Big-endian writers |
| `px_battle_pack3` | `1084d9a0` | Three fields into 2 or 3 bytes |
| `px_battle_pack2` | `1084dab0` | Two fields into one byte |

Runtime addresses are static − `0x1015b000`.

## Queue and formats

- **The queue** (the context's command state, `+0xc`) holds a word
  position at `+0` and data from `+8`, 6000 bytes.
- **Overflow:** every write checks the position against a limit that
  leaves room for its width. Bytes stop at `0x1770`, u16 at `0x176f`, u24
  at `0x176e` and u32 at `0x176d`. A full queue calls `108003a4`, after
  which the write still happens.
- **Formats** (the info byte as a whole):
  - Formats 1–9 are that many bytes or packed fields.
  - Formats `0x11`–`0x16` mix bytes, packed fields and u16 words.
  - `0x21`/`0x22` carry u32 words.
  - `0x23`–`0x26` and `0x33`–`0x36` carry 5-bit fields packed into u16 or
    u24 words.
  - `0x30` has no arguments.
  - `0x42`–`0x44` and `0x53`/`0x54` mix bytes with u16 or u32 words.
- **The packers** fault on zero widths and on values that do not fit,
  using signed compares as the original does. The three-field packer
  checks its third width plus 10 against the size, as the original does.
- **Arguments:** the original is variadic. Here callers pass an array, and
  arguments they do not supply read as 0; the original reads whatever
  follows.

## Validation

`make -C reconstructed/battle_command test` covers 333 cases:

- **Writers:** byte order and each limit's overflow edge.
- **Packers:** results, every fault, signed values and widths over 8.
- **Formats:** every format byte for byte, the field checks of the
  bit-packed ones, and all 256 info values against the format list.
- **The writer:** the byte id, the bound, missing arguments, nine
  arguments and info 0.

ASan and UBSan pass, and each of 29 mutations is detected.

[`reproduction/check_commands.py`](reproduction/check_commands.py)
confirms from the original module that:

- the literals point at the info bytes and the buffer;
- all 104 encoded commands use a format whose argument count equals their
  info byte's low nibble (the other 5 are format 0 or message formats);
- the jump table is relocated, with format 0 at the fault exit.

It also confirms that all 56 literal command call sites in the seven
reconstructed routine milestones (36 distinct commands) pass exactly the
number of arguments the original declares. This independently checks every
argument list read from assembly in those milestones
([record](reproduction/record-comparison.json)).

## Limits

The message routines (`1086b5e0`, `1086bbe8`), the display side that reads
the queue, and the fault routines remain external. No running-game
comparison has been made. Static evidence and host comparisons do not
establish a matching original build.
