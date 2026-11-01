# Battle change routines

This fortieth host-source milestone reconstructs four more `DllBattle.cro`
routines that the [action processor](../battle_process/README.md) hands
record kinds to. All were reviewed through live REA MCP/Ghidra listings.
With [battle_kinds](../battle_kinds/README.md),
[battle_effects](../battle_effects/README.md) and
[battle_outcomes](../battle_outcomes/README.md), 18 of the 27 kind routines
are now reconstructed.

| Routine | Address (static) | Kind | Effect |
| --- | --- | --- | --- |
| `px_battle_kind_condition` | `1081dd1c` | 6 | For a positioned, live target: optionally requires `10874348` = `0x12`, then event `0x89` (key 4, key `0x53` starting at 1; clearing it refuses). Then an optional `1086a6e0` and `108671c0`, wrapped by the source's popup |
| `px_battle_kind_hp` | `1081f7a0` | 7 | For up to ten targets: signed HP changes (restore with a 16-bit cap and command 3, or damage through `10867a04`), command `0x48` for a positioned battler, and the held-item event `0x9c` (key `0x2f` = 1) keeping the stack word |
| `px_battle_kind_status` | `1081f1a0` | `0xb` | Unless `10862858` reports a blocker, applies through `108629b8` (quiet when the message has no kind and `+0xd` is 0), wrapped by the source's popup |
| `px_battle_kind_types` | `10822e28` | `0x13` | Sets three 5-bit types (not for species 493, Arceus) with command `0x17`. The popup shows only when the types change (the pair in either order, and the third). Then message `0x383` naming a single new type, or the generic message `0x4a` |

Runtime addresses are static − `0x1015b000`.

## Details

- **The source** is the header battler. With the popup flag and no source
  (id `0x1f`), kinds 6 and `0xb` read the id byte at address `0x19`, as the
  original does; the tests set that byte so the effect is visible.
- **Kind 7's result** is 1 once any target got past the position and HP
  checks.
- **Kind `0x13`'s single-type message** needs the first two types equal, no
  third type (`0x12`), and a packed value different from the battler's
  previous packed types (`10870994`). The generic branch sets no stack
  arguments for `1086b5e0` in the original; the host passes four. The
  literals are `0x383` (`10823010`) and `0xffff0000` (`1082300c`).

## Validation

`make -C reconstructed/battle_changes test` covers 580 cases:

- **Kind 6:** 128 combinations, checking that the event stack is balanced
  on every path.
- **Kind 7:** 64 combinations over three targets (restore, damage, zero),
  plus none processed and 16-bit wrap.
- **Kind `0xb`:** 128 combinations, plus the quiet flag.
- **Kind `0x13`:** 256 combinations over type sets (unchanged, swapped,
  third changed, single), popup, both messages and Arceus.
- **End to end:** an HP record and a gated type record committed through
  `battle_action` and `battle_process`.

ASan and UBSan pass, and each of 24 mutations is detected.

[`reproduction/check_changes.py`](reproduction/check_changes.py) confirms
from the original module that the processor's jump table hands the four
kinds to these routines. It also confirms that the original record sizes
match the fields read: kinds 6 and `0xb` end with their eight-argument
message, kind 7 holds exactly ten targets, and kind `0x13` holds byte `+8`
([record](reproduction/record-comparison.json)).

## Limits

The callees named above, the command writer and the other nine kind
routines remain external. No running-game comparison has been made.
Static evidence and host comparisons do not establish a matching original
build.
