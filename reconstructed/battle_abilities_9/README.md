# Battle hook abilities

This milestone reconstructs the ninth batch of the remaining
`DllBattle.cro` ability handlers: 20 handlers of abilities that act through
a hook — a relocated code address stored in their handler's word `+0xc`
when a move starts and cleared when it ends — or that set a key or work word
for the move. Fourteen were reviewed through live REA MCP/Ghidra listings;
six have no Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder). The
hook-word helpers `1084aef8` and `1084aeec` come from
[battle_moves_3](../battle_moves_3/README.md) and the owner-byte helper
`1084c700` from [battle_items_4](../battle_items_4/README.md); this batch
links them.

| Event | Address (static) | Host routine | Abilities | Effect |
| --- | --- | --- | --- | --- |
| `0x03` | `108c3cc4` | `px_battle_damp_hook` | Damp (6) | Hook `108c4494` for every move |
| `0x04` | `1084aee8` | `px_battle_damp_unhook` | Damp (6) | Clear it (falls through into `1084aeec`, battle_moves_3) |
| `0x73` | `108c1a3c` | `px_battle_damp_unhook_entry` | Damp (6) | Clear it for its battler |
| `0x03` | `108c21dc` | `px_battle_infiltrator_hook` | Infiltrator (151) | Attacker: hook `108c45d0` |
| `0x04` | `108c12a8` | `px_battle_infiltrator_unhook` | Infiltrator (151) | Clear it for its battler |
| `0x05` | `108c5640` | `px_battle_infiltrator_pass` | Infiltrator (151) | Attacker: key `0x53` = 1 |
| `0x03` | `108c3200` | `px_battle_mold_breaker_hook` | Mold Breaker (104), Turboblaze (163), Teravolt (164) | Attacker, once: hook `108c4e48`, work = 1 |
| `0x04` | `108c1fbc` | `px_battle_mold_breaker_unhook` | (same) | Its battler and work 1: clear, work = 0 |
| `0x73` | `108c2aac` | `px_battle_mold_breaker_leave` | (same) | The same when leaving |
| `0xb7` | `108c1ff8` | `px_battle_scrappy_self` | Scrappy (113) | Its own move at itself: work `+0x10` = 1 |
| `0xaf` | `108c1ebc` | `px_battle_liquid_ooze_owner` | Liquid Ooze (64) | Its battler: byte `+0x10` = 6 (`1084c700`) |
| `0x1f` | `108c2f4c` | `px_battle_klutz_fling` | Klutz (103) | Fling (363) fails (reason `0x13`); work `+8` = the rewrite |
| `0x21` | `108c26a0` | `px_battle_klutz_fling_message` | Klutz (103) | Its message (`0x38f`), work `+8` = 0 |
| `0x73` | `108c36c8` | `px_battle_klutz_leave` | Klutz (103) | Holding an item: kind-`0x22` record |
| `0x93` | `108c3724` | `px_battle_klutz_unhook` | Klutz (103) | Clear the hook for its battler |
| `0x5f`, `0x61` | `108c4c80` | `px_battle_klutz_rehook` | Klutz (103) | Once: hook `108c3750`, work = 1 |
| `0x5e` | `108c49c8` | `px_battle_unnerve_effect` | Unnerve (127) | Its battler, once: announce, hook `108c4e70` |
| `0x61`, `0x5f` | `108c5324` | `px_battle_unnerve_rehook` | Unnerve (127) | The same for any battler key |
| `0x61` | `108c503c` | `px_battle_slow_start_restart` | Slow Start (112) | Once: restart its work words and message `0x1f0` |
| `0x40` | `108c44ac` | `px_battle_slow_start_attack` | Slow Start (112) | While active: physical moves ×0.5 |

Runtime addresses are static − `0x1015b000`; ability and move numbers are
the original ids, named after the public Generation VI lists.

## Details

- **Hooks** are ABS32 literals (`108c220c`, `108c3ccc`, `108c3240`,
  `108c4a84`, `108c53c8`, `108c4ca8`) whose values the host takes from the
  caller, as battle_switchin does for Unnerve and Klutz; the verifier checks
  each original word and its S+A value. Ghidra lists `1084aef8` as a second
  body range of `108c21dc`, so those eight bytes are verified here as well.
- **Damp** hooks every move without a battler check and clears on event
  `0x04` the same way; on event `0x73` only for its battler.
- **Klutz's Fling check** always clears work `+8` first, reads key `0x02`
  a second time before rewriting key `0x23`; its message handler reads
  key `0x02` again too and names key `0x12`'s whole word.
- **Unnerve** names the opposing side (`1084af00` of `1084bfc0`) in an
  unflagged kind-1 message `0xb6` inside the popup, then sets the hook and its
  work word. **Slow Start's** restart writes work `+0` 1, `+4` 0, `+8` 0,
  `+0xc` 1 and a flagged message; its attack handler needs category 1 of key
  `0x12`'s low half.

## Validation

`make -C reconstructed/battle_abilities_9 test` covers every hook, unhook
and key handler over all 27 combinations of the three battler
keys (including a high-bit id) and three work-word values; Klutz's Fling
check over moves and key `0x23` already rewritten; both message handlers;
Klutz leaving with and without an item; both Unnerve handlers; Slow Start's
restart and attack over categories; and Mold Breaker hooked on event `0x03`
and unhooked on `0x04` through the real dispatcher (1,117 cases).
ASan/UBSan pass; 32 mutations are each detected by a failing test (one that
did not compile was replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 20 handlers are bound to exactly 28 (ability, event) pairs and that
the other ability entries of
events `0x03`, `0x04`, `0x05`, `0xb7`, `0xaf` and `0x5f` are exactly the
deferred Scrappy (`108c2ae8`) and Stance Change (`108c0e14`) handlers
([record](reproduction/record-comparison.json)).

## Limits

Scrappy's event-`0x04` handler (it ends in `bx lr`, which the decoder does not
list) and Stance Change are not reconstructed yet; nor are the hook functions
themselves. The side, category, popup, message and action services remain
external. No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
