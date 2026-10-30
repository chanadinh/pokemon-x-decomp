# Message-resource construction and move-resource boot

This nineteenth host-source milestone reconstructs how message resources are
built, opened and destroyed, and how the move resources are created at boot.
Together with the earlier resource milestone, the boot path now constructs
every object the message, move, personal, growth and learnset services read.
Eight routines were reviewed through live REA MCP/Ghidra and ARM instructions:

| Address | Host routine | Behavior |
| --- | --- | --- |
| `0011e264` | `px_construct_message_resource` | Initialize the 0x28-byte object, construct its archive, open the member |
| `0011e308` | `px_construct_listed_message_resource` | The same, using the path-table archive constructor `0011c3e8` |
| `0012ad74` | `px_open_message_member` | Read the text header and offsets, select the language, preload a cached section |
| `0038fc20` / `0038fbc4` | `px_destroy_message_resource` / `px_delete_message_resource` | Release section and header, delete the archive; the second also frees the object |
| `00112894` | `px_initialize_move_resources` | Open the move archive, create its message resource, record and cache |
| `0012b968` / `0011edec` | `px_construct_move_record` / `px_construct_move_cache` | 36-byte move payload; byte-capacity cache of move records |

Names are analyst interpretations; pointers are logical ARM32 addresses.
Reads and seeks go through the reconstructed archive milestone
(`px_seek_archive_member`, `px_archive_read`), so every byte a constructed
message resource loads now passes through reconstructed code.

## Construction and opening

The constructor clears the archive word, stores vtable `005985b4` and the heap,
stores the mode byte at `+0x0c` and clears `+0x10`–`+0x1c`. It allocates the
0x78-byte archive with the heap's alignment, constructs it with the caller's
id and flags (the boot passes 5, which also builds the archive index), stores
it at `+8`, and opens the member.

Opening requires a nonzero archive and otherwise enters the abort loop before
writing anything. It clears the entry scratch (`+0x20` word, then the `+0x24`
and `+0x26` halfwords), seeks to the member, and reads its 12-byte header.
It allocates `0x0c + 4 × sections` bytes, copies the header and reads the
section offsets. It then stores the header (even when allocation failed, in
which case later reads use address zero) and the member. The language comes
from `005d113c`, reset to zero when the section count does not exceed it; the
mode byte is loaded before that store.

In streamed mode (`+0x0c` zero) the section pointer is cleared. In cached mode
it allocates the header's largest-section size and rereads the language,
header, archive and member fields. It seeks to the selected section, reads its
four-byte size into the buffer, seeks back and reads the whole section using
that size. With no archive at that point it would point into the header
instead.

The destructor rereads the archive word, restores the vtable and, only when an
archive exists, releases the section then the header through `0013a50c` and
deletes the archive through its virtual `+4`. The deleting form then frees the
object through `002ff520`.

## Move-resource boot

`00112894` opens the move archive into `005e5338` from configuration word 12
(the shape of the reconstructed `px_open_resource_archive`). It then creates
the move message resource at `005d1888` from words `0x11` and `0x15` in cached
mode with flags 5, the move record at `005d188c`, and the move cache at
`005d1890` with word 13's low byte as its capacity. These are the cells the
move-resource milestone reads.

## Validation

`make -C reconstructed/message_construction test` builds synthetic two-language
text members in a GARC. For both modes, four languages and six members it
checks construction, entry-scratch clearing, the header copy, language
fallback and the cached section. Every line loaded through the constructed
resource matches a hand-built resident copy of the same member (195 cases).
It also checks the archive abort, a failed header allocation, the listed
constructor, destructor release order and object free (3 cases). The move boot
is checked with the reconstructed move-resource service and the capacity mask
(24 cases). ASan/UBSan pass; eleven mutations of observed bounds, order, masks,
config words and constructors are each detected.

The optional original-data sweep constructs every non-empty text member of
`a/0/7/4` and `a/0/8/2` in both modes (575 members, 1,150 constructions). All
70,424 lines and 3,352,056 UTF-16 units loaded through the constructed
resources match the independent native text decoder. Each resource is then
destroyed, with 1,725 buffer releases and 1,150 archive deletions. Booting the
move resources from `a/2/1/2` gives 3,090 field reads across all 618 moves,
all matching the raw records. See [reproduction](reproduction/README.txt).

## Limits

The archive constructors `00128288`/`0011c3e8`, block release, object free and
OS services remain external, as does the configuration table's content. The
header copy uses a caller-supplied scratch region for the original stack
buffer. Static evidence and host/file comparisons do not establish original
ARM execution or running-game equivalence.
