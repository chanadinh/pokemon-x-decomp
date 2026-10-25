Name evidence reproduction, 2026-10-25

Original inputs, installed tools and the saved project are local prerequisites.
Use pinned Ghidra 12.1.4/JDK 21 from tools/versions.json. From the repo root:

python3 reconstructed/pokemon_names/reproduction/export_names.py \
  --project projects/cro_20261023T150920079989Z.gpr \
  --project-owner agent --output outputs/pokemon-names/repeat

The output directory must not exist. The wrapper reuses
pokemon_stats/reproduction/ExportFunctionSet.java with -process main_text.bin
-noanalysis -readOnly and does not repair or save the project. Alternate
paths use --workspace, --ghidra and --jdk. Omit --project-owner on a project
already owned by the current user. command.json/headless.log record each run.

The initial run exported eight functions, then a second read-only run exported
0013ff50 as text-decryption dependency evidence. The retained evidence combines
both lists and literal records; the final wrapper exports all nine in order.
No source implementing 0013a19c or 0013ff50 is claimed by this milestone.

For a fresh evidence directory, copy the .asm/.c exports, functions.json and
literals.json and generate provenance with the shared verifier:

python3 reconstructed/pokemon_stats/reproduction/verify_evidence.py \
  --code inputs/pokemon-x-0f6d3e1d66cb/code.bin \
  --evidence NEW_EVIDENCE_DIRECTORY --write-provenance

Verify the retained evidence plus the name writer's fall-through to encoding:

python3 reconstructed/pokemon_names/reproduction/verify_names.py \
  --code inputs/pokemon-x-0f6d3e1d66cb/code.bin

All retained instructions/literals are checked at address minus 00100000 after
checking the full pinned input hash. Some Ghidra pseudocode incorporates tails
beyond an exported instruction body; the writer's following encoding routine
is separately exported and checked. No original ARM execution is involved.

Host tests: make -C reconstructed test. On this WSL installation first source
tools/native-sdl/activate.sh. Validation metadata records tested hashes and
commands. The native original-data check lives in native/reproduction/name-services.
