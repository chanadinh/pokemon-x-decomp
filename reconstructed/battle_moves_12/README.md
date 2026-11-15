# Battle move handlers: Sky Drop and lock releases

This milestone reconstructs 11 `DllBattle.cro` move handlers bound to 23
(move, event) pairs: Sky Drop's four remaining handlers and the handlers
that release a move's lock or report its outcome on events `0x21` (after the
move), `0x23` (on success) and `0x26` (after a miss). With it every move
entry of events `0x21`, `0x23`, `0x26`, `0x91` and `0xb6` is reconstructed,
and Sky Drop (507) is complete across all its events. All eleven were
reviewed through live REA MCP/Ghidra listings (session `outputs/rea-battle-7`).

| Event | Address (static) | Moves | Effect |
| --- | --- | --- | --- |
| `0xa0` | `108b95a0` | Sky Drop (507) | Copies the action data (key `0x40`, five words) to the global state `10b11be0`; lifts the target (`1081cb44`) with message `0x464`, or fails (key `0x42`; key `0x50` when the lift reports a block) |
| `0xa3` | `108ba8b4` | Sky Drop | Fails unless the carried battler (counter 4 − 1) is present with an opposing side; lands (`108683e4`) and records the drop in work words 0 and 1 |
| `0x26` | `108b9558` | Sky Drop | After a miss (key `0x02`), releases the carried battler once (`1084af8c`) |
| `0x91`, `0xb6` | `108ba140` | Sky Drop | When interrupted (key `0x03`), the same |
| `0x21` | `108b3618` | Bide (117) | Ends its lock (condition `0x19`) when work word 6 is set |
| | `108b7610` | Protect (182), Detect (197), Endure (203), Mat Block (561), King's Shield (588), Spiky Shield (596) | Resets the protect streak (kind `0x27`) and removes the handler |
| `0x21`, `0x26` | `108b8df8` | Rollout (205), Ice Ball (301) | Ends the lock and removes every handler of the move for the battler |
| `0x26` | `108b9138` | Jump Kick (26), High Jump Kick (136) | Crash damage of half the user's HP (`1084d2f4`), message `0x38c` |
| `0x23` | `108b7adc`, `108b80d8` | Heal Bell (215), Aromatherapy (312) | Message `0x73` / `0x74` (no key test) |
| | `108b8110` | Grass, Fire, Water Pledge (518–520) | A combined pledge (work word 0) announces itself, message `0xc1` |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list.

## Details

- **The lift** reads the five words with one `ldmia` and writes them with
  one `stmia`, so all are read before any is written; the host keeps that
  order. The block flag is a stack byte cleared first (a logical slot here)
  and passed as `1081cb44`'s fifth, stacked, argument. The message record is
  built only when the announcement rewrite of key `0x53` takes.
- **The drop** keeps the candidate as its target even when the move fails on
  the selector test; without a candidate the target is `0x1f`.
- **Removal** goes through the [registration milestone](../battle_handler/README.md);
  Rollout restarts its search after each removal, like its power handler in
  [battle_moves_1](../battle_moves_1/README.md).

## Validation

`make -C reconstructed/battle_moves_12 test` runs the handlers on the real
event stack and handler registry: no action for another battler (keys
`0x02` and `0x03` as each tests); the lift over targets, results, block
flags and an earlier announcement, and an overlapping copy; the drop over
every carried value and both selector results; the releases; Bide's lock;
Protect's reset and removal with a second handler kept; Rollout's removal
of two matching handlers among others; the crash; the three messages; and
Bide with Heal Bell's done handler through the real dispatcher (123 cases).
ASan/UBSan pass; 30 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 11 handlers are bound to exactly 23 (move, event) pairs and that
every move entry of events `0x21`, `0x23`, `0x26`, `0x91` and `0xb6` is one
of them ([record](reproduction/record-comparison.json)).

## Limits

The keys of these events are taken as the tests give them (ids for `0x02`,
`0x03` and `0x06`, values otherwise). The record, message, counter,
selector, Sky Drop, and HP-fraction services remain external. No
running-game comparison has been made. Static evidence and host comparisons
do not establish a matching original build.
