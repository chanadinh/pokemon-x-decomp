# Battle end-of-turn abilities

This milestone reconstructs the third batch of the remaining
`DllBattle.cro` ability handlers: the nine ability handlers of events
`0x80` (end of turn) and `0x7f` (end-of-turn status) that were left;
Hydration, the tenth, is in [battle_abilities_2](../battle_abilities_2/README.md).
Eight were reviewed through live REA MCP/Ghidra listings; Moody has no
Ghidra function and is listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder). HP
shares come from `1084d2f4`, reconstructed in
[battle_abilities_1](../battle_abilities_1/README.md).

| Event | Address (static) | Host routine | Ability | Effect |
| --- | --- | --- | --- | --- |
| `0x80` | `108bcff0` | `px_battle_speed_boost` | Speed Boost (3) | Speed +1 once battle flag 0 is set |
| `0x80` | `108bdb4c` | `px_battle_moody` | Moody (141) | One random stat +2 and another −1 |
| `0x80` | `108beb00` | `px_battle_harvest` | Harvest (139) | Regain a consumed berry: always in sun, otherwise 50% |
| `0x80` | `108bf590` | `px_battle_pickup` | Pickup (53) | Take an item an opponent consumed this turn |
| `0x80` | `108bf750` | `px_battle_bad_dreams` | Bad Dreams (123) | Each sleeping opponent loses 1/8 |
| `0x80` | `108c4bac` | `px_battle_slow_start_turn` | Slow Start (112) | Counts five acting turns, then announces the end |
| `0x7f` | `108bc800` | `px_battle_shed_skin` | Shed Skin (61) | Cure its status, 33% |
| `0x7f` | `108bff30` | `px_battle_healer` | Healer (131) | Cure each other ally on its side, 30% each |
| `0x7f` | `108c282c` | `px_battle_levitate_turn` | Levitate (26) | Clear its work word |

Runtime addresses are static − `0x1015b000`; ability numbers are the
original ids, named after the public Generation VI lists.

## Details

- Key `0x02` is the battler concerned. Battlers: species `+0xc`, held item
  `+0x12`, consumed item `+0x14`, id byte `+0x19`.
- **Chances** use `1084c0f8(percent)` with the `1084c0ec(context, 1)`
  fallback: Shed Skin 33, Healer 30, Harvest 50 (skipped in sun).
- **Cures** are kind-`0xa` records (`+4` `0x26`, `+5` the battler,
  `+0x11` 1, flags `0x2800000`).
- **Moody** asks `108722e8` for every stat 1–7, raising (+1) then lowering
  (−1), into a `1084c7d0(context, 0xb6)` buffer: raisable stats at `+0xa8`,
  lowerable at `+0xaf`, and, when both exist, every pair of different stats as
  halfwords `raise << 8 | lower` from `+0` (at most `0x54`). One
  `1084c7bc` roll picks a pair, or a single stat when one list is empty. The
  stat records (kind `0xd`, stages 2 and `0xff`) are not flagged and sit
  inside the popup, which is shown even when nothing changes. With one stat
  in both lists there are no pairs: the roll is `1084c7bc(0)` and the buffer's
  first halfword decides.
- **Harvest and Pickup** commit kind-`0x21` item records (`+4` item, `+6` the
  battler, messages `0x1db` and `0x1ea` with the battler and item). Pickup
  needs no held item; it lists the opponents (`1084cb78 | 0x200`) into a
  `1084c7d0(context, 0xe)` buffer (count `+0xc`, `+0xd` cleared, candidates
  from `+6`), keeps those with turn flag 8 (`1084c818`) and a consumed item,
  rolls one, and asks `1085441c` whether its species may hold the item.
  `+9` names the battler it came from.
- **Bad Dreams** lists the opponents (`| 0x100`) into a stack area and damages
  each sleeping one (`10856a88(…, 2)`) by kind-`0x6` records (message `0x1e4`),
  showing the popup once around them.
- **Slow Start** uses work words `+4` (turns), `+8` (finished) and `+0xc`.
  Under five turns it needs a queued action from `1084b784` whose low nibble
  is not 3 before counting; on the fifth it commits message `0x1f3` and writes
  `+8` = 1, `+0xc` = 0.
- **Healer** lists its side (`10873158 | 0x400`) into a stack area and skips
  itself.

## Validation

`make -C reconstructed/battle_abilities_3 test` covers Moody over all 128 ×
128 raisable/lowerable sets and three rolls against a model of its pair
table; Harvest over item, berry, held item, weather and chance; Pickup over
three opponents' flags and items, the held item, rolls and the lock; Bad
Dreams over sleeping opponents; Slow Start over eight turn counts and every
action kind; Shed Skin and Healer over statuses and chances; Levitate; and
Speed Boost then Levitate through the real dispatcher (51,259 cases).
ASan/UBSan pass; 34 mutations are each detected by a failing test (one
equivalent mutation and three that did not compile were replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the nine handlers are bound to exactly nine (ability, event) pairs and
that the only other ability entry of events `0x80` and `0x7f` is Hydration
([record](reproduction/record-comparison.json)).

## Limits

The chance, random, stage, flag, item, buffer, side, popup, message and
action services remain external. The field handlers of event `0x7f` are not
reconstructed yet. No running-game comparison has been made. Static evidence
and host comparisons do not establish a matching original build.
