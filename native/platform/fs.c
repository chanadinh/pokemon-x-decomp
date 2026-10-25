#include "fs.h"

#include <stdlib.h>
#include <string.h>

struct mount { char root[1024]; int mounted, writable; };
static struct mount mounts[PX_FS_ARCHIVES];

void px_fs_reset(void) { memset(mounts, 0, sizeof mounts); }

enum px_fs_result px_fs_mount(enum px_fs_archive a, const char *root, int writable)
{
    if ((unsigned)a >= PX_FS_ARCHIVES || !root || !*root) return PX_FS_BAD_PATH;
    const size_t n = strlen(root);
    if (n >= sizeof mounts[a].root) return PX_FS_BAD_PATH;
    memcpy(mounts[a].root, root, n + 1);
    while (strlen(mounts[a].root) > 1 && mounts[a].root[strlen(mounts[a].root) - 1] == '/')
        mounts[a].root[strlen(mounts[a].root) - 1] = 0;
    mounts[a].mounted = 1; mounts[a].writable = writable;
    return PX_FS_OK;
}

enum px_fs_result px_fs_mount_file(enum px_fs_archive a, const char *host_file, int writable, char *name,
                                   size_t name_size)
{
    if (!host_file || !*host_file || !name || !name_size) return PX_FS_BAD_PATH;
    const char *slash = strrchr(host_file, '/');
    const char *base = slash ? slash + 1 : host_file;
    if (!px_fs_path_valid(base) || strlen(base) >= name_size) return PX_FS_BAD_PATH;
    char root[1024];
    if (!slash) snprintf(root, sizeof root, ".");
    else if (slash == host_file) snprintf(root, sizeof root, "/");
    else {
        const size_t n = (size_t)(slash - host_file);
        if (n >= sizeof root) return PX_FS_BAD_PATH;
        memcpy(root, host_file, n); root[n] = 0;
    }
    memcpy(name, base, strlen(base) + 1);
    return px_fs_mount(a, root, writable);
}

int px_fs_mounted(enum px_fs_archive a) { return (unsigned)a < PX_FS_ARCHIVES && mounts[a].mounted; }

int px_fs_path_valid(const char *path)
{
    if (!path || !*path || strlen(path) > 255) return 0;
    const char *start = path;
    for (const char *p = path;; ++p) {
        if (*p == '\\' || *p == ':' || (*p && (unsigned char)*p < 32)) return 0;
        if (*p != '/' && *p) continue;
        const size_t length = (size_t)(p - start);
        if (!length || (length == 1 && *start == '.') || (length == 2 && start[0] == '.' && start[1] == '.'))
            return 0;
        if (!*p) return 1;
        start = p + 1;
    }
}

static enum px_fs_result host_path(enum px_fs_archive a, const char *path, char *out, size_t size)
{
    if (!px_fs_mounted(a)) return PX_FS_NOT_MOUNTED;
    if (!px_fs_path_valid(path)) return PX_FS_BAD_PATH;
    const int n = strcmp(mounts[a].root, "/") ? snprintf(out, size, "%s/%s", mounts[a].root, path)
                                               : snprintf(out, size, "/%s", path);
    return n > 0 && (size_t)n < size ? PX_FS_OK : PX_FS_BAD_PATH;
}

enum px_fs_result px_fs_open(enum px_fs_archive a, const char *path, FILE **out)
{
    if (out) *out = NULL;
    char host[1300];
    const enum px_fs_result r = host_path(a, path, host, sizeof host);
    if (r != PX_FS_OK) return r;
    FILE *f = fopen(host, "rb");
    if (!f) return PX_FS_NOT_FOUND;
    *out = f;
    return PX_FS_OK;
}

int px_fs_exists(enum px_fs_archive a, const char *path)
{
    FILE *f = NULL;
    if (px_fs_open(a, path, &f) != PX_FS_OK) return 0;
    fclose(f);
    return 1;
}

enum px_fs_result px_fs_read_all(enum px_fs_archive a, const char *path, size_t max, uint8_t **data, size_t *size)
{
    *data = NULL; *size = 0;
    FILE *f = NULL;
    enum px_fs_result r = px_fs_open(a, path, &f);
    if (r != PX_FS_OK) return r;
    long n = -1;
    if (!fseek(f, 0, SEEK_END)) n = ftell(f);
    if (n < 0 || fseek(f, 0, SEEK_SET)) { fclose(f); return PX_FS_IO_ERROR; }
    if ((unsigned long)n > max) { fclose(f); return PX_FS_TOO_LARGE; }
    uint8_t *buffer = malloc(n ? (size_t)n : 1);
    if (!buffer) { fclose(f); return PX_FS_NO_MEMORY; }
    const size_t got = fread(buffer, 1, (size_t)n, f);
    fclose(f);
    if (got != (size_t)n) { free(buffer); return PX_FS_IO_ERROR; }
    *data = buffer; *size = got;
    return PX_FS_OK;
}

enum px_fs_result px_fs_write_all(enum px_fs_archive a, const char *path, const void *data, size_t size)
{
    char host[1300], temporary[1310];
    const enum px_fs_result r = host_path(a, path, host, sizeof host);
    if (r != PX_FS_OK) return r;
    if (!mounts[a].writable) return PX_FS_READ_ONLY;
    snprintf(temporary, sizeof temporary, "%s.tmp", host);
    FILE *f = fopen(temporary, "wb");
    int ok = f && (!size || fwrite(data, 1, size, f) == size);
    if (f && fclose(f)) ok = 0;
    if (ok) ok = !rename(temporary, host);
    if (!ok) { remove(temporary); return PX_FS_IO_ERROR; }
    return PX_FS_OK;
}

const char *px_fs_result_string(enum px_fs_result r)
{
    switch (r) {
    case PX_FS_OK: return "success";
    case PX_FS_NOT_MOUNTED: return "archive not mounted";
    case PX_FS_BAD_PATH: return "invalid archive path";
    case PX_FS_NOT_FOUND: return "file not found";
    case PX_FS_READ_ONLY: return "archive is read-only";
    case PX_FS_TOO_LARGE: return "file too large";
    case PX_FS_IO_ERROR: return "input/output error";
    case PX_FS_NO_MEMORY: return "out of memory";
    }
    return "unknown result";
}

const char *px_fs_archive_name(enum px_fs_archive a)
{
    static const char *const names[] = {"romfs", "exefs", "save"};
    return (unsigned)a < PX_FS_ARCHIVES ? names[a] : "?";
}
