# Pokémon state initialization and refresh

This seventh host-source milestone reconstructs the four remaining constructor
state routines at `0x0013a8d8`, `0x0012b744`, `0x0012b808`, and `0x0013a7bc`,
together with 13 field getters/setters they use. It extends the six preceding
modules through adapters to their existing callback interfaces. Earlier
milestone source, evidence, and checksum manifests remain immutable checkpoints.

The implementation provides the reviewed orchestration, data accesses, and
storage transformations. Level resolution, learnset resources, move insertion,
name construction/serialization, maximum-PP calculation, and derived-stat
engines remain explicit service boundaries. Consequently these routines do not
yet construct a complete game-equivalent Pokémon or provide a playable battle.
All descriptive names are analyst choices, not recovered original symbols.

Run the synthetic host checks with a C11 compiler and Make:

```sh
make -C reconstructed/pokemon_state test
make -C reconstructed test
```

The local suite links all seven modules. It uses independent synthetic memory,
resources, and expected bytes; it does not execute the original game.

Validation on Ubuntu 24.04 under WSL2 passed all seven suites with strict C11
warnings, plus AddressSanitizer/UndefinedBehaviorSanitizer for the new suite
and integration chain. Leak detection was disabled for the tracing environment.
The Python suite ran 38 tests successfully with two optional extraction checks
skipped because PowerShell/3dstool were unavailable. Commands, output, and
environment overrides are retained in `validation.json` and `validation.txt`.
Native Windows execution and comparisons against a running game remain untested.

The new tests cover 1,536 slot-field cases, 512 extra-state combinations,
1,792 derived-state combinations, all four clear-buffer alignments with exact
store-width/order assertions, corruption fallbacks, callback mutations, and
fixed expected decoded buffers across the seven-module chain. Additional
boundary stress cases supply synthetic counts and PP results beyond observed
resource ranges to test index narrowing, unsigned arithmetic, and capture
timing; they do not establish values returned by actual game resources.

This WSL installation initially lacked a compiler and Make. Its local tools
are activated with `source tools/native-build/activate.sh` from the repository
root; that ignored setup prepends a compiler/Make directory and selected
runtime libraries. `reproduction/local-toolchain.json` records the package
versions, verified download hashes, and compiler wrapper. A standard Linux
installation with a C11 compiler and Make needs no such activation.

## Implemented routines

| Original address | Host implementation |
| --- | --- |
| `0x0013a8d8` | `px_initialize_pokemon_base_state` |
| `0x0012b744` | `px_initialize_pokemon_extra_state` |
| `0x0012b808` | `px_update_pokemon_derived_state` |
| `0x0013a7bc` | `px_refresh_pokemon_state` |

The routines reuse the sixth milestone's concrete codec, block selectors, and
existing settings/auxiliary-level setters. Derived-state acquisition and
restoration reuse the fifth milestone's state helpers; their manager-transform
callbacks must be supplied, normally with adapters to the concrete codec.
The new field accessors call that concrete codec directly.

## Field accessors

`px_read_pokemon_state_value` and `px_write_pokemon_state_value` dispatch the
following original functions. A/B denote selected logical blocks, independent
of their physical permutation. Extra offsets are relative to the auxiliary
buffer selected below.

| Enum | Operation | Original address | Access |
| --- | --- | --- | --- |
| `PX_STATE_SPECIES` | Read | `0x00152fe0` | A `+0`, unsigned halfword |
| `PX_STATE_FORM` | Read | `0x00152fa8` | A `+0x15`, unsigned byte shifted right by 3 |
| `PX_STATE_GROWTH` | Read | `0x0016b45c` | A `+8`, full word |
| `PX_STATE_MAX_HP` | Read | `0x00152f54` | Extra `+0x0a`, unsigned halfword |
| `PX_STATE_CURRENT_HP` | Write | `0x0014f930` | Extra `+8`, low halfword |
| `PX_STATE_STATUS` | Write | `0x0014fcb4` | Extra `+0`, full word |
| `PX_STATE_NAME_FLAG` | Write | `0x0014f8cc` | B word `+0x34`: `(old & 0x7fffffff) \| (value << 31)` |

`px_read_pokemon_state_slot` and `px_write_pokemon_state_slot` provide the other
six original functions. Their slot argument is a full unsigned word; values
at least 4 return zero for reads or do nothing for writes, before decoding or
accessing memory. Valid slots use these offsets:

