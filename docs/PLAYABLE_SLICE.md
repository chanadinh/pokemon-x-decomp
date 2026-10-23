# First playable slice

The original files needed to continue are already on this machine. Further
uploads are unnecessary for the current reconstruction and asset investigation.
The next obstacle is implementing and connecting the required game and platform
systems. The native asset viewer and host tests do not yet provide a playable game.

## Current position

Eight source milestones connect wild-battle request preparation to Pokémon
construction, payload storage, state initialization/refresh, and HP/stat
calculation. Growth-to-level lookup and nature adjustment are reconstructed
alongside the HP and five stat updates. Their tests still use synthetic
personal/growth resources and callbacks for learnset access, move insertion,
name construction, and maximum-PP services. Scope and validation are described
in the [source overview](../reconstructed/README.md) and
[stats milestone](../reconstructed/pokemon_stats/README.md).

| Requirement | Current state |
| --- | --- |
| Reviewed game logic | Eight connected host-source milestones, including HP/stat updates and level resolution |
| Original executable and modules | Present locally; baseline hashes checked |
| Extracted game assets | Present locally; inventory recorded below |
| Native application and runtime | Native SDL2 asset viewer with window/event loop, timing, pan/zoom, and image export; game runtime integration pending |
| Renderer and game asset loading | Selected GARC → optional LZ10/LZ11 → raw/ETC1/ETC1A4 BCLIM → RGBA textures display; map/model/game renderer still missing |
| Player controls, movement, and collision | Viewer keyboard controls work; player movement and collision are not implemented |
| Interactive battle and return to the field | Not implemented; request/construction code is a partial dependency |
| Save/load for a playable scenario | Not implemented |
| Reference execution comparisons | No repeatable running-game capture is part of the current validation |

Source milestones measure reviewed behavior at specific boundaries. They do
not establish a completion percentage for a playable game, and the current
evidence does not support a delivery date.

## Local input and asset audit

The 2026-10-23 audit found these existing files and directories:

