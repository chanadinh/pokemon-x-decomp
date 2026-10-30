# Growth-resource selection and caching

This fifteenth host-source milestone reconstructs the growth-rate resource
service that the stats milestone previously left behind its `select_growth`
callback. The four Ghidra function bodies (five host routines) were reviewed
through live REA MCP/Ghidra and ARM instructions:

| Address | Host routine | Behavior |
| --- | --- | --- |
| `00167db0` | `px_select_growth_resource` | Select species/form personal data, read its growth rate, then find or load/store the growth record |
| `00174cac` | `px_store_cached_growth_resource` | Suppress duplicate rates and replace the cursor's slot |
| `00175fec` | `px_find_cached_growth_resource` | Full-width rate search and copy into the destination |
| `00174dd8` | `px_load_growth_resource` | Read the archive member only when the stored rate differs |
| `0017c5b0` | `px_copy_growth_resource` | Copy 404 threshold bytes and the rate word, preserving the vtable |

Names are analyst interpretations; pointers are logical ARM32 addresses.
Ghidra places the tail-called store `00174cac` in the non-contiguous body of
`00167db0`; the host keeps it as its own routine. The base literal
`005d1790` gives three cells: a second personal descriptor at `005d17a8`, the
selected growth record at `005d17ac` (already read by threshold lookup
`00167f98`) and the growth cache at `005d17b0`. The archive handle cell is
`005e52b0`.

A growth record has a vtable at `+0`, a full rate word at `+4` and 101 inline
u32 experience thresholds at `+8`. A cache has a slot-pointer table at `+4`,
a capacity byte at `+8` and a cursor byte at `+9`, the same layout as the
move-resource cache.

## Observed sequence

`00167db0` reads the personal-descriptor and growth-record cells with one
`ldmia` before anything else. It then calls the reconstructed personal
selector `00174c0c`, rereads the descriptor cell and requests selector `0x17`
(the byte at payload `+0x15`). That value is the rate, without range checks.
The cache cell is read for the lookup. On a hit the slot is copied into the
growth record and the function returns. On a miss it calls the loader with the
captured record, rereads the cache cell and tail-calls the store routine.

The lookup rereads capacity after each miss and reloads the slot pointer
before copying a match. Rates are compared as full 32-bit words. The store
routine captures the source rate once, scans for a duplicate with the same
reloads, then copies the source into the cursor's slot and writes
`(cursor + 1) % capacity`, rereading cursor and capacity after the copy.
A miss with zero capacity divides by zero in the original; host callers must
supply a positive capacity, as for the move cache.

The loader compares the stored rate word with the requested rate. Only when
they differ does it read the archive handle and call `00138214` with the rate
as member index and `record + 8` as destination, then store the full rate
word. The archive callback may change logical memory before that store.

The copy calls `00144f34`, a word-aligned forward transfer. For length `0x194`
it moves twelve groups of eight word loads followed by eight word stores,
then one four-word group and one final word. The host reproduces that
grouping for this length only, including overlapping layouts. It then copies
the rate word, after the payload. The division helper `0015e958`'s remainder
is represented with `%`. Both helpers are recorded as reviewed dependencies.

## Validation

`make -C reconstructed/pokemon_growth test` checks the grouped transfer trace
and 13 overlap layouts, 84 lookup and 243 store cases (capacity, cursor,
duplicate, full-width and reload behavior), 18 loader cases and eight
selection cases through the reconstructed personal service. They include
the order of cell reads. An integration fixture connects the selector to the
reconstructed `px_pokemon_level_from_growth` and `px_pokemon_growth_value`
in place of the stats callback. ASan/UBSan pass; mutations of the five
observed reload/order rules are each detected by the suite.

The optional original-data sweep reads `a/2/1/8` and the eight 404-byte
curves in `a/2/1/7`. All 1,607 species/form selections load the curve named
by the independently decoded personal growth byte. It checks 163,914
threshold reads, including the level cap, and 482,100 level resolutions
against an independent scan, through a three-slot cache with 78 archive
loads. Original resources remain local. See [reproduction](reproduction/README.txt).

## Limits

Archive access `00138214`, allocation, record/cache/descriptor construction and
lifetime remain external. The source assumes valid, aligned, initialized
ordinary logical memory and a valid archive member for every rate it is asked
to load. The meaning of curves 6 and 7, which are unused by personal data,
is not inferred. Older fixtures keep their callbacks. Static evidence and
host/resource checks do not establish original ARM execution or running-game
equivalence.
