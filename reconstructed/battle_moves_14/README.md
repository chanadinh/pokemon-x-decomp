# Battle move handlers: events 0x01–0x0e

This milestone reconstructs 16 `DllBattle.cro` move handlers bound to 25
(move, event) pairs: every move entry of the lowest-numbered events `0x01`,
`0x03`, `0x04`, `0x06`, `0x07`, `0x08`, `0x09`, `0x0d` and `0x0e`. These are
the start-of-action and lock bookkeeping of Rage, Destiny Bond, Grudge,
Charge, Protect's family, Uproar, Thrash, Rollout, the scheduling of Future
Sight and Doom Desire, Snatch, Magic Coat and Teleport. Thirteen handlers were
reviewed through live REA MCP/Ghidra listings and three, which have no Ghidra
function, through the bytes read live and listed by
[battle_moves_3's decoder](../battle_moves_3/reproduction/decode_mult.py)
(session `outputs/rea-battle-8`).

| Event | Address (static) | Moves | Effect |
| --- | --- | --- | --- |
| `0x01` | `108b6d8c` | Rage (99) | While active (work word 6) and the user acts with an action kind (key `0x0c`) other than 8, every handler of the move for the user's value byte is removed |
| | `108b9ab8` | Destiny Bond (194), Grudge (288) | The same case removes the handler itself |
| `0x03` | `108b7b50` | Charge (268) | Another move (key `0x12`) marks work word 0 = 2 |
| | `108b7db8` | Protect (182), Detect (197), Endure (203), Mat Block (561), King's Shield (588), Spiky Shield (596) | Unless the user's last move is one of the eight protect-type moves (table `108d4b28`, literal `108b7e4c`), the protect streak resets (record `0x27`) |
| `0x04` | `108b6e48` | Charge | Marked: the handler removes itself |
| | `108b7058` | Uproar (253) | Interrupted (key `0x53` clear): the lock (condition `0x19`) ends |
| | `108b7240` | Thrash (37), Petal Dance (80), Outrage (200) | Counts a turn down (work word 0); interrupted, the lock ends; on the last turn the user is confused (condition 6, message `0x168`); then the move's handlers go |
| | `108b7c58` | Rollout (205), Ice Ball (301) | Interrupted: the lock ends, work word 6 clears and the move's handlers go |
| `0x06` | `108b27d4`, `108b6ca4` | Future Sight (248), Doom Desire (353) | Once key `0x53` is taken, record `0x1f` schedules the attack two turns on; with no target (`0xa`) one is chosen (`1086de1c`) and written to key `0x0d` |
| `0x07` | `108b8418`, `108baabc` | Future Sight, Doom Desire | Messages `0x438` and `0x43b` |
| `0x08` | `108b6494` | Snatch (289) | Once it has taken a move: message `0x2f2` naming both battlers; the handler removes itself |
| `0x09` | `108b8ec0` | Magic Coat (277) | Once key `0x53` is taken and the user can reflect (`1084b804`): message `0x2fc` naming the move |
| `0x0d` | `108b7958` | Teleport (100) | No key test: once key `0x53` is taken, message `0x2ff` |
| `0x0e` | `108ba778` | Uproar | Started (work word 6): the move fails (key `0x42`) |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list.

## Details

- **Removal** goes through the [registration milestone](../battle_handler/README.md)
  and restarts its search after each removal, like Rollout's in
  [battle_moves_12](../battle_moves_12/README.md). Rage and Thrash search by
  the user's value byte (pokemon +0x19), Rollout by the battler id itself.
- **The protect table** is read two halfwords per round. The literal
  `108b7e4c` is an ABS32 relocation; the eight halfwords it points at are
  retained as a data block: 182, 197, 203, 469, 501, 596, 588, 561 (Wide Guard
  and Quick Guard keep the streak too, though they have no handler here).
- **Thrash** looks the user up before testing work word 6, so a missing
  battler aborts even when inactive; the host keeps that order. The
  countdown stops at 0.
- **Future Sight and Doom Desire** are one routine at two addresses. Key
  `0x0d` is read as a byte but rewritten with the full chosen value; the
  record keeps a byte.
- **Uproar's failure check** is seven instructions ending in `bx lr`, which is
  why decode_mult lists it.

## Validation

`make -C reconstructed/battle_moves_14 test` runs the handlers on the real
event stack and handler registry: no action for another battler (keys `0x02`
or `0x03` as each tests); Rage over its flag and action kinds; Destiny Bond;
Charge's mark and end over moves and states; the protect streak over every
table move, others and a changed table; Uproar's interruption; Thrash over
its flag, turns left and interruption, and its lookup order; Rollout's
interruption and value; the schedulers over targets and an earlier
announcement; the messages; Snatch; Magic Coat; Teleport; Uproar's failure;
and Uproar's failure with Teleport through the real dispatcher (105 cases).
ASan/UBSan pass; 45 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 16 handlers are bound to exactly 25 (move, event) pairs and that
every move entry of the nine events is one of them
([record](reproduction/record-comparison.json)).

## Limits

The keys of these events are taken as the tests give them (ids for `0x02`
and `0x03`, values otherwise). The record, message, last-move, condition,
battle-mode, target-selection, turn and reflection services remain external.
No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
