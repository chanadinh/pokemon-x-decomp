# Battle damage calculation

This twentieth host-source milestone is the first reconstruction from a CRO
module. It reconstructs the battle engine's damage calculation in
`DllBattle.cro`, reviewed through live REA MCP/Ghidra and ARM instructions and
checked call by call against damage calculated in the running game.

| Address | Host routine | Behavior |
| --- | --- | --- |
| `1086a1e0` (static) | `px_battle_calculate_damage` | Base power, attack/defense, level, core formula and modifiers; 16-bit damage |
| (helper) | `px_battle_apply_modifier` | 4096-based product, rounded up when the remainder exceeds `0x800` |

CRO addresses are synthetic static-analysis addresses from
`scripts/prepare_cro_analysis.py` (DllBattle at `0x10800000`). In the running
game the module was loaded at static address − `0x1015b000`; all 318
instruction words there equalled the analysed copy. Names are analyst
interpretations.

## Finding the routine

Source-file strings in the module (`btl_calc.cpp`, `btl_event.cpp`,
`btl_pokeparam.cpp`) and a static scan of its 895 import veneers showed that
battle code reaches main-executable services through `ldr pc, [pc, #-4]`
trampolines, and that damage is reached through event handlers rather than
direct calls. A live trace of the main move service from a battle showed the
engine does not read moves through it. The routine was then located by the
damage formula's constants: `0x55`, `0x64` and `0x10` (the 85–100 roll), `0x32`
and `0x2` (÷50, +2), and `0x1000`, `0x1800` and `0x800` (4096-based 1.0×, 1.5×
and 0.5×).

## Observed calculation

Arguments are context, attacker, defender and move record, then six stack
words: effectiveness, spread modifier, critical flag, fixed-roll flag, an extra
event value and the 16-bit output pointer. A battle Pokémon's id byte is at
`+0x19`; a move record has its id at `+0`, type at `+6` and a further byte at `+7`.

1. The move category (main `0039ddc0`) is fetched first. Event `0x4e` receives
   effectiveness, ids, critical flag, type, move, category and the extra value;
   a nonzero result in key `0x38` is the damage itself and ends the calculation
   (return value 1).
2. Base power (main `0039e044`) goes through events `0x3c`/`0x3d` with a power
   multiplier (key `0x32`, base `0x1000`, range `0x29`–`0x200000`). The power is
   narrowed to 16 bits, multiplied and narrowed again.
3. Attack and defense come from battle helpers, which also write a flag used
   by burn; level comes from `10874420(attacker, 0xf)`.
4. `((level & 0xff) × 2 / 5 + 2) × power × attack / defense / 50 + 2`, with all
   products in 32 bits and the quotient from the unsigned divide helper
   `0015e958`.
5. A spread modifier other than `0x1000` is applied.
6. Weather comes from the field state (`context + 0x1c`, byte `+1`) unless
   event `0x83` sets key `0x42`: sun (1) gives fire (type 9) 1.5× and water
   (type 10) 0.5×, rain (2) the reverse.
7. A critical hit multiplies by 15 and divides by 10.
8. Unless rule flag 7 is set, the roll multiplies by `100 − random(16)` (or 85
   with the fixed-roll flag) and divides by 100. The generator is the word at
   `+0x54` of a battle data object (relocated literal `1086a6dc`).
9. Except for type `0x12`, events `0x48`/`0x49` decide same-type bonus (1.5×
   by default when key `0x53` is set) and apply the resulting multiplier.
10. Type effectiveness is applied by `1089e7f0`.
11. Physical moves (category 1) from a burned attacker (status 4) are halved
    unless the attack helper's flag is set.
12. Event `0x4f` applies a final multiplier; damage is at least one.
13. Event `0x50` is dispatched, the outer event context closes, and the low
    16 bits are stored.

Every 4096-based step rounds up only above `0x800`, and the multiplication
wraps in 32 bits. The host keeps the original nesting of event contexts.

## Validation

`make -C reconstructed/battle_damage test` checks the exact call sequence of an
ordinary hit, the fixed-damage path and its 16-bit output, every weather/type
combination with suppression, critical hits, spread, rolls, rule flag 7,
same-type bonus, typeless moves, burn by category, status and flag, power
narrowing, 32-bit wrap and the minimum of one (547 cases against an
independent model). ASan/UBSan pass; twelve mutations of observed constants,
order and conditions are each detected.

[`scripts/trace_live_damage.py`](../../scripts/trace_live_damage.py) traces the
routine in Azahar: at each of its 61 call sites it records arguments before
the call and results after. [`scripts/replay_live_damage.c`](../../scripts/replay_live_damage.c)
replays the reconstruction with those results and requires the same callees,
order, arguments and damage. Two damage calculations from a wild battle,
Thunder Shock (move 84, attack 10, defense 18, level 5) and Pound (move 1,
attack 11, defense 8, level 11), matched with 59 and 60 calls and damage 6 and
6. See [reproduction](reproduction/README.txt).

## Limits

The event system and its handlers, the attack, defense, level, type,
effectiveness and status helpers, the RNG and rule services remain external,
so ability, item and field effects that act through events are not
reconstructed here. The live replay checks this routine's own arithmetic,
control flow and calls, not those callees. Two live samples do not exercise
weather, critical hits, burn, same-type bonus or multi-target spread in the
running game. Static evidence and host/live-call comparisons do not establish
a matching original build.
