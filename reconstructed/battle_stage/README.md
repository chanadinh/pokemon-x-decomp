# Battle stat-stage handlers

This thirty-first host-source milestone reconstructs every `DllBattle.cro`
handler of the four stat-stage events: `0x63` (the size of a change), `0x64`
(block a drop), `0x65` (the blocked-drop message) and `0x66` (after a drop).
They belong to twelve abilities. Eight were reviewed through live REA
MCP/Ghidra listings; six have no Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder).

| Event | Address (static) | Host routine | Ability | Effect |
| --- | --- | --- | --- | --- |
| `0x63` | `108bd42c` | `px_battle_simple` | Simple (86) | Stages doubled |
| `0x63` | `108bef24` | `px_battle_contrary` | Contrary (126) | Stages negated; result in the work word |
| `0x64` | `108c2740` | `px_battle_clear_body` | Clear Body (29), White Smoke (73) | Any drop by another battler blocked |
| `0x64` | `108c489c` | `px_battle_hyper_cutter` | Hyper Cutter (52) | Attack (stat 1) drops blocked |
| `0x64` | `108c2bdc` | `px_battle_keen_eye` | Keen Eye (51) | Accuracy (stat 6) drops blocked |
| `0x64` | `108c1e40` | `px_battle_big_pecks` | Big Pecks (145) | Defense (stat 2) drops blocked |
| `0x64` | `108c30a4` | `px_battle_flower_veil` | Flower Veil (166) | Drops on a Grass-type ally blocked |
| `0x65` | `108c2790`, `108c48fc`, `108c2c3c`, `108c1ea0`, `108c3134` | `px_battle_*_message` | the same five | Message `0xc6`, `0xc9`, `0xcf`, `0xcc`, `0x49c` |
| `0x66` | `108bdab0` | `px_battle_defiant` | Defiant (128) | After a drop by a non-ally: Attack +2 |
| `0x66` | `108bd070` | `px_battle_competitive` | Competitive (172) | After a drop by a non-ally: Sp. Atk +2 |

Runtime addresses are static − `0x1015b000`. Ability numbers are the original
table ids, named after the public Generation VI list.

## Details

- Keys: `0x02` is the battler whose stats change, `0x03` the one causing it,
  `0x20` the stat, `0x21` the signed stages and `0x42` the blocked flag.
  Every handler except Flower Veil's message requires key `0x02` to be its
  battler.
- **Simple** rewrites key `0x21` to its 32-bit double, **Contrary** to its
  negation (and stores the rewrite result in its work word). As key `0x21`
  is a scaled entry, both apply in turn.
- **The blockers** require another battler as the cause and negative stages,
  optionally for one stat, then store the result of rewriting key `0x42` in
  their work word.
- **Flower Veil** protects the battler named by the low byte of key `0x02`
  when `1084c5a8(id, battler)` reports an ally (or itself) and
  `1086f960(battler, 11)` the Grass type; key `0x02` must equal that low byte
  in full.
- **The messages** call `1084a9d4(context, battler, id, work, message)`;
  Flower Veil passes the protected battler.
- **Defiant and Competitive** need a cause that `1084c5a8` does not report as
  an ally and negative stages, then commit an action record of kind `0xd`:
  word `+0` gains `0x800000`, byte `+4` the stat (1 or 3), `+0x17 = 2`,
  `+0x18` gains bit 1, `+0x16 = 1` and `+0xc` the battler.

## Validation

`make -C reconstructed/battle_stage test` checks Simple and Contrary over
−6 to +6 and both together; every blocker over the changed battler, the
cause, eight stats and five stage values, and an already blocked drop; Flower
Veil over targets, causes, allies, Grass type, stages and a wide key; all
five messages; Defiant and Competitive over targets, causes, allies and
stages, with the record bytes and call order; and Simple then Contrary
through the real dispatcher (1,811 cases). ASan/UBSan pass; 15 mutations are
each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 14 handlers are bound to exactly 16 (ability, event) pairs and that
these are all of the four events' entries
([record](reproduction/record-comparison.json)).

## Limits

The ally, type, message, action-record and commit services remain external,
as do the stat-change routine that raises these events and the moves bound
to the neighbouring events `0x62` and `0x67`. No running-game comparison has
been made. Static evidence and host comparisons do not establish a matching
original build.
