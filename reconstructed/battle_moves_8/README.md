# Battle move handlers: move failure

This milestone reconstructs the 11 `DllBattle.cro` move handlers of event
`0x2e`, which decides whether a move fails against its target, bound to 13
(move, event) pairs, so every move entry of the event is now reconstructed
(Sky Drop's included). A handler sets key `0x41` to 1 to make the move fail.
Nine were reviewed through live REA MCP/Ghidra listings (session
`outputs/rea-battle-7`); Nightmare and the Mean Look family have no Ghidra
function and are listed by the
[battle_power decoder](../battle_power/README.md#decoder).

| Address (static) | Moves | Fails when |
| --- | --- | --- |
| `108bb560`, `108af1e4` | Attract (213), Captivate (445) | Either battler is genderless (2) or both share a gender |
| `108afe50`, `108b4b64` | Dream Eater (138), Nightmare (171) | The defender is not asleep (status 2) |
| `108b5494` | Synchronoise (485) | None of the attacker's types is one of the defender's |
| `108b5624` | Sucker Punch (389) | The defender has moved, or has not chosen a damaging move; then message `0x4a` |
| `108b9034` | Rototiller (563) | The defender is airborne (`1084bf24`) |
| `108b96ac` | Sky Drop (507) | The defender is Flying (type 2) |
| `108b9b70` | Worry Seed (388) | The defender has Truant (54) or Insomnia (15) |
| `108ba338` | Spider Web (169), Mean Look (212), Block (335) | The defender is Ghost (type 7) |
| `108bb9cc` | Endeavor (283) | The defender's HP does not exceed the attacker's |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list.

## Details

- Every handler requires key `0x03` (attacker) to be its battler; the
  defender is the low byte of key `0x04`.
- **Attract and Captivate** compare the low bytes of value `0x12`, reading
  the attacker's first; Attract looks the attacker up first, Captivate the
  defender.
- **Synchronoise** unpacks three 5-bit types from `10870994` for each
  battler (bits 0–4, 5–9, 10–14; `0x12` none) and skips the attacker's
  empty slots.
- **Sucker Punch** asks `1084c818(defender, 3)` whether it has moved; if not,
  it copies the defender's chosen action (`1084b784`) to a stack buffer and
  succeeds when `1084b648` names a move that `10800434` reports damaging.
  On every other path it examines the buffer once more, even when nothing
  was copied into it (the host gives it a logical slot that it does not
  clear). When the failure rewrite takes, message `0x4a` goes to the buffer
  named by key `0x40`.
- **Worry Seed** reads the ability a second time for the Insomnia test.
- **Endeavor** subtracts the attacker's HP from the defender's and fails on
  a signed result of zero or less.

## Validation

`make -C reconstructed/battle_moves_8 test` runs the handlers on the real
event stack: no action for another battler; every gender pair including
wide values; the lookup aborts; sleep against other statuses; Synchronoise
over 125 × 42 type words with high garbage bits; Sucker Punch over moved,
chosen action, damage class and stale buffer contents, with the call order
and an earlier rewrite; Rototiller, Sky Drop and Mean Look over ten types;
Worry Seed over abilities; Endeavor over HP grids and signed edges; and Mean
Look and Dream Eater through the real dispatcher (5,425 cases). ASan/UBSan
pass; 25 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 11 handlers are bound to exactly 13 (move, event) pairs and that no
other move entry of event `0x2e` exists
([record](reproduction/record-comparison.json)).

## Limits

Key `0x40` and `0x41` are taken as value entries, as the tests give them;
the routine that dispatches event `0x2e` is not reconstructed here. The
status, type, airborne, turn-flag, action, damage-class and message services
remain external. No running-game comparison has been made. Static evidence
and host comparisons do not establish a matching original build.
