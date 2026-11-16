# Battle move handlers: events 0xae to 0xb4

This milestone reconstructs the 5 `DllBattle.cro` move handlers of the four
highest-numbered move events, bound to 8 (move, event) pairs: Final Gambit's
faint (event `0xb4`), the forced-switch message of Whirlwind and Roar
(`0xb3`), the contact penalties of Spiky Shield and King's Shield (`0xb1`)
and the Pledge combination (`0xae`). With it every move entry of those four
events is reconstructed. Two handlers were reviewed through live REA
MCP/Ghidra listings and three, which have no Ghidra function, through the
bytes read live and listed by the
[battle_power decoder](../battle_power/reproduction/decode_arm.py) (session
`outputs/rea-battle-8`). This is the first batch taken from the top of the
move table downward (the battle_moves_N milestones work upward).

| Event | Address (static) | Host routine | Moves | Effect |
| --- | --- | --- | --- | --- |
| `0xb4` | `108ba248` | `px_battle_final_gambit_faint` | Final Gambit (515) | Once key `0x53` is rewritten, the user faints (kind `0x12`) |
| `0xb3` | `108adce8` | `px_battle_roar_message` | Whirlwind (18), Roar (46) | Key `0x59` = message `0x2c9` for Roar, `0x2ca` otherwise |
| `0xb1` | `108ba570` | `px_battle_spiky_shield_hit` | Spiky Shield (596) | A contact move into it: the attacker loses 1/8 of its max HP |
| `0xb1` | `108baa30` | `px_battle_kings_shield_hit` | King's Shield (588) | A contact move into it: the attacker's Attack −2 |
| `0xae` | `108b93a4` | `px_battle_pledge_combine` | Water (518), Fire (519), Grass Pledge (520) | Combined with the partner's Pledge: the move's type and the combination |

Runtime addresses are static − `0x1015b000`; move numbers are the original
ids, named after the public Generation VI lists.

## Details

- The handler's id halfword `+0x38` is its move; Roar is told apart from
  Whirlwind by it (literals `108add28`, `108add2c`).
- **Spiky Shield** (key `0x04` is the protected battler, move flag 0 of key
  `0x12`'s low half) commits an unflagged kind-6 record: `+6` the attacker
  (key `0x03`'s low byte), `+4` 1/8 of its max HP through `1084d2f4`
  (battle_abilities_1), `+7` bit 0 cleared, message `0x4c0` naming the
  attacker. **King's Shield** commits kind `0xd`: stat 1, stages −2 (`0xfe`),
  count 1, flags `+0x18 | 0x16`, the attacker's id byte at `+0xc`.
- **The Pledges** need the user (key `0x03`) and `108c86a0(pokemon, &partner,
  &move)` to report a partner (a byte and a halfword in an 8-byte stack
  frame). Water (518, literal `108b94bc`), Fire (`518 | 518 >> 9` = 519) and
  Grass (520) pair as 1 water + fire, 2 grass + fire, 3 water + grass, in
  either order; any other pair does nothing. A combination stores work `+0`
  1, `+4` the combination and `+8` the partner, and rewrites the type key
  `0x16` to 10, 9 or 11.

## Validation

`make -C reconstructed/battle_moves_b1 test` covers Final Gambit over the
attacker key and key `0x53` already rewritten; Roar and Whirlwind over three
move halfwords; both shields over the defender key, contact and max HP; the
Pledges over all 35 pairs of five own moves and seven partner moves (two with
other high bytes), with and without a partner and with a stale frame; and both
shields through the real dispatcher (310 cases). ASan/UBSan pass; 29
mutations are each detected by a failing test (one that did not compile was
replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the five handlers are bound to exactly eight (move, event) pairs and that
every move entry of events `0xb4`, `0xb3`, `0xb1` and `0xae` is one of them
([record](reproduction/record-comparison.json)).

## Limits

The move-flag, partner, message and action services remain external. No
running-game comparison has been made. Static evidence and host comparisons
do not establish a matching original build.
