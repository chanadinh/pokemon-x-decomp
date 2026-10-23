# Pokémon HP, stats, and level calculations

This eighth host-source milestone reconstructs the HP and five stat-update
engines used by the preceding state milestone. It also implements their scalar
arithmetic, nature adjustment, growth-to-level scan, object-level/max-HP
helpers, and additional field accessors. Adapters can replace the seventh
milestone's `update_mode`, `update_stat`, and `level_from_growth` callbacks
without changing any earlier module. All seven earlier source, evidence, and
checksum checkpoints remain immutable.

Personal and growth resource selection remain external through the existing
parameter/data APIs. Neither real resources nor the nature table are embedded
in this implementation. It supplies additional reconstructed host behavior,
not a complete Pokémon implementation, playable battle, or original-binary
equivalence. The broader target is documented in the
[first playable slice plan](../../docs/PLAYABLE_SLICE.md). Names in this module
are analyst choices, not recovered original declarations.

Run the synthetic host suite with a C11 compiler and Make:

```sh
make -C reconstructed/pokemon_stats test
make -C reconstructed test
```

The module's integration fixture links all eight source modules. Its resources
and expected bytes are synthetic; the checks do not execute the original game.

Validation on Ubuntu 24.04 under WSL2 passed all eight C suites with strict
C11 warnings, plus AddressSanitizer/UndefinedBehaviorSanitizer for the new suite
and eight-module chain. Leak detection was disabled in the tracing environment.
The Python suite ran 38 tests successfully with two optional extraction checks
skipped because PowerShell/3dstool were unavailable. Commands and outputs are
retained in `validation.json` and `validation.txt`. Native Windows execution
and comparisons with a running game remain untested.

Checks include fixed arithmetic vectors, unsigned boundaries, all five nature
columns, 768 field cases across 32 block rows and four codec states, 160 stat
updates, HP mode/delta/clamp cases, resource mutations, and independently fixed
expected payload/auxiliary bytes through all eight modules. These synthetic
values test the reviewed operations; they do not verify actual game resources.

The final evidence contains 45 function exports: 4,080 instruction bytes and
116 literal bytes match the pinned executable. The repeated export matches
all 88 ASM/C files in the initial set. All 429 previous milestone checksums
and all 433 restored analysis-checkpoint files retain their recorded hashes.
The shared nature tail is included in an existing export, as explained below.
See `reproduction/README.txt` for the read-only export and verification steps.

## Public entry points and original boundaries

| Host operation | Original address or role |
| --- | --- |
| `px_update_pokemon_hp` | `0x0013aa84` |
| `px_update_pokemon_stat` | Dispatch of `0x0014fb00`, `0x0014fbd4`, `0x0014fda4`, `0x0014fe78`, and `0x0014fa2c` using the existing `px_state_stat` enum |
| `px_pokemon_object_level` | `0x00168ae4`, including its fallthrough to the growth-to-level resolver |
| `px_pokemon_object_max_hp` | `0x0015306c` |
| `px_pokemon_level_from_growth` | `0x00168b3c` |
| `px_pokemon_hp_value` | `0x00168b84` |
| `px_pokemon_stat_value` | Scalar entries `0x0014ffa4`, `0x0014ffd4`, `0x0014ff74`, `0x00150004`, and `0x00150034` |
| `px_apply_pokemon_nature` | Shared tail `0x001689fc`, included in the `0x00150034` export |

Each of the five stat-update listings includes its auxiliary setter as a
fallthrough tail. The caller restores its frame, executes a no-op, and enters
the setter prologue. The host implementation retains both portions using the
ordinary `px_write_pokemon_stat` call; it does not stop at the first stack
restore or require a new original function boundary.

The internal `px_stat` enum follows the resource field order: HP, Attack,
Defense, Speed, Special Attack, Special Defense. The previous milestone's
`px_state_stat` enum instead identifies its original five call addresses.
The host dispatcher explicitly maps between those two orders.

| Stat | Update entry | Scalar entry | Personal attribute | Nature column | Auxiliary result offset | Setter-tail entry |
| --- | --- | --- | --- | --- | --- | --- |
| HP | `0x0013aa84` | `0x00168b84` | 0 | None | `+0x0a` | `0x0014fd54` |
| Attack | `0x0014fb00` | `0x0014ffa4` | 1 | 1 | `+0x0c` | `0x0014fb84` |
| Defense | `0x0014fbd4` | `0x0014ffd4` | 2 | 2 | `+0x0e` | `0x0014fc58` |
| Speed | `0x0014fa2c` | `0x0014ff74` | 3 | 5 | `+0x10` | `0x0014fab0` |
| Special Attack | `0x0014fda4` | `0x00150004` | 4 | 3 | `+0x12` | `0x0014fe28` |
| Special Defense | `0x0014fe78` | `0x00150034` | 5 | 4 | `+0x14` | `0x0014fefc` |

