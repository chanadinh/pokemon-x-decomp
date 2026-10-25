Move-resource service reproduction (2026-10-25)

No game files, installed tools or generated analysis databases are tracked.
The six core routines were analyzed through live REA stdio MCP, not by reusing
the preceding Ghidra exports. See docs/REA_MCP.md for the local setup and the
analysis-only ELF carrier. The existing saved Ghidra project is never opened.

Verify the retained original-byte and live-memory comparisons:

  python3 reconstructed/pokemon_move_resource/reproduction/verify_rea.py \
    --code inputs/pokemon-x-0f6d3e1d66cb/code.bin

Run synthetic host tests:

  make -C reconstructed/pokemon_move_resource test

Optional original move-archive comparison, from the repository root:

  cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror \
    reconstructed/pokemon_move_resource/reproduction/check_records.c \
    reconstructed/pokemon_move_resource/pokemon_move_resource.c \
    native/assets/garc.c native/assets/pokedata.c \
    -o reconstructed/pokemon_move_resource/build/check_records
  reconstructed/pokemon_move_resource/build/check_records ROMFS/a/2/1/2

All 618 records must be 36 bytes. The independent native record decoder
supplies semantic expected values; every selector 0..32 is compared.
record-comparison.json retains counts, not original record payloads.

The local initial session is outputs/rea-live/: JSON-RPC requests/responses,
full evidence-bundle.json, analysis-snapshot.json, doctor.json, and tool logs.
Its ELF SHA-256 is fe2a17e1009d2b8d46b160ace54702aaf104d4092b2c04eff9edd261ac631bba.
REA server 6.8.0 negotiated MCP 2025-06-18. Ghidra 12.1.4 used the default
analyzer preset, ARM:LE:32:v6/default, and seven explicit analyst function seeds.
The first query imports/analyzes the entire main image; later requests reuse it.

To repeat the live review, prepare a fresh target with scripts/prepare_rea_target.py,
start scripts/run_rea_mcp.sh with REA_GHIDRA_SEED_FILE pointing at its seeds.tsv,
then call open_binary(path=the generated ELF, provider_id=ghidra).
Call analyze_function(detail=complete) for 00168bbc, 00176328, 001762b4,
00174eb4, 00174f30 and 0017c5d8. read_bytes at these ranges retains the code:
  00168bbc +152, 00176328 +552, 001762b4 +116,
  00174eb4 +124, 00174f30 +92, 0017c5d8 +40.
The field range includes its 132-byte jump table; the wrapper/loader ranges
include their literal words. Also call list_segments to check permissions and
BSS mapping. The fresh ELF omits CRO linking/runtime state. Function names
seeded before analysis are analyst assertions, not recovered symbols.
Finally export_evidence_bundle and close_binary(snapshot_path=NEW_LOCAL_PATH).
The retained evidence includes all core function and memory observations;
dependencies.json is a derived extract, with links to the full local bundle.

The archive reader and resource lifecycle remain external. Tests do not run
the original ARM functions or the game. Older reproduction records remain
historical and unchanged.
