# Battle task creation and request consumption

This milestone reconstructs the control flow of main functions
`0x004f37e8` (task creation), `0x003bc520` (request consumption), and
`0x003bcb44` (submission). It also incorporates the reviewed base
initializer, list registration, special-location byte setter, and
optional-key extraction. Several larger called functions remain explicit
callbacks. This is host source with synthetic-memory tests, not a running
battle engine.

Run all three milestones from the parent directory with `make test`, or
run this milestone here with `make test`. Without Make:

```sh
cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror battle_task.c test_battle_task.c ../wild_battle_setup/wild_battle_setup.c ../call_wild_battle/call_wild_battle.c -o test_battle_task
./test_battle_task
```

For MinGW on Windows use `gcc` and run `.\test_battle_task.exe`. The actual
recorded checks ran on Linux; Windows execution is not yet verified.

## Reconstructed behavior

Task creation requests `0x26c` (620) bytes using heap ID 8. It performs
another lookup of heap ID 8 for base initialization, initializes specific
fields without clearing the whole allocation, and prepends the object to
the list whose head is the word at the manager address. The task's link
is its word at offset `+4`.

The original heap shortcut at `0x0012b27c` sets r0 to 8 and falls through
into `0x0012b284`; it is not an independent eight-byte returning function.
The reconstruction retains both lookups because their results must not
be assumed identical across calls. Original vtable words `0x0058ea24`
and `0x0059911c` are stored as logical address tokens; they are not callable
host function pointers.

The consumer obtains heap ID 8, caches request bytes `+0x8c` and `+0x8d`
and the field-context pointer, then initializes a scene subobject. It
allocates and constructs a `0x1c` (28) byte auxiliary object, asks the
Field module to populate it, and invokes the inline-state constructor.
The inline state starts at task `+0x2c`; task word `+0x20` is then set
to that address. The auxiliary handle is stored at task `+0x258`.

The exact seven arguments to the state constructor are:

```text
(task + 0x2c, field, current_aux, task + 0x230,
 aux_byte_at_0x18 > 1 ? 4 : 0, selected_config, heap)
```

The last three arguments were passed on the ARM stack and are absent
from the initial Ghidra pseudocode call expression. They are included in
the host callback contract and tests.

After construction, the following effects are verified. Flag writes OR
into existing state flags; conditionally untouched bytes are preserved.
Field names remain descriptive placeholders rather than recovered names.

| Input or condition | Effect |
| --- | --- |
| Saved field-context location halfword is `0x103` or `0x11e` | Set inline-state byte `+0xb2` to 1 |
| Request byte `+0x8f` equals 1 | OR current-state flags `+0xa8` with `0x80` |
| Same byte equals 2 | OR flags with `0x4` |
| Same byte equals 3 | OR flags with `0x200` |
| Request word `+0xa4` contains bit 1 | Set task byte `+0x2a`, OR state flags with 1, decode an optional key, call the environment decoder, and copy its output byte 2 to captured-state `+0x0a` |
| Low byte of the current field-context halfword at `+0x128` is 3 or 11 | OR flags with `0x4000` |
| Same low byte is 4, 5, 6, or 7 | OR flags with `0x8000` |
| All normal returns | Store the full mode word at task `+0x1c` and set task byte `+0x28` |
| Mode bit 0 is set | Set task byte `+0x29`; otherwise leave it unchanged |

The optional-key decoder returns bits 16–23 of its input unless the
input's top byte is `0xff`, in which case it returns zero. The original
exception path here contains disabled assertion instructions and returns
normally. This differs from the active fatal diagnostic used by some
other game functions.

The submission wrapper returns the original task handle after consumption.
It does not return a consumer result or reread the manager's list head.

## Remaining callback boundaries

