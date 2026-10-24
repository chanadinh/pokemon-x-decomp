/* Local validation bridge for verify_talk.py.
 *
 *   dump_talk SCRIPT.bin ID...   one JSON line per script id: the
 *                                px_zone_talk run result, steps, yields,
 *                                native calls and message lines
 *   dump_talk --text TEXT.bin    one JSON line: every line rendered by
 *                                px_gf_text_render (UTF-8, player "PLAYER")
 * Output may contain local asset data; keep it in ignored outputs/. */
#include "../../assets/gf_script.h"
#include "../../assets/gf_text.h"
#include "../../assets/zone_talk.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t *read_file(const char *path, size_t *size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    uint8_t *data = NULL;
    if (!fseek(f, 0, SEEK_END)) {
        const long n = ftell(f);
        if (n >= 0 && !fseek(f, 0, SEEK_SET) && (data = malloc(n ? (size_t)n : 1)) &&
            fread(data, 1, (size_t)n, f) != (size_t)n) { free(data); data = NULL; }
        *size = (size_t)n;
    }
    fclose(f);
    return data;
}
static void json_string(const char *s)
{
    putchar('"');
    for (const unsigned char *p = (const unsigned char *)s; *p; ++p) {
        if (*p == '"' || *p == '\\') printf("\\%c", *p);
        else if (*p == '\n') printf("\\n");
        else if (*p == '\f') printf("\\f");
        else if (*p < 32) printf("\\u%04x", *p);
        else putchar(*p);
    }
    putchar('"');
}
int main(int argc, char **argv)
{
    if (argc < 3) { fprintf(stderr, "usage: %s SCRIPT.bin ID... | --text TEXT.bin\n", argv[0]); return 2; }
    size_t size = 0;
    if (!strcmp(argv[1], "--text")) {
        uint8_t *data = read_file(argv[2], &size);
        if (!data) return 1;
        struct px_gf_text t;
        const enum px_gf_text_result r = px_gf_text_decode(data, size, &t);
        printf("{\"result\":%d,\"lines\":[", (int)r);
        static char buffer[1 << 16];
        for (uint32_t i = 0; i < t.line_count; ++i) {
            if (i) putchar(',');
            if (px_gf_text_render(&t, i, "PLAYER", buffer, sizeof buffer) < 0) printf("null");
            else json_string(buffer);
        }
        printf("]}\n");
        px_gf_text_free(&t); free(data);
        return 0;
    }
    uint8_t *data = read_file(argv[1], &size);
    if (!data) return 1;
    struct px_gf_script s;
    const enum px_gf_script_result r = px_gf_script_decode(data, size, &s);
    if (r != PX_GF_SCRIPT_OK) { printf("{\"decode\":%d}\n", (int)r); free(data); return 0; }
    for (int i = 2; i < argc; ++i) {
        struct px_zone_talk talk;
        px_zone_talk(&s, (int32_t)strtol(argv[i], NULL, 10), 1, &talk);
        printf("{\"result\":%d,\"steps\":%" PRIu32 ",\"yields\":%" PRIu32 ",\"natives\":%" PRIu32 ",\"lines\":[",
               (int)talk.run.result, talk.run.steps, talk.run.yields, talk.native_calls);
        for (uint32_t k = 0; k < talk.line_count; ++k) printf("%s%" PRId32, k ? "," : "", talk.lines[k]);
        printf("]}\n");
    }
    px_gf_script_free(&s); free(data);
    return 0;
}
