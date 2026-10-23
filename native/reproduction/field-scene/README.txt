XY field selection and terrain inspection milestone
Recorded 2026-10-23.

Scope

Native C11 readers decode the bounded XY ZO/MM/GR subset and stored collision
triangles. The SDL2 field inspector follows zone 5 -> matrix 12 -> region 165,
draws its terrain grid, optionally overlays collision edges, and admits an
inspection marker only to exact raw bytes 20 00 00 00. Initial marker, focus,
colors, cardinal steps and first-triangle height readout are explicit
inspection policies. This is not reconstructed player movement or gameplay.

The selected field uses BCH graphics, not the CGFX path from earlier native
milestones. Environment rendering, area/prop bindings, player spawn, terrain
flag consumers, transitions, interaction, battle integration and save/load
remain outside this milestone. No new original input is required to continue
the next resource and code investigation.

Evidence inventory

baseline.json pins the previous 60-file native state at 97dd659 and records the
eight immutable game-logic checkpoints (539 checksum entries). Previous native
source can be reviewed at that commit. Every earlier native reproduction
record remains unchanged; all new evidence belongs to this directory.

reference-comparison.json records primary source pins/hashes, archive and
selected-member hashes, independent original-byte comparisons, coordinate
placement, exact standard-cell labels and limitations. All 6,400 terrain bytes
and 20,736 decoded XYZ bytes match independent reads. The 52 exact standard-cell
centers independently intersect geometry at Y=0. This is a geometric fact about
the selected data, not proof of the original collision algorithm.

Original-code evidence and ExportFieldEvidence.java document read-only saved
Ghidra inspection of the archive table, zone/matrix selection, GR loading and
script transition interface. Main addresses are baseline game addresses;
DllField addresses are synthetic saved-project addresses. The original
project is not modified. Generated exports remain in ignored local outputs.

validation.json / validation.txt record the integrated commands, results,
tested source hashes and immutable-checkpoint verification. rendered-sample.json
records the final desktop captures and their local-only hashes. Images, game
resources, downloaded references, SDK files and binaries are not tracked.

Reproduction

From a standard SDL2/C11 development environment:
  make -C native all test test-runtime
  make -C reconstructed test
  python3 -m unittest discover -s tests -v

On this WSL workspace, first source tools/native-sdl/activate.sh. The checked
RomFS root is documented in outputs/local-handoff/CONTINUE-IN-CODEX.txt; select
it explicitly, then run:
  native/build/pokemon_field_viewer --romfs PATH --zone 5 --inspect
A repeatable movement-policy example is --steps RRDRRRUU:
start (3,10), finish (8,11), six
accepted and two rejected steps. The selected original sample is not a test
fixture shipped in Git.

Re-export the original instruction evidence from a fresh output directory:
  python3 native/reproduction/field-scene/export_original_code.py \
    --project projects/cro_20261023T150920079989Z.gpr \
    --output outputs/field-evidence-repeat --project-owner agent
The script uses -noanalysis -readOnly and verifies the selected instruction
bytes against the pinned original inputs. Existing exports can be verified with:
  python3 native/reproduction/field-scene/verify_original_code.py \
    --exports outputs/native-field-scene/ghidra-session

The portable C test uses independently constructed offsets, asymmetric raw
words, camera bounds, LOD tables, sloped/vertical/overlapping triangles,
terminators, malformed counts/offsets/padding and cleanup cases. Python runtime
tests independently construct GARC/LZ/ZO/MM/GR chains and test pixel locations,
complete-word movement policy, matrix seams, absent regions, absent standard
cells, bad CLI/resources, exclusive output and symlink handling, clipped
extreme coordinates, and the cost of repeated collision instances.

The local validation harness is outputs/native-field-scene/validate.py.
Sanitizer builds use -fsanitize=address,undefined with detect_leaks=0 because
of the previously documented environment ptrace restriction. A passing run
does not claim LeakSanitizer coverage. The normal CI runs synthetic tests with
SDL's dummy video backend. Actual x11/software windows and the Windows-to-WSL
launcher are separately checked locally; a native Windows ABI is unverified.

See docs/native-field.md for API limits, layout and coordinate contracts,
the precise distinction between resource evidence and inspection policy,
and remaining first-playable boundaries.

Final integrated result

All 13 recorded commands passed: ten native C suites, 34 Python native
container/runtime tests, eight reconstructed suites and 38 analysis tests
(two optional dependency skips). Field parser and all nine field runtime
cases passed ASan/UBSan. Actual x11/software previews, input checks and the
Windows-to-WSL launcher passed. All 539 reconstruction hashes and 25 previous
native reproduction records remain unchanged. See validation.json for the
exact commands and tested source hashes.
