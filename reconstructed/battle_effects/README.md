# Battle effect routines

This thirty-eighth host-source milestone reconstructs six more
`DllBattle.cro` routines that the
[action processor](../battle_process/README.md) hands record kinds to. All
six were reviewed through live REA MCP/Ghidra listings. With the three in
[battle_kinds](../battle_kinds/README.md), 9 of the 27 kind routines are now
reconstructed.

| Routine | Address (static) | Kind | Effect |
| --- | --- | --- | --- |
| `px_battle_kind_value` | `1081d558` | 5 | `10863f5c` on a positioned, live target with the u16 at `+4`; shows the message at `+8` when it has a kind |
| `px_battle_kind_set_stages` | `1081f648` | `0xe` | Sets all seven stat stages through `108714fc` and byte `+0x155` (at most 3), with command `0xc` |
| `px_battle_kind_clear_drops` | `10823704` | `0x11` | Command `0xd`, then every lowered stage (signed, below 6) returns to 6; succeeds if any did |
| `px_battle_kind_clear_field` | `108266b8` | `0x1a` | Up to 17 field flags cleared through `108ace94`, each with its own message (`1086b5e0`, group `0x69`) |
| `px_battle_kind_side_entry` | `10824fb0` | `0x1c` | A side-table entry through `108712c0` (command `0x24`), else a 6-bit fallback value (`1086f878`, command `0x25`); entry `0xa` through `1086fde0` (commands `0x26`, `0x62`, `0x59`) |
| `px_battle_kind_transfer` | `10822cbc` | `0x2b` | Event `0x73` (key 2) when the first battler has bits in `+0x5c`, then `1082aea4`, command `0x2a`, and command `0x5d` with the position when `+0x208` is set |

Runtime addresses are static − `0x1015b000`.

## Details

- **Kind `0x1a`'s messages** come from an inner jump table (`10826724`, 15
  relocated cases):

  | Flags | Messages |
  | --- | --- |
  | 0–8 | `0x84`–`0xa4` |
  | 11–13 | `0xac`–`0xb4` |
  | 14 | `0xd8` |

  Flags 9, 10, 15 and 16 have no message. The extra word is the literal
  `0xffff0000`.
- **Kind `0x1c`'s fallback** applies only when byte `+0x30` is set. It reads
  bits 3–8 of the word for type 3 and bits 9–14 for type 4, and rejects the
  value `0x1f`.
- **Kind `0x2b`** dispatches event `0x73` inline. It raises the battle
  state's depth (literal `10822e24` → `108fca00`) and runs one handler pass
  with owner 7 without the blocked check (`10864e98(…, 0)`). On leaving the
  outermost level it clears every registration depth. The real
  `px_battle_event_run_handlers` does the pass.

## Validation

`make -C reconstructed/battle_effects test` covers 238 cases:

- **Kind 5:** 32 combinations of source, position, HP, result and message.
- **Kind `0xe`:** position and HP gates, and the `+0x155` bound.
- **Kind `0x11`:** all 128 lowered-stage patterns, a fainted battler, and
  signed stages.
- **Kind `0x1a`:** every flag, with and without success, and the length
  limit.
- **Kind `0x1c`:** the `0xa` entry, the primary path, both fallback types
  and the `0x1f` rejection.
- **Kind `0x2b`:** with and without the event, checking that the blocked
  callback is never consulted and that registration depths are cleared.
- **End to end:** kinds 5 and 0x11 (the latter gated) committed through
  `battle_action` and `battle_process`.

ASan and UBSan pass, and each of 21 mutations is detected.

[`reproduction/check_effects.py`](reproduction/check_effects.py) confirms
from the original module that:

- the processor's jump table hands the six kinds to these routines;
- each original record is large enough for the fields read, and kind 5's
  ends exactly with its eight-argument message;
- the inner jump table decodes to exactly the C message list
  ([record](reproduction/record-comparison.json)).

## Limits

The callees named above, the command writer and the other 18 kind
routines remain external. No running-game comparison has been made.
Static evidence and host comparisons do not establish a matching original
build.
