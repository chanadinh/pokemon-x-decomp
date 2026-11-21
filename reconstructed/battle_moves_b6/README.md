# Battle move handlers: event 0xac, fifth group (upper half)

## The split

Every event-0xac move handler absent from all committed `evidence/functions.json`
files at `5be12cb`, sorted by static address (21 handlers, produced by
`outputs/rea-abilities/split_list.py`). The split point is after the first
ceil(21 / 2) = 11 entries: the lower half belongs to `battle_moves_19`, the
upper half (entries 12 to 21) to this milestone.

|  # | Address | Moves | Half |
| --- | --- | --- | --- |
|  1 | `1084bd3c` | 182, 197 | lower (`battle_moves_19`) |
|  2 | `108add30` | 174 | lower |
|  3 | `108adfb4` | 271, 415 | lower |
|  4 | `108ae1a0` | 180 | lower |
|  5 | `108aeb70` | 156 | lower |
|  6 | `108aeff8` | 166 | lower |
|  7 | `108b0248` | 516 | lower |
|  8 | `108b0468` | 244 | lower |
|  9 | `108b1898` | 385 | lower |
| 10 | `108b1c7c` | 391 | lower |
| 11 | `108b1e4c` | 293 | lower |
| 12 | `108b23bc` | 432 | upper (this milestone) |
| 13 | `108b2b2c` | 384 | upper (this milestone) |
| 14 | `108b2eb8` | 254 | upper (this milestone) |
| 15 | `108b3458` | 226 | upper (this milestone) |
| 16 | `108b40fc` | 375 | upper (this milestone) |
| 17 | `108b42b4` | 502 | upper (this milestone) |
| 18 | `108b481c` | 367 | upper (this milestone) |
| 19 | `108b50c8` | 596 | upper (this milestone) |
| 20 | `108b5620` | 312 | upper, **not done** |
| 21 | `108b87e0` | 215 | upper, **not done** |

Of the ten upper-half handlers, eight are reconstructed here. The remaining two,
`108b5620` (Aromatherapy, a single branch into `108b87e4`) and `108b87e0`
(Heal Bell, a no-op word before the same body at `108b87e4`), are **not**
done: that body (about 0x1a8 bytes, with stack arrays and a loop over both
sides) contains a post-indexed byte load, word `e4d03001` at `108b8950`, which
the shared battle_power decoder refuses and for which REA has no listing (no
Ghidra function there). They need `decode_arm.py` to support that form first.

## Handlers

This milestone reconstructs 8 `DllBattle.cro` move handlers of event `0xac`,
each bound to one move. Four are Ghidra-listed (live REA MCP listings, session
`outputs/rea-battle-9`); four have no Ghidra function and are listed by the
[battle_power decoder](../battle_power/reproduction/decode_arm.py) from bytes
read live (session `outputs/rea-battle-10`).

| Address (static) | Host routine | Move | Effect |
| --- | --- | --- | --- |
| `108b23bc` | `px_battle_defog_effect` | Defog (432) | Evasion −1 for the target, then two side-bit records for both sides |
| `108b2b2c` | `px_battle_power_swap_effect` | Power Swap (384) | Attack and Special Attack stages exchanged (two kind-`0xe` records), message `0x2a4` |
| `108b2eb8` | `px_battle_stockpile_effect` | Stockpile (254) | Below three stockpiles: counters 0 to 2 and Defense / Special Defense +1, message `0x2d1` |
| `108b3458` | `px_battle_baton_pass_effect` | Baton Pass (226) | Kind `0x1f`, `0x17` and `0x2a` records when a replacement exists |
| `108b40fc` | `px_battle_psycho_shift_effect` | Psycho Shift (375) | The user's status goes to a status-free target, then key `0x53` is rewritten |
| `108b42b4` | `px_battle_ally_switch_effect` | Ally Switch (502) | Partner lookup by battle mode (1 or 2), kind `0x33`, message `0x477` |
| `108b481c` | `px_battle_acupressure_effect` | Acupressure (367) | A random stat of the target that can rise by 2 goes up by 2 |
| `108b50c8` | `px_battle_spiky_shield_effect` | Spiky Shield (596) | Protection start and work `+0x18` = 1 (no user check) |

Runtime addresses are static − `0x1015b000`; move numbers are the original
ids, named after the public Generation VI lists.

## Details

- The stat table Acupressure reads is original data: the relocated literal at
  `108b4930` (ABS32, `segment_1` + 19526) points to seven bytes at `108d2c46`,
  1, 2, 5, 3, 4, 6, 7. The host keeps a copy; the original-data check compares
  the module's bytes with it, and the byte read from live REA is retained in
  the evidence.
- Defog's side-bit records add bits to `+5` and `+6` only when the count at `+4`
  (always 4) exceeds a minimum, as the original's inline expansions do.
- Ally Switch looks at the battle mode (`1084bf14`) first: mode 1 needs the
  owner state 4, mode 2 a clear side check; otherwise nothing happens.

## Validation

`make -C reconstructed/battle_moves_b6 test` covers every handler over the
user key (own, other, high-bit id), the services' answers, every record's
unwritten bytes, the Ally Switch and Acupressure lookups and the dispatcher
(401 cases). ASan/UBSan pass; 56 mutations are each detected by a failing test
(six that did not compile or were equivalent were replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 8 handlers are bound to exactly 8 (move, event) pairs and that the
Acupressure table equals the original
([record](reproduction/record-comparison.json)).

## Limits

The side, status, owner, slot, counter and message services remain external.
No running-game comparison has been made. Static evidence and host comparisons
do not establish a matching original build.
