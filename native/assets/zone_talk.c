#include "zone_talk.h"

#include <string.h>

struct context { struct px_zone_talk *out; int female; };

static int32_t native(void *ctx, uint32_t hash, const int32_t *args, uint32_t count, int *failed)
{
    struct context *c = ctx;
    (void)failed;
    ++c->out->native_calls;
    switch (hash) {
    case 0x9ADF1616u:
        if (count && c->out->line_count < PX_ZONE_TALK_MAX_LINES) c->out->lines[c->out->line_count++] = args[0];
        return 0;
    case 0x59008363u: return c->female ? 1 : 0;
    case 0x8E50F765u: return 0;
    case 0x15F86E59u: case 0xC8E7FB61u: return 1;
    default: return 0;
    }
}

void px_zone_talk(const struct px_gf_script *script, int32_t script_id, int female, struct px_zone_talk *out)
{
    memset(out, 0, sizeof *out);
    struct context c = {out, female};
    px_gf_script_run(script, 0xD7477C97u, script_id, native, &c, 200000, 500, &out->run);
}
