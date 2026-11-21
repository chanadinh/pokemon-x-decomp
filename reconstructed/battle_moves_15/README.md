# Battle move handlers: events 0x15–0x27

This milestone reconstructs 11 `DllBattle.cro` move handlers bound to 16
(move, event) pairs: every move entry of the next lowest-numbered events,
`0x15`, `0x1a`, `0x1c`, `0x1d`, `0x22`, `0x25` and `0x27`. Nine handlers were
reviewed through live REA MCP/Ghidra listings and two, which have no Ghidra
function, through the bytes read live and listed by the
[battle_power decoder](../battle_power/reproduction/decode_arm.py) (session
`outputs/rea-battle-8`).

| Event | Address (static) | Moves | Effect |
| --- | --- | --- | --- |
| `0x15` | `108b22d0` | Focus Punch (264) | Record `0x15` (kind 5); unless `1084b804` holds for the user, record `0x38` (halfword `0x2ad`, literal `108b23b8`; the user's side; `0xa`; 1); message `0x268` |
| `0x1a` | `108b91bc` | Snatch (289) | Another battler's move with flag 5, the user without status `0x21`: keys `0x02` and `0x04` become the user and work word 0 is set |
| `0x1c` | `108b9d90` | Heal Bell (215), Helping Hand (270), Aromatherapy (312) | Key `0x53` set |
| `0x1d` | `108b8c84` | Snore (173), Sleep Talk (214) | Fail (key `0x42`) when key `0x23` is 2 |
| `0x22` | `108b9e14` | Struggle (165) | Message `0x347` (literal `108b9e74`) |
| `0x25` | `108b3834` | Roost (355) | Condition `0x18` (record `0xb`), data `1084beac(1, 2)` marked by `10876ca8` |
| | `108b7a88` | Perish Song (195) | Message `0x7d` |
| `0x27` | `108b6af0` | Spit Up (255), Swallow (256) | The Defense and Sp. Def stages Stockpile gave (counters 1 and 2) are removed (record `0xd`) and their counters reset; with a stockpile (counter 0), it resets with message `0x2d4` |
| | `108b7ebc` | Fling (374) | Unless handled already (work word 0), the held item (+0x12) is used up (record `0x24`) |
| | `108b9bdc` | Natural Gift (363) | The item is used up |
| | `108ba040` | Self-Destruct (120), Explosion (153) | Record `0x12`: the user faints |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list.

## Details

- **Snatch** tests the attacker's low byte, the move id's low halfword and
  status `0x21`, then rewrites key `0x02` (a value on this event); only when
  that rewrite takes does it rewrite key `0x04` and set work word 0.
- **Spit Up and Swallow** read all three counters before any record. Each
  stat record keeps its byte +0x18 and sets bit 2, and stores minus the stages
  as a byte (`rsb`).
- **Roost's** data is `1084beac(1, 2)`; the same service gives lock counts
  elsewhere (Uproar, battle_moves_13).
- The state check `1084b804` is the service battle_moves_14 names
  `reflection_blocked` for Magic Coat; here it is kept under a neutral name.

## Validation

`make -C reconstructed/battle_moves_15 test` runs the handlers on the real
event stack: no action for another battler (keys `0x02` or `0x03` as each
tests); Focus Punch over the state check; Snatch over its own move, the flag,
the status and an earlier rewrite; the support moves; Snore over key `0x23`;
Struggle, Roost and Perish Song; Spit Up/Swallow over all 27 combinations of
three counter values; Fling over work word 0 and held items; Natural Gift;
Explosion; and Explosion with Natural Gift through the real dispatcher (81
cases). ASan/UBSan pass; 41 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 11 handlers are bound to exactly 16 (move, event) pairs and that
every move entry of the seven events is one of them
([record](reproduction/record-comparison.json)).

## Limits

The keys of these events are taken as the tests give them (ids for `0x02`
and `0x03`, except key `0x02` on event `0x1a`; values otherwise). The record,
message, state-check, move-flag, status, condition-data, condition-flag and
counter services remain external. No running-game comparison has been made.
Static evidence and host comparisons do not establish a matching original
build.
