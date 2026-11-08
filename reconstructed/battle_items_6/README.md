# Battle item handlers, batch 6: accuracy and duration items

This milestone reconstructs the sixth batch of the `DllBattle.cro` item
handlers, 11 handlers in all, bound through the item table `108d382c`. Each
item here has only this one entry:

- the four accuracy items of event `0x39`
- the four weather rocks of event `0x85`
- Light Clay (event `0xab`)
- Grip Claw and Binding Band (event `0x6b`)

Nine handlers were reviewed through live REA MCP/Ghidra listings in session
`outputs/rea-battle-7`. Two have no Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder):

- Heat Rock matches the REA listing of Icy Rock apart from the weather.
- Light Clay uses only forms REA lists in this batch, or at two spelling
  references.

| Event | Address (static) | Item | Effect |
| --- | --- | --- | --- |
| `0x39` | `108a7598` | BrightPowder 213 | The defender (key `0x04`) holds it: accuracy × (100 − p)% |
| | `108a7230` | Lax Incense 255 | The same, with (100 − p) narrowed to 16 bits |
| | `108a6f28` | Wide Lens 265 | The attacker (key `0x03`): the item factor |
| | `108a6504` | Zoom Lens 276 | The attacker, when the defender has moved this turn (`1084c818(defender, 1)`): the item factor |
| `0x85` | `108a5d60`, `108a6c74`, `108a6ce8`, `108a7344` | Heat 284, Damp 285, Icy 282, Smooth Rock 283 | The setter's weather (key `0x3a`: 1 sun, 2 rain, 3 hail, 4 sandstorm) lasts parameter 3 turns (key `0x25`) |
| `0xab` | `108a7e18` | Light Clay 269 | Key `0x56`'s low byte 0 or 1: the condition word's turns become parameter 3's low byte (`10876c4c`) |
| `0x6b` | `108a98f8` | Grip Claw 286 | Key `0x1e` is 8: `10876cf4(word, 8)` |
| | `108a7af4` | Binding Band 544 | `10876ca8(word, 1)` |

Runtime addresses are static − `0x1015b000`. Item numbers are the original
ids, named after the public Generation VI lists.

## Details

- **The accuracy drop**, for p = item parameter 3, works in single
  precision:
  1. r = 100 − p.
  2. When r is 0, take −0.5. Otherwise take `float(r << 12)` + 0.5. The
     constants are the literals at `108a7610`/`108a7614`, and at
     `108a72b0`/`108a72b4` for Lax Incense.
  3. Convert with `vcvt.s32.f32`, which rounds toward zero and saturates.
     The host version saturates explicitly.
  4. Divide by 100 as a signed value, through `smull` with `0x51eb851f`,
     an arithmetic shift of 5 and a sign correction.

  For p ≤ 100 this is `((100 − p) << 12) / 100` (`0xe66` for 10). For
  larger p, BrightPowder's r wraps and saturates, while Lax Incense keeps
  only 16 bits.
- **Multipliers** go to key `0x36` through `1084ce84`. The item factor is
  `1084cbe4` ((p + 100)% in 4096 units), linked from
  [battle_modifier](../battle_modifier/README.md).
- **Condition-word edits** work on a copy of key `0x1f` held in a stack word
  (`scratch` here). The handler passes its address to the helper, then
  writes the result back to key `0x1f` through `108658d4`.

## Validation

`make -C reconstructed/battle_items_6 test` runs 66,856 cases:

- **The accuracy drop:** every reduction 1–`0x10000` against the exact
  formula, plus zero and six wrap or saturation values worked out by hand.
- **BrightPowder and Lax Incense:** parameters 0–120 and seven wrapping
  values, over three holder keys.
- **Wide Lens and Zoom Lens:** six parameters, both attackers, both turn
  answers and three defender words.
- **The rocks:** each over seven weather words, both setters and five
  parameters.
- **Light Clay:** eight key-`0x56` words, three holders and four
  parameters, with the scratch word and key `0x1f` checked.
- **Grip Claw and Binding Band:** five key-`0x1e` values and both
  attackers.
- **Dispatcher:** three entries through the real dispatcher.

ASan/UBSan pass. 32 mutations are each detected. Two equivalent mutations
were replaced: −0.5 and 0.5 both truncate to 0, and the factor is never
negative, so signed and unsigned division agree. One that did not compile
was also replaced.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 11 handlers are bound to exactly 11 (item, event) pairs of the item
table and nothing else, and that these are every entry of the 11 items
([record](reproduction/record-comparison.json)).

## Limits

These remain external:

- the turn flag and the three condition-word helpers
- the item parameters and the event-key rewrite

No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
