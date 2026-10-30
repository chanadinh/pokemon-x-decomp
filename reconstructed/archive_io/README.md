# Archive member access and file streams

This sixteenth host-source milestone reconstructs the archive service that the
learnset, move-resource and growth milestones reached through a
`read_member` callback, plus the seek used by message loading. Nine routines
were reviewed through live REA MCP/Ghidra and ARM instructions:

| Address | Host routine | Behavior |
| --- | --- | --- |
| `00138214` | `px_read_archive_member` | Look up member information, then read the whole member |
| `001381c4` | `px_seek_archive_member` | Seek to a member offset plus a 32-bit intra-member offset |
| `00127c5c` | `px_archive_read_at` | Lock, seek, read, report the count, unlock |
| `00127b6c` | `px_archive_read` | Check the archive object, read through its stream, report failures |
| `00127ccc` | `px_archive_member_info` | Copy a 12-byte resident information row, or use the external path |
| `00133634` | `px_file_stream_read` | Repeat platform reads, advancing a 64-bit position |
| `001336bc` | `px_file_stream_seek` | Begin/current/end origins over a 64-bit offset |
| `0013358c` | `px_file_stream_set_position` | Validate a position against a cached, refreshed file size |
| `00133130` | `px_file_get_size` | Reject a null handle, otherwise call the size virtual |

Names are analyst interpretations; pointers are logical ARM32 addresses. The
existing callback signatures of the learnset, move-resource and growth
milestones match `px_read_archive_member`; the message milestone's seek and
read callbacks match `px_seek_archive_member` and `px_archive_read`.

## Layouts

An archive has an object at `+8` whose virtual `+0x28` reports whether reading
may proceed, a file stream at `+0x18`, a u16 member count at `+0x34`, a
resident information table at `+0x3c`, a signed resident flag word at `+0x4c`
and a critical section at `+0x60`. Information rows are 12 bytes: member size,
a word these routines do not use, and the absolute member offset.

A file stream has its handle word at `+0` (bit 0 is cleared before use), a
signed 64-bit position at `+4` and a signed 64-bit cached size at `+0x0c`.

## Observed behavior

Member information reads the flag word; a negative value takes the external
non-resident path `00127d3c..00127ea8`, which constructs reader objects and
allocates. Otherwise it rereads the flag, reads count then table, and requires
the full-width index to be below the u16 count. An index at or above the count
never returns: the original loop rereads the hook word `005d1144` and calls a
nonzero hook with message `00563574`, forever. Rows are read as two words then
one, and all three words are returned.

A member read takes the archive lock, seeks with the sign-extended 32-bit
offset and origin zero, and reports a failed seek through `00124504` at site
`00127ca4`, then **still reads** from the old position. The read checks the
archive object first and returns zero without reading when it reports zero. A
failed stream read is reported at site `00127bcc` and returns zero. The count
is stored only when the out pointer is nonzero, before unlocking. A member seek
takes no lock: it adds the intra-member offset to the member offset as 32-bit
values, sign-extends the sum and reports failure at site `00138208`.

Stream reads stop on a zero-length request, a complete read or a zero-byte
platform read. Each platform read starts at the stored position; the position
is reread and advanced by the sign-extended count after each success. A failed
platform read returns its result without storing a count. Positions with the
sign bit set fail with `e0e046c1` before any size query. A position below the
cached size (signed) is stored directly. Otherwise the size is queried; a
failure clears the cached size and returns that result, and success stores
the high word, then the low word, before checking the position against it. A
seek with origin 1 adds the stored position, origin 2 refreshes and adds the
size, and other origins return `e0e046bd`. A null handle yields `e0e046bc`.

## Validation

`make -C reconstructed/archive_io test` runs 460 stream cases: signed 64-bit
bounds, cache refresh order and failures, every origin, chunked and short reads,
read failures and sign-extended position updates. It runs 64 archive cases:
resident rows, the panic loop (escaped by `longjmp` in the test), the
external information path, the open check, lock order, failed-seek/read sites
and offset wrap. An integration fixture drives the reconstructed growth selector
and level resolver through member lookup, locked seek and chunked platform
reads. ASan/UBSan pass; ten mutations of observed order, width and bound rules
are each detected.

The optional original-data sweep builds resident information rows for every
GARC archive under `romfs/a` and reads each member through the reconstructed
path, with platform reads limited to varying chunk sizes. All 271 archives,
44,512 members and 43,228 seek-plus-partial reads match the independent native
GARC reader, covering 1,589,162,407 bytes. See [reproduction](reproduction/README.txt).

## Limits

Archive construction, the non-resident information path, the critical section,
platform read/size virtual calls, the fatal reporter `00124504` and the panic
hook remain external. Resident rows in the original-data sweep are built from
the GARC table rather than by the game's initializer, so the meaning of the
unused middle word is not established. The source assumes valid ordinary
logical memory and destination buffers. Static evidence and host/file
comparisons do not establish original ARM execution or running-game equivalence.
