# Battle form abilities

This milestone reconstructs the tenth batch of the remaining
`DllBattle.cro` ability handlers: the 14 form handlers of Forecast
(Castform), Flower Gift (Cherrim) and Zen Mode (Darmanitan). Twelve were
reviewed through live REA MCP/Ghidra listings; two have no Ghidra function
and are listed by the [move-power milestone's decoder](../battle_power/README.md#decoder).
The switch-in Forecast and Flower Gift handlers are in
[battle_switchin](../battle_switchin/README.md).

| Event | Address (static) | Host routine | Abilities | Effect |
| --- | --- | --- | --- | --- |
| `0x73` | `108c22fc` | `px_battle_forecast_leave` | Forecast (59) | Leaving: Castform back to its normal form |
| `0x82` | `108c2c58` | `px_battle_forecast_suppressed` | Forecast (59) | The same once started (work word) |
| `0x02`, `0x81`, `0x87` | `108c2cec` | `px_battle_forecast_weather` | Forecast (59) | Once started: follow the weather |
| `0x93` | `108c3dc8` | `px_battle_forecast_lost` | Forecast (59) | Losing the ability: normal form |
| `0x60`, `0x61` | `108c50b4` | `px_battle_forecast_start` | Forecast (59) | Follow the weather and mark it started |
| `0x41` | `108c2fac` | `px_battle_flower_gift_defense` | Flower Gift (122) | Sun: allies' special defense ×1.5 |
| `0x40` | `108c3024` | `px_battle_flower_gift_attack` | Flower Gift (122) | Sun: allies' physical attack ×1.5 |
| `0x73` | `108c39b0` | `px_battle_flower_gift_leave` | Flower Gift (122) | Leaving: the form routine with "not sunny" |
| `0x82` | `108c40b0` | `px_battle_flower_gift_suppressed` | Flower Gift (122) | The same once started |
| `0x87`, `0x02`, `0x81` | `108c4108` | `px_battle_flower_gift_weather` | Flower Gift (122) | Once started: the form routine for the weather |
| `0x93` | `108c4d34` | `px_battle_flower_gift_lost` | Flower Gift (122) | Losing the ability: "not sunny" |
| `0x60`, `0x61` | `108c56d8` | `px_battle_flower_gift_start` | Flower Gift (122) | The form routine for the weather; mark it started |
| `0x81` | `108bfbe8` | `px_battle_zen_mode` | Zen Mode (161) | Zen form at half HP or less, standard above |
| `0x73`, `0x93` | `108c4cac` | `px_battle_zen_mode_leave` | Zen Mode (161) | Back to the standard form |

Runtime addresses are static − `0x1015b000`; ability and species numbers are
the original ids, named after the public Generation VI lists.

## Details

- **Form records** are kind `0x3a` (`+4` the battler, `+5` the form).
  Forecast's message is `0xde` naming the battler (kind 2), Zen Mode's `0xbf`
  (Zen) or `0xc0` (standard), kind 1; Zen Mode's event-`0x81` message has no
  argument. Changes toward the weather or Zen form are flagged `0x800000`;
  returns to the normal form are not.
- **Forecast** checks species 351 (`+0xc`) and the form byte `+0x153`: resets
  need a form other than 0; following the weather maps 1–3 to forms 1–3 and
  everything else to 0, reading the weather (`1084be58`) before the battler.
  The start handler sets its work word whatever the species.
- **The lost-ability checks** require key `0x02` and key `0x10` to differ
  from the handler's ability halfword `+0x38`.
- **Flower Gift** checks species 421 and calls the Cherrim form routine
  `1084a944(handler, context, id, sunny, flag)` (the flag on the stack); its
  boosts need the target (key `0x04`) or attacker (key `0x03`) to be an ally
  (`1084c5a8`) and key `0x1b` = 2 or move category 1. The start handler sets
  its work word only for Cherrim.
- **Zen Mode** checks species 555 and compares HP with `1084c124(…, 2)`
  (max HP / 2, battle_ability); the leave handler resets whatever the form.

## Validation

`make -C reconstructed/battle_abilities_10 test` covers every handler over
the battler key (including a high-bit id), four species words (one with a
high bit), forms, weathers 0–5, three work-word values, the new-ability key,
the ally answer and four categories; Zen Mode over HP and max HP; and Forecast
starting on event `0x61` and following the weather on `0x81` through the real
dispatcher (2,378 cases). ASan/UBSan pass; 30 mutations are each detected by a
failing test (four that did not compile and one equivalent were replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 14 handlers are bound to exactly 21 (ability, event) pairs and that
every ability entry of events `0x82`, `0x81`, `0x87` and `0x60` is one of
them ([record](reproduction/record-comparison.json)).

## Limits

The weather, ally, Cherrim form, category, message and action services remain
external. No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
