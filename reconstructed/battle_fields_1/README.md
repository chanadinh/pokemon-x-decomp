# Battle side-effect handlers

This milestone reconstructs every handler of the `DllBattle.cro`
side-effect table (`108d4fe8`, 17 rows of 12 bytes: an id, a table function
and a maximum layer count). That is 20 handlers for the conditions that
cover one side of the battle, such as screens, hazards and guards. It also
reconstructs the four small helpers they share.

Seven handlers and the four helpers were reviewed through live REA
MCP/Ghidra listings in session `outputs/rea-battle-9`. Thirteen handlers
have no Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder) plus two
narrow rules for forms it refuses (see Evidence).

The row names below are analyst interpretations of what each row does; the
ids are the table's. A handler's id parameter is the side (0 or 1) it was
registered for, and a battler slot belongs to side 1 from slot 12 up.

| Row | Address (static) | Name | Event | Effect |
| --- | --- | --- | --- | --- |
| 0 | `108ac688` | Reflect | `0x4f` | A non-critical physical hit on the side: multiplier ×0.5, or the literal `0xaac` (about ×0.67) in every battle mode except 0 and 3 |
| 1 | `108aca28` | Light Screen | `0x4f` | The same for special hits |
| 2 | `108acc94`, `108acc24` | Safeguard | `0x6e`, `0x70` | A major status, confusion (6) or `0xe` from another battler is blocked (key `0x42`); event `0x70` shows it once (message `0x34a`) |
| 3 | `108acbc8`, `108acb2c` | Mist | `0x64`, `0x65` | A stat drop (key `0x21` negative) from another battler is blocked; event `0x65` shows it once per move (message `0x34d`) |
| 4 | `108ac16c` | Tailwind | `0x13` | The side's speed ×2 |
| 5 | `108ac568` | Lucky Chant | `0x3b` | Key `0x42` = 1 for the battler hit |
| 6 | `108ac460` | Spikes | `0x5e` | A battler that is not airborne loses 1/8, 1/6 or 1/4 of max HP for 1, 2 or 3 layers |
| 7 | `108ac300` | Toxic Spikes | `0x5e` | Not airborne: a Poison type absorbs them (kind `0x1a`); another battler is poisoned, badly from the second layer |
| 8 | `108ac8fc` | Stealth Rock | `0x5e` | Max HP scaled by the Rock effectiveness against the battler's types, through a 14-entry jump table of divisors from 1/256 to 1/1 |
| 9 | `108ac704` | Wide Guard | `0x2e` | A damaging move aimed at all opponents or battlers (target class 4 or 5) is blocked |
| 10 | `108ac5e4` | Quick Guard | `0x2e`, `0xb2` | A priority move that move flag 3 covers, from another battler, is blocked |
| 11 | `108ac02c` | Rainbow | `0x6d`, `0x59` | The attacker's side doubles key `0x27` (16 bits) |
| 11 | `108ac5b0` | Rainbow | `0x75` | Key `0x46` = 1 for the attacker's side |
| 12 | `108ac1c8` | Sea of Fire | `0x7f` | A present, non-Fire battler loses 1/8 (animation kind `0x38`, then kind 6) |
| 13 | `108ac0d4` | Swamp | `0x13` | The side's speed ×0.25 |
| 14 | `108ac850` | Sticky Web | `0x5e` | A battler that is not airborne loses one Speed stage (kind `0xd`) |
| 15 | `108acaa4` | Mat Block | `0x2f`, `0xb2` | A damaging move is blocked, with message `0xd5` and key `0x53` |
| 16 | `108ac7a4` | Crafty Shield | `0x2e` | A non-damaging move whose category is not 8 or 6, from another battler, is blocked |

| Address | Helper | Behavior |
| --- | --- | --- |
| `1084bfc0` | `px_battle_slot_side` | The side of a slot: 1 from slot 12 |
| `1084bf04` | `px_battle_major_status` | A status below 6 |
| `1084bf14` | `px_battle_mode` | The battle mode: byte `+1` behind context `+4` and then `+0x10` |
| `108802a8` | `px_battle_message_flag` | Set bit 15 of the halfword at `+2` of a message buffer |

Runtime addresses are static − `0x1015b000`.

