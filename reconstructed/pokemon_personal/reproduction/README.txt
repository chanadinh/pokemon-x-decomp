Personal-data reproduction, 2026-10-25

Four implemented functions: 00174c0c, 00175c78, 00167e2c and 00167e18.
The selector reads an already loaded flat table; archive loading, allocation
and initialization remain external. The learnset integration calls these
reconstructed selection and field services through its existing API.

  make -C reconstructed/pokemon_personal test

Original-byte/live-REA evidence verification:

  python3 reconstructed/pokemon_personal/reproduction/verify_rea.py \
    --code inputs/pokemon-x-0f6d3e1d66cb/code.bin

Optional original resource comparison, from the repo root:

  cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror \
    reconstructed/pokemon_personal/reproduction/check_personal.c \
    reconstructed/pokemon_personal/pokemon_personal.c \
    reconstructed/pokemon_learnset/pokemon_learnset.c \
    native/assets/garc.c native/assets/pokedata.c native/platform/guest.c \
    -o reconstructed/pokemon_personal/build/check_personal
  reconstructed/pokemon_personal/build/check_personal ROMFS

The checker reads a/2/1/8 and confirms that its final flat-table member matches
all 799 independent 64-byte row members. It maps that table on guest memory,
compares all 46 selectors against the prior native decoder plus an independent
raw-field reference, then checks valid and boundary forms. It drives the
reconstructed learnset loader through the new personal selector/accessor and
compares the original a/2/1/4 entries. No original payloads are retained here.

Live MCP reproduction uses the workflow in docs/REA_MCP.md. The local run is
outputs/rea-personal/, with raw JSON-RPC requests/replies and retained analysis.
Open the same byte-verified ELF32 ARMv6 carrier with provider_id=ghidra and the
same seven analyst seeds. The immutable message-analysis snapshot can reuse
retained observations; uncached queries still start fresh whole-image Ghidra
analysis. Existing saved projects are never opened or saved.

Call analyze_function(detail=complete) for all four addresses, list_segments,
and read_bytes at:
  00174c0c,160   00175c78,828   00167e18,20   00167e2c,24
These ranges contain the function instructions, four literal words, and the
46-entry field jump table. Export Evidence and close_binary to a fresh snapshot
path when the review is complete. The tracked verifier checks original/live
instruction bytes, assembly text/pseudocode, target mappings and permissions.

All thirteen preceding reconstruction directories and their 688 file hashes
remain unchanged. Host tests and original-resource comparisons do not execute
the original ARM routines or establish running-game behavior.
