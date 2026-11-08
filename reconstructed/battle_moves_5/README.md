# Battle move handlers: field and side conditions

This milestone reconstructs the 27 `DllBattle.cro` move handlers of the field
event `0xaa` and the side event `0xad`, each bound to one move, so every move
entry of both events is now reconstructed. It also reconstructs the shared
side-condition setter `1084bcb0` and the turn-count encoder `1084be3c`.
Twenty-three routines were reviewed through live REA MCP/Ghidra listings
(session `outputs/rea-battle-7`); six handlers have no Ghidra function and
are listed by [`reproduction/decode_store.py`](reproduction/decode_store.py),
the [battle_moves_3 decoder](../battle_moves_3/README.md#decoder-extension)
plus one store form.

| Event | Address (static) | Move | Record |
| --- | --- | --- | --- |
| `0xaa` | `108af744` | Gravity (356) | Field 2, 5 turns, halfwords `+0xc` = `0x7b`, `+0xe` low bits 1; effect `0x36`; not while active |
| | `108b14a8`, `108b2940` | Mud Sport (300), Water Sport (346) | Field 5 / 4, 5 turns, message `0x78` / `0x76` |
| | `108b1700` | Fairy Lock (587) | Field 9, 2 turns, message `0x102` |
| | `108b6294` | Ion Deluge (569) | Field 8, 1 turn, message `0x101` |
| | `108b3164` | Trick Room (433) | Field 1, 5 turns, message `0x35c` naming the user; ends it when active |
| | `108b4988`, `108b2738` | Wonder Room (472), Magic Room (478) | Field 6 / 7, 5 turns, message `0xb8` / `0xba`; ends it when active |
| | `108b3684`, `108b28bc`, `108b15c8` | Grassy (580), Misty (581), Electric Terrain (604) | Field 10, terrain 1 / 2 / 3, 5 turns, message `0xcd` / `0xcf` / `0xd1`; not over the same terrain |
| | `108b2630` | Haze (114) | Kind `0xf` listing the battlers of the opposing selector with `0x800`, then message `0x69` |
| `0xad` | `108b2d6c`, `108b59f0`, `108b70bc`, `108b2e34`, `108b0db4` | Reflect (115), Light Screen (113), Safeguard (219), Mist (54), Lucky Chant (381) | Own side, condition 0 / 1 / 2 / 3 / 5, 5 turns |
| | `108aed68` | Tailwind (366) | Own side, condition 4, 4 turns |
| | `108b4784` | Crafty Shield (578) | Own side, condition 16, 1 turn |
| | `108b176c`, `108b320c` | Quick Guard (501), Wide Guard (469) | Own side, condition 10 / 9, 1 turn; then a kind-`0x27` record raising counter 3 |
| | `108b6300` | Mat Block (561) | Own side, condition 15, 1 turn, message `0x4a5` naming the user |
| | `108b092c`, `108b0140`, `108b5440`, `108b5074` | Spikes (191), Toxic Spikes (390), Stealth Rock (446), Sticky Web (564) | Opposing side, condition 6 / 7 / 8 / 14, count 1 |
| | `108b1ae0` | Happy Hour (603) | `1084c3cc(context, id, 1)`, message `0xff` |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list. The record layouts are in
[`battle_moves_5.h`](battle_moves_5.h).

## Details

- Every handler requires key `0x03` (attacker) to be its battler; the side
  conditions test it inside `1084bcb0`, after the handler has already asked
  for the side (`1084bfc0` own, `1084bab4` opposing). The side handlers'
  messages (`1084bcb0`) name the side; Mat Block builds the same record
  itself with a message of kind 2 naming its battler.
- **`1084bcb0(handler, context, id, work, side, condition, count,
  message)`** takes its last four arguments on the stack (Ghidra's
  pseudocode drops them). It copies the count word through a VFP register
  (`vldr.32`/`vstr.32`) and narrows the message id to 16 bits.
- **`1084be3c(turns)`** returns `((turns << 3) & 0x1f8) | 2` with the bits of
  literal `0x01fffe00` cleared (none are set). Hazards pass a plain 1.
- **Quick and Wide Guard** add, after the side condition and without a key
  check, a kind-`0x27` record: battler, counter 3, `1084bb2c(battler, 3) + 1`
  and flag `0x1000000`.
- **Gravity** reads halfword `+0xe` after building the turn count and keeps
  its bits above the low seven.

## Decoder extension

`decode_store.py` adds `str rd, [rn,#±imm]!` (STR word, immediate,
pre-indexed with writeback), which Mat Block uses to store its count. The
REA analyses that list this form (five functions, six instructions) are
retained as references with their words read live, and the verifier
requires the decoder to reproduce all six as well as every supported
instruction of the REA-listed routines here. The VFP instructions of
`1084bcb0` are refused by the decoder and checked as bytes.

## Validation

`make -C reconstructed/battle_moves_5 test` runs the handlers on the real
event stack: the turn count for every byte and wide values; every field
condition's record and message; Gravity's guard and halfwords; both states
of every room and Trick Room; every terrain against every current terrain;
Haze with 0–3 listed battlers; all ten plain side conditions for their own
and another battler; the setter's narrowing and unused arguments; Quick and
Wide Guard for both battlers; Mat Block; Happy Hour; no action for another
battler; and Reflect and Spikes through the real dispatcher (356 cases).
ASan/UBSan pass; 35 mutations are each detected (one equivalent mutation of
the turn-count mask was replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 27 handlers are bound to exactly 27 (move, event) pairs and that
every move entry of events `0xaa` and `0xad` is one of them
([record](reproduction/record-comparison.json)).

## Limits

The record, commit, message, field, terrain, effect, side, selector, Happy
Hour and counter services remain external. No running-game comparison has
been made. Static evidence and host comparisons do not establish a matching
original build.
