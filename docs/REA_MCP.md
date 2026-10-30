# REA MCP in this workspace

REA 6.8.0 now supplies live reverse engineering through its stdio MCP server
and the existing Ghidra 12.1.4/JDK 21 installation. The first completed review
is [move resources](../reconstructed/pokemon_move_resource/README.md), followed
by [message resources](../reconstructed/message_resources/README.md). A local
JSON-RPC client performed MCP initialization, tool discovery, `open_binary`,
function analysis, memory reads, evidence export and snapshot/close operations.
This session did not change global agent configuration.

Install the pinned [official package](https://github.com/morluto/rea) with a
supported Node.js runtime (the verified local runtime is 24.22.1):

```sh
npm install --prefix tools/rea --save-exact --no-fund --no-audit rea-agents@6.8.0
```

The package, lockfile and installed dependencies stay under ignored `tools/`.
[tools/versions.json](../tools/versions.json) records its version/integrity.
The launcher accepts `REA_NODE` for an explicit Node executable; otherwise it
uses `node` on PATH or the local `tools/rea-runtime/bin/node` link.

REA starts a fresh ephemeral Ghidra database and does not attach to the existing
combined project. Prepare an analysis carrier from the original decompressed
code and its matching ExHeader:

```sh
python3 scripts/prepare_rea_target.py \
  --code inputs/pokemon-x-0f6d3e1d66cb/code.bin \
  --exheader inputs/pokemon-x-0f6d3e1d66cb/exheader.bin \
  --output outputs/rea-target-new
```

The output directory must be new. The script verifies the pinned input hashes
and copies each original section into a synthetic ELF32 ARM container at its
ExHeader address and permissions. BSS remains uninitialized. It verifies every
file-backed section byte and writes `target.json` and `seeds.tsv`. The ELF is
analysis input; it is not an original executable format or a native game build.
CRO relocations, CRO modules and initialized game state are absent.

Configure a local stdio MCP client to run the launcher with absolute paths:

```sh
REA_GHIDRA_SEED_FILE="/absolute/workspace/outputs/rea-target-new/seeds.tsv" \
  bash /absolute/workspace/scripts/run_rea_mcp.sh
```

The launcher writes MCP protocol output to stdout. Client calls begin with:

```json
{"name":"open_binary","arguments":{"path":"/absolute/workspace/outputs/rea-target-new/pokemon-x-main.elf","provider_id":"ghidra"}}
{"name":"analyze_function","arguments":{"procedure":"0x00168bbc","detail":"complete"}}
```

The launcher selects `ARM:LE:32:v6/default`; REA's generic raw ARM import selects
v7, so the ELF carrier preserves the established language and separate memory
permissions. The first provider query waits for default whole-image analysis;
the launcher allows 900,000 ms for startup. Allow corresponding time in the
MCP client. Later queries use the open database. Export evidence and call
`close_binary` with a new `snapshot_path` when the investigation is finished.

The initial local run is `outputs/rea-live/`. Its provider doctor check passed,
MCP negotiated `2025-06-18`, and 229 tools were advertised. Its raw requests,
replies, full evidence bundle and provider-neutral snapshot are retained there.
Reviewed core evidence is tracked in the milestone directory, with an executable
verifier. Seeds supply analyst function names and entry assertions. Decompiler
types still require assembly review. This workflow performs static analysis;
it does not run the game or establish running-game equivalence.

The message review is retained separately in `outputs/rea-messages/`. It
reopened the same target/profile with the previous immutable snapshot, then
queried uncached functions and memory through live MCP. Those queries start a
fresh Ghidra Program and whole-image analysis; a provider-neutral snapshot
retains observations rather than the live Ghidra database. Its core evidence
and dependency extracts have their own tracked verifier. All twelve preceding
reconstruction directories retain their recorded hashes.

The personal-data review is in `outputs/rea-personal/`, using the same target,
language/profile and a fresh Program for uncached queries. Its four implemented
routines and original-data validation are recorded in
[pokemon_personal](../reconstructed/pokemon_personal/README.md). The preceding
thirteen reconstruction checkpoints remain unchanged.

The growth-resource review is in `outputs/rea-growth/`, using the same target,
language/profile and seeds. Its first provider query again waited for
whole-image analysis; later queries returned in under a second. Five host
routines and two reviewed helper bodies are recorded in
[pokemon_growth](../reconstructed/pokemon_growth/README.md). The preceding
fourteen reconstruction checkpoints remain unchanged.

The archive review is in `outputs/rea-archive/`. Its first analysis again
waited about 7.5 minutes for whole-image analysis; later queries took under
0.3 seconds. Nine routines and three helper bodies are recorded in
[archive_io](../reconstructed/archive_io/README.md). The preceding fifteen
reconstruction checkpoints remain unchanged.

The archive-index review is in `outputs/rea-archive-init/`. It used `xrefs`
and `procedure_callers` to find the archive initializers. Ghidra had not created
functions at the GARC parser vtable targets, so those methods were decoded with
`inspect_native_instruction` one instruction at a time and retained with live
bytes. See [archive_index](../reconstructed/archive_index/README.md).

The resource-initialization review is in `outputs/rea-resources/`. Cross
references to every resource cell led to `0x00112368`; its constructors,
caches and the non-resident member lookup are recorded in
[pokemon_resources](../reconstructed/pokemon_resources/README.md).

The message-construction review is in `outputs/rea-messages-init/`. Each REA
session starts a fresh, ephemeral Ghidra project and repeats whole-image
analysis (about seven to eight minutes here); later queries take well under a
second. See [message_construction](../reconstructed/message_construction/README.md).

CRO modules use the carrier from `scripts/prepare_rea_module_target.py`: the main
image plus relocated module blocks from `scripts/prepare_cro_analysis.py` at their
synthetic addresses. The battle review is in `outputs/rea-battle/`; its first query
took about 9.4 minutes because more code is analysed. See
[battle_damage](../reconstructed/battle_damage/README.md).
