Message-resource reproduction, 2026-10-25

The three implemented functions are 0013a19c, 0013ff50 and 0014e984.
Seek 001381c4, read 00127b6c and release 0013a50c are evidence-only dependencies.
Their underlying filesystem, allocator and virtual services remain external.

Synthetic C checks require no game input or analysis tools:

  make -C reconstructed/message_resources test

Verify the retained instructions, pseudocode, literals, section mappings and
live REA byte responses against the pinned original main image:

  python3 reconstructed/message_resources/reproduction/verify_rea.py \
    --code inputs/pokemon-x-0f6d3e1d66cb/code.bin

Optional comparison with original English game and story text, from repo root:

  cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror \
    reconstructed/message_resources/reproduction/check_messages.c \
    reconstructed/message_resources/message_resources.c \
    reconstructed/pokemon_names/pokemon_names.c \
    reconstructed/pokemon_data/pokemon_codec.c \
    reconstructed/pokemon_constructor/pokemon_constructor.c \
    reconstructed/pokemon_constructor/pokemon_parameters.c \
    native/assets/garc.c native/assets/gf_text.c native/platform/guest.c \
    -o reconstructed/message_resources/build/check_messages
  reconstructed/message_resources/build/check_messages ROMFS

This checker loads a/0/7/4 and a/0/8/2 through the native GARC reader, decodes
the reference units with the previous native GF text decoder, and runs every
line through resident, archive-cached and streamed reconstructed loading on
guest memory. It compares every supplied unit, calculates token-aware length
separately, checks entry metadata and guest faults, and exercises 721 species
name builds in each mode. Files and generated buffers remain local; retained
record-comparison.json stores counts and archive hashes, not payloads.

Live REA MCP review

See docs/REA_MCP.md for the package and launcher. This run negotiated REA 6.8.0,
MCP 2025-06-18 and Ghidra 12.1.4. It reopened the same main-image ELF and immutable
profile as outputs/rea-live/analysis-snapshot.json. Uncached function requests
started a fresh ephemeral Program with default analysis, ARM:LE:32:v6/default,
and the same seven analyst function seeds. Existing Ghidra projects were not
opened or saved. Analysis snapshots retain observations, not the live Program.

The local run is outputs/rea-messages/. It contains raw JSON-RPC requests and
replies, logs, the exported full Evidence bundle and a closed analysis snapshot.
The analyzed ELF SHA-256 is:
  fe2a17e1009d2b8d46b160ace54702aaf104d4092b2c04eff9edd261ac631bba

To repeat, prepare a fresh target using scripts/prepare_rea_target.py, launch
scripts/run_rea_mcp.sh with REA_GHIDRA_SEED_FILE selecting the generated seeds,
then call open_binary on that ELF with provider_id=ghidra. analyze_function
with detail=complete at the six addresses above, and call list_segments.
Retain live bytes using these read_bytes(address,length) pairs:
  0013a19c,516   0013ff50,388   0014e984,92
  001381c4,80    00127b6c,108   0013a50c,156   0013a3a0,4
The 388-byte decode range includes its key literal at 001400d0.
Finally export_evidence_bundle and close_binary(snapshot_path=NEW_PATH).

evidence/rea-evidence.json retains eleven complete core/function/load/memory
observations; dependencies.json contains derived dependency extracts with their
source Evidence IDs. The full original dependency Evidence remains in the
local export. functions.json distinguishes implemented routines from dependencies.
The verifier checks all 1,336 instruction bytes (992 implemented) and eight
literal bytes against both the live reads and original input, including
instruction text/pseudocode and ExHeader section mapping/permissions/BSS.

Earlier twelve reconstruction manifests and their 659 file hashes are preserved.
Allocation failure retains old text; active overflow clears it. Control-token
zeros do not terminate length calculation. Invalid or malformed resource memory
is outside the valid-memory contract. Static analysis and host/resource checks
do not establish original ARM or running-game execution equivalence.
