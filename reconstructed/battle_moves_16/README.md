# Battle move handlers: events 0x2a–0x53

This milestone reconstructs 19 `DllBattle.cro` move handlers bound to 24
(move, event) pairs: every move entry of events `0x2a`, `0x2b`, `0x35`,
`0x36`, `0x39`, `0x3a`, `0x42`, `0x48` and `0x53`. Thirteen handlers were
reviewed through live REA MCP/Ghidra listings and six, which have no Ghidra
function, through the bytes read live and listed by the
[battle_power decoder](../battle_power/reproduction/decode_arm.py) (sessions
`outputs/rea-battle-8` and `-9`).

| Event | Address (static) | Moves | Effect |
| --- | --- | --- | --- |
| `0x2a` | `1084b458`, `108b8404`, `108b8f78` | Counter (68), Mirror Coat (243), Metal Burst (368) | One shared routine at `1084b46c` (the three are thunks passing category 1, 2 and 0): the first hit on the user from a non-ally (of that category) names the target (key `0x04`); a target that is out of reach is replaced by the hit's stored selector, then the move's chosen target, then the hitter |
| | `108b5848` | Bide (117) | Released (work word 0 at least 2, signed): the first damage kind with a record names the target, picked at random among the side's battlers when out of reach (`0x1f` when none); work word 2 keeps it |
| `0x2b` | `108b7a20` | Sky Drop (507) | The carried battler (counter 4 − 1) becomes the target |
| | `108bb270` | Follow Me (266), Rage Powder (476) | The user draws the move aimed at another battler unless a list of conditions holds (ally, move 507, status `0x21`, a switch pending, the Rage Powder case against Grass, the ability `0x8e` or item `0x28a`, a blocked redirect) |
| `0x35` | `108b6ff8` | Present (217) | A 20% chance sets key `0x53` (once, in work word 0); the result raises the effect flag (`1084bef8`) |
| `0x36` | `108b51d0` | Present | A full-HP defender gets message `0xd2`; otherwise record 4 heals a quarter, message `0x183` (literal `108b52b8`) |
| `0x39` | `108b8d3c` | Thunder (87), Hurricane (542) | Weather 1 sets key `0x2c` to `0x32` |
| `0x3a` | `108b56f0` | Beat Up (251) | Counts the party members that can attack (up to six rounds, each skipping one more qualifying member): key `0x2b`, at least 1; work word 0 clears |
| | `108ba7f8` | Triple Kick (167) | Key `0x43` set |
| `0x42` | `108bbdf4` | Heal Bell (215), Aromatherapy (312) | Key `0x53` set |
| `0x48` | `108ba004` | Grass, Fire, Water Pledge (518–520) | Key `0x53` set when work word 0 is set |
| `0x53` | `108b1404` | Clear Smog (499) | Record `0xf` and message `0xc3` naming the defender, unless key `0x47` is set |
| | `108b4db8` | Rage (99) | A hit on the user with HP left raises its Attack (record `0xd`), message `0x214` |
| | `108b57f8` | Bide | Key `0x33` is added to work word 1 while Bide's flag (word 6) is set |
| | `108b69f4` | Flame Burst (481) | Each other battler of the target's side takes a sixteenth (record 6), message `0x457` (literal `108b6aec`) |
| | `108b847c` | Grudge (288) | When the user faints from a move that may be disabled, the hitter's move used loses all its PP (record 9), message `0x27b` (literal `108b858c`); the handler removes itself |
| | `108ba3cc` | Fling (374) | The held item is used up (record `0x24`; work word 0 set); with an item effect (parameter 5) and keys `0x47`/`0x48` clear, record `0x23` |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list.

## Details

- **The shared routine** `1084b46c` is entered at its start by the three
  thunks (`1084b458`, `108b8404`, `108b8f78`: each moves the arguments and
  loads r3 = 1, 2 or 0). The verifier retains the thunk and the shared body
  with the first; the other two thunks are five instructions each.
- **Hits** are read from a 16-byte stack buffer the damage-record service
  fills (move halfword at +0, target at +5, selector at +6); Bide's buffer
  overlaps its side-battler list, the host uses one scratch region for both.
- **Beat Up** mirrors a quirk exactly: its outer round index doubles as the
  number of qualifying members to skip, and `r5` is reset every round.
- **Pay attention to signs:** Bide's release test is a signed comparison, and
  Grudge stores minus the PP as a byte.

## Validation

`make -C reconstructed/battle_moves_16 test` runs the handlers on the real
event stack and handler registry: no action for another attacker or
defender; the three reflecting moves over no hit, allies, wrong categories
and every selector fallback; Bide over its threshold, damage kinds and random
pick; Sky Drop over modes and carried values; Follow Me over every refusal;
Present, weather, Beat Up over every party size and member pattern, the flag
moves; Clear Smog, Rage, Bide's counter, Flame Burst, Grudge, Fling; and two
handlers through the real dispatcher (570 cases). ASan/UBSan pass; 76
mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 19 handlers are bound to exactly 24 (move, event) pairs and that
every move entry of the nine events is one of them
([record](reproduction/record-comparison.json)).

## Limits

The keys of these events are taken as the tests give them (ids for `0x02`,
`0x03` and `0x04` where compared, values where rewritten). The record,
message, damage-record, target, selector, party, status and item services
remain external. No running-game comparison has been made. Static evidence
and host comparisons do not establish a matching original build.
