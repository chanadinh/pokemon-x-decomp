# Pokémon name construction and storage

This eleventh host-source milestone reconstructs four reviewed routines. All ten
earlier checkpoint directories remain unchanged. Names are analyst choices.

```sh
make -C reconstructed/pokemon_names test
make -C reconstructed test
```

| Original address | Host implementation |
| --- | --- |
| `0x001679f4` | `px_copy_game_string` |
| `0x00167c38` | `px_pokemon_form_index` |
| `0x0014f224` | `px_build_pokemon_name` |
| `0x001685c0` | `px_write_pokemon_name` |

## Shared string copying

The original copy routine reads and writes unsigned halfwords through logical
ARM32 addresses. Plain units are copied through the first zero, returning the
number of nonzero units copied. It preserves code units exactly, including
surrogates, private-use glyphs and `0xffff`; it does not render or normalize text.
Destination bytes beyond the written terminator are untouched.

Unit `0x0010` starts a control token. Its next halfword supplies a count `n`.
After storing the marker, the routine copies the next `n+2` units, including
the count, arguments and **the unit immediately following the token**. Zeros
inside the token do not stop the copy. The following unit is then tested for
zero; if nonzero, the next loop starts after it. A following `0x0010` is therefore
copied as that following unit, without being interpreted as another token in
that iteration. These rules follow the odd-prefix and paired halfword loop at
`0x00167a30..0x00167a68`, rather than a generic UTF-16 string library.

The `0x800` counter guard is checked only after processing a nonzero unit or
a whole token and its following unit. On guard exit the routine writes zero
at the next destination position and returns the previous counter. For a
plain unterminated string it copies 2,048 units, appends zero and returns 2,047.
A large token can copy well beyond that threshold before the guard is checked;
this is **not a bounded Pokémon name-field operation**. Both source and
destination must have enough backing storage. Ordinary overlapping memory
follows the ascending halfword load/store order; it does not have `memmove`
semantics. Address arithmetic wraps as unsigned ARM32 values. Fault/timing
behavior and asynchronous memory mutation are outside the accessor contract.

## Species/form indices and construction

`px_pokemon_form_index` maps species `722`, `723` and all values above `721`
to index zero. A valid species with form zero returns the species without
accessing memory. For a nonzero form, it reads the table pointer at
`0x005d177c`, then full words at species-record offsets `+0x1c` and `+0x20`.
Their low halfword and low byte are first-form index and form count. If the
first index is nonzero and the full incoming form is below that count, the
result is `first + form - 1`; otherwise it is the base species. Word accesses
and full-width comparison are retained even for values outside observed data.

The name builder captures the string descriptor pointer at `0x005d1790`.
For species below `724` it resolves the index with form zero, reloads the text
resource at `0x005d1794`, and calls resource service `0x0013a19c` with the
captured descriptor. It retains that service's return value. Species `722/723`
therefore request text index zero. Species at least `724` skip loading and
set the return value to zero.

In both cases the builder reloads the **current** string descriptor, reads its
buffer pointer at `+4`, and copies it to the supplied logical destination.
An invalid species can therefore copy stale text; a resource callback can
replace the current descriptor while the original destination descriptor
remains captured for that callback. No blanket scratch clearing is introduced.

## Name storage

The name writer captures the manager/source, uses the existing concrete manager
decoder, selects writable logical block B, calls the shared string copier at
the block's start, then encodes the captured manager. The assembly export stops
at a no-op after restoring registers; execution falls through directly into
`0x001685f0`. The verifier checks that boundary explicitly.

It does not change the nickname flag, add a length limit, clear trailing name
bytes, or check for a missing buffer. Corrupt data uses the existing write
fallback block B at `0x006483d4`, and the manager codec retains its previous
corruption behavior. Actual names must fit within the 26-byte area before
the move slots at B `+0x1a`; the **native adapter** enforces that limit.

The old state API's name callbacks receive a host scratch array. The new
integration fixture adapts it to separate logical scratch memory, preserving
all 32 bytes around the call, and invokes both new routines. That fixture
connects name construction/storage to reconstructed learnset selection/access,
move insertion, maximum PP and base-state initialization/refresh. Resource
services and level resolution are synthetic there; the older integration
checkpoints remain unchanged.

## Evidence and limits

Nine read-only Ghidra exports include the four routines and five dependency
exports. All 1,692 instruction bytes and 32 literal bytes match the pinned
executable. Decoder, encoder and block selection reuse earlier reconstructions.
The resource loader `0x0013a19c` and its text-decode helper `0x0013ff50` are
retained as dependency evidence, **not implemented here**. Text loading,
language/resource initialization and allocation remain external. The latter
helper corroborates the line-key multiply/rotate sequence used by the earlier
native decoder; no complete original message-loader reconstruction is claimed.

Synthetic tests cover 65,535 non-token UTF-16 unit values, 2,051 plain string
lengths, 128 control-count/prefix/following-unit combinations, adjacent markers,
overlap, 945 form-index combinations, builder threshold/reload cases, and 3,328
name-writer cases across all selector rows, acquired/encoded states and corrupt
fallbacks. Long control sequences stress the original guard rather than assert
that those sequences fit a real name field. The new suite passes ASan/UBSan
with leak detection disabled.

Native battlers now keep validated UTF-16 names, and save/load writes them
through this reconstructed name writer. All 721 base-species name records match
an independent text/PK6 decoder and round-trip byte for byte; the maximum name
length is 11 units. See [the native record](../../native/reproduction/name-services/README.txt).
The new code does not execute the original ARM game or establish running-game
equivalence. Native Windows execution remains unverified.
