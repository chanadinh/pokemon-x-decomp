# Message loading and decoding

This thirteenth source milestone reconstructs three routines from Pokémon X's
main executable, reviewed through live REA 6.8.0 MCP and Ghidra 12.1.4:

| Original address | Host routine | Behavior |
| --- | --- | --- |
| `0013a19c` | `px_load_message` | Select a language section, obtain an entry, decode it and return its u16 metadata |
| `0013ff50` | `px_decode_message` | Decode encrypted or plaintext UTF-16 into a string descriptor |
| `0014e984` | `px_game_string_length` | Count units while skipping control-token arguments, with the original guard |

Names and structures are analyst interpretations. Addresses are logical ARM32
addresses. The [name builder](../pokemon_names/README.md) can now call this
implementation through its existing `load_text` service. New integration tests
connect that builder and its control-token copy to all three message-loading
modes. Earlier source checkpoints and the native viewer retain their existing
adapters.

## Layout and loading

The observed resource fields are heap pointer `+4`, archive pointer `+8`, cached
mode byte `+0c`, member ID `+10`, selected section u32 `+14`, header pointer
`+18`, cached section pointer `+1c`, and an eight-byte entry scratch area `+20`.
The header contains a u16 section count at `+0`, u16 line count at `+2`, decode
mode u32 at `+8`, and section offsets from `+0c`. Offsets are relative to the
member/header. A section begins with its size, then eight-byte entries:
relative payload offset u32, unit count u16, and returned metadata u16.

Language comes from the u32 cell `005d113c`. A value outside the header's u16
section count becomes zero; the selected value is then masked to eight bits.
An invalid full-width line index returns zero and clears an active destination
without reading the language or calling services.

The three paths preserve the original sequencing:

- A cached resource with archive pointer zero selects the section directly in
  the resident member bytes.
- A cached resource with an archive reads four bytes for the section size,
  seeks back, and reads that whole section. It repeats those reads only when
  language selection changes.
- A streamed resource seeks to the requested eight-byte entry, reads it into
  the scratch area, allocates `unit_count * 2` bytes, then seeks/reads/decodes
  the payload and releases that allocation.

Allocation captures the heap before its alignment callback, uses the negated
alignment as a 32-bit value, and requests tail allocation. Failure preserves
the previous destination text and still returns the entry metadata. Streamed
loading reloads the archive after seeks, the section selection after allocation,
the unit count after payload reading, and metadata after release. Cached section
loading retains its captured archive/member/offset across both seeks, while
reloading the destination pointer for each read. Tests exercise these mutation
boundaries, including a mode change after an entry read.

## Decoding and length

The destination string has buffer pointer `+4`, capacity u16 `+8`, stored length
u16 `+0a`, and active byte `+0c`. Capacity includes room for a terminator. Any
nonzero active byte enables writes; this includes bytes with the high bit set.
An inactive destination still causes the first source halfword to be read
when the count is nonzero, then returns without writing.

A zero header decode-mode word selects XOR decoding. The initial key is the
low 16 bits of `(index + 3) * 0x2983`; it rotates left three bits after every
unit. Any nonzero mode word selects plaintext. Units are copied unchanged after
decoding, including control arguments, surrogate units, embedded zeros and
units following a zero. The supplied count controls decoding; a zero is not an
early exit.

Each appended unit reserves a trailing zero. Filling the last available slot
is allowed only when that unit itself is zero. Overflow clears the first unit
and stored length instead of truncating. Successful decoding recalculates the
length through `0014e984`. That helper skips `0010, count, arguments...` by
`count + 2` units, so zeros within arguments do not terminate the scan. It
tests the 2,048-unit guard after skipping a complete token; its full-width
result can exceed 2,048 or 65,535. The decoder stores only the low halfword.

## Validation and remaining boundaries

`make -C reconstructed/message_resources test` runs 131,396 unit/capacity
cases, six maximum-capacity cases, all 65,536 token counts, 32 token/guard
boundaries, and 2,051 plain lengths. Further checks cover adjacent tokens,
length wrapping, inactive output, language width, cached reuse, invalid indices,
zero-size allocation, allocation failure and six callback-mutation scenarios.
The reconstructed name builder and token-copy routine run through this loader
in all three modes. All thirteen reconstructed suites and the new module's
ASan/UBSan run pass; leak detection was disabled for the sanitizer run.

An optional local-data comparison checks 35,212 lines across 575 original
English game/story text files. All 105,636 loads agree in decoded UTF-16 units,
token-aware length and entry metadata. It compares 5,028,084 units and checks
2,163 species-name builds. The existing independent native text decoder
supplies the unit reference; length expectations are parsed separately.

The evidence includes 992 instruction bytes for the three implemented routines
and 344 bytes for observed seek/read/free dependencies, plus eight literal
bytes. All match live REA memory reads and the pinned original `code.bin`.
See [reproduction](reproduction/README.txt), the validation records and
[the REA workflow](../../docs/REA_MCP.md).

Seek, archive read, allocation/alignment, release, resource construction and
language initialization remain external. The implementation assumes valid
ordinary logical memory and initialized resource/string descriptors; it does
not validate file offsets or recover from malformed resources. Length and
capacity handling follows the observed instructions, including the guard.
Required service callbacks must reproduce the relevant I/O
effects and keep subsequent memory valid. Partial overlaps and descriptor/
payload aliasing are not validated. Host tests and archive comparisons do not
establish original ARM execution or running-game equivalence.
