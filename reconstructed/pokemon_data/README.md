# Pokémon payload population and storage codec

This sixth host-source milestone reconstructs the payload-population routine at
`0x0014f6b8`, its 26 field setters, ability selection, growth-value lookup, and
the manager's checksum/XOR storage operations. It extends the preceding
[constructor milestone](../pokemon_constructor/README.md) with concrete C
implementations for the population and manager-transform boundaries. The
constructor API still exposes those boundaries as callbacks; a host adapter
connects them to this module.

The result is a reconstruction of observed data-writing control flow. Real
species resources, growth-resource loading, random-number services, and larger
object/state initialization remain external. The module does not yet construct
every part of a working game Pokémon or establish gameplay or original-binary
execution equivalence. Function and field names describe observed roles and
are provisional analyst names.

Run the synthetic host tests from this directory:

```sh
make test
```

The Makefile links this module with the five preceding reconstructed modules.
`make -C reconstructed test` passed all six suites with strict C11 warnings.
The new suite and six-module chain also passed AddressSanitizer and
UndefinedBehaviorSanitizer checks, with leak detection disabled for the tracing
environment. `validation.json` and `validation.txt` retain the commands, outputs,
and environment overrides. Native Windows execution has not yet been tested.

Coverage includes 21,952 field-setter cases, 130 population cases, 258 ability
slots, 144 manager-flag combinations, fixed cipher/checksum vectors, and
corruption/fallback paths. The integration fixture checks the complete decoded
payload against a fixed expected buffer, copies the encoded buffers before
poisoning their originals, then verifies decoding and re-encoding of the retained
copies. Species and growth data, larger state services, allocation, copying, and
cleanup remain explicitly synthetic fixture dependencies.

## Implemented functions and layout

| Original address | Host implementation or use |
| --- | --- |
| `0x0014f6b8` | `px_populate_pokemon_data`: populate fields through a temporary manager |
| `0x0014ff4c` | Reuse `px_initialize_pokemon_manager` from the constructor module |
| `0x00168548` | `px_bind_pokemon_data`: bind buffers and immediately encode |
| `0x001685f0` | `px_encode_pokemon_manager` |
| `0x00168660` | `px_decode_pokemon_manager` |
| `0x00174e0c` | `px_pokemon_checksum` |
| `0x00167fb4`, `0x00174e6c` | Reuse `px_xor_pokemon_data`; both reviewed routines apply the same halfword transform |
| `0x0016b358`, `0x0016b3d4`, `0x00176168`, `0x001761e8` | `px_pokemon_block` for logical blocks A, B, C, and D |
| `0x001688c8` | `px_resolve_pokemon_ability`, with personal-resource services external |
| `0x00167f98`, including its tail at `0x00176138` | `px_pokemon_growth_value`: cap the level and read the selected table |
| The 26 addresses in the field table below | `px_set_pokemon_field` |

The data buffer is **`0xe8` bytes**: an eight-byte header followed by four
physical `0x38`-byte blocks. Logical A/B/C/D select those physical blocks through
a permutation table; their order is not fixed by the C implementation.

| Offset in data | Size | Observed use |
| --- | --- | --- |
| `+0` | 4 bytes | XOR key and block-permutation selector |
| `+4` | 2 bytes | Flags; mask `0x0004` marks checksum corruption |
| `+6` | 2 bytes | Checksum of the decoded `0xe0`-byte block region |
| `+8` | `0xe0` bytes | Four physical blocks, each `0x38` bytes |

The logical manager is `0x10` bytes. Its word at `+0` holds vtable token
`0x00598a78`, `+4` holds the optional `0x1c`-byte auxiliary buffer, and `+8`
holds the data buffer. Byte `+0x0c` records encoded state and byte `+0x0d`
controls suppression of encoding during acquired state. These bytes are tested
for zero versus nonzero, rather than restricted to Boolean input values.

Binding stores `data` at manager `+8`, zero at `+0x0c`, `extra` at `+4`, and
zero at `+0x0d`, in that order, then invokes encoding. It does not clear either
buffer or decode preexisting contents. The caller must provide suitable initial
contents; the preceding constructor clears its new data allocation.

## Checksum, XOR, and corruption handling

`px_pokemon_checksum` reads `size >> 1` little-endian halfwords in ascending
address order and sums them modulo `65536`. An odd trailing byte is ignored;
sizes zero and one perform no reads. The original paired accumulators are
represented by one accumulator with the same load order and 16-bit result.

For each complete halfword, the shared XOR primitive advances the key before
using its upper half:

```text
key = key * 0x41c64e6d + 0x6073   (modulo 2^32)
halfword ^= key >> 16
```

