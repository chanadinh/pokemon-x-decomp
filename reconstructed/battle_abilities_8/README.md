# Battle item and after-move abilities

This milestone reconstructs the eighth batch of the remaining
`DllBattle.cro` ability handlers: nine handlers that keep, steal, pass or
eat held items, or act once a move has finished. Eight were reviewed through
live REA MCP/Ghidra listings; Sticky Hold's message handler has no Ghidra
function and is listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder). Fainted
(`1085f844`) and the max-HP share (`1084d2f4`) come from
[battle_abilities_1](../battle_abilities_1/README.md).

| Event | Address (static) | Host routine | Abilities | Effect |
| --- | --- | --- | --- | --- |
| `0xa6` | `108be5b8` | `px_battle_sticky_hold_check` | Sticky Hold (60) | Its item cannot be taken (key `0x42`); work word = rewritten |
| `0xa7` | `108c3bb8` | `px_battle_sticky_hold_message` | Sticky Hold (60) | Announces it (message `0x1ed`), clears the word |
| `0x90` | `108c0aec` | `px_battle_pickpocket` | Pickpocket (124) | Steals a contact attacker's item into an empty hand |
| `0x8a` | `108c2444` | `px_battle_magician_held` | Magician (170) | Notes whether it holds an item before its move |
| `0x9b` | `108bdf78` | `px_battle_symbiosis` | Symbiosis (180) | Passes its item to an ally that used its own |
| `0x9b` | `108bf22c` | `px_battle_cheek_pouch` | Cheek Pouch (167) | Eating a berry also heals a third of max HP |
| `0x8c` | `108bfe10` | `px_battle_moxie` | Moxie (153) | Attack + one stage per fainted target |
| `0x8c` | `108be340` | `px_battle_color_change` | Color Change (16) | A target takes the move's type |
| `0x53` | `108c10a0` | `px_battle_cursed_body` | Cursed Body (130) | Disables the attacker's last move, 30% |

Runtime addresses are static − `0x1015b000`; ability and move numbers are
the original ids, named after the public Generation VI lists.

## Details

- **Item records** are kind `0x25` (`+4` the battler the item comes from,
  flagged `0x800000`) with a message at `+8`: Pickpocket's `0x1cc` names the
  thief and the item, Symbiosis's `0x4c3` (literal `108be0c4`) itself, the
  item and the ally.
- **Pickpocket** needs to be among the targets, `1084bb5c(context, id,
  attacker)` not to block the theft, move flag 0 (contact), its own hand empty
  and the attacker's item.
- **Symbiosis** needs the battler of key `0x02` to be its ally, its own item
  and the ally's empty hand, turn flags `0xf` and `0x11` (`1084c818`) clear,
  and `1085441c` to allow the item for the ally's species and then its own.
- **Cheek Pouch** needs key `0x52` and a berry (`108007a4`) in key `0x2e`'s
  low half; the flagged kind-`0x4` heal (message `0x183`, literal `108bf2fc`)
  names the handler's ability (`+0x38`).
- **Moxie** counts fainted targets (low bytes of keys from `0x06`) unless the
  battle is decided (`10865c48`); the flagged kind-`0xd` record has stages
  equal to the count.
- **Color Change** stops when the battle is decided, it fainted, the type's
  low byte is 18 or it already has the type, then commits kind `0x13` (`+4`
  both types `t | t << 5 | 0x4800`, `+6` the battler) inside the popup.
- **Cursed Body** needs key `0x47` clear, the attacker not disabled already
  (status `0xd`), its last move (key `0x14`, low half) set, not Struggle (165)
  and not refused by `1084c808`; then 30% or forced; four turns, five with
  turn flag 1; `1084beac(turns, move)` gives the condition word of the
  kind-`0xb` record.

## Validation

`make -C reconstructed/battle_abilities_8 test` covers Sticky Hold's two
handlers with key `0x42` already rewritten; Pickpocket over 0–3 targets,
the block, contact and both hands; Magician; Symbiosis over the ally answer,
both hands, both turn flags and both species locks; Cheek Pouch over keys,
the berry answer and max HP; Moxie over 0–4 targets with high-bit ids and
fainted masks; Color Change over the target list, type words above 31 and
both checks; Cursed Body over every check; and Sticky Hold's two handlers
through the real dispatcher (2,518 cases). ASan/UBSan pass; 34 mutations are
each detected by a failing test (two that did not compile were replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the nine handlers are bound to exactly nine (ability, event) pairs and
that every other ability entry of the events covered is a contact handler of
battle_abilities_1 or one of the deferred handlers of Magician (event `0x8c`)
and Illusion (event `0x53`) ([record](reproduction/record-comparison.json)).

## Limits

Magician's event-`0x8c` handler, which sorts the targets by speed through a
relocated global table, and Illusion are not reconstructed yet. The item,
flag, type, status, disable, chance, popup, message and action services remain
external. No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
