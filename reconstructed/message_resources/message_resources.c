#include "message_resources.h"

static void clear_string(uint32_t string, const struct px_pokemon_api *m)
{
    m->store16(m->user, m->load32(m->user, string + UINT32_C(4)), 0);
    m->store16(m->user, string + UINT32_C(10), 0);
}

uint32_t px_game_string_length(uint32_t source, const struct px_pokemon_api *m)
{
    uint32_t length = 0;
    if (m->load16(m->user, source) == 0) return 0;
    do {
        const uint32_t span = m->load16(m->user, source) == UINT16_C(0x10)
            ? (uint32_t)m->load16(m->user, source + UINT32_C(2)) + UINT32_C(2)
            : UINT32_C(1);
        source += span * UINT32_C(2);
        length += span;
        if (length >= UINT32_C(0x800)) break;
    } while (m->load16(m->user, source) != 0);
    return length;
}

static int append_unit(uint32_t string, uint16_t unit, const struct px_pokemon_api *m)
{
    const uint32_t old_length = m->load16(m->user, string + UINT32_C(10));
    const uint32_t capacity = m->load16(m->user, string + UINT32_C(8));
    const uint32_t next = old_length + UINT32_C(1);
    if (next < capacity) {
        const uint32_t buffer = m->load32(m->user, string + UINT32_C(4));
        m->store16(m->user, string + UINT32_C(10), (uint16_t)next);
        m->store16(m->user, buffer + old_length * UINT32_C(2), unit);
        const uint32_t current_length = m->load16(m->user, string + UINT32_C(10));
        const uint32_t current_buffer = m->load32(m->user, string + UINT32_C(4));
        m->store16(m->user, current_buffer + current_length * UINT32_C(2), 0);
        return 1;
    }
    if (next == capacity && unit == 0) {
        const uint32_t buffer = m->load32(m->user, string + UINT32_C(4));
        m->store16(m->user, string + UINT32_C(10), (uint16_t)next);
        m->store16(m->user, buffer + old_length * UINT32_C(2), 0);
        return 1;
    }
    clear_string(string, m);
    return 0;
}

void px_decode_message(uint32_t resource, uint32_t source, uint32_t units,
                        uint32_t index, uint32_t string, const struct px_pokemon_api *m)
{
    if (m->load8(m->user, string + UINT32_C(12)) != 0) clear_string(string, m);
    const uint32_t header = m->load32(m->user, resource + UINT32_C(24));
    const int encrypted = m->load32(m->user, header + UINT32_C(8)) == 0;
    uint32_t key = ((index + UINT32_C(3)) * UINT32_C(0x2983)) & UINT32_C(0xffff);
    for (uint32_t i = 0; i < units; ++i) {
        uint32_t active;
        uint16_t unit;
        if (encrypted) {
            unit = m->load16(m->user, source);
            active = m->load8(m->user, string + UINT32_C(12));
        } else {
            active = m->load8(m->user, string + UINT32_C(12));
            unit = m->load16(m->user, source);
        }
        if (active == 0) return;
        if (encrypted) unit ^= (uint16_t)key;
        if (!append_unit(string, unit, m)) return;
        if (encrypted) key = ((key << 3) | (key >> 13)) & UINT32_C(0xffff);
        source += UINT32_C(2);
    }
    if (m->load8(m->user, string + UINT32_C(12)) != 0)
        m->store16(m->user, string + UINT32_C(10),
                   (uint16_t)px_game_string_length(m->load32(m->user, string + UINT32_C(4)), m));
}

