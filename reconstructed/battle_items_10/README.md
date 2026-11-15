# Battle item handlers, batch 10: the herbs

This milestone reconstructs the tenth batch of the `DllBattle.cro` item
handlers: every entry of White Herb (214), Mental Herb (219) and Power Herb
(271). That is 12 handlers bound through the item table `108d382c`. The
batch also reconstructs the helper `1084c140`, which reports a lowered stat.

Eleven routines were reviewed through live REA MCP/Ghidra listings in session
`outputs/rea-battle-7`. Two of Mental Herb's handlers have no Ghidra function
and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder), using
only instruction shapes REA lists in this batch. `108aa05c` ends in `bx lr`,
which the decoder refuses. A `bx<cond>` rule spells it, and that rule must
reproduce REA's listing of the same word in `1084c140`.

| Event | Address (static) | Item | Effect |
| --- | --- | --- | --- |
| | `1084c140` | (helper) | Any of the seven stages at battler `+0x104`–`+0x10a` (signed bytes, 6 neutral) is below 6 |
| `0x02` | `108a9ad0` | White Herb | Unless key `0x0c` is 6 (no battler test): a stage is lowered → ask for the herb |
| `0x60`, `0x61` | `108ab69c` | White Herb | A stage is lowered → ask (no key test) |
| `0x80` | `108aace0` | White Herb | Key `0x02` is the holder, a stage is lowered → ask |
| `0x9c` | `108aa6dc` | White Herb | Key `0x2f` bit 3, key `0x02` the holder, a stage lowered → ask |
| `0x7b` | `108a81b8` | White Herb | Kind `0x11` (`+4` the holder) restores the stages; kind 3 shows message `0x3f8` |
| `0x7c` | `108a9b28` | White Herb | The same, only with work word 6 set |
| `0x5e`, `0x9c` | `108a98a0` | Mental Herb | Key `0x02` the holder, a condition of the first batch's six-entry list (`1084c844`) → ask |
| `0x7b` | `108a8714` | Mental Herb | `mov r3, #0x28; b 1084c4c0`: the first batch's cure body with cure `0x28` |
| `0x7c` | `108aa05c` | Mental Herb | The same, only with work word 6 set (otherwise `bx lr`) |
| `0x9f` | `108abb48` | Power Herb | The attacker's move, unless Sky Drop (507): key `0x53` = 1; work word 0 = 1 when that took |
| `0xa2` | `108ab910` | Power Herb | The attacker with work word 0 set → ask |
| `0x7b` | `108a91ec` | Power Herb | Kind 3, message `0x404` (literal `108a9264`) |

Runtime addresses are static − `0x1015b000`. Item numbers are the original
ids, named after the public Generation VI lists.

## Details

- **Asking for a herb** marks the handler for re-entry (`1084c96c`) and
  commits the kind-0 record of `1084c948`. Both come from the
  [first batch](../battle_items_1/README.md), as do Mental Herb's scan
  `1084c844` and cure body `1084c4c0`.
- **`1084c140`** compares the stage bytes in order, signed, and stops at the
  first one below 6.
- **Power Herb's charge** compares the whole key `0x12` with Sky Drop. The
  event-key rewrite (`108658d4`) reports 0 when the key was already rewritten
  once, and then the work word keeps its value.
- **The event-`0x7c` uses** read work word 6 (`+0x18`) before anything else.

## Validation

`make -C reconstructed/battle_items_10 test` runs 2,483 cases:

- **`1084c140`:** every value of each of the seven bytes, with the
  neighbouring bytes set, and every set of lowered stages.
- **White Herb:** the four checks over lowered or not, three holders, five
  key-`0x0c` values and four key-`0x2f` words. Both uses are checked over
  holders and work word 6, with the restore record checked byte by byte.
- **Mental Herb:** the check and both uses over three condition states and
  three holders.
- **Power Herb:** the charge over four move words (including Sky Drop with a
  high half) and a rewrite that does not take. Also the check over three work
  words, and the use.
- **Dispatcher:** Power Herb's three entries through the real dispatcher.

ASan/UBSan pass. 26 mutations are each detected; two that did not compile
were replaced.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 12 handlers are bound to exactly 14 (item, event) pairs of the item
table and nothing else. It also confirms that `1084c140` is bound to nothing
and that these are every entry of the three herbs
([record](reproduction/record-comparison.json)).

## Limits

The record stack, messages and commit remain external, along with the
effect of the kind-`0x11` and kind-`0xa` records. No running-game comparison
has been made. Static evidence and host comparisons do not establish a
matching original build.
