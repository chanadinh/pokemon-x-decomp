# Pokémon constructor and parameter normalization

This fifth host-source milestone replaces the encounter builder's constructor
boundary with reviewed C implementations of parameter normalization, data-buffer
allocation, object/manager initialization, auxiliary-buffer preparation, and
state acquisition/restoration. The constructor still calls explicit services
for writing the Pokémon payload and initializing larger data/state structures.
It does not yet produce a complete original-format Pokémon object or run a game
battle. Names are analyst choices; original source declarations were not recovered.

Run the module's synthetic host tests with a C11 compiler and Make:

```sh
make test
```

The Makefile links this milestone with all four preceding source modules. The
parent directory's `make test` passed all five suites with strict C11 warnings.
The constructor suite also passed AddressSanitizer and UndefinedBehaviorSanitizer
checks (leak detection disabled). `validation.json` and `validation.txt` record
the commands, environment overrides, and results. Native Windows execution has
not yet been tested.

The synthetic tests cover 66,388 parameter cases, including every 16-bit level,
all 256 state-byte values for acquisition, 2,304 state/mask combinations for
restoration, fixed XOR vectors, allocation and callback mutations, and a chain
through all five reconstructed modules. These checks exercise the host contracts;
they do not compare execution with the original game.

## Implemented behavior

| Original address | Host implementation or use |
| --- | --- |
| `0x0039a5a8` | `px_construct_pokemon`: derived constructor and helper sequence |
| `0x0039daf4` | `px_initialize_pokemon_base`: base object, data, and manager setup |
| `0x0039a8a4` | `px_create_pokemon_data`, including the local defaults exposed as `px_default_pokemon_parameters` |
| `0x0014f324` | `px_normalize_pokemon_parameters`: complete parameter-normalization control flow through service callbacks |
| `0x0014ff4c` | `px_initialize_pokemon_manager` |
| `0x0013a7a0` | `px_attach_pokemon_buffers` |
| `0x00140120` | Manager/data/key pointer chain inlined in `px_construct_pokemon` |
| `0x0012b6d0` | `px_create_pokemon_extra`: allocation, clearing, and XOR preparation |
| `0x00167fb4` | `px_xor_pokemon_data`: complete halfword XOR primitive |
| `0x0012f9a4` | `px_acquire_pokemon_state` |
| `0x0012f978` | `px_restore_pokemon_state` |
| `0x0013a768`, `0x0013a720` | Internal acquire/restore helpers; manager transforms remain service callbacks |
| `0x00301a9c` | Zero-fill effect at the constructor's fixed aligned call sites; the original ARM store grouping is not reproduced |

Some exported functions in `evidence/` remain dependencies rather than host
implementations. The callback tables below distinguish those boundaries from
the reconstructed control flow.

## Parameters and random choices

Normalization first copies **all `0x38` bytes** from source to destination,
including byte `+0x1b` and bytes `+0x35..+0x37`. No service runs until that copy
is complete. The preceding converter initializes a local default block, but the
complete copy overwrites it: defaults are not merged into missing source fields.
The separately exposed defaults helper preserves its unwritten padding bytes.

| Offset | Observed normalization |
| --- | --- |
| Words `+0`, `+4` | Only the pair `{0xffffffff, 0xffffffff}` triggers a random word in the low word and zero in the high word. |
| Words `+8`, `+0x0c` | Low word `0xffffffff` selects special behavior only when the high word is 1, 2, or 3. Other pairs are preserved. |
| Words `+0x10`, `+0x14` | The same two-word all-ones sentinel rule as `+0`, `+4`. |
| Halfword `+0x18` | Species argument for lookup services; not independently clamped here. |
| Byte `+0x1a` | Form `0xff` uses the base-species probe. A nonzero probe selects zero; otherwise attribute `0x20` supplies the low-byte bound for a random form. |
| Halfword `+0x1c` | Unsigned values above 100 become 100; lower values, including zero, remain unchanged. |
| Halfword `+0x1e` | Only exact value `0x00ff` triggers attribute-based gender selection; the final resolver runs for every input and receives the current low byte. |
| Halfword `+0x20` | `0xffff` triggers a bound-25 random draw, stored with 16-bit truncation. |
| Byte `+0x22` | `0xff` and `0xfe` trigger bound-2 and bound-3 draws respectively, stored with byte truncation. Other values remain unchanged. |
| Byte `+0x23` | Mode-3 candidate-attempt budget, reloaded after every failed predicate call. |
| Six halfwords `+0x24..+0x2f` | Selected entries become 31; remaining `0xffff` entries take bound-32 draws, stored with 16-bit truncation. Other entries are retained when no reset is requested. |
| Word `+0x30` | Only full 32-bit value `0x0000ffff` triggers lookup attribute `0x16`; the result is stored as a full word. |
| Byte `+0x34` | A nonzero value resets all six halfwords to `0xffff`. Values 1–5 request that many distinct entries set to 31; values above 5 become zero but still cause the reset. |

