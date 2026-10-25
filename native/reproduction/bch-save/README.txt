Save and load: Route 2, 2026-10-25

The field slice can now be saved and resumed. F5 (or "s" in --play) writes
the zone, the player's tile and facing, the party, the Poké Balls and the
random state to a file; --load starts from it. Each party Pokémon is stored
as the game's own 0x104-byte party record, written and read through the
reconstructed codec of reconstructed/pokemon_data (its checksum, block
order and XOR stream, and its field setters). Tracked files contain
independently authored source, synthetic tests and metadata only.

Native preview

native/build/pokemon_model_viewer --romfs /path/to/romfs --zone 259 \
  --code-bin /path/to/code.bin --save my.sav       (F5 saves)
native/build/pokemon_model_viewer --romfs /path/to/romfs --load my.sav \
  --code-bin /path/to/code.bin                     (F5 saves back to it)

--save and --load need --code-bin: the record block order is the table at
code.bin 0x00570eec. --load replaces --zone and may not be combined with
--partner, --partner-level, --balls, --seed or --arrive. During a battle,
F5 or "s" saves once the battle is over.

The file (game/save.h; the viewer's container, not the game's save data)

36-byte header ("PXSV", version 1, party count, zone, world tile x and z,
facing, balls, random state, trainer id 0), then six 0x104-byte slots: an
encoded 0xe8-byte record and its 0x1c-byte party extension, zero when
unused (1,596 bytes in all).

What the records hold

Through the reconstructed setters (in the population routine's order): the
key (encryption constant), trainer id, species, party level, nature, the six
IVs, the ability (the species' first, from a/2/1/8) and its slot flag, and
the experience. Written directly into the decoded blocks: moves (B+0x1a),
PP (B+0x22), current HP (extension +8) and the six stats (extension +0x0a).
The PK6 offsets of PKHeX locate the same fields (species 0x08, experience
0x10, nature 0x1c, moves 0x5a, PP 0x62, IVs 0x74, level 0xec, HP 0xf0,
stats 0xf2). Everything else (nickname, met data, EVs, gender, friendship,
PID) stays zero; the viewer does not model it.

Policy

- The encryption constant: derived from the record's contents (FNV-1a over
  species, level, nature, IVs, experience, slot and trainer) the first time
  a Pokémon is saved, then kept. Not the game's PID or constant generation.
- Loading rebuilds each Pokémon from species, level, nature, IVs and moves
  with the reconstructed stat routines; a record whose checksum fails, whose
  stored stats disagree, or whose HP or PP exceed their maximum is refused.
- The file is written to PATH.tmp and renamed over PATH.
- The partner is made when first needed: the first encounter, or a save.

Independent comparison (verify_save.py)

python3 -I -B native/reproduction/bch-save/verify_save.py --romfs /path/to/romfs \
  --code-bin /path/to/code.bin --viewer native/build/pokemon_model_viewer \
  --out outputs/new-save-check

On Route 2 the viewer catches Scatterbug and saves. Read independently with
PKHeX's PK6 conventions (no native code), both records' checksums hold and
their fields at the PK6 offsets equal what the viewer printed for the
partner and the wild Pokémon (species, nature, IVs, stats, moves, level, HP),
the ability is each species' first and each level follows from the
experience and the growth table. Encoding those records again independently
gives the same bytes. Loading prints the same party, and loading then saving
at once rewrites the file byte for byte (reference-comparison.json).

Tests

game/test_save.c (round trip, the stored layout read back without the
codec, re-encoding identity, damaged byte, magic, size, count, facing,
tampered stats and HP, empty party, species out of range),
tests/test_bch_save_viewer.py (option contract, a save where the walk
starts, the record read independently, load placement, resuming repeats the
saved run's encounters exactly, a save deferred until after a catch, a
damaged file refused).

Limits

Not the game's save file (no main save blocks, PC boxes, bag, flags,
trainer card); the fields the viewer does not model are zero, so the
records are not complete game Pokémon. No running-game comparison.
