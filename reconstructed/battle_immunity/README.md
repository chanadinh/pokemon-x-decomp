# Battle immunity and absorb handlers

This thirtieth host-source milestone reconstructs every `DllBattle.cro`
handler of event `0x31`, which decides whether a move fails against its
target: fourteen abilities, one item and two moves, with the shared absorb
test. A handler blocks the move by rewriting key `0x41` to 1. Twelve
handlers and the absorb test were reviewed through live REA MCP/Ghidra
listings; five have no Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder).

| Address (static) | Host routine | Table entry | Blocks | Then |
| --- | --- | --- | --- | --- |
| `108c1320` | `px_battle_water_absorb` | Water Absorb (ability 11) | Water | Heal ¼ |
| `108c2db4` | `px_battle_volt_absorb` | Volt Absorb (ability 10) | Electric | Heal ¼ |
| `108c3170` | `px_battle_dry_skin_absorb` | Dry Skin (ability 87) | Water | Heal ¼ |
| `108c485c` | `px_battle_motor_drive` | Motor Drive (ability 78) | Electric | Stat 5 (Speed) +1 |
| `108c54dc` | `px_battle_lightning_rod` | Lightning Rod (ability 31) | Electric | Stat 3 (Sp. Atk) +1 |
| `108c5698` | `px_battle_storm_drain` | Storm Drain (ability 114) | Water | Stat 3 (Sp. Atk) +1 |
| `108c53cc` | `px_battle_flash_fire` | Flash Fire (ability 18) | Fire | Activates (state bit 13) |
| `108c1d5c` | `px_battle_wonder_guard` | Wonder Guard (ability 25) | Damaging moves that are not super effective, except Struggle | Message |
| `108bc750` | `px_battle_soundproof` | Soundproof (ability 43) | Sound moves (flag 8) | Message |
| `108c3888` | `px_battle_oblivious` | Oblivious (ability 12) | Captivate, Taunt | Message |
| `108c213c` | `px_battle_sticky_hold` | Sticky Hold (ability 60) | Trick, Switcheroo | Message |
| `108be0c8` | `px_battle_telepathy` | Telepathy (ability 140) | An ally's damaging moves | Message `0x1d5` |
| `108c47b0` | `px_battle_overcoat` | Overcoat (ability 142) | Powder moves | Message |
| `108c3244` | `px_battle_magic_bounce` | Magic Bounce (ability 156) | Reflectable moves (flag 4) | Reflects |
| `108aba84` | `px_battle_safety_goggles` | Safety Goggles (item 650) | Powder moves | Message `0x4ae` |
| `108b7570` | `px_battle_magic_coat` | Magic Coat (move 277) | Reflectable moves | Reflects |
| `108bb7dc` | `px_battle_telekinesis` | Telekinesis (move 477) | Diglett, Dugtrio, Mega Gengar, two statuses | — |

`10865824` (`px_battle_absorbs`) is the absorb test the six absorbers and
Flash Fire share. Runtime addresses are static − `0x1015b000`; ability, item,
move and species numbers are the original ids, named after the public
Generation VI lists.

## Details

- **The absorb test** requires this battler as the defender, not as the
  attacker, and the full type word. For a move of kind 2 (`108007d4`) with
  stat changes (`108014bc`, `108014c4`), it absorbs only if one change is
  negative, so a move that only raises stats passes through. It then
  rewrites key `0x41`; a key already rewritten refuses, and nothing follows.
- **Messages** use an action record (`10869f7c(context, 3, id)`), message
  `0xd2` (or the literal ids above) with the battler id as argument
  (`1084d2d4`, `1084d29c`), and `10867b48(context, record)`. Wonder Guard,
  Soundproof, Oblivious, Overcoat and Flash Fire show the ability popup
  around it (`1084c948` phases 1 and 2); Sticky Hold and Telepathy set
  record flag `0x800000` instead. Safety Goggles adds the item and the move.
- **Flash Fire** reports when state bit 13 is already set; otherwise it shows
  message `0x1ab` and commits a second record (kind `0x17`, bytes `+4 = 0xd`,
  `+5 = id`) that sets the bit.
- **Wonder Guard** looks the battler up without using the result, and tests
  the low byte of key `0x39` against 7.
- **Magic Coat and Magic Bounce** need key `0x4f` zero and `1084b804` to
  allow the battler, then call `1084b3f8(context, id, attacker)` and rewrite
  key `0x53`.
- **Telekinesis** is the attacker's handler and tests the target's species
  (`+0xc`): 50, 51, or 94 when `108c8540` reports the Mega form; and statuses
  `0x1f` and `0x15` (`10856a88`).

The [damage-modifier milestone](../battle_modifier/README.md) was corrected
here: `10867b48` takes the action record as its second argument.

## Validation

`make -C reconstructed/battle_immunity test` covers the absorb test over the
three sides, every type, four move kinds and seven stat-change patterns, and
its call order; all six absorbers; Flash Fire inactive and active with both
records; Wonder Guard over codes, Struggle and status moves; Soundproof,
Overcoat, Oblivious and Sticky Hold over 512 moves; Telepathy; Safety
Goggles' message; both reflectors over all conditions; Telekinesis over
species, Mega form and statuses; and two absorbers through the real
dispatcher, where only the first blocks (3,673 cases). ASan/UBSan pass; 23
mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that each handler is bound to exactly its row, only for event `0x31`, and
that these are all of the event's entries (14 ability, 1 item, 2 move)
([record](reproduction/record-comparison.json)).

## Limits

Healing, stat changes, messages, popups, reflection, move data, ally,
status and form services remain external, as do these abilities' other
entries. No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
