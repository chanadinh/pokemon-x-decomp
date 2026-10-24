# Roadmap to a complete native Pokémon X PC game

## Goal and current position

The eventual goal is to run the game's logic and content in a native PC
application, with working graphics, audio, controls, saves, progression,
and the agreed feature set. A successful source build is only one part of
that goal. Completion must be demonstrated by playable coverage and
comparisons with the original game.

As of 2026-10-23, the uploaded executable and 133 code modules have been
mapped and linked for static analysis. There are 44,279 candidate functions
and 230 previously exported pseudocode functions. Those counts describe
analysis output, not reconstructed-source completion.

Eight connected host-source milestones now exist: the `CallWildBattle`
wrapper, its record-preparation path, task creation/request consumption,
the per-record encounter builder/population/configuration helpers, the
Pokémon constructor/parameter-normalization path, data population/storage,
base/extra/derived/refresh state orchestration, and HP/stat calculation.
They include buffer initialization, decoding of a 12-byte record,
optional-context handling, task/list initialization, state flag updates,
creation-parameter preparation, weighted optional-value selection, random
parameter resolution, allocation/manager setup, 26 data-field setters,
checksum/XOR storage transforms, block selection, 13 state-field accessors,
move initialization, auxiliary clearing, HP/PP refresh, HP and five stat updates,
nature adjustment, growth-to-level resolution, and a final script-state write.
Wider setup, real personal/growth resource loading, learnset access, move
insertion, name construction, and maximum-PP services still use stand-ins.
The native asset viewer now opens a window, handles pan/zoom input, and displays
selected original raw, ETC1, and ETC1A4 textures through bounded GARC/LZ/BCLIM
and CGFX loading. A separate native model viewer now previews a checked
unskinned model with orbit/zoom controls, wireframes, and an opt-in unlit
textured mode. A field inspector now follows an original-code-backed zone/matrix
selection and loads region terrain/collision data, with an explicitly restricted
inspection marker. The field's BCH environment model now has a native wireframe
an unlit stage-0 textured preview and a six-stage combiner preview with vertex
colors, and directional fragment lighting from the scene environment; the
vertex shader, point lights, projection and scene integration are still missing.
Game-runtime integration, full material/scene rendering,
player movement/collision, the interactive
battle loop, and playable save/load are still missing.
Per-milestone validation records document the actual tests. Game-logic tests
use synthetic memory; native asset tests also compare original-data decoding
with independent references and exercise rendered previews. No running-game
comparison is established.

The [first playable slice](docs/PLAYABLE_SLICE.md) defines an end-to-end
acceptance scenario and records the local asset audit. The original container
and extracted RomFS are already available locally; further uploads are not
needed to begin resource decoding. Their presence does not establish decoded
asset roles or complete asset integrity.

## Milestones and completion criteria

| Milestone | Work | Evidence required to call it complete |
| --- | --- | --- |
| 1. Fix the baseline and reference | Identify exact base-game revision, updates, language targets, and asset inventory. Run a reproducible reference on hardware or a suitable emulator; record inputs, saves, selected memory states, and outputs. | Stable input hashes, repeatable reference runs, a written feature matrix, and automated comparison fixtures. |
| 2. Map systems and build the host foundation | Identify module lifecycle, core structures, allocators, script VM, resource loaders, save layout, and service boundaries. Choose source reconstruction versus static translation per subsystem. Build a native executable with explicit 32-bit game-address handling, module initialization, logging, and asset access. | A repeatable host build can initialize a bounded subsystem and match selected reference computations without invented game dependencies. |
| 3. Implement PC platform support | Replace 3DS graphics/shaders, audio, input/touch, filesystem, timing, threading, and relevant system services. Preserve assumptions about alignment, endianness, pointer width, and execution order. | Original asset samples render correctly; audio plays correctly; controls work; service tests have documented reference behavior. |
| 4. Deliver one complete playable slice | Load one map, display the player, move and collide, run an interaction, start one wild battle, choose an action, finish it, and return to the map. Support a save/load round trip for this slice. | The full scenario runs in the native application with real game logic in the path and a repeatable comparison against the original. |
| 5. Complete the main adventure | Expand maps, story scripts, NPC behavior, transitions, battle rules and AI, party/box management, items, catching, evolution, menus, and saving. | A new game can reach the credits; representative battle and progression scenarios pass regression checks. |
| 6. Cover the full agreed content | Add postgame, optional locations and events, all relevant moves/abilities/items/forms, breeding, special battles, localization, and any included updates. Audit features beyond the main story. | Every item in the feature matrix has a test or a documented playthrough result; save compatibility is checked across milestones. |
| 7. Finish compatibility and distribution | Resolve platform bugs, rendering/audio differences, timing, performance, controller options, and packaging. Handle communication features as their own workstream. | Reproducible Windows builds, clean-machine installs, long playthroughs, save durability, performance targets, and no known progression blockers within the supported scope. |

These phases overlap: platform work and reverse engineering can proceed in
parallel once interfaces are understood. A small vertical slice should
arrive before attempting to reconstruct every function. It tests whether
the architecture, asset pipeline, and behavior-preservation strategy work.

## Immediate sequence for this workspace

