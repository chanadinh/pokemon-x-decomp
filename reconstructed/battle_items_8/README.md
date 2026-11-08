# Battle item handlers, batch 8: items set off by a hit

This milestone reconstructs the eighth batch of the `DllBattle.cro` item
handlers: the items set off when their holder is hit (events `0x53` and
`0x54`), and their uses on event `0x7b`. That makes 19 handlers bound
through the item table `108d382c` to 28 items:

- Rocky Helmet and Air Balloon
- Absorb Bulb, Cell Battery, Snowball and Luminous Moss
- Weakness Policy
- Enigma, Jaboca and Rowap Berries
- the 18 resist berries' event-`0x53` handler (their event-`0x4f` halves are
  in [battle_modifier](../battle_modifier/README.md))

Thirteen handlers were reviewed through live REA MCP/Ghidra listings in
session `outputs/rea-battle-7`. Six have no Ghidra function and are listed by
the [move-power milestone's decoder](../battle_power/README.md#decoder). Every
instruction shape they use (mnemonic and operand layout, with registers,
immediates and literal addresses abstracted) is REA-listed in this batch or
at one of five spelling references.

| Event | Address (static) | Item | Effect |
| --- | --- | --- | --- |
| `0x53` | `108a6e58` | Rocky Helmet 540 | A contact hit: the attacker loses max HP / parameter 3 (kind 6, `+6` the attacker, message `0x1a8`) |
| `0x5e` | `108a8f24` | Air Balloon 541 | Unless gravity (`1084c374(context, 2)`): kind 3, message `0x198` |
| `0x12` | `108aa524` | Air Balloon | The holder floats: key `0x53` = 1 |
| `0x53` | `108ab254` | Air Balloon | Any hit: kind `0x21` (`+4` 0, `+6` the holder, `+0xa` 1), message `0x19b` |
| `0x53` | `108aaa9c`, `108aaf64`, `108ab194`, `108ab61c` | Absorb Bulb 545, Cell Battery 546, Snowball 649, Luminous Moss 648 | A Water, Electric, Ice or Water hit when Sp. Atk, Attack, Attack or Sp. Def can still rise (`108722e8`): ask for the item |
| `0x7b` | `108a7130`, `108a7788`, `108a8530` | the same (546 and 649 share one) | Kind `0xd`: that stat +1 |
| `0x53` | `108aafe8` | Weakness Policy 639 | A super-effective hit (key `0x39` > 7): remember the attacker |
| `0x7b` | `108a9fc0` | Weakness Policy | Attack +2 and Sp. Atk +2 (two kind-`0xd` records) |
| `0x53` | `108ab960` | Resist berries 184–200, 686 | Work word 0 positive (the berry weakened a hit): clear it, note the eater (`10821720`), kind `0x24` with message `0xdb` |
| `0x53` | `108a6310` | Enigma Berry 208 | A super-effective hit, unless condition `0xf`: ask for the item |
| `0x7b` | `108a7840` | Enigma Berry | Kind 4: max HP / parameter 3, no message |
| `0x54` | `108a8f98`, `108a99a8` | Jaboca 211, Rowap 212 | A physical or special hit (key `0x1b`) by a move `1084c808` does not exclude: remember the attacker |
| `0x7b` | `108a76a0` | Jaboca, Rowap | The remembered attacker, unless fainted, loses max HP / parameter 3 (kind 6, message `0x40d`) |

Runtime addresses are static − `0x1015b000`. Item numbers are the original
ids, named after the public Generation VI lists.

## Details

- **Checks** on event `0x53` require key `0x04` (the battler hit) to be the
  holder and key `0x47` to be clear. The two exceptions are Air Balloon's
  pop, which tests only key `0x04`, and the resist berries' handler, which
  tests the work word first.
- **Asking for an item** marks the handler for re-entry (`1084c96c`) and
  commits the kind-0 record of `1084c948`. Weakness Policy, Jaboca and
  Rowap instead keep the attacker (key `0x03`) + 1 in work word 0, then
  commit a kind-0 record with `+4` bit 1 themselves. Jaboca and Rowap's use
  reads that word back, minus 1, as the battler to damage.
- **Shares of max HP** use parameter 3's low byte as the divisor, through
  `1084d2f4`. The fainted test is `1085f844`. Both are linked from
  [battle_abilities_1](../battle_abilities_1/README.md).
- **Kind `0xd`** holds `+4` the stat, `+0xc` the holder, `+0x16` 1 and
  `+0x17` the stages. The handlers store these bytes in different orders.
- **`10821720(context, id)`** writes the battler into a packed 5-bit slot
  of a context word. Its role, noting the berry's eater, is an inference
  from its callers; it stays external.

## Validation

`make -C reconstructed/battle_items_8 test` runs 81,249 cases:

- **Rocky Helmet:** the holder, key `0x47`, contact, four parameters (with
  high bits), five max HPs and two move words.
- **Air Balloon:** all three handlers over three holders and both gravity
  answers.
- **The four stat-item checks:** each over types 0–`0x12`, the holder, key
  `0x47` and both stage answers.
- **The stat uses and Weakness Policy's two records:** five holders, with
  every byte of each record checked.
- **The remembering checks:** six effectiveness words (including negative
  ones, which must not count), and Jaboca and Rowap over four categories and
  the exclusion answer.
- **The resist berries:** six work words, including negative ones.
- **Enigma Berry:** both handlers.
- **Jaboca and Rowap's use:** five work words, fainted or not.
- **Sweeps:** max HP 0–2,000 with parameters 1–40 for the three HP
  shares.
- **Dispatcher:** Jaboca through the real dispatcher.

ASan/UBSan pass. 42 mutations are each detected; one that did not compile
was replaced.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 19 handlers are bound to exactly 38 (item, event) pairs of the item
table and nothing else. It also confirms that every other entry of the 28
items is one of the resist berries' event-`0x4f` handlers, which
battle_modifier covers ([record](reproduction/record-comparison.json)).

## Limits

These remain external:

- the contact, stage, field, exclusion and berry-note queries
- the status test, item parameters, record stack, messages and commit

No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
