# Battle item handlers, batch 9: turn order, survival and flinch items

This milestone reconstructs the ninth batch of the `DllBattle.cro` item
handlers, 12 handlers bound through the item table `108d382c` to eight
items:

- **Turn order (event `0xf`):** Custap Berry, Quick Claw, Lagging Tail and
  Full Incense.
- **Holding on with 1 HP:** Focus Band and Focus Sash.
- **Flinching:** King's Rock and Razor Fang.

Ten handlers were reviewed through live REA MCP/Ghidra listings in session
`outputs/rea-battle-7`. Two have no Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder), using
only instruction shapes REA lists in this batch.

| Event | Address (static) | Item | Effect |
| --- | --- | --- | --- |
| `0xf` | `108aba18` | Custap Berry 210 | At a quarter of max HP or less (the first batch's pinch test, halved for Gluttony): key `0x11` = 2; when the rewrite takes, ask for the item |
| `0x7b` | `108a761c` | Custap Berry | Kind 3, message `0x407`; work word 0 cleared |
| `0xf` | `108abc80` | Quick Claw 217 | Parameter 3's low byte as a percent chance (`1084c0f8`) or forced (`1084c0ec(context, 2)`): the same |
| `0x7b` | `108a9a4c` | Quick Claw | As Custap's |
| `0xf` | `108a8000` | Lagging Tail 279, Full Incense 316 | Key `0x11` = 0 |
| `0x75` | `108a7aa0` | King's Rock 221, Razor Fang 327 | The attacker's flinch chance key `0x27` = parameter 3's low byte, unless key `0x26` is set |
| `0x7c` | `108aac94` | the same | Kind `0x2c`: `+4` the holder, `+5` 100 |
| `0x7d` | `108ab354` | Focus Band 230 | Hit, with the parameter-3 chance or forced: key `0x3b` = 3, work word 0 = whether that took |
| `0x7e` | `108aaa48` | Focus Band | Work word 0 set: ask for the item, clear the word |
| `0x7b` | `108ab81c` | Focus Band | Kind 3, message `0x3f5` |
| `0x7d` | `108aa578` | Focus Sash 275 | Hit at full HP (`1084d318`): key `0x3b` = 3, work word 0 = whether that took |
| `0x7e` | `108a9810` | Focus Sash | Work word 0 set: kind `0x24`, message `0x3f5`, clear the word |

Runtime addresses are static − `0x1015b000`. Item numbers are the original
ids, named after the public Generation VI lists.

## Details

- **Event-key rewrites** (`108658d4`, from
  [battle_ability](../battle_ability/README.md)) report whether they took. A
  value that has already been rewritten once is not rewritten again, and
  the call reports 0.
  - The priority items ask for themselves only when the rewrite took.
  - The Focus items keep the answer in work word 0, which their event-`0x7e`
    handlers test.
  - Lagging Tail ignores it.
- **Messages** name the holder and the item: at `+4` of a kind-3 record, or
  at `+8` of Focus Sash's kind-`0x24` record. The literals are `108a769c`
  and `108a9acc` (`0x407`), and `108ab894` and `108a989c` (`0x3f5`).

## Validation

`make -C reconstructed/battle_items_9 test` runs 466 cases:

- **Custap Berry:** every HP 0–40 against a max HP of 80, with and without
  Gluttony, with the rewrite taking or not.
- **Quick Claw:** both chance answers, both forced answers and three
  parameter words.
- **The event-0x7b uses:** three holders, with the work word checked.
- **King's Rock:** three key-`0x26` values and three parameters, plus its
  use record byte by byte.
- **Both Focus items:** every endure and show combination, including a
  rewrite that does not take.
- **Dispatcher:** Focus Sash's two entries through the real dispatcher.

ASan/UBSan pass. 31 mutations are each detected; one that did not compile
was replaced.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 12 handlers are bound to exactly 15 (item, event) pairs of the item
table and nothing else, and that these are every entry of the eight items
([record](reproduction/record-comparison.json)).

## Limits

These remain external:

- the chance, forced-chance and full-HP queries
- the item parameters, record stack, messages and commit

No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
