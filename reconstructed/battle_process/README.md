# Battle action processor

This thirty-sixth host-source milestone reconstructs `10819600`, the
`DllBattle.cro` routine that the
[action services'](../battle_action/README.md) commit runs on every record.
It is 5,696 bytes, the largest single function reconstructed so far, and
was reviewed through a live REA MCP/Ghidra listing. It gates the record,
switches on its kind (a 63-entry relocated jump table) and records the
outcome in the action-stack word. The tests drive a handler-style popup and
message sequence from push to commit through both milestones.

## Gating and outcome

- **Stack word** (context + `0x437c`):
  - `0x40000000`: a record has run.
  - `0x20000000`: some record succeeded.
  - `0x10000000`: the last record succeeded.
- **Before any record runs**, "the last one succeeded" reads as true.
- **Header `0x1000000`**: the record is skipped, with the word left as it
  is, unless the last record succeeded.
- **Header `0x2000000`**: the record is skipped unless the header battler
  (bits 8–12) has a nonzero value `0xd` (HP).
- **Header `0x800000`** (the flag handlers set): the ability popup (commands
  `0x66`/`0x67`) wraps the message or form change.
- **Battler lookup**: ids from `0x18`, and empty slots of the context's
  table (`+8`, then `+0x84 + 4 id`), call the fault routine `10800394`.
- **Faulting kinds**: `0xc`, `0x3d` and kinds from `0x3f` fault, and the
  outcome stays the previous one.

## Kinds

27 kinds hand over to their own routine with the context and record. Four
of them (4, 8, `0xa`, `0xd`) also receive the stack word's low 10 bits. The
rest are handled inline:

| Kind | Effect |
| --- | --- |
| 0 | Faints the battler (`1086c06c`) if alive or forced; word `+4` bit 0 also requires missing HP |
| 1, 2 | Popup show/hide (commands `0x66`, `0x67`); kind 1 skips a battler without a position (`1086c2cc` = `0xa`) |
| 3 | Shows the message at `+4`, wrapped by the popup when bit 23 is set; message bit 15 sets context `+0x1093` bit 4 |
| 9 | `1086bf44`, then its message and event `0x57` (keys 2, `0x18`, `0x53`); a nonzero `0x53` faints |
| `0xf` | Sets the seven stat stages (`+0x104`) of every listed live battler to 6, with command `0xe` |
| `0x14` | Changes type (`+0x102` unless already one of `+0x100`/`+0x101`), with command `0x18` and message `0x386` |
| `0x15`–`0x18` | Sets or clears flag bits at `+0x166` / `+0x169` (the latter with commands `0x1b`/`0x1c`) |
| `0x19` | Event `0xab` (keys 2, `0x55`, `0x56`, scaled `0x1f`), then `108acd0c` and the message at `+0x10` |
| `0x1b` | Up to 17 field flags through `108ace70` or `108ace4c` |
| `0x1d` | Releases a side-table entry (below `0xb`) |
| `0x1f` | `108a5aac`, then command `0x34` |
| `0x22` | Event `0x9c` for an item holder, restoring the stack word from `1086b284` |
| `0x24` | Uses the held item (command `0x49`, `1086b140`, `1086b098`) |
| `0x26`, `0x27`, `0x2e` | A move slot (`1082b064`), one of five bytes at `+0x16d`, the counter at `+0x150` (0 stored as 1) |
| `0x29`, `0x2c`, `0x2f`, `0x31`, `0x32`, `0x37` | Single routines (`1086af2c`, which sets context `+0x18` to 5; `1086add4`; `1086a8f8`; `108237fc`; `1081c950`; `1081fc20`) |
| `0x30` | The first matching 16-byte entry from `+0x10bc` (count at `+0x108c`, literal `1081ac6c`) through `1086a7fc` |
| `0x35` | Clears bit 6 of `+0x1b` and word `+4`, with command `0x55` |
| `0x38`, `0x39` | `1086a6e0` (command `0x62` first if requested); command `0x62` alone |
| `0x3a` | Form change (`1086fff0`, command `0x5b`) unless locked (`+0x1b` bit 5) or unchanged |
| `0x3b`, `0x3c` | Field byte `+4`, field bit `+5` |
| `0x3e` | Command 100 for a message of at most two arguments |

The assembly corrects Ghidra's argument lists in several places:

- the variadic command routine `1086c22c` takes the battler id byte for the
  popup and type-change commands, and stack words for kinds `0x26`, `0x27`
  and `0x3e`;
- `1086b098` takes a fourth argument;
- `108ace4c` takes the same two arguments as `108ace70`;
- the main-executable thunk `108007a4` receives the held item in r0.

The event stack is the real one from the
[event milestone](../battle_event/README.md). Every other callee goes
through one `call(address, arguments)` callback.

## Validation

`make -C reconstructed/battle_process test` covers 286 cases:

- **Gating:** bits 24 and 25, the first-record rule and the boundary id
  `0x18`.
- **Routine kinds:** all 27, with both results and the low bits where they
  apply.
- **Faulting kinds:** each one keeps the previous result.
- **Inline kinds:** every one, over its branches. Kinds 9, `0x19` and `0x22`
  run with and without handlers on the real event stack.
- **End to end:** a popup–message–popup sequence pushed and committed
  through `battle_action`, and a gated record skipped after a failure.

ASan and UBSan pass, and each of 36 mutations is detected.

[`reproduction/check_cases.py`](reproduction/check_cases.py) decodes the
original jump table and the first instructions of every case. It confirms
that exactly the 27 kinds in the C routine table hand over to those
routines, with the same four taking the low bits, and that kinds `0xc` and
`0x3d` fault ([record](reproduction/record-comparison.json)).

## Limits

The 27 kind routines, the command and message routines, and the other
callees remain external. No running-game comparison has been made. Static
evidence and host comparisons do not establish a matching original build.
