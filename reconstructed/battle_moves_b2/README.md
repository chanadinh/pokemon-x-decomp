# Battle move handlers: event 0xac, first group

This milestone reconstructs 17 `DllBattle.cro` move handlers of event `0xac`
(the main effect of a status move), the second batch taken from the top of the
move table downward (the battle_moves_N milestones work upward). They are the
17 shortest REA-listed handlers of that event, bound to 18 (move, event) pairs,
reviewed through live REA MCP/Ghidra listings (session `outputs/rea-battle-8`).
Every handler acts only when key `0x03` is the user; key `0x05` is the target
count and key `0x06` the first target.

| Address (static) | Host routine | Moves | Effect |
| --- | --- | --- | --- |
| `108b0f9c` | `px_battle_teleport_effect` | Teleport (100) | Re-entry mark, escape record `0x29` |
| `108b4524` | `px_battle_hold_hands_effect` | Hold Hands (607) | Modes 1 and 2: key `0x53` rewritten, record `0x3c` |
| `108b29a8` | `px_battle_wish_effect` | Wish (273) | Kind `0x1f` at the user's position |
| `108b5948` | `px_battle_kings_shield_effect` | King's Shield (588) | Protect start, kind `0x15` (`0x10`), work `+0x18` = 1 |
| `108b5168` | `px_battle_grudge_effect` | Grudge (288) | Message `0x278`, work `+0x18` = 1 |
| `108b987c` | `px_battle_follow_me_effect` | Follow Me (266), Rage Powder (476) | Message `0x29e`, work `+0x18` = 1 |
| `108b2dc4` | `px_battle_quash_effect` | Quash (511) | Kind `0x32` for the target, message `0x471` |
| `108b26c4` | `px_battle_magic_coat_effect` | Magic Coat (277) | Once (work `+0x18` = 0): message `0x2f9` |
| `108afa98` | `px_battle_refresh_effect` | Refresh (287) | Status 5, 1 or 4 cured (kind `0xa`) |
| `108b3974` | `px_battle_forests_curse_effect` | Forest's Curse (571) | Target gains Grass (kind `0x14`) unless it has it |
| `108ae854` | `px_battle_transform_effect` | Transform (144) | Kind `0x34` for the target, message `0x284` |
| `108b359c` | `px_battle_magnet_rise_effect` | Magnet Rise (393) | Condition `0x1e`, five turns, message `0x292` |
| `108b11e0` | `px_battle_snatch_effect` | Snatch (289) | Unless the user moves last: message `0x2ef`, work `+0x18` = 1 |
| `108adb64` | `px_battle_imprison_effect` | Imprison (286) | Kind `0x1c`, condition, message `0x24a` |
| `108af690` | `px_battle_torment_effect` | Torment (259) | Condition `0xc` unless the target has it |
| `108b3be8` | `px_battle_soak_effect` | Soak (487) | One pure-Water record (`0x494a`) per target |
| `108afcec` | `px_battle_conversion_effect` | Conversion (160) | The user takes its first move's type unless it has it |

Runtime addresses are static − `0x1015b000`; move numbers are the original
ids, named after the public Generation VI lists. The re-entry mark
(`1084c96c`) is `battle_items_1`'s and the turn-count word (`1084be3c`) is
`battle_moves_5`'s, both linked rather than repeated.

## Details

- Records are created by `10869f7c` and committed by `10867b48`; messages use
  `1084d2d4` / `1084d29c`. The header-only record `1084c948(context, kind, id)`
  is a service (Teleport `0x29`, Hold Hands `0x3c`).
- **Soak** loads its literal `108b3c68` with `ldrh`: the halfword is the packed
  type pair (`10 | 10 << 5 | 0x4800`) of pure Water. **Conversion** packs the
  type the same way from the first move's type.
- **Imprison** keeps the top two bits of the halfword at `+0xe` and ORs `0x82`.

## Validation

`make -C reconstructed/battle_moves_b2 test` covers every handler over the
user key (own, other, high-bit id), the services' results and the
unwritten bytes of each record, plus the dispatcher (341 cases). ASan/UBSan
pass; 39 mutations are each detected by a failing test (one that did not
compile was replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 17 handlers are bound to exactly 18 (move, event) pairs
([record](reproduction/record-comparison.json)).

## Limits

The condition, status, type, move, message and action services remain
external. No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
