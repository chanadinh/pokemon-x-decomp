# Battle move handlers: the lower half of event 0xac's last handlers

## The split

Both halves of the last event-`0xac` move handlers are taken from one
deterministic list: every move-table handler of event `0xac` that is in no
committed `evidence/functions.json` as of `5be12cb`, sorted by static address
(21 handlers). The first ceil(21/2) = **11** are this milestone
(`battle_moves_19`); the other **10** are `battle_moves_b6`'s and `battle_moves_b7`'s (b7: Heal Bell `108b87e0` and Aromatherapy `108b5620`; its shared body `108b87e4` is not a table handler).

| # | Address | Half | Moves |
| --- | --- | --- | --- |
| 1 | `1084bd3c` | 19 | 182, 197 |
| 2 | `108add30` | 19 | 174 |
| 3 | `108adfb4` | 19 | 271, 415 |
| 4 | `108ae1a0` | 19 | 180 |
| 5 | `108aeb70` | 19 | 156 |
| 6 | `108aeff8` | 19 | 166 |
| 7 | `108b0248` | 19 | 516 |
| 8 | `108b0468` | 19 | 244 |
| 9 | `108b1898` | 19 | 385 |
| 10 | `108b1c7c` | 19 | 391 |
| 11 | `108b1e4c` | 19 | 293 |
| 12 | `108b23bc` | b6 | 432 |
| 13 | `108b2b2c` | b6 | 384 |
| 14 | `108b2eb8` | b6 | 254 |
| 15 | `108b3458` | b6 | 226 |
| 16 | `108b40fc` | b6 | 375 |
| 17 | `108b42b4` | b6 | 502 |
| 18 | `108b481c` | b6 | 367 |
| 19 | `108b50c8` | b6 | 596 |
| 20 | `108b5620` | b7 | 312 |
| 21 | `108b87e0` | b7 | 215 |

Split point: after entry 11 (`108b1e4c`). [`reproduction/check_bindings.py`](reproduction/check_bindings.py)
recomputes the list from the move table and all evidence except the three split
participants (`battle_moves_19`, `b6`, `b7`), so it does not depend on what is
on disk; it checks that the lower ceil(n/2) are the 11 here and that the upper
half is exactly the move-table handlers of `b6` and `b7`, and that with all three
every move-table handler is covered.

## What this milestone reconstructs

11 `DllBattle.cro` move handlers bound to 13 (move, event) pairs on event
`0xac`, the effect of the move itself. Ten have no Ghidra function and were
reviewed through the bytes read live and listed by the
[battle_power decoder](../battle_power/reproduction/decode_arm.py); only Psych
Up has a REA/Ghidra listing (session `outputs/rea-battle-10`).

| Address (static) | Moves | Effect |
| --- | --- | --- |
| `1084bd3c` | Protect (182), Detect (197) | Record `0x15` (kind 7); the protect streak (counter 3) grows by one (record `0x27`, bit 24 of its first word cleared); message `0x205` (literal `1084be38`). The first three instructions are a thunk shifting the arguments |
| `108add30` | Curse (174) | A Ghost user with HP left and a target loses half its HP (records `0xb` and 7, message `0x42e`, literal `108adf10`); a Ghost always raises the effect flag; others lower Speed and raise Attack and Defense (three records `0xd`) |
| `108adfb4` | Trick (271), Switcheroo (415) | The two items swap unless forbidden (owner 1 without `108544ec`, no item, a locked item, or four `1085441c` tests): record `0x25`, messages `0x2aa` and `0x2ad` (literals `108ae198`, `108ae19c`) |
| `108ae1a0` | Spite (180) | The target's last move (`1084c278`) loses up to 4 PP (record 9), message `0x281` (literal `108ae280`) |
| `108aeb70` | Rest (156) | Unless at full HP: sleep (condition 2, 3 turns, message `0x27e`, literal `108aeca8`), the missing HP is healed (record 4), record `0x22`, key `0x53` set to 0 |
| `108aeff8` | Mimic (166) | The target's last move is copied into the user's Mimic slot unless none, Mimic, Struggle, `0x1c0`, banned or blocked (record `0x26`, message `0x2b0`) |
| `108b0248` | Bestow (516) | The user's item goes to a target without one (two records `0x21`, message `0x45d`, literal `108b03d0`) unless locked or a form cannot hold it |
| `108b0468` | Psych Up (244) | The target's seven stat stages and flag byte are copied to the user (record `0xe`), record `0x17` or `0x18` (kind 9), message `0x41d` (literal `108b05f0`) |
| `108b1898` | Guard Swap (385) | The user's and target's Defense and Sp. Def stages trade (two records `0xe`), message `0x2a7` (literal `108b1a68`) |
| `108b1c7c` | Heart Swap (391) | All seven stages and the flag byte trade (two records `0xe`, the user's saved in a 28-byte buffer), message `0x2a1` (literal `108b1e48`) |
| `108b1e4c` | Camouflage (293) | The user's type word becomes the terrain's (jump table `108b1e80`), refined by the environment's (jump table `108b1ed8`), unless it already holds it (record `0x13`) |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list.

## Details

- **Camouflage's jump tables** (`108b1e80`, eight words; `108b1ed8`, six words)
  are ABS32 relocations of code addresses between its instructions; the
  verifier checks each original word and S+A value (14 relocations) and the
  listing is split into three instruction segments around them. The type word
  is `t | t << 5 | 0x4800` (a five-bit type, repeated once).
- **Mimic** reads the target's last move with `1084c7fc`, then with `1084c278`
  and keeps the second whenever they differ (the compare and conditional move
  in the listing).
- **Guard Swap** reads the user's Defense and Sp. Def stages before any
  record and writes them into the target's record; Heart Swap saves the
  user's seven stages in the buffer first and writes them into the second
  record.
- **Curse** treats the HP as a signed word: a negative value counts as no HP.

## Validation

`make -C reconstructed/battle_moves_19 test` runs the handlers on the real
event stack: no action for another attacker; the streak and record bits;
Curse over type, HP and target; Trick and Switcheroo over every item,
lock and block combination; Spite over slots and PP; Rest over HP and its
three records; Mimic over every refusal and both last-move services; Bestow;
Psych Up; Guard and Heart Swap with every stage; Camouflage over all ten
terrains, eight environments and the current type; and two handlers through
the real dispatcher (589 cases). ASan/UBSan pass; 128 mutations are each
detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 11 handlers are bound to exactly 13 (move, event) pairs on event
`0xac` ([record](reproduction/record-comparison.json)).

## Limits

The keys of this event are taken as the tests give them (ids for `0x03` and
`0x06`). The record, message, item, type, stage, PP, last-move, terrain,
environment, buffer and turn-word services remain external. No running-game
comparison has been made. Static evidence and host comparisons do not
establish a matching original build.
