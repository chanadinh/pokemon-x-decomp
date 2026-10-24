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
| Native application and runtime | Native SDL2 viewers; `--zone` runs a real-time field scene at 400 × 240 with frame pacing, scripted input and frame capture, drawing in row bands on several cores; game runtime integration pending |
| Renderer and game asset loading | BCLIM/CGFX/BCH textures, texture transforms and base mip levels; zones load from game data with their regions (several per town), terrain and collision, region props, area textures, six combiner stages, vertex colors and directional lighting. 328 of 360 zones load. Vertex shader, bump maps, Fresnel, point lights, fog, projection, culling and the game camera remain missing |
| Player controls, movement, and collision | The animated player walks and runs in real time through the house and Vaniville Town on exact standard cells over the collision floor; NPCs block, doors and stairs load their target zones, and NPCs answer with their script's lines. The game's own controller, spawn and collision rules are not reconstructed |
| Interactive battle and return to the field | Wild one-on-one battles from tall grass on Route 2: game species, moves, learnsets, type chart and natures; stats from the reconstructed routines; published damage formulas; returns to the field with the partner's HP. No battle models, catching or experience |
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

Embedded CGFX textures now decode through an explicit shared payload interface,
with texture selection and listing in the viewer. Eleven real embedded images
match an independent pixel reference. The separate native model viewer displays
767 vertices and 936 triangles from six shapes in `a/1/0/2`, entry 0, with
keyboard orbit/zoom controls. Its stored name `collision02` does not establish
a map or collision-system role. See the [geometry scope](native-geometry.md)
and [new validation record](../native/reproduction/cgfx-textures/validation.json).

The same mesh now supports an opt-in unlit textured preview. Six material
bindings connect UV0 and vertex colors to embedded textures, with checked
sampler, depth, culling, alpha-test, and supported blend states. The preview
uses an inspection camera and a stable translucency-layer/priority order; it does not reproduce the
game's lighting, fog, texture combiners, shaders, or transparent sorting.
See the [material scope](native-materials.md) and
[textured-preview evidence](../native/reproduction/textured-model/README.txt).

The [field inspector](native-field.md) now follows zone 5 → matrix 12 → region
165 and displays 1,600 raw terrain cells plus optional collision triangle edges.
Original-code evidence corroborates resource selection. Keyboard movement uses
an explicit exact-cell inspection policy; it does not reconstruct player rules
or spawn. The selected field's BCH geometry now displays in the native model viewer
as a wireframe requiring an explicit triangle-list interpretation. The
[BCH decoders](native-bch.md) read twelve meshes' positions, UV0 and indices and
preserve their external texture names. Original area selection and the texture
container consumer are corroborated by executable evidence. Primitive GPU state,
later material bindings and projected effects still need to be connected and verified.

These are bounded asset-preview paths. The applications do not yet call
the eight reconstructed game-logic modules. Palette extensions, full material rendering,
animation, BCH environment graphics and player scene bindings remain unimplemented. The separate
[CGFX inspector](native-containers.md) provides resource metadata alongside
the native decoders.
A repeatable running-game capture is still required to select
and compare the first map-to-battle scenario.

## What remains before the first playable

Status on 2026-10-24 (after the town milestone), by acceptance criterion below. Every item needs original
code or a reference capture behind it, not only a plausible native version.

| Criterion | Done | Remaining |
| --- | --- | --- |
| 1. Load one map with player, environment and UI assets | Zones from game data: the house and Vaniville Town (four regions) with geometry, textures, combiners, lighting, terrain/collision and region props in one scene; the player-character model skinned, textured and animated from its GF1 motion pack; 328 of 360 zones load | Trace how a zone binds its environment and day/night, and which player model and outfit the game loads; which motions the game plays and how (rate, blending, foot placement); vertex shader outputs; culling, fog, projected textures and the field camera; decode the field UI |
| 2. Move, collide, interact | `--zone`: the player walks and runs in real time over standard cells, NPCs block, warps load their target zone (the house's two floors and Vaniville Town), walking into the next zone's cells switches zone (Route 1, Aquacorde Town), Space runs the NPC's zone-script event and shows its story-text lines in a message window | Region streaming instead of reloads, zone-entry scripts, type-1 warps, story flags for NPC visibility; reconstruct the player controller (spawn, speeds, terrain flags, collision response) from the game's code; script natives beyond the five inferred ones (story flags, choices, actor commands, battles), the game's font and window |
| 3. Wild encounter and interactive battle | Tall-grass encounters from the zone's table; a one-on-one battle with Fight/Run on the game's data, stats from the reconstructed routines; encounter-record preparation, Pokémon construction and data/state (eight milestones, synthetic resources) | Trace the encounter rate, damage, move effects and AI in the game's code (DllBattle.cro); battle models, animations and the game's battle UI; catching, experience and the party |
| 4. Return to the field with state preserved | The partner's HP and the player's position persist after a battle | Battle-to-field transition and party/world state |
| 5. Save and load | — | Save layout, serialization and the slice's state |
| 6. Reference comparisons | Static asset and code-byte checks | A recorded run (emulator or hardware) of the chosen scenario with states and frames to compare |
| Audio | — | `sound/` BCSAR decoding and playback for the field and battle |

The field and battle sides can proceed in parallel. The largest single items
are the battle engine, the script VM and the reference capture.

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
after checking their dependencies. The field milestone establishes a small
numeric resource chain; it does not yet select a reference-backed playable scenario.

The broader scope and completion gates remain in the
[roadmap](../ROADMAP.md).
