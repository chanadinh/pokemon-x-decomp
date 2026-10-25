#include "guest.h"

#include <stdlib.h>
#include <string.h>

void px_guest_init(struct px_guest *g)
{
    memset(g, 0, sizeof *g);
    g->alignment = PX_ALIGN_STRICT;
}

void px_guest_free(struct px_guest *g)
{
    for (unsigned i = 0; i < g->region_count; ++i)
        if (g->regions[i].owned) free(g->regions[i].bytes);
    px_guest_init(g);
}

static int add(struct px_guest *g, uint32_t base, uint32_t size, unsigned access, uint8_t *bytes, int owned,
               const char *name)
{
    if (!size || g->region_count >= PX_GUEST_MAX_REGIONS || (uint64_t)base + size > UINT64_C(0x100000000))
        return 0;
    for (unsigned i = 0; i < g->region_count; ++i) {
        const struct px_guest_region *r = &g->regions[i];
        if ((uint64_t)base < (uint64_t)r->base + r->size && (uint64_t)r->base < (uint64_t)base + size) return 0;
    }
    struct px_guest_region *r = &g->regions[g->region_count++];
    r->base = base; r->size = size; r->bytes = bytes; r->access = access; r->owned = owned;
    {
        size_t n = name ? strlen(name) : 0;
        if (n >= sizeof r->name) n = sizeof r->name - 1;
        memcpy(r->name, name ? name : "", n);
        r->name[n] = 0;
    }
    return 1;
}

int px_guest_map(struct px_guest *g, uint32_t base, uint32_t size, unsigned access, const void *init,
                 size_t init_size, const char *name)
{
    if (init_size > size) return 0;
    uint8_t *bytes = calloc(size ? size : 1, 1);
    if (!bytes) return 0;
    if (init && init_size) memcpy(bytes, init, init_size);
    if (!add(g, base, size, access, bytes, 1, name)) { free(bytes); return 0; }
    return 1;
}

int px_guest_map_view(struct px_guest *g, uint32_t base, uint32_t size, unsigned access, void *bytes,
                      const char *name)
{
    return bytes && add(g, base, size, access, bytes, 0, name);
}

static struct px_guest_region *find(struct px_guest *g, uint32_t address)
{
    for (unsigned i = 0; i < g->region_count; ++i) {
        struct px_guest_region *r = &g->regions[i];
        if (address - r->base < r->size) return r;
    }
    return NULL;
}

uint8_t *px_guest_span(struct px_guest *g, uint32_t address, uint32_t size)
{
    struct px_guest_region *r = find(g, address);
    if (!r || (uint64_t)(address - r->base) + size > r->size) return NULL;
    return r->bytes + (address - r->base);
}

static void fault(struct px_guest *g, enum px_guest_fault kind, uint32_t address, uint32_t width, int write)
{
    if (g->fault == PX_FAULT_NONE) {
        g->fault = kind; g->fault_address = address; g->fault_width = width; g->fault_write = write;
    }
    ++g->fault_count;
}

/* Alignment for a `width`-byte access: strict wants natural alignment; ARMv6
 * accepts unaligned halfwords and words and wants doublewords word-aligned. */
static int aligned(const struct px_guest *g, uint32_t address, uint32_t width)
{
    if (g->alignment == PX_ALIGN_ARMV6) return width < 8 || address % 4 == 0;
    return address % width == 0;
}

/* Resolve every byte first, so that a faulting access changes nothing. */
static int access(struct px_guest *g, uint32_t address, uint32_t width, int write, uint8_t *bytes[8])
{
    if (!aligned(g, address, width)) { fault(g, PX_FAULT_ALIGNMENT, address, width, write); return 0; }
    for (uint32_t k = 0; k < width; ++k) {
        const uint32_t a = address + k;
        if (a < address) { fault(g, PX_FAULT_UNMAPPED, address, width, write); return 0; } /* wrapped */
        struct px_guest_region *r = find(g, a);
        if (!r) { fault(g, PX_FAULT_UNMAPPED, address, width, write); return 0; }
        if (!(r->access & (write ? PX_GUEST_WRITE : PX_GUEST_READ))) {
            fault(g, PX_FAULT_PERMISSION, address, width, write); return 0;
        }
        bytes[k] = r->bytes + (a - r->base);
    }
    return 1;
}

static uint64_t load(struct px_guest *g, uint32_t address, uint32_t width)
{
    uint8_t *b[8];
    if (!access(g, address, width, 0, b)) return 0;
    uint64_t v = 0;
    for (uint32_t k = 0; k < width; ++k) v |= (uint64_t)*b[k] << (8 * k);
    return v;
}

