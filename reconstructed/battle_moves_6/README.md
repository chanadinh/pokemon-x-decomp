# Battle move handlers: two-turn moves

This milestone reconstructs 16 `DllBattle.cro` move handlers of two-turn
moves, bound to 19 (move, event) pairs: the charging turn (event `0xa0`),
Skull Bash's charge (`0xa1`) and the release turn (`0xa3`). Every move entry
of these events is now reconstructed except Sky Drop's two (`108b95a0`,
`108ba8b4`), which belong with its handlers of events `0x1f`, `0x26`, `0x2e`,
`0x91` and `0xb6`. Nine handlers were reviewed through live REA MCP/Ghidra
listings (session `outputs/rea-battle-7`); seven have no Ghidra function and
are listed by the
[battle_moves_3 decoder](../battle_moves_3/README.md#decoder-extension).

| Event | Address (static) | Moves | Records |
| --- | --- | --- | --- |
| `0xa0` | `108baf70`, `108baffc` | Fly (19), Bounce (340) | Kind `0x17` in the air (3); message `0x211` / `0x220` |
| | `108b8174` | Dive (291) | Kind `0x17` underwater (4); message `0x217` |
| | `108b9f10` | Dig (91) | Kind `0x17` underground (5); message `0x21a` |
| | `108baee4` | Shadow Force (467), Phantom Force (566) | Kind `0x17` vanished (6); message `0x21d` |
| | `108ba2a0`, `108b8bd4`, `108ba794`, `108ba9cc` | Razor Wind (13), Sky Attack (143), Solar Beam (76), Geomancy (601) | Message `0x223`, `0x226`, `0x229`, `0x4bd` |
| | `108ba948` | Freeze Shock (553), Ice Burn (554) | Message `0x362` for move 553, otherwise `0x365` |
| `0xa1` | `108bb5e8` | Skull Bash (130) | Message `0x22c`; kind `0xd`: Defense (2) +1 for the user |
| `0xa3` | `108bb7d8`, `108bb8a8`, `108b94e8`, `108bb0c4`, `108bb768` | Fly, Bounce, Dive, Dig, Shadow Force / Phantom Force | Nothing: each is a lone `bx lr` |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list. Message ids above `0xff` come
from literal pools (all 40 literal bytes are checked); Bounce's `0x220` and
Skull Bash's `0x22c` are immediates.

## Details

- Every charging handler requires key `0x03` (attacker) to be its battler.
  The messages are kind-3 records whose message (kind 2) names the battler.
- **The semi-invulnerable charges** commit the state record (battler at
  `+5`, state at `+4`) before the message record.
- **Freeze Shock and Ice Burn** share one handler that compares its move id
  (handler `+0x38`, 16 bits) with 553.
- **Skull Bash** reads byte `+0x18` of its stat record first and sets bits 1
  and 2 in it; it stores the battler (`+0xc`), count 1 (`+0x16`), Defense
  (`+4`) and one stage (`+0x17`) in that order.
- **The release handlers** are each a single `bx lr`: the release turn of
  these moves needs nothing from their handlers. The table still binds them.

## Validation

`make -C reconstructed/battle_moves_6 test` runs the handlers on the real
event stack: every charge for its own battler, another one and an id
differing above the low byte; Freeze Shock over five move ids; Skull Bash
over four flag bytes; the release handlers leaving all memory unchanged with
and without a frame; and Dig and Solar Beam through the real dispatcher (45
cases). ASan/UBSan pass; 26 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 16 handlers are bound to exactly 19 (move, event) pairs, that Sky
Drop's two deferred handlers are bound to exactly move 507 on events `0xa0`
and `0xa3`, and that no other move entry of these events exists
([record](reproduction/record-comparison.json)).

## Limits

Sky Drop's handlers remain. The record, commit and message services remain
external. No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
