# Battle move handlers: events 0x54–0x6d

This milestone reconstructs 13 `DllBattle.cro` move handlers bound to 28
(move, event) pairs: every move entry of events `0x54`, `0x55`, `0x58`,
`0x5b`, `0x67`, `0x69`, `0x6a`, `0x6b` and `0x6d`. Twelve handlers were
reviewed through live REA MCP/Ghidra listings and one, Tri Attack's, which
has no Ghidra function, through the bytes read live and listed by the
[battle_power decoder](../battle_power/reproduction/decode_arm.py) (session
`outputs/rea-battle-10` and earlier).

| Event | Address (static) | Moves | Effect |
| --- | --- | --- | --- |
| `0x54` | `108b4a24` | Incinerate (510) | Unless key `0x47` is set, a defender's held item (+0x12) that `108007a4` or `10801cfc` accepts is destroyed: record `0x21`, message `0x45a` (literal `108b4ae8`) |
| | `108babcc` | Destiny Bond (194) | Once the user has fainted: if the move can be disabled, the attacker is not an ally and has HP left, the attacker faints too (record `0x12`), with the user's message `0x275` (literal `108bacf8`); the handler removes itself |
| `0x55` | `108b4ef8` | Rapid Spin (229) | Frees the user of statuses `0x12` and `8` (record `0xa`) and lifts side conditions 6, 7, 8 and 14 (record `0x1a`) |
| `0x58` | `108b9de8` | Struggle (165) | Key `0x53` set |
| `0x5b` | `108b3e78` | Pursuit (228) | When a pursuit is possible and the target is not an ally and the redirect is not blocked, `1081d748` carries it out |
| `0x67` | `108b1368` | move 475 | A streak above 1 restores its excess over 1000 (at least 1; literal `108b13fc` = 1001): record `0x2e`, message `0x454` (literal `108b1400`) |
| `0x69` | `108b5574` | move 477 | Key `0x1e` becomes `0x20`; message `0x47a` (literal `108b55d0`) in the message area (key `0x40`) |
| `0x6a` | `108b5fac` | moves 20, 35, 83, 128, 250, 328, 463, 611 | With key `0x1e` = 8, the move's effect text (literals `108b608c`–`108b60a0` and two immediates) names the target and the user |
| `0x6b` | `108b09d0`, `108b3ad8` | moves 193 and 316; move 357 | The effect value (key `0x1f`) is scaled with parameter 7 or `0x10` (`1084b990`) |
| | `108bb084` | move 73 | The effect value becomes the one of the user's opposing selector (`1084c3b0`) |
| `0x6d` | `108b0980` | the same eight moves | The effect value is scaled with the handler's own move id |
| | `108b30fc` | Tri Attack (161) | One of three effect kinds (table `108d2c34`: 1, 4, 3) is picked at random: kind to key `0x1e`, its value (`10860e00`) to key `0x1f`, and key `0x27` is `0x14` |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list where known.

## Details

- **Tri Attack's table** is read through the relocated literal `108b3160`
  (an ABS32 relocation, checked); the three bytes are retained as a data
  block. The random range is 3, masked to a byte before the lookup.
- **Rapid Spin's mask** is the size-4 side-condition mask of
  [battle_moves_13](../battle_moves_13/README.md): condition i sets bit
  i % 8 of byte 1 + i / 8 while that byte is within the size.
- **Destiny Bond** removes itself on every path once its user has fainted
  and the move check passed or failed; it only keeps itself when the user is
  not out of HP or the event is for another defender.
- **Scaling** updates a word in place (`1084b990`); the host passes one
  logical word of scratch memory and rewrites key `0x1f` from it.

## Validation

`make -C reconstructed/battle_moves_17 test` runs the handlers on the real
event stack and handler registry: no action for another attacker or defender;
Incinerate over the flag and both item tests; Destiny Bond over every check
and its removal; Rapid Spin over the four status combinations and the mask;
Pursuit over every check; the streak thresholds; the message and effect
texts for every move and kind; the scaling moves; the selector value; Tri
Attack over each pick; and Tri Attack with Struggle through the real
dispatcher (110 cases). ASan/UBSan pass; 69 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 13 handlers are bound to exactly 28 (move, event) pairs and that
every move entry of the nine events is one of them
([record](reproduction/record-comparison.json)).

## Limits

The keys of these events are taken as the tests give them (ids for `0x03`,
`0x04` and `0x06`, values otherwise). The record, message, item, status,
side, pursuit, streak, scaling, selector, random and type services remain
external. No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
