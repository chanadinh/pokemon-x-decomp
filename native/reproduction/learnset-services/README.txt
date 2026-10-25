Learnset access, selection and constructor integration, 2026-10-25

New reconstruction: reconstructed/pokemon_learnset. Eleven routines cover
current-record count/entry access, form-aware member selection, the 26-entry
terminator scan, cache lookup/replacement and 104-byte record copying. Its
14-function evidence set includes five resource/compiler dependencies; those
are not claimed as complete implementations. All earlier reconstruction and
native reproduction records, including move-services, remain unchanged.

Native integration

game/learnset.c converts an already selected native resource into a temporary
logical record, then calls the reconstructed count/entry accessors and
00150064 move insertion to choose an encounter's initial moves. The 26-entry
limit applies. Temporary Pokémon data uses an invented identity block-order
row; it never becomes a save record. Native input guards reject NULL pointers,
out-of-array counts, levels above 100 and eligible moves outside the move table.
Outputs are zeroed on failure. The existing archive/decoded-resource path still
selects the source; the game's cache lifecycle is tested separately, not
installed into the viewer. Interactive level-up learning remains separate.

Optional local-data check

  source tools/native-sdl/activate.sh  # this WSL setup only
  make -C native build/check_learnsets
  native/build/check_learnsets ROMFS

check_learnsets.c loads the local personal, move and learnset archives. It:
- compares the reconstructed getters against raw pairs for every resource,
  including invalid-index fallback to entry zero;
- compares the viewer's reconstructed initial-move path with the pure native
  helper at all levels 0..100 of every learnset (using a 26-entry prefix);
- runs full reconstructed form selection/loading/cache operations through
  native resource callbacks on the base species and stored form-count ranges;
- repeats every request and requires the repeat to avoid another member read.

Results: 799 learnsets, at most 25 stored entries, 29,334 entry/fallback cases,
80,699 level cases, 885 species/form selections and 885 repeat cache hits all
agree. The native 26-entry prefix does not truncate any of these resources.
The resource decoders are shared with earlier native code; prior independent
byte-layout comparisons remain historical evidence in bch-battle. This sweep
compares host paths, not execution of the original ARM game.

Other checks

The new reconstruction suite covers 66,816 accessor cases and 25,515 loader
cases, cache copy/replacement and callback mutation. It joins selection and
access to the existing reconstructed base-state initialization/refresh, move
insertion and maximum PP. Both this suite and native battle tests with the
new bridge pass ASan/UBSan (detect_leaks=0). Synthetic native tests exercise
ordered insertion, duplicates, forgetting/relearning, the 26-entry cap and
invalid moves. Validation records describe the full regression results.

Remaining boundaries

Personal selection/getters and archive reads remain callbacks. Resource
initialization, allocation, disposal and the real cache's initial state are
not reconstructed. Cache insertion requires nonzero capacity and valid slots;
copying requires ordinary, nonoverlapping or identical payload storage separate
from metadata. Native Windows and a captured running-game comparison remain
untested. Prior images/moveset ordering remain as in move-services.
