# Battle type matchup

This twenty-third host-source milestone reconstructs the four `DllBattle.cro`
routines that decide how effective a move is against a battler, on top of the
[type-effectiveness](../battle_types/README.md) chart lookup, combination and
application. All four were reviewed through live REA MCP/Ghidra and ARM
instructions.

| Address (static) | Host routine | Behavior |
| --- | --- | --- |
| `10870994` | `px_battle_packed_types` | The battler's three types as 5-bit fields, without Flying while roosting |
| `1086094c` | `px_battle_single_matchup` | One attacking type against one defending type, through event `0x44` |
| `10826264` | `px_battle_matchup` | A move against all defending types, Ground immunity, immunity overrides and the message |
| `10863d44` | `px_battle_matchup_by_index` | The same from battler indices and a move id, without a message |

Runtime addresses are static − `0x1015b000`. Names are analyst
interpretations; type 2 is Flying, 4 is Ground, `0x12` is typeless.

## Packed types (`10870994`)

The types are bytes `+0x100`, `+0x101` and `+0x102`. When the low three bits
of word `+0x7c` are nonzero (Roost), a Flying first or second type becomes
typeless. A typeless second type copies the first. A typeless first type takes
the second, and if both are typeless the battler becomes Normal (type 0), so a
roosting pure Flying type is Normal. The result is
`third << 10 | second << 5 | first`, each masked to five bits.

## Single matchup (`1086094c`)

Event `0x44` receives both ids, the defending type (key `0x15`), the attacking
type (key `0x16`), key `0x4c = 0`, key `0x4d = 0`, key `0x39 = 0xe` and key
`0x17 = 0x12`. The helper `10888a88(defender)` runs just before the dispatch.
Afterwards:

1. Key `0x4d` nonzero: neutral (7).
2. Key `0x39` (low byte) is the code; `0xe` means none was supplied, and the
   chart lookup is used. Other values are used as given.
3. Key `0x17` (low byte) other than typeless adds a second attacking type,
   combined with the first.
4. A zero result with key `0x4c` nonzero becomes neutral.

## Full matchup (`10826264`)

A typeless move is neutral. Otherwise the first type is matched, then the
second and third types are combined in when they are not typeless and differ
from the ones before. A single type with no third type skips both checks.

For a Ground move:

- If the code is nonzero, the field word (`[context + 0x1c] + 0x27c`) is zero
  and `108651d8(context, defender, 0)` reports the defender airborne, event
  `0x46` (key `0x42 = 0`) decides. A nonzero key `0x42` keeps the code;
  otherwise the defender floats and is treated as immune.
- If the code is zero (a Flying type), a set field word or a defender that
  `108651d8(context, defender, 1)` does not report airborne gives neutral (7).

Every immunity (any move) goes to event `0x45`, which can replace it with a
code through key `0x39`. If it stays zero and the caller asked for a message,
`1086b5e0([context + 0xc], 0x6a, 0xd2, defender id, 0xffff0000)` shows it.
For a floating defender, event `0x1b` runs first between saving and restoring
the message state at `context + 0x437c`; the message is skipped when that word
then has bits `0x40000000` and `0x20000000` both set.

## By index (`10863d44`)

Both indices go through the battler table (`[context + 8] + 0x84 + 4 ×
index`), each with the returning abort helper for an index of `0x18` or more
or an empty slot. The nesting counter at `context + 0x1080` is raised for the
call and lowered again afterwards. A defender with flag `0x40` in byte `+0x1b`
is replaced by `10872ff8([context + 4], [context + 8], defender)`. The move
record is built by `10869c5c` into a 20-byte stack area, which the host takes
from the caller as `move_scratch`, and the full matchup runs without a
message.

## Validation

`make -C reconstructed/battle_matchup test` checks packed types for every
pair of type bytes (including out-of-range ones) with five Roost words; the
exact single-matchup sequence, every type pair in normal and inverse mode and
every combination of the four handler results; every defending type
combination for every non-Ground attacking type, with the number of
single-matchup events; exact immune and floating sequences; every Ground
combination of type, field word, airborne result, grounding, override,
message request and message flags (with the message state restored); and the
index path with replacement, the counter and each abort (19,629 cases).
ASan/UBSan pass; 24 mutations of observed conditions, keys, masks and order
are each detected.

## Limits

The event handlers (abilities, items, field effects), the airborne check, the
replacement lookup, move loading, messages and the abort helper remain
external. No running-game comparison of these routines has been made. Static
evidence and host comparisons do not establish a matching original build.
