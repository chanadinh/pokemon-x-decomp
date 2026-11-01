# Battle restore routines

This forty-first host-source milestone reconstructs four more
`DllBattle.cro` routines that the
[action processor](../battle_process/README.md) hands record kinds to. All
were reviewed through live REA MCP/Ghidra listings. With
[battle_kinds](../battle_kinds/README.md),
[battle_effects](../battle_effects/README.md),
[battle_outcomes](../battle_outcomes/README.md) and
[battle_changes](../battle_changes/README.md), 22 of the 27 kind routines
are now reconstructed.

| Routine | Address (static) | Kind | Effect |
| --- | --- | --- | --- |
| `px_battle_kind_restore` | `10821948` | 4 | Restores HP to a live, not-full target (not one `108007cc` excludes). Unless forced, a heal block (`+0x58` bits) shows message `0x37a`, sets context `+0x1093` bit 5 and fails. Otherwise: command `0x57` (message `0x29e`) for a positioned battler, `10865788`, then the record's message or message `0x392` with the stack word's low bits |
| `px_battle_kind_pp` | `10821b80` | 8 | Restores PP of one move slot, in the first pair (`+2`/`+3`) or the second (`+8`/`+9`) of its 14-byte entry. It caps at the maximum, mirrors into the other pair when linked (`+0xc`), and sends command 9 or 8. It refuses a full pair, and fainted targets unless flag bit 1 |
| `px_battle_kind_item` | `1081db1c` | `0x23` | Uses a held item (`108abf28`): queues `10863e4c` and runs event `0x7c` (scaled keys 2, 3) inline without the blocked check. If records it ran succeeded, reports through `108659b8` and `1086b098`. Then `108a5bd4`, and optionally marks `+0x16c` with command `0x1f` |
| `px_battle_kind_transform` | `1081defc` | `0x34` | Transforms into the target (`1082a720`, `108ae5e0`, `108679ac`, `108678f8`, command `0x5f`, message), wrapped by the popup. Refused if either battler is locked (`+0x1b` bit 6) or `10874348` ≠ `0x12`. A changed ability (key `0x11` against `0x10`) runs event `0x94` and `108677e4` |

Runtime addresses are static − `0x1015b000`.

## Details

- **Kind `0x23`** reads the action-stack word after its nested event and
  before restoring it. The item counts as used only when records ran during
  the event (bit 30) and one succeeded (bit 29). Its inline dispatch uses
  the battle state through the relocated literal `1081dd14` (→ `108fca00`);
  literal `1081dd18` is the word's offset `0x437c`.
- **Kind 4's literals** are `0xffff0000`, `0x29e`, `0x37a` and `0x392`
  (`10821b70`–`10821b7c`).

## Validation

`make -C reconstructed/battle_restores test` covers 869 cases:

- **Kind 4:** 512 combinations of exclusion, HP, heal block, force,
  position, popup, message and low bits.
- **Kind 8:** 256 combinations of exclusion, HP, empty and full pairs, link,
  amount, pair and override, plus a case where only the second pair is full,
  the slot bound and 8-bit wrap.
- **Kind `0x23`:** the event, the success bits, marking and restoring the
  stack word, with the blocked callback never consulted and registration
  depths cleared.
- **Kind `0x34`:** the locks, the gates and the ability change.
- **End to end:** kinds 4 and 8, both receiving the stack word's low bits,
  committed through `battle_action` and `battle_process`.

ASan and UBSan pass, and each of 25 mutations is detected.

[`reproduction/check_restores.py`](reproduction/check_restores.py)
confirms from the original module that the processor's jump table hands the
four kinds to these routines (4 and 8 with the low bits). It also confirms
that the original record sizes match the fields read
([record](reproduction/record-comparison.json)).

## Limits

The callees named above, the command writer and the other five kind
routines remain external. No running-game comparison has been made.
Static evidence and host comparisons do not establish a matching original
build.
