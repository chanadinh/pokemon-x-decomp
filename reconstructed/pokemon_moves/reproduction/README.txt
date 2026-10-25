Move-service evidence reproduction, 2026-10-25

The original executable, saved project and tools are local prerequisites,
not files shipped with the repository. No new extraction is needed locally.
Use the pinned Ghidra 12.1.4/JDK 21 setup from tools/versions.json.

From the repository root, export to a new directory:

python3 reconstructed/pokemon_moves/reproduction/export_moves.py \
  --project projects/cro_20261023T150920079989Z.gpr \
  --project-owner agent --output outputs/pokemon-moves/repeat

The wrapper reuses pokemon_stats/reproduction/ExportFunctionSet.java with
-process main_text.bin -noanalysis -readOnly. It does not repair or save the
project. --project-owner is needed only for this restored local project;
--workspace, --jdk and --ghidra allow other installations. The output directory
must not exist; command.json and headless.log record the invocation.

To prepare a new evidence directory, copy the .asm/.c files, functions.json
and literals.json from the export, then run the existing generic verifier:

python3 reconstructed/pokemon_stats/reproduction/verify_evidence.py \
  --code inputs/pokemon-x-0f6d3e1d66cb/code.bin \
  --evidence NEW_EVIDENCE_DIRECTORY --write-provenance

To verify the retained record (including selector-5 dispatch and division):

python3 reconstructed/pokemon_moves/reproduction/verify_moves.py \
  --code inputs/pokemon-x-0f6d3e1d66cb/code.bin

The generic verifier checks the full pinned input hash, exported instructions
and PC-relative literal words. The new verifier also checks the switch-table
word that selects the byte +5 getter and the arithmetic reduction. The
dependency export includes shared getter tails; this does not mean the
cache/load service or all its selectors have host implementations.

Host tests require only C11 and Make. On this WSL setup first source
tools/native-build/activate.sh, then make -C reconstructed test.
The new suite integrates state initialization/refresh, slot fields, codec
and constructor helpers with the new routines. It is not a ninth-module
replacement for the earlier eight-module chain, which remains unchanged.
