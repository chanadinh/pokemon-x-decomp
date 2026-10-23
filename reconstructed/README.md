# Pokémon X host reconstruction milestones

This source bundle contains eight connected, independently reviewed pieces:

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

Run all host tests with a C11 compiler and Make:

```sh
make test
```

Each subdirectory documents its behavior and build instructions. Tests use
synthetic memory and records. The eight-module integration fixture uses the
reconstructed HP/stat and level routines. Wider game-state setup, real
personal/growth resource loading, learnset access, move insertion, name
construction/serialization, and maximum-PP services remain stand-ins.
This bundle does not run a Pokémon battle
or provide a playable PC game. These are analyst-authored reconstructions,
not recovered original source or builds that match the original machine code.

The next targets are the remaining learnset, move/name, maximum-PP, personal,
and growth-resource services. Details, assembly evidence, and limitations are
in [the stats milestone](pokemon_stats/README.md),
[the state milestone](pokemon_state/README.md), and the earlier directories.
Native application, rendering, controls, map/collision, interactive battle,
and save/load work is tracked in the
[first playable slice](../docs/PLAYABLE_SLICE.md).
