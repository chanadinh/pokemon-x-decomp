# Battle item handlers, batch 4: gems

This milestone reconstructs the fourth batch of the `DllBattle.cro` item
handlers: the 18 gems, Fire through Steel Gem (items 548–564) and Fairy Gem
(715). Each gem has three handlers:

- a power handler on event `0x3d`
- a use stub on event `0x8a`
- one handler on event `0x91`, shared by all 18

That makes 37 handlers bound through the item table `108d382c`. Three
helpers they share are also covered:

- `1084c284`, the use body
- `1084c700`, which marks an item consumed
- `10881638`, which tests that mark

Twenty-six routines were reviewed through live REA MCP/Ghidra listings in
session `outputs/rea-battle-7`. Fourteen gem handlers have no Ghidra
function. They are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder), and
each listing must match an REA-listed sibling with the same shape.

| Event | Handlers | Effect |
| --- | --- | --- |
| `0x8a` (before the move) | 18 stubs `mov r12, #type; str; bl 1084c284` | The holder uses a damaging move of the gem's type: the gem is consumed, message `0xbc`, work word 0 = 1 |
| `0x3d` (power) | 18 handlers, literal `0x14cd` after each | The holder's move of that type: power ×1.3 (`0x14cd`) once used, or in a pass that does not consume items |
| `0x91` | `108a85cc` | Work word 0 from 1 to 2, so the boost applies once |

| Item | Gem | Type | `0x3d` | `0x8a` |
| --- | --- | --- | --- | --- |
| 548 | Fire | 9 | `108a8ec0` | `108aa510` |
| 549 | Water | 10 | `108a87bc` | `108aa120` |
| 550 | Electric | 12 | `108a8d78` | `108aa46c` |
| 551 | Grass | 11 | `108a86b0` | `108aa048` |
| 552 | Ice | 14 | `108a9168` | `108aa668` |
| 553 | Fighting | 1 | `108a9104` | `108aa564` |
| 554 | Poison | 3 | `108a84cc` | `108a9de8` |
| 555 | Ground | 4 | `108a903c` | `108aa550` |
| 556 | Flying | 2 | `108a8e5c` | `108aa480` |
| 557 | Psychic | 13 | `108a7d50` | `108a9684` |
| 558 | Bug | 6 | `108a8870` | `108aa134` |
| 559 | Rock | 5 | `108a7ea8` | `108a97fc` |
| 560 | Ghost | 7 | `108a7db4` | `108a9698` |
| 561 | Dragon | 15 | `108a9620` | `108aa948` |
| 562 | Dark | 16 | `108a7cd0` | `108a958c` |
| 563 | Steel | 8 | `108a96ac` | `108aa95c` |
| 564 | Normal | 0 | `108a88dc` | `108aa148` |
| 715 | Fairy | 17 | `108a8aa8` | `108aa1f4` |

Runtime addresses are static − `0x1015b000`. Item numbers are the original
ids, named after the public Generation VI lists.

## Details

- **`1084c284(handler, context, id, work, type)`** takes the type as a
  stack argument. It acts only if all of these hold, checked in this order:
  - key `0x03` (the attacker) is the holder
  - key `0x16` (the whole type word) is the gem's type
  - `10800434` reports the move (the low half of key `0x12`) as damaging
  - `1084c720` does not report it
  - the gem is not already consumed (`10881638`)
  - `1084c70c(context)` does not report a pass that does not consume items

  It then marks the gem consumed (`1084c700`: handler byte `+0x10` = 6) and
  commits a kind-`0x24` record with message `0xbc` (kind 1), naming the gem
  (handler `+0x38`) and the whole key `0x12`. Finally it sets work word 0
  to 1.
- **The power handlers** check the attacker and the whole type word. They
  then multiply key `0x32` by `0x14cd` when work word 0 is 1. Otherwise they
  ask `1084c70c`, and multiply when it reports a non-consuming pass, so a
  forecast sees the boost without spending the gem.
- **`108a85cc`** acts only for the attacker. It is a single conditional
  sequence (`ldreq`, `cmpeq`, `moveq`, `streq`).
- **`1084c700` and `10881638`** end in `bx lr`, which the decoder refuses.
  Both are REA-listed, so their listing is REA's.

## Validation

`make -C reconstructed/battle_items_4 test` runs 6,619 cases:

- **Mark helpers:** both over all 256 values of the byte, with its
  neighbours checked.
- **`1084c284`:** both attackers, four type words (including a high bit),
  every answer of the three queries, the consumed byte and two move words
  (including a high half). The work word and consumed byte are checked
  after each run.
- **Power handlers:** every gem over types 0–`0x12`, both attackers, four
  work words and both pass answers. The multiply's rounding is checked
  against a reference.
- **Use stubs:** every gem over types 0–`0x12`.
- **`108a85cc`:** six work words and five attackers.
- **Dispatcher:** one gem handler with three entries through the real
  dispatcher: use, boost, spent, then no boost and no second use.

ASan/UBSan pass. 35 mutations are each detected; one that did not compile
was replaced.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 37 handlers are bound to exactly 54 (item, event) pairs of the item
table and nothing else, that the three helpers are bound to nothing, and that
every entry of the 18 gems is one of those pairs
([record](reproduction/record-comparison.json)).

## Limits

The damaging-move and fixed-target queries, the non-consuming pass, the
record stack, messages and commit remain external. No running-game
comparison has been made. Static evidence and host comparisons do not
establish a matching original build.
