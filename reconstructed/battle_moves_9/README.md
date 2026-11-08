# Battle move handlers: after the move, and Secret Power

This milestone reconstructs the 13 `DllBattle.cro` move handlers of event
`0x24`, which runs after a battler has used its move, Secret Power's handler
of event `0x8c` and Secret Power's environment routine `108bb8ac`. They are
bound to 20 (move, event) pairs, so every move entry of event `0x24` is now
reconstructed, and with [battle_moves_7](../battle_moves_7/README.md) every
move entry of event `0x8c`. Twelve handlers were reviewed through live REA
MCP/Ghidra listings (session `outputs/rea-battle-7`); Bide, the Thrash family
and `108bb8ac` have no Ghidra function and are listed by the
[battle_power decoder](../battle_power/README.md#decoder).

| Address (static) | Moves | After the move |
| --- | --- | --- |
| `108adbe8` | Bide (117) | Turn 1: condition `0x19` (locked, value from `1084c3b0`), lock state 2; turn 2: lock state 2; then effect flag 1 and the lock ends |
| `108af3e0` | Thrash (37), Petal Dance (80), Outrage (200) | Locked for `random(2) + 2` turns unless already locked |
| `108b7b94` | Rollout (205), Ice Ball (301) | Locked for 5 turns unless locked or asleep; work word 1 = 5, word 0 = 0 |
| `108b1f70` | Rage (99) | Kept while it is used again; otherwise every Rage handler of the battler is removed |
| `108b2168` | Charge (268) | Message `0x298` and lock state 1 after its own use |
| `108afbcc` | Round (496) | The first user this turn records the move (kind `0x31`) |
| `108b0194` | Toxic (92) | Used by a Poison type: condition `0x1c` on the user for one turn |
| `108b39e8`, `108b55d4` | Defense Curl (111), Minimize (107) | State 7 / 8 for the user |
| `108b836c` | Magnitude (222) | `random(100)` picks magnitude 4–10 (table `108d4aa8`): power to work word 0, message `0x6c + i` |
| `108b9f9c` | Grass, Fire, Water Pledge (518–520) | A combined pledge sets key `0x13` to move `0x206`, `0x207` or `0x208` (by work word 1; otherwise 0) |
| `108bab20` | Secret Power (290) | Effect flag = the animation from `108bb8ac` |
| `108bb0c8` | Selfdestruct (120), Explosion (153) | Effect flag 1, then `1081cf14(context, side, side, move, 0, 0)` |
| `108b741c` (event `0x8c`) | Secret Power (290) | When the 30% roll succeeds: a condition, a stat drop or a flinch by `108bb8ac` |
| `108bb8ac` | (routine) | Class, parameter and animation for environments 0–7 (jump table) and terrains 1–3 |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list.

## Details

- Event `0x24` handlers test key `0x02` (the battler); Explosion and Secret
  Power's event-`0x8c` handler test key `0x03`. Work word 6 (`+0x18`) marks a
  lock; key `0x3f` is the move's lock state.
- **The locks** (Thrash, Rollout) add condition `0x19` (kind `0xb`) with the
  value `1084beac(turns, move)` and byte `+0xc` = 0.
- **Rage** removes, like Rollout's power handler, every registered move
  handler of the battler's own id (`+0x19`) with its move id.
- **Magnitude** walks seven (limit, power) pairs; the table is followed by
  two zero bytes, which a roll of 100 or more would select (the host table
  carries them).
- **`108bb8ac(context, class_out, value_out)`** reads the environment
  (`1084b81c`) and dispatches through eight relocated jump-table words:
  0 or 8+ (0, 1, 0), 1 (1, 6, 1), 2 (2, 0, 6), 3 (0, 2, 7), 4 (1, 1, 5),
  5 (1, 5, 2), 6 (0, 3, 3), 7 (0, 3, 4). Terrain (`1084b8d4`) 1, 2 or 3
  overrides with (0, 2, 7), (1, 3, 8) or (0, 1, 9). Null outputs are skipped.
- **Secret Power's hit** skips when key `0x48` is set, asks
  `108261e8(context, id, target, 30)` and then builds: class 0 a condition
  (kind `0xb`, value from `1084c438`), class 1 a one-stage drop (kind `0xd`),
  class 2 a certain flinch (kind `0x2c`).

## Validation

`make -C reconstructed/battle_moves_9 test` runs the handlers on the real
event stack and handler registry: no action for another battler (keys
`0x02` and `0x03` told apart); Bide's three turns; Thrash and Rollout over
lock, sleep and random values; Rage with registered handlers; Charge;
Round over user counts; Toxic over types; Defense Curl and Minimize; every
Magnitude roll from 0 to 255 and wider; the pledges; every environment and
terrain of `108bb8ac` with and without outputs; Secret Power's three effect
classes, roll, key `0x48` and lookup order; Explosion; and Defense Curl and
Minimize through the real dispatcher (443 cases). ASan/UBSan pass; 38
mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 14 handlers are bound to exactly 20 (move, event) pairs and that
every move entry of event `0x24` is one of them
([record](reproduction/record-comparison.json)).

## Limits

The keys `0x13`, `0x3f` and `0x48` are taken as value entries, as the tests
give them. The record, message, status, type, random, history, lock-value,
effect-flag, environment, terrain, effect-roll, status-value and explosion
services remain external. No running-game comparison has been made. Static
evidence and host comparisons do not establish a matching original build.
