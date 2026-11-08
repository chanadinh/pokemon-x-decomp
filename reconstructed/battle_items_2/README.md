# Battle item handlers, batch 2: pinch berries

This milestone reconstructs the second batch of the `DllBattle.cro` item
handlers: the pinch berries, which work when the holder's HP is low. It
covers the Figy family (Figy, Wiki, Mago, Aguav and Iapapa, 159–163) and the
stat berries (Liechi, Ganlon, Salac, Petaya, Apicot, Lansat and Starf,
201–207). That is 23 handlers bound through the item table `108d382c`, plus
the Figy family's shared use body `1084c5e0`.

Sixteen routines were reviewed through live REA MCP/Ghidra listings. Eight
handlers have no Ghidra function, and REA's `inspect_native_instruction`
reports their words as undecodable. These are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder), plus a
`bx<cond>` rule that reproduces REA's own listing of a reference word.

The checks call the [first batch's](../battle_items_1/README.md) HP-berry
check `1084c198`, pinch test `1084c730` and re-entry mark `1084c96c`, and the
batch links that code. It also links two routines from other milestones:
`1084d2f4` (max HP / divisor, from
[battle_abilities_1](../battle_abilities_1/README.md)) and `1084ca4c` (a
battler state bit, from [battle_modifier](../battle_modifier/README.md)).

| Group | Address (static) | Items | Effect |
| --- | --- | --- | --- |
| Figy checks | `108abd68` (`0x9c`), `108abd10` (`0x5e`), `108abdc0` (`0x61`) | 159–163 | Give up once work word 0 is set or with condition `0xf`; then `1084c198` (divisor 2, flag 1 on `0x9c`, 0 on `0x5e`), or the pinch test and a request on `0x61` |
| Figy uses | `108a7584`, `108a6d3c`, `108a782c`, `108a7d34`, `108a6a50` (`0x7b`) → `1084c5e0` | Figy, Wiki, Mago, Aguav, Iapapa | Flavors 0–4: spicy, dry, sweet, bitter, sour |
| Figy uses on `0x7c` | `108a8e3c`, `108a8298`, `108a91cc`, `108a95a0`, `108a7e88` | the same | `bxeq lr` when work word 6 is zero, then the `0x7b` stub's sequence |
| Figy body | `1084c5e0` | | A kind-4 restore of max HP / parameter 3 with message `0x392`; a kind-`0xb` confusion when the flavor is disliked; work word 0 set |
| Stat checks | `108a9948` (`0x9c`), `108aa67c` (`0x5e`), `108ab100` (`0x61`) | 201–207 | As the HP checks, with parameter 3 as the divisor |
| Stat uses | `108a6924`, `108a67b4`, `108a6a64`, `108a6d98`, `108a60c8` | Liechi, Ganlon, Salac, Petaya, Apicot | Kind `0xd`: Attack, Defense, Speed, Sp. Atk or Sp. Def +1 |
| Lansat | `108a5fc0` | 206 | Kind `0x17` state 9 (getting pumped), then message `0x3ef` |
| Starf | `108a680c` | 207 | A random stat (table `108d2c3a`: 1–5) that can still rise, +2 |

Runtime addresses are static − `0x1015b000`. Item numbers are the original
ids, named after the public Generation VI lists.

## Details

- **The Figy checks** read the handler's work word 0, which `1084c5e0` sets
  after a use. They then require the battler not to have condition `0xf`
  (`10856a88`) before running the first batch's check with a divisor of 2.
  The event-`0x61` check does not test key `0x02`.
- **`1084c5e0(handler, context, id, work, flavor)`** takes the flavor as a
  stack argument and returns when work word 0 is set or key `0x02` is not the
  battler. It commits a kind-4 record: `+6` the battler, `+4` (halfword)
  `1084d2f4(pokemon, parameter 3)`, and message `0x392` (literal `1084c6fc`)
  naming the battler and the item. It then asks `108014cc(pokemon word 0,
  flavor)` about the taste. When the answer is 2, it commits a kind-`0xb`
  record: `+0xf` the battler, `+4` status 6 (confusion), and `+8` the word
  that `1084c438(6, pokemon, +8)` writes. Work word 0 becomes 1 whether or
  not the battler was confused.
- **The `0x7c` Figy uses** load work word 6 (`+0x18`) and return through
  `bxeq lr` when it is zero. Otherwise they run the five instructions of the
  matching `0x7b` stub inline.
- **The stat checks** look the battler up and discard it. They pass the
  item's parameter 3 to `1084c198` or `1084c730` as the divisor. The
  event-`0x61` check does not test key `0x02`.
- **Kind `0xd`** holds `+4` the stat, `+0xc` the battler, `+0x16` count 1
  and `+0x17` the stages: 1 for the five single-stat berries, 2 for Starf.
  The handlers store these bytes in different orders.
- **Lansat** gives up when the battler already has state 9: bit 1 of the
  byte at `+0x16a`, read through `1084ca4c`. Its kind-`0x17` record holds
  `+4` 9 and `+5` the battler. The kind-3 record that follows carries message
  `0x3ef` (literal `108a607c`), naming the battler and the item.
- **Starf** copies each stat of the five-byte table (literal `108a68e0`, an
  ABS32 relocation to `108d2c3a`) for which `108722e8(pokemon, stat, +1)`
  allows a rise into the context's temporary buffer (`1084c7d0(context,
  5)`). It then picks one with `1084c7bc(count)`. The roll happens before the
  key-`0x02` test, so it is used even when the battler does not match.

## Validation

`make -C reconstructed/battle_items_2 test` runs 26,184 cases:

- **Figy checks:** each of the three checks over work word 0, condition
  `0xf`, three HP values, key `0x2f`, the battler and Gluttony.
- **Figy body:** eleven max HPs, nine divisors, four taste answers and six
  flavors. Every byte of both records is checked. The body is also run with
  the work word set, for seven other battlers, and with another battler and
  item.
- **Figy uses:** each berry's two uses, over work words 0 and 6 and the
  battler.
- **Stat checks:** each check over eight divisors (0 faults), max HP 0–9,
  HP 0–10, Gluttony, key `0x2f` and the battler.
- **Stat uses:** each single-stat berry for six battlers.
- **Lansat:** all 256 values of the state byte, with and without its
  neighbours set.
- **Starf:** every set of stats that can rise, every roll and two stat
  tables, with the buffer contents checked.
- **Dispatcher:** a Figy berry and Starf through the real dispatcher, from
  the check (which keeps the handler running) to the use (which then blocks
  the check).

ASan/UBSan pass. 45 mutations are each detected; one that did not compile
was replaced.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 23 handlers are bound to exactly 60 (item, event) pairs of the item
table and nothing else. It also confirms that `1084c5e0` is bound to nothing,
that every entry of items 159–163 and 201–207 is one of those pairs, and that
Starf's literal resolves to the table 1, 2, 3, 4, 5
([record](reproduction/record-comparison.json)).

## Limits

These remain external:

- the flavor answer `108014cc`, the condition word `1084c438`, the stage
  query `108722e8`, the random roll and the temporary buffer
- the item parameters, record stack, messages and commit

The divide in `1084d2f4` is C division, and a divisor of 0 is not tested,
since the original items use 8. Leppa (154), Enigma and the resist berries
are not reconstructed yet. No running-game comparison has been made. Static
evidence and host comparisons do not establish a matching original build.
