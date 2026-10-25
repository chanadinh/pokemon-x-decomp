# Pokémon X host reconstruction milestones

This source bundle contains eleven connected, independently reviewed pieces:

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
using a text-resource callback. Wider game-state setup, real personal/growth
resources, underlying archive/message loading, and move-resource caching
remain stand-ins in those fixtures.
This bundle does not run a Pokémon battle
or provide a playable PC game. These are analyst-authored reconstructions,
not recovered original source or builds that match the original machine code.

The next targets are message loading, move-resource, personal, archive and growth-resource
services and resource initialization. Details, assembly evidence, and limitations
are in [the name milestone](pokemon_names/README.md),
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
