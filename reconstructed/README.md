# Pokémon X host reconstruction milestones

This source bundle contains nineteen connected, independently reviewed pieces:

- `call_wild_battle/`: script argument handling and helper call sequence.
- `wild_battle_setup/`: temporary-buffer initialization, 12-byte record
  decoding, preparation sequencing, and the final context state write.
- `battle_task/`: task allocation/initialization/list registration,
  request consumption, state flag updates, and submission return behavior.
- `encounter_builder/`: per-record creation parameters, weighted optional
  value selection, population-loop behavior, and configuration selection.
- `pokemon_constructor/`: creation-parameter normalization, object/data/manager
  construction, extra-buffer XOR transformation, and state acquire/restore.
- `pokemon_data/`: data population, field setters, checksum, manager encoding
  and decoding, block selection, and ability/growth lookup orchestration.
- `pokemon_state/`: base/extra initialization, derived-state sequencing,
  HP/PP refresh, and 13 storage accessors for species, form, growth, moves,
  PP, auxiliary HP/status, and the name flag.
- `pokemon_stats/`: HP and five stat updates, fixed-width scalar calculations,
  nature adjustment, growth-to-level scanning, object-level/max-HP lookup,
  IV/EV/nature/auxiliary getters, and six auxiliary stat setters.
- `pokemon_moves/`: constructor move insertion with ordered slot shifting,
  preserved PP/PP Ups, and maximum-PP calculation through move selector 5.
- `pokemon_learnset/`: count/entry accessors, form-aware loading, bounded
  terminator scanning, cache lookup/replacement and record copying.
- `pokemon_names/`: shared UTF-16/control-token copying, species/form indices,
  name construction through a text service, and name storage through the codec.
- `pokemon_move_resource/`: move-field dispatch, bounded resource selection,
  payload copying and byte-sized cache lookup/replacement, reviewed through live REA MCP.
- `message_resources/`: resident/cached/streamed text loading, language selection,
  UTF-16 decoding and token-aware length, connected to the name builder.
- `pokemon_personal/`: species/form row selection, 64-byte grouped copying,
  46 field selectors and current-record wrappers, connected to learnset loading.
- `pokemon_growth/`: growth-rate selection through personal data, curve loading,
  cache lookup/replacement and grouped 404-byte copying, connected to level resolution.
- `archive_io/`: resident member information, locked member reads/seeks and
  64-bit file-stream read/seek/position, connected through growth loading.
- `archive_index/`: GARC index parsing, resident member-table construction,
  partial reads and growth/move archive opening.
- `pokemon_resources/`: boot-time personal-table loading, archive opening,
  descriptor/record/cache construction and non-resident member lookup.
- `message_construction/`: message-resource construction, member opening and
  destruction, plus move-resource boot.

Run all host tests with a C11 compiler and Make:

```sh
make test
```

Each subdirectory documents its behavior and build instructions. Tests use
synthetic memory and records. The eight-module integration fixture uses the
reconstructed HP/stat and level routines. A new state-initialization/refresh
fixture also runs the reconstructed move-insertion and maximum-PP services.
The learnset fixture adds reconstructed selection, access and caching to that
state path. The name fixture adds reconstructed name building and storage,
using a text-resource callback. Wider game-state setup, real personal
resources and underlying archive I/O/resource initialization remain stand-ins in those
fixtures. The move-resource fixture connects its reconstructed selector/cache
to maximum PP. The message-resource fixture connects reconstructed loading and
decoding to the name builder. The personal-data fixture connects record/field
services to learnset form selection. The growth fixture connects the
reconstructed selector/cache to level resolution, and the archive fixture runs
it through reconstructed member reads; older state fixtures retain their callbacks.
This bundle does not run a Pokémon battle
or provide a playable PC game. These are analyst-authored reconstructions,
not recovered original source or builds that match the original machine code.

The next targets are the archive object constructors and the configuration
table. Details, assembly evidence, and limitations
are in [the message-construction milestone](message_construction/README.md),
[the resource milestone](pokemon_resources/README.md),
[the archive-index milestone](archive_index/README.md),
[the archive milestone](archive_io/README.md),
[the growth-resource milestone](pokemon_growth/README.md),
[the personal-data milestone](pokemon_personal/README.md),
[the message-resource milestone](message_resources/README.md),
[the move-resource milestone](pokemon_move_resource/README.md),
[the name milestone](pokemon_names/README.md),
[the learnset milestone](pokemon_learnset/README.md),
[the move milestone](pokemon_moves/README.md),
[the stats milestone](pokemon_stats/README.md),
[the state milestone](pokemon_state/README.md), and the earlier directories.
A separate [native viewer](../native/README.md) now runs a walkable field and
wild battles, using reconstructed stats, level, learnset access, move insertion,
maximum-PP, name-copy/writing and storage routines.
Game runtime, map/collision, interactive battle, and save/load work is
tracked in the
[first playable slice](../docs/PLAYABLE_SLICE.md).
