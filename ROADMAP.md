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

Five connected host-source milestones now exist: the `CallWildBattle`
wrapper, its record-preparation path, task creation/request consumption,
the per-record encounter builder/population/configuration helpers, and the
Pokémon constructor/parameter-normalization path.
They include buffer initialization, decoding of a 12-byte record,
optional-context handling, task/list initialization, state flag updates,
creation-parameter preparation, weighted optional-value selection, random
parameter resolution, allocation/manager setup, the extra-data XOR transform,
and a final script-state write. Wider setup, data-population setters, and
larger state services still use stand-ins. There is no playable game
or renderer yet. Per-milestone validation records document the actual tests;
these are synthetic-memory checks rather than a running-game comparison.

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

1. Reconstruct data-population routine `0x0014f6b8` and its field setters,
   then the remaining state services such as `0x0013a8d8`, `0x0012b744`,
   `0x0012b808`, and `0x0013a7bc`. The outer constructor `0x0039a5a8`,
   base initializer `0x0039daf4`, allocation path `0x0039a8a4`, parameter
   normalizer `0x0014f324`, and XOR primitive `0x00167fb4` now have host
   implementations through explicit service boundaries. Copy-before-cleanup
   behavior is traced; a full original-format Pokémon object is not yet
   produced by the host chain.
2. Reconstruct setup function `0x15c75434` and its required callees, keeping
   field names provisional until callers, consumers, or runtime behavior
   support them. Establish the table's actual record count and field meanings.
3. Replace the remaining stand-ins with checked implementations as their
   dependencies become understood. Add reference execution comparisons;
   the current tests alone do not prove equivalence to the running game.
4. In parallel, document RomFS assets and identify the minimal map, model,
   texture, animation, UI, and audio loading paths for the playable slice.
5. Establish the native app and platform interfaces, then connect the
   smallest complete field-to-battle-to-field path.

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

Plan for a substantial, likely multi-year effort for a small experienced
team, with uncertainty about full feature coverage. A defensible schedule
requires the first playable slice and measurements of actual dependency
and compatibility work. There is no evidence yet for a near-term complete
port or a reliable completion percentage.

Useful parallel workstreams are ARM/game reverse engineering, graphics and
platform integration, tools/assets, and reference-based testing. Progress
should be reported through completed playable scenarios and retired
unknown dependencies, alongside technical source milestones.
