# Battle type effectiveness

This twenty-second host-source milestone reconstructs the three `DllBattle.cro`
routines that turn types into an effectiveness code and apply it to damage.
All three were reviewed through live REA MCP/Ghidra and ARM instructions.

| Address (static) | Host routine | Behavior |
| --- | --- | --- |
| `10860c80` | `px_battle_type_effectiveness` | One attacking type against one defending type, with Inverse Battle |
| `10860b80` | `px_battle_combine_effectiveness` | Product of two codes (dual types), clamped |
| `1089e7f0` | `px_battle_apply_effectiveness` | Damage scaled by a code |

Runtime addresses are static − `0x1015b000` (see the
[damage milestone](../battle_damage/README.md)). Names are analyst
interpretations.

## Effectiveness codes

A code `c` from 0 to 13 stands for the factor 2^(c−7): 0 is immune, 6 is
×½, 7 is neutral, 8 is ×2. The power table at `108d2248` holds
`0, 1, 2, 4, … 4096` for codes 0–13.

## Single matchup (`10860c80`)

Type `0x12` (typeless) on either side gives 7 without any read. Otherwise the
byte `chart[attack × 18 + defend]` is read from `108d12a8` (indices are not
bounds-checked), and then the signed byte at `+1` of the static object
`108fca00`, which is nonzero in an Inverse Battle:

| Chart byte | Normal | Inverse |
| --- | --- | --- |
| 0 (no effect) | 0 | 8 |
| 2 (not very effective) | 6 | 8 |
| 4 (neutral) | 7 | 7 |
| 8 (super effective) | 8 | 6 |
| other | abort helper, then 0 | same |

The abort helper `10800394(0, 0, 0)` returns, as in the other battle
milestones.

## Combination (`10860b80`)

Each code of 14 or more calls the abort helper, but the table is still read at
that index. The two table words are multiplied with 32-bit wraparound and
shifted right by 6. A product above `0x1000` aborts and becomes `0x1000`
(×64); a zero product with two nonzero codes aborts and becomes 1 (×1/64).
The result is the index of the lowest set bit plus one (0 for no bits). A
final check of the code against 14 is unreachable with the original table,
because the product never exceeds `0x1000` at that point.

## Application (`1089e7f0`)

A 14-entry jump table (`1089e804`, relocated words) selects: code 0 returns 0,
codes 1–6 shift right by 7 − code, code 7 returns the damage, codes 8–13
shift left by code − 7 (32-bit). Codes of 14 or more call the abort helper and
return the damage unchanged.

## Validation

`make -C reconstructed/battle_types test` checks every pair of a synthetic
chart in normal and inverse mode (including negative flag bytes) and the exact
read order; typeless on either side; every invalid chart byte; unchecked
indices; every pair of codes against the dual-type factor rule, including the
aborts; out-of-range codes, wraparound, both clamps and the lowest-bit rule;
and all 64 codes for fourteen damage values (2,974 cases). ASan/UBSan pass.
Seventeen mutations of observed bounds, directions, offsets and constants are
each detected.

The original-data check ([reproduction](reproduction/README.txt)) reads the
chart and power table from `DllBattle.cro` at their static addresses and
compares all 324 type pairs in normal and inverse mode with the native decoder
`px_type_chart_decode`, plus all 196 code combinations: 8 immunities, 61
resisted and 51 super-effective matchups, all matching
([record](reproduction/record-comparison.json)).

## Limits

The per-type and full-matchup wrappers (`1086094c`, `10826264`, `10863d44`),
which add events, abilities, Ground immunity while airborne and messages,
remain external. No running-game comparison of these routines has been made.
Static evidence and host comparisons do not establish a matching original
build.
