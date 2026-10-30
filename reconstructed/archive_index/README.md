# Archive index construction and GARC parsing

This seventeenth host-source milestone reconstructs how an archive builds the
resident member table that the [archive milestone](../archive_io/README.md)
reads. It covers the GARC index parser, the table builder, parser lifetime,
partial member reads and the two resource-archive initializers. They were
reviewed through live REA MCP/Ghidra and ARM instructions:

| Address | Host routine | Behavior |
| --- | --- | --- |
| `00127efc` | `px_build_archive_index` | Read and parse the GARC index, build size/length/offset rows, then signal |
| `00128134` | `px_create_garc_parser` | Allocate a 0x14-byte parser with the heap's alignment |
| `00355f4c` | `px_destroy_garc_parser` | Restore the vtable word and release the parser |
| `00355f0c` | `px_garc_parser_parse` | Locate FATO, FATB and FIMB from an index buffer |
| `00495c24` / `00495c30` | `px_garc_parser_count` / `_largest` | FATO entry count; header word `+0x18` |
| `00495b60` | `px_garc_parser_span` | Selected record's `end - start` |
| `00495cbc` | `px_garc_parser_length` | Selected record's length field |
| `00495c3c` | `px_garc_parser_offset` | Header data offset plus the record start |
| `00495d40` | `px_garc_parser_length_offset` | Header field `0x10`, the index length |
| `00127eb4` | `px_read_archive_member_part` | Read bytes at an offset inside a member |
| `0011ecf8` / `0011ee68` | `px_open_resource_archive` | Create the growth or move archive once |

Names are analyst interpretations; pointers are logical ARM32 addresses.
Ghidra did not create functions at the parser's vtable (`00597728`) targets;
those methods are retained as per-instruction REA decodes plus live bytes.

## The resident table, resolved

Each 12-byte row built here is `end - start`, the record's length field, and
the archive data offset plus `start`. The previous milestone read the first
word as the member size and left the middle word unexplained. That first word
is therefore the padded span between record boundaries. In the original
archives 14,782 rows have a span larger than the length field, so member reads
return the padded bytes. The middle word is the unpadded length.

## Builder sequence

The builder first tries to enter the recursive critical section at `+0x6c`
and does nothing else if that fails. It then:

1. Obtains a temporary heap through the archive heap's virtual `+0x34`.
2. Creates a parser only to ask for the index-length field offset (`0x10`),
   then destroys it.
3. Reads that four-byte length through the locked archive read, allocates the
   index buffer from the temporary heap and reads the index from offset zero.
4. Creates a second parser on the temporary heap and parses the buffer.
5. Releases an existing table and clears `+0x3c`.
6. Allocates `count * 12` bytes from the caller's heap. For each entry it
   rereads the selector word `005d1124` before each of the three row calls.
7. Stores the table, the u16 count at `+0x34` and the largest-member word at
   `+0x38`, destroys the parser, releases the index buffer and signals the
   light event at `+0x4c`.

The event starts at `-2` (unsignaled) in the constructor, so member lookups
use the external non-resident path until the table is built and the event is
signaled. A second build by the owning thread re-enters the section and
replaces the table.

The parser narrows entry indices to u16. A selector picks a record through the
entry's FATB bitmap: if the selector's bit is clear, record zero is used;
otherwise the slot is the number of set bits below it, with 16-bit counters.
Both bit tests use ARM register shifts, so only the selector's low byte
chooses the bit and amounts of 32 or more test nothing. Selectors of 256 or
more can count bits more than once and select past the entry's records. In
the original archives, 18 archives have entries with several records; most
use bitmap `0x1bd` (seven bits, the game's text-language count), so the
selector is likely a language index, though that is not established here.

The initializers check their cell (`005e52b0` growth, `005e5338` moves); when
it is empty they allocate a 0x78-byte object through the existing allocator
(`00122920`), call the constructor `00128288` with flags 1, store the result,
even a failed zero, and tail-call the builder.

## Validation

`make -C reconstructed/archive_index test` checks parsing, entry narrowing, 22
selectors including ARM shift wrap, past-record and wide-selector counting,
and 32-bit wrap (736 cases). It checks the builder over five selectors with
call order, heap choice, release order, rebuild and a failed entry (262
cases), with member and partial reads through the built table. An integration
fixture opens the growth archive from synthetic GARC bytes, builds its index,
and resolves 144 levels through growth loading. ASan/UBSan pass; twelve
mutations of observed width, shift, heap, order and offset rules are each
detected.

The optional original-data sweep builds the index of all 271 GARC archives
under `romfs/a` from their real bytes, for every selector 0..32 on archives
with multi-record entries (847 builds). All 45,824 rows match an independent
FATB reading, as do count and largest-member words. Through the built tables,
44,256 member reads match the native GARC reader's bytes. See
[reproduction](reproduction/README.txt).

## Limits

The constructor `00128288`, critical section, light event, heap allocation
and release, temporary-heap selection, and the meaning of the selector word
remain external or unestablished. The builder's four-byte stack slot is a
caller-supplied logical scratch region, and the vtable calls are direct
calls to the reconstructed methods. No validation of GARC structure is added.
Static evidence and host/file comparisons do not establish original ARM
execution or running-game equivalence.
