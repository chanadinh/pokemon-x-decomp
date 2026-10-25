# Pokémon learnset access and selection

This tenth host-source milestone reconstructs the learnset count/entry wrappers,
their accessors, form-aware loading, cache lookup/replacement and record copying.
It preserves all nine earlier reconstruction checkpoints. Names describe observed
roles; they are not recovered original symbols.

```sh
make -C reconstructed/pokemon_learnset test
make -C reconstructed test
```

## Reviewed routines

| Original address | Host implementation |
| --- | --- |
| `0x0014f294` | `px_current_learnset_count` |
| `0x0014f2a4` | `px_current_learnset_level` |
| `0x0014f310` | `px_current_learnset_move` |
| `0x0016b1e0` | `px_learnset_record_count` (tail in the count wrapper export) |
| `0x0016b1e8` | `px_learnset_record_level` |
| `0x0016b224` | `px_learnset_record_move` |
| `0x0014f2b8` | `px_select_pokemon_learnset` |
| `0x0016b25c` | `px_find_cached_learnset` |
| `0x00167e44` | `px_store_cached_learnset` (tail in the selection export) |
| `0x00167ca4` | `px_load_learnset_record` |
| `0x00175fb4` | `px_copy_learnset_record` |

The record has species as a halfword at `+0`, form as a byte at `+2`, a
32-bit buffer pointer at `+4` and count as a byte at `+8`. Its payload consists
of four-byte entries: unsigned halfword move, unsigned halfword level.
The 12-byte host descriptor covers these reviewed fields with alignment space;
the original allocation size and unrelated bytes are not established here.
The global object at `0x005d1790` holds the current record pointer at `+0x24`
and cache pointer at `+0x28`; each public access wrapper reloads the record.
The record accessors compare the full unsigned index with the count and use
**entry zero on any out-of-range index**, including when the count is zero.
They do not return zero or narrow the incoming index to 16 bits. The caller
must provide storage for entry zero even on an empty record.

## Loading and forms

Loading first compares stored species/form with the full incoming words and
returns immediately if both match. Otherwise species `0x2d2`, `0x2d3` or any
value above `0x2d1` select archive entry zero. Base forms select the species
entry directly. For a nonzero form of a supported species, the routine:

1. Reloads the personal-record pointer at `0x005d17e8` and selects the base
   species through `0x00174c0c`, passing form zero.
2. Reloads that pointer separately for selectors `0x1e` and `0x20` through
   `0x00175c78`, capturing the first result across the second call.
3. Uses `first + form - 1` when the first result is nonzero and the second
   result is unsigned-greater than the form. Otherwise it uses the base species.

The selector switch in the original code confirms that `0x1e` reads unsigned
personal halfword `+0x1c`, and `0x20` reads unsigned byte `+0x20`. The verifier
checks both switch-table entries as well as their retained instruction bytes.

The destination pointer is loaded from the captured record before the archive
handle is reloaded from `0x005d17ec`. Service `0x00138214` reads that member
into the destination. The routine then **reloads the record's buffer pointer**
and counts up to 26 entries, stopping only when both halfwords are `0xffff`.
It writes count, low-halfword species, then low-byte form in that order.
The resource callback may change pointers; the reviewed capture/reload order
is preserved. No synthetic resource-size check is added inside the reconstruction.

## Cache and copying

The cache has a pointer table at `+4`, unsigned slot count at `+8`, and
replacement cursor at `+0x0c`. Lookup scans those slots for full-word species
and form matches against the stored halfword/byte values. A hit copies the
record to the captured current destination and returns one. A miss returns zero.

Copying moves all 104 payload bytes into the destination's existing buffer,
then copies species, form and count. Buffer pointers and descriptor padding
are retained. Heap allocation, disposal and cache initialization are outside
this milestone. Payloads must be disjoint or identical and separate from
descriptors/cache metadata; ordinary-memory byte copying replaces the compiler
copy helper without claiming its ARM access widths or instruction grouping.

Registration suppresses any existing key without updating it or advancing the
cursor. Otherwise it copies into the cursor's slot and advances modulo the
reloaded count. Registration requires a nonzero slot count, valid initialized
slots and a valid cursor. The compiler's unsigned remainder service is replaced
by C remainder for that contract; its divide-by-zero behavior is not implemented.

Selection captures the active record and initial cache, tries lookup, loads on
a miss, then reloads the cache pointer for registration while retaining the
original record. Tests cover a callback changing both globals during loading.

## Dependencies, integration and validation

Three game resource services remain explicit callbacks: personal selection
`0x00174c0c`, personal value access `0x00175c78`, and archive member reading
`0x00138214`. The retained 14-function evidence set includes these dependencies
and compiler helpers; it does not claim their full reconstruction. All 1,924
exported instruction bytes and 36 literal bytes match the pinned executable.
Pseudocode can include shared compiler tails outside a function's exported
instruction body; no complete compiler-helper execution comparison is claimed.

Synthetic checks cover 66,816 index/count cases, 25,515 species/form/resource
cases, partial terminators, all scan lengths through the 26-entry limit,
independent cache buffers, duplicate suppression and replacement for capacities
1..8, full-width key comparisons, and callback pointer changes. Boundary tests
use some counts/resource results outside real input ranges. A base-state
initialization/refresh fixture now uses reconstructed learnset selection and
access, move insertion and maximum PP together; level, name and underlying
resource services remain synthetic in that fixture. This extends the connected
state path while retaining the older eight-module integration fixture unchanged.

The native viewer now runs the reconstructed accessors and move insertion when
choosing an encounter's initial moves. Native code still selects and decodes
the resource first; the game's cache/resource lifecycle is not integrated into
the viewer. The bridge keeps the original 26-entry limit and checks invalid
native input. The pure native helper remains available as a comparison path.

Local original data: all 799 resources, 80,699 level cases and 885 species/form
selections agree; 885 repeat selections avoid another resource read. The largest
stored learnset has 25 entries. See the [native record](../../native/reproduction/learnset-services/README.txt).
ASan/UBSan cover the new reconstruction suite and native battle/bridge tests
with leak detection disabled. Native Windows execution and comparisons against
the original running ARM game remain unverified.
