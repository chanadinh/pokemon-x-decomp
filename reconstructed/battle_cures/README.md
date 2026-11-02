# Battle cure routines: the last kind routines

This forty-second host-source milestone reconstructs the last five
`DllBattle.cro` routines that the
[action processor](../battle_process/README.md) hands record kinds to. All
were reviewed through live REA MCP/Ghidra listings. With
[battle_kinds](../battle_kinds/README.md),
[battle_effects](../battle_effects/README.md),
[battle_outcomes](../battle_outcomes/README.md),
[battle_changes](../battle_changes/README.md) and
[battle_restores](../battle_restores/README.md), **all 27 kind routines are
now reconstructed**: every record the processor hands over reaches
reconstructed code.

| Routine | Address (static) | Kind | Effect |
| --- | --- | --- | --- |
| `px_battle_kind_cure` | `10820904` | `0xa` | Cures each listed live target (not one `108007cc` excludes) through `108695c0`, then the record's message or one built by `10882188` with the stack word's low bits (cleared after). Condition `0x13` triggers the item event `0x9c` |
| `px_battle_kind_set_item` | `1081f36c` | `0x21` | Sets a held item. Event `0xa6` can block it when another battler acts (event `0xa7` follows). Message, optional `+0x14` clears (command `0x2e`), `1086b140`, event `0x9b` when an item is removed, item event `0x9c` |
| `px_battle_kind_swap_items` | `10820c9c` | `0x25` | Swaps the items of two battlers under the same `0xa6`/`0xa7` gate: three messages, `1086b140` both ways, then item events |
| `px_battle_kind_use_move` | `10825eb8` | `0x28` | Runs a move (`10869c5c` into a 20-byte frame, then four phases). Fainted or unpositioned targets are dropped from the target list (`+0x1268`). With none left it shows the generic message. The field struct (`+0xdb4`) is saved, set for the move, and restored around the final phase `108225dc`, whose `+5` bit 0 is the result |
| `px_battle_kind_cure_side` | `10824414` | `0x36` | For each live battler of the header battler's side: clears flag 3 of `+0x169` (commands `0x1c`, `0x38`, marking its entry, `1086844c` for a pending mark), runs `108651d8` and the cures `0x1e`/`0x20`, and shows message `0x441` when anything changed |

Runtime addresses are static − `0x1015b000`.

## Kind `0xa`'s cure codes

Per target, the loop re-reads the current status (`1086d8ac`) after every
cure:

| Code | Cures |
| --- | --- |
| `0x26` | The current status |
| `0x27` | The current status, then condition 6 |
| `0x28` | Every active condition from the six-entry list (literal `10820c98` → `108d146a`), repeated until none is left; a zero ends the list |
| Below `0x26` | That condition, if active |

## Host interface

`px_battle_cures_api` adds two things to the processor API:

- **The cure list's logical address**, which stays original data.
- **A scratch area for the original frame locals:**
  - kind `0xa`'s out word at `+0`;
  - kind `0x36`'s id buffers at `+0x10`;
  - kind `0x28`'s move frame at `+0x20`.

Kind `0x28`'s field save uses VFP `vldmia`/`vstmia` of three words; the host
copies the same 12 bytes.

## Validation

`make -C reconstructed/battle_cures test` covers 313 cases:

- **Kind `0xa`:** every cure code with and without status and conditions,
  popup and both message paths, two targets with an exclusion, the list's
  end and zero, and the bounds.
- **Kind `0x21`:** 128 combinations of blocking, own record, popup, removal,
  clears and event `0x9b`.
- **Kind `0x25`:** 16 combinations.
- **Kind `0x28`:** 32 combinations checking the target list, the phases,
  the field the final phase sees, the result bit and the restore, plus
  emptied lists.
- **Kind `0x36`:** 64 combinations checking the marks, the cures and the
  messages.
- **End to end:** kinds `0xa` (with the low bits) and `0x21` committed
  through `battle_action` and `battle_process`.

ASan and UBSan pass, and each of 33 mutations is detected. Three
equivalent mutants were replaced.

[`reproduction/check_cures.py`](reproduction/check_cures.py) confirms from
the original module that:

- the processor's jump table hands the five kinds to these routines (`0xa`
  with the low bits);
- the original record sizes match the fields read;
- the cure list holds six distinct valid conditions;
- the six kind-routine milestones together cover exactly the processor's
  27 routines ([record](reproduction/record-comparison.json)).

## Limits

The callees named above (the cure, item, move-phase and command routines)
remain external. No running-game comparison has been made. Static
evidence and host comparisons do not establish a matching original build.
