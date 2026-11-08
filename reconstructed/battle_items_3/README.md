# Battle item handlers, batch 3: power-boosting held items

This milestone reconstructs the third batch of the `DllBattle.cro` item
handlers. It covers every remaining item handler of event `0x3d`, which sets
a move's power multiplier (key `0x32`), apart from the gems. There are 28
such handlers: the type items with their plates, the incenses, the three
orbs, Muscle Band and Wise Glasses. Poison Barb's only other entry, an
event-`0x7c` handler, makes 29 in all, bound through the item table
`108d382c` to 44 items.

Twenty-four routines were reviewed through live REA MCP/Ghidra listings.
Five have no Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder). Their
one instruction form not seen in the other listings (`uxth r0, r0`) is
pinned to REA's own listing of the same word at `108bc9ec`.

The batch links the item factor `1084cbe4` from
[battle_modifier](../battle_modifier/README.md) and the multiplier update
`1084ce84` from [battle_ability](../battle_ability/README.md).

| Group | Address (static) | Items | Fits when |
| --- | --- | --- | --- |
| Normal | `108a9268` | Silk Scarf 251 | Key `0x16` (the whole type word) is 0 |
| Fighting | `108a5de0` | Black Belt 241, Fist Plate 303 | Type 1 |
| Flying | `108a92b0` | Sharp Beak 244, Sky Plate 306 | 2 |
| Poison | `108a6140` | Poison Barb 245, Toxic Plate 304 | 3 |
| Ground | `108a82b8` | Soft Sand 237, Earth Plate 305 | 4 |
| Rock | `108a6254`, `108a6e10` | Hard Stone 238, Stone Plate 309; Rock Incense 315 | 5 |
| Bug | `108a65c8` | Silver Powder 222, Insect Plate 308 | 6 |
| Ghost | `108a78c4` | Spell Tag 247, Spooky Plate 310 | 7 |
| Steel | `108a6650` | Metal Coat 233, Iron Plate 313 | 8 |
| Fire | `108a5e28` | Charcoal 249, Flame Plate 298 | 9 |
| Water | `108a8170`, `108a6d50`, `108a8940` | Mystic Water 243, Splash Plate 299; Sea Incense 254; Wave Incense 317 | 10 |
| Grass | `108a77e4`, `108a72bc` | Miracle Seed 239, Meadow Plate 301; Rose Incense 318 | 11 |
| Electric | `108a6080` | Magnet 242, Zap Plate 300 | 12 |
| Psychic | `108a8058`, `108a699c` | Twisted Spoon 248, Mind Plate 307; Odd Incense 314 | 13 |
| Ice | `108a7b98` | Never-Melt Ice 246, Icicle Plate 302 | 14 |
| Dragon | `108a6c2c` | Dragon Fang 250, Draco Plate 311 | 15 |
| Dark | `108a70e8` | Black Glasses 240, Dread Plate 312 | 16 |
| Fairy | `108a7398` | Pixie Plate 644 | 17 |
| Orbs | `108a6b14`, `108a63e4`, `108a69e4` | Adamant 135, Lustrous 136, Griseous 112 | The holder is Dialga (483), Palkia (484) or Giratina (487), and the type's low byte is Dragon or Steel, Water or Ghost |
| Category | `108aa384`, `108a8820` | Muscle Band 266, Wise Glasses 267 | `108007dc` gives category 1 (physical) or 2 (special) for key `0x12`'s low half |
| Poison Barb, event `0x7c` | `108a8ddc` | Poison Barb 245 | Key `0x02` is the holder |

Runtime addresses are static − `0x1015b000`. Item numbers are the original
ids, named after the public Generation VI lists.

## Details

- **Every event-`0x3d` handler** first requires key `0x03` (the attacker)
  to be the holder. When the move fits, it multiplies key `0x32` by the item
  factor `1084cbe4`: (parameter 3 + 100)% in 4096 units, so `0x1333` for a
  parameter of 20. The multiply rounds at `0x800` and respects the entry's
  limits.
- **Griseous Orb** multiplies by a fixed `0x1333` (literal `108a6a4c`) and
  does not read the item.
- **The orbs** read the species as the halfword at battler `+0xc`, and
  compare only the low byte of the type. The type items compare the whole
  word.
- **Muscle Band and Wise Glasses** pass `key 0x12 & 0xffff` to `108007dc`.
- **Poison Barb on event `0x7c`** requires key `0x02` to be the holder. It
  then commits a kind-`0xb` record for the low byte of key `0x03` with:
  - `+0xf`: the holder
  - `+4`: status 5 (poison), as a halfword
  - `+8`: the condition word from `10860e00(5)`

## Validation

`make -C reconstructed/battle_items_3 test` runs 11,244 cases:

- **Type items:** each of the 23 over types 0–`0x13`, three types with high
  bits set, both attackers, four parameters and two starting powers, against
  an independent factor and rounding reference.
- **Orbs:** each of the three over ten species values, 22 type words
  (including high bytes), both attackers and two parameters. Further cases
  check that the bytes next to the species are ignored and that another
  holder's species is read.
- **Category items:** both, over four category answers, six move words
  (including high halves) and both attackers.
- **Poison Barb's event-`0x7c` handler:** six holders and six key-`0x03`
  words, with every byte of the record checked.
- **Dispatcher:** three linked handlers (Charcoal, Lustrous Orb, Muscle
  Band) through the real dispatcher, for a Water move, a Fire move and
  another attacker.

ASan/UBSan pass. 35 mutations are each detected; four that did not compile
were replaced with wrong-value versions.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 29 handlers are bound to exactly 45 (item, event) pairs of the item
table and nothing else. Those are 44 on event `0x3d` and Poison Barb's one
on `0x7c`. It also confirms that every entry of the 44 items is one of them
([record](reproduction/record-comparison.json)).

## Limits

The move category, the condition word, the item parameters, the record stack
and the commit remain external. The gems (548–564 and 715, events `0x3d` and
`0x8a`) and the other remaining items are not reconstructed yet. No
running-game comparison has been made. Static evidence and host comparisons
do not establish a matching original build.
