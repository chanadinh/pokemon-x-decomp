# Local analysis workspace notes

These notes preserve the original analysis history. Paths under `inputs/`,
`outputs/`, and `projects/`, and named ZIP bundles, refer to local artifacts;
they are not public downloads. Run commands below from the repository root.

The seventh milestone is [Pokémon state](../reconstructed/pokemon_state/README.md):
base/extra initialization, derived-state sequencing, refresh, and 13 field
accessors. Its local WSL continuation restored and verified the checkpoint,
installed the pinned Linux Ghidra/JDK, and exported additional evidence without
saving project changes. See its reproduction and validation records for current
local commands; the older notes below retain their original analysis history.

The Git repository tracks the analysis tools, reconstructed host source,
tests, reviewed evidence, and provenance. References below to `inputs/`,
`outputs/`, `projects/`, installed tools, and ZIP downloads describe local
workspace artifacts; those files are excluded from Git. The host C tests
use synthetic data and do not require the extracted game files.

The twelfth milestone now uses [live REA MCP](REA_MCP.md) with the pinned
Ghidra/JDK for [move-resource selection and caching](../reconstructed/pokemon_move_resource/README.md).
It analyzes an original-byte ELF carrier in a fresh ephemeral database;
the earlier saved project and eleven source checkpoints remain unchanged.
The older direct-Ghidra commands below retain their historical meaning.

The thirteenth milestone extends that workflow to
[message resources](../reconstructed/message_resources/README.md): language
selection, cached/streamed loading, decoding and token-aware length. Its
original-data comparison checks all 35,212 lines of the English game/story
archives in three modes, and connects the loader to the name builder. Seek/read
and allocation/lifetime services remain external.

The fourteenth milestone adds [personal-data selection and fields](../reconstructed/pokemon_personal/README.md).
Its four routines copy a selected 64-byte species/form row, dispatch 46 fields,
and follow the current-record cell. The learnset loader now has an integration
fixture using those real implementations. The original-data sweep checks all
799 rows, 1,607 form/boundary selections and 24,675 learnset entries. Table
loading, allocation and resource initialization remain external.

The fifteenth milestone adds [growth-resource selection and caching](../reconstructed/pokemon_growth/README.md).
Its five host routines read a species/form growth rate through the personal
service, then find, load or store a 404-byte experience curve. The level
resolver now has an integration fixture using them. The original-data sweep
checks 1,607 selections, 163,914 thresholds and 482,100 level resolutions over
all eight curves. Archive access and allocation remain external.

The sixteenth milestone adds [archive member access and file streams](../reconstructed/archive_io/README.md).
Nine routines look up resident member information, lock/seek/read members and
manage a 64-bit stream position against a cached file size. The original-data
sweep reads all 44,512 members of 271 GARC archives through that path. Archive
construction, platform calls and locking remain external.

The seventeenth milestone adds [archive index construction](../reconstructed/archive_index/README.md).
It parses the GARC index, builds the resident member rows (padded span, record
length, absolute offset) for a language-like selector, and opens the growth
and move archives. Rebuilding all 271 original indexes matches an independent
FATB reading in all 45,824 rows. The constructor and OS services remain external.

The eighteenth milestone adds [resource initialization](../reconstructed/pokemon_resources/README.md).
It boots the Pokémon data resources: the personal table from the flat final
member, the learnset/growth archives, descriptors, records and caches. From that
state alone the reconstructed services match the original data for all 884
species/form selections. Message resources and the archive constructor remain external.

The nineteenth milestone adds [message construction](../reconstructed/message_construction/README.md).
It builds, opens and destroys message resources and boots the move resources.
Every original text member is constructed in both modes, and all 70,424 lines
load through it identically to the native decoder. Archive constructors remain external.

The twentieth milestone is the first from a CRO module: the [battle damage
calculation](../reconstructed/battle_damage/README.md) in `DllBattle.cro`. A carrier of
the main image plus relocated battle modules was analysed through REA; the routine was
found by the damage formula's constants and matched in the running game, where two
attacks replay call by call through the reconstruction.

The twenty-first milestone adds the [attack and defense helpers](../reconstructed/battle_stats/README.md)
that supply the formula's A and D terms, including battler substitution, the
critical-hit stage rule, the stat swap and sandstorm. Its live trace is prepared.

The twenty-second milestone adds [type effectiveness](../reconstructed/battle_types/README.md):
the chart lookup with Inverse Battle, the dual-type combination and damage scaling,
compared over all 324 type pairs with the chart decoded from the original module.

The twenty-third milestone adds the [type matchup](../reconstructed/battle_matchup/README.md):
packed types with Roost, the per-type event, every defending type, airborne Ground
immunity and the immunity message.