| Local location | Observed contents |
| --- | --- |
| `inputs/pokemon-x-0f6d3e1d66cb/` | Decompressed `code.bin`, exheader, and extraction manifest |
| `inputs/pokemon-x-modules-a035d9775a1f/` | 133 CRO files, one CRS, one CRR, and manifests |
| `D:\Downloads\Pokemon X (USA) (En,Ja,Fr,De,Es,It,Ko).3ds` | Original container present, 2,147,483,648 bytes |
| `D:\Downloads\PokemonX-analysis-20261023-102030-ff2c2690\work\main.cxi` | Extracted container present, 1,801,052,160 bytes |
| `D:\Downloads\PokemonX-modules-20261023-104349-3eee2a4b\work\romfs.bin` | RomFS container present, 1,797,873,664 bytes |
| `D:\Downloads\PokemonX-modules-20261023-104349-3eee2a4b\work\romfs\` | Extracted tree: 653 files, 1,783,776,455 bytes |

Under WSL, the final path is
`/mnt/d/Downloads/PokemonX-modules-20261023-104349-3eee2a4b/work/romfs`.
These are local audit paths, not portable configuration defaults. The original
filename does not independently establish the game's region or revision.

The extracted tree contains:

| Directory/group | Files | Bytes |
| --- | ---: | ---: |
| `a/` | 271 | 1,587,902,328 |
| `sound/` | 234 | 163,981,300 |
| `m/` | 6 | 23,926,157 |
| `shaders/` | 4 | 2,836 |
| `sounddebug/` | 2 | 103,618 |
| `.crr/` | 1 | 8,192 |
| Files directly in the root | 135 | 7,852,024 |

Four 32-byte samples from `a/0/0/0`, `a/0/1/7`, `a/0/1/8`, and `a/0/2/1`
start with ASCII `CRAG`. This is header evidence for investigating these
archives; it does not identify which entries contain maps, models, textures,
personal data, or learnsets. The tree also has filenames ending in `.aac`,
`.bcsar`, `.moflex`, and `.shbin`; their payload formats were not decoded by
this audit.

All 135 module files in this extracted tree were checked against both the
workspace copies and the size/SHA-256 entries in the saved module manifest.
That associates the tree with the current code-analysis baseline. It does not
verify the remaining asset files or prove that the container was completely
extracted. The large ROM, CXI, and RomFS containers were not rehashed.

Detailed local-only evidence is saved in
`outputs/playable-inventory/romfs-inventory.json` and
`outputs/playable-inventory/module-association.json`. The first records every
file's path and size, summary counts, container metadata, and sampled headers;
the second records each module comparison. Assets and these generated reports
remain outside Git.

## Native foundation now available

The [native viewer](../native/README.md) reads archive indices without loading
whole archives, selects a bounded member, decompresses supported streams, and
decodes supported standalone textures. Local checks display a raw RGBA8 texture
at `a/1/1/8`, entry 1, and an LZ11-compressed RGBA4 texture at `a/1/6/0`, entry 0.
The latter is a 40×40 pink pastry icon. ETC1 at `a/0/0/5`, entry 35, now
decodes a 320×240 gray geometric background; ETC1A4 at `a/1/5/9`, entry 0,
exercises LZ11 decompression and transparency in a 40×40 image.
These observed appearances do not
identify their in-game ownership or scene bindings.

The application provides a native window, keyboard pan/zoom/reset/exit, resizing,
monotonic timing, software-renderer fallback, original-pixel export, and rendered
frame capture. It refuses to overwrite existing output files. Synthetic and
local real-asset checks are recorded in
[the foundation validation record](../native/reproduction/validation.json) and
[the ETC milestone](../native/reproduction/texture-formats/README.txt).

This completes a bounded asset-preview path. The application does not yet call
the eight reconstructed game-logic modules. Palette extensions,
embedded textures, model/map formats, and rendering scene bindings remain
unimplemented in the viewer. A separate [CGFX inspector](native-containers.md)
now indexes selected model and texture records, including their names, counts,
and bounded texture extents; it does not render models or identify map roles.
A repeatable running-game capture is still required to select
and compare the first map-to-battle scenario.

## Acceptance criteria for the first playable

The first slice is one reproducible field-to-battle-to-field scenario, chosen
from a captured reference run. A scene viewer or a scripted battle animation
alone does not satisfy these criteria.

1. A native application starts from documented local inputs and loads one
   identified map with its required player, environment, and interface assets.
2. The player can move using PC controls, encounters the map's collision
   boundaries, and completes one interaction with observable game-state changes.
3. A repeatable wild encounter enters an interactive battle. The player can
   choose an action, the supported battle rules update state, and the battle
   reaches a supported outcome.
4. The application returns to the field with the resulting party and world
   state preserved. Required UI and audio work for the selected scenario.
5. Saving and loading restores the slice's position and relevant game state.
   The save format and supported state are documented; compatibility with
   original save files is a separate claim that needs evidence.
6. The complete scenario has repeatable reference comparisons for selected
   state transitions, battle results, and visible behavior. Any unresolved
   differences are recorded. Synthetic callbacks are replaced for the game
   dependencies actually exercised by this scenario.

This bounded slice does not require every map, move, or online feature. Its
purpose is to demonstrate that real assets, reconstructed logic, platform
services, and persistence work together.

## Prioritized work in parallel

| Workstream | Next bounded result | Evidence for advancing |
| --- | --- | --- |
| Reference capture | Fix the base revision/update/language target and capture one candidate map, interaction, encounter, battle action, and return using a repeatable starting save and input sequence | Input/save identifiers, screenshots or video, recorded inputs, and selected observable state before/after; memory observations where available |
| Native application | Extend the working texture viewer with explicit game-address/platform interfaces and bounded game-state initialization | Reconstructed logic can run from checked real resources inside the application with documented lifecycle and reference comparisons |
| Asset decoding | Extend the bounded GARC/LZ/BCLIM path to required palette/model formats and scene bindings; identify candidate map/player/UI resources through loaders and reference observations | Documented paths/indices and supported formats, checksums for selected samples, and a visible comparison to the reference |
| Game reconstruction | Replace personal/growth resources, learnset access, move insertion, name construction, maximum-PP, and wider encounter/setup dependencies needed by the selected scenario | Reviewed instruction bytes, independent tests, explicit remaining services, and comparisons to captured reference values |
| Slice integration | Connect movement/collision, interaction, battle, return, and save/load after their contracts are understood | One automated or recorded end-to-end scenario satisfying the acceptance criteria above |

Continue the first three workstreams alongside bounded code reconstruction.
Rendering and reference capture can reveal missing dependencies earlier than
continuing through source functions alone. Choose the concrete map and battle
after checking their dependencies; this audit has not established which
numbered archives provide them.

The broader scope and completion gates remain in the
[roadmap](../ROADMAP.md).
