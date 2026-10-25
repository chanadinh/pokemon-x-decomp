Pokémon UTF-16 names in party records, 2026-10-25

New checkpoint: reconstructed/pokemon_names, four routines for shared string
copying, species/form index resolution, name-building orchestration and manager
name writing. The resource/message loader remains external. Ten prior source
checkpoints and all previous native reproduction records stay unchanged.

The viewer retains the original UTF-16 species-text lines from a/0/7/4 member
80, alongside its rendered display strings. New battlers copy a validated plain
name through reconstructed 001679f4 into a 13-unit field (12 plus zero).
Saving maps that name to logical memory and calls reconstructed 001685c0:
decode, writable block B, shared copy, encode. Loading reads the 13 units and
validates them before rebuilding the native name. The file stays PXSV version 1
with the same 36-byte header and 0x104-byte party slots.

The shared original copy routine handles control tokens and can exceed a name
field. Native guards require a terminator within 13 units and reject 0010
controls before invoking it. UTF-16 units are kept exactly, including private
glyphs and surrogate pairs; padding after the first zero is normalized to zero
on load. Earlier viewer saves with empty names retain empty names. Missing or
placeholder text leaves a newly created battler's stored name empty. The UI
continues to use the existing display-name table; this does not add renaming.

Original-data check (keep generated records in ignored outputs/)

  source tools/native-sdl/activate.sh  # this WSL setup only
  make -C native build/check_names
  native/build/check_names ROMFS CODE_BIN outputs/new-name-records.bin
  python3 -I native/reproduction/name-services/verify_names.py \
    --romfs ROMFS --code CODE_BIN --records outputs/new-name-records.bin

The output-record path must not exist. check_names.c checks 724 name-builder
requests through a native text resource callback, then generates one party
record for every base species 1..721, including all 32 selector rows. Each
round-trips through native save/load byte for byte. The separate Python check
reads the text archive using the earlier independent GARC reader, decrypts its
text units itself, independently unshuffles/decrypts every generated PK6 record
and checks names, zero padding, species, checksum, move boundary and nickname
flag. Only case counts and hashes are retained here; original names and binary
records stay local. The compiler/text/guest/record lifecycle is host behavior,
not execution of the original ARM game.

Results: 800 text lines, 724 name-builder requests, 721 saved names, maximum
name length 11 UTF-16 units, all 32 selector rows. All independent comparisons
agree. The native name guard separately checks all 800 lines: 722 plain names
are accepted, 78 placeholder/control lines are rejected and remain empty.

The existing bch-save/verify_save.py scenario also passes with the current
viewer on Route 2: catch, save, reload and save again are byte-identical.
Pass --viewer-save PATH to this record's verify_names.py to compare that
scenario's stored names against the original text too. Both party names in
the recorded Route 2 save match independently. Use a fresh --out directory
when reproducing the earlier save scenario; its old evidence stays unchanged.

Tests and effect on earlier records

New reconstructed tests cover plain units, token copying and guard behavior,
index boundaries, builder capture/reloads, writer codec states/permutations,
corruption fallback and integration with earlier state/move/learnset routines.
Native battle/save tests cover Unicode units, aliased input, guard failure
without mutation, 12-unit names stopping before moves, independent stored-name
reads and round-trip preservation. Valid-checksum records with unterminated
names or a control token are rejected as names. ASan/UBSan checks use
detect_leaks=0 in this environment.

Newly generated save bytes change because the previously empty name field is
now populated. Older records and evidence files are historical and unchanged.
The stored name is not a complete reconstruction of nickname mechanics,
language selection, message-resource caching, font rendering or the game's
save blocks. Captures and text output keep their existing display-name path.
Native Windows and captured running-game comparisons remain unverified.