The twenty-fourth milestone adds the [event system](../reconstructed/battle_event/README.md):
the event variable stack, dispatch and the handler walk with deferred removal.

The twenty-fifth milestone adds [handler registration](../reconstructed/battle_handler/README.md):
the handler pool, sorted registration and the ability, item, move and field lookup
tables, with every original table row registered through the reconstruction.

The sixth source milestone implements Pokémon data population, 26 field setters,
the payload checksum, manager encoding/decoding, and block selection. See
[pokemon_data](../reconstructed/pokemon_data/README.md) for the storage layout,
service boundaries, and tests. Its integration fixture connects all six modules
using synthetic species and growth resources. Larger state routines and real
resource loading remain to be reconstructed. `PokemonX-pokemon-data-source.zip`
contains the six host-source milestones with reviewed evidence and tests.

The fifth source milestone adds the constructor path and its creation-parameter
normalizer, including random-default resolution, bounded selections, level
clamping, buffer allocation, the extra-data XOR transform, and state handling.
See [pokemon_constructor](../reconstructed/pokemon_constructor/README.md) for
reviewed behavior and its service interfaces. Its original integration test
connects five modules. `PokemonX-pokemon-constructor-source.zip` contains those five
host-source milestones with reviewed evidence and tests.

The fourth source milestone adds the per-record encounter builder,
population loop, and config selector. See
[encounter_builder](../reconstructed/encounter_builder/README.md) for callback
contracts, copied-data ownership findings, and exact validation scope.
`PokemonX-encounter-builder-source.zip` contains all four milestones.

The third host-source milestone follows the prepared request into task
creation and state initialization. See
[battle_task](../reconstructed/battle_task/README.md) for the reconstructed
consumer, exact callback boundaries, and data-lifetime findings.
`PokemonX-battle-task-source.zip` contains all three connected milestones.

See [ROADMAP.md](../ROADMAP.md) for the native PC game milestones, completion
criteria, current position, and next reconstruction targets.

The second host-source milestone reconstructs wild-battle record preparation:
[wild_battle_setup](../reconstructed/wild_battle_setup/README.md) connects the
earlier wrapper to verified buffer initialization, packed-record decoding,
and the final context update. Wider game-state setup and battle submission
remain stand-ins. Download `PokemonX-wild-battle-source.zip` for both pieces,
tests, and reviewed assembly evidence.

The first small host C reconstruction is available in
[reconstructed/call_wild_battle](../reconstructed/call_wild_battle/README.md).
It reproduces a reviewed script wrapper's argument handling and call order,
with explicit stand-ins for unreconstructed helpers. It compiles and runs
isolated behavior tests; it does not yet run a battle or provide a PC game.

The module upload is now validated: **133 CROs, one CRS, and one CRR**. See
[the module analysis report](../outputs/modules-20261023T145553Z/MODULE_PASS.md)
for the combined code map, relocation audit, gameplay leads, and focused
decompilation results. Inputs are in
`inputs/pokemon-x-modules-a035d9775a1f/`; results are in
`outputs/modules-20261023T145553Z/`.

Download `PokemonX-module-analysis.zip` for the final inventory, relocation
audit, and 230 distinct pseudocode exports. The combined project has 44,279
candidate functions. A proven false no-return classification was corrected
before the final export; 28 exported functions still carry warning comments.
The saved project is `projects/cro_20261023T150920079989Z.gpr` with its matching
`.rep` directory. The input bytes remain unchanged.

The notes below describe the preceding main-executable pass and extraction
workflow, which is now complete for the uploaded modules.

The first analysis of the uploaded Pokémon X executable is complete. Ghidra
identified **15,576 candidate functions** and produced C-like pseudocode for
**107 distinct functions**: the first 100 by address, plus seven additional
functions selected through references and assembly review. This is an analysis
baseline; the output is not yet buildable source or a PC port.

See [the first-pass report](../outputs/20261023T142251572337Z/FIRST_PASS.md) for
verified findings, input hashes, memory mapping, and analysis limitations.
Downloadable bundles are `PokemonX-first-pass.zip` and
`PokemonX-modules-kit.zip` in this directory.

## Inputs and retained results

The user successfully ran the original Windows extraction helper and uploaded
`PokemonX-analysis-upload.zip`. Its three entries passed archive validation,
and the code/header sizes and SHA-256 hashes match the extraction manifest.

- Inputs: `inputs/pokemon-x-0f6d3e1d66cb/`.
- Analysis: `outputs/20261023T142251572337Z/`.
- Saved Ghidra project: `projects/code_20261023T142251572337Z.gpr`, with its
  companion `.rep` directory.
