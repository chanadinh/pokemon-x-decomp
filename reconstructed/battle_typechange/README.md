# Battle type-change handlers

This twenty-ninth host-source milestone reconstructs every `DllBattle.cro`
handler of event `0x28`, which decides a move's type before damage: four
abilities and seven moves. Nine were reviewed through live REA MCP/Ghidra
listings; two have no Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder).

| Address (static) | Host routine | Table entry | Effect on key `0x16` (type) |
| --- | --- | --- | --- |
| `108c0114` | `px_battle_normalize` | Normalize (ability 96) | Normal |
| `108bde48` | `px_battle_aerilate_type` | Aerilate (ability 184) | Normal → Flying, recorded in the work word |
| `108bf0b0` | `px_battle_pixilate_type` | Pixilate (ability 182) | Normal → Fairy, recorded |
| `108bf0fc` | `px_battle_refrigerate_type` | Refrigerate (ability 174) | Normal → Ice, recorded |
| `108b8750` | `px_battle_weather_ball` | Weather Ball (move 311) | Sun Fire, rain Water, hail Ice, sandstorm Rock |
| `108b8008` | `px_battle_curse` | Curse (move 174) | Key `0x1c`: 0 for a Ghost user, 7 otherwise |
| `108ba824` | `px_battle_struggle` | Struggle (move 165) | Key `0x4c` set to 1 |
| `108b9a54` | `px_battle_hidden_power` | Hidden Power (move 237) | The type service `10801afc` |
| `108b7708` | `px_battle_judgment` | Judgment (move 449) | The held plate's type |
| `108b9c28` | `px_battle_natural_gift` | Natural Gift (move 363) | Parameter 8 of the held item |
| `108b6398` | `px_battle_techno_blast` | Techno Blast (move 546) | The held Drive's type |

`1089e61c` (`px_battle_curse_form`) is the Ghost test behind Curse.
Runtime addresses are static − `0x1015b000`. Ability, move and item numbers
are the original table ids; names follow the public Generation VI lists.

## Details

- In this event key `0x02` (not `0x03`) names the move's user; every handler
  requires it to be its battler.
- All changes go through the [event rewrite](../battle_ability/README.md):
  a value entry accepts one rewrite, a scaled entry any number. Which kind
  key `0x16` is in this frame is set by the code that builds it, which is not
  part of this milestone. If it is a value entry, a second type change in the
  same frame is refused, and the converting abilities' work word (the flag the
  [move-power handlers](../battle_power/README.md) read) stays 0.
- **Aerilate, Pixilate and Refrigerate** clear their first work word, and for
  a type word of exactly 0 store the rewrite's result in it.
- **Weather Ball** keeps the low byte of the current type when there is no
  weather, and always tells `1084bef8(context, code)` the result (code 1 sun,
  4 rain, 2 hail, 3 sandstorm, 0 none).
- **Judgment and Techno Blast** do nothing unless `1084b5b8(context, id)`
  allows the held item. Judgment uses the 8-entry jump table at `108b7778`
  (relocated words) for Flame–Earth Plate (items 298–305), comparisons with
  Sky Plate (306, literal `108b784c`) for Sky–Iron Plate (306–313) and Pixie
  Plate (644); any other item gives Normal. Techno Blast gives Water,
  Electric, Fire or Ice for the Douse, Shock, Burn and Chill Drives (116–119),
  Normal otherwise, and tells `1084bef8` which (1–4, 0).
- **Hidden Power** aborts and uses Normal when the battler slot is empty.

## Validation

`make -C reconstructed/battle_typechange test` checks the battler test of all
eleven handlers; Normalize for every type; the converters over every type,
wide values and an already rewritten entry; every weather with the code;
Curse with and without the Ghost type; Struggle; Hidden Power including the
empty slot; Judgment for every item id below 768 against an independent
plate table, with and without the item allowed; Natural Gift; Techno Blast
over 30 items; and Normalize followed by Aerilate through the real
dispatcher (1,932 cases). ASan/UBSan pass; 17 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that each handler is bound to exactly its row, only for event `0x28`, and
that these are all of the event's entries (4 ability, 7 move)
([record](reproduction/record-comparison.json)).

## Limits

The weather, notice, item-permission, Hidden Power, item-parameter and type
services remain external, as does the code that builds the event `0x28`
frame. No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
