# Battle item handlers, batch 12: Leppa and Micle Berries

This milestone reconstructs the twelfth batch of the `DllBattle.cro` item
handlers: every entry of Leppa Berry (154) and Micle Berry (209). That is 13
handlers bound through the item table `108d382c`. The batch also
reconstructs three helpers:

- Leppa Berry's use body `108a9e1c`
- the last-move reader `1084c278`
- the move-slot lookup `1084c224`

Fourteen routines were reviewed through live REA MCP/Ghidra listings in
session `outputs/rea-battle-8`. Two handlers have no Ghidra function and are
listed by the [move-power milestone's decoder](../battle_power/README.md#decoder).
Every instruction shape they use is REA-listed in this batch or at one of
three spelling references:

- `bxeq lr` at `108695c4`
- `stmdb sp!,{r3,lr}` and `ldmia sp!,{r3,pc}` at `108a7584`

The `bx<cond>` form is spelled by the same rule as batches 10 and 11.

| Event | Address (static) | Item | Effect |
| --- | --- | --- | --- |
| | `1084c278` | (helper) | The last move used: the halfword at battler `+0x162` |
| | `1084c224` | (helper) | The slot (0–3) whose move id (halfwords at `+0x11c`, stride `0xe`) is `move`; 4 when none, or when `move` is 0 |
| | `108a9e1c` | (helper) | Leppa Berry's use: restore PP to a slot (below) |
| `0x04` | `108aa494` | Leppa | The last move's slot has no PP (`108520e4`): ask for the berry |
| `0x57` | `108a9710` | Leppa | The same test: key `0x53` = 1 |
| `0x5e`, `0xa9` | `1084c8b8` | Leppa | Any slot below the move count (`+0x152`) has no PP: ask. The routine falls through into `1084c948` |
| `0x61` | `108a977c` | Leppa | The same, without testing key `0x02` |
| `0x9c` | `108aa970` | Leppa | The same, with key `0x2f` bit 1 |
| `0x7b` | `108a8588` | Leppa | The use for the holder |
| `0x7c` | `108a9dfc` | Leppa | With work word 6 set (`bxeq lr` otherwise): the use with any restorable slot allowed |
| `0x02` | `108ab518` | Micle | Unless key `0x0c` is 6: at a pinch (parameter 3's low byte as the divisor, the first batch's `1084c730`), ask |
| `0x5e`, `0x61`, `0x9c` | `108aab1c`, `108ab5a0`, `108aab94` | Micle | The same pinch test; event `0x61` has no battler test, and `0x9c` also needs key `0x2f` bit 0 |
| `0x7b`, `0x7c` | `108a871c`, `108aa074` | Micle | Kind `0xb`: `+4` status `0x22`, `+8` `1084c3b0(0x78)`, `+0xf` the holder, message `0x40a` (event `0x7c` also needs work word 6) |

Runtime addresses are static − `0x1015b000`. Item numbers are the original
ids, named after the public Generation VI lists.

## Details

- **`108a9e1c(handler, context, id, work, any)`** chooses a slot in this
  order and returns whether it restored PP:
  1. The last move's slot, when that slot has no PP.
  2. The first slot below the move count with no PP. If that slot is slot
     4, this step gives up.
  3. With `any` (the stack argument, 1 only on event `0x7c`): the first slot
     below the count that `1084d184` accepts, unless that is slot 4.

  It then commits a kind-8 record: `+4` `0xa`, `+5` the holder, `+6` the
  slot, `+7` bit 0 set. Its message `0x395` (literal `108a9fbc`) names the
  holder, the berry and the slot's move (`10855c68`).
- **Asking for a berry** marks the handler for re-entry (`1084c96c`) and
  commits the kind-0 record of `1084c948`, both from the
  [first batch](../battle_items_1/README.md).
- **Micle's event-`0x9c` check** reads the divisor before testing key `0x02`
  a second time and the use flags.

## Validation

`make -C reconstructed/battle_items_12 test` runs 3,104 cases:

- **The helpers:** every move id below `0x30`, move 0, a duplicated move and
  an empty slot.
- **Leppa's checks:**
  - The last-move checks over six PP layouts, five last moves and three
    holders.
  - The any-slot checks over the layouts, move counts 0–6 (including a fifth
    slot), holders and use flags.
- **The use body:** six layouts, three last moves, four counts, both values
  of `any` and ten restorable masks, against an independent reference. The
  exact call sequences of three cases are checked, and `+7` keeps its other
  bits.
- **Leppa's uses:** both events, three holders and work word 6.
- **Micle:**
  - The four checks over holders, key `0x0c`, key `0x2f`, three parameters
    (one with a high byte) and three HPs.
  - The uses, with every byte of the record checked.
- **Dispatcher:** Leppa's check and use through the real dispatcher.

ASan/UBSan pass. 35 mutations are each detected. One equivalent mutation was
replaced: it removed Micle's repeated key-`0x02` test, which the earlier test
in the same frame makes redundant. Two that did not compile were also
replaced.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 13 handlers are bound to exactly 14 (item, event) pairs of the item
table and nothing else. It also confirms that the three helpers are bound to
nothing, and that these are every entry of the two berries
([record](reproduction/record-comparison.json)).

## Limits

These remain external:

- the PP, restorable-slot and slot-move queries
- the condition word `1084c3b0`
- the item parameters, record stack, messages and commit

No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
