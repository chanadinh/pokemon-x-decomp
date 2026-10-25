Constructor move order and maximum PP, 2026-10-25

The constructor's move insertion (00150064) and maximum PP (00150220)
now have host implementations in reconstructed/pokemon_moves. Their new
instruction exports, original-byte verifier, callback contracts and synthetic
tests live there. The previous eight reconstruction directories and all old
native reproduction records remain unchanged.

The native learnset selector previously overwrote slots in rotating order.
The original insertion shifts slot 1 into 0, 2 into 1, 3 into 2, then appends
the new move at 3. Given moves A B C D E F in that order, the old selector
produced E F C D; the corrected selector produces C D E F. A duplicate move
keeps its slot and its PP. This affects constructor/encounter selection;
the interactive level-up decision remains separate.

Battler creation now calls reconstructed 00150220 with zero PP Ups, through
an adapter providing selector 5 from the native decoded move table. The
reconstructed routine also handles PP Ups and low-byte wrapping, as checked
in its own suite. The current battler/save format does not store PP Ups;
this change does not add PP Up items or alter that format.

Local original-data comparison

  source tools/native-sdl/activate.sh  # this WSL installation only
  make -C native build/check_moves
  native/build/check_moves ROMFS

check_moves.c reads a/2/1/2 and a/2/1/4 with the native GARC/resource
readers. At every level 0..100 of every learnset it runs the reconstructed
insertion on logical memory through the guest platform, then checks the
native selector's slots and the reconstructed PP. It also checks maximum PP
on every decoded move with PP Ups 0, 1, 2, 3, 255 and UINT32_MAX. Logical
block order is an invented identity row, not the game's resource table.
The printed JSON records case counts and how many results differ in slot
order from the earlier rotating implementation. Only counts/hashes are
retained; the game files remain local.

Result: all 80,699 level cases (799 learnsets, levels 0..100) agree in moves,
slot order and initial PP; 53,283 differ in slot order from the old selector.
All 3,708 maximum-PP cases across 618 decoded move records agree as well.

Validation and reference-comparison.json record the current run. Host checks
also cover six-move overflow, duplicates and relearning a forgotten move.
The new reconstructed fixture links insertion and maximum PP to the old
state initialization/refresh and codec. Sanitizer coverage applies to that
new suite, with leak detection disabled for this environment.

Effect on old records

Earlier captures and comparisons at levels with at most four learned moves
are unaffected. The old bch-battle/verify_battle.py independently models the
rotating order at levels 5 and 50, so its full moveset comparison is superseded
by this record. Its retained results and source are historical and unchanged.
The new routine changes slot order, which can change the action associated
with a battle menu slot at higher levels. Old save files retain their stored
slot order; loading does not reconstruct or reorder an existing moveset.

Boundaries

The move-resource cache/load service 00168bbc remains external. The original
selector-5 dispatch is checked, but its complete resource lifecycle is not
reconstructed. The data sweep compares native host paths, not ARM execution
or a captured running-game scenario. Other move effects, level-up prompts,
resource/name services and battle rules retain their earlier limits.
