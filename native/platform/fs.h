#ifndef PX_FS_H
#define PX_FS_H
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/* The filesystem service: the game opens files inside archives, not host
 * paths. Each archive is mounted on a host directory: the RomFS (read-only),
 * the ExeFS inputs (code.bin and exheader.bin, read-only) and the save
 * archive (read/write). Paths are archive-relative: '/'-separated, no empty,
 * "." or ".." components, no '\\' or ':', at most 255 bytes, so nothing can
 * leave its archive. Writes replace a whole file atomically (a temporary
 * file renamed over the target), as the 3DS save archive commits. The mount
 * table is process-wide and set up before files are opened. */
enum px_fs_archive { PX_FS_ROMFS = 0, PX_FS_EXEFS, PX_FS_SAVE, PX_FS_ARCHIVES };
enum px_fs_result {
    PX_FS_OK = 0, PX_FS_NOT_MOUNTED, PX_FS_BAD_PATH, PX_FS_NOT_FOUND, PX_FS_READ_ONLY, PX_FS_TOO_LARGE,
    PX_FS_IO_ERROR, PX_FS_NO_MEMORY
};
void px_fs_reset(void);
/* root: a host directory ("." for the working directory). */
enum px_fs_result px_fs_mount(enum px_fs_archive archive, const char *root, int writable);
/* Mount the directory holding host_file and give its file name (for the
 * --save / --code-bin style options that name a file). */
enum px_fs_result px_fs_mount_file(enum px_fs_archive archive, const char *host_file, int writable,
                                   char *name, size_t name_size);
int px_fs_mounted(enum px_fs_archive archive);
int px_fs_path_valid(const char *path);
/* A read-only, seekable binary stream (the caller closes it). */
enum px_fs_result px_fs_open(enum px_fs_archive archive, const char *path, FILE **out);
int px_fs_exists(enum px_fs_archive archive, const char *path);
enum px_fs_result px_fs_read_all(enum px_fs_archive archive, const char *path, size_t max_bytes, uint8_t **data,
                                 size_t *size);
enum px_fs_result px_fs_write_all(enum px_fs_archive archive, const char *path, const void *data, size_t size);
const char *px_fs_result_string(enum px_fs_result result);
const char *px_fs_archive_name(enum px_fs_archive archive);
#endif
