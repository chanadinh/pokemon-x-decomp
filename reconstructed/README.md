# Pokémon X host reconstruction milestones

This source bundle contains four connected, independently reviewed pieces:

- `call_wild_battle/`: script argument handling and helper call sequence.
- `wild_battle_setup/`: temporary-buffer initialization, 12-byte record
  decoding, preparation sequencing, and the final context state write.
- `battle_task/`: task allocation/initialization/list registration,
  request consumption, state flag updates, and submission return behavior.
- `encounter_builder/`: per-record creation parameters, weighted optional
  value selection, population-loop behavior, and configuration selection.

Run all host tests with a C11 compiler and Make:

```sh
make test
```

Each subdirectory also documents direct compiler commands. Tests use
synthetic memory and records. The wider game-state setup and constructors
inside the submission path remain stand-ins. This bundle does not run a Pokémon battle
or provide a playable PC game. These are analyst-authored reconstructions,
not recovered original source or builds that match the original machine code.

The next target is the constructor that turns prepared parameters into
Pokémon data. Details, assembly evidence, and limitations are in
`encounter_builder/README.md` and the earlier milestone directories.
