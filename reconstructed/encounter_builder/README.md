# Encounter record builder

This milestone reconstructs Field functions `0x15ce4a10` (one-record builder),
`0x15ce4958` (population loop), and `0x15ce49c4` (configuration selection).
The previous battle-task module can now call these implementations in its
population and config adapters. The constructor, data lookup, copy, cleanup,
and random-number implementations remain explicit dependencies.

Run `make test` here, or in the parent directory to run all four milestones.
The Makefile shows the direct GCC/Clang C11 compiler command. The tests use
synthetic memory and data. They do not launch the game or create a complete
original-format Pokémon object. Recorded validation was performed on Linux;
native Windows execution is not yet verified.

## Parameter preparation

The builder accepts an 18-byte slot inside the earlier request buffer. It
prepares a temporary parameter block, allocates a 16-byte object, invokes a
constructor, optionally applies a selected value, appends through a copy
operation, and invokes the temporary object's virtual cleanup method.
Source names here are analyst choices, not recovered original names.

The parameter block uses bytes through `+0x34`; the host storage size is
`0x38`, including untouched padding. Byte `+0x1b` and bytes `+0x35..0x37`
are preserved. The API takes caller-provided parameter storage so tests
can check these holes without assigning them invented values.

| Parameter offset | Verified source or rule |
| --- | --- |
| Words `+0`, `+4`, `+8` | `0xffffffff` |
| Word `+0x0c` | Initially 3; slot byte 6 equal to 1/2 changes it to 2/1 |
| Words `+0x10`, `+0x14` | Request word `+0x9c`, then zero |
| Halfword `+0x18` | Slot halfword 0 |
| Byte `+0x1a` | Slot byte 5 |
| Halfword `+0x1c` | Zero-extended slot byte 4 |
| Halfword `+0x1e` | Slot byte `+0x10` equal to 1/2 gives 0/1; otherwise `0xff` |
| Halfword `+0x20` | Initially `0xffff`; request flag `0x400` replaces it with request byte `+0x97` |
| Byte `+0x22` | Initially `0xff`; slot byte 7 in 1–3 gives 0–2; request mode 2 overrides this through two context helpers |
| Byte `+0x23` | Initially 1; for slot byte 6 outside 1/2, derives from request flags, mode, and halfword `+0xa2`, with byte truncation |
| Six halfwords `+0x24..0x2f` | `0xffff` |
| Word `+0x30` | `0x0000ffff` |
| Byte `+0x34` | Attribute selectors `0x18`/`0x19` can force 3; otherwise request mode 2 gives 2, else slot byte `+0x11` |

Initial species/form/slot-byte-4 fields and request word `+0x9c` are sampled
before the data preloader. Its actual arguments are **both** the slot
halfword 0 and slot byte 5. Main helper `0x00167e2c` consumes both even
though the initial decompiler call display omitted the second argument.
The constructor likewise consumes three arguments: allocated handle,
heap, and parameter-block pointer.

Attribute `0x18` is checked first. Only a full 32-bit result equal to 15
skips the `0x19` lookup. The context override caches request word `+0x78`
across its two callbacks, then reads the `0x400` flag afterward. These
ordering details allow later reads to observe helper side effects.

## Optional value selection

Slot halfword `+2` is sampled after construction. Values 1 through `0x2cc`
(716) go to the setter directly. Larger values skip the setter. Zero
causes all three candidate attributes `0x11..0x13` to be queried and narrowed
to 16 bits. Equal first and second candidates select the first without
calling RNG. Otherwise:

1. Call the random helper with bound 100 and keep its low eight bits.
2. Read request flag bit 9 and the probability table after that call.
3. Compare against cumulative thresholds, narrowing every cumulative sum
   to eight bits and using strict unsigned comparisons.
4. Select a candidate or zero, then invoke the setter even when zero wins.

The table is writable module data. Its recovered synthetic address is
`0x15d3d33e`, and the uploaded bytes contain rows `{50,5,0}` and `{60,20,0}`.
The API supplies a logical table base and memory reader rather than freezing
those values in the implementation. This preserves behavior if game code
changes the table. The tests include changed tables and threshold wrapping.

