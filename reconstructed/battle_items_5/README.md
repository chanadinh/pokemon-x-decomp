# Battle item handlers, batch 5: stat and critical-hit items

This milestone reconstructs the fifth batch of the `DllBattle.cro` item
handlers: the held items that multiply speed, the attacking stat or the
defending stat, or that raise the critical-hit stage. It also covers those
items' other entries:

- the Choice items' move lock and release
- Iron Ball's grounding
- Light Ball's event-`0x7c` paralysis

That makes 23 handlers bound through the item table `108d382c` to 24 items.
All 23 were reviewed through live REA MCP/Ghidra listings in session
`outputs/rea-battle-7`.

| Event | Address (static) | Items | Effect |
| --- | --- | --- | --- |
| `0x13` speed (key `0x02`) | `108a802c`, `108ab0d4`, `108ab168` | Macho Brace 215, Iron Ball 278, Power items 289–294 | ×0.5 |
| | `108a7fd4` | Choice Scarf 287 | ×1.5 |
| | `108a73e0` | Quick Powder 274 | Ditto (132), not transformed: ×2 |
| `0x3b` critical hit (key `0x03`) | `108a63ac` | Scope Lens 232, Razor Claw 326 | Stage +1 |
| | `108a6b90`, `108a629c` | Lucky Punch 256, Stick 259 | Chansey (113) or Farfetch'd (83): +2 |
| `0x40` attacking stat (key `0x03`) | `108ab3c4`, `108aa5c4` | Choice Band 220, Choice Specs 297 | Physical or special move (`108007dc`): ×1.5 |
| | `108aa608` | Soul Dew 225 | Latias or Latios (380, 381), key `0x1b` special: ×1.5 |
| | `108a7b34` | DeepSeaTooth 226 | Clamperl (366), special: ×2 |
| | `108a64bc` | Light Ball 236 | Pikachu (25): ×2 |
| | `108a6564` | Thick Club 258 | Cubone or Marowak (104, 105), physical: ×2 |
| `0x41` defending stat (key `0x04`) | `108ab074` | Soul Dew 225 | Latias or Latios, special: ×1.5 |
| | `108a80a8` | DeepSeaScale 227 | Clamperl, special: ×2 |
| | `108a7188` | Metal Powder 257 | Ditto, not transformed, physical: ×2 |
| | `108a8104` | Eviolite 538 | Can still evolve (`1081caf0`): ×1.5; never Floette (670) in form 5 |
| | `108a8b0c` | Assault Vest 640 | Special: ×1.5 |
| `0x22` | `108ab898` | Choice Band, Scarf, Specs | Lock the attacker into its move, unless it is Struggle (165) |
| `0xa8` | `108abb94` | the same | Release the lock |
| `0x12` | `108ab408` | Iron Ball | Key `0x42` = 1 (grounded) |
| `0x7c` | `108a95c0` | Light Ball | Paralysis for battler key `0x03` |

Runtime addresses are static − `0x1015b000`. Item numbers are the original
ids, named after the public Generation VI lists. Species are national dex
numbers.

## Details

- **Multipliers** go to key `0x36` through `1084ce84` (`0x800` ×0.5,
  `0x1800` ×1.5, `0x2000` ×2). Species is the halfword at battler `+0xc`.
- **Category sources differ.** The attack items use `108007dc` (the move's
  category from its data, for key `0x12`'s low half), except Soul Dew. Soul
  Dew and the defence items compare key `0x1b` (the category carried in the
  event).
- **Quick Powder and Metal Powder** ask `10851c28` whether the battler is
  transformed, and only after the species test.
- **Eviolite** reads the form byte `+0x153` only for species 670
  (`subs`, then `ldrbeq`/`cmpeq`).
- **Critical-hit items** rewrite key `0x2d` (`108658d4`) with the current
  stage plus 1 or 2, kept to its low byte.
- **The Choice lock** commits a kind-`0xb` record with:
  - `+4`: status `0x1b`, as a halfword
  - `+8`: the word `1084c3b0(move)` returns
  - `+0xf`: the battler
  - `+0x10`: 2

  The release commits a kind-`0xa` cure with `+4` `0x1b`, `+5` the battler
  and `+0x11` and `+0x12` both 1.
- **Light Ball's event-`0x7c` record** is kind `0xb` for the low byte of
  key `0x03`. It holds `+4` status 1 (paralysis), `+8` the word
  `10860e00(1)` returns and `+0xf` the holder.

## Validation

`make -C reconstructed/battle_items_5 test` runs 29,191 cases:

- **The 15 multiplier handlers:** each is checked against a table that
  describes it: holder key, species, transformed test, category source and
  factor. The table is run over ten species values (with the neighbouring
  bytes set), both transformed answers, four data categories, four key-`0x1b`
  values, three holder keys and two move words.
- **Eviolite:** four species, eight forms and both evolution answers.
- **Critical-hit items:** six stages (including byte wrap), four species and
  both attackers.
- **The Choice lock:** five moves (including Struggle with a high half).
- **The release:** five holders.
- **Iron Ball and Light Ball's other entries:** five holders, and six
  key-`0x03` words for Light Ball. Every byte of each record is checked.
- **Dispatcher:** Choice Band's three entries through the real dispatcher.

ASan/UBSan pass. 40 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 23 handlers are bound to exactly 33 (item, event) pairs of the item
table and nothing else, and that every entry of the 24 items is one of them
([record](reproduction/record-comparison.json)).

## Limits

These remain external:

- the transformed and can-evolve queries
- the lock condition and paralysis condition words
- the move category, record stack and commit

The accuracy items of event `0x39` (BrightPowder, Lax Incense, Wide Lens and
Zoom Lens) are not reconstructed yet. No running-game comparison has been
made. Static evidence and host comparisons do not establish a matching
original build.
