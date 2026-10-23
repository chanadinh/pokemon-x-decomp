# Pokémon X decompilation

**Reconstructing Pokémon X game logic in portable C, one verified subsystem at a time.**

[![Host tests](https://github.com/chanadinh/pokemon-x-decomp/actions/workflows/tests.yml/badge.svg)](https://github.com/chanadinh/pokemon-x-decomp/actions/workflows/tests.yml)
[![Roadmap](https://img.shields.io/badge/project-roadmap-blue)](ROADMAP.md)

An early-stage reverse-engineering project for Pokémon X on Nintendo 3DS.
Explore seven connected C reconstruction milestones, reviewed assembly evidence,
and synthetic tests that run without game files.

**Current status: research and host-side tests. There is no playable PC port,
renderer, or runnable battle yet.** The long-term goal is a native PC game;
[the roadmap](ROADMAP.md) explains the work and evidence still needed.

## Try the reconstructed logic

You need Git, Make, and a C11 compiler such as GCC or Clang. Python 3 is needed
for the analysis-tool tests. On Linux or WSL:

```sh
git clone https://github.com/chanadinh/pokemon-x-decomp.git
cd pokemon-x-decomp
make -C reconstructed test
python3 -m unittest discover -s tests -v
```

The host C tests use synthetic data and require no ROM, extracted assets, or
Ghidra installation. Extraction integration tests skip when their optional
PowerShell and 3dstool dependencies are unavailable.

## What works today

| Reconstructed subsystem | Scope |
| --- | --- |
| [Wild-battle script wrapper](reconstructed/call_wild_battle/README.md) | Argument handling and helper call order |
| [Wild-battle setup](reconstructed/wild_battle_setup/README.md) | Buffer initialization, packed-record decoding, context updates |
| [Battle task](reconstructed/battle_task/README.md) | Task creation, request consumption, list and state handling |
| [Encounter builder](reconstructed/encounter_builder/README.md) | Creation parameters, population loop, configuration selection |
| [Pokémon constructor](reconstructed/pokemon_constructor/README.md) | Parameter normalization, allocation, manager setup, XOR transform |
| [Pokémon data](reconstructed/pokemon_data/README.md) | Population, 26 field setters, checksum, storage codec, block selection |
| [Pokémon state](reconstructed/pokemon_state/README.md) | Base/extra initialization, derived-state sequencing, HP/PP refresh, 13 field accessors |

A seven-module integration fixture connects these paths using synthetic species,
growth, learnset, name, PP, and stat services. Wider game setup, real resource
loading, and deeper stat calculations still use stand-ins. These are analyst-authored reconstructions, not
recovered original source or builds that match the original machine code.

The analysis baseline covers 133 CRO modules, with 44,279 candidate functions
and 230 exported pseudocode functions. These are analysis counts, **not a
completion percentage**.

## Help build the next milestone

Contributions are useful across C, ARM reverse engineering, tooling,
documentation, and reference-based testing. Start with
[CONTRIBUTING.md](CONTRIBUTING.md) and the
[immediate roadmap](ROADMAP.md#immediate-sequence-for-this-workspace).

Current priorities:

- Reconstruct the remaining HP/stat calculations, level lookup, and move/name services.
- Replace species and growth-resource stand-ins with checked resource access.
- Compare reconstructed behavior against a running reference.
- Document the asset-loading paths needed for a first playable slice.

[Report a reproducible bug](https://github.com/chanadinh/pokemon-x-decomp/issues/new?template=bug_report.yml)
or [propose a research task](https://github.com/chanadinh/pokemon-x-decomp/issues/new?template=research_task.yml).
If this work interests you, **star the repository** to bookmark it and help
others discover it. Use **Watch → Custom → Releases** for release notifications.

## Project notes

- [Source milestone overview](reconstructed/README.md)
- [Roadmap and completion criteria](ROADMAP.md)
- [Local analysis history and reproduction workflow](docs/ANALYSIS_WORKSPACE.md)
- [Recorded tool versions](tools/versions.json)

Game inputs, generated analysis directories, saved Ghidra projects, and named
ZIP bundles in the workspace notes are local artifacts excluded from Git.
This repository does not distribute a ROM or extracted game assets.
Pokémon is owned by its respective rights holders; this project is unofficial
and unaffiliated. No project license has been declared yet.