Modes 1 and 2 draw a candidate, then read the current key at `+0x10` and pass
both to their respective value service. They store the result at `+8` and zero
the high word at `+0x0c`. Mode 3 performs no draw when its initial budget is
zero. Otherwise it writes each candidate pair before calling the predicate,
stops on a nonzero predicate result, or continues according to the freshly read
budget. Predicate changes to the destination remain visible.

Selection of the entries set to 31 uses the random result's low byte as an
index. A draw selecting an already assigned entry is retried. The requested
count is reread after each successful selection. Subsequent random-field draws,
form lookups, and gender processing read their inputs when reached, so earlier
service mutations can affect them.

For gender sentinel `0x00ff`, attribute `0x14` is compared at its full 32-bit
width. Values zero, `0xfe`, and `0xff` select 0, 1, and 2. Other values select 1
only when the attribute is strictly greater than `random_below(252) + 1`, with
32-bit wrapping in the addition; otherwise they select zero. The resolver
receives the current species, form, and low byte of this halfword and its return
is narrowed to 16 bits. Word `+0x30` is reloaded after that service returns,
before its result is stored, and its sentinel comparison remains full-width.

## Allocation, pointers, and state

The earlier encounter builder allocates the **16-byte outer object** before
calling `px_construct_pokemon`. The constructor does not allocate that object
again. Its internal allocation sequence is:

1. Normalize the caller's parameters and allocate a `0xe8`-byte data buffer.
2. Clear a nonzero data allocation twice, then call the data-population service.
3. Allocate a `0x10`-byte manager, initialize it if nonzero, and attach the data
   pointer with no auxiliary buffer yet.
4. Run base-state initialization, install the derived vtable token, read the
   manager/data/key chain, and allocate the `0x1c`-byte auxiliary buffer.
5. Clear a nonzero auxiliary allocation twice, apply the XOR primitive using
   the captured key, attach the current data and new auxiliary buffer, and run
   the state sequence.

The object stores logical vtable tokens `0x00598a88` during base initialization
and `0x00598a50` afterward. Its data, auxiliary, and manager fields occupy
`+4`, `+8`, and `+0x0c`. The manager's vtable token is `0x00598a78`; attaching
buffers writes manager byte `+0x0c = 1`, word `+8 = data`, byte `+0x0d = 0`,
then word `+4 = auxiliary`, preserving its bytes `+0x0e..+0x0f`.

Zero allocation is not converted into a graceful error result. A zero data
allocation still enters the unconditional clear and population path. A zero
manager still reaches buffer attachment. A zero auxiliary allocation still
receives the unconditional clear and XOR pass. Synthetic adapters can record
these logical accesses; this does not establish that the original game safely
handles allocation failure. Valid memory for all selected accesses is a caller
precondition, and a logical zero address is never converted into a native C
null-pointer dereference by this module.

The key is read through `object+0x0c -> manager+8 -> data+0` before auxiliary
allocation. It remains captured if allocation services later change the object
or data pointers. After allocation, the constructor stores the returned
auxiliary handle, then reloads the manager and the object's current data field
for attachment. Other service boundaries likewise preserve the reviewed
distinction between saved values and fresh pointer reads.

Acquisition checks manager byte `+0x0d`: nonzero returns zero; zero causes a
manager reload, invokes the decode service, sets the captured manager's byte to
one, and returns one. Restoration sign-extends that byte to a 32-bit word and
ANDs it with the **full 32-bit mask**. A nonzero result reloads the manager,
clears its state byte before invoking the encode service, and returns one.
For example, a byte with bit 7 set can match a mask whose bits lie above the low
byte. This is not a Boolean-only or low-byte-only test.

