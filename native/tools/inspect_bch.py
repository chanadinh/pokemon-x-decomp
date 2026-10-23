#!/usr/bin/env python3
"""Bounded, read-only BCH v7 resource index inspection (no GPU decoding)."""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys

MAX_BYTES = 64 * 1024 * 1024
CATEGORIES = ('models', 'materials', 'shaders', 'textures', 'lookup_tables',
              'lights', 'cameras', 'fogs', 'skeletal_animations',
              'material_animations', 'visibility_animations', 'light_animations',
              'camera_animations', 'fog_animations', 'scenes')


def extract_gr_section(data, section):
    """Return one bounded raw GR section; offsets may include alignment gaps."""
    if len(data) > MAX_BYTES:
        raise ValueError('input exceeds 64 MiB')
    if len(data) < 8 or data[:2] != b'GR':
        raise ValueError('not a GR minipack')
    count = struct.unpack_from('<H', data, 2)[0]
    if not 1 <= count <= 256 or not 0 <= section < count:
        raise ValueError('invalid GR section count/selection')
    table_end = 4 + 4 * (count + 1)
    if len(data) < table_end:
        raise ValueError('truncated GR table')
    offsets = struct.unpack_from('<' + 'I' * (count + 1), data, 4)
    if (offsets[0] < table_end or offsets[-1] > len(data)
            or any(a > b for a, b in zip(offsets, offsets[1:]))):
        raise ValueError('invalid GR section extents')
    return data[offsets[section]:offsets[section + 1]]


def inspect(data):
    def span(offset, size, start=0, end=None):
        end = len(data) if end is None else end
        if offset < start or size < 0 or offset > end or size > end - offset:
            raise ValueError('extent outside section')

    def u32(offset):
        span(offset, 4)
        return struct.unpack_from('<I', data, offset)[0]

    if len(data) > MAX_BYTES:
        raise ValueError('input exceeds 64 MiB')
    span(0, 56)
    if data[:6] != b'BCH\0\x07\x07':
        raise ValueError('only BCH compatibility 7/7 is supported')
    names = ('contents', 'strings', 'commands', 'raw_data', 'relocation')
    sections = {}
    for i, name in enumerate(names):
        offset, size = u32(8 + i * 4), u32(28 + i * 4)
        span(offset, size)
        if size and offset < 56:
            raise ValueError('section overlaps header')
        sections[name] = {'offset': offset, 'size': size}
    occupied = sorted((s['offset'], s['offset'] + s['size'])
                      for s in sections.values() if s['size'])
    if any(a[1] > b[0] for a, b in zip(occupied, occupied[1:])):
        raise ValueError('overlapping sections')
    contents, strings = sections['contents'], sections['strings']
    base, end = contents['offset'], contents['offset'] + contents['size']
    span(base, 180, base, end)
    if sections['relocation']['size'] % 4:
        raise ValueError('unaligned relocation table')
    # Index fields are section-relative. No relocation is applied to input bytes.
    resources = {}
    total = 0
    for i, category in enumerate(CATEGORIES):
        table, count, tree = struct.unpack_from('<III', data, base + i * 12)
        total += count
        if total > 65535:
            raise ValueError('resource count exceeds budget')
        span(base + table, count * 4, base, end)
        span(base + tree, (count + 1) * 12, base, end)
        records = []
        for node in range(count + 1):
            at = base + tree + node * 12
            _, left, right, name = struct.unpack_from('<IHHI', data, at)
            if left > count or right > count:
                raise ValueError('tree child outside dictionary')
            if not node:
                continue
            name_at = strings['offset'] + name
            string_end = strings['offset'] + strings['size']
            span(name_at, 1, strings['offset'], string_end)
            stop = data.find(b'\0', name_at, min(string_end, name_at + 1025))
            if stop < 0:
                raise ValueError('unterminated or over-budget resource name')
            target = base + u32(base + table + (node - 1) * 4)
            span(target, 4, base, end)
            raw = data[name_at:stop]
            records.append({'index': node - 1, 'name': raw.decode('utf-8', 'replace'),
                            'name_hex': raw.hex(), 'object_offset': target})
        resources[category] = records
    return {'sha256': hashlib.sha256(data).hexdigest(), 'size': len(data),
            'compatibility': [7, 7], 'sections': sections, 'resources': resources,
            'scope': 'resource index only; relocation commands, object bodies and GPU data are not validated'}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--file', type=Path, required=True)
    parser.add_argument('--gr-section', type=int, help='select a section from a raw GR minipack')
    args = parser.parse_args()
    try:
        with args.file.open('rb') as stream:
            data = stream.read(MAX_BYTES + 1)
        if len(data) > MAX_BYTES:
            raise ValueError('input exceeds 64 MiB')
        if args.gr_section is not None:
            data = extract_gr_section(data, args.gr_section)
        print(json.dumps(inspect(data), indent=2))
    except (OSError, ValueError, struct.error) as error:
        print(f'error: {error}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
