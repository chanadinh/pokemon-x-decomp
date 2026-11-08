# Battle move handlers: moves that call other moves

This milestone reconstructs the 10 `DllBattle.cro` move handlers of events
`0x18` (choosing the move another move calls) and `0x19` (announcing it),
bound to 10 (move, event) pairs, so every move entry of both events is now
reconstructed. Eight were reviewed through live REA MCP/Ghidra listings
(session `outputs/rea-battle-7`); Sleep Talk and Metronome's announcement
have no Ghidra function and are listed by the
[battle_power decoder](../battle_power/README.md#decoder).

| Event | Address (static) | Move | Calls |
| --- | --- | --- | --- |
| `0x18` | `108aea60` | Sleep Talk (214) | While asleep, a random one of its own moves that Sleep Talk may call (`108a46d4`) and that does not charge (flag 1); none: failure |
| | `108b0c58` | Assist (274) | A random move of another party member that Assist may call (`108a47b4`); members that neither fainted nor can battle are skipped; none: nothing |
| | `108b4aec` | Metronome (118) | `1089e724` over the pool from `108a48a0` |
| | `108b711c` | Nature Power (267) | By terrain: 1 `0x19c`, 2 `0x249`, 3 `0x55`, 4 `0x38`, 5 `0x3a`; otherwise by environment 0–7: `0xa1`, `0x19e`, `0x198`, `0x19c`, `0x38`, `0x1aa`, `0x20c`, `0x3a` (8+: `0xa1`) |
| | `108b99e8` | Copycat (383) | The battle's last move (`1081cf08`) unless Copycat may not call it |
| | `108ba62c` | Me First (382) | The target's chosen damaging move if it has not moved, is present, is not charging and the move is allowed; otherwise failure |
| | `108bad6c` | Mirror Move (119) | The target's last move when it can be mirrored (flag 6); otherwise failure |
| `0x19` | `108b691c` | Bide (117) | Message `0x2e9` (turn 1) or `0x2ec` (turn 2) naming the user |
| | `108b79c0`, `108b8fd4` | Metronome, Nature Power | Message `0x7e` / `0x7f` naming the called move |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list. Key `0x12` receives the
called move, key `0x0d` its target (from `10871bac`), key `0x42` = 1 a
failure; on event `0x19`, key `0x40` names the message buffer and key `0x53`
= 1 marks the announcement.

## Details

- Every handler requires key `0x02` (the battler) to be its own.
- **Sleep Talk** lists candidates as words in its work area and narrows the
  picked word to 16 bits; **Assist** lists halfwords in a 0x30-byte buffer
  (`1084c7d0`). Both use byte-wide counters and `random(count)` narrowed to a
  byte.
- **Nature Power** dispatches through two relocated jump tables (terrain
  `108b7150`, 6 words; environment `108b7194`, 8 words; Ghidra lists the
  function in three ranges around them). It keeps the original target; with
  none (`0xa`) it calls the target chooser and drops the result.
- **Me First and Mirror Move** take the original target (key `0x0e`); with
  none, the selector `1086de1c(format, opposing selector, 0)`. The battlers
  it names are listed into the work area; Me First copies the chosen action
  to a stack buffer (a logical slot here).

## Validation

`make -C reconstructed/battle_moves_11 test` runs the handlers on the real
event stack: no action for another battler; Sleep Talk over move counts,
bans and charge flags; Assist over 256 party patterns, counts and bans with
its call order; Metronome; Nature Power over every terrain and environment
with and without a target; Copycat; Me First over 256 condition patterns
with and without a target; Mirror Move; the three announcements; and
Copycat and Metronome through the real dispatcher (6,150 cases). ASan/UBSan
pass; 36 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 10 handlers are bound to exactly 10 (move, event) pairs and that
every move entry of events `0x18` and `0x19` is one of them
([record](reproduction/record-comparison.json)).

## Limits

The keys of events `0x18` and `0x19` are taken as value entries (key `0x02`
an id), as the tests give them. The move-list, ban, flag, random, target,
party, buffer, faint, terrain, environment, history, selector, action and
message services remain external. No running-game comparison has been made.
Static evidence and host comparisons do not establish a matching original
build.
