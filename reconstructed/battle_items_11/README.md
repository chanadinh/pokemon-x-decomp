# Battle item handlers, batch 11: items of event 0x8c

This milestone reconstructs the eleventh batch of the `DllBattle.cro` item
handlers. These are the items of event `0x8c`, which runs after a move for
the battlers it hit, together with their uses. That makes 7 handlers bound
through the item table `108d382c`, covering every entry of four items:

- Red Card (542)
- Eject Button (547)
- Kee Berry (687)
- Maranga Berry (688)

The batch also reconstructs two switch-flag helpers, `1084c000` and
`1084c020`.

Six routines were reviewed through live REA MCP/Ghidra listings in session
`outputs/rea-battle-8`. Three handlers have no Ghidra function and are listed
by the [move-power milestone's decoder](../battle_power/README.md#decoder).
Every instruction shape they use (with registers, immediates, literal
addresses and branch targets abstracted) is REA-listed in this batch or at
one of three spelling references (`108af7a4`, `108af7a8`, `108bbaf0`).

| Event | Address (static) | Item | Effect |
| --- | --- | --- | --- |
| | `1084c000` | (helper) | Claim this turn's Eject Button: set bit 3 of context `+0x1093`, 0 if it was already set |
| | `1084c020` | (helper) | Bit 0 of context `+0x1093`: a switch is pending |
| `0x8c` | `108a5e78` | Red Card | The holder was hit, and the attacker (key `0x03`) has an opposing slot (`1084cb78` ≠ `0xa`) and a replacement (`1084ca78`). Then, unless key `0x4e` or the battle is decided: kind `0x24` shows message `0x1a1`, and kind `0x2f` forces the switch |
| `0x8c` | `108ab734` | Eject Button | The holder was hit, and none of these hold: key `0x4e`, charging (`1084c030`), the battle decided, a switch pending. With a replacement, and the button claimed: ask for the item |
| `0x7b` | `108aa3d4` | Eject Button | With a replacement: kind 3 shows message `0x19e`, and kind `0x2a` (`+0x4c` the holder) switches out |
| `0x8c` | `108aaec4`, `108aadac` | Kee, Maranga | The holder was hit by a physical or special move (key `0x1b`) and Defense or Sp. Def can rise: ask for the berry |
| `0x7b`, `0x7c` | `108a7478`, `108a8a30` | Kee, Maranga | When the stat can rise: kind `0xd`, +1 |

Runtime addresses are static − `0x1015b000`. Item numbers are the original
ids, named after the public Generation VI lists.

## Details

- **The battlers hit** are key `0x05` (their number) and keys `0x06`,
  `0x07`, and so on. Each handler scans them for the holder, runs its checks
  at the first match, and returns. Stopping the scan at the first match is
  therefore equivalent.
- **Red Card's kind-`0x2f` record** holds `+4` halfword `0x2a7` (literal
  `108a5fbc`) and `+6` the attacker (key `0x03`, byte). It also sets the high
  nibble of `+7` to 1 and keeps the low nibble. The kind-`0x24` message
  (`0x1a1`, literal `108a5fb8`) names the holder and the attacker.
- **Asking for an item** marks the handler for re-entry (`1084c96c`) and
  commits the kind-0 record of `1084c948`, both from the
  [first batch](../battle_items_1/README.md).

## Validation

`make -C reconstructed/battle_items_11 test` runs 1,556 cases:

- **The helpers:** every value of the flag byte, with its neighbours
  checked, and the claim run twice.
- **Red Card:** eight lists of battlers hit (empty, the holder alone, others
  only, the holder later or twice, a high byte), over every answer of the
  four queries and key `0x4e`.
- **Eject Button:** the same lists, over all seven early returns, with the
  flag byte checked after each.
- **Eject Button's use:** three holders and both replacement answers.
- **Kee and Maranga:** the checks over the lists, four categories and both
  stage answers, and the uses over three holders. Every byte of each record
  is checked.
- **Dispatcher:** Eject Button through the real dispatcher, from the hit to
  the switch.

ASan/UBSan pass. 35 mutations are each detected. One equivalent mutation (a
mask differing only in a bit the `| 0x10` sets anyway) was replaced. Two that
did not compile were also replaced.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 7 handlers are bound to exactly 9 (item, event) pairs of the item
table and nothing else. It also confirms that both helpers are bound to
nothing, and that these are every entry of the four items
([record](reproduction/record-comparison.json)).

## Limits

These remain external:

- the opposing-slot, replacement, charging, battle-decided and stage
  queries
- the record stack, messages and commit

No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