The internal state helpers keep their selected manager across transform
callbacks. Their diagnostic branches target the previously verified returning
no-op at `0x0014f220`. The constructor retains the acquisition result across
auxiliary-state initialization, derived-state update with mode 1, and refresh,
then restores using the current manager and that saved result.

## Halfword XOR primitive

For each of `size >> 1` halfwords, `px_xor_pokemon_data` advances a 32-bit key:

```text
key = key * 0x41c64e6d + 0x6073   (modulo 2^32)
halfword ^= key >> 16
```

The key advances before each XOR. Halfwords use the logical little-endian
memory adapters, and an odd trailing byte is untouched. Zero and one-byte
lengths perform no memory access. Applying the same sequence twice reverses
the transformation, but this primitive alone does not implement the complete
Pokémon storage format, its block arrangement, or all manager transitions.

## Explicit service boundaries

| Parameter service | Original function |
| --- | --- |
| `random_word` | `0x00167c0c` |
| `random_below` | `0x00167bdc` |
| `mode1_value` | `0x001689a4` |
| `mode2_value` | `0x00168944` |
| `candidate_matches` | `0x001688a4` |
| `select_species_base` | `0x00167ee4` |
| `form_probe` | `0x00167f44` |
| `select_species` | `0x00167e2c` |
| `personal_value` | `0x00167e18` |
| `resolve_gender` | `0x00168aa0` |

| Constructor service | Original function |
| --- | --- |
| `allocate` | `0x00122920` |
| `populate_data` | `0x0014f6b8` |
| `initialize_base_state` | `0x0013a8d8` |
| `decode_manager` | `0x00168660` |
| `encode_manager` | `0x001685f0` |
| `initialize_extra_state` | `0x0012b744` |
| `update_derived_state` | `0x0012b808` |
| `refresh_state` | `0x0013a7bc` |

The source and normalized scratch arrays must each provide `0x38` valid bytes
and must not overlap. The caller supplies scratch storage so padding, copied
bytes, and callback changes can be checked without inventing uninitialized
stack values. Both API structures, required callbacks, and selected logical
memory addresses must remain valid throughout the call.

Memory callbacks perform ordinary little-endian reads and writes to logical
ARM32 addresses; those addresses are not native host pointers. Service callbacks
may mutate the parameter destination or game memory through their user context,
and subsequent reads follow the observed timing. The implementation does not
model asynchronous mutation, hardware faults, ARM execution timing, physical
load/store widths, or the original ABI.

The bound-6 RNG path must produce a low-byte index into the six selected
halfwords. Other services must satisfy their original argument contracts, and
retry callbacks must eventually make progress. The source adds no attempt cap,
index clamp, or replacement draw to hide malformed service behavior. Tests may
exercise truncation and wraparound with synthetic return values; such tests do
not establish the behavior of an actual game's RNG or data resources.

## Evidence and remaining work

The saved Ghidra view initially ended `0x0039a8a4` after 212 bytes because
returning zero-fill helper `0x00301a9c` was incorrectly treated as no-return.
`RepairPokemonConstructorForExport.java` verifies specific instruction and
literal words, clears the two affected flow overrides at `0x0039a968` and
`0x0039a974`, marks the helper returning, and restores the converter's complete
236-byte body. It runs in a temporary read-only analysis session, then exports
the requested evidence; the repair is not saved to the project and does not
change the input executable.

`evidence/provenance.json` records comparisons of **3,560 instruction bytes
across 19 exported functions**, plus **24 literal bytes**, against the uploaded
executable. These exported functions include both implemented behavior and
remaining dependencies. The six literals establish the three vtable tokens,
XOR multiplier, normalization sentinel, and default species value. Reproduction
scripts and the exact headless command accompany the evidence.

The next substantial boundary is payload population at `0x0014f6b8` and its
field-writing services, followed by the remaining manager/state operations.
Real species resources and RNG implementations, complete data semantics, and
comparisons with the running game remain necessary. These are analyst-authored
host adaptations supported by static evidence and synthetic tests, not original
source, instruction-matching builds, or demonstrated gameplay equivalence.
