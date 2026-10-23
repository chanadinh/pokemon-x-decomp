Pokemon HP/stat evidence reproduction

Use the pinned Linux Ghidra 12.1.4 and full JDK 21.0.13+9 recorded in
tools/versions.json. The original input, project, and installed tools remain
local ignored prerequisites. The native compiler setup is recorded in the
immutable pokemon_state/reproduction/local-toolchain.json checkpoint.

From the workspace root:

python3 reconstructed/pokemon_stats/reproduction/export_stats.py \
  --project projects/cro_20261023T150920079989Z.gpr \
  --project-owner agent \
  --output outputs/pokemon-stats/reproduce-new

The output directory must not exist. --workspace, --ghidra, and --jdk allow
other local locations. --project-owner sets the JVM user.name to the owner
recorded in this restored project's project.prp; omit for a project already
owned by the current user. It does not change ownership metadata.

The launcher uses -process main_text.bin -noanalysis -readOnly, without flow
repair or project save. Ghidra may create transient locks and cache files.
reviewed-command.json records the headless arguments. JAVA_HOME and PATH
select the pinned JDK; JAVA_TOOL_OPTIONS sets user.home to tools/runtime-state
and user.name to agent. XDG_CONFIG_HOME and XDG_CACHE_HOME use its config/cache.

The initial export contained 44 functions. An attempted expanded export
failed because 001689fc is not an independent function in the preserved
project: its complete shared nature-adjustment tail is already exported
inside 00150034. The final list removes that separate request and adds the
existing growth field getter at 0016b45c, for 45 function exports. There was
no function-boundary repair. The extra-level reader at 00176268 is similarly
included inside the 00168ae4 export. Stat-update exports include setter tails.
Some exported dependencies remain service boundaries, not new implementations.

Verify the retained evidence:

python3 reconstructed/pokemon_stats/reproduction/verify_evidence.py \
  --code inputs/pokemon-x-0f6d3e1d66cb/code.bin \
  --evidence reconstructed/pokemon_stats/evidence

The verifier checks the pinned full input hash first, then every exported
instruction and PC-relative literal at address minus 0x00100000. The retained
provenance.json records counts and ranges. --write-provenance emits that file
when preparing a new evidence directory; omit when verifying the checkpoint.
This does not verify or embed the nature table or other game resources.

Tests use synthetic ordinary logical memory and resources. No original ARM
code executes in them, and they do not establish running-game equivalence.