An odd trailing byte is untouched. The key restarts from the header value for
each separate transform: the `0xe0`-byte main region and optional `0x1c`-byte
auxiliary region do not share a running key. The checksum covers only the main
decoded region, excluding the header and auxiliary buffer.

Encoding returns immediately when manager `+0x0c` is nonzero, without reading
`+0x0d`. Otherwise it checks `+0x0d` and returns when that byte is nonzero.
When both are zero, it computes the checksum, reloads the data pointer to store
the checksum at `+6`, reloads it again to obtain the key and main region, and
applies XOR. It then reads the auxiliary pointer; if nonzero, it reloads the
data pointer and key for a second XOR pass. Finally it stores one at manager
`+0x0c`.

Decoding checks only `+0x0c`; zero returns immediately, regardless of `+0x0d`.
It transforms the main region and optional auxiliary region, then clears
`+0x0c` **before** validating the checksum. The saved checksum and checksum
region come from the same post-clear data-pointer load. A mismatch causes a
fresh data-pointer load and ORs mask `0x0004` into that buffer's flags. It does
not replace the checksum. A match does not clear an existing corruption flag.

Each ordinary field setter decodes, changes its field, then attempts encoding.
Consequently `+0x0d` can leave a modified buffer decoded when the final encode
is suppressed. The auxiliary-level setter is the exception at entry: a zero
auxiliary pointer returns before even invoking decode.

## Block selection and fallback memory

For uncorrupted data, the selector computes:

```text
row = (key & 0x0003e000) >> 13
physical_block = load8(0x00570eec + 4 * row + logical_block)
address = current_data + 8 + 0x38 * physical_block
```

Logical block numbers 0, 1, 2, and 3 mean A, B, C, and D. The initial data
pointer supplies flags and key; the manager's data pointer is reloaded before
the permutation byte is read. The mask makes rows 0–31 possible, so the
original out-of-range-row diagnostic has no reachable effect. The table byte
is used without a clamp. Callers must supply a valid block number and valid
memory for the selected table entry and resulting address.

When corruption mask `0x0004` is set, selection returns one of the following
logical addresses without reading the key, table, or a replacement data pointer:

| Block | `write_mode == 0` | `write_mode != 0` |
| --- | --- | --- |
| A | `0x006482a0` | `0x0064839c` |
| B | `0x006482d8` | `0x006483d4` |
| C | `0x00648310` | `0x0064840c` |
| D | `0x00648348` | `0x00648444` |

The auxiliary-level setter separately uses fallback base `0x00648380` when
corruption is set. These addresses are logical ARM memory locations, never
native pointers. The module does not embed the original permutation-table or
fallback-buffer contents, load files, or recover those resources at runtime.
Synthetic fixtures must provision the table, selected buffers, and any fallback
regions exercised by their cases. Supplying a synthetic permutation table tests
selection behavior; it does not establish the game's actual block order.

`write_mode` names the observed selector argument. It does not impose host
memory permissions: three settings setters deliberately request mode zero and
then store into the returned block. Their behavior is retained below.

## All 26 field setters

Offsets A/B/C/D below are relative to the selected logical block. Unless noted,
block lookups use mode one. `v` is the full 32-bit argument to the host setter;
stores and explicit masks determine the resulting width. Parameters `p` refer
to the population routine's input block, whose complete layout is documented
in the constructor milestone.