| Enum | Original read | Original write | Access |
| --- | --- | --- | --- |
| `PX_STATE_MOVE` | `0x00153014` | `0x00168824` | B `+0x1a + 2 * slot`, unsigned/low halfword |
| `PX_STATE_PP_UPS` | `0x00152e50` | `0x001684f4` | B `+0x26 + slot`, unsigned/low byte |
| `PX_STATE_PP` | `0x00152efc` | `0x0014f9d8` | B `+0x22 + slot`, unsigned/low byte |

Each supported accessor decodes the captured manager, accesses its selected
field, then attempts encoding. Getter values survive encoding unchanged.
Block getters request selector mode zero; block setters request mode one.
The codec and fallback-block behavior are documented in
[the payload milestone](../pokemon_data/README.md).

The three auxiliary operations first test manager `+4`. A zero pointer makes
the maximum-HP getter return zero and makes the two setters return without
decoding. Otherwise, after decoding, they reload manager `+8` and test that
data header's corruption bit `0x0004`. A clear bit selects a fresh load of
manager `+4`. A set bit selects **`0x00648284` for reading maximum HP**, but
**`0x00648380` for writing current HP or status**. The resulting accesses are
`0x0064828e`, `0x00648388`, and `0x00648380`, respectively. The read fallback
is not the write fallback. These are logical addresses requiring supplied
storage, not native pointers or embedded game resources.

The dispatch APIs accept only the read/write enum combinations in the table;
other combinations return zero or do nothing without memory access. This is
a host dispatch convention, not an additional recovered original routine.

## Base-state initialization

Base initialization first clears each of the four move slots in order: move,
PP Ups, then PP. It reloads `object+0x0c` before every setter. It then reads
species and form for later learnset selection, separately reads species, form,
and growth for level resolution, and calls the level service. The earlier
species/form pair remains captured across that service before it is passed to
the learnset selector.

The learnset count is read once initially. When nonzero, the routine queries
the current entry's required level and stops immediately if that unsigned
word exceeds the captured level. Otherwise it fetches the move and calls the
move-insertion service, increments a 32-bit index, and reloads the count for
the continuation comparison. Both index-taking queries receive only the low
16 bits of the index, while the comparison uses the full 32-bit index. No
count query follows a threshold-triggered break. Services must make the loop
terminate; the host implementation adds no iteration cap or sorting rule.

After the loop, including when the count was zero, the routine rereads species
and calls the name builder. It reloads the current manager for the name writer,
then reloads it again to clear the name flag. It directly invokes the
reconstructed refresh routine before writing settings in this order:

1. Settings byte `+0x69` through existing setter `0x001682a4`.
2. Settings byte `+0x6a` through existing setter `0x00168594`.
3. Settings byte `+0x68` through existing setter `0x001687cc`.
4. Settings byte `+0x6b` through existing setter `0x00168404`.

The global settings pointer at logical `0x005d1870` is reloaded separately for
each byte. The byte is captured before the corresponding manager reload and
setter. This differs from the preceding population routine's settings order.
There is no additional species-zero early return in this base routine.

The original function reserves `0x20` stack bytes for the name. The host API
instead requires a caller-owned `0x20`-byte array, passes that exact pointer
to both name services, and performs no initial clearing or extra copy. The
services determine which bytes they initialize and consume. Scratch must remain
valid for the whole call and be disjoint from logical-memory backing storage
and API tables. It may be a separate array within the callback's user context.

## Auxiliary-state initialization

The extra-state routine returns immediately when its initial load of
`manager+4` is zero. Otherwise it captures the original state byte at `+0x0d`.
An original zero invokes the decode callback on that same manager, sets its
state byte to one, and reads its encoded-state byte at `+0x0c` for the original
diagnostic check. An original nonzero skips these operations.

The auxiliary pointer is then reloaded from the captured manager, and exactly
`0x1c` bytes at that address are cleared. It is not retested for zero. If the
original state byte was zero, the routine clears that manager's state byte,
calls its encode callback, and performs the original diagnostic reads: state
first, followed by encoded-state only when state is zero. Changes to the
current state byte do not replace the captured restoration decision.

The clear call at `0x0015e914` handles an unaligned prefix and enters the
zero-fill tail at `0x00301aa0`. This host implementation reproduces the fixed
28-byte clear with the observed byte/halfword alignment prefix, ascending word
stores, and halfword/byte tail. ARM multiple-word stores become individual
word stores; instruction grouping is outside the ordinary-memory adapter
contract. The diagnostic targets are the previously verified returning no-op
at `0x0014f220`; their conditional memory reads are retained.

## Derived-state update

