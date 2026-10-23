# Pokémon X host reconstruction milestones

This source bundle contains seven connected, independently reviewed pieces:

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

Run all host tests with a C11 compiler and Make:

```sh
make test
```

Each subdirectory documents its behavior and build instructions. Tests use
synthetic memory and records. The wider game-state setup, real resource loading,
and deeper stat, learnset, name, and move-resource services remain stand-ins.
This bundle does not run a Pokémon battle
or provide a playable PC game. These are analyst-authored reconstructions,
not recovered original source or builds that match the original machine code.

The next targets are the remaining HP/stat calculation routines and level,
learnset, move/name, species, and growth-resource services. Details, assembly evidence,
and limitations are in `pokemon_state/README.md` and the earlier
milestone directories.
