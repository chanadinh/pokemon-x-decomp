# Battle outcome routines

This thirty-ninth host-source milestone reconstructs five more
`DllBattle.cro` routines that the
[action processor](../battle_process/README.md) hands record kinds to. All
were reviewed through live REA MCP/Ghidra listings. With
[battle_kinds](../battle_kinds/README.md) and
[battle_effects](../battle_effects/README.md), 14 of the 27 kind routines
are now reconstructed.

| Routine | Address (static) | Kind | Effect |
| --- | --- | --- | --- |
| `px_battle_kind_set_values` | `10821d98` | `0x10` | For each mask bit (`+0xf`), sets battler value 8–`0xc` from the u16s at `+4` through `10871250`, with command `0x14`; then the message at `+0x10` |
| `px_battle_kind_faint` | `1081cf54` | `0x12` | Faints a positioned battler (alive, or forced by `+5`): message, event `0xb7` (keys 4, 3), HP `+0x10` set to 0, `108003fc`, commands 4 and `0x4b`, then `108646c4` with the source frame |
| `px_battle_kind_leave` | `10824288` | `0x2a` | Unless `10865c4c`, a pending mark (`+0x171` 1–`0x18`), `+0xa0` bits or context `+0x18` refuse: message `+4`, `10866c68`, `10810cc0` on the slot (`10873164`), message `+0x28`, then context `+0x18` = 1 |
| `px_battle_kind_heal` | `1081e0f8` | `0x2d` | Adds the u16 at `+6` to HP (16-bit, capped at `+0xe`), command 3, moves a pending battler into the 24-entry list at context `+0xd98`, message, and refreshes its position (`1086c720`, command `0x44`, `1086c500`) |
| `px_battle_kind_swap` | `10820f3c` | `0x33` | Swaps two live battlers of one side by their party indices (`1086fc8c` on the side's 28-byte party, sides below 4): `10867e74`, message, `10867d64` |

Runtime addresses are static − `0x1015b000`.

Kind `0x12` passes its source as a frame struct holding the source battler
word and the u16 at `+6`. The host places this struct at 8 bytes at the
API's scratch address. The pointer is passed only when there is a source.

## Validation

`make -C reconstructed/battle_outcomes test` covers 228 cases:

- **Kind `0x10`:** every mask.
- **Kind `0x12`:** 32 combinations of position, HP, force, source and the
  u16, on the real event stack.
- **Kind `0x2a`:** 64 combinations of the refusals, the flag and the result.
- **Kind `0x2d`:** heal capping and 16-bit wrap, the pending list (present,
  absent, full) and the position refresh.
- **Kind `0x33`:** same battler, different sides, a fainted battler,
  negative indices (either one) and the side bound.
- **End to end:** a heal and a gated faint committed through `battle_action`
  and `battle_process`.

ASan and UBSan pass, and each of 24 mutations is detected.

[`reproduction/check_outcomes.py`](reproduction/check_outcomes.py)
confirms from the original module that the processor's jump table hands
the five kinds to these routines. It also confirms that each original
record size equals the fields read: four end with their eight-argument
message, and kind `0x2a` holds two messages and two bytes
([record](reproduction/record-comparison.json)).

## Limits

The callees named above, the command writer and the other 13 kind
routines remain external. No running-game comparison has been made.
Static evidence and host comparisons do not establish a matching original
build.
