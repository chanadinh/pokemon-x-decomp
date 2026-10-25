#!/usr/bin/env python3
"""Independently decode generated PK6 slots and compare original text units."""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path
import struct


def decode_name(slot, code):
    key = struct.unpack_from('<I', slot)[0]
    row = (key >> 13) & 31
    state = key
    shuffled = bytearray()
    for offset in range(8, 0xe8, 2):
        state = (state * 0x41c64e6d + 0x6073) & 0xffffffff
        shuffled.extend(struct.pack('<H', struct.unpack_from('<H', slot, offset)[0] ^ (state >> 16)))
    assert sum(struct.unpack('<112H', shuffled)) & 0xffff == struct.unpack_from('<H', slot, 6)[0]
    order = code[0x470eec + 4 * row:0x470eec + 4 * row + 4]
    block_a = shuffled[order[0] * 0x38:(order[0] + 1) * 0x38]
    block_b = shuffled[order[1] * 0x38:(order[1] + 1) * 0x38]
    return struct.unpack_from('<H', block_a)[0], list(struct.unpack_from('<13H', block_b)), row, block_b


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--romfs', type=Path, required=True)
    parser.add_argument('--code', type=Path, required=True)
    parser.add_argument('--records', type=Path, required=True)
    parser.add_argument('--viewer-save', type=Path, help='Also compare names in a native viewer save')
    args = parser.parse_args()
    reader = Path(__file__).resolve().parents[1] / 'bch-walk/verify_walk.py'
    spec = importlib.util.spec_from_file_location('walk_reader', reader)
    walk = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(walk)
    raw = walk.garc_member(args.romfs / 'a/0/7/4', 80)
    code = args.code.read_bytes()
    if hashlib.sha256(code).hexdigest() != '0f6d3e1d66cbcd55d46fa453db4b6aa489e03c3537c6b24026f2a53af142797a':
        raise SystemExit('Input does not match the pinned executable')
    section = struct.unpack_from('<I', raw, 12)[0]
    lines = struct.unpack_from('<H', raw, 2)[0]
    names = []
    for i in range(lines):
        offset, length = struct.unpack_from('<IH', raw, section + 4 + 8 * i)
        key = (0x7c89 + i * 0x2983) & 0xffff
        units = []
        for j in range(length):
            units.append(struct.unpack_from('<H', raw, section + offset + 2 * j)[0] ^ key)
            key = ((key << 3) | (key >> 13)) & 0xffff
        names.append(units)
    data = args.records.read_bytes()
    assert len(data) == 721 * 0x104
    rows = set()
    for species in range(1, 722):
        slot = data[(species - 1) * 0x104:species * 0x104]
        actual_species, actual_name, row, block_b = decode_name(slot, code)
        rows.add(row)
        assert actual_species == species
        units = names[species][:names[species].index(0)]
        assert len(units) <= 12 and 0x10 not in units
        assert actual_name == units + [0] * (13 - len(units))
        assert block_b[0x1a:0x22] == bytes(8), 'name overwrote move fields'
        assert struct.unpack_from('<I', block_b, 0x34)[0] >> 31 == 0
    viewer_names = 0
    if args.viewer_save:
        saved = args.viewer_save.read_bytes()
        assert len(saved) == 36 + 6 * 0x104 and saved[:4] == b'PXSV'
        version, count = struct.unpack_from('<HH', saved, 4)
        assert version == 1 and 1 <= count <= 6
        for i in range(count):
            species, actual_name, _, _ = decode_name(saved[36 + i * 0x104:36 + (i + 1) * 0x104], code)
            units = names[species][:names[species].index(0)]
            assert actual_name == units + [0] * (13 - len(units))
            viewer_names += 1
    print(json.dumps({'independent_saved_names': 721, 'checksums': 721, 'block_selector_rows': len(rows),
                      'viewer_saved_names': viewer_names,
                      'text_member_sha256': hashlib.sha256(raw).hexdigest(),
                      'records_sha256': hashlib.sha256(data).hexdigest(), 'all_match': True}, sort_keys=True))


if __name__ == '__main__':
    main()
