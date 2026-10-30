# Battle attack and defense values

This twenty-first host-source milestone reconstructs the two `DllBattle.cro`
helpers that supply the attack (A) and defense (D) terms of the
[damage calculation](../battle_damage/README.md). Both were reviewed through
live REA MCP/Ghidra and ARM instructions.

| Address (static) | Host routine | Behavior |
| --- | --- | --- |
| `108202d8` | `px_battle_attack_value` | Attack or special attack, battler substitution, critical-hit stages, final multiplier, burn-exception flag |
| `10822330` | `px_battle_defense_value` | Defense or special defense, stat swap, critical-hit stages, sandstorm, final multiplier |

Runtime addresses are static − `0x1015b000` (see the damage milestone). Names
are analyst interpretations. Battle Pokémon keep their id byte at `+0x19` and
signed stat stages at `+0x104..+0x107` (attack, defense, special attack,
special defense; 6 is neutral). Stat keys are 8–11 in that order; category 2
(special) selects keys 10 and 11.

## Attack

1. The move category selects key 10 (special) or 8.
2. Event `0x3e` receives both ids with key `0x3c = 0x1f` and key `0x53 = 0`.
   A low byte of key `0x3c` other than `0x1f` names a battler: the context's
   table (`context + 8`) word at `+0x84 + 4 × index` replaces the attacker. An
   index of `0x18` or more, or an empty slot, calls the abort helper
   `10800394(0, 0, 0)`; that call returns and execution continues.
3. A nonzero key `0x53` (full word) selects the stat without stages. Otherwise
   a critical hit asks `108620d4` which stage applies; offensive stats ignore
   stages below 6 and defensive stats ignore stages above 6 by using the raw
   stat. In every other case the staged stat is used. The value is narrowed to
   16 bits.
4. Event `0x40` receives move id, type and byte `+7`, the value in key `0x34`,
   a multiplier (key `0x36`, base `0x1000`, range `0x19a`–`0x20000`) and key
   `0x54 = 0`. The 16-bit value is multiplied with 4096-based rounding and
   narrowed. Whether key `0x54` is nonzero is stored as a byte through the
   flag pointer before the event context closes; the damage calculation uses
   it to skip the burn halving.

## Defense

1. The category selects key 11 (special) or 9, and byte `+7` of the move is
   kept as the category value for later events.
2. Event `0x3f` receives both ids, the key (`0x3d`), key `0x3e = 0` and key
   `0x53 = 0`. An odd key `0x3e` swaps defense and special defense and sets the
   category value to 2 or 1 accordingly. Only the **low byte** of key `0x53`
   is tested.
3. Stage selection follows the attack rule for the defender.
4. Unless event `0x83` sets key `0x42`, weather 4 (sandstorm) with a defender
   of type 5 (`1086f960(defender, 5)`) raises special defense by `3 << 11`
   (= `0x1800`) with the usual rounding test.
5. Event `0x41` applies the final multiplier as for attack.

The damage milestone's same-type callback was corrected while reviewing these
routines: `1086f960` takes a Pokémon and a type, and the damage routine passes
the move's type. Both previously traced attacks show that argument (12 and 0)
and still replay identically with it checked.

## Validation

`make -C reconstructed/battle_stats test` checks both exact call sequences;
every category, critical flag, ignore value and stage from 0 to 12 for both
sides against an independent rule; negative stages; battler substitution,
low-byte indices and both returning aborts; final multipliers with 16-bit
narrowing and the boolean flag; the stat swap and its category value; and
sandstorm by weather, type, stat and suppression (364 cases). ASan/UBSan pass;
eleven mutations of observed bounds, directions, widths and constants are each
detected.

The live trace and replay are prepared but not yet run against the game:
[`scripts/trace_live_function.py`](../../scripts/trace_live_function.py) with
[`scripts/battle_stats_sites.json`](../../scripts/battle_stats_sites.json)
records every internal call, and
[`scripts/replay_live_stats.c`](../../scripts/replay_live_stats.c) checks callee,
order, arguments, value and flag. See [reproduction](reproduction/README.txt).

## Limits

Event handlers (abilities, items, field effects), stat and stage services,
type checks and the abort helper remain external. No running-game comparison
of these two routines has been made yet. Static evidence and host comparisons
do not establish a matching original build.
