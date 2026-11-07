# Battle message routines

This forty-fourth host-source milestone reconstructs the three
`DllBattle.cro` routines that queue battle messages. These are the
`1086b5e0` and `1086bbe8` calls every reconstructed routine makes. They
write to the same queue as the [command writer](../battle_command/README.md),
through its writers. All three were reviewed through live REA MCP/Ghidra
listings.

| Function | Address (static) | Role |
| --- | --- | --- |
| `px_battle_message_command` | `1086b5e0` | Writes a message command: group and message id as big-endian u16s, then 32-bit words up to and including the terminator `0xffff0000`; group `0x6b` writes its first word ahead of that list |
| `px_battle_message_plain` | `10867bbc` | Group `0x69` with up to four argument words (other counts send none) |
| `px_battle_show_message` | `1086bbe8` | Shows a message struct. Kind 1 → group `0x69` (through `10867bbc`), kind 2 → `0x6a`; with flag bit 14 the extra word at `+0x20` comes first, in groups `0x6b`/`0x6c`. Up to four arguments, then the terminator. Other kinds return 0 |

Runtime addresses are static − `0x1015b000`.

## Details

- **Message groups:** the four groups `0x69`–`0x6c` are the commands
  outside the command writer's info table (with command 0).
- **The terminator** is the literal `0xffff0000` (`1086be74`, `10867c98`).
  Every word is checked once against the queue's u32 limit `0x176d`
  (literal `1086b6ac`).
- **Group `0x6b`'s first word** is written before the list, so an extra
  word equal to the terminator does not end it; in group `0x6c` it does.
- **Arguments:** the original passes them variadically. `1086bbe8` and
  `10867bbc` copy argument words with VFP `vldmia`/`vstmia`; the host copies
  the same words.

## Validation

`make -C reconstructed/battle_message test` covers 80 cases:

- **The command:** terminators, group `0x6b`'s first word, the byte group
  and the u16 id.
- **Overflow:** each write's overflow edge.
- **Plain messages:** every argument count.
- **Shown messages:** every kind, count and flag, the kind mask, and the
  terminator as an extra word.

ASan and UBSan pass, and each of 15 mutations is detected. One equivalent
mutant was replaced.

[`reproduction/check_messages.py`](reproduction/check_messages.py)
confirms from the original module that the commands outside the encoder
are exactly command 0 and the four message groups. It also confirms that
all 8 message calls in the reconstructed battle routines use group `0x69`
or `0x6a` and end with the terminator
([record](reproduction/record-comparison.json)).

## Limits

The display side that reads the queue remains external. No running-game
comparison has been made. Static evidence and host comparisons do not
establish a matching original build.
