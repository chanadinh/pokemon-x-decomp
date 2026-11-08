# Battle move handlers: damage-calculation inputs

This milestone reconstructs the 15 `DllBattle.cro` move handlers that remained
on the damage-calculation events `0x3c`–`0x40`, bound to 20 (move, event)
pairs. Together with [battle_moves_1](../battle_moves_1/README.md), every move
entry of these five events is now reconstructed. Thirteen were reviewed
through live REA MCP/Ghidra listings; Eruption/Water Spout and Helping Hand
have no Ghidra function and are listed by
[`reproduction/decode_mul.py`](reproduction/decode_mul.py), the
[battle_power decoder](../battle_power/README.md#decoder) plus one form, MUL.

| Event | Address (static) | Moves | Effect |
| --- | --- | --- | --- |
| `0x3c` power | `108adae8` | Eruption (284), Water Spout (323) | Base power × HP / max HP, at least 1 |
| `0x3d` multiplier | `108afc9c` | Brine (362) | ×2 when the defender has at most 50% HP |
| | `108b2200` | Charge (268) | ×2 for an Electric move while work word 0 is 2 |
| | `108b2248` | Retaliate (514) | ×2 when an ally is listed in faint record kind 1 |
| | `108b3548` | Venoshock (474) | ×2 against a poisoned defender |
| | `108b682c` | Fusion Flare (558), Fusion Bolt (559) | ×2 after the other one this turn; effect flag 1 |
| | `108b7850` | Me First (382) | ×1.5 |
| | `108b8048` | Solar Beam (76) | ×0.5 in rain, hail or sandstorm |
| | `108b86e4` | Helping Hand (270) | ×1.5 for the battler in work word 0 |
| | `108b8d7c` | Facade (263) | ×2 while poisoned, paralysed or burned |
| | `108ba1dc` | Knock Off (282) | ×1.5 when the defender holds an item that can be removed |
| `0x3e` attack source | `108af710` | Foul Play (492) | Key `0x3c` becomes the defender: its attack is used |
| `0x3f` defense | `108b420c` | Psyshock (473), Psystrike (540), Secret Sword (548) | With key `0x3d` 11, key `0x3e` + 1 (defense instead) |
| | `108b9b44` | Chip Away (498), Sacred Sword (533) | Key `0x53` = 1: the defender's stages are ignored |
| `0x40` attack | `108b8340` | Facade (263) | Key `0x54` = 1: a burn does not halve the attack |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list. Factors are 4096-based
(`0x2000` ×2, `0x1800` ×1.5, `0x800` ×0.5) and go through the event multiply
service on key `0x32`.

## Details

- Every handler except Helping Hand first requires key `0x03` (attacker) to
  equal its battler id. Helping Hand compares key `0x03` with its work word
  0 instead (the battler it helps) and ignores its own id. Charge tests its
  work word before key `0x03`.
- **Eruption** reads HP (`0xd`) and max HP (`0xe`), the move's base power
  through `10801634` (main `0039e044`) from handler `+0x38`, multiplies them
  (32-bit `mul`) and divides unsigned (`1080041c`).
- **Brine** compares `108713a0`'s share (4096 per percent) signed with
  `0x32000`.
- **Retaliate** reads the record at context `+0xca4` (`1081cc04`); the
  index is a byte, so a count above 255 revisits index 0.
- **Fusion Flare and Bolt** take their partner ids from literals `108b68bc`
  (559) and `108b68c0` (558) and ask `108c9b28` for the move last used this
  turn in the history at context `+0x20`; afterwards `1084bef8(context, 1)`.
- **Knock Off** asks `1084bb5c(context, attacker, defender)` whether the item
  is locked.
- **Foul Play, Psyshock, Chip Away and Facade's event `0x40` entry** rewrite
  keys of the attack and defense routines of the
  [stats milestone](../battle_stats/README.md): key `0x3c` (scaled `0x1f`,
  else a battler index), key `0x3e` (an odd value swaps to defense), keys
  `0x53` and `0x54`. Value entries change once per frame, so an earlier
  rewrite of key `0x53` or `0x54` stays.

## Decoder extension

`decode_mul.py` adds `mul{s}{cond} rd, rm, rs` and refuses MLA, long and
halfword multiplies, MUL with `pc` or a nonzero `Rn` field; everything else
goes to the frozen battle_power decoder. Twelve REA analyses from the battle
sessions that list MUL are retained as references, with the 20 MUL words read
live; the verifier requires the extension to reproduce all 20.

## Validation

`make -C reconstructed/battle_moves_2 test` runs the handlers on the real
event stack with each key given the kind its event gives it: no action for
another battler; Helping Hand's work-word comparison; Eruption over HP,
max HP and base-power grids, its 32-bit product and the unsigned divide;
Brine's signed threshold; Charge over work words and types and its test
order; Retaliate over counts and ally patterns, index 255 and the byte wrap;
Venoshock; both Fusion moves against neighbouring move ids; Me First; Solar
Beam over weathers; Facade over statuses; Knock Off over items and locks;
Foul Play's narrowing; Psyshock over stat keys and indexes; Chip Away and the
burn exception including an earlier rewrite; and Helping Hand, Me First and
Venoshock through the real dispatcher (3,737 cases). ASan/UBSan pass; 29
mutations are each detected (one equivalent mutation was replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 15 handlers are bound to exactly 20 (move, event) pairs and that
every move entry of events `0x3c`–`0x40` is a handler of this milestone or of
battle_moves_1 ([record](reproduction/record-comparison.json)).

## Limits

The HP-share, weather, move-data, history, effect-flag, faint-record, ally
and item-lock services remain external. Division by zero is outside the
host's domain. No running-game comparison has been made. Static evidence
and host comparisons do not establish a matching original build.
