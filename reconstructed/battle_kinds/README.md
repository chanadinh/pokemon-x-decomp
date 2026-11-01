# Battle kind routines: stat stages, weather, ability change

This thirty-seventh host-source milestone reconstructs the three
`DllBattle.cro` routines that the [action processor](../battle_process/README.md)
hands these record kinds to:

- kind `0xd`: stat stages, pushed by Intimidate, Download and the
  stat-stage handlers;
- kind `0x1e`: weather, pushed by the switch-in weather setters;
- kind `0x20`: ability change, pushed by Trace.

A record pushed by a reconstructed handler now runs through reconstructed
code from commit to effect. Each routine was reviewed through a live REA
MCP/Ghidra listing.

| Routine | Address (static) | Kind | Bytes |
| --- | --- | --- | --- |
| `px_battle_kind_stages` | `10823014` | `0xd` | 944 |
| `px_battle_kind_weather` | `10824774` | `0x1e` | 516 |
| `px_battle_kind_ability` | `10824978` | `0x20` | 544 |

Runtime addresses are static − `0x1015b000`.

## Behaviour

- **Stat stages** (`+4` stat, `+0x17` signed stages, targets at `+0xc`,
  count at `+0x16`, flags at `+0x18`, message at `+0x1c`):
  1. A first pass finds the first target that has a position (`1086c2cc`),
     is alive, and accepts the change (`108722e8`). The ability popup
     (bit 23) is shown for the source once.
  2. A second pass, per target, runs event 5 (keys 3 source, 4 target,
     `0x12`, and `0x53` starting at `1080049c`'s value). Flag bit 4 skips
     the event and counts the change as blocked; a source of `0x1f` counts
     it as allowed.
  3. It then calls the applier `10860e74` with twelve arguments: the stack
     word's low bits, word `+8`, flag bits 1 and 0, the message when bit 3
     keeps it, and the event result.
  4. On success it shows the message unless bit 3 is set.
- **Weather** (`+4` weather, `+5` turns, `+6` clear allowed, message at `+8`;
  field at context `+0x1c` with the weather at `+1` and turns at `+4`):
  - Weather 0 clears it through event `0x82`, if allowed.
  - Otherwise, weathers below 5 are set. The same weather again only
    succeeds when it upgrades a limited weather to permanent (`0xff`).
  - It sends command `0x45`, then runs event `0x86` (key `0x3a`), and event
    `0x87` unless key `0x42` is set.
  - Nested events keep the action-stack word (`1086b284`).
- **Ability change** (`+4` ability, `+6` battler, `+7` force, message at
  `+0xc`):
  - It refuses the two fixed abilities (list `108d10f0` through `1086fe6c`),
    fainted battlers, and the same ability unless forced.
  - It sends command `0x53` and the message, then runs event `0x93` (keys
    2, `0xf` old, `0x10` new).
  - It stores the ability at `+0x14e` between `108679ac` and `108678f8`,
    then sends commands `0x20` and `0x67`.
  - A new ability takes effect through event `0x94` (as on entry). The
    routine finishes with `108677e4`.

The assembly again corrects Ghidra's argument lists. Command `0x45` takes
the turns, and the stat-stage applier takes eight stack words.

## Validation

`make -C reconstructed/battle_kinds test` covers 172 cases on the real
event stack, with handlers on events 5, `0x82`, `0x86`, `0x87`, `0x93` and
`0x94`:

- **Stat stages:** 64 flag, event and result combinations; the initial key
  `0x53`; a source of `0x1f`; three targets with position, HP and
  acceptance skips.
- **Weather:** set and refuse over current weather and turns; bounds; clear.
- **Ability change:** 32 combinations of fixed, HP, force, same ability and
  popup.
- **End to end:** a switch-in weather record and a gated Intimidate-style
  stage record pushed and committed through `battle_action` and
  `battle_process`, which route them here.

ASan and UBSan pass, and each of 25 mutations is detected.

[`reproduction/check_routes.py`](reproduction/check_routes.py) confirms
from the original module that the processor's jump table hands kinds `0xd`,
`0x1e` and `0x20` to these routines, with the low bits for `0xd`. It also
confirms that the original record sizes make each record end exactly with
its eight-argument message, and that the ability-list literal relocates to
`108d10f0` ([record](reproduction/record-comparison.json)).

## Limits

The stat-stage applier, the ability hooks, the command writer and the
other 24 kind routines remain external. No running-game comparison has
been made. Static evidence and host comparisons do not establish a
matching original build.