Derived-state update acquires state when the current manager's state byte is
zero and captures the resulting zero-or-one acquisition mask. It then reads
species, form, and growth through separate current-manager reloads and calls
the level service. The full result is passed to the existing auxiliary-level
setter, which performs its own null-pointer check, decoding, and byte store.

The mode service receives the original object and the complete 32-bit mode
argument. The five stat services then run in address order `0x0014fb00`,
`0x0014fbd4`, `0x0014fda4`, `0x0014fe78`, and `0x0014fa2c`. Finally the routine
uses the saved acquisition mask to restore the current manager. With mask
one, restoration depends on the low bit of its current state byte: an even
nonzero byte such as 2 does not trigger restoration. An initially nonzero
state yields mask zero and never triggers restoration here. The manager may
have changed during callbacks; the acquisition mask remains captured.

## Refresh arithmetic and repeated reads

Refresh reads maximum HP, reloads the manager, writes that value narrowed to a
halfword as current HP, and then reloads the manager to clear the status word.
For each of four slots, it follows the complete repeated-read sequence:

1. Read move and PP Ups separately; call the maximum-PP service, narrowing PP
   Ups to a byte. Save the returned full word as `first_max`.
2. Independently reread the move. If zero, skip the remaining work for that
   slot. The initial maximum-PP call has already occurred.
3. Reread move and PP Ups and call maximum PP again, obtaining `second_max`.
   Read current PP and compute `sum = current_pp + first_max` modulo `2^32`.
4. Compute the unsigned minimum of `sum` and `second_max`, then capture only
   its low byte.
5. Reread move and PP Ups and call maximum PP a third time. Take the unsigned
   minimum of that result and the captured byte, then write its low byte as PP
   through a freshly loaded manager.

Every getter has its own manager reload; none of the repeated move, PP Ups,
or maximum-PP results are combined or cached. The byte truncation in step 4
precedes the third callback. For example, a minimum of 302 becomes 46 before
comparison with the third result. Synthetic callbacks can mutate memory or
replace the object's manager between these operations, and the subsequent
reads observe those changes according to the reviewed sequence.

## Remaining services and contracts

| `px_state_api` service | Original function |
| --- | --- |
| `level_from_growth` | `0x00168b3c` |
| `select_learnset` | `0x0014f2b8` |
| `learnset_count` | `0x0014f294` |
| `learnset_level` | `0x0014f2a4` |
| `learnset_move` | `0x0014f310` |
| `learn_move` | `0x00150064` |
| `build_name` | `0x0014f224` |
| `write_name` | `0x001685c0` |
| `maximum_pp` | `0x00150220` |
| `update_mode` | `0x0013aa84` |
| `update_stat` | The five addresses listed above, identified by `enum px_state_stat` |

These callbacks use `px_state_api.user`; memory access and the reused
acquire/restore/extra transform callbacks use `px_pokemon_api.user`. Selected
callbacks, API tables, scratch, and logical memory must remain valid during
their use. Logical addresses and arithmetic are unsigned 32-bit values, and
all memory adapters perform ordinary little-endian accesses. No logical
address is converted to a host pointer. Service callbacks may mutate memory;
the contract does not include asynchronous writes or memory-mapped I/O side
effects on loads and stores. It does not reproduce ARM instruction timing,
faults, ABI register contents, or original stack addresses.

The next bounded work can replace the remaining resource/value helpers and
then the move-insertion and stat engines using their own reviewed dependencies.
Fresh immediate-callee exports here are evidence for that work, not claims
that those callees are already implemented. Real resource loading, name
serialization, growth/learnset/move tables, and the derived-stat calculations
still require reconstruction. Earlier RNG, allocator, encounter ownership,
battle services, and platform boundaries also remain external where previously
documented. Synthetic tests cannot establish gameplay or original-binary
equivalence.

## Evidence and reproduction

The evidence directory contains ARM listings and supporting Ghidra pseudocode
for implemented routines and remaining dependencies. Instruction widths,
masks, conditions, and reloads are taken from the ARM listings. Reproduction
scripts export from the preserved analysis project in a read-only session and
verify emitted instruction and literal bytes against the original executable.
The final set contains 44 function exports: 5,176 instruction bytes and 64
literal bytes match the pinned executable. A repeated export matched all 86
ASM/C files from the initial 43-function set; the final set also includes the
zero-fill tail. All 433 restored checkpoint files retained their recorded
sizes and hashes after the exports, and all 321 earlier milestone checksums
still match. The provenance and validation records describe the checked ranges and
commands. Extracted game inputs, Ghidra project/tool installations, and build
products remain outside the tracked source milestone.
