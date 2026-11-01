# Battle insight handlers

This thirty-fourth host-source milestone reconstructs the last five
`DllBattle.cro` handlers of event `0x94` (a battler entering), which also
serve event `0x5e` (an ability taking effect mid-battle). Each one looks at
the opposing battlers. Together with the
[switch-in milestone](../battle_switchin/README.md), every handler of event
`0x94` is now reconstructed. Trace, Forewarn and Anticipation were reviewed
through live REA MCP/Ghidra listings. Download and Frisk have no Ghidra
function, so the
[move-power milestone's decoder](../battle_power/README.md#decoder) lists
them.

| Handler | Address (static) | Ability | Effect |
| --- | --- | --- | --- |
| Trace | `108bcb0c` | 36 | Copies a random traceable opponent's ability (message `0x17d`); with none, waits for an opponent to enter |
| Download | `108be1dc` | 88 | Attack +1 if the opponents' summed Defense is lower than their Sp. Def, else Sp. Atk +1 |
| Forewarn | `108bd460` | 108 | Names a random one of the opponents' strongest moves (message `0x1b1`) |
| Anticipation | `108bf390` | 107 | Warns (message `0x1b4`) of a one-hit-KO or super-effective damaging move |
| Frisk | `108be6bc` | 119 | Reports every held item of the opponents (message `0x1b7`, then `0xfe`) |

Runtime addresses are static − `0x1015b000`. Ability numbers are the
original ids, named after the public Generation VI lists.

## Details

- Key `0x02` is the entering battler. Every handler acts only on its own
  entry, except Trace's pending path.
- **Trace** lists the opposing side (`1084cb78` with bit 8, then
  `1084bc20`). It keeps the battlers whose ability `108a4854` does not
  forbid and picks one with `1084c7bc`; with exactly one candidate it skips
  the random call. With no candidates it sets its work word to 1. Later, a
  non-ally entering in a position other than `0xa` triggers the copy (in
  battle mode 2 only if `108673f8` places that position on the faced side).
  The record has kind `0x20`: `+6` id, `+4` ability, `+8` = 1, and flag
  `0x800000`. Ability 0, or an id of `0x1f`, skips it.
- **Download** sums values 9 and `0xb` (`10874420`) as 16-bit numbers over
  the opponents (`1084ac1c`). It commits a kind-`0xd` record with `+0xc` id,
  `+4` stat (1 or 3), and `+0x16` and `+0x17` set to 1. With no opponents it
  does nothing.
- **Forewarn's power rule**: a status move counts as 1. A listed power other
  than 1 is used as it is. Otherwise a one-hit KO (kind 9) counts as 150;
  Counter, Mirror Coat and Metal Burst as 120; any other move as 80. Ties
  are kept in a 20-byte temporary buffer (`1084c7d0`), in 4-byte entries of
  battler id and move.
- **Anticipation** takes the move type from `1080043c`. For Hidden Power
  (237) it uses the holder's type (`10801afc`); if the holder slot is empty,
  it aborts and uses type 0. A one-hit KO counts regardless of type. A
  damaging move counts when `10870900` rates it above 7 against this
  battler's packed types.
- **Frisk** stores the opponents' items (`+0x12`) at buffer `+0`, their ids
  at `+0xa` and the indices of those holding an item at `+0xf`. The first
  record's message `0x1b7` also names this battler. The rest use message
  `0xfe` (kind 1). The popup surrounds all of them.

## Validation

`make -C reconstructed/battle_insight test` covers 238 cases:

- **Every handler** stays inactive for another battler.
- **Download:** ties and 16-bit wraparound.
- **Forewarn:** each power class, ties, the reset when a stronger move
  appears, and the random pick.
- **Anticipation:** one-hit KOs, damaging moves, the effectiveness threshold
  and Hidden Power (holder present or absent).
- **Frisk:** 0–3 holders and its messages.
- **Trace on entry:** untraceable abilities, one candidate and several, the
  waiting mark, ability 0 and id `0x1f`.
- **Trace's pending path:** ally, work word, position `0xa` and the
  battle-mode-2 side check.
- **The real dispatcher:** Download then Frisk.

ASan and UBSan pass, and each of 23 mutations is detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the five handlers are bound to exactly 10 (ability, event) pairs on
events `0x94` and `0x5e`. It also confirms that all 31 entries of event
`0x94` belong to these handlers or to the 23 of battle_switchin
([record](reproduction/record-comparison.json)).

## Limits

The opponent, side, buffer, random, ability-rule, move-data, effectiveness,
popup, message and action services remain external. No running-game
comparison has been made. Static evidence and host comparisons do not
establish a matching original build.
