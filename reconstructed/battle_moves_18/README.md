# Battle move handlers: events 0x7d–0x9f

This milestone reconstructs 13 `DllBattle.cro` move handlers bound to 23
(move, event) pairs: every remaining move entry of events `0x7d`, `0x80`,
`0x8a`, `0x8e`, `0x8f`, `0x92`, `0x99`, `0x9e` and `0x9f`. Ten handlers were
reviewed through live REA MCP/Ghidra listings and three, which have no Ghidra
function, through the bytes read live and listed by the
[battle_power decoder](../battle_power/reproduction/decode_arm.py) (session
`outputs/rea-battle-10`). With it every move-table entry below event `0xac` is
reconstructed.

| Event | Address (static) | Moves | Effect |
| --- | --- | --- | --- |
| `0x7d` | `108af958` | moves 206, 610 | Key `0x53` set: key `0x3b` becomes 2 |
| | `108b6e80` | move 203 | With the move's flag (work word 6) and key `0x53`: key `0x3b` becomes 1 |
| `0x80` | `108b8590` | Uproar (253) | Ending with HP left: with the lock (status `0x19`) message `0x2cb` (literal `108b86dc`); without it message `0x2ce` (literal `108b86e0`) and the move's handlers go |
| | `108b898c` | Thrash (37), Petal Dance (80), Outrage (200) | A sleeping user wakes (record `0xa`, condition `0x19`) and the move's handlers go |
| `0x8a` | `108b67ec` | moves 168, 343 | Work word 0 records whether the user holds an item |
| | `108bbbb4` | Fling (374) | Message `0x30b` (literal `108bbc60`) naming the user and its item; record `0x15` (kind `0xf`) |
| `0x8e` | `108b4e64` | move 547 | A user of form `0x288` gets record `0x3a` (flag byte +`0x153`) and message `0xde` |
| | `108b9244` | Grass, Fire, Water Pledge (518–520) | By the pledge's form (work word 1): a condition (record `0x19`, kind `0xb`–`0xd`) for 4 turns on the user's or target's side, a message, and record `0x38` (halfwords `0x27b`–`0x27d`) |
| `0x8f`, `0x92` | `108b4678` | U-turn (369), Volt Switch (521); Parting Shot (575) | When the user can be replaced and the turn's switch is claimed: record `0x2a` (message `0x302`, literal `108b477c`) and record `0x38` (halfword `0x2bf` for U-turn, `0x2c0` otherwise, literal `108b4780`) |
| `0x99` | `108b4bb8` | moves 234–236 | Weather 1: key `0x36` = `0xaac` (literal `108b4c18`); 2–4: `0x400`; otherwise `0x800` |
| | `108b7670` | Spit Up (256) | The stockpile of 2 gives `0x800`, 3 gives `0x1000`, any other `0x400` |
| `0x9e` | `108b8b84` | Sky Drop (507) | An ally as the target fails the move (key `0x42`) |
| `0x9f` | `108b9c70` | move 76 | Weather 1 sets key `0x53` |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list where known.

## Details

- **108af958** is a 15-instruction handler; REA's byte read continues into the
  next function (not part of this milestone).
- **Uproar** removes its handlers like Rage and Thrash in
  [battle_moves_14](../battle_moves_14/README.md): by the user's value byte,
  restarting the search after each removal.
- **The pledges** read the target's side (`1084bfc0`) before the form switch;
  forms 2 and 3 use it, form 1 the user's own side. The record's turn word is
  `1084be3c(4)`.
- **Switching moves** set bit `0x1000000` in the first word of record `0x38`
  after reading it (the other bits are the service's).

## Validation

`make -C reconstructed/battle_moves_18 test` runs the handlers on the real
event stack and handler registry: no action for another battler, attacker or
defender; the flag and key `0x53` gates; Uproar over its flag, HP and lock;
the sleep wake and removals; the item flag and Fling; move 547's form and
flag; the pledges over work flags, every form, targets and positions; the
switching moves over both checks and three moves; the weather and stockpile
powers; the ally target; move 76; and two handlers through the real
dispatcher (150 cases). ASan/UBSan pass; 83 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 13 handlers are bound to exactly 23 (move, event) pairs and that
every move entry of the nine events is one of them
([record](reproduction/record-comparison.json)).

## Limits

The keys of these events are taken as the tests give them. The record,
message, HP, status, ally, weather, counter, switch, owner, side, position
and turn-word services remain external. No running-game comparison has been
made. Static evidence and host comparisons do not establish a matching
original build.