| Enum value | Original setter | Population source | Destination and narrowing |
| --- | --- | --- | --- |
| `PX_DATA_KEY` | `0x00168360` | Word `p+0` | Header word `data+0 = v`; no block lookup |
| `PX_DATA_WORD_10` | `0x001686e8` | Word `p+0x10` | A `+4`, full word |
| `PX_DATA_WORD_08` | `0x00168028` | Word `p+8` | A `+0x10`, full word |
| `PX_DATA_SPECIES` | `0x001687f8` | Halfword `p+0x18` | A `+0`, low 16 bits |
| `PX_DATA_FORM` | `0x00168780` | Byte `p+0x1a` | A `+0x15`, replace bits 3–7 with `(v << 3) & 0xf8` |
| `PX_DATA_EXTRA_LEVEL` | `0x0014fd04` | Low byte of halfword `p+0x1c` | Auxiliary `+4`, low byte; zero auxiliary pointer skips the entire setter; corruption selects fallback `0x00648380+4` |
| `PX_DATA_NATURE` | `0x00167ffc` | Halfword `p+0x20` | A `+0x14`, low byte |
| `PX_DATA_GENDER` | `0x00168740` | Halfword `p+0x1e` | A `+0x15`, replace bits 1–2 with `(v << 1) & 6` |
| `PX_DATA_LEVEL` | `0x00168054` | Low byte of halfword `p+0x1c` | D `+0x2d`, replace bits 0–6 with `v & 0x7f`, retaining bit 7 |
| `PX_DATA_ZERO_BYTE` | `0x00168568` | Zero | D `+0x2f`, low byte |
| `PX_DATA_PARAM_30` | `0x0016830c` | Low byte of word `p+0x30` | Read C `+0x1b` with mode zero; if nonzero, freshly select C with mode one and store its `+0x2a`; otherwise freshly select D with mode one and store its `+0x1a`; low byte |
| `PX_DATA_SETTING_68` | `0x001687cc` | Settings byte `+0x68` | D `+0x33`, **mode zero**, low byte |
| `PX_DATA_SETTING_69` | `0x001682a4` | Settings byte `+0x69` | D `+0x30`, mode one, low byte |
| `PX_DATA_SETTING_6A` | `0x00168594` | Settings byte `+0x6a` | D `+0x31`, **mode zero**, low byte |
| `PX_DATA_SETTING_6B` | `0x00168404` | Settings byte `+0x6b` | D `+0x32`, **mode zero**, low byte |
| `PX_DATA_STAT_24` | `0x00168090` | Low byte of halfword `p+0x24` | B word `+0x34`, replace bits 0–4 with low five bits |
| `PX_DATA_STAT_26` | `0x0016810c` | Low byte of halfword `p+0x26` | B word `+0x34`, replace bits 5–9 with low five bits |
| `PX_DATA_STAT_28` | `0x0016814c` | Low byte of halfword `p+0x28` | B word `+0x34`, replace bits 10–14 with low five bits |
| `PX_DATA_STAT_2A` | `0x00168384` | Low byte of halfword `p+0x2a` | B word `+0x34`, replace bits 20–24 with low five bits |
| `PX_DATA_STAT_2C` | `0x001683c4` | Low byte of halfword `p+0x2c` | B word `+0x34`, replace bits 25–29 with low five bits |
| `PX_DATA_STAT_2E` | `0x001680cc` | Low byte of halfword `p+0x2e` | B word `+0x34`, replace bits 15–19 with low five bits |
| `PX_DATA_ABILITY` | `0x00168278` | Ability resolver result | A `+0x0c`, low byte |
| `PX_DATA_ABILITY_FLAG_0` | `0x00168430` | Fresh comparison `p[0x22] == 0` | A byte `+0x0d = (old & ~1) \| v`, narrowed to a byte |
| `PX_DATA_ABILITY_FLAG_1` | `0x00168468` | Fresh comparison `p[0x22] == 1` | A byte `+0x0d = (old & ~2) \| (v << 1)`, narrowed to a byte |
| `PX_DATA_ABILITY_FLAG_2` | `0x001684a0` | Fresh comparison `p[0x22] == 2` | A byte `+0x0d = (old & ~4) \| (v << 2)`, narrowed to a byte |
| `PX_DATA_GROWTH` | `0x00168714` | Selected growth-table word | A `+8`, full word |

The six packed-stat setters retain every bit outside their respective five-bit
mask, including bits 30 and 31. Their field order is deliberately
`0, 5, 10, 20, 25, 15`, matching the parameter offsets above.

The ability-flag setters clear only the selected bit but do **not** mask the
input to one bit before ORing it into the byte. A direct caller can therefore
set adjacent bits with a larger argument. Population supplies zero or one.
Invalid host enum values return without a memory access; this dispatch behavior
has no corresponding original standalone setter.

## Population order, resources, and captured values

Population initializes the caller's scratch manager and binds the data with a
zero auxiliary pointer. Binding immediately calculates a checksum and XORs the
main region. Only then does population read species at `p+0x18`. **Species zero
returns after that initial encoding**, without field writes or personal/growth
resource calls; it is not a no-op on the supplied data and manager.

For nonzero species, setters run in the table's order. Because this routine
binds no auxiliary buffer, its auxiliary-level call returns at its initial
pointer check under the ordinary memory contract; no explicit resource callback
runs before that call. The direct field API also supports other callers with
a nonzero auxiliary pointer. The routine neither copies nor renormalizes `p`, clears
the data, allocates buffers, nor sets the acquired-state byte to batch writes.
Under ordinary state, successive setters therefore decode and re-encode the
main data around each individual update.

Each parameter value is read when its setter is reached. The full parameter
words at `+0`, `+8`, and `+0x10` are consumed, but their paired high words at
`+4`, `+0x0c`, and `+0x14` are not used here. Level and six stat inputs are
read as halfwords and narrowed to a byte before their setter-specific masks;
word `+0x30` likewise contributes only its low byte. These conversions are
part of population, not extra range validation in the generic setters.

