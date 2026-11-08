# Battle move handlers: the check before a move

This milestone reconstructs the 20 `DllBattle.cro` move handlers of event
`0x1f`, which runs just before a move is executed, bound to 29 (move, event)
pairs, and the shared Counter check `1084b2d8`. Every move entry of the
event is now reconstructed (Sky Drop's included). A handler writes a failure
code to key `0x23`: `0x1b` makes the move fail. Sixteen were reviewed
through live REA MCP/Ghidra listings (session `outputs/rea-battle-7`);
Counter, Mirror Coat, Metal Burst and Fake Out have no Ghidra function and
are listed by the [battle_power decoder](../battle_power/README.md#decoder).

| Address (static) | Moves | Fails (`0x1b`) when |
| --- | --- | --- |
| `1084b2c4`, `108b9af4`, `108ba3b8` | Counter (68), Mirror Coat (243), Metal Burst (368) | No damage record from a non-ally of the category (physical, special, any); shared check `1084b2d8` |
| `108b10e8` | Last Resort (387) | It is not known, fewer than two moves are known, or another move is unused |
| `108b73c8` | Bide (117) | From turn 2: no stored damage or no attacker; then work word 0 = 3 |
| `108b7d30` | Protect (182), Detect (197), Endure (203), King's Shield (588), Spiky Shield (596) | The streak's `random(1, 3, 9, … 729)` is nonzero, or the battler moves last |
| `108b7f2c` | Rest (156) | Asleep (`0x1b`), heal-blocked (`0xd`), full HP (`0xe`), Insomnia or Vital Spirit (`0xf`); only when no code is set |
| `108b8c38`, `108b9cdc` | Spit Up (255), Swallow (256); Stockpile (254) | Nothing stockpiled; three stockpiled |
| `108b8cc4` | Snore (173), Sleep Talk (214) | Awake; a sleep failure (code 2) is cleared |
| `108b9080` | Teleport (100) | Escape is forbidden or the battle format is not 0; or trapped (statuses 8, `0x16`) or under Fairy Lock unless escape is allowed |
| `108b94ec` | Sky Drop (507) | The carried battler (counter 4 − 1) is gone |
| `108b98e8` | Magic Coat (277) | Its own use: moving last. Another battler's reflectable status move at it: bounced (code `0x19`, `1084b3f8`) |
| `108b9e78` | Wide Guard (469), Quick Guard (501), Crafty Shield (578) | Moving last; the protect streak then resets |
| `108ba4a0` | Fling (374) | No item, a fling power of 0, an unusable item, or one its holder cannot lose |
| `108ba850` | Magnet Rise (393) | Statuses `0x15` or `0x1f` |
| `108bb3cc`, `108b3c6c` | Mat Block (561), Fake Out (252) | Not the battler's first turn (state bit 0); Mat Block also when moving last |
| `108bb76c` | Natural Gift (363) | The item cannot be used, or there is no item or gift power |
| `108bbb4c` | Follow Me (266), Rage Powder (476) | In battle format 0 or 3 when no code is set |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list.

## Details

- Most handlers require key `0x02` (the battler about to move) to be theirs.
  Teleport, the guards and Follow Me test other conditions first; Magic Coat
  acts both for its own battler and for others (key `0x02` narrowed to a byte).
- **Counter** (`1084b2c4`) moves its arguments, sets kind 1 and falls into
  `1084b2d8(context, id, work, kind)`, which Mirror Coat (kind 2) and Metal
  Burst (kind 0) reach by branch. It walks the battler's damage records
  (index wrapping at 256), skips allies of its own id (`+0x19`) and other
  categories, and fails when the records run out.
- **Last Resort** compares each known move (`10855c68`) with literal 387 and
  counts the others marked used (`108c8714`), with byte-wide counters.
- **Protect** reads its streak (counter 3), caps the table index at 6 and
  rolls `1084c7bc` over table `108d4b38` (literal `108b7db4`, relocated).
- **Magic Coat** for another battler checks, in order: not an ally,
  `1084af30` zero, the mover not an ally of it, move target class `0xb`
  (`108007d4`) and move flag 4 (`1080049c`); the bounce runs only when the
  code rewrite takes.
- **The guards** commit a kind-`0x27` record (battler, counter 3, value 0)
  when the failure rewrite takes.

## Validation

`make -C reconstructed/battle_moves_10 test` runs the handlers on the real
event stack: no action for another battler; the Counter family over record
and category patterns, call order and the index wrap; Last Resort over move
lists and use masks; Bide's words; the Protect streak over every index,
rolls and moving last; Rest over earlier codes, statuses, HP and abilities;
the stockpile counters; Snore over codes; Teleport over all its conditions;
Sky Drop over carried ids; Magic Coat for itself and 64 bounce patterns;
the guards; Fling, Magnet Rise, Mat Block, Fake Out and Natural Gift over
their conditions; Follow Me over formats; and Fake Out and Spit Up through
the real dispatcher (2,208 cases). ASan/UBSan pass; 40 mutations are each
detected (one equivalent mutation of the streak cap was replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 20 handlers are bound to exactly 29 (move, event) pairs and that
every move entry of event `0x1f` is one of them
([record](reproduction/record-comparison.json)).

## Limits

Keys `0x12` and `0x23` are taken as value entries, as the tests give them.
The damage-record, ally, move-list, counter, random, order, status, HP,
escape, format, field, presence, type, move-data, bounce, item and record
services remain external. No running-game comparison has been made. Static
evidence and host comparisons do not establish a matching original build.