## Details

- **Layer counts** live at `10b11c00` (a relocated literal): 17 rows of 16
  bytes per side, with the layer byte at `+0xc` of each. A handler's row is
  its value halfword (`+0x38`) masked to a byte; a row of 17 or more, or a
  side of 2 or more, faults and reads as 0. The maximum layers are 3 for
  Spikes and Mat Block's row and 2 for Toxic Spikes.
- **Blocking** works through the event key `108658d4` rewrites: key `0x41`
  for the guards and key `0x42` for Safeguard, Mist and Lucky Chant. A
  value already rewritten once does not take, and the handler then does
  nothing more. Safeguard and Mist keep the answer in work word 0.
- **The guards** write their message into the buffer that key `0x40` points
  to: message `0x31d` (Wide Guard), `0x320` (Quick Guard), `0x323` (Crafty
  Shield) or `0xd5` (Mat Block, with the move).
- **Toxic Spikes' absorb record** (kind `0x1a`) is filled by a sequence of
  pre-indexed stores: `+8` the side, `+4` 4, `+5` 0, `+6` and `+7` 0, and
  then `+5` gets bit 7 because `+4` is above 1.
- **Stealth Rock's table** maps effectiveness codes 0–13 to divisors 8,
  `0x100`, `0x100`, `0x80`, `0x40`, `0x20`, `0x10`, 8, 4, 2, 1, 1, 1 and 1.
  Any other code gives 8.

## Evidence

All 3,008 instruction bytes (including the 14 jump-table words) and 52
literal bytes match live REA bytes and the original `DllBattle.cro`. Sixteen
words are ABS32 relocations whose original words and values are checked:
the literals `108ac45c` and `108ac560` (the layer array) and the 14
jump-table words `108ac950`–`108ac984`, kept as `.word` data aimed into
their own handler.

The listings of the 13 decoded handlers come from `decode_arm.py` with:

- a `bx<cond>` rule (no REA-listed function of this batch needs it)
- a `str`/`strb` rule for a pre-indexed, written-back store (Toxic Spikes'
  record), which must reproduce REA's listing of the same form at four
  recorded references

Every instruction shape they use (registers, immediates, literal addresses
and branch targets abstracted) is REA-listed in this batch or at one of 18
spelling references, each checked against REA's listing and the live word.

## Validation

`make -C reconstructed/battle_fields_1 test` runs 7,411 cases:

- **The helpers:** every slot and status below `0x40`, all 256 battle
  modes and 256 message-flag values.
- **Reflect and Light Screen:** 25 defender slots on both sides, four
  categories, both critical answers and five battle modes.
- **Safeguard and Mist:** slots, attackers, every status below `0x12`, every
  drop sign and a rewrite that does not take. Also the show handlers over
  work words and remembered moves.
- **Speed, Lucky Chant and the Rainbow handlers:** 26 slots for both sides,
  and six chance values for the doubling.
- **Spikes and Toxic Spikes:** every layer count, both sides, airborne, rows
  that fault and max HPs; the absorb record is checked byte by byte against a
  record pre-filled with `0xaa`.
- **Stealth Rock:** every effectiveness code 0–20 and `0xffffffff`.
- **The guards, Sea of Fire and Sticky Web:** every early return, a rewrite
  that does not take, and every battler slot.
- **Dispatcher:** Tailwind and Safeguard through the real dispatcher.

ASan/UBSan pass. 90 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) decodes
the 17 rows of the original table and confirms that they hold exactly these
20 handlers, bound to 23 (row, event) pairs, with the maximum layers above.
It also confirms that the four helpers are bound to nothing in any of the
original tables, that the jump table has the 14 expected targets, and that
every handler is listed in `evidence/functions.json`
([record](reproduction/record-comparison.json)).

## Limits

These remain external:

- the airborne, slot-present, battle-decided, position, type, effectiveness,
  move-value, damaging-move and move-flag queries
- the record stack, messages and commit

The 17 four-instruction table functions and the registrar that installs a
side effect (`108acd0c`) are not reconstructed here, and neither are the
position-effect table's handlers. No running-game comparison has been made.
Static evidence and host comparisons do not establish a matching original
build.