- Tool versions and provenance: [tools/versions.json](../tools/versions.json).

The ExHeader identifies program `0004000000055D00`, application `kujira-1`.
The full 2 GiB cartridge image remains on the user's Windows computer. Its
reported hash has not been independently checked in this workspace. Exact game
revision and region are not established by the imported header alone.

## Module extraction workflow (completed)

The main executable contains **84 distinct CRO resource paths**. The upload did
not include the external CRO modules, static CRS, or CRR registration data.
The helper below collected these files from the existing extraction.

Extract `PokemonX-modules-kit.zip` into
`D:\Downloads\PokemonX-modules-kit`, then run:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "D:\Downloads\PokemonX-modules-kit\Extract-PokemonX-Modules.ps1"
```

The helper defaults to the user's existing folder:

```text
D:\Downloads\PokemonX-analysis-20261023-102030-ff2c2690
```

If the earlier helper stopped with `Unable to find type
[IO.Compression.ZipArchiveMode]`, replace it with the updated script and append
`-ResumeLatest` to the command. This checks the saved extraction manifest and
module hashes, then packages the completed RomFS without extracting it again.
The updated helper explicitly loads both .NET compression assemblies before
starting extraction. An abandoned partial ZIP is left untouched; an existing
complete ZIP is never overwritten.

It reuses `work/main.cxi` and the pinned `tool/3dstool.exe`, extracts RomFS into
a fresh folder, and prints the path to `PokemonX-modules-upload.zip`. Upload
that ZIP for the next analysis pass. It contains CRO/CRS/CRR files with their
paths and hashes, plus extraction provenance. Separately installed updates are
outside this extraction's scope. Allow roughly twice the RomFS size in
additional disk space; the helper prints its estimate before extraction.

If the previous folder moved, append
`-ExtractionFolder "D:\path\PokemonX-analysis-folder"` to the command.
The helper reads the previous extraction and retains the new RomFS image and
tree for inspection. It performs no downloads or uploads. No administrator
session is needed; the execution-policy option applies to this process only.

## Reproduce the main-executable pass

The pinned Ghidra 12.1.4 and JDK 21 installations are shared with the earlier
Omega Ruby workspace through local symlinks. From this directory:

```bash
python3 scripts/run_decompile.py \
  --code inputs/pokemon-x-0f6d3e1d66cb/code.bin \
  --exheader inputs/pokemon-x-0f6d3e1d66cb/exheader.bin \
  --decompressed --max-functions 100
```

Each run creates a new output directory and project. The initial export covers
the first 100 functions; the additional anchor exports and diagnostic summaries
in the recorded run were produced in a separate review step. The workflow uses
Ghidra directly; REA was not used for this analysis.

## Verification and limitations

Run the tracked test suites from the repository root:

```sh
python3 -m unittest discover -s tests -v
make -C reconstructed test
```

The extraction integration tests require local PowerShell and 3dstool
installations at `tools/pwsh/pwsh` and `tools/3dstool-linux/3dstool`, or paths
provided through `POKEMON_X_PWSH` and `POKEMON_X_3DSTOOL`. They skip when these
tools are unavailable. The fixtures contain invented code and assets, and
PowerShell's Linux state stays in each test's temporary directory. These
checks do not rerun the full Ghidra analysis or verify a playable game.

All 21 intake validation tests passed during setup. The original extraction
helper passed three synthetic integration cases under PowerShell 7.4.6 and real
3dstool 1.2.6 on Linux, and the user subsequently reported a successful real
Windows extraction. The new module helper passed a synthetic integration test
that checks exact module bytes, hidden `.crr` paths, hashes, asset exclusion,
and preservation of the input. Recovery from a leftover partial ZIP also passed
without running the extractor; altered module bytes were rejected. The original
helper failed at ZIP creation on Windows because the compression assembly was
not loaded. Native Windows verification of the updated helper is pending.

The real Ghidra run finished without a timeout. Assembly review confirmed the
startup BSS-clearing routine and one ARM-to-Thumb call. Automatic analysis also
reported 80 function-body repair failures and other warnings, documented in the
report. Function boundaries, types, and pseudocode remain subject to review.

Sources: [3DS extraction workflow](https://github.com/ihaveamac/3DS-rom-tools/wiki/Extract-a-game-or-application-in-.3ds-or-.cci-format),
[3dstool source](https://github.com/dnasdw/3dstool/tree/v1.2.6),
[ExHeader definitions](https://github.com/devkitPro/libctru/blob/master/libctru/include/3ds/exheader.h).