## Scalar arithmetic

The standalone scalar interfaces accept full 32-bit arguments. The separate
orchestrators perform their original caller-side byte/halfword narrowing.
For both HP and ordinary stats, the common calculation is:

```text
sum = iv + (base << 1) + (ev >> 2)       modulo 2^32
product = sum * level                  modulo 2^32
q = high32(product * 0x028f5c29)        full 64-bit unsigned product
```

The HP formula returns the low 16 bits of `q + level + 10`. Exact full-word
species value `0x00000124` instead returns one immediately. A larger word with
the same low halfword does not match that standalone formula's special case.

An ordinary stat takes the low 16 bits of `q + 5` and applies nature. The
source preserves the exact unsigned multiply-high operation; it does not
replace it with an assumed integer division by 100. This distinction can
matter for large synthetic arguments. All sums, shifts, and low-word products
retain the reviewed unsigned wrapping behavior.

The nature helper first validates its **full 32-bit** column argument. Only
columns 1–5 read memory; other columns return the supplied value unchanged.
For a valid column it reads the signed byte at logical address:

```text
0x00570f6c + 5 * nature + column - 1    modulo 2^32
```

Nature is not clamped to an assumed number of rows. Byte `0x01` selects factor
110, and byte `0xff` selects factor 90. Every other byte returns the supplied
value unchanged. For either selected factor the exact calculation is:

```text
t = low16(value * factor)
result = ((t * 0x0000a3d7 + 0x0000a3d7) modulo 2^32) >> 22
```

The low-halfword truncation occurs **before** the rounded multiplication.
This is not an unrestricted percentage multiplication or a floating-point
formula. The logical nature-table contents must be supplied by the host
adapter; this milestone does not embed them or claim to verify their contents.

Invalid host `px_stat` values make the generic stat-value function return zero
and make the generic updater/setter return without access. Invalid IV/EV enum
values likewise return zero. Those dispatch rules are host interface behavior,
not new recovered original functions.

## Level selection and object helpers

`px_pokemon_level_from_growth` captures the growth argument before calling
`px_data_api.select_growth(species, form)`. It then queries thresholds in
ascending order at levels 1 through 100, using the existing
`px_pokemon_growth_value` implementation. The first unsigned threshold strictly
greater than the captured growth ends the scan. The result is the preceding
level, with a minimum of one; passing every threshold yields 100. Level zero
is never queried. Equality passes the threshold, and a nonmonotonic synthetic
table is consumed in its original order without sorting or a binary search.

Each threshold lookup reloads the selected growth-table pointer at logical
`0x005d17ac` and reads `table + 8 + 4 * level`. Actual resource loading remains
external. The callback must leave valid selected storage available, and its
changes to other logical memory remain visible to subsequent operations.

The object-level helper checks auxiliary presence through the current manager,
then reloads `object+0x0c`. If an auxiliary buffer is present, it reads its
level byte through `px_read_pokemon_extra_level`; that byte is not independently
clamped or raised to one. Otherwise it separately reads species, form, and
growth, reloading the object's manager before each later getter, and calls the
growth-to-level resolver with the captured values.

The object-max-HP helper performs the same presence-check/reload pattern. With
an auxiliary buffer it reads the existing maximum-HP getter from the seventh
milestone. Without one it captures species, calls the object-level helper,
reads HP IV and EV with separate manager reloads, rereads form, selects the
captured species/current form through the personal-resource service, and
requests attribute zero. It then calls the HP formula with level narrowed to
a byte and base/IV/EV narrowed to halfwords. Species remains captured across
the nested level calculation and both personal-resource callbacks.

## HP update ordering

The HP updater retains all distinct calculations in `0x0013aa84`:

1. Capture the old maximum using the object-max-HP helper.
2. Check auxiliary presence again and reload the manager. Capture current HP
   through its getter when an auxiliary buffer exists; otherwise calculate
   an initial HP value using the full sequence described above.
3. Independently calculate a new maximum using fresh species, object level,
   IV, EV, form, and personal-resource operations. Reload the manager and write
   the result as maximum HP.
4. If the complete mode word is nonzero and the captured initial/current HP
   was zero, return. The new maximum has already been written.
5. Otherwise call the object-max-HP helper again. If this freshly observed
   maximum is at least the captured old maximum, add their unsigned difference
   to the captured initial/current HP, with 32-bit wrapping. If the observed
   maximum is smaller, clamp current HP to it only when current HP exceeds it.
6. Reload the manager and write the low 16 bits of the adjusted current HP
   through the existing current-HP setter.

The final calculation uses the freshly observed maximum, not a cached scalar
result from step 3. Mode is tested for zero/nonzero, not just its low bit.
Neither the original maximum nor initial/current HP is refreshed after later
resource callbacks. A missing auxiliary buffer does not skip the top-level
calculations; individual auxiliary setters retain their own zero-pointer early
returns. Resource callbacks may replace the object's manager or attach an
auxiliary buffer, so subsequent reloads can select a different destination.

