/* The filesystem service: mounts, path rules, read-only archives, atomic
 * writes. Uses a temporary directory given on the command line. */
#include "fs.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    assert(argc == 2);
    const char *dir = argv[1];
    char path[1024], name[64];
    snprintf(path, sizeof path, "%s/file.bin", dir);
    FILE *f = fopen(path, "wb");
    assert(f && fwrite("abc", 1, 3, f) == 3 && !fclose(f));
    px_fs_reset();
    uint8_t *data; size_t size;
    assert(px_fs_read_all(PX_FS_ROMFS, "file.bin", 16, &data, &size) == PX_FS_NOT_MOUNTED);
    assert(px_fs_mount(PX_FS_ROMFS, dir, 0) == PX_FS_OK && px_fs_mounted(PX_FS_ROMFS));
    assert(px_fs_read_all(PX_FS_ROMFS, "file.bin", 16, &data, &size) == PX_FS_OK && size == 3 && !memcmp(data, "abc", 3));
    free(data);
    assert(px_fs_read_all(PX_FS_ROMFS, "file.bin", 2, &data, &size) == PX_FS_TOO_LARGE && !data);
    assert(px_fs_read_all(PX_FS_ROMFS, "none.bin", 16, &data, &size) == PX_FS_NOT_FOUND);
    assert(px_fs_exists(PX_FS_ROMFS, "file.bin") && !px_fs_exists(PX_FS_ROMFS, "a/b"));
    /* Nothing leaves its archive. */
    const char *bad[] = {"", "/file.bin", "../file.bin", "a/../file.bin", "a//b", "./file.bin", "a\\b", "c:x", "a/"};
    for (unsigned i = 0; i < sizeof bad / sizeof *bad; ++i) {
        assert(!px_fs_path_valid(bad[i]));
        assert(px_fs_read_all(PX_FS_ROMFS, bad[i], 16, &data, &size) == PX_FS_BAD_PATH);
    }
    assert(px_fs_path_valid("a/0/1/2") && px_fs_path_valid("DllBattle.cro"));
    /* The RomFS is read-only; the save archive writes atomically. */
    assert(px_fs_write_all(PX_FS_ROMFS, "file.bin", "x", 1) == PX_FS_READ_ONLY);
    snprintf(path, sizeof path, "%s/game.sav", dir);
    assert(px_fs_mount_file(PX_FS_SAVE, path, 1, name, sizeof name) == PX_FS_OK && !strcmp(name, "game.sav"));
    assert(px_fs_write_all(PX_FS_SAVE, name, "save", 4) == PX_FS_OK);
    assert(px_fs_read_all(PX_FS_SAVE, name, 16, &data, &size) == PX_FS_OK && size == 4 && !memcmp(data, "save", 4));
    free(data);
    snprintf(path, sizeof path, "%s/game.sav.tmp", dir);
    assert(!fopen(path, "rb")); /* the temporary file is gone */
    assert(px_fs_mount_file(PX_FS_EXEFS, "code.bin", 0, name, sizeof name) == PX_FS_OK && !strcmp(name, "code.bin"));
    assert(px_fs_mount_file(PX_FS_EXEFS, "dir/", 0, name, sizeof name) == PX_FS_BAD_PATH);
    assert(!strcmp(px_fs_result_string(PX_FS_READ_ONLY), "archive is read-only"));
    assert(!strcmp(px_fs_archive_name(PX_FS_SAVE), "save"));
    puts("filesystem tests passed");
    return 0;
}
