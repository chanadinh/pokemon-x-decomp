# Battle move handlers: event 0xac, second group

This milestone reconstructs 12 more `DllBattle.cro` move handlers of event
`0xac` (the main effect of a status move), bound to 13 (move, event) pairs. It
is the third batch taken from the top of the move table downward (the
battle_moves_N milestones work upward). All 12 were reviewed through live REA
MCP/Ghidra listings (session `outputs/rea-battle-9`). Every handler acts only
when key `0x03` is the user (Helping Hand tests the owner first); key `0x06` is
the first target.

| Address (static) | Host routine | Moves | Effect |
| --- | --- | --- | --- |
| `108ae9c8` | `px_battle_mind_reader_effect` | Mind Reader (170), Lock-On (199) | Condition `0x1d` with the target's turn word, message `0x28b` |
| `108af13c` | `px_battle_electrify_effect` | Electrify (582) | Unless the target has moved: condition `0x24`, message `0x4ab` |
| `108af7d8` | `px_battle_endure_effect` | Endure (203) | Message `0x1ff`, work `+0x18` = 1, kind `0x27` with counter 3 + 1 |
| `108af9f0` | `px_battle_recycle_effect` | Recycle (278) | With no held item, the previous item (`+0x14`) returns (kind `0x21`) |
| `108b0718` | `px_battle_focus_energy_effect` | Focus Energy (116) | Kind `0x17` (9) and message `0x417` unless flag 9 is set |
| `108b0ba0` | `px_battle_role_play_effect` | Role Play (272) | Copies the target's ability (kind `0x20`) unless it is blocked |
| `108b38a4` | `px_battle_disable_effect` | Disable (50) | Condition `0xd` on the target's last move (4 or 5 turns) |
| `108b3b18` | `px_battle_reflect_type_effect` | Reflect Type (513) | Kind `0x13` with the target's types, message `0x447` |
| `108b402c` | `px_battle_psych_up_effect` | Psych Up (379) | Kind `0x10` with two raw stats and flags `| 3`, then kind `0x17` |
| `108b5be0` | `px_battle_healing_wish_effect` | Healing Wish (361) | When a replacement exists: kind `0x1f` (2) and `0x12` |
| `108b6f68` | `px_battle_lunar_dance_effect` | Lunar Dance (461) | The same with field 1 |
| `108b787c` | `px_battle_helping_hand_effect` | Helping Hand (270) | With a partner: message `0x41a`, target saved in work word 0, item marked consumed |

Runtime addresses are static − `0x1015b000`; move numbers are the original
ids, named after the public Generation VI lists. The consumed mark
(`1084c700`) is `battle_items_4`'s, linked rather than repeated.

## Details

- Records are created by `10869f7c` and committed by `10867b48`; messages use
  `1084d2d4` / `1084d29c`. Endure clears flag `0x1000000` of its record's first
  word, Reflect Type sets it.
- Helping Hand asks `10874634` about the owner (`1087425c`) before looking at
  key `0x03`: with one battler or fewer it does nothing.
- Role Play keeps only the low byte of the target's ability (key `0x10`).

## Validation

`make -C reconstructed/battle_moves_b3 test` covers every handler over the
user key (own, other, high-bit id), the services' results and the unwritten
bytes of each record, plus the dispatcher (475 cases). ASan/UBSan pass; 48
mutations are each detected by a failing test (two that did not compile were
replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 12 handlers are bound to exactly 13 (move, event) pairs
([record](reproduction/record-comparison.json)).

## Limits

The turn-flag, condition, status, move, type, owner and message services
remain external. No running-game comparison has been made. Static evidence and
host comparisons do not establish a matching original build.
