# Battle status-prevention handlers

This thirty-second host-source milestone reconstructs every `DllBattle.cro`
handler of events `0x6e` (block a status condition) and `0x70` (the blocked
status message): twelve abilities and the move Uproar, 17 handlers. Fourteen
were reviewed through live REA MCP/Ghidra listings; three have no Ghidra
function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder).

| Event | Address (static) | Host routine | Table entry | Blocks |
| --- | --- | --- | --- | --- |
| `0x6e` | `108c2a58` | `px_battle_limber` | Limber (ability 7) | Paralysis (1) |
| `0x6e` | `108c1cb8` | `px_battle_insomnia` | Insomnia (15), Vital Spirit (72) | Sleep (2), drowsiness (`0xe`) |
| `0x6e` | `108c556c` | `px_battle_magma_armor` | Magma Armor (40) | Freeze (3) |
| `0x6e` | `108c4b58` | `px_battle_water_veil` | Water Veil (41) | Burn (4) |
| `0x6e` | `108c2afc` | `px_battle_immunity_status` | Immunity (17) | Poison (5) |
| `0x6e` | `108c2b88` | `px_battle_own_tempo` | Own Tempo (20) | Confusion (6) |
| `0x6e` | `108bce68` | `px_battle_oblivious_status` | Oblivious (12) | Attraction (7), `0xb` |
| `0x6e` | `108bf864` | `px_battle_leaf_guard` | Leaf Guard (102) | In sun: major statuses, drowsiness |
| `0x6e` | `108c4618` | `px_battle_sweet_veil` | Sweet Veil (175) | Sleep, drowsiness on allies |
| `0x6e` | `108c4db8` | `px_battle_flower_veil_status` | Flower Veil (166) | Codes below 6 and drowsiness on Grass-type allies |
| `0x6e` | `108c333c` | `px_battle_aroma_veil` | Aroma Veil (165) | Statuses accepted by `108a4764` on allies |
| `0x6e` | `108bae4c` | `px_battle_uproar` | Uproar (move 253) | Sleep while the uproar lasts |
| `0x70` | `108c35f8` | `px_battle_status_blocked_message` | the nine single-target abilities | Message `0xd2` |
| `0x70` | `108c4fa4`, `108c3b00`, `108c51c8`, `108c5798` | `px_battle_*_message` | Own Tempo, Aroma, Flower and Sweet Veil | Messages `0x165`, `0x49f`, `0x49c`, `0x4a2` |

Runtime addresses are static − `0x1015b000`. Ability, move and status
numbers are the original ids; names follow the public Generation VI lists
and are analyst interpretations.

## Details

- Keys: `0x03` the cause, `0x04` the target (compared in full with the
  battler or with the low byte the handler read), `0x1e` the status narrowed
  to 16 bits, `0x42` blocked.
- **The blockers** store the result of rewriting key `0x42` in their first
  work word, or 0 when the status is not theirs; the two-status abilities try
  the second code only when the first did not block. Leaf Guard, Flower Veil
  and Aroma Veil leave the work word alone unless they block, and the Veils
  return early for non-allies.
- **The messages** (event `0x70`) run when the work word is 1 (for the
  abilities that guard themselves) or nonzero (for the Veils, which also
  require the target to be an ally and name it). They show the ability popup
  around the message (`1084c948`, `10869f7c`, `1084d2d4`, `1084d29c`,
  `10867b48`) and then clear the work word.
- **Aroma Veil** also rewrites key `0x44` when it blocks.
- **Uproar** reads work word 6, compares key `0x1e` in full with 2, and
  reports message `0x2c8` when the target is its own battler, `0x2c5`
  otherwise.

## Validation

`make -C reconstructed/battle_status test` covers the seven own-target
abilities over targets, 18 codes and wide values, and an already blocked
status; Leaf Guard over weather and codes; the three Veils over targets,
allies, Grass type, causes and codes, including Aroma Veil's key `0x44`;
Uproar over its work word, targets and codes; all five messages over work
values and allies; and Limber's block and message through the real
dispatcher on events `0x6e` and `0x70` (2,328 cases). ASan/UBSan pass; 19
mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 17 handlers are bound to exactly 25 (row, event) pairs and that
these are all of the two events' entries
([record](reproduction/record-comparison.json)).

## Limits

The ally, type, weather, status-class, popup, message and action services
remain external, as does the routine that raises these events. No
running-game comparison has been made. Static evidence and host comparisons
do not establish a matching original build.
