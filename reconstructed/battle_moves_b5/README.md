# Battle move handlers: event 0xac, fourth group

This milestone reconstructs 14 more `DllBattle.cro` move handlers of event
`0xac` (the main effect of a status move), bound to 16 (move, event) pairs. It
is the fifth batch taken from the top of the move table downward (the
battle_moves_N milestones work upward). None of the 14 has a Ghidra function in
the live REA project: their bytes were read live (session
`outputs/rea-battle-10`) and listed by the
[battle_power decoder](../battle_power/reproduction/decode_arm.py). Every
handler acts only when key `0x03` is the user; key `0x05` is the target count
and keys `0x06`.. the targets.

| Address (static) | Host routine | Moves | Effect |
| --- | --- | --- | --- |
| `108ad640` | `px_battle_gastro_acid_effect` | Gastro Acid (380) | Condition `0x10` on the target, message `0x235` |
| `108adf14` | `px_battle_celebrate_effect` | Celebrate (606) | Key `0x53` rewritten, kind `0x3c`, message `0x10b` naming the owner |
| `108ae754` | `px_battle_powder_effect` | Powder (600) | Condition `0x25` (1 turn) on every target, message `0x4ba` |
| `108ae800` | `px_battle_splash_effect` | Splash (150) | Message `0x6a` |
| `108b000c` | `px_battle_aqua_ring_effect` | Aqua Ring (392) | Condition `0x23` on the user unless it has it, message `0x259` |
| `108b00a0` | `px_battle_taunt_effect` | Taunt (269) | Condition `0xb` for 3 turns (4 when the target has moved) |
| `108b0848` | `px_battle_trap_effect` | Spider Web (169), Mean Look (212), Block (335) | Condition `0x16` on each target neither trapped nor Ghost |
| `108b1a6c` | `px_battle_trick_or_treat_effect` | Trick-or-Treat (567) | The target gains Ghost (kind `0x14`) |
| `108b43f4` | `px_battle_simple_beam_effect` | Simple Beam (493) | Ability `0x56` on each target without ability `0x36` |
| `108b4c1c` | `px_battle_flower_shield_effect` | Flower Shield (579) | Defense +1 for every Grass-type target |
| `108b60a4` | `px_battle_entrainment_effect` | Entrainment (494) | The user's ability on each target without ability `0x36` |
| `108b619c` | `px_battle_worry_seed_effect` | Worry Seed (388) | Ability `0xf` for the first target |
| `108b6224` | `px_battle_after_you_effect` | After You (495) | Kind `0x30` for the first target, message `0x474` |
| `108b7e50` | `px_battle_destiny_bond_effect` | Destiny Bond (194) | Message `0x272`, work `+0x18` = 1 |

Runtime addresses are static − `0x1015b000`; move numbers are the original
ids, named after the public Generation VI lists. The turn-count word
(`1084be3c`) is `battle_moves_5`'s, linked rather than repeated.

## Details

- Records are created by `10869f7c` and committed by `10867b48`; messages use
  `1084d2d4` / `1084d29c`. Powder, Simple Beam, Entrainment and Flower Shield
  read target `i` at key `i + 6`; the trap handler reads key `6` on every
  iteration.
- The ability handlers store the target's low byte at `+6` and a halfword at
  `+4`, then name both in the message.

## Validation

`make -C reconstructed/battle_moves_b5 test` covers every handler over the
user key (own, other, high-bit id), the target counts, the services' answers,
every record's unwritten bytes and the dispatcher (439 cases). ASan/UBSan pass;
42 mutations are each detected by a failing test (one equivalent mutation was
replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 14 handlers are bound to exactly 16 (move, event) pairs
([record](reproduction/record-comparison.json)).

## Limits

The status, type, owner, ability and message services remain external. No
running-game comparison has been made. Static evidence and host comparisons do
not establish a matching original build.