uint32_t px_load_message(uint32_t resource, uint32_t index, uint32_t string,
                         const struct px_message_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const uint32_t header = m->load32(m->user, resource + UINT32_C(24));
    if (index >= m->load16(m->user, header + UINT32_C(2))) {
        if (m->load8(m->user, string + UINT32_C(12)) != 0) clear_string(string, m);
        return 0;
    }
    const uint32_t language_count = m->load16(m->user, header);
    uint32_t language = m->load32(m->user, UINT32_C(0x005d113c));
    if (language_count <= language) language = 0;
    language &= UINT32_C(0xff);
    if (m->load32(m->user, resource + UINT32_C(20)) != language) {
        const uint32_t cached = m->load8(m->user, resource + UINT32_C(12));
        m->store32(m->user, resource + UINT32_C(20), language);
        if (cached != 0) {
            const uint32_t archive = m->load32(m->user, resource + UINT32_C(8));
            const uint32_t section_offset = m->load32(m->user, header + UINT32_C(12) + language * UINT32_C(4));
            const uint32_t member = m->load32(m->user, resource + UINT32_C(16));
            if (archive == 0) {
                m->store32(m->user, resource + UINT32_C(28), header + section_offset);
            } else {
                api->seek(api->user, archive, member, section_offset);
                api->read(api->user, archive, 4, m->load32(m->user, resource + UINT32_C(28)));
                api->seek(api->user, archive, member, section_offset);
                const uint32_t destination = m->load32(m->user, resource + UINT32_C(28));
                api->read(api->user, archive, m->load32(m->user, destination), destination);
            }
        }
    }

    const uint32_t entry_offset = UINT32_C(4) + index * UINT32_C(8);
    uint32_t entry;
    if (m->load8(m->user, resource + UINT32_C(12)) != 0) {
        entry = m->load32(m->user, resource + UINT32_C(28)) + entry_offset;
    } else {
        const uint32_t selected = m->load32(m->user, resource + UINT32_C(20));
        const uint32_t current_header = m->load32(m->user, resource + UINT32_C(24));
        const uint32_t section = m->load32(m->user, current_header + UINT32_C(12) + selected * UINT32_C(4));
        const uint32_t archive = m->load32(m->user, resource + UINT32_C(8));
        const uint32_t member = m->load32(m->user, resource + UINT32_C(16));
        api->seek(api->user, archive, member, section + entry_offset);
        api->read(api->user, m->load32(m->user, resource + UINT32_C(8)), 8, resource + UINT32_C(32));
        entry = resource + UINT32_C(32);
    }

    if (m->load8(m->user, resource + UINT32_C(12)) != 0) {
        const uint32_t section = m->load32(m->user, resource + UINT32_C(28));
        const uint32_t source = m->load32(m->user, entry) + section;
        const uint32_t units = m->load16(m->user, entry + UINT32_C(4));
        px_decode_message(resource, source, units, index, string, m);
    } else {
        const uint32_t bytes = (uint32_t)m->load16(m->user, entry + UINT32_C(4)) * UINT32_C(2);
        const uint32_t heap = m->load32(m->user, resource + UINT32_C(4));
        const uint32_t alignment = api->alignment(api->user, heap);
        const uint32_t buffer = api->allocate(api->user, heap, bytes, UINT32_C(0) - alignment, 1);
        if (buffer != 0) {
            const uint32_t selected = m->load32(m->user, resource + UINT32_C(20));
            const uint32_t current_header = m->load32(m->user, resource + UINT32_C(24));
            const uint32_t relative = m->load32(m->user, entry);
            const uint32_t section = m->load32(m->user, current_header + UINT32_C(12) + selected * UINT32_C(4));
            const uint32_t member = m->load32(m->user, resource + UINT32_C(16));
            const uint32_t archive = m->load32(m->user, resource + UINT32_C(8));
            api->seek(api->user, archive, member, relative + section);
            api->read(api->user, m->load32(m->user, resource + UINT32_C(8)), bytes, buffer);
            const uint32_t units = m->load16(m->user, entry + UINT32_C(4));
            px_decode_message(resource, buffer, units, index, string, m);
            api->release(api->user, buffer);
        }
    }
    return m->load16(m->user, entry + UINT32_C(6));
}
