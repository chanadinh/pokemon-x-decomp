# Pokémon resource initialization

This eighteenth host-source milestone reconstructs the boot-time routine that
creates the Pokémon data services' state: the personal table, archives,
descriptors, records and caches that earlier milestones read from fixed
cells. After it runs, the reconstructed personal, growth and learnset
services work from that state alone. Twenty-two routines were reviewed through
live REA MCP/Ghidra and ARM instructions:

| Address | Host routine | Behavior |
| --- | --- | --- |
| `00112368` | `px_initialize_pokemon_resources` | Open five archives, create 17 resource objects, load a final-member table |
| `0011e884` | `px_load_personal_table` | Open the personal archive, check sizes, load its flat final member |
| `0011e960` | `px_open_learnset_resources` | Open and index the learnset archive; create its personal descriptor |
| `0011e9dc` / `0011eab0` | `px_open_resource_archive` (previous milestone) | Same shape as the growth opener, for cells `005e5288` / `005d1760` |
| `00127ccc` (non-resident path) | `px_archive_member_info_streamed` | Re-read the GARC index for one member's row |
| `00127bd8` | `px_archive_member_count` | Resident u16 count, or the header's FATO count |
| `0011c3cc` | `px_archive_member_size` | Row word zero: the padded span |
| `0011e864` | `px_resource_abort` | Endless panic-hook loop |
| `001228ec` | `px_allocate_aligned` | Allocate with the heap's own alignment |
| `0012b554` / `0012b6a4` / `0012b5d4` | descriptor / growth-record / learnset-record constructors | Personal (64-byte), growth (rate `-1`, 404 cleared bytes), learnset (104 bytes) |
| `0012b610` / `0012b650` | record constructors | 0x30- and 0x18-byte payload records (roles not established) |
| `0012acd8` | `px_construct_halfword_buffer` | 0x100 u16 entries with a valid flag |
| `0011ea34` / `0011eb08` / `0011eb84` / `0011ec00` / `0011ec7c` | cache constructors | Growth, personal, learnset and the two unnamed caches |
| `00495d38` | `px_garc_parser_count_offset` | Parser vtable `+0x24`: header offset `0x24` |

Names are analyst interpretations; pointers are logical ARM32 addresses.

## Boot sequence

The configuration pointer is reread from `005d1870` for each use. Words 0, 1
and 2 select the personal, learnset and growth archives, words 3 and 4 two
further archives, word 6 a final-member archive, words 7–11 cache capacities
and words `0x11`–`0x14` the message archive and members. The resources are
stored from base `005d1790`:

| Cell | Object |
| --- | --- |
| `+0x00` | 0x100-entry halfword buffer |
| `+0x04`/`+0x08`/`+0x0c` | message resources (external constructor; mode 1, then 0, 0) |
| `+0x10` / `+0x14` / `+0x18` | personal-descriptor cache (capacity byte from word 7 masked to 8 bits); two personal descriptors |
| `+0x1c` / `+0x20` | growth record; growth cache (full word 8 slots, low byte stored) |
| `+0x24` / `+0x28` | learnset record; learnset cache (word 9) |
| `+0x2c` / `+0x30` | 0x30-byte record and cache (word 10) |
| `+0x34` / `+0x38` / `+0x3c` | two 0x18-byte records and their cache (word 11) |
| `+0x40` / `+0x44` | final-member archive (word 6) and its loaded final member |

Objects are created in that table's address order, except that the second
personal descriptor (`+0x18`) and second 0x18 record (`+0x34`) come last. A
failed allocation stores zero and skips construction. Cache slot tables use
the heap's alignment; each slot is allocated, constructed when nonzero, and
stored through a reloaded table pointer. The final member's size is compared
with `0x5a4`, but the comparison has no effect.

The personal loader opens its archive without building a resident table, so
its count and member rows come from the GARC header and index on each call. It
requires the final member to be `(count − 1) × 64` bytes and member zero to be
64 bytes, and otherwise enters the abort loop. It then reads the flat member
into a heap block stored at `005d177c`. The streamed member lookup allocates
its index buffer from the heap tail (negated alignment) and releases it after
copying the row.

Constructed learnset records and cache slots begin as species 0, form 0.
Immediately after boot, a species-0 learnset lookup therefore matches an empty
slot and loads nothing.

## Validation

`make -C reconstructed/pokemon_resources test` checks every constructor and
cache, including failed allocations, byte versus word capacities and slot
table reloads (16 cases). It checks the member count, streamed rows and tail
allocations, both personal-table aborts and learnset reopening (15 cases). The
full boot runs from synthetic GARC files and checks the exact order and size of
every object allocation, every cell and the message arguments. It then runs
the reconstructed personal, growth-level and learnset services from the booted
state (769 cases). ASan/UBSan pass; thirteen mutations of the observed masks,
checks, reloads, alignment, order and constants are each detected.

The optional original-data sweep boots the resource system from `a/2/1/8`,
`a/2/1/4`, `a/2/1/7` and `a/2/1/9`, using `a/2/1/5` and `a/2/1/6` as stand-ins
for configuration words 3 and 4. The loaded 51,136-byte personal table equals
all 799 independently decoded rows, and the final-member block equals the
1,444-byte last member of `a/2/1/9`. From the booted state alone, 884
species/form selections, 5,304 personal fields, 10,608 growth-level
resolutions and 13,435 learnset entries match independent decoders. See
[reproduction](reproduction/README.txt).

## Limits

The configuration table's contents, and therefore which archive each word
names, come from elsewhere at run time. The sweep's mapping follows the data
each consumer reads, but it is not established from code, and words 3 and 4
are stand-ins. Message-resource construction (`0011e264`/`0012ad74`), the
archive constructor, heap block allocation and OS services remain external.
Clear helpers are represented by byte stores. Static evidence and host/file
comparisons do not establish original ARM execution or running-game
equivalence.