static void store(struct px_guest *g, uint32_t address, uint32_t width, uint64_t value)
{
    uint8_t *b[8];
    if (!access(g, address, width, 1, b)) return;
    for (uint32_t k = 0; k < width; ++k) *b[k] = (uint8_t)(value >> (8 * k));
}

uint8_t px_guest_load8(struct px_guest *g, uint32_t a) { return (uint8_t)load(g, a, 1); }
uint16_t px_guest_load16(struct px_guest *g, uint32_t a) { return (uint16_t)load(g, a, 2); }
uint32_t px_guest_load32(struct px_guest *g, uint32_t a) { return (uint32_t)load(g, a, 4); }
uint64_t px_guest_load64(struct px_guest *g, uint32_t a) { return load(g, a, 8); }
void px_guest_store8(struct px_guest *g, uint32_t a, uint8_t v) { store(g, a, 1, v); }
void px_guest_store16(struct px_guest *g, uint32_t a, uint16_t v) { store(g, a, 2, v); }
void px_guest_store32(struct px_guest *g, uint32_t a, uint32_t v) { store(g, a, 4, v); }
void px_guest_store64(struct px_guest *g, uint32_t a, uint64_t v) { store(g, a, 8, v); }

void px_guest_clear_fault(struct px_guest *g)
{
    g->fault = PX_FAULT_NONE; g->fault_address = g->fault_width = 0; g->fault_write = 0; g->fault_count = 0;
}

const char *px_guest_fault_string(enum px_guest_fault f)
{
    switch (f) {
    case PX_FAULT_NONE: return "none";
    case PX_FAULT_UNMAPPED: return "unmapped address";
    case PX_FAULT_PERMISSION: return "access not permitted";
    case PX_FAULT_ALIGNMENT: return "misaligned access";
    }
    return "unknown fault";
}

static uint32_t le32(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }

int px_guest_map_image(struct px_guest *g, const uint8_t *code, size_t code_size, const uint8_t *ex, size_t ex_size)
{
    if (!code || !ex || ex_size < 0x40) return 0;
    /* Exheader system control info: text {address, pages, size} at 0x10,
     * rodata at 0x20, data at 0x30, bss size at 0x3c (3dbrew layout). */
    const uint32_t text = le32(ex + 0x10), text_pages = le32(ex + 0x14), text_size = le32(ex + 0x18);
    const uint32_t ro = le32(ex + 0x20), ro_pages = le32(ex + 0x24), ro_size = le32(ex + 0x28);
    const uint32_t data = le32(ex + 0x30), data_pages = le32(ex + 0x34), data_size = le32(ex + 0x38);
    const uint32_t bss = le32(ex + 0x3c);
    const uint64_t total = ((uint64_t)text_pages + ro_pages + data_pages) * 4096;
    if (total != code_size || text_size > (uint64_t)text_pages * 4096 || ro_size > (uint64_t)ro_pages * 4096 ||
        data_size > (uint64_t)data_pages * 4096 || ro != text + text_pages * 4096 ||
        data != ro + ro_pages * 4096) return 0;
    const uint32_t data_end = (uint32_t)(((uint64_t)data + data_size + bss + 4095) / 4096 * 4096);
    return px_guest_map(g, text, text_pages * 4096, PX_GUEST_READ | PX_GUEST_EXECUTE, code, text_pages * 4096u,
                        "text") &&
           px_guest_map(g, ro, ro_pages * 4096, PX_GUEST_READ, code + (ro - text), ro_pages * 4096u, "rodata") &&
           px_guest_map(g, data, data_end - data, PX_GUEST_READ | PX_GUEST_WRITE, code + (data - text),
                        data_pages * 4096u, "data+bss");
}

static uint8_t api_load8(void *u, uint32_t a) { return px_guest_load8(u, a); }
static uint16_t api_load16(void *u, uint32_t a) { return px_guest_load16(u, a); }
static uint32_t api_load32(void *u, uint32_t a) { return px_guest_load32(u, a); }
static void api_store8(void *u, uint32_t a, uint8_t v) { px_guest_store8(u, a, v); }
static void api_store16(void *u, uint32_t a, uint16_t v) { px_guest_store16(u, a, v); }
static void api_store32(void *u, uint32_t a, uint32_t v) { px_guest_store32(u, a, v); }

struct px_pokemon_api px_guest_pokemon_api(struct px_guest *g)
{
    struct px_pokemon_api a;
    memset(&a, 0, sizeof a);
    a.user = g;
    a.load8 = api_load8; a.load16 = api_load16; a.load32 = api_load32;
    a.store8 = api_store8; a.store16 = api_store16; a.store32 = api_store32;
    return a;
}
