# Battle event system

This twenty-fourth host-source milestone reconstructs the `DllBattle.cro`
event system that every other battle milestone calls: the event variable
stack, the dispatch wrapper and the handler walker. All nine routines were
reviewed through live REA MCP/Ghidra and ARM instructions.

| Address (static) | Host routine | Behavior |
| --- | --- | --- |
| `1086bb84` | `px_battle_event_push` | Open a frame (marker key 1) |
| `1086b880` | `px_battle_event_pop` | Clear the frame and return to the previous one |
| `1086bac0` | `px_battle_event_set_id` | Add an entry of kind 4 |
| `1086b9fc` | `px_battle_event_set_value` | Add an entry of kind 1 |
| `1086b520` | `px_battle_event_set_scaled` | Add an entry of kind 0 |
| `10867c9c` | `px_battle_event_set_multiplier` | Add an entry of kind 3 with a minimum and maximum |
| `1086b91c` | `px_battle_event_get` | Read an entry of the current frame |
| `1086b9a4` | `px_battle_event_dispatch` | Count the dispatch depth and run the handlers |
| `10864e98` | `px_battle_event_run_handlers` | Walk the handler list and call every match |

Runtime addresses are static − `0x1015b000`. Names are analyst
interpretations. The stack (`10b07350`) and the free handler array
(`10b1196c`) are in the module's uninitialized segment and the battle state
object (`108fca00`, the same object whose byte `+1` selects an Inverse
Battle) is in its data segment; all three are reached through relocated
literals, so the host takes their addresses from the caller.

## Variable stack

The stack holds 96 entries. Word `+0` is the position (the first entry of
the current frame); keys are 16-bit at `+4 + 2i` (0 empty, 1 frame marker),
values at `+0xc4 + 4i`, maxima at `+0x244 + 4i`, minima at `+0x3c4 + 4i` and
kind bytes at `+0x544 + i`.

- **Push** skips the current frame's entries (storing each step), and at a
  position of `0x5f` or more calls the overflow helper `108003a4` (a veneer
  to `0011e864`) instead. Otherwise it writes the marker and moves past it.
- **Pop** at position 0 calls the abort helper. Otherwise it clears the
  frame's entries (its scan index is narrowed to 16 bits), clears the slot
  before the position (the marker) and walks down to the previous marker,
  stopping just after it or at 0.
- **Set** aborts for keys 0 and 1 but still writes them. It writes the
  first empty slot of the frame; a duplicate key aborts and is overwritten,
  and a full stack aborts and writes slot 0. Ids, values and scaled values
  clear the minimum and maximum; multipliers store them.
- **Get** aborts for keys 0 and 1, then scans the frame: a matching key
  returns its value, an empty slot or the end aborts and returns 0. Key 0
  matches the first empty slot before the empty test, so it returns 0
  without a second abort.

The abort helper `10800394(0, 0, 0)` returns, as in the other battle
milestones.

## Handlers

The battle state keeps the handler list at `+0x60`, the free handler count
at `+0x64` and the dispatch depth at `+0x68`. A handler (0x3c bytes) links
to its previous and next handler, points to entries of 8 bytes (event byte,
function word), and has an owner byte `+0x10`, flags `+0x18`, a work area
`+0x1c` and a value byte `+0x3a`. The flags hold a registration depth (low
16 bits), the entry count (bits 16–23) and state bits: running
(`0x1000000`), disabled (`0x2000000`, `0x40000000`), accepts event `0x7c`
(`0x4000000`), removal requested (`0x8000000`), re-entry (`0x10000000`),
registered (`0x20000000`) and a visit mark (`0x80000000`).

**Dispatch** raises the depth, runs the handlers for any owner (7) with the
blocking check on, lowers the depth, and on leaving the outermost dispatch
clears every handler's registration depth.

**The walk** marks each handler's successor before looking at the handler. A
handler runs when it is not running (or allows re-entry), is not disabled,
matches the owner, was registered at a depth below the current one (or at 0),
is registered, and accepts event `0x7c` if that is the event. Its first entry
for the event is used: the blocking helper `108c59b4(context, handler, event)`
may stop it; otherwise the running bit is set, the function is called with
the handler, the context, the value byte and the work area, and afterwards
the re-entry bit is cleared if set, else the running bit.

A handler that asked for its removal is unlinked, cleared and pushed onto the
free array (the count is lowered first). One that is still running (a nested
call) only keeps the request until the outer call finishes. With no free
slot the abort helper runs and the handler stays unlinked but uncleared.
Before that, an unregistered successor is replaced by the handler's current
link; the original never advances this search, so an unregistered handler
found there loops forever, and the reconstruction does the same.

The walk then continues with the successor if it is still registered and
marked (clearing the mark), and otherwise with the handler's current link.

## Validation

`make -C reconstructed/battle_event test` checks the exact layout of a frame
with every entry kind; 400 random sequences of pushes, pops, sets and gets
against an independent frame model with the full memory layout after every
step; frame visibility; reserved keys, duplicates, a full stack, overflow
bounds and both pop endings; 2,592 flag, event, owner and depth combinations
for one handler against an independent eligibility rule; the entry search,
entry counts up to 255 and the blocking check; call order, arguments and the
visit mark; relinking; removal of the head, middle and tail, without a free
slot, of an unregistered handler and from a nested call; and dispatch depth
counting and clearing (3,994 cases). ASan/UBSan pass; 29 mutations are each
detected.

## Limits

Handler functions, the blocking helper, the overflow and abort helpers, and
the code that registers handlers remain external. The endless search is
reproduced, not tested. No running-game comparison of these routines has
been made. Static evidence and host comparisons do not establish a matching
original build.
