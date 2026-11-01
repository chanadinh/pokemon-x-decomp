# Battle switch-in handlers

This thirty-third host-source milestone reconstructs 23 of the 28
`DllBattle.cro` handlers of event `0x94`, which runs when a battler enters
(most of them also serve event `0x5e`, when an ability takes effect
mid-battle). Fourteen were reviewed through live REA MCP/Ghidra listings;
nine have no Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder). Trace,
Download, Forewarn, Anticipation and Frisk are the remaining five.

| Group | Address (static) | Abilities | Effect |
| --- | --- | --- | --- |
| Weather | `1084aca8`, `108bef5c`, `108bf8f8`, `108c02dc` (and `1084acbc`) | Drought, Drizzle, Sand Stream, Snow Warning | Sun, rain, sandstorm or hail for 5 turns; 8 with Heat, Damp, Smooth or Icy Rock |
| Cures | `108c06c8`, `108bfd2c`, `108c072c`, `108c07b0`, `108c2b50`, `108c4368`, `108c0518` | Limber, Insomnia/Vital Spirit, Immunity, Own Tempo, Water Veil, Magma Armor, Oblivious | Cure their statuses (Oblivious two) through `1084aba4` |
| Intimidate | `108c1ee8` | Intimidate | One record: Attack −1 for every opposing battler |
| Unnerve | `108c202c` | Unnerve | Announced once, naming the opposing side; hook stored |
| Forecast | `108c2210` | Forecast | Castform's form for sun, rain or hail |
| Flower Gift | `108c393c` | Flower Gift | Cherrim's helper with "sunny" |
| Slow Start | `108c4500` | Slow Start | Four work words and its message |
| Truant | `108c0074` | Truant | Work word when `1084c818` reports |
| Announcements | `108c4918`, `108c3c3c`, `108c3800`, `108c4028`, `108c3f58` | Mold Breaker/Turboblaze/Teravolt, Pressure, Dark Aura, Fairy Aura, Aura Break | The ability popup around a message |
| Klutz | `108c3fe0` | Klutz | Hook stored once |

Runtime addresses are static − `0x1015b000`; ability, item and species
numbers are the original ids, named after the public Generation VI lists.

## Details

- Key `0x02` is the entering battler; every handler except Unnerve requires
  it to be its own.
- **The weather setter** `1084acbc(context, id, weather, rock)` reads the
  held item (`+0x12`) only when there is a rock, then asks `1084b5b8` whether
  it may be used; it commits a record of kind `0x1e` (`+4` weather, `+5`
  turns, flag `0x800000`). Drought's handler falls through into it; the
  other three branch to it with their rock in a literal.
- **Intimidate** takes the opposing selector (`1084cb78`) with bit 8, a
  10-byte temporary buffer (`1084c7d0`) and the listed battlers
  (`1084bc20`); with none it does nothing. The record (kind `0xd`) holds stat
  1, stages `0xff`, the count and the battler ids from `+0xc`.
- **Unnerve and Klutz** store relocated code addresses in their handler's
  word `+0xc` (literals `108c20d0` → `108c4e70`, `108c4024` → `108c3750`),
  which the host takes from the caller, and mark their work word.
- **Forecast** compares species `+0xc` with 351 and the form byte `+0x153`
  with 1, 2, 3 or 0 by weather, commits a kind-`0x3a` record with message
  `0xde` when they differ, and always marks its work word.
- **Mold Breaker's** message is chosen by the handler's ability id
  (`+0x38`): Turboblaze `0x1f9`, Teravolt `0x1f6`, otherwise `0x1ba`.

## Validation

`make -C reconstructed/battle_switchin test` checks that no handler acts for
another battler; every weather setter over eight held items with and without
permission; all seven cures; Intimidate with 0–3 opposing battlers and the
record; Unnerve once and its side; Klutz; Forecast over species, weathers and
forms; Flower Gift; Slow Start's words; Truant; the five announcements; and
Drought then Intimidate through the real dispatcher (171 cases). ASan/UBSan
pass; 23 mutations are each detected (one further mutation was equivalent and
replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 23 handlers are bound to exactly 45 (ability, event) pairs on events
`0x94` and `0x5e`, and that the other entries of event `0x94` are exactly the
five deferred handlers ([record](reproduction/record-comparison.json)).

## Limits

Trace, Download, Forewarn, Anticipation and Frisk are not reconstructed yet.
The cure, item, weather, side, buffer, popup, message and action services
remain external. No running-game comparison has been made. Static evidence
and host comparisons do not establish a matching original build.