The settings pointer at logical address `0x005d1870` is reloaded separately
before each read of settings bytes `+0x68`, `+0x69`, `+0x6a`, and `+0x6b`.
The returned byte is captured before the corresponding setter runs.

Ability resolution receives the current species halfword, form byte, and slot
byte from `p`. Its standalone API accepts full 32-bit arguments. It chooses
attribute `0x1b` for slot 1, `0x1c` for slot 2, and `0x1a` for every other
full-width slot, then calls the personal-resource selector. The chosen
attribute survives that callback, and the subsequent attribute lookup result
is narrowed to a byte. Population captures that ability for its setter but
rereads the slot separately for each of the three flag comparisons. A service
that changes the parameter block can thus affect later flags without changing
the already selected ability.

The personal services are `memory->parameters->select_species`
(`0x00167e2c`) and `memory->parameters->personal_value` (`0x00167e18`), using
the parameter API's own user context. This module does not reconstruct the
actual species/form resource selection or attribute tables.

Growth selection uses the separate `px_data_api.select_growth` callback
(`0x00167db0`) and `px_data_api.user`, with freshly read species and form.
The actual loader/cache and choice of growth resource remain external. After
that callback returns, population rereads the level halfword and takes its low
byte. `px_pokemon_growth_value` caps its full unsigned argument at 100, loads
the selected table pointer from logical `0x005d17ac`, and directly reads the
word at `table + 8 + 4 * level`. There is no callback for the final value and
no growth formula or original table data embedded in the source. A growth
callback must leave a valid selected table available in logical memory.

The standalone codec and setters use only the memory access members of
`px_pokemon_api`. That shared header also contains allocator, constructor,
normalization, and state-service callbacks for earlier milestones; their
presence does not make them dependencies of these codec operations. The
population path additionally uses the two personal services and growth
selection when species is nonzero. It makes no random-number calls itself.
Real RNG, parameter-mode and gender services, allocation, and larger base,
auxiliary, derived, and refresh-state behavior still belong to the broader
constructor's external boundaries.

## Host memory and scratch contract

All target addresses are unsigned 32-bit logical addresses routed through
`load8/load16/load32` and `store8/store16/store32`. Adapters must provide
ordinary little-endian memory behavior and valid storage for every accessed
address, including selected tables and fallbacks. Address arithmetic retains
32-bit wrapping. No address is cast to a host pointer, and no original vtable
token is called as native code.

The caller provides a valid `0x38`-byte host parameter array, a logical
`0xe8`-byte data buffer, and a logical **`0x10`-byte scratch manager**. The
scratch region must be disjoint from the data and parameter storage and valid
for the entire call. Resource services may mutate mutable parameter storage or
logical memory through their user contexts; reviewed captures and fresh reads
determine which later operations see those changes. The API and callback
tables must themselves remain valid. Ordinary memory accesses do not imply
memory-mapped I/O, concurrent writes, or arbitrary side effects on loads.

The original population routine uses a stack-local manager and returns its
stack address in `r0` as the frame expires. The reviewed constructor caller
ignores that token. The host population function deliberately returns `void`
and uses caller-owned scratch storage instead of exposing a dangling host
pointer or manufacturing an original ARM stack address. This is an explicit
host interface adaptation, not an original ABI reproduction.

## Evidence and limits

The reviewed evidence set contains **42 function exports**, including reused
helpers and external resource dependencies as well as implemented functions.
Verification covers **3,360 instruction bytes**, **76 literal bytes**, and
the **128-byte permutation table**. All 84 reproduced ASM/C files match the
retained exports. That count is not a claim that 42 new functions or all their
dependencies are fully reconstructed. ARM listings determine access widths,
ordering, masks, and branch behavior; generated decompiler C is supporting
evidence. The evidence/provenance bundle records the reviewed ranges and
original input. Keeping the table as reviewed evidence does not embed its
contents into the host implementation or replace the synthetic test fixtures.

This milestone's export uses the existing Ghidra analysis with `-noanalysis`
and `-readOnly`; no temporary Ghidra flow or function-body repair was needed.
This differs from the earlier constructor's separately documented repair.

The host tests use synthetic resources and logical memory. They can check
checksum vectors, storage round trips, field masks, callback mutations, and
the reconstructed six-module chain. Such checks do not supply game assets,
resource loaders, Nintendo 3DS services, battle rules, rendering, input, audio,
or a playable PC executable, and do not establish full original-game or
machine-code equivalence.
