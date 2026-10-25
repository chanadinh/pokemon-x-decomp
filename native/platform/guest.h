#ifndef PX_GUEST_H
#define PX_GUEST_H
#include <stddef.h>
#include <stdint.h>

#include "../../reconstructed/pokemon_constructor/pokemon_constructor.h"

/* The game's view of memory, kept on any host: a 32-bit address space whose
 * multi-byte values are little-endian (assembled byte by byte, whatever the
 * host's own order) and whose pointers are 32-bit guest addresses, never host
 * pointers. Reconstructed code reaches memory only through it.
 *
 * Regions are mapped at fixed guest addresses with read/write/execute
 * permissions, either owning a zeroed or copied buffer or viewing a host
 * buffer. Accesses resolve byte by byte, so an access may span adjacent
 * regions. A bad access does not stop the program: a load yields 0, a store
 * is dropped, and the first fault is recorded (kind, address, width).
 *
 * Alignment (ARM11 MPCore, ARMv6K): with PX_ALIGN_ARMV6 halfword and word
 * accesses may be unaligned (the ARMv6 unaligned-access model) while
 * doubleword accesses (LDRD/STRD) need word alignment; PX_ALIGN_STRICT
 * requires natural alignment for every access. Whether the game's kernel
 * runs with unaligned support enabled is not established here; the
 * reconstructed routines only make naturally aligned accesses in the cases
 * tested, which both modes accept, and PX_ALIGN_STRICT is the default so that
 * any deviation shows as a fault. */
enum { PX_GUEST_READ = 1, PX_GUEST_WRITE = 2, PX_GUEST_EXECUTE = 4, PX_GUEST_MAX_REGIONS = 24 };
enum px_guest_alignment { PX_ALIGN_STRICT = 0, PX_ALIGN_ARMV6 = 1 };
enum px_guest_fault { PX_FAULT_NONE = 0, PX_FAULT_UNMAPPED, PX_FAULT_PERMISSION, PX_FAULT_ALIGNMENT };
struct px_guest_region {
    uint32_t base, size;
    uint8_t *bytes;
    unsigned access;
    int owned;
    char name[24];
};
struct px_guest {
    struct px_guest_region regions[PX_GUEST_MAX_REGIONS];
    unsigned region_count;
    enum px_guest_alignment alignment;
    enum px_guest_fault fault;      /* the first fault */
    uint32_t fault_address, fault_width;
    int fault_write;
    uint64_t fault_count;
};
/* The 3DS application memory layout the platform uses: code at 0x00100000
 * (text, rodata, data then bss, as the exheader places them), the heap at
 * 0x08000000, the main thread's stack ending at 0x10000000. */
enum {
    PX_GUEST_CODE = 0x00100000u, PX_GUEST_HEAP = 0x08000000u, PX_GUEST_STACK_END = 0x10000000u
};

void px_guest_init(struct px_guest *guest);
void px_guest_free(struct px_guest *guest);
/* Owning map: zeroed, or a copy of `init` (init_size <= size). Fails (0) on
 * overlap, a zero size, address wrap-around or a full table. */
int px_guest_map(struct px_guest *guest, uint32_t base, uint32_t size, unsigned access, const void *init,
                 size_t init_size, const char *name);
/* A view of host memory (not copied, not freed). */
int px_guest_map_view(struct px_guest *guest, uint32_t base, uint32_t size, unsigned access, void *bytes,
                      const char *name);
/* The host bytes behind [address, address + size) when one region holds
 * them all (NULL otherwise); for bulk transfers by platform services. */
uint8_t *px_guest_span(struct px_guest *guest, uint32_t address, uint32_t size);

uint8_t px_guest_load8(struct px_guest *guest, uint32_t address);
uint16_t px_guest_load16(struct px_guest *guest, uint32_t address);
uint32_t px_guest_load32(struct px_guest *guest, uint32_t address);
uint64_t px_guest_load64(struct px_guest *guest, uint32_t address);
void px_guest_store8(struct px_guest *guest, uint32_t address, uint8_t value);
void px_guest_store16(struct px_guest *guest, uint32_t address, uint16_t value);
void px_guest_store32(struct px_guest *guest, uint32_t address, uint32_t value);
void px_guest_store64(struct px_guest *guest, uint32_t address, uint64_t value);
void px_guest_clear_fault(struct px_guest *guest);
const char *px_guest_fault_string(enum px_guest_fault fault);

/* The process image from code.bin and its exheader (2048 bytes): text
 * read/execute, rodata read-only, data and the zeroed bss read/write, each
 * at the exheader's address, page-rounded. code.bin's file offset of each
 * segment is its address minus the text address (the segments are stored
 * page-aligned and back to back). Returns 0 on a size mismatch. */
int px_guest_map_image(struct px_guest *guest, const uint8_t *code, size_t code_size, const uint8_t *exheader,
                       size_t exheader_size);

/* The reconstructed modules' memory interface over this guest (load/store
 * only; their service callbacks stay NULL for the caller to fill). */
struct px_pokemon_api px_guest_pokemon_api(struct px_guest *guest);
#endif
