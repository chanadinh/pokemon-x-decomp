# Battle redirect, absorb and blocking abilities

This milestone reconstructs the fifth batch of the remaining
`DllBattle.cro` ability handlers: 15 handlers that draw moves to their
battler, absorb them, or stop a move or status from taking effect. Thirteen
were reviewed through live REA MCP/Ghidra listings; two have no Ghidra
function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder). The
shared absorb test `10865824` and the stat-raise, reflection, move-kind,
move-flag, ally, popup, message and action services come from
[battle_immunity](../battle_immunity/README.md), whose callback struct this
batch takes.

| Event | Address (static) | Host routine | Abilities | Effect |
| --- | --- | --- | --- | --- |
| `0x2b` | `108be440` | `px_battle_lightning_rod_redirect` | Lightning Rod (31) | Draws Electric moves (key `0x04`); work word = target changed |
| `0x2b` | `108bebf8` | `px_battle_storm_drain_redirect` | Storm Drain (114) | Draws Water moves |
| `0x24` | `108c5270` | `px_battle_redirect_message` | Lightning Rod, Storm Drain | Announces the draw once (message `0x1c0`) if it is a target |
| `0x30` | `108c5600` | `px_battle_sap_sipper` | Sap Sipper (157) | Absorbs Grass moves: Attack +1 |
| `0x93` | `108c20d4` | `px_battle_flash_fire_leave` | Flash Fire (18) | Clears its Flash Fire state bit (13) |
| `0x40` | `108c33b0` | `px_battle_flash_fire_power` | Flash Fire (18) | While active: Fire moves ×1.5 |
| `0x2e` | `108bcdbc` | `px_battle_bulletproof` | Bulletproof (171) | Blocks ball moves (`108a46c4`) with "it doesn't affect" |
| `0x1f` | `108bea04` | `px_battle_damp_check` | Damp (6) | Explosion and Self-Destruct fail (reason `0x13`) |
| `0x21` | `108c4408` | `px_battle_damp_message` | Damp (6) | Names the user and the move (message `0x38f`) |
| `0x1f` | `108c4eb4` | `px_battle_magic_bounce_reflect` | Magic Bounce (156) | Reflects reflectable status moves (reason `0x19`) |
| `0x09` | `108c4a88` | `px_battle_magic_bounce_message` | Magic Bounce (156) | Marks the reflection (key `0x53`) and announces it (`0x2fc`) |
| `0x0e` | `108c3d90` | `px_battle_sweet_veil` | Sweet Veil (175) | It and its allies: key `0x42` = 1 |
| `0x0e` | `108c4170` | `px_battle_insomnia_block` | Insomnia (15), Vital Spirit (72) | Itself: key `0x42` = 1 |
| `0x0e` | `108c573c` | `px_battle_flower_veil` | Flower Veil (166) | Grass-type allies: key `0x42` = 1 |
| `0x97` | `108bf18c` | `px_battle_liquid_ooze` | Liquid Ooze (64) | The drain (key `0x21`) is cancelled and dealt to the attacker |

Runtime addresses are static − `0x1015b000`; ability and move numbers are
the original ids, named after the public Generation VI lists.

## Details

- **Redirection** compares the attacker's and the previous target's low
  bytes with the battler and the whole type word with 12 or 10. It is
  skipped when `1084b67c(context, attacker, id, move)` blocks it, for
  fixed-target moves (`1084c720`) and for Sky Drop (507). The work word
  becomes 1 only when the rewrite of key `0x04` succeeds and the target was
  another battler; the announcing handler looks for the battler among key
  `0x05` targets from key `0x06` on and always clears the word.
- **Sap Sipper** calls the shared absorb test with type 11 and then
  `10867a88(context, id, 1, 1)`.
- **Flash Fire** reads state bit 13 through `1084ca4c`; leaving commits a
  kind-`0x18` record (`+4` bit 13, `+5` the battler).
- **Damp** keeps two work words (`+0` the failure was set, `+4` the move); it
  needs key `0x23`'s low byte clear. Its message names key `0x02`'s low byte.
- **Magic Bounce** stops when `1084af30` reports so or the user is its ally,
  then needs move kind 11 (`108007d4`) and move flag 4 and a successful rewrite
  of key `0x23`, and reflects through `1084b3f8(context, id, user)`. The
  announcing handler re-reads key `0x02` inside the popup and checks
  `1084b804`; its last argument is the whole key `0x12` word.
- **Liquid Ooze** rewrites key `0x21` to 0 before testing its low half; the
  kind-`0x6` record (`+4` HP, `+6` the attacker) is flagged `0x800000`.

## Validation

`make -C reconstructed/battle_abilities_5 test` covers redirection over the
battler keys (including high-bit ids), four type words, Sky Drop, both
blocking services and the previous target; the announcement over 0–4
targets; Sap Sipper over the absorb test's branches; Flash Fire, Bulletproof,
Damp and Magic Bounce over every check, including keys already rewritten;
the veils and Insomnia; Liquid Ooze over drain words; and Lightning Rod then
its announcement through the real dispatcher (970 cases). ASan/UBSan pass; 35
mutations are each detected by a failing test (one equivalent mutation and
two that did not compile were replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 15 handlers are bound to exactly 17 (ability, event) pairs and that
the only other ability entry of events `0x2b`, `0x24` and `0x0e` is Leaf
Guard from battle_abilities_2 ([record](reproduction/record-comparison.json)).

## Limits

The redirection, target, ball-move, bounce, type and state-bit services, and
the services taken from battle_immunity, remain external. No running-game
comparison has been made. Static evidence and host comparisons do not
establish a matching original build.
