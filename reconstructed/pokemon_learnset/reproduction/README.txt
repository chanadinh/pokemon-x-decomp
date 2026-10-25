Learnset evidence reproduction, 2026-10-25

Use the pinned local Ghidra 12.1.4 and JDK 21 installations described in
tools/versions.json. Inputs and the saved project are local prerequisites.
From the repository root, export to a fresh directory:

python3 reconstructed/pokemon_learnset/reproduction/export_learnset.py \
  --project projects/cro_20261023T150920079989Z.gpr \
  --project-owner agent --output outputs/pokemon-learnset/repeat

The wrapper uses pokemon_stats/reproduction/ExportFunctionSet.java with
-process main_text.bin -noanalysis -readOnly, without repair or project save.
--project-owner is needed only for the restored local project's recorded owner;
omit it on a project already owned by the current user. Alternate paths use
--workspace, --ghidra and --jdk. The output directory must not already exist.

The initial local run exported nine learnset functions, followed by a second
read-only run for five compiler/resource dependencies. Their instruction,
pseudocode, metadata and literal records are combined in evidence/. The final
wrapper exports all 14 in that same order in one session. Count and cache-store
tails are included in their wrapper exports, not requested as separate functions.

For a fresh evidence directory, copy the .asm/.c files, functions.json and
literals.json, then generate provenance with the shared generic verifier:

python3 reconstructed/pokemon_stats/reproduction/verify_evidence.py \
  --code inputs/pokemon-x-0f6d3e1d66cb/code.bin \
  --evidence NEW_EVIDENCE_DIRECTORY --write-provenance

Verify the retained record and the two personal-selector dispatch entries:

python3 reconstructed/pokemon_learnset/reproduction/verify_learnset.py \
  --code inputs/pokemon-x-0f6d3e1d66cb/code.bin

The generic verifier checks the full pinned input hash and every retained
instruction/literal at address minus 00100000. Some dependency pseudocode
incorporates shared compiler tails not included in its assembly body. These
helpers are evidence for the memory-copy/remainder boundary, not new complete
compiler-helper reconstructions or ARM execution comparisons.

Host tests: make -C reconstructed test. On this WSL setup first source
tools/native-sdl/activate.sh. Validation records give exact source hashes,
test results, original-data comparison and preservation checks.