| Callback | Original function | Current status |
| --- | --- | --- |
| `heap_for_id` | `0x0012b284`; ID-8 shortcut `0x0012b27c` | Explicit memory-system boundary; invalid IDs enter an original fatal loop |
| `allocate` | `0x00122920` | Allocator dispatch remains an adapter |
| `init_scene` | `0x003c8ab4` | Exported and traced; its internals remain unreconstructed |
| `construct_aux` | `0x0011294c` | Initializes six child handles and count bytes; child construction remains unresolved |
| `populate_aux` | Relocated Field `0x15ce4958` | Loops over 18-byte slots and calls `0x15ce4a10`; full population logic remains unresolved |
| `select_config` | Relocated Field `0x15ce49c4` | Branch behavior traced; not integrated into this source yet |
| `init_state` | `0x001cca9c` | Allocates two further auxiliary objects and invokes copy helpers; remains unreconstructed |
| `decode_environment` | `0x0045a5c8` | Uses tables and RNG; must remain a call even though this consumer uses only one result byte |

The source retains callback order and repeated loads. The location check
uses the field-context pointer saved before scene initialization. The later
category check reloads it. The optional branch captures its destination
state before calling the decoder, while later flag writes reload task
`+0x20`. The initial request byte `+0x8c` stays cached across helper calls.
The auxiliary handle is reloaded after population and again after config
selection. Tests deliberately mutate these values at callback boundaries.

## Data lifetime findings and limits

The consumer does not perform a whole-buffer copy. The population helper
uses `buffer + 18*i` for record slots and rereads the signed count at
`buffer + 0x6c` after each iteration. Its positive-count path calls the
per-record builder at `0x15ce4a10`. The state constructor's helper
`0x0039e7e0` invokes a copy operation on six child pairs, then copies the
count byte. These are concrete leads toward understanding ownership.

Complete ownership, destruction, and retention rules are not established.
No code here promises that an arbitrary implementation of the remaining
callbacks copies every needed input or safely retains the temporary request.
The Field zero-count/config fallback paths call a different helper from
the active fatal diagnostic. Their veneer resolves to `0x0014f220`, a
single `bx lr` instruction, so those paths really do return zero. Other
callees, including the state constructor, call `0x0011e864`, which loops
forever while invoking an optional handler. Similar-looking diagnostic
calls must be resolved individually before interpreting their return paths.

## API contract and validation limits

All game addresses are logical unsigned 32-bit values. Memory callbacks
perform ordinary little-endian loads and stores; arithmetic wraps as on
ARM32. The request pointer addresses host bytes and must expose at least
`0xa8` valid bytes. Valid memory mappings, callbacks, heaps, and normal
callee returns are required. `decode_environment` must provide bytes
0–9 of its output. Unknown callbacks may update the modeled game state,
as the original functions can.

The original task factory still calls list registration after a zero
allocation result, which attempts a write at logical address 4. It has
no recoverable allocation-failure path. A synthetic test may record that
attempt, but the source does not claim such a failure is valid gameplay.
Likewise, the consumer has no graceful handling for a missing auxiliary
object. A future PC integration needs an explicit failure policy outside
the reconstructed valid-input behavior.

The reconstruction does not reproduce the ARM ABI, physical memory access
widths, timing, invalid-memory faults, or concurrency. Source names and
types are analyst choices. Tests use independent assembly-derived
expectations and synthetic state, not differential execution of the 3DS
game. The evidence directory contains raw assembly and unreviewed generated
pseudocode; only the named host source files belong to the build.

The final strict C11 build passed all 65,536 selector/category combinations,
65,536 optional-key combinations, boundary and pointer-mutation cases, and
integration with the two earlier source milestones. The same tests passed
AddressSanitizer and UndefinedBehaviorSanitizer with leak detection disabled
for the container tracing environment. All 3,364 exported instruction bytes
across 20 functions were checked against the uploaded inputs. These results
validate the bounded reconstruction and test model, not a complete battle.
See `validation.txt` for exact commands and outputs.

## Next target

Replace the population callback by reconstructing the per-record builder
`0x15ce4a10`, then follow its child-copy and destruction calls to establish
ownership. The larger state and scene constructors remain additional
dependencies before this path can launch a real battle.

All Field addresses are synthetic static-analysis addresses. Main
executable addresses preserve the original map. See provenance and
validation files for the exact bytes and checks supporting this milestone.