## Five stat updates and captured inputs

Each ordinary stat updater calls the object-level helper, then separately
reads its IV, EV, and nature, with fresh manager loads. It immediately captures
nature's low byte. It subsequently reads species and form, selects their
personal resource, and requests the stat's attribute from the table above.
The scalar formula receives the earlier level's low byte, base/IV/EV low
halfwords, and captured nature. Finally the routine reloads the current manager
and invokes its auxiliary setter.

Personal-resource callbacks therefore can change the final destination or
affect later updates, while previously captured IV/EV/nature/level values remain
the inputs to the current calculation. The source does not consolidate the
five updates into one resource selection or shared set of input reads. Their
caller still controls the sequence, including the seventh milestone's original
Attack, Defense, Special Attack, Special Defense, Speed order.

## Added field accessors

The new IV and EV getters use the concrete codec and block selectors from the
payload milestone. A/B are logical blocks, independent of physical permutation.
Each getter decodes its captured manager, selects its block in mode zero,
captures the result, then attempts encoding before returning that saved value.

| Stat | IV getter | B word `+0x34` shift/mask | EV getter | A byte offset |
| --- | --- | --- | --- | --- |
| HP | `0x0016b318` | Shift 0, mask 31 | `0x0016b2ec` | `+0x16` |
| Attack | `0x00152d20` | Shift 5, mask 31 | `0x00152c94` | `+0x17` |
| Defense | `0x00152d54` | Shift 10, mask 31 | `0x00152cc0` | `+0x18` |
| Speed | `0x00152cec` | Shift 15, mask 31 | `0x00152c68` | `+0x19` |
| Special Attack | `0x00152de0` | Shift 20, mask 31 | `0x00152d88` | `+0x1a` |
| Special Defense | `0x00152e14` | Shift 25, mask 31 | `0x00152db4` | `+0x1b` |

`px_read_pokemon_nature` (`0x00152c3c`) follows the same codec sequence and
reads A byte `+0x14`. `px_pokemon_has_extra` (`0x0016b348`) simply tests
manager word `+4` for nonzero and returns zero or one, without decoding.

`px_read_pokemon_current_hp` (`0x00152ea8`) reads an unsigned halfword at
extra `+8`; `px_read_pokemon_extra_level` (`0x00176268`) reads an unsigned byte
at extra `+4`. The six result setters write the low halfword at the auxiliary
offsets in the earlier table. Each of these auxiliary accessors checks manager
`+4` first and returns zero or does nothing when it is zero, before decoding.

Otherwise it decodes, reloads manager `+8`, and tests corruption mask `0x0004`
in that data header's halfword `+4`. A clear bit selects a fresh auxiliary
pointer load. A set bit selects **read fallback `0x00648284`** for current HP
and level, and **write fallback `0x00648380`** for all result setters. Their
offsets are applied to those distinct bases. Encoding follows the access;
the corruption fallback does not bypass that operation.

## Host contract, evidence, and remaining work

The existing `px_data_api` is the complete context for the orchestrators; this
milestone introduces no new resource-callback table. Personal operations use
`api->memory->parameters->select_species` and `personal_value`, with that
parameter API's user context. Growth selection uses `api->select_growth` and
`api->user`. All ordinary memory operations use `api->memory` and its own user
context. Callback tables must remain valid throughout their use.

All game addresses are unsigned 32-bit logical addresses handled through
ordinary little-endian memory adapters. Required selected buffers, fallbacks,
permutation/nature tables, and growth thresholds must be valid. No address is
converted to a native pointer. Services may mutate game memory through their
contexts; the reviewed capture/reload timing determines later observations.
The contract excludes asynchronous mutation, memory-mapped I/O side effects,
ARM instruction timing, faults, original register ABI, and physical stack
layout. No additional scratch array is required by this milestone.

Fresh read-only exports include implemented routines and their supporting
dependencies. ARM listings establish widths, arithmetic, condition codes,
fallthroughs, and captures; generated C is supporting evidence. The shared
nature tail at `0x001689fc` is included in the `0x00150034` listing rather than
represented as a separately exported Ghidra function. The exporter does not
repair or invent a function boundary there. Provenance verification covers
instruction and literal bytes against the original input; it does not verify
the nature-table contents or supply real resources. The recorded provenance
and validation files describe the actual checked ranges and commands.

The remaining state services include learnset selection and queries, move
insertion, name construction/serialization, and maximum-PP calculation.
Actual personal/growth resource loaders and data, earlier RNG/allocator
services, encounter ownership, battle execution, and platform dependencies
also remain outside this milestone. More static reconstruction, synthetic
integration, and eventually original-execution comparisons are required before
claiming complete data or gameplay behavior.
