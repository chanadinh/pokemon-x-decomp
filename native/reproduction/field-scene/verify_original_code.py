#!/usr/bin/env python3
"""Verify selected exported instruction bytes against unchanged original inputs."""
import argparse
import hashlib
import json
from pathlib import Path
import re


def digest(data):
    return hashlib.sha256(data).hexdigest()


def verify(workspace, exports):
    evidence = json.loads(Path(__file__).with_name('original-code-evidence.json').read_text())
    originals = {}
    for key, row in evidence['inputs'].items():
        data = (workspace / row['path']).read_bytes()
        if len(data) != row['bytes'] or digest(data) != row['sha256']:
            raise ValueError('Original input does not match: ' + key)
        originals[key] = data
    total = 0
    for row in evidence['functions']:
        name = row['address'][2:]
        data = bytearray()
        count = 0
        for line in (exports / (name + '.asm')).read_text().splitlines():
            match = re.fullmatch(r'([0-9a-f]+)  ([0-9a-f]+)  (.*)', line)
            if not match:
                raise ValueError('Malformed assembly line: ' + name)
            address = int(match[1], 16)
            chunk = bytes.fromhex(match[2])
            key = row['source']
            offset = address - (0x15c00000 if key == 'field' else 0x100000)
            if offset < 0 or originals[key][offset:offset + len(chunk)] != chunk:
                raise ValueError('Instruction mismatch at ' + hex(address))
            data.extend(chunk)
            count += 1
        if (count != row['instruction_count'] or len(data) != row['instruction_bytes']
                or digest(data) != row['sha256_of_instruction_bytes_in_address_order']):
            raise ValueError('Export boundary or digest changed: ' + name)
        total += len(data)
    excerpt = evidence['startup_excerpt']
    offset = int(excerpt['address'], 16) - 0x100000
    chunk = originals['main'][offset:offset + excerpt['instruction_bytes']]
    if digest(chunk) != excerpt['sha256']:
        raise ValueError('Startup excerpt changed')
    print(f'Verified {len(evidence["functions"])} selected functions, '
          f'{total} instruction bytes, and the {len(chunk)}-byte startup excerpt.')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--workspace', type=Path, default=Path(__file__).resolve().parents[3])
    parser.add_argument('--exports', type=Path, required=True)
    args = parser.parse_args()
    verify(args.workspace.resolve(), args.exports.resolve())


if __name__ == '__main__':
    main()
