# Battle position-effect handlers

This milestone reconstructs every handler of the `DllBattle.cro`
position-effect table (`108d6768`, 11 rows of 8 bytes: an id and a table
function). That is 17 handlers for the conditions tied to the battlefield
rather than to a side, such as the terrains, Water Sport, Mud Sport and Ion
Deluge.

Six handlers (the terrains' power, heal, block and show handlers) were
reviewed through live REA MCP/Ghidra listings in session
`outputs/rea-battle-10`. The other eleven have no Ghidra function and are
listed by the [move-power milestone's decoder](../battle_power/README.md#decoder)
plus a narrow `bx<cond>` rule (see Evidence).

The names below are analyst interpretations of what each row does; the ids
are the table's. The ids run 0–8 and 10, with 9 in the ninth row and 8 in
the tenth.

| Row (id) | Address (static) | Name | Event | Effect |
| --- | --- | --- | --- | --- |
| 0 (0) | `108bc154` | no-op | `0x4f` | Returns at once |
| 1 (1) | `108bc28c` | flag | `0x13` | Key `0x4b` = 1 |
| 2 (2) | `108bc680` | accuracy | `0x39` | Key `0x36` × `0x1ab8` |
| 2 (2) | `108bc658` | Ground | `0x44` | Move type 4 sets key `0x4c` = 1 |
| 3, 7, 8 (3, 7, 9) | `108bc07c` | no-op | `0` | One shared handler that does nothing |
| 4 (4) | `108bc260` | Water Sport | `0x3d` | Type 9 (Fire): key `0x32` × `0x548` (about one third) |
| 5 (5) | `108bc234` | Mud Sport | `0x3d` | Type `0xc` (Electric): key `0x32` × `0x548` |
| 6 (6) | `108bc3a4` | counter | `0x3f` | Key `0x3e` + 1 |
| 9 (8) | `108bc298` | Ion Deluge | `0x29` | Type 0 (Normal) becomes `0xc` (Electric) |
| 10 (10) | `108bc158` | Grassy power | `0x3d` | Grass moves × 1.5 from a grounded attacker; the moves `0x59`, `0xde` and `0x20b` × 0.5 against a grounded target |
| 10 (10) | `108bc500` | Grassy heal | `0x7f` | A grounded battler restores 1/16 of max HP (kind 4, message `0x183`) |
| 10 (10) | `108bc0f4` | Misty power | `0x3d` | Dragon (type `0xf`) moves × 0.5 against a grounded target |
| 10 (10) | `108bc3c0` | Misty block | `0x6e` | A major status given to a grounded battler is blocked (key `0x42`, kind `0x17` reason `0x11`) |
| 10 (10) | `108bc5b0` | Misty show | `0x70` | Shows the block (message `0x4c9`) |
| 10 (10) | `108bc094` | Electric power | `0x3d` | Electric moves × 1.5 from a grounded attacker |
| 10 (10) | `108bc2c0` | Electric block | `0x6e` | Sleep (2) or the status `0xe` given to a grounded battler is blocked (reason `0x10`) |
| 10 (10) | `108bc458` | Electric show | `0x70` | Shows the block (message `0x4b7`) |

Row 10's table function (`108bbf98`) takes a parameter: 1 gives the Grassy
handlers (table `108d67d0`), 2 the Misty handlers (`108d67e0`), 3 the
Electric handlers (`108d67f8`) and any other value a single no-op on event 0
(`108d380c`). A grounded battler is one for which neither the airborne
query (`1084bf24`) nor the exemption query (`1084af30`) answers non-zero.

Runtime addresses are static − `0x1015b000`.

## Details

- **Keys:** `0x03` the attacker, `0x04` the battler hit, `0x02` the battler,
  `0x12` the move, `0x16` the move type, `0x1e` the status being given,
  `0x32` and `0x36` multipliers and `0x42` the block flag.
- **Blocking** works through `108658d4`'s rewrite: a value already rewritten
  once does not take, and the handler then does nothing more. Electric block
  tries sleep first; if that rewrite does not take, it falls through to the
  second status test.
- **A quirk of the original:** Misty show and Electric show commit the first
  record (kind `0x18`) twice and never commit the message record (kind 3).
  The reconstruction keeps this.
- **Literal words:** `0x183`, `0x4b7`, `0x4c9`, `0x1ab8` and `0x548` (twice)
  are plain literals, not relocations.

## Evidence

All 1,492 instruction bytes and 24 literal bytes match live REA bytes and the
original `DllBattle.cro`; no relocation is involved.

The listings of the 11 decoded handlers come from `decode_arm.py` with a
`bx<cond>` rule for the forms it refuses. Every instruction shape they use
(registers, immediates, literal addresses and branch targets abstracted) is
REA-listed in this batch or at one of four spelling references, each
checked against REA's listing and the live word: `bx lr` at `1084bfcc`, and
the push and pops at `108b9d90`, `108b9da8` and `108b9db8`.

## Validation

`make -C reconstructed/battle_fields_2 test` runs 4,555 cases covering the
regular rows' types and keys, the terrains' power and blocks for grounded,
airborne and exempt battlers and for every status the blocks test, Grassy
heal over slots (including the excluded `0x1f`) and max HPs with the record
and message bytes, and the shows with and without the state bit (both
commits are compared with the first record).

ASan/UBSan pass. 60 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) decodes
the 11 rows of the original table. Ten rows are read with the
battle_handler milestone's table-function reader. Row 10's irregular table
function is checked word by word and its four entry tables are read
directly. It confirms that the whole table holds exactly these 17 handlers,
bound to 20 (table, event) pairs, and that every handler has evidence in
this milestone ([record](reproduction/record-comparison.json)).

## Limits

These remain external:

- the airborne and exemption queries, the pokemon lookup, the state test,
  the record stack, messages and commit
- the table functions (the ten regular ones are four-instruction stubs, row
  10's is irregular) and the registrar that installs a position effect
  (`108bc690`)

No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
