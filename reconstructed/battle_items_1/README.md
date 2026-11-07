# Battle item handlers, batch 1: cure and HP berries

This milestone reconstructs the first batch of the `DllBattle.cro` item
handlers: the seven status-curing berries and the three HP-restoring items
(Berry Juice, Oran and Sitrus Berry), 19 handlers bound through the item
table `108d382c`, with the seven shared routines they call. Twelve
routines were reviewed through live REA MCP/Ghidra listings; fourteen
two-instruction handlers have no Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder).

| Group | Address (static) | Items | Effect |
| --- | --- | --- | --- |
| Cure checks | `108a6b88`, `108a624c`, `108a678c`, `108a64b4`, `108a6be4`, `1084cc40`, `108a5e70` → `1084cc48` | Cheri 149, Chesto 150, Pecha 151, Rawst 152, Aspear 153, Persim 156, Lum 157 | Events `0x5e`, `0x9c`, `0x02`: ask for the berry when the battler has paralysis (1), sleep (2), poison (5), burn (4), freeze (3), confusion (6), or a status or confusion (`0x27`) |
| Cure uses | `108a86a8`, `108a77dc`, `108a80a0`, `108a7d48`, `108a88d4`, `1084c4b8`, `108a733c` → `1084c4c0` | the same | Events `0x7b`, `0x7c`: a kind-`0xa` cure record |
| HP checks | `108aa1a8` (`0x9c`), `108aa15c` (`0x5e`), `108aac30` (`0x61`) | Berry Juice 43, Oran 155, Sitrus 158 | Ask for the item at half HP or less, unless the battler has condition `0xf` |
| HP uses | `108a7a08`, `108a790c` | Berry Juice and Oran; Sitrus | A kind-4 restore of the item's amount, or that percentage of max HP, with message `0x392` |
| Shared | `1087bfa0`, `1084c96c`, `1084c844`, `1084c730`, `1084c198` | | Event-key lookup, re-entry mark, listed-condition scan, pinch test, HP-berry check |

Runtime addresses are static − `0x1015b000`; item and ability numbers are
the original ids, named after the public Generation VI lists.

## Details

- **Asking for the item.** A check that passes marks its handler for
  re-entry (`1084c96c`: flag `0x10000000`, only while the running flag
  `0x1000000` is set, so the dispatcher keeps it running) and commits a
  header-only record of kind 0 for the battler through `1084c948`.
- **The cure stubs** load the cure into r3 and branch to the shared bodies;
  Persim's two handlers fall through into them instead (`mov r3, #6;
  mov r0, r0`). Both bodies require key `0x02` to be the battler. The check
  also looks up key `0x2f` with `1087bfa0` and gives up when the frame has it
  without bit 2. Cure `0x26` tests the current status (`1086d8ac`), `0x27`
  the status and then condition 6, `0x28` the six-entry list (`1084c844`,
  literal `1084c8b4` → `108d146a`), and any other value that condition
  (`10856a88`). The use pushes kind `0xa` with `+4` the cure, `+5` the
  battler and `+0x11` count 1.
- **`1087bfa0`** walks the current frame like the event getter but returns 0
  for an absent key; keys 0 and 1 call the abort helper and are then looked
  up anyway (key 0 matches the frame's first empty slot).
- **The pinch test** `1084c730(context, id, divisor)` halves divisors above 2
  for Gluttony (82, value `0x11`), faults on a divisor of 0 (then uses 1),
  fails for a max HP of 1 or less, and compares HP with
  `1084c124` (max HP / divisor).
- **The HP checks** call `1084c198(handler, context, id, 2, flag)` with flag 1
  on event `0x9c` and 0 on `0x5e`; the flag also requires key `0x2f` bit 0.
  The event-`0x61` check does not test key `0x02`.
- **The HP uses** require key `0x02`. Oran's amount is the item's parameter 3
  (`1084cf74`, the item at handler `+0x38`) stored as a halfword; Sitrus takes
  its low byte as a percentage: `umull` with `0x51eb851f` and a shift of 37
  divides the 32-bit product by 100. The record holds `+4` the amount and
  `+6` the battler; the message names the battler and the item.

## Validation

`make -C reconstructed/battle_items_1 test` checks `1087bfa0` on real event
frames (nested frames, keys 0 and 1, a full stack, positions 0x5f–0x61,
entries after a terminator); the re-entry mark over eight flag words; the
condition list (each entry and bit, a zero terminator, faulting entries);
the pinch test over every divisor 0–9, max HP 0–13, HP 0–15 and three
abilities; the HP-berry check over its flag, key `0x2f` and battler; both cure
bodies over every cure 0–`0x1f` and `0x26`–`0x28`, six condition sets, two
statuses and three list states; key `0x2f` for the check; each berry's
handlers; the three HP checks; Oran over six amounts; Sitrus over fifteen
max HPs and ten parameters (and every seventh max HP below 200,000 at 100 %);
and Lum and an HP berry through the real dispatcher, which keeps the handler
running after a request (9,732 cases). ASan/UBSan pass; 43 mutations are each
detected (one that did not compile was replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 19 handlers are bound to exactly 49 (item, event) pairs of the item
table and nothing else, and that every entry of items 43, 149–153 and
155–158 is one of them ([record](reproduction/record-comparison.json)).

## Limits

Leppa (154), Enigma and the other berries, and the remaining item handlers
are not reconstructed yet. The status and condition queries, item
parameters, record stack, messages and commit remain external; the battler
lookup, HP fraction and event stack are the earlier milestones' code. No
running-game comparison has been made. Static evidence and host comparisons
do not establish a matching original build.
