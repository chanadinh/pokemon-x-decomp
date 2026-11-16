# Battle move handlers: protection breakers and event 0x4d

This milestone reconstructs 10 `DllBattle.cro` move handlers bound to 21
(move, event) pairs: the handlers of the moves that break protection (Feint,
Shadow Force, Phantom Force, Hyperspace Fury) on events `0x2c`, `0x32`,
`0x33`, `0x4d` and `0x8d`, and the rest of event `0x4d` (after a move's
damage): Brick Break, Uproar, Pay Day, Self-Destruct and Explosion. With it
every move entry of those five events is reconstructed. Six handlers were
reviewed through live REA MCP/Ghidra listings and four, which have no Ghidra
function, through the bytes read live and listed by the
[battle_power decoder](../battle_power/reproduction/decode_arm.py) (session
`outputs/rea-battle-8`).

| Event | Address (static) | Moves | Effect |
| --- | --- | --- | --- |
| `0x4d` | `108b8a70` | Feint (364) | Defender (key `0x04`): when a protection condition of its side is up or it protected itself (turn flag 7), `1084b0ec` ends the protection, message `0x20e` (literal `108b8b54`) |
| | `108bb43c` | Hyperspace Fury (593) | The same, message `0x208` |
| `0x8d` | `108bb688` | Shadow Force (467), Phantom Force (566) | The same for the target (key `0x06`), message `0x208` |
| `0x33` | `108b8b58` | all four | Key `0x53` set to 1 (the move goes through protection) |
| `0x32` | `108ba084` | all four | No key test: record `0x1b` lifts every protection condition (`1084b3e8`) of the opposing side, byte +9 set |
| `0x2c` | `108bb138` | all four | The same without byte +9 |
| `0x4d` | `108bba4c` | Brick Break (280) | With Reflect or Light Screen (entries 0 and 1 of the side-condition global, literal `108bbb48`) on the defender's side: record `0x1a` removes both, then record `0x3b` |
| | `108aee4c` | Uproar (253) | Unless locked (status `0x19`) or started (work word 6): the lock (condition `0x19`, count `1084beac(3, move)`), message `0x2bf`, and every sleeping opponent wakes (message `0x2c2`) |
| | `108b50e8` | Pay Day (6) | Coins of five times the user's level (`1081ca98`); message `0x80` when added |
| | `108bbd34` | Self-Destruct (120), Explosion (153) | Record `0x12`: the user faints |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list.

## Details

- **The search** for a protection condition walks entries 0–16 of the
  target's side (`108acedc`) and stops at the first that `1084b3e8` reports
  as a protection. Entry 16 would be skipped if register r6 were clear; every
  handler sets it to 1 first, and the host keeps that test as a constant.
  The turn flag is read after the search and or-ed with its result.
- **The mask** of records `0x1a` and `0x1b` is a size byte (4) followed by
  three bytes; condition i sets bit i % 8 of byte 1 + i / 8 when that byte is
  within the size. With the size of 4 these handlers write, the test never
  fails.
- **Uproar** keeps the context in a stacked register (it saves r0–r3 too).
  Its first loop test uses the count `1084bc20` returned, later tests the byte
  stored at the buffer's start, which is re-read each round; the host does the
  same (a count of `0x100` stores 0 yet runs one round).
- **Pay Day** multiplies with `add r1, r0, r0, lsl #2`, i.e. modulo 2³².

## Validation

`make -C reconstructed/battle_moves_13 test` runs the handlers on the real
event stack: no action for another attacker; the three breakers over every
lone condition, protecting or not, and the turn flag, the early stop and
condition 16; the bypass key; both unprotect handlers over 27 masks; Brick
Break over four sides and every Reflect/Light Screen value with neighbouring
entries set; Uproar's lock and start gates, sleep patterns, the stored-count
cases and a count changed during the loop; Pay Day over levels and the prize
result; Explosion; and Explosion with the bypass through the real dispatcher
(376 cases). ASan/UBSan pass; 44 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 10 handlers are bound to exactly 21 (move, event) pairs and that
every move entry of events `0x2c`, `0x32`, `0x33`, `0x4d` and `0x8d` is one
of them ([record](reproduction/record-comparison.json)).

## Limits

The keys of these events are taken as the tests give them (ids for `0x03`,
`0x04` and `0x06`, a value for `0x53`). The record, message, side,
side-condition, protection, turn-flag, status, lock-count, selector, buffer,
side-battler, level and prize services remain external. No running-game
comparison has been made. Static evidence and host comparisons do not
establish a matching original build.
