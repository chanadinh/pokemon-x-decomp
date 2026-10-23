Pokemon state evidence reproduction

Use the pinned Linux Ghidra 12.1.4 and full JDK 21.0.13+9 recorded in
tools/versions.json. Their downloaded archives were verified against those
SHA-256 hashes before extraction under ignored tools/ directories.
The game input and saved Ghidra project are local, ignored prerequisites.

From the workspace root:

python3 reconstructed/pokemon_state/reproduction/export_state.py \
  --project projects/cro_20261023T150920079989Z.gpr \
  --project-owner agent \
  --output outputs/pokemon-state/reproduce-new

The output directory must not exist. --workspace, --ghidra, and --jdk can
select other local locations. --project-owner sets the JVM user.name to the
owner recorded in a restored project's project.prp. Omit it for a project
already owned by the current user. The initial attempt without this option
failed with NotOwnerException; no project ownership metadata was changed.

The launcher uses -process main_text.bin -noanalysis -readOnly. It performs
no flow repair or project save. Ghidra may create transient locks and cache
files while opening the database. reviewed-command.json records the final
headless arguments; the owner option is a JVM environment setting, not a
headless argument. JAVA_HOME and PATH select the pinned JDK; JAVA_TOOL_OPTIONS
sets user.home to tools/runtime-state and user.name to agent. XDG_CONFIG_HOME
and XDG_CACHE_HOME point to tools/runtime-state/config and cache.

44 function exports include the four state orchestrators, their direct
callees, and the zero-fill tail at 00301a9c/00301aa0. Several exports are
explicit unresolved services, not host implementations. The four orchestrator
ASM files also match the constructor checkpoint. A second export reproduced
all 86 ASM/C files from the initial 43-function export; it additionally exports
the zero-fill tail and records conditional PC-relative literal loads.

Verify the retained evidence:

python3 reconstructed/pokemon_state/reproduction/verify_evidence.py \
  --code inputs/pokemon-x-0f6d3e1d66cb/code.bin \
  --evidence reconstructed/pokemon_state/evidence

Verification compares 5,176 instruction bytes and 64 literal bytes against
the pinned original input, using address minus 0x00100000. It verifies the
complete input hash first. --write-provenance emits a new provenance.json
after verification when preparing a new evidence directory; omit this flag
when checking the immutable retained checkpoint.

The public tests use synthetic logical memory and external service fixtures.
No original ARM code executes here, and no gameplay equivalence is claimed.
