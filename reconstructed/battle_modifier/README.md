# Battle damage-modifier handlers

This twenty-eighth host-source milestone reconstructs every `DllBattle.cro`
handler of event `0x4f`, the final damage multiplier of the
[damage calculation](../battle_damage/README.md): 30 handlers for six
abilities, 21 items and 12 moves, with the four helpers they share. Each
multiplies key `0x36`. 28 were reviewed through live REA MCP/Ghidra listings;
two have no Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder).

| Group | Handlers | Effect |
| --- | --- | --- |
| Type-resist berries (items 184–200, 686) | 18 wrappers of `1084c980` | Defender, move of the berry's type, super effective (Chilan Berry: any Normal hit): ×½ once |
| Expert Belt (268) | `108a8250` | Attacker, super effective: item factor |
| Life Orb (270) | `108aaa10` | Attacker: item factor |
| Metronome item (277) | `108a6698` | Same move used consecutively: (100 + n × parameter)%, at most 200% |
| Sniper (97) | `108bd260` | Critical hit (key `0x46`): ×1.5 |
| Solid Rock (116), Filter (111) | `108be2fc` | Defender, super effective: ×0.75 |
| Tinted Lens (110) | `108bf300` | Attacker, resisted: ×2 |
| Multiscale (136) | `108c002c` | Defender at full HP: ×½ |
| Friend Guard (132) | `108c07e8` | An ally of the defender: ×0.75 |
| Surf (57), Whirlpool (250) | `108b0b4c`, `108b4934` | Defender underwater (state 4): ×2 |
| Earthquake (89), Magnitude (222) | `108b5c70` | Defender underground (state 5): ×2 |
| Stomp, Body Slam and six others | `108ba188` | Defender minimized (state 8): ×2 |

| Address | Host routine | Behavior |
| --- | --- | --- |
| `1089e58c` | `px_battle_effectiveness_class` | Code 0 → 0, 7 → 1, above 7 → 2, otherwise 3 |
| `1084ca4c` | `px_battle_pokemon_state` | Bit `flag` of the state bytes at `+0x169` |
| `1084cbe4` | `px_battle_item_factor` | (item parameter 3 + 100)% in 4096 units |
| `1084c980` | `px_battle_resist_berry` | The shared berry rule and its consumption |

Runtime addresses are static − `0x1015b000`. Item, ability and move numbers
are the original table ids; names follow the public Generation VI lists.

## Details

- **Resist berries** require the defender, the full type word, key `0x47`
  zero and, unless the berry accepts any hit, a signed effectiveness code
  above 7. The handler's first work word marks the berry as used; with it
  zero the damage is halved, and unless `1084c70c(context)` reports a
  non-consuming pass the word is set and an action record (`10869f7c(context,
  0x15, id)` with bytes `+4 = 0x11`, `+5 = id`) is committed by `10867b48`.
- **The item factor** reads parameter 3 of the item at handler `+0x38`
  through `1084cf74` and computes `(p << 12) + 0x64000` in single precision
  with ±0.5 (literal `1084cc38`), truncated and divided by 100 with the
  truncating multiply-by-reciprocal (`0x51eb851f`). Expert Belt (20) gives
  `0x1333` and Life Orb (30) `0x14cc`. Because the scaled value is always a
  whole number, the ±0.5 never changes the result.
- **Metronome** needs a move not excluded by `1084c808`, a nonzero use streak
  (`108c86c8`, 16 bits) and the move equal to the last move (`1084c7fc`). The
  percentage `(parameter × streak + 100)` is narrowed to 16 bits before the
  200% cap; 0% (possible only through that narrowing) gives a zero factor.
- **Effectiveness** handlers pass the low byte of key `0x39`; Expert Belt and
  the berries compare the full signed word with 7.
- **Friend Guard** acts for every battler other than the defender (low byte
  of key `0x04`) that `1084c5a8(id, defender)` reports as its ally.

## Validation

`make -C reconstructed/battle_modifier test` covers the effectiveness classes
and state bits exhaustively; the item factor for parameters −4000 to 3989
against integer arithmetic; all 18 berries over every type, codes 5–9,
another defender, suppression, prior use and the non-consuming pass with the
action record; Expert Belt and Life Orb; Metronome over streaks, parameters,
moves and exclusion, both narrowing cases and its call order; every ability
condition; the four move handlers over sixteen state bits; and four handlers
through the real dispatcher (18,950 cases). ASan/UBSan pass; 27 mutations are
each detected (two further mutations of the ±0.5 constants are equivalent).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 30 handlers are bound to exactly 39 table rows, only for event
`0x4f`, and that these are all of the event's entries (6 ability, 21 item,
12 move) ([record](reproduction/record-comparison.json)).

## Limits

The item parameter, consumption, action-record, full-HP, ally, exclusion,
streak and last-move services remain external. The berries' other entries
(event `0x53`) and the moves' other effects are not part of this milestone.
No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