The selected value's gameplay meaning is not used to justify the implementation;
`set_value` preserves the verified numeric behavior without requiring a
recovered field declaration.

## Population and configuration helpers

Population uses **18-byte slots**. The count word at request `+0x6c` is
interpreted as signed: initially zero returns zero, initially negative
returns one without a builder call. A positive count starts the loop,
which reloads the count after every builder call and returns one after
it stops. The host caller must supply readable storage for every slot the
changing count selects; this source does not invent a six-slot clamp.

Configuration normally returns request byte `+0x8e`, zero-extended. Value
`0x40` selects an indirect byte through request word `+0x88`: byte 5 if
request byte `+0x84` is 10, otherwise byte 4. A zero indirect pointer returns
zero. Both helpers' fallback diagnostic resolves to main `0x0014f220`, a
returning no-op, rather than the separate active fatal-loop diagnostic.

## Copy and cleanup findings

The append function `0x0039e814` loads an existing destination slot and
passes it with the temporary object to `0x0039a52c`. It increments the
count afterward; it does not put the temporary handle into the slot array.
The copy chain moves two regions, `0xe8` and `0x1c` bytes, through shared
buffer `0x0063bd10`, then updates the destination's data state through
additional helpers. The parameter layout is therefore not itself a full
serialized Pokémon object.

After append returns, the builder reloads the temporary object's vtable
and calls the entry at `+8`. The observed constructor installs vtable
`0x00598a50`, whose entry points to `0x0039a728`. Its raw four-instruction
sequence calls `0x0039dc54` then tail-branches to `0x002ff520`. The former
releases internal allocations and invokes nested cleanup; the latter
dispatches deallocation through the owning heap. Full data transformation
and heap behavior remain unreconstructed.

This establishes copying before temporary cleanup for the reviewed path.
It does not establish every ownership rule throughout the game. Allocation
failure is not a new recoverable path: the original builder can still call
the setter/append with zero, and the real append dereferences its argument.
Synthetic tests record that call sequence without claiming it is safe game
behavior.

## Explicit dependency boundaries

| Callback | Resolved main function |
| --- | --- |
| `select_species(species, form)` | `0x00167e2c` |
| `personal_value(selector)` | `0x00167e18` |
| `context_index(context)` | `0x00432a7c` |
| `context_probe(context, index)` | `0x00432f00` |
| `allocate(size, heap)` | `0x00122920` |
| `construct(storage, heap, parameters)` | `0x0039a5a8` |
| `random_below(bound)` | `0x0045a908` |
| `set_value(object, value)` | `0x0039d804` |
| `append(destination, object)` | `0x0039e814` |
| `destroy_virtual(function, object)` | Dynamic target read after append |

Memory callbacks interpret logical ARM32 addresses, never native host
pointers. Request and slot pointers address decoded little-endian host
bytes; parameter storage must not overlap them. Inputs, callback targets,
and all selected memory accesses must be valid. Ordinary callback-induced
mutations are modeled; concurrent unsynchronized mutation, ARM timing,
physical access widths, invalid-memory faults, and the ABI are outside
scope. These are source reconstructions with explicit adapters, not
instruction-matching original source.

## Next target

Replace the constructor boundary `0x0039a5a8`, beginning with its base
initializer `0x0039daf4` and parameter-to-data helper `0x0039a8a4`. That
will move the chain from prepared creation parameters toward actual
Pokémon data. Scene/state constructors, assets, rendering, audio, and
reference-game comparisons remain necessary for a playable port.

Field addresses are synthetic static-analysis addresses. Main addresses
retain the original map. `evidence/provenance.json` binds the reviewed
instructions and table bytes to uploaded inputs. `validation.txt` records
the actual host checks; they are not differential execution of the game.

The strict C11 build and AddressSanitizer/UndefinedBehaviorSanitizer run
passed the new parameter, mutation, population, and integration checks,
including 768 weighted draws and 512 configuration cases. Earlier milestone
tests also passed. Leak detection was disabled for the container tracing
environment. Evidence verification matched 2,592 instruction bytes across
22 reviewed functions and the six writable table bytes against the uploads.
