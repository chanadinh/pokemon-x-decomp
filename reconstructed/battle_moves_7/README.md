# Battle move handlers: effects after a hit

This milestone reconstructs 11 `DllBattle.cro` move handlers of event `0x8c`,
which runs after a move has hit, bound to 15 (move, event) pairs, and the
shared Smack Down routine `108b65f8`. Every move entry of the event is now
reconstructed except Secret Power's (`108b741c`), which relies on a terrain
routine (`108bb8ac`, with a jump table) and is deferred. Nine handlers were
reviewed through live REA MCP/Ghidra listings (session
`outputs/rea-battle-7`); Scald, Thousand Waves and `108b65f8` have no Ghidra
function and are listed by the
[battle_power decoder](../battle_power/README.md#decoder).

| Address (static) | Moves | Effect |
| --- | --- | --- |
| `108aecac` | Scald (503), Steam Eruption (592) | Every hit target still standing and frozen thaws (cure record, status 3) |
| `108af56c` | Thief (168), Covet (343) | With no item of its own and work word 0 clear, takes the first target's item unless locked; message `0x427`; then a kind-`0x3b` mark |
| `108afd74` | Pluck (365), Bug Bite (450) | Eats the first target's berry: removal (message `0x308`) and use by the attacker |
| `108b11a4` | Smack Down (479) | `108b65f8` for the first target |
| `108b6438` | Thousand Arrows (614) | `108b65f8` for every target |
| `108b65f8` | (routine) | Grounds an airborne target (condition `0x1f`, message `0x46e`) unless charging or already grounded, ends Magnet Rise and Telekinesis, and brings it down from the air (state 3) |
| `108b6528` | Thousand Waves (615) | Traps every target not already trapped (condition `0x16`, value from `1089e638`, message `0x36b`) |
| `108b3088` | Circle Throw (509), Dragon Tail (525) | Forces the first target out; message `0x2c7` for move 509, otherwise `0x2c8` |
| `108b45bc` | Fell Stinger (565) | Attack +2 when any target fainted |
| `108b9700` | Knock Off (282) | Knocks off the first target's item unless locked; message `0x420` |
| `108b97fc` | Smelling Salts (265) | Cures the first target's paralysis |
| `108bbd78` | Wake-Up Slap (358) | Wakes the first target |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list. The record layouts are in
[`battle_moves_7.h`](battle_moves_7.h).

## Details

- Every handler requires key `0x03` (attacker) to be its battler. Key `0x05`
  counts the targets hit and keys `0x06 + i` name them (low byte; `0x1f`
  none). Thief and Smelling Salts test the first target against `0x1f`;
  Pluck, Knock Off, Smack Down and Wake-Up Slap do not.
- **Thief** reads the target's item twice (for the test and for the
  message) and marks the take with a kind-`0x3b` record (`+4` = 1, flag
  `0x1000000`).
- **Pluck** commits a removal record (kind `0x21`) and then a use record
  (kind `0x23`: battler, flag 1, the item, flag `0x1000000`).
- **`108b65f8(context, id, target)`** looks the target up first, asks
  `1084c030` (charging) and `1084bf24` (airborne), tests statuses `0x21`,
  `0x1e` and `0x20`, and tests state bit 3 through `1084ca4c` (from the
  [modifier milestone](../battle_modifier/README.md)). The final record
  carries a message only when the target was not grounded just before. It
  returns whether the target was grounded; the handlers ignore it.
- **Fell Stinger** counts fainted targets (`1085f844`) and builds a stat
  record (Attack, 2 stages, flag bit 2) after reading its flag byte.

## Validation

`make -C reconstructed/battle_moves_7 test` runs the handlers on the real
event stack: no action for another battler; Scald over every combination of
up to three standing, fainted and frozen targets; Thief over every
combination of its six conditions; Pluck; the Smack Down routine over
charging, airborne, the three statuses, the frozen bit and the air state,
with its call order; Thousand Arrows and Thousand Waves over target counts
and patterns; Circle Throw and Dragon Tail; Fell Stinger over counts,
patterns and flag bytes; Knock Off over locks and items; Smelling Salts and
Wake-Up Slap over statuses; and Wake-Up Slap and Knock Off through the real
dispatcher (568 cases). ASan/UBSan pass; 32 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 11 handlers are bound to exactly 15 (move, event) pairs, that
Secret Power's deferred handler is bound to exactly move 290 on event
`0x8c`, and that no other move entry of the event exists
([record](reproduction/record-comparison.json)).

## Limits

Secret Power's handler remains. The record, commit, message, status, faint,
berry, item-lock, charging, airborne and trap services remain external. No
running-game comparison has been made. Static evidence and host comparisons
do not establish a matching original build.
