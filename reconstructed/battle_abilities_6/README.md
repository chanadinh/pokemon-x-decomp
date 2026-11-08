# Battle status-cure and secondary-effect abilities

This milestone reconstructs the sixth batch of the remaining
`DllBattle.cro` ability handlers: 21 handlers that cure or soften statuses
or remove a move's secondary effects, and four helpers they call. Seventeen
functions were reviewed through live REA MCP/Ghidra listings; eight
handlers have no Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder).
Fainted (`1085f844`) and the max-HP share (`1084d2f4`) come from
[battle_abilities_1](../battle_abilities_1/README.md).

| Event | Address (static) | Host routine | Abilities | Effect |
| --- | --- | --- | --- | --- |
| — | `1084aba4` | `px_battle_cure_status` | (helper) | If the battler has the status, cure it inside the popup |
| — | `1084a890` | `px_battle_secondary_effect` | (helper) | The move has a secondary effect |
| — | `1084d318` | `px_battle_full_hp` | (helper) | HP equals max HP |
| — | `1089e6fc` | `px_battle_max` | (helper) | Signed maximum |
| `0x02` | `1084ab94` | `px_battle_insomnia_cure` | Insomnia (15), Vital Spirit (72) | Cure sleep (falls through into `1084aba4`) |
| `0x02` | `108c1774` | `px_battle_limber_cure` | Limber (7) | Cure paralysis |
| `0x02` | `108c18b8` | `px_battle_immunity_cure` | Immunity (17) | Cure poison |
| `0x02` | `108c18c8` | `px_battle_own_tempo_cure` | Own Tempo (20) | Cure confusion |
| `0x02` | `108c3ba8` | `px_battle_water_veil_cure` | Water Veil (41) | Cure burn |
| `0x02` | `108c4f50` | `px_battle_magma_armor_cure` | Magma Armor (40) | Cure freeze |
| `0x02` | `108c1578` | `px_battle_oblivious_cure` | Oblivious (12) | Cure infatuation, then taunt |
| `0x02` | `108c18d8` | `px_battle_truant_sleep` | Truant (54) | Key `0x0c` = 7: clear its work word |
| `0x59` | `108c08b8` | `px_battle_shield_dust_effect` | Shield Dust (19) | Hit by another: key `0x42` = 1 |
| `0x6d` | `108c08f4` | `px_battle_shield_dust_status` | Shield Dust (19) | The same unless key `0x1e` = 8 |
| `0x52` | `108c11c0` | `px_battle_shield_dust_poison` | Shield Dust (19) | Target: key `0x48` = 1 |
| `0x76` | `108c1990` | `px_battle_shield_dust_flinch` | Shield Dust (19) | Target: key `0x42` = 1 |
| `0x8b` | `108c43a0` | `px_battle_shield_dust_targets` | Shield Dust (19) | The single target: key `0x48` = 1 |
| `0x8b` | `108c3e88` | `px_battle_sheer_force_power` | Sheer Force (125) | A move with a secondary effect: key `0x49` = 1 |
| `0x6d`, `0x59` | `108c5178` | `px_battle_sheer_force_effect` | Sheer Force (125) | The same: key `0x42` = 1 |
| `0x75` | `108c566c` | `px_battle_sheer_force_flinch` | Sheer Force (125) | Attacker: key `0x42` = 1 |
| `0x75` | `108bd678` | `px_battle_stench` | Stench (1) | No flinch chance yet (key `0x26`): key `0x27` = 10 |
| `0x5c` | `108c11ec` | `px_battle_natural_cure` | Natural Cure (30) | Cure every status on switch-out, quietly |
| `0x5c` | `108c0940` | `px_battle_regenerator` | Regenerator (144) | Heal a third of max HP, at most what is missing |
| `0x74` | `108c0140` | `px_battle_poison_heal` | Poison Heal (90) | Poison damage cancelled; heal 1/8 |
| `0x74` | `108c349c` | `px_battle_heatproof_burn` | Heatproof (85) | Burn damage halved, at least 1 |

Runtime addresses are static − `0x1015b000`; ability and move numbers are
the original ids, named after the public Generation VI lists.

## Details

- **The cure helper** `1084aba4(context, id, status)` asks `10856a88` and
  commits an unflagged kind-`0xa` record (`+4` status, `+5` the battler,
  `+0x11` 1) between the popup's phases. The event-`0x02` handlers do not
  check a battler key; Oblivious cures 7 and then `0xb`. Insomnia's
  handler sets up the arguments and falls through into the helper.
- **The secondary-effect test** `1084a890(move)` asks the move kind
  (`108007d4`) and then field `0xa` (`10800494`): an effect chance always
  counts; kind 4 counts unless field `0xb`'s low half is 8; kind 6 counts;
  kind 7 counts unless one of its stat changes (`108014bc`/`108014c4`, a stack
  word) is negative; otherwise only Secret Power (290).
- **Shield Dust** on event `0x8b` walks the targets (count key `0x05`,
  targets from key `0x06`) and acts only when it is found and the count is 1.
  Key `0x1e` and key `0x26` are compared as the whole word and the low byte.
- **Natural Cure** commits kind `0xa` with `+4` `0x26`, `+0x11` 1 and
  `+0x12` 1. **Regenerator** commits kind 7 (`+4` 1, `+5` 1, `+7` the
  battler, `+0x14` the HP word) unless it fainted or is at full HP.
- **Poison Heal** needs status key `0x1e`'s low half 5, rewrites damage key
  `0x33` to 0 first and commits a flagged kind-`0x4` heal of 1/8. **Heatproof**
  needs 4 and rewrites key `0x33` to the signed half (toward zero), at least
  1.

## Validation

`make -C reconstructed/battle_abilities_6 test` covers each cure over status
masks; Oblivious's two cures; Truant; every Shield Dust and Sheer Force
handler over all battler-key combinations and key values; the target list
over 0–4 targets with high-bit ids; the secondary-effect test over six move
kinds, the chance, field `0xb`, 0–2 stat changes of every sign including
zero, and three moves; Stench; the helpers; Natural Cure; Regenerator over
HP and max HP; Poison Heal and Heatproof over statuses and signed damage;
and Limber then Oblivious through the real dispatcher (32,079 cases).
ASan/UBSan pass; 32 mutations are each detected by a failing test (three
equivalent mutations and one that did not compile were replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 21 handlers are bound to exactly 23 (ability, event) pairs, that the
four helpers are bound to nothing, and that every other ability entry of the
events covered is Inner Focus or Serene Grace (battle_abilities_4) or one of
the deferred event-`0x02` handlers of Forecast and Flower Gift
([record](reproduction/record-comparison.json)).

## Limits

Forecast's and Flower Gift's event-`0x02` handlers are not reconstructed yet.
The status, move-data, popup and action services remain external. No
running-game comparison has been made. Static evidence and host comparisons
do not establish a matching original build.
