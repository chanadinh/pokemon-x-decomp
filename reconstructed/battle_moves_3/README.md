# Battle move handlers: fixed damage and type matchups

This milestone reconstructs the 20 `DllBattle.cro` move handlers of the
fixed-damage event `0x4e`, its follow-up `0x50` and the type-matchup events
`0x38` and `0x43`–`0x46`, bound to 22 (move, event) pairs, so every move
entry of these seven events is now reconstructed. It also reconstructs the
shared Counter routine `1084b028` and the handler-word setters `1084aef8`
and `1084aeec`. Seventeen routines were reviewed through live REA
MCP/Ghidra listings; six are listed by
[`reproduction/decode_mult.py`](reproduction/decode_mult.py), the
[battle_moves_2 decoder](../battle_moves_2/README.md#decoder-extension)
plus UMULL and BX.

| Event | Address (static) | Moves | Effect |
| --- | --- | --- | --- |
| `0x4e` damage | `108b2e8c`, `108b5414` | Sonic Boom (49), Dragon Rage (82) | 20, 40 |
| | `108b4578` | Seismic Toss (69), Night Shade (101) | The attacker's level (low byte) |
| | `108b2cfc` | Psywave (149) | Level × (`random(101)` + 50) / 100, 16 bits, at least 1 |
| | `108b5b88` | Super Fang (162) | Half the defender's HP (signed), 16 bits, at least 1 |
| | `108b1824` | Endeavor (283) | Defender HP − attacker HP; 1 when negative |
| | `108bab60` | Final Gambit (515) | The attacker's HP |
| | `108b68c4` | Bide (117) | From work word 0 = 2: twice work word 1; then word 0 = 3 |
| | `108b94c0`, `108bacfc`, `108bb3a4` | Counter (68), Mirror Coat (243), Metal Burst (368) | `1084b028`: physical ×2, special ×2, any ×1.5 |
| | `108ba304` | Brick Break (280) | Its hook (`108baba4`) in handler word `+0xc` |
| `0x50` | `108bb244` | Brick Break (280) | Clears handler word `+0xc` |
| `0x43` | `108af540`, `108bb87c` | Thunder Wave (86), Thousand Arrows (614) | Key `0x53` = 1 |
| `0x44` matchup | `108b020c` | Freeze-Dry (573) | Against Water (10): code 8 (super effective) |
| | `108b4cdc` | Flying Press (560) | Key `0x17` = 2: Flying as a second attacking type |
| `0x45` immunity | `108bbcf8` | Thousand Arrows (614) | A Ground (4) move's immunity becomes neutral (7) |
| `0x46` airborne | `108bbccc` | Thousand Arrows (614) | Key `0x42` = 1: the code is kept |
| `0x38` | `108ba544` | Chip Away (498), Sacred Sword (533) | Key `0x4c` = 1 |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list. The keys of events
`0x44`–`0x46` are described in the [matchup milestone](../battle_matchup/README.md).

## Details

- Every handler first requires key `0x03` (attacker) to equal its battler id.
  Key `0x38` is a scaled entry, so a later handler on the same frame replaces
  an earlier damage; the other keys are value entries that change once.
- **Psywave** narrows `random(0x65) + 50` to 16 bits, multiplies by the
  level's low byte (32-bit `mul`), divides by 100 through `umull` with
  `0x51eb851f` and a shift by 5, keeps 16 bits and raises 0 to 1.
- **Super Fang** adds the sign bit before shifting (rounding toward zero)
  and keeps bits 1–16 (`lsl 15`, `lsr 16`).
- **Endeavor** reads the defender's HP first; the difference is tested
  signed.
- **Bide** tests work word 0 signed and looks the attacker up without using
  it.
- **Counter, Mirror Coat and Metal Burst** call `1084b028(context, id, work,
  kind, factor)` with the factor on the stack (Ghidra drops it). The routine
  walks damage records `0, 1, …` (`1084bac4`, index wrapping at 256) of the
  attacker, skips those whose hitter (`+5`) is an ally of the attacker's own
  id (`+0x19`, `1084c5a8`) and, for a nonzero kind, those whose move (`+0`)
  has another category (`108007dc`), and sets key `0x38` to the record's
  damage (`+2`) through `1089e708` (4096-based, rounded). Zero damage counts.
  The work argument is unused.
- **Brick Break** stores its relocated hook (literal `108ba334`) in its
  handler's word `+0xc` with `1084aef8`, and clears it on event `0x50` with
  `1084aeec`. Ghidra merges `1084aef8` into the non-contiguous body of
  `108c21dc`, so it is listed by the decoder.

## Decoder extension

`decode_mult.py` adds `umull{s}{cond} rdlo, rdhi, rm, rs` and `bx{cond} rm`
to the battle_moves_2 decoder; SMULL, UMLAL and SMLAL stay refused. Sixteen
REA analyses that list MUL or UMULL are retained as references with their 30
multiply words read live, and the verifier requires the decoder to
reproduce all of them as well as every supported instruction, `bx lr`
included, of the REA-listed routines here.

## Validation

`make -C reconstructed/battle_moves_3 test` runs the handlers on the real
event stack with the key kinds their events give: no action for another
battler; the fixed amounts and levels; Psywave over every level byte (and
wider values) and random value; Super Fang over signed and wide HP; Endeavor
over HP grids and its signed test; Bide's states; Counter, Mirror Coat and
Metal Burst over record and category patterns, ally skipping, the call
order, zero and wide damage, the index wrap and other arguments; Brick
Break's handler word; every type for Freeze-Dry and Thousand Arrows; the key
flags; and Counter, Sonic Boom and Dragon Rage through the real dispatcher
(29,418 cases). ASan/UBSan pass; 36 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 20 handlers are bound to exactly 22 (move, event) pairs and that
every move entry of events `0x38`, `0x43`–`0x46`, `0x4e` and `0x50` is one of
them ([record](reproduction/record-comparison.json)).

## Limits

Brick Break's handler for event `0x4d` and its hook remain. The random,
damage-record, ally and move-category services remain external. No
running-game comparison has been made. Static evidence and host comparisons
do not establish a matching original build.
