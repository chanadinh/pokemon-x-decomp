# Pokémon X decompilation

**Reconstructing Pokémon X game logic in portable C, one verified subsystem at a time.**

[![Host tests](https://github.com/chanadinh/pokemon-x-decomp/actions/workflows/tests.yml/badge.svg)](https://github.com/chanadinh/pokemon-x-decomp/actions/workflows/tests.yml)
[![Roadmap](https://img.shields.io/badge/project-roadmap-blue)](ROADMAP.md)

An early-stage reverse-engineering project for Pokémon X on Nintendo 3DS.
Explore eight connected C reconstruction milestones, reviewed assembly evidence,
and synthetic tests that run without game files.

**Current status: eight tested game-logic milestones, a native texture viewer,
and native model and field terrain inspectors.
There is no playable PC port or interactive battle yet.** The viewer loads
selected original textures, including ETC1 and ETC1A4, through GARC/LZ/BCLIM
decoding, plus embedded CGFX textures. A separate viewer previews a bounded
subset of unskinned CGFX geometry with checked texture/material bindings and
an unlit software renderer. A field inspector follows a checked zone-to-matrix-to-region
chain and displays terrain cells, collision edges, and a restricted inspection marker.
The long-term goal is a native PC game;
[the roadmap](ROADMAP.md) explains the work and evidence still needed. The
[first playable slice](docs/PLAYABLE_SLICE.md) has concrete acceptance criteria
and an inventory of the original assets already available locally.

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

## Run the native asset viewer

With SDL2 development files and pkg-config installed:

```sh
make -C native
native/build/pokemon_asset_viewer --demo
# Use your locally extracted RomFS for the checked compressed texture sample:
native/build/pokemon_asset_viewer --romfs /path/to/romfs --archive a/1/6/0 --entry 0
```

The texture viewer provides pan/zoom, image exports, and bounded archive/texture loading.
The demo requires no game files. [Build instructions and supported formats](native/README.md)
describe the actual limits. `native/build/pokemon_model_viewer` provides a
separate preview with orbit/zoom controls. Add `--textured` for supported
texture, vertex-color, depth, and blend states. Lighting, maps, and gameplay
remain incomplete; this is not original-game rendering equivalence.

`native/build/pokemon_field_viewer --romfs /path/to/romfs --zone 5` loads the
checked field layout. Arrows/WASD move an inspection marker on exact standard
cells; `C` shows collision edges. See the [field evidence and limits](docs/native-field.md).
The environment's native BCH rendering and actual player movement are still pending.
A [BCH inspection tool](docs/native-bch.md) now decodes the selected field's
positions, UV0 and indices and checks its external texture names.

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
| [Pokémon stats](reconstructed/pokemon_stats/README.md) | HP and five stat updates, nature adjustment, growth-to-level lookup, and supporting field accessors |

An eight-module integration fixture connects these paths with reconstructed
HP/stat calculations and level resolution. Personal/growth resources, learnset
access, move insertion, name construction, and maximum-PP services still use
synthetic data or callbacks, as do wider game setup and platform dependencies.
These are analyst-authored reconstructions, not
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

- Reconstruct learnset access, move insertion, name construction, and maximum-PP services.
- Replace personal and growth-resource stand-ins with checked resource access.
- Compare reconstructed behavior against a running reference.
- Extend the native viewer with the map/model asset formats and game-state integration needed for the first playable slice.

[Report a reproducible bug](https://github.com/chanadinh/pokemon-x-decomp/issues/new?template=bug_report.yml)
or [propose a research task](https://github.com/chanadinh/pokemon-x-decomp/issues/new?template=research_task.yml).
If this work interests you, **star the repository** to bookmark it and help
others discover it. Use **Watch → Custom → Releases** for release notifications.

## Project notes

- [Native asset viewer](native/README.md)
- [Source milestone overview](reconstructed/README.md)
- [Roadmap and completion criteria](ROADMAP.md)
- [First playable slice and local asset inventory](docs/PLAYABLE_SLICE.md)
- [Local analysis history and reproduction workflow](docs/ANALYSIS_WORKSPACE.md)
- [Recorded tool versions](tools/versions.json)

Game inputs, generated analysis directories, saved Ghidra projects, and named
ZIP bundles in the workspace notes are local artifacts excluded from Git.
This repository does not distribute a ROM or extracted game assets.
Pokémon is owned by its respective rights holders; this project is unofficial
and unaffiliated. No project license has been declared yet.
