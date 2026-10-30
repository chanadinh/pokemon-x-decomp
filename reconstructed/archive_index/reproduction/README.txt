Archive-index reproduction, 2026-10-30

Implemented: 00127efc, 00128134, 00127eb4, 0011ecf8, 0011ee68, and GARC
parser methods 00355f0c, 00355f4c, 00495b60, 00495c24, 00495c30, 00495c3c,
00495cbc and 00495d40 (vtable 00597728). Helpers 00128288, 00123a6c,
0016f1e8, 0012b0c8, 0014663c and 001465e8 are reviewed dependencies.

  make -C reconstructed/archive_index test

Original-byte/live-REA evidence verification:

  python3 reconstructed/archive_index/reproduction/verify_rea.py \
    --code inputs/pokemon-x-0f6d3e1d66cb/code.bin

Optional original resource comparison, from the repo root:

  cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror \
    reconstructed/archive_index/reproduction/check_index.c \
    reconstructed/archive_index/archive_index.c \
    reconstructed/archive_io/archive_io.c \
    native/assets/garc.c native/platform/guest.c \
    -o reconstructed/archive_index/build/check_index
  reconstructed/archive_index/build/check_index ROMFS

The checker walks ROMFS/a. For each GARC it serves the original bytes through
the reconstructed file stream, builds the index with px_build_archive_index
(for selectors 0..32 when an entry has several records) and compares every
row, the count and the largest-member word with an independent FATB reading.
With selector zero it reads each member through the built table and compares
the bytes with the native GARC reader. No original payloads are retained here.
