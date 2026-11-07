# Battle battler services

This forty-fifth host-source milestone reconstructs nine `DllBattle.cro`
services that the action processor's
[kind routines](../battle_kinds/README.md) call to change a battler. All nine
were reviewed through live REA MCP/Ghidra listings.

| Function | Address (static) | Role |
| --- | --- | --- |
| `px_battle_stage_can_change` | `108722e8` | Whether stat 1–7's stage (signed byte at `+0x104`…`+0x10a`) can move up (below 12) or down (above 0); stat 0 or above 7 faults and reads as 0 |
| `px_battle_set_stage` | `108714fc` | Sets a stage (at most 12; otherwise, or for a bad stat, faults) |
| `px_battle_value_key` | `108620d4` | Exchanges keys 9 and `0xb` while the field effect at `+0x210` has `+0x28c` set |
| `px_battle_set_value` | `10871250` | Sets value key 8–`0xc` (u16 at `+0xf6`…`+0xfe`) after that exchange |
| `px_battle_lose_hp` | `10867a04` | Loses HP (to 0 with `108003fc`), command 1 or 2; nothing while `108007e4` reports bit 4 of the battle setup |
| `px_battle_change_hp` | `10865788` | Restores (16-bit, capped at `+0xe`, command 3) or loses HP; optionally command `0x48` for a positioned battler |
| `px_battle_cure_condition` | `108695c0` | Cures a condition, copying its word out first if asked. A major status (below 6) clears all five with condition 9's word and turns (commands `0x11`, `0x3c`); others go through `1086fd8c` (command `0x12`) |
| `px_battle_status_blocked` | `10862858` | Nonzero when a status cannot be applied: a reason from `1082008c` (reported through `1081cd58`), or event `0x6e` (keys 3, 4, `0x1e`, `0x42`, signed `0x44`). A shown block runs event `0x70` and sets context `+0x1093` bit 4 |
| `px_battle_apply_stage` | `10860e74` | The stat-stage applier |

Runtime addresses are static − `0x1015b000`.

## The stat-stage applier

`px_battle_apply_stage` (`10860e74`) takes twelve arguments, the ones the
stat-stage kind routine passes. In order:

1. **Early exit:** it stops if `10865c4c` reports.
2. **Event `0x63`** (keys 2, 3, `0x20`, `0x21`) may adjust the stages.
3. **A stage that cannot move** gets message `0x99` or `0xae` when the call
   asks for a report.
4. **A protected battler** (`+0x208`) refuses changes from others unless
   forced (message `0xd2`).
5. **Event `0x64`** (keys 2, 3, `0x20`, `0x21`, `0x1a`) may block the change,
   which then runs event `0x65`.
6. **Otherwise:** the message (`1086bbe8`), `1081c0a0`, and event `0x66`.

Nested events keep the action-stack word.

The three small setters dispatch through inner jump tables (`10872304`,
`10871508`, `10871270`, 21 relocated words).

## Validation

`make -C reconstructed/battle_services test` covers 1,223 cases on the real
event stack:

- **Stages:** every stat, stage and direction, and every setter value.
- **Value keys:** each key with and without the exchange.
- **HP loss:** the setup bit, saturation, exactly 0, and both commands.
- **HP change:** restore with cap and 16-bit wrap, loss, report and
  position.
- **Cures:** every condition with and without the out word, including its
  bound.
- **The status check:** 32 combinations of reason, report, block, shown and
  source.
- **The applier:** 512 combinations of every branch above.

ASan and UBSan pass, and each of 32 mutations is detected. One equivalent
mutant was replaced.

[`reproduction/check_services.py`](reproduction/check_services.py) decodes
the original inner jump tables. It confirms that stat 0 faults, that stats
1–7 reach `+0x104`…`+0x10a` in order, and that keys 8–`0xc` store
`+0xf6`…`+0xfe` in order. It also confirms that all 10 calls the
reconstructed kind routines make to these services pass the number of
arguments they take ([record](reproduction/record-comparison.json)).

## Limits

The status applier `108629b8` (next) and the callees named above remain
external. No running-game comparison has been made. Static evidence and
host comparisons do not establish a matching original build.
