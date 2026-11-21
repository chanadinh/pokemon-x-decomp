# Battle low-address helpers, batch 1 (10802000-10840000)

This milestone starts the low band of `DllBattle.cro`, `10802000`-`10840000`
(import thunks below `10802000` skipped), which holds 569 procedures in REA's
`list_procedures` of the carrier. The 15 functions here were chosen from the
frontier, the uncovered procedures that committed milestones already call
(28 in the band): the plain, self-contained ones. The rest of the frontier is
larger or uses jump tables, vector code or import thunks and is left for later
batches. All 15 have a live REA/Ghidra listing (session
`outputs/rea-battle-10`). The names are analyst interpretations. Runtime
addresses are static − `0x1015b000`.

## Functions

| Address | Function | Behavior |
| --- | --- | --- |
| `10814f9c` | `px_battle_is_mode_2_or_3` | A value is 2 or 3 |
| `1081cc04` | `px_battle_context_0ca4` | `context + 0xca4` |
| `1081cf08` | `px_battle_half_4674` | The halfword at `+0x4674` |
| `1081d83c` | `px_battle_next_counter` | Steps the counter at `+0x1084`; a wrap to 0 is stored as 1 |
| `1081d748` | `px_battle_push_byte` | Appends a byte to the list at `+0x1094` (length byte `+0x1091`, at most 10) |
| `1081ca98` | `px_battle_add_capped` | For a state byte of 0 or 1, adds to the word at `+0x804` of the second-level object, capped at 99,999 unless flag 8 of `+0x81c` is set |
| `10810cc0` | `px_battle_add_unique` | Adds a byte to the list at `+0x189c` (length `+0x18a6`) unless present; aborts first when the list already has 6 |
| `1081e28c` | `px_battle_slot_state` | `(index + 2) & 3` when that slot is present, else 4 |
| `1082001c` | `px_battle_partner_entry` | The 28-byte entry (from `+4` of the table at context `+8`) of the owner's partner slot, 0 when none |
| `1081cf14` | `px_battle_record_0x37` | Forwards its arguments to the record writer with kind `0x37` |
| `1081d2b0` | `px_battle_count_better` | Counts entries of other battlers whose score beats an entry's score |
| `1081d770` | `px_battle_event_0xb_allows` | A Ghost (type 7) always passes, a battler without HP never; otherwise event `0xb` runs with key `0x02` and the result is key `0x53` |
| `108261e8` | `px_battle_chance_event` | Event `0xb5` with keys 3 and 4 and a scaled chance in key `0x27`; true when a roll below 100 is under the final chance |
| `1082b064` | `px_battle_set_move_slot` | Sets one of two move slots (move, PP from the move data capped at a limit, current PP), committing the first slot when flagged |
| `1081fc20` | `px_battle_clear_bit_flag` | Clears a bit of a battler's array (`+0x169`), records two events (kinds `0x1c`, `0x38`) and a queued change when the battler's word `+0x84` has its low 3 bits set, and marks the battler's context entry unless quiet |

## Evidence

All 1,552 instruction bytes and 16 literal bytes match live REA bytes and the
original `DllBattle.cro`. The four literals are the list offset `0x189c`, the
cap `99999`, the entry-count offset `0x108c` and an ABS32 relocation
(`10826260`, `segment_2` + 18136, the random state object `108fca00`) whose
original word and value are checked. REA lists the abort call
(`mov r2,#0; cpy r1,r2; cpy r0,r2; bl 10800394`) as one `FAULT` line; the
evidence lists the four instructions, and
[`verify_rea.py`](reproduction/verify_rea.py) checks them by bytes and with the
[battle_power decoder](../battle_power/reproduction/decode_arm.py).

## Validation

`make -C reconstructed/battle_low_1 test` runs 3,013 cases: every pure value
over ranges and wrap points, every list length and value of the byte lists,
every state, flag, base and amount of the capped adder, slot masks, owners and
partner lookups, random entry lists against a reference count, the event
builders through the real dispatcher (including a missing battler), every
slot, move, limit and PP of the move slot setter and 600 random bit-flag
cases against a byte-for-byte reference memory. ASan/UBSan pass; 52 mutations
are each detected by a failing test (one that did not compile was replaced and
four survivors led to wider tests).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that each function is a plain procedure of the band in REA's list, is called
from another milestone and is listed by none; 46 of the band's 569 procedures
are covered after this batch.

## Limits

The slot, owner, entry-iteration, record, random and move-data services remain
external. No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
