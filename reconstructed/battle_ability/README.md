# Battle ability handlers: attack and defense multipliers

This twenty-sixth host-source milestone reconstructs the first event handlers
themselves: the thirteen `DllBattle.cro` ability handlers that change the
attack (event `0x40`) or defense (event `0x41`) value of the
[attack/defense routines](../battle_stats/README.md), with the event-stack
and battle helpers they use. They were reviewed through live REA MCP/Ghidra
and ARM instructions, and the original ability table confirms each binding.

| Address (static) | Host routine | Abilities | Effect |
| --- | --- | --- | --- |
| `108bf010` | `px_battle_thick_fat` | Thick Fat (47) | Defender: Fire or Ice moves ×½ |
| `108c0258` | `px_battle_huge_power` | Huge Power (37), Pure Power (74) | Attacker: physical ×2 |
| `108bca9c` | `px_battle_blaze` | Blaze (66) | At or below ⅓ HP: Fire ×1.5 |
| `108bd838` | `px_battle_torrent` | Torrent (67) | At or below ⅓ HP: Water ×1.5 |
| `108bea50` | `px_battle_overgrow` | Overgrow (65) | At or below ⅓ HP: Grass ×1.5 |
| `108c1030` | `px_battle_swarm` | Swarm (68) | At or below ⅓ HP: Bug ×1.5 |
| `108bd110` | `px_battle_guts` | Guts (62) | With a status: physical ×1.5, no burn halving |
| `108c1648` | `px_battle_marvel_scale` | Marvel Scale (63) | Defender with a status: physical ×1.5 |
| `108c3a64` | `px_battle_hustle` | Hustle (55) | Attacker: physical attack value ×1.5 |
| `1084cd44` | `px_battle_plus_minus` | Plus (57), Minus (58) | An ally with Plus or Minus: special ×1.5 |
| `108bd618` | `px_battle_defeatist` | Defeatist (129) | At or below ½ HP: ×½ |
| `108bd7fc` | `px_battle_fur_coat` | Fur Coat (169) | Defender: physical ×2 |
| `108c0f10` | `px_battle_grass_pelt` | Grass Pelt (179) | Defender: physical ×1.5 when `1084b8d4` returns 1 |

| Address | Host routine | Behavior |
| --- | --- | --- |
| `1084ce84` | `px_battle_event_multiply` | Multiply a multiplier entry, rounded and clamped |
| `108658d4` | `px_battle_event_rewrite` | Replace an id/value entry's value |
| `108544a0` | `px_battle_pokemon_by_id` | Battler table lookup with the returning aborts |
| `1084c124` | `px_battle_hp_fraction` | Maximum HP divided by a number |
| `1089e708` | `px_battle_modify` | 4096-based product with rounding above `0x800` |

Runtime addresses are static − `0x1015b000`. Ability numbers are the ids in
the original lookup table; names follow the public Generation VI list and are
analyst interpretations. Handlers are called by the
[event walk](../battle_event/README.md) with the handler, the context, the
battler id (the registration value byte) and the work area.

## Rules

Each handler first checks that it belongs to the right side of the move: key
`0x03` (attacker) or `0x04` (defender) must equal its battler id.

- **Multipliers** use `px_battle_event_multiply(0x36, f)`: the stored value
  is multiplied as a signed 64-bit product plus `0x800`, shifted right by 12,
  and clamped to the entry's signed minimum and maximum when either is
  nonzero (so a lone bound also applies the other, zero, bound). A missing
  key or one that is not a multiplier entry aborts and changes nothing.
- **Rewrites** replace a value (kind 1) once, marking it kind 2, or a scaled
  value (kind 0) any number of times; ids and multipliers abort, an entry
  already rewritten is refused silently. Guts sets key `0x54` to 1, which the
  [damage calculation](../battle_damage/README.md) reads to skip the burn
  halving. Hustle rewrites the attack value (key `0x34`) itself.
- The **move category** is `108007dc(key 0x12 narrowed to 16 bits)` on the
  attacking side and key `0x1b` (the move's category byte) on the defending
  side; 1 is physical, 2 special.
- **Thick Fat** tests the low byte of the type; the **pinch abilities**
  compare the full word with 9, 10, 11 or 6 after reading maximum HP ÷ 3 and
  then the current HP (equal to the third still counts).
- **Plus and Minus** ask `10873158` for the battler's side (`0xa` means none),
  have `1084bc20(context, side | 0x700, work)` write that side's ids into the
  handler's work area, and look for another battler whose ability (value
  `0x11`) is Plus, then for one with Minus.

## Marvel Scale

The table function for Marvel Scale points at `108c1648`, where Ghidra
created no function. Its 88 bytes are checked against the module and its
listing is an analyst decode whose branch targets are recomputed from the
instruction words by the verifier.

## Validation

`make -C reconstructed/battle_ability test` runs the handlers on the real
event stack of the event-system milestone: multiplication over signs,
factors, rounding and every clamp combination, entry kinds and missing keys;
rewrites by kind, once and twice; battler lookup and HP fractions; each
handler over its side, types (including wide values), categories, statuses,
HP around its threshold, field result and ally list; and three handlers
registered for event `0x40` running through the real dispatcher in list
order (23,897 cases). ASan/UBSan pass; 30 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) reads the
original lookup tables with the
[handler-registration](../battle_handler/README.md) table reader and confirms
that the thirteen handlers are bound to exactly these fifteen abilities and
events (twelve on `0x40`, three on `0x41`) and to no item, move or field row
([record](reproduction/record-comparison.json)).

## Limits

The value, category, status, side, side-list and field services
(`10874420`, `108007dc`, `1086d8ac`, `10873158`, `1084bc20`, `1084b8d4`) and
the abort helper remain external; the field service is not identified
beyond its result. Hustle's accuracy handler (event `0x39`) and the other
entries of these abilities are not part of this milestone. No running-game
comparison has been made. Static evidence and host comparisons do not
establish a matching original build.
