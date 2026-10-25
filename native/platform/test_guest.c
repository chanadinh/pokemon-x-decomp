/* Guest memory: little-endian values on any host, region permissions,
 * alignment modes, faults that change nothing, the process image. */
#include "guest.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    struct px_guest g;
    px_guest_init(&g);
    assert(px_guest_map(&g, 0x1000, 16, PX_GUEST_READ | PX_GUEST_WRITE, "\x01\x02\x03\x04\x05\x06\x07\x08", 8, "a"));
    assert(px_guest_map(&g, 0x1010, 16, PX_GUEST_READ, NULL, 0, "b"));
    assert(!px_guest_map(&g, 0x1008, 16, PX_GUEST_READ, NULL, 0, "overlap"));
    assert(!px_guest_map(&g, 0xfffffff0u, 0x20, PX_GUEST_READ, NULL, 0, "wrap"));
    assert(!px_guest_map(&g, 0x3000, 0, PX_GUEST_READ, NULL, 0, "empty"));
    /* Little-endian, assembled byte by byte. */
    assert(px_guest_load16(&g, 0x1000) == 0x0201 && px_guest_load32(&g, 0x1004) == 0x08070605);
    assert(px_guest_load64(&g, 0x1000) == UINT64_C(0x0807060504030201));
    px_guest_store32(&g, 0x1008, 0xa1b2c3d4);
    uint8_t *span = px_guest_span(&g, 0x1008, 4);
    assert(span && span[0] == 0xd4 && span[3] == 0xa1 && !px_guest_span(&g, 0x100c, 8));
    assert(g.fault == PX_FAULT_NONE);
    /* Strict alignment (the default): a misaligned word faults and is not stored. */
    px_guest_store32(&g, 0x1002, 0xffffffff);
    assert(g.fault == PX_FAULT_ALIGNMENT && g.fault_address == 0x1002 && g.fault_width == 4 && g.fault_write);
    assert(px_guest_load32(&g, 0x1000) == 0x04030201);
    px_guest_clear_fault(&g);
    /* ARMv6: unaligned halfwords and words, doublewords need word alignment. */
    g.alignment = PX_ALIGN_ARMV6;
    assert(px_guest_load32(&g, 0x1001) == 0x05040302 && g.fault == PX_FAULT_NONE);
    assert(px_guest_load64(&g, 0x1002) == 0 && g.fault == PX_FAULT_ALIGNMENT);
    px_guest_clear_fault(&g);
    /* An access may span adjacent regions; permission is checked per byte. */
    px_guest_store16(&g, 0x100e, 0xcafe);
    assert(px_guest_load32(&g, 0x100e) == 0x0000cafe); /* two bytes from each region */
    assert(g.fault == PX_FAULT_NONE);
    px_guest_store32(&g, 0x100e, 0x11111111); /* the upper two bytes are read-only: nothing stored */
    assert(g.fault == PX_FAULT_PERMISSION && px_guest_load16(&g, 0x100e) == 0xcafe);
    px_guest_clear_fault(&g);
    assert(px_guest_load8(&g, 0x2000) == 0 && g.fault == PX_FAULT_UNMAPPED && g.fault_count == 1);
    assert(px_guest_load8(&g, 0x2001) == 0 && g.fault_address == 0x2000 && g.fault_count == 2); /* the first is kept */
    assert(!strcmp(px_guest_fault_string(PX_FAULT_UNMAPPED), "unmapped address"));
    /* A view writes through to host memory. */
    uint8_t host[4] = {0};
    assert(px_guest_map_view(&g, 0x4000, 4, PX_GUEST_READ | PX_GUEST_WRITE, host, "view"));
    px_guest_store16(&g, 0x4002, 0xbeef);
    assert(host[2] == 0xef && host[3] == 0xbe);
    /* The reconstructed modules' interface. */
    const struct px_pokemon_api api = px_guest_pokemon_api(&g);
    assert(api.load32(api.user, 0x1008) == 0xa1b2c3d4 && !api.allocate && !api.populate_data);
    px_guest_free(&g);
    assert(g.region_count == 0);
    /* The process image: three page-aligned segments from an exheader. */
    static uint8_t code[3 * 4096 + 2 * 4096 + 4096], ex[2048];
    for (size_t i = 0; i < sizeof code; ++i) code[i] = (uint8_t)(i / 4096);
    const uint32_t fields[] = {0x00100000, 3, 3 * 4096 - 12, 0, 0x00103000, 2, 5000, 0, 0x00105000, 1, 100, 9000};
    for (unsigned i = 0; i < 12; ++i)
        for (unsigned k = 0; k < 4; ++k) ex[0x10 + 4 * i + k] = (uint8_t)(fields[i] >> (8 * k));
    px_guest_init(&g);
    assert(px_guest_map_image(&g, code, sizeof code, ex, sizeof ex) && g.region_count == 3);
    assert(px_guest_load8(&g, 0x00100000) == 0 && px_guest_load8(&g, 0x00104000) == 4 && px_guest_load8(&g, 0x00105000) == 5);
    px_guest_store8(&g, 0x00103000, 1);
    assert(g.fault == PX_FAULT_PERMISSION);                     /* rodata */
    px_guest_clear_fault(&g);
    px_guest_store8(&g, 0x00105000 + 100 + 8999, 7);            /* the last bss byte */
    assert(g.fault == PX_FAULT_NONE && px_guest_load8(&g, 0x00105000 + 100 + 8999) == 7);
    assert(!strcmp(g.regions[0].name, "text") && (g.regions[0].access & PX_GUEST_EXECUTE));
    px_guest_free(&g);
    px_guest_init(&g);
    assert(!px_guest_map_image(&g, code, sizeof code - 1, ex, sizeof ex));
    assert(!px_guest_map_image(&g, code, sizeof code, ex, 0x20));
    px_guest_free(&g);
    puts("guest memory tests passed");
    return 0;
}
