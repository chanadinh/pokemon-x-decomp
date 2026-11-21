# Battle move handlers: event 0xac, last group (Heal Bell and Aromatherapy)

This milestone finishes the move table: the two handlers left over from the
upper half of the last 21 (`battle_moves_b6`), Heal Bell (move 215, `108b87e0`)
and Aromatherapy (move 312, `108b5620`). Together with `battle_moves_19`, every
one of the 386 move-table handlers is covered by some `evidence/functions.json`
(checked by [`check_bindings.py`](reproduction/check_bindings.py)).

Both handlers share one body, the Ghidra function `108b87e4` (424 bytes,
live REA listing in session `outputs/rea-battle-10`): Heal Bell's entry is a nop
word falling into it, Aromatherapy's a single branch to it. The two entries
have no Ghidra function and are listed by the decoder copy described below.

| Address (static) | Host routine | Move | Effect |
| --- | --- | --- | --- |
| `108b87e0` | `px_battle_heal_bell_effect` | Heal Bell (215) | The shared body |
| `108b5620` | `px_battle_aromatherapy_effect` | Aromatherapy (312) | The shared body |

The body (`party_cure` in the host code) walks one or two battler lists (the
user's side, `1084b950`, and a second one, `1082001c`, when it exists; a list
holds its size at `+0x18` and `1085daf8(list, index)` gives a member whose
battler id is the byte at `+0x19`). A member is cured when its state
(`1084cb78`) is `0xa` or when it is one of the move's targets (keys `0x06`..,
count in key `0x05`), and it exists and has a status (`1086d8ac`). The cured
ids are collected, one byte each, in the handler's work area and committed in a
kind-`0xa` record (`+4` `0x26`, `+5`.. the ids, `+0x11` their count); with none,
key `0x53` is rewritten to 1.

## The decoder copy

The body contains a post-indexed byte load, word `e4d03001` at `108b8950`, which
the battle_power decoder refuses. REA decodes it itself: `inspect_native_instruction`
(session `outputs/rea-battle-10`, evidence id `ev_94f8b8dadf4c041a62956d322f0d2bf7798f1325c47cc00d66d32113f7e1e10d`, retained in
`evidence/rea-evidence.json`) gives `ldrb r3,[r0],#0x1`, and the Ghidra listing of
`108b87e4` spells it `ldrb r3, [r0],#0x1`.

[`reproduction/decode_arm.py`](reproduction/decode_arm.py) is a **copy** of
`reconstructed/battle_power/reproduction/decode_arm.py` (SHA-256
`7b8658ce88254bf87eece1a767521fba45b277301e56f1984eca5ed22624fcd2`), which is left
untouched. The copy adds exactly one form, the post-indexed immediate byte load
`ldrb<cond> rd, [rn],#imm` (add, no write-back flag, `rn` not `pc`); the unified
diff is [`reproduction/decode_arm.diff`](reproduction/decode_arm.diff): one
four-line branch before the original's "unsupported indexing" refusal.
`verify_rea.py` imports the copy and also checks the retained inspect record
(status decoded, bytes equal to the original word, REA's spelling equal to the
decoder's up to spaces).

[`reproduction/check_decoder.py`](reproduction/check_decoder.py) shows the
extension changes nothing else: both decoders give identical results (or refusals) on
every instruction word of every evidence listing under `reconstructed/*/evidence`
(all committed `battle_moves_b*/` and `battle_moves_N/` milestones included);
fourteen listed words (this milestone's own included) are of the extended form, which the original refuses and the
copy spells as the listings do; and on 4,000,000 pseudo-random words plus the
form's neighbourhood they agree except on the extended form itself.

## Validation

`make -C reconstructed/battle_moves_b7 test` covers both entries over the user
key, one or two lists, member states, targets, statuses and a missing pokemon,
and the dispatcher (1945 cases). ASan/UBSan pass; 27 mutations are each detected
by a failing test.

## Limits

The battler-list, state and status services remain external. No running-game
comparison has been made. Static evidence and host comparisons do not establish
a matching original build.
