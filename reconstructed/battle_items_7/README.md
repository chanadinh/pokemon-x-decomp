# Battle item handlers, batch 7: residual held items

This milestone reconstructs the seventh batch of the `DllBattle.cro` item
handlers: 12 handlers of eight residual held items, bound through the item
table `108d382c`.

- Toxic Orb and Flame Orb
- Sticky Barb
- Leftovers and Black Sludge
- Shell Bell
- Life Orb's recoil (its event-`0x4f` power handler is in
  [battle_modifier](../battle_modifier/README.md))
- Big Root

Ten handlers were reviewed through live REA MCP/Ghidra listings in session
`outputs/rea-battle-7`. Two have no Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder), using
only instruction forms REA lists in this batch.

| Event | Address (static) | Item | Effect |
| --- | --- | --- | --- |
| `0x80` end of turn (key `0x02`) | `108a74f0` | Toxic Orb 272 | Kind `0xb`: badly poisoned (status 5, condition word `0x79`), message `0xf0` |
| | `108a61a8` | Flame Orb 273 | Kind `0xb`: burned (status 4, `10860e00(4)`), `+0x1c` the item, message `0x102` |
| | `108ab434` | Sticky Barb 288 | Kind 6: loses max HP / parameter 3, message `0x414` |
| `0x7c` the item's use | `108aa8e8`, `108a90a0` | Toxic Orb, Flame Orb | The same status for battler key `0x03` |
| `0x53` after a hit (key `0x04`) | `108abbec` | Sticky Barb | A contact move (`1080049c(move, 0)`) by an attacker with no held item (`+0x12`): kind `0x25`, `+4` the attacker |
| `0x7f` | `108aad38` | Leftovers 234 | Below full HP (`1084d318`): kind 0 with `+4` bit 0, and the handler is marked for re-entry |
| `0x7b` | `108a8988` | Leftovers | Below full HP: kind 4 restores 1/16, message `0x398` |
| `0x7f` | `108a6f60` | Black Sludge 281 | Unless the battle is decided (`10865c48`): a Poison type restores max HP / parameter 3; any other type loses 1/8 |
| `0x8e` after the holder's hit (key `0x03`) | `108a7f0c` | Shell Bell 253 | Unless key `0x4e`: restores the damage dealt (key `0x33`) / parameter 3, at least 1 |
| | `108ab2cc` | Life Orb 270 | Kind 6: loses 1/10, message `0x3fb` naming the battler only |
| `0x96` | `108a7304` | Big Root 296 | Key `0x36` × the item factor |

Runtime addresses are static − `0x1015b000`. Item numbers are the original
ids, named after the public Generation VI lists.

## Details

- **Shares of max HP** come from `1084d2f4` (max HP / divisor, at least 1),
  linked from [battle_abilities_1](../battle_abilities_1/README.md).
- **Kind 6 (HP loss)**, for Sticky Barb and Black Sludge, holds:
  - `+4`: the amount
  - `+6`: the battler
  - `+8`: halfword `0x26f`
  - `+7`: bit 1 set, other bits kept
  - `+0xa`: the battler's side (`10873158`)
  - `+0xb`: `0xa`
  - a message at `+0xc`

  Life Orb's recoil fills only `+4`, `+6` and the message.
- **Black Sludge's type test** reads three 5-bit types from `10870994`
  (bits 0–4, 5–9 and 10–14) and compares each with Poison (3). Its restore
  has no full-HP test.
- **Shell Bell** divides with the unsigned divide veneer `1080041c`, which
  the host code represents with C division. A parameter of 0 is not tested.
- **The Leftovers check** passes `+4` of the kind-0 record through an `orr`,
  keeping its other bits. It marks the handler with the first batch's
  `1084c96c`.

## Validation

`make -C reconstructed/battle_items_7 test` runs 13,466 cases:

- **The orbs:** both events, five holders and six key-`0x03` words. Every
  byte of each record is checked.
- **Sticky Barb:** eight max HPs and four parameters. The contact transfer
  is checked over the holder, key `0x47`, contact, three held items and two
  move words.
- **Leftovers:** both handlers over the full-HP answer and four flag words.
- **Black Sludge:** fifteen type words (including 0x13 in each field) and
  both decided answers.
- **Shell Bell:** nine damages, four parameters and key `0x4e`.
- **Life Orb:** six max HPs.
- **Big Root:** three parameters.
- **Sweeps:** every max HP up to 2,000, and parameters 1–40.
- **Dispatcher:** the Leftovers check and restore through the real
  dispatcher.

ASan/UBSan pass. 39 mutations are each detected. One surviving mutation
exposed a missing type value, so that case was added. Two that did not
compile were replaced.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 12 handlers are bound to exactly 12 (item, event) pairs of the item
table and nothing else. It also confirms that every other entry of the eight
items is Life Orb's event-`0x4f` handler `108aaa10`, which battle_modifier
covers ([record](reproduction/record-comparison.json)).

## Limits

These remain external:

- the condition word, full-HP, packed-type, battle-decided, contact and side
  queries
- the item parameters, record stack, messages and commit

No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
