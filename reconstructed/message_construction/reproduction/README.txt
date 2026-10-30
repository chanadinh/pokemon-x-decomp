Message-construction reproduction, 2026-10-30

Implemented: 0011e264, 0011e308, 0012ad74, 0038fc20, 0038fbc4, 00112894,
0012b968 and 0011edec. Helpers 0011c3e8, 0013a50c and 002ff520 are reviewed
dependencies.

  make -C reconstructed/message_construction test

Original-byte/live-REA evidence verification:

  python3 reconstructed/message_construction/reproduction/verify_rea.py \
    --code inputs/pokemon-x-0f6d3e1d66cb/code.bin

Optional original resource comparison, from the repo root (R=reconstructed):

  cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror \
    $R/message_construction/reproduction/check_construction.c \
    $R/message_construction/message_construction.c \
    $R/pokemon_resources/pokemon_resources.c $R/archive_index/archive_index.c \
    $R/archive_io/archive_io.c $R/message_resources/message_resources.c \
    $R/pokemon_move_resource/pokemon_move_resource.c \
    native/assets/garc.c native/assets/gf_text.c native/platform/guest.c \
    -o $R/message_construction/build/check_construction
  $R/message_construction/build/check_construction ROMFS

The checker serves a/0/7/4, a/0/8/2 and a/2/1/2 through the reconstructed file
stream and archive index. It constructs a message resource for every non-empty
text member in streamed and cached modes, loads every line with px_load_message
and compares the units and metadata with the native text decoder, then destroys
the resource. It boots the move resources from a/2/1/2 and reads five byte
fields of every move through px_get_move_resource_value. No original payloads
are retained here.
