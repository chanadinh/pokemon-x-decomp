# Move resources, reviewed through REA MCP

This twelfth host-source milestone reconstructs six routines from the original
Pokémon X main executable. Analysis used live `tools/call` requests to REA 6.8.0
with Ghidra 12.1.4, ARMv6, and a fresh ephemeral analysis database. The prior
eleven reconstruction directories are unchanged.

| Address | Host routine | Behavior |
| --- | --- | --- |
| `00168bbc` | `px_get_move_resource_value` | Find/load/store, then dispatch a field from the selected descriptor |
| `00176328` | `px_move_resource_field` | 33 selectors, signed bytes, nibble/halfword reads and special accuracy handling |
| `001762b4` | `px_find_cached_move_resource` | Full-width ID search and a deep copy into the destination |
| `00174eb4` | `px_store_cached_move_resource` | Suppress duplicates and replace the cursor's slot |
| `00174f30` | `px_load_move_resource` | Normalize IDs >=618 to zero; load only when the stored ID differs |
| `0017c5d8` | `px_copy_move_resource` | Copy 36 payload bytes and the move ID, preserving descriptor ownership |

The descriptor has a move ID at `+4` (u16), padding at `+6`, and a payload
pointer at `+8` (u32). Its vtable and payload pointer stay with the destination.
The cache has a pointer table at `+4`, **u8** capacity at `+8`, and **u8** cursor
at `+9`. A hit does not change replacement order. A new entry copies into the
cursor's existing buffer and advances `(cursor + 1) % capacity`. There is no
separate validity flag in the reviewed lookup.

The selected descriptor and cache cells are `005d188c` and `005d1890`.
The archive cell is `005e5338`. The wrapper searches with the full requested
ID; normalization happens in the loader. The wrapper's initial range checks
contain disabled assertion instructions and do not clamp its saved arguments.
Even an invalid field selector still runs the cache/load path before returning
zero. Archive callbacks can change logical memory: the wrapper keeps its
captured descriptor for loading/storing, reloads the cache after loading, and
reloads the selected descriptor before field dispatch.

The field decoder returns the ARM `r0` value. Ghidra's `void` wrapper and
`ulonglong` field-return guesses are retained as unreviewed pseudocode, not
used as C signatures. Selector 6 and selectors `13..15` sign-extend bytes;
8/9 select the high/low hit-count nibbles. Selectors `0b` and `1c` read u16.
Selectors `19/1a` retain positive recoil/healing components; `1e/1f` return the
magnitude of negative components. Selector 4 maps accuracy 101 to 100, while
`20` reports that sentinel as a boolean. Selector `1d` and out-of-range
selectors return zero without dereferencing a descriptor. Move flag decoding
is outside these selectors.

Run the synthetic tests with `make -C reconstructed/pokemon_move_resource test`.
They cover 8,448 byte/selector combinations, 131,072 halfword values, invalid
selectors with unmapped descriptors, and all cursors of capacities 1, 2, 3, 4,
31, 127 and 255. Cache tests cover deep copies, descriptor padding, duplicate
suppression, empty lookup and full-width ID misses. Loader tests cover fallback,
reuse, cached restoration and callback mutations. Another 3,090 cases connect
the service to reconstructed maximum PP through `px_move_api.move_value`.
ASan/UBSan pass with leak detection disabled. The optional original-data check
compares all 20,394 selector results on 618 moves with the existing independent
native decoder, including 174 accuracy sentinels and 11 negative priorities.

`evidence/` retains complete live REA observations for the six core routines
and byte reads, exported assembly/pseudocode, and provenance. All 928 core
instruction bytes and 148 literal/jump-table bytes match both REA's live memory
reads and the pinned original executable. Dependency extracts describe the
copy and unsigned division helpers; those helpers are not additional host
reconstructions. See [reproduction](reproduction/README.txt) and
[REA setup](../../docs/REA_MCP.md).

Logical memory and required services must remain valid. Cache insertion requires
positive capacity and a valid cursor. Payloads must be disjoint or identical
and disjoint from descriptors/cache metadata. Byte copying models ordinary
memory; it does not preserve the library copy routine's ARM store grouping.
Archive access `00138214`, allocation, descriptor/cache initialization,
resource lifetime, and archive errors remain external. Older state fixtures
and the native viewer retain their previous move callbacks. Static analysis,
synthetic tests and archive comparisons do not establish running-game fidelity.
