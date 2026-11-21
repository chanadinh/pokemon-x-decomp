# Battle item handlers, batch 13: the last item-table handlers

This milestone reconstructs the thirteenth and last batch of the
`DllBattle.cro` item handlers. It covers the remaining 8 handlers, bound
through the item table `108d382c`, for:

- Amulet Coin and Luck Incense (223 and 319)
- Smoke Ball (228)
- Destiny Knot (280)
- Float Stone (539)
- Ring Target (543)
- Safety Goggles (650)

With it, every handler bound in the item table (247 distinct routines)
appears in some milestone's `evidence/functions.json`, and
[`reproduction/check_bindings.py`](reproduction/check_bindings.py) checks
this.

Six handlers were reviewed through live REA MCP/Ghidra listings in session
`outputs/rea-battle-8`. Two have no Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder). Every
instruction shape they use is REA-listed in this batch or at the spelling
reference `108b1080` (`ble`).

| Event | Address (static) | Item | Effect |
| --- | --- | --- | --- |
| `0x5e` | `108a79d4` | Amulet Coin, Luck Incense | Key `0x02` is the holder: `1084c3cc(context, id, 0)`, the prize-money routine Happy Hour also calls |
| `0x61` | `1084c3bc` | the same | The same call with no battler test: the routine moves its arguments into place and falls through into `1084c3cc` |
| `0x0b` | `108a6adc` | Smoke Ball | Key `0x02`'s low byte and the holder are allies (`1084c5a8`): key `0x53` = 1 (escape) |
| `0x0d` | `108a85fc` | Smoke Ball | The same; when the rewrite took, kind 3 shows message `0x3f2` naming the holder and the item, with the extra value `0xd0024` (`10876c28`) |
| `0x72` | `108a5c9c` | Destiny Knot | The condition given to the holder hit (key `0x1e`) is 7, infatuation: the attacker (unless key `0x03` is `0x1f`) gets it too. Kind `0xb`: `+4` 7, `+8` `10876e20(holder)`, `+0xc` 1, `+0xd` 1, `+0xf` the attacker, message `0x14a` |
| `0x84` | `108a5db4` | Float Stone | The holder's multiplier key `0x36` × 0.5 |
| `0x44` | `108a71f4` | Ring Target | Unless the pass does not consume items (`1084c70c`, tested first), the holder hit: key `0x4c` = 1 |
| `0x88` | `108ab6f8` | Safety Goggles | The holder, with key `0x33` positive (signed): key `0x42` = 1 |

Runtime addresses are static − `0x1015b000`. Item numbers are the original
ids, named after the public Generation VI lists. Safety Goggles' other entry,
the event-`0x31` handler `108aba84`, is in
[battle_immunity](../battle_immunity/README.md).

## Details

- **Event-key rewrites** (`108658d4`) do not take for a value that has
  already been rewritten once. Smoke Ball's event-`0x0d` message depends on
  that answer; the other handlers ignore it.
- **The literals** are `108a86a0` (`0x3f2`), `108a86a4` (`0xd0024`) and
  `108a5d5c` (`0x14a`).

## Validation

`make -C reconstructed/battle_items_13 test` runs 165 cases:

- **Amulet Coin:** both handlers over five holders.
- **Smoke Ball:** four key-`0x02` words (one with a high byte), both ally
  answers and a rewrite that does not take.
- **Destiny Knot:** three holders, four conditions and five attacker words
  (including `0x1f` with and without a high byte), with every byte of the
  record checked.
- **Float Stone:** four starting multipliers, against the rounding
  reference.
- **Ring Target and Safety Goggles:** both answers of their tests, six
  key-`0x33` words (including negative ones) and a rewrite that does not
  take.
- **Dispatcher:** Destiny Knot and Amulet Coin's event-`0x61` handler
  through the real dispatcher.

ASan/UBSan pass. 27 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms:

- the 8 handlers are bound to exactly 10 (item, event) pairs of the item
  table and nothing else
- every other entry of the seven items is Safety Goggles' event-`0x31`
  handler, which battle_immunity covers
- all 247 distinct handlers of the item table appear in a milestone's
  `evidence/functions.json`; the record lists how many each milestone
  holds ([record](reproduction/record-comparison.json))

## Limits

These remain external:

- the prize-money, ally, extra-message, infatuation-condition and
  non-consuming-pass routines
- the record stack, messages and commit

No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
