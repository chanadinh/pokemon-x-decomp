# Battle weather abilities

This milestone reconstructs the second batch of the remaining
`DllBattle.cro` ability handlers: 17 handlers of abilities that act by
weather. Thirteen were reviewed through live REA MCP/Ghidra listings; four
have no Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder). The
HP shares come from `1084d2f4`, reconstructed in
[battle_abilities_1](../battle_abilities_1/README.md).

| Event | Address (static) | Host routine | Abilities | Effect |
| --- | --- | --- | --- | --- |
| `0x88` | `108bd8fc` | `px_battle_ice_body` | Ice Body (115) | Hail: heal 1/16, cancel the hail damage |
| `0x88` | `108bfb58` | `px_battle_rain_dish` | Rain Dish (44) | Rain: heal 1/16, cancel |
| `0x88` | `108c4280` | `px_battle_dry_skin_weather` | Dry Skin (87) | Sun: lose 1/8 (with popup); rain: heal 1/8 |
| `0x88` | `108c340c` | `px_battle_solar_power_weather` | Solar Power (94) | Sun: lose 1/8 (with popup) |
| `0x88` | `108c368c` | `px_battle_overcoat` | Overcoat (142) | Cancel any weather damage |
| `0x88` | `108c4584` | `px_battle_sand_immunity` | Sand Veil (8), Sand Rush (146), Sand Force (159) | Cancel sandstorm damage |
| `0x88` | `108c4750` | `px_battle_snow_cloak_weather` | Snow Cloak (81) | Cancel hail damage |
| `0x13` | `108bd29c` | `px_battle_swift_swim` | Swift Swim (33) | Rain: Speed ×2 |
| `0x13` | `108c029c` | `px_battle_chlorophyll` | Chlorophyll (34) | Sun: Speed ×2 |
| `0x13` | `108beac0` | `px_battle_sand_rush` | Sand Rush (146) | Sandstorm: Speed ×2 |
| `0x39` | `108c01d8` | `px_battle_sand_veil` | Sand Veil (8) | Sandstorm: accuracy against it ×0.8 |
| `0x39` | `108c02f4` | `px_battle_snow_cloak` | Snow Cloak (81) | Hail: accuracy against it ×0.8 |
| `0x40` | `108c3d0c` | `px_battle_solar_power` | Solar Power (94) | Sun: special moves ×1.5 |
| `0x7f` | `108bf9fc` | `px_battle_hydration` | Hydration (93) | Rain: cure its status at the end of the turn |
| `0x0e` | `108c55c0` | `px_battle_leaf_guard` | Leaf Guard (102) | Sun: block a status |
| `0x5e` | `108c2ee0` | `px_battle_air_lock` | Cloud Nine (13), Air Lock (76) | Announce that weather has no effect |
| `0x83` | `108c51bc` | `px_battle_air_lock_suppress` | Cloud Nine (13), Air Lock (76) | Cancel weather (key `0x42`) |

Runtime addresses are static − `0x1015b000`; ability numbers are the
original ids, named after the public Generation VI lists. Weather: 1 sun,
2 rain, 3 hail, 4 sandstorm.

## Details

- **Two weather sources.** The end-of-turn handlers (event `0x88`) read the
  event's weather key `0x3a`; the others ask `1084be58(context)`. Ice Body,
  Rain Dish and Solar Power compare the whole key `0x3a` word; Dry Skin only
  its low byte. Ice Body and Rain Dish read key `0x3a` before the battler key.
- **Cancelling** rewrites key `0x42` to 1 (`108658d4`). Overcoat and the sand
  and hail immunities require weather damage, key `0x33`, to be positive as a
  signed word. Dry Skin does not cancel.
- **HP records** are kind `0x4` (heal) and `0x6` (damage): `+6` the battler,
  `+4` the share of max HP. Heals are flagged `0x800000`; the sun damage of
  Dry Skin and Solar Power is not, and sits inside the ability popup. Solar
  Power takes its share before the popup.
- **Multipliers** go to key `0x36` through `1084ce84`: `0x2000` for speed,
  `0x1800` for Solar Power's special moves (category 2 of key `0x12`'s low
  half), `0xccd` (literals `108c0218`, `108c0334`) for evasion, where the
  battler is the defender (key `0x04`).
- **Hydration** checks `1086d8ac` and commits a kind-`0xa` record (`+4`
  `0x26`, `+5` the battler, `+0x11` 1, flag `0x2000000`) inside the popup.
- **Cloud Nine / Air Lock** commit a kind-`0x1e` weather record with weather
  0 and `+6` set (message `0x61`, kind 1).

## Validation

`make -C reconstructed/battle_abilities_2 test` runs every handler against a
reference model over all combinations of the three battler keys, seven
values of key `0x3a` (including high-bit words), five weathers, three signs of
key `0x33`, three move categories and max HP values, and two statuses,
checking the call log, key `0x36`, key `0x42` and every committed record;
plus Overcoat then Ice Body through the real dispatcher (85,681 cases).
ASan/UBSan pass; 32 mutations are each detected by a failing test (two that
did not compile under `-Werror` were replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 17 handlers are bound to exactly 21 (ability, event) pairs and that
every ability entry of events `0x88` and `0x83` is one of them
([record](reproduction/record-comparison.json)).

## Limits

The weather, popup, message and action services remain external. Forecast
and Flower Gift, which also follow the weather, are not reconstructed yet. No
running-game comparison has been made. Static evidence and host comparisons
do not establish a matching original build.
