#!/usr/bin/env python3
"""Wrap the validated Pokemon X main image for REA's Ghidra provider.

The generated ELF is an analysis carrier, not an original ELF or a runnable
port. Keep the ExHeader's addresses, permissions, and uninitialized BSS;
copy the original section bytes without applying the CRO analysis relocations.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from inspect_code import inspect_pair

ROOT = Path(__file__).resolve().parents[1]
SEEDS = {
    0x0013A19C: 'message_resource_get',
    0x0013FF50: 'message_decode',
    0x00168BBC: 'move_resource_get',
    0x00174EB4: 'move_cache_store',
    0x00174F30: 'move_resource_load',
    0x001762B4: 'move_cache_lookup',
    0x00176328: 'move_resource_field',
}


def prepare(code_path, exheader_path, output):
    manifest = inspect_pair(code_path, exheader_path, decompressed=True)
    pinned = json.loads((ROOT / 'tools/versions.json').read_text())['game_input']
    for field in ('code', 'exheader'):
        if manifest[field]['sha256'] != pinned[field + '_sha256']:
            raise ValueError(f'{field} differs from the pinned Pokemon X input')
    code = Path(code_path).read_bytes()
    if hashlib.sha256(code).hexdigest() != manifest['code']['sha256']:
        raise ValueError('Code changed after validation')
    segments = manifest['segments']
    image = bytearray(52 + 32 * len(segments))
    strings = bytearray(b'\0')
    names = {}
    for name in [s['name'] for s in segments] + ['.shstrtab']:
        names[name] = len(strings)
        strings.extend(name.encode('ascii') + b'\0')
    program_headers = []
    section_headers = [bytes(40)]
    mapping = []
    for segment in segments:
        image.extend(bytes(-len(image) % 4096))
        offset = len(image)
        address, size = segment['address'], segment['size']
        initialized = segment['offset'] is not None
        payload = code[segment['offset']:segment['offset'] + size] if initialized else b''
        if initialized and len(payload) != size:
            raise ValueError('Truncated section after validation')
        image.extend(payload)
        flags = 4 | (2 if segment['write'] else 0) | (1 if segment['execute'] else 0)
        program_headers.append(struct.pack('<IIIIIIII', 1, offset, address, address,
                                           len(payload), size, flags, 4096 if initialized else 1))
        flags = 2 | (1 if segment['write'] else 0) | (4 if segment['execute'] else 0)
        section_headers.append(struct.pack('<IIIIIIIIII', names[segment['name']],
                                           1 if initialized else 8, flags, address,
                                           offset, size, 0, 0, 4, 0))
        mapping.append({
            'section': segment['name'], 'address': hex(address), 'size': size,
            'elf_offset': offset, 'code_offset': segment['offset'],
            'sha256': hashlib.sha256(payload).hexdigest() if initialized else None,
            'read': True, 'write': segment['write'], 'execute': segment['execute'],
        })
    string_offset = len(image)
    image.extend(strings)
    section_headers.append(struct.pack('<IIIIIIIIII', names['.shstrtab'], 3, 0, 0,
                                       string_offset, len(strings), 0, 0, 1, 0))
    image.extend(bytes(-len(image) % 4))
    section_offset = len(image)
    image.extend(b''.join(section_headers))
    # ELF32 little endian, ET_EXEC, EM_ARM, synthetic EABI5 container metadata.
    ident = b'\x7fELF' + bytes([1, 1, 1, 0, 0]) + bytes(7)
    image[:52] = struct.pack('<16sHHIIIIIHHHHHH', ident, 2, 40, 1,
                             manifest['entry_point'], 52, section_offset,
                             0x05000000, 52, 32, len(program_headers),
                             40, len(section_headers), len(section_headers) - 1)
    for index, header in enumerate(program_headers):
        image[52 + 32 * index:52 + 32 * (index + 1)] = header
    for item in mapping:
        if item['code_offset'] is not None:
            actual = image[item['elf_offset']:item['elf_offset'] + item['size']]
            expected = code[item['code_offset']:item['code_offset'] + item['size']]
            if actual != expected:
                raise ValueError('Generated section differs from original code')
    output = Path(output).resolve()
    output.mkdir(parents=True, exist_ok=False)
    elf = output / 'pokemon-x-main.elf'
    elf.write_bytes(image)
    record = {
        'purpose': 'Analysis-only ELF32 carrier of original decompressed 3DS main image. '
                   'The ELF metadata is generated, not original game metadata. '
                   'CROs and runtime initialization are absent.',
        'code': manifest['code'], 'exheader': manifest['exheader'],
        'elf': {'path': str(elf), 'sha256': hashlib.sha256(image).hexdigest(), 'size': len(image)},
        'language': 'ARM:LE:32:v6', 'entry_point': hex(manifest['entry_point']),
        'mapping': mapping, 'all_file_backed_bytes_verified': True,
    }
    (output / 'target.json').write_text(json.dumps(record, indent=2) + '\n')
    (output / 'seeds.tsv').write_text(''.join(
        f'0x{address:x}\tfunction\t{name}\n' for address, name in sorted(SEEDS.items())))
    return record


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--code', type=Path, required=True)
    parser.add_argument('--exheader', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True, help='New local output directory')
    args = parser.parse_args()
    try:
        record = prepare(args.code, args.exheader, args.output)
    except (OSError, ValueError) as exc:
        parser.exit(2, f'error: {exc}\n')
    print(f"Prepared {record['elf']['path']}; SHA-256 {record['elf']['sha256']}")


if __name__ == '__main__':
    main()
