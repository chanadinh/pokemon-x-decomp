# Personal-data selection and fields

This fourteenth host-source milestone reconstructs four main-executable
routines, reviewed through live REA MCP/Ghidra and ARM instructions:

| Address | Host routine | Behavior |
| --- | --- | --- |
| `00174c0c` | `px_load_personal_record` | Resolve a species/form row and copy its 64-byte payload |
| `00175c78` | `px_personal_resource_field` | Dispatch 46 byte, halfword, word and packed-bit fields |
| `00167e2c` | `px_select_personal_resource` | Select through the current descriptor cell |
| `00167e18` | `px_current_personal_value` | Read a field through the current descriptor cell |

Names are analyst interpretations; pointers are logical ARM32 addresses.
The flat table pointer is at `005d177c`; the current descriptor pointer is at
`005d17a4`. A descriptor contains requested species u16 at `+4`, requested
form u8 at `+6`, and its destination buffer pointer u32 at `+8`.

Selection always reads the table pointer and copies a row, including repeated
requests for the same identity. Species above 721 select row zero. Valid
species with form zero select their base row. A nonzero form reads the base
row's full words at `+1c` and `+20`, retaining the low u16 first-form index
and low u8 form count. A nonzero first index and form below the count select
`first + form - 1`; other forms use the base species row. Comparisons use the
full requested values. The descriptor stores their low halfword/byte after
copying, including when the payload came from a fallback row.

The ARM transfer uses two 32-byte VFP load/store groups. The host implementation
loads eight words before storing each group, preserving the ordinary-memory
behavior when source and destination overlap. It does not replace this with
an overlap-safe library copy. Descriptor vtable, padding and destination
ownership remain unchanged. The field accessor returns zero for selectors
outside `0..45` without loading a descriptor or payload. Its global wrapper
still reads the current descriptor cell first. Both wrappers reload that cell
for each call. Ghidra's guessed `void`/`ulonglong` signatures do not define the
host return contract: fields return the ARM `r0` value.

The selectors expose base-stat/type bytes, several other scalar bytes,
halfwords, five full words, packed two-bit values from the word at `+0a`, and
the selected bits of `+21`. Exact offsets and shifts are in the source and
reviewed assembly. Those observations do not establish a semantic name for
every field. In particular, no new species/form or move-compatibility policy
is inferred beyond the observed dispatch.

`make -C reconstructed/pokemon_personal test` checks 536,064 byte/halfword/
bitfield cases and 864 selection cases spanning invalid and wide identities,
zero and nonzero form tables, form boundaries, descriptor preservation,
unconditional reload and both form-metadata word reads. Five overlapping
layouts check the two-group copy. An integration fixture replaces the
personal selection/value stand-ins in the reconstructed learnset loader.

The optional original-data sweep verifies that the 51,136-byte final personal
archive member equals all 799 individual rows concatenated. All 36,754 selector
results match the independent native decoder/field reference. It checks 1,607
valid and boundary species/form selections and 24,675 learnset entries through
the reconstructed personal and learnset services. Original resources remain
local. See [reproduction](reproduction/README.txt) for commands and evidence.

Allocation, descriptor/table construction, archive loading, lifetime and growth
resources remain external. The source assumes valid initialized ordinary
logical memory, including selected form rows and fallback row zero. It does
not validate table length or resource integrity. Descriptor/payload aliases
outside the tested layouts are not a host API guarantee. Older integration
fixtures and the native viewer retain their existing adapters. Static evidence
and host/resource checks do not establish original ARM execution or running-game
equivalence.
