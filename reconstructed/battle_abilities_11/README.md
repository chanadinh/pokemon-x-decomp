# Battle field (position) handlers

This milestone reconstructs the five handlers that the `DllBattle.cro`
field table (`0x108d6810`) binds. Each belongs to a battle position, not a
battler: its value is the position, its work words hold a delayed effect,
and when the effect ends it clears its field entry and removes itself.
Three were reviewed through live REA MCP/Ghidra listings; two have no
Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder).
Fainted (`1085f844`) and the max-HP share (`1084d2f4`) come from
[battle_abilities_1](../battle_abilities_1/README.md), full HP (`1084d318`)
from [battle_abilities_6](../battle_abilities_6/README.md).

| Field | Event | Address (static) | Host routine | Effect |
| --- | --- | --- | --- | --- |
| 0 | `0x7f` | `108a5430` | `px_battle_field_wish` | A turn later, the battler at the position regains half the maker's max HP |
| 1 | `0x5e`, `0xb0` | `108a5890` | `px_battle_field_lunar_dance` | The battler entering is fully restored, with its PP |
| 2 | `0x5e`, `0xb0` | `108a5714` | `px_battle_field_healing_wish` | The battler entering is fully restored |
| 3 | `0x7f` | `108a55e0` | `px_battle_field_future_sight` | On its turn, the delayed attack strikes the battler at the position |
| 4 | `0x5d` | `108a555c` | `px_battle_field_entry` | The battler entering the position: a kind-`0x2b` record |

The field names follow the effects (Wish, Lunar Dance, Healing Wish,
Future Sight / Doom Desire); they are analyst names. Runtime addresses are
static − `0x1015b000`.

## Details

- **Ending** calls `1086c218(context, field, position)` with the low byte of
  the handler's id halfword `+0x38`, then `1084d1cc(handler)` (battle_handler)
  to remove it; both stay callbacks.
- **Wish** runs only for key `0x02` = `0x1f` (no battler) and when the battle
  is not decided (`10865c48`). The first time it stores the turn
  (`1084d364`) in work `+4` and marks work `+0`; from a later turn on, the
  battler at the position (`1084d348`, `0x1f` none) not at full HP regains
  half of the maker's (work `+0x18`) max HP (kind 4, message `0x2bc`); then
  it ends.
- **The delayed attack** waits until the turn's low byte reaches work `+0`
  (signed), then, unless the position is empty, holds its user (work `+0x18`)
  or a fainted battler, announces it (message `0x43e`, literal `108a5710`,
  with the target and the move, work `+4`) and commits kind `0x28` (`+4`
  user, `+5` target, `+6` work `+8`, `+8` the move); then it ends.
- **The entry handler** commits kind `0x2b` (`+4` work byte, `+5` the
  battler) for no battler (`0x1f`) when the battler entering (key `0x02`'s low
  byte) is at the position.
- **Healing Wish and Lunar Dance** need the entering battler's position
  (`10873158`) to be theirs. Healing Wish acts when it is hurt or has a status;
  Lunar Dance also when a move is short of PP (`1084d184`). Kind `0x38` (`+4`
  `0x2bb` or `0x2bc`, `+6` the position, `+7` 10, Lunar Dance `+0xb` 1)
  announces it (message `0x2b9` = `0x2bb` without bit 1, or `0x2b6`); then max
  HP minus HP is healed (kind 4), every status cured (kind `0xa`, `+4`
  `0x26`), and for Lunar Dance each short move refilled (kind 8: `+4` PP,
  `+5` battler, `+6` slot). Both end whether or not anything was needed.

## Validation

`make -C reconstructed/battle_abilities_11 test` covers Wish over the battler
key, the decided answer, the started flag, three turns, empty and occupied
positions and HP; the delayed attack over signed strike turns including a
negative one, empty, user-held, occupied and fainted positions; the entry
handler over battler ids including a high-bit one; Healing Wish and Lunar
Dance over position, HP, status, 0–2 moves and every PP mask; and Healing Wish
through the real dispatcher (642 cases). ASan/UBSan pass; 32 mutations are
each detected by a failing test (one that did not compile was replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the five handlers are bound to exactly seven (field, event) pairs and
that every entry of the field table is one of them
([record](reproduction/record-comparison.json)).

## Limits

The turn, position, PP, field-clear, handler-removal, status, decided,
message and action services remain external. No running-game comparison has
been made. Static evidence and host comparisons do not establish a matching
original build.
