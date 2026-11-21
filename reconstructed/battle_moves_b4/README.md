# Battle move handlers: event 0xac, third group

This milestone reconstructs 10 more `DllBattle.cro` move handlers of event
`0xac` (the main effect of a status move), each bound to one move. It is the
fourth batch taken from the top of the move table downward (the
battle_moves_N milestones work upward). All 10 were reviewed through live REA
MCP/Ghidra listings (analyses in session `outputs/rea-battle-9`, byte reads in
`outputs/rea-battle-10`). Every handler acts only when key `0x03` is the user;
key `0x06` is the first target.

| Address (static) | Host routine | Move | Effect |
| --- | --- | --- | --- |
| `108ae618` | `px_battle_encore_effect` | Encore (227) | Locks the target's last move (condition `0x17`, 3 or 4 turns), message `0x22f` |
| `108b0a10` | `px_battle_mimic_effect` | Mimic (102) | Kind `0x26` replaces the move's slot with the target's, message `0x2b0` |
| `108b0fdc` | `px_battle_conversion_2_effect` | Conversion 2 (176) | A random type resisting the target's last move (+`0x157`) and not already held (kind `0x13`) |
| `108b1b44` | `px_battle_belly_drum_effect` | Belly Drum (187) | Half the max HP (kind 7) buys the Attack boost (kind `0xd`), message `0x265` |
| `108b2038` | `px_battle_pain_split_effect` | Pain Split (220) | Both battlers' HP to the average (kind 7) |
| `108b2a4c` | `px_battle_memento_effect` | Memento (262) | Two stat records (−2) for the target, then kind `0x12` |
| `108b3708` | `px_battle_guard_split_effect` | Guard Split (470) | Raw stats 9 and `0xb` of both averaged (kind `0x10` twice), message `0x451` |
| `108b3f00` | `px_battle_power_split_effect` | Power Split (471) | Raw stats 8 and `0xa` averaged, message `0x44e` |
| `108b5a48` | `px_battle_topsy_turvy_effect` | Topsy-Turvy (576) | Every stage `v` of the target becomes `12 − v` (kind `0xe`), message `0x499` |
| `108b5cc4` | `px_battle_shell_smash_effect` | Shell Smash (504) | Stats 2 and 4 −1, 1, 3 and 5 +2 (five kind-`0xd` records) |

Runtime addresses are static − `0x1015b000`; move numbers are the original
ids, named after the public Generation VI lists.

## Details

- Records are created by `10869f7c` and committed by `10867b48`; messages use
  `1084d2d4` / `1084d29c`. The helper services (last move, move checks, raw
  stats, the temporary list buffer, the resist-type table, the random draw
  and the byte move `1080042c`) are external callbacks.
- **Conversion 2** removes from the resisting-type list every type the user
  already has (moving the tail down with `1080042c`), then draws one with
  `1084c7bc`; the packed type halfword is `t | t << 5 | 0x4800`.
- **Guard Split / Power Split** average with a signed halving (the add of the
  sign bit before the shift); the first record names the target in `+0xe`, the
  second the user.
- **Topsy-Turvy** builds the seven mirrored stages in a 0x1c-byte temporary
  buffer and stores only their low bytes in the record.

## Validation

`make -C reconstructed/battle_moves_b4 test` covers every handler over the
user key (own, other, high-bit id), the services' results, every record's
unwritten bytes and the Conversion 2 list walk, plus the dispatcher (1693
cases). ASan/UBSan pass; 57 mutations are each detected by a failing test
(three that did not compile or were equivalent were replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 10 handlers are bound to exactly 10 (move, event) pairs
([record](reproduction/record-comparison.json)).

## Limits

The move, status, type, stat, random and message services remain external. No
running-game comparison has been made. Static evidence and host comparisons do
not establish a matching original build.