1. Reconstruct learnset access and move insertion, name construction/copy,
   and maximum-PP resource service `0x00150220`. Replace personal/growth-resource
   stand-ins with checked resource access as those dependencies are reviewed.
   HP/stat services `0x0013aa84`, `0x0014fb00`, `0x0014fbd4`, `0x0014fda4`,
   `0x0014fe78`, and `0x0014fa2c` now have host implementations, along with
   scalar formulas, nature adjustment, object-level/max-HP helpers, and level
   resolution `0x00168b3c`. State orchestration at
   `0x0013a8d8`, `0x0012b744`, `0x0012b808`, and `0x0013a7bc` now has host
   implementations, with 13 associated state-field accessors. Population `0x0014f6b8`, its 26
   setters, checksum `0x00174e0c`, manager codec `0x001685f0`/`0x00168660`,
   and four block selectors now have host implementations. The outer constructor `0x0039a5a8`,
   base initializer `0x0039daf4`, allocation path `0x0039a8a4`, parameter
   normalizer `0x0014f324`, and XOR primitive `0x00167fb4` now have host
   implementations through explicit service boundaries. Copy-before-cleanup
   behavior is traced. The eight-module fixture produces and checks the encrypted
   payload and auxiliary state using reconstructed HP/stat/level routines and
   synthetic resources. A complete game-valid object and behavior against a
   running reference remain unverified.
2. Reconstruct setup function `0x15c75434` and its required callees, keeping
   field names provisional until callers, consumers, or runtime behavior
   support them. Establish the table's actual record count and field meanings.
3. Replace the remaining stand-ins with checked implementations as their
   dependencies become understood. Add reference execution comparisons;
   the current tests alone do not prove equivalence to the running game.
4. Extend the [native viewer](native/README.md) beyond its verified raw,
   ETC1, and ETC1A4 standalone textures, including LZ11-compressed samples.
   Identify and decode the minimal map,
   model, texture, animation, UI, and audio paths for the playable slice.
   Embedded CGFX textures and a restricted static textured model preview now work.
   Zone/matrix/region selection and a bounded terrain/collision inspector now
   work; see [field evidence](docs/native-field.md). Palette extensions, complete
   material shaders, animation, faithful BCH field rendering and player scene bindings
   remain unsupported.
   The [CGFX inspector](docs/native-containers.md) now identifies bounded model
   and texture records; [geometry evidence](docs/native-geometry.md) records the
   supported mesh subset. The [material preview](docs/native-materials.md)
   binds checked UVs/textures and selected sampler/depth/blend states.
   The [BCH model inspector](docs/native-bch.md) now decodes the selected field's
   positions, UV0 and indices, with an independent numeric comparison and exact
   external texture-name matches through area 75. Native BCH wireframe rendering
   now works with a mandatory explicit triangle-list interpretation and exact
   per-component geometry checks. Area 75's nineteen textures decode in the native
   asset viewer, with every RGBA pixel checked independently. Original code now
   corroborates area selection and the AD section-11 BCH resource consumer.
   The native model viewer now textures the field: each material's TEV stage 0
   selects its sampled unit (unit 1 for the four projection-mapped materials),
   names resolve exactly in area 75's pack, and sampler/alpha/depth/blend state
   comes from the material's command registers. An independent parser matches
   all 12 bindings and 432 register observations; see
   [textured evidence](native/reproduction/bch-textured/README.txt).
   `--combiners` now runs all six TEV stages with decoded stored/fixed vertex
   colors, matching an independent oracle of the pinned emulator behavior on
   116,000 evaluations; see [combiner evidence](native/reproduction/bch-combiners/README.txt).
   For this field the projected term and fragment secondary cannot change the
   output under the stored constants.
   The field's lights come from the scene environment in `a/0/0/3` entry 137.
   The viewer applies each material's light set as directional diffuse and
   ambient lighting, using the light-vector sign and color products traced in
   the original code; an independent oracle matches it. See
   [lighting evidence](native/reproduction/bch-lighting/README.txt).
   Next trace the environment's binding to the field and the day/night
   selection, then execute the vertex shader (normal quaternion, primary color),
   point lights, projected textures with a camera, fog, the original primitive
   state, culling and camera conventions, and integrate the field scene. Establish actual player entry and terrain collision consumers.
   Capture the corresponding reference scenario.
5. Connect explicit game-state/platform interfaces to the native application,
   then implement movement/collision, interaction, battle, return to the field,
   and save/load according to the
   [slice acceptance criteria](docs/PLAYABLE_SLICE.md#acceptance-criteria-for-the-first-playable).

These addresses identify the current analysis baseline. CRO addresses are
synthetic and must not be treated as observed runtime addresses.

## What “100%” needs to mean

Track at least three separate completion measures: playable content,
behavioral fidelity, and platform/feature compatibility. Finishing the
story does not establish every optional feature or edge case. A raw
function count cannot produce an honest overall completion percentage.

Multiplayer, trading, and online features need separate protocol and
compatibility work. Availability of original online services must be
verified; reproducing offline gameplay does not automatically restore
those services. A compatible replacement service, if included in scope,
is an additional project.

Original variable names, comments, and exact source organization may be
unrecoverable. They are not necessary for a functioning native port, but
functional equivalence must still be tested rather than assumed from
decompiler output. Static translation may reduce some source-recovery
work; it still requires accurate runtime and platform replacements.

## Effort and planning

This remains substantial work with uncertainty about dependency scope and
feature coverage. A defensible schedule requires the first playable slice and
measurements of actual dependency and compatibility work. The current evidence
does not support a delivery date or a reliable completion percentage.

Useful parallel workstreams are ARM/game reverse engineering, graphics and
platform integration, tools/assets, and reference-based testing. Progress
should be reported through completed playable scenarios and retired
unknown dependencies, alongside technical source milestones.
