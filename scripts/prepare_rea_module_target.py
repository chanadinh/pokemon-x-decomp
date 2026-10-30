#!/usr/bin/env python3
"""Wrap the main image plus selected relocated CRO modules for REA's Ghidra provider.

Input is a manifest written by scripts/prepare_cro_analysis.py. Main-image
blocks and the selected modules' blocks keep their synthetic static-analysis
addresses and permissions; module blocks carry that preparation's relocation
values. Every block is checked against the manifest's SHA-256. The ELF is an
analysis carrier, not an original file, runtime load layout or runnable port.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct


def prepare(manifest_path, modules, output):
    manifest_path = Path(manifest_path).resolve()
    manifest = json.loads(manifest_path.read_text())
    wanted = {'|static|'} | set(modules)
    blocks = [b for b in manifest['blocks'] if b['module'] in wanted and b['size'] > 0]
    missing = set(modules) - {b['module'] for b in blocks}
    if missing:
        raise ValueError('modules not in manifest: ' + ', '.join(sorted(missing)))
    blocks.sort(key=lambda b: b['address'])
    image = bytearray(52 + 32 * len(blocks))
    strings = bytearray(b'\0')
    names = {}
    for name in [b['name'] for b in blocks] + ['.shstrtab']:
        names[name] = len(strings)
        strings.extend(name.encode('ascii') + b'\0')
    program_headers, section_headers, mapping = [], [bytes(40)], []
    for block in blocks:
        image.extend(bytes(-len(image) % 4096))
        offset = len(image)
        payload = b''
        if block['initialized']:
            path = manifest_path.parent / 'blocks' / (block['name'] + '.bin')
            payload = path.read_bytes()
            if len(payload) != block['size'] or hashlib.sha256(payload).hexdigest() != block['sha256']:
                raise ValueError('block %s differs from the manifest' % block['name'])
        image.extend(payload)
        flags = 4 | (2 if block['write'] else 0) | (1 if block['execute'] else 0)
        program_headers.append(struct.pack('<IIIIIIII', 1, offset, block['address'], block['address'],
                                           len(payload), block['size'], flags, 4096 if payload else 1))
        flags = 2 | (1 if block['write'] else 0) | (4 if block['execute'] else 0)
        section_headers.append(struct.pack('<IIIIIIIIII', names[block['name']], 1 if payload else 8, flags,
                                           block['address'], offset, block['size'], 0, 0, 4, 0))
        mapping.append({'section': block['name'], 'module': block['module'], 'address': hex(block['address']),
                        'size': block['size'], 'elf_offset': offset, 'sha256': block['sha256'],
                        'read': True, 'write': block['write'], 'execute': block['execute']})
    string_offset = len(image)
    image.extend(strings)
    section_headers.append(struct.pack('<IIIIIIIIII', names['.shstrtab'], 3, 0, 0, string_offset, len(strings), 0, 0, 1, 0))
    image.extend(bytes(-len(image) % 4))
    section_offset = len(image)
    image.extend(b''.join(section_headers))
    ident = b'\x7fELF' + bytes([1, 1, 1, 0, 0]) + bytes(7)
    image[:52] = struct.pack('<16sHHIIIIIHHHHHH', ident, 2, 40, 1, manifest.get('entry_point', 0x100000), 52,
                             section_offset, 0x05000000, 52, 32, len(program_headers), 40,
                             len(section_headers), len(section_headers) - 1)
    for index, header in enumerate(program_headers):
        image[52 + 32 * index:52 + 32 * (index + 1)] = header
    output = Path(output).resolve()
    output.mkdir(parents=True, exist_ok=False)
    elf = output / ('pokemon-x-' + '-'.join(m.lower() for m in modules) + '.elf')
    elf.write_bytes(image)
    record = {'purpose': 'Analysis-only ELF32 carrier: original main image plus relocated CRO module blocks '
                         'at synthetic static-analysis addresses. Not runtime load state.',
              'manifest': str(manifest_path), 'modules': modules,
              'elf': {'path': str(elf), 'sha256': hashlib.sha256(image).hexdigest(), 'size': len(image)},
              'language': 'ARM:LE:32:v6', 'mapping': mapping}
    (output / 'target.json').write_text(json.dumps(record, indent=2) + '\n')
    (output / 'seeds.tsv').write_text('')
    return record


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--manifest', type=Path, required=True, help='prepare_cro_analysis.py manifest.json')
    parser.add_argument('--module', action='append', required=True, help='module name, e.g. DllBattle (repeatable)')
    parser.add_argument('--output', type=Path, required=True, help='new output directory')
    args = parser.parse_args()
    try:
        record = prepare(args.manifest, args.module, args.output)
    except (OSError, ValueError) as exc:
        parser.exit(2, f'error: {exc}\n')
    print(f"Prepared {record['elf']['path']}; SHA-256 {record['elf']['sha256']}")


if __name__ == '__main__':
    main()
