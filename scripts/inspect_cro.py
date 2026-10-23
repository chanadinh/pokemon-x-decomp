#!/usr/bin/env python3
"""Bounded, read-only CRO0/CRS metadata inspection. Never executes game bytes.

SegmentOffset's low nibble selects a segment TABLE ENTRY, not its type field.
For a CRS, segment offsets are addresses in the main process, not file offsets.
Format reference: the pinned ghidra-ctr-loader CRO0Header.kt; interpretation is
checked against this title's segment table and main executable map.
"""
from __future__ import annotations

import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import struct


HEADER_SIZE = 0x138
MAX_FILE_SIZE = 64 * 1024 * 1024
HEADER_FIELDS = (
    'magic', 'name_offset', 'next_loaded_cro', 'previous_loaded_cro',
    'file_size', 'bss_size', 'unknown_98', 'unknown_9c',
    'nnro_control_object', 'on_load', 'on_exit', 'on_unresolved',
    'code_offset', 'code_size', 'data_offset', 'data_size',
    'module_name_offset', 'module_name_size', 'segment_table_offset', 'segment_table_count',
    'named_export_table_offset', 'named_export_table_count',
    'indexed_export_table_offset', 'indexed_export_table_count',
    'export_strings_offset', 'export_strings_size', 'export_tree_offset', 'export_tree_count',
    'import_module_table_offset', 'import_module_table_count',
    'import_patches_offset', 'import_patches_count',
    'named_import_table_offset', 'named_import_table_count',
    'indexed_import_table_offset', 'indexed_import_table_count',
    'anonymous_import_table_offset', 'anonymous_import_table_count',
    'import_strings_offset', 'import_strings_size',
    'static_anonymous_symbol_table_offset', 'static_anonymous_symbol_table_count',
    'relocation_patches_offset', 'relocation_patches_count',
    'static_anonymous_patch_table_offset', 'static_anonymous_patch_table_count',
)


class FormatError(ValueError):
    """The input violates a format/range invariant needed for analysis."""


def parse_cro(path: Path | str) -> dict:
    path = Path(path)
    size = path.stat().st_size
    if size < HEADER_SIZE or size > MAX_FILE_SIZE:
        raise FormatError(f'{path.name}: unsupported file length {size}')
    return parse_bytes(path.read_bytes(), path.name, str(path.resolve()))


def parse_bytes(data: bytes, filename: str = 'module.cro', source_path: str | None = None) -> dict:
    """Parse bytes for tests or immutable input; .crs suffix chooses static mode."""
    size = len(data)
    if size < HEADER_SIZE or size > MAX_FILE_SIZE:
        raise FormatError(f'{filename}: unsupported file length {size}')
    if data[0x80:0x84] != b'CRO0':
        raise FormatError(f'{filename}: missing CRO0 magic at 0x80')
    kind = 'crs' if filename.lower().endswith('.crs') else 'cro'
    h = dict(zip(HEADER_FIELDS, struct.unpack_from('<46I', data, 0x80)))
    warnings = []
    if h['file_size'] != size:
        raise FormatError(f'{filename}: declared file size {h["file_size"]} differs from {size}')

    def bounded(offset, length, description):
        if offset < 0 or length < 0 or offset > size or length > size - offset:
            raise FormatError(f'{filename}: {description} range {offset:#x}+{length:#x} exceeds file')
        return offset, offset + length

    def table(prefix, stride):
        offset, count = h[prefix + '_offset'], h[prefix + '_count']
        bounded(offset, count * stride, prefix)
        return offset, count

    def cstring(offset, start, length, description):
        bounded(start, length, description)
        if not start <= offset < start + length:
            raise FormatError(f'{filename}: {description} string offset is outside its declared range')
        end = data.find(b'\0', offset, start + length)
        if end < 0:
            raise FormatError(f'{filename}: unterminated {description}')
        try:
            return data[offset:end].decode('ascii')
        except UnicodeDecodeError as exc:
            raise FormatError(f'{filename}: non-ASCII {description}') from exc

    name = cstring(h['module_name_offset'], h['module_name_offset'], h['module_name_size'], 'module name')
    if not name:
        raise FormatError(f'{filename}: empty module name')
    bounded(h['export_strings_offset'], h['export_strings_size'], 'export strings')
    bounded(h['import_strings_offset'], h['import_strings_size'], 'import strings')
    bounded(h['code_offset'], h['code_size'], 'code region')
    bounded(h['data_offset'], h['data_size'], 'data region')
    table('export_tree', 8)
    if h['static_anonymous_symbol_table_count']:
        raise FormatError(f'{filename}: nonempty static anonymous symbol table is not supported')
    bounded(h['static_anonymous_symbol_table_offset'], 0, 'empty static anonymous symbol table')

    offset, count = table('segment_table', 12)
    if not 1 <= count <= 16:
        raise FormatError(f'{filename}: segment table must contain 1..16 entries')
    segments = []
    for i in range(count):
        start, length, typ = struct.unpack_from('<III', data, offset + 12 * i)
        if typ not in (0, 1, 2, 3):
            raise FormatError(f'{filename}: unknown segment type {typ}')
        if kind == 'cro' and typ != 3:
            bounded(start, length, f'segment {i}')
        segments.append({'index': i, 'offset': start, 'size': length, 'type': typ})

    def segment_offset(encoded, description, width=0, check_target=True):
        idx, off = encoded & 15, encoded >> 4
        if idx >= len(segments):
            raise FormatError(f'{filename}: {description} references missing segment index {idx}')
        if check_target and (off > segments[idx]['size'] or width > segments[idx]['size'] - off):
            raise FormatError(f'{filename}: {description} extends outside segment {idx}')
        return {'encoded': encoded, 'segment_index': idx, 'offset': off}

    specials = {}
    for field in ('nnro_control_object', 'on_load', 'on_exit', 'on_unresolved'):
        value = h[field]
        specials[field] = None if value == 0xffffffff else segment_offset(value, field)

    named_exports = []
    offset, count = table('named_export_table', 8)
    for i in range(count):
        string_offset, encoded = struct.unpack_from('<II', data, offset + 8 * i)
        symbol = cstring(string_offset, h['export_strings_offset'], h['export_strings_size'], 'export name')
        entry = segment_offset(encoded, 'named export')
        entry['name'] = symbol
        named_exports.append(entry)
    indexed_exports = []
    offset, count = table('indexed_export_table', 4)
    for i in range(count):
        encoded = struct.unpack_from('<I', data, offset + 4 * i)[0]
        entry = segment_offset(encoded, 'indexed export')
        entry['index'] = i
        indexed_exports.append(entry)

    patches = {}
    for category, prefix in (('internal', 'relocation_patches'), ('imports', 'import_patches'), ('static', 'static_anonymous_patch_table')):
        offset, count = table(prefix, 12)
        parsed = []
        for i in range(count):
            pos = offset + i * 12
            encoded, typ, b5, b6, b7, addend = struct.unpack_from('<IBBBBI', data, pos)
            entry = segment_offset(encoded, f'{category} patch site', width=4)
            signed_addend = addend if addend < 0x80000000 else addend - 0x100000000
            entry.update({'file_offset': pos, 'type': typ, 'byte5': b5, 'byte6': b6, 'byte7': b7, 'addend': addend, 'signed_addend': signed_addend})
            if category == 'internal':
                if b5 >= len(segments):
                    raise FormatError(f'{filename}: relocation target segment index {b5} is absent')
                if not 0 <= signed_addend <= segments[b5]['size']:
                    warnings.append(f'Internal relocation at {pos:#x} uses segment {b5} plus signed addend {signed_addend}; resulting target lies outside that segment and requires interpretation, not automatic rejection')
            elif category == 'imports' and b5 not in (0, 1):
                raise FormatError(f'{filename}: import patch batch terminator must be 0 or 1')
            if typ not in (2, 3):
                warnings.append(f'Unsupported patch type {typ} at {pos:#x}; inspection only')
            if b6 or b7:
                warnings.append(f'Nonzero auxiliary patch flag at {pos:#x}; inspection only')
            parsed.append(entry)
        patches[category] = parsed

    used_patches = set()

    def batch(list_offset):
        base, count = h['import_patches_offset'], h['import_patches_count']
        relative = list_offset - base
        if relative < 0 or relative % 12 or relative // 12 >= count:
            raise FormatError(f'{filename}: import patch list starts outside patch table')
        index = relative // 12
        end = index
        while end < count:
            used_patches.add(end)
            terminal = patches['imports'][end]['byte5']
            end += 1
            if terminal:
                return {'patch_index': index, 'patch_count': end - index}
        raise FormatError(f'{filename}: unterminated import patch batch')

    imports = {'named': [], 'modules': []}
    offset, count = table('named_import_table', 8)
    for i in range(count):
        string_offset, list_offset = struct.unpack_from('<II', data, offset + 8 * i)
        symbol = cstring(string_offset, h['import_strings_offset'], h['import_strings_size'], 'import name')
        imports['named'].append({'name': symbol, **batch(list_offset)})

    # Module ranges select subarrays of the global indexed/anonymous import tables.
    table('indexed_import_table', 8)
    table('anonymous_import_table', 8)
    owned_indices = {'indexed': set(), 'anonymous': set()}

    def module_imports(start, count, category):
        prefix = category + '_import_table'
        base, total = h[prefix + '_offset'], h[prefix + '_count']
        if count == 0:
            return []
        relative = start - base
        if relative < 0 or relative % 8 or count > total - relative // 8:
            raise FormatError(f'{filename}: {category} import subarray is outside its global table')
        entries = []
        for i in range(count):
            index = relative // 8 + i
            if index in owned_indices[category]:
                raise FormatError(f'{filename}: overlapping module {category} import ranges')
            owned_indices[category].add(index)
            encoded, list_offset = struct.unpack_from('<II', data, start + i * 8)
            if category == 'indexed':
                entry = {'index': encoded}
            else:
                # This addresses the OTHER module; its bounds require linking.
                entry = {'encoded': encoded, 'segment_index': encoded & 15, 'offset': encoded >> 4}
            entry.update(batch(list_offset))
            entries.append(entry)
        return entries

    offset, count = table('import_module_table', 20)
    for i in range(count):
        name_offset, indexed_head, indexed_count, anonymous_head, anonymous_count = struct.unpack_from('<5I', data, offset + 20 * i)
        module_name = cstring(name_offset, h['import_strings_offset'], h['import_strings_size'], 'import module name')
        imports['modules'].append({
            'name': module_name,
            'indexed': module_imports(indexed_head, indexed_count, 'indexed'),
            'anonymous': module_imports(anonymous_head, anonymous_count, 'anonymous'),
        })
    for category in ('indexed', 'anonymous'):
        expected = h[category + '_import_table_count']
        if len(owned_indices[category]) != expected:
            raise FormatError(f'{filename}: {category} imports are not fully covered by module records')
    if len(used_patches) != len(patches['imports']):
        warnings.append(f'{len(patches["imports"]) - len(used_patches)} import patches are not referenced by parsed imports')

    return {
        'schema_version': 1, 'kind': kind, 'path': source_path or filename,
        'name': name, 'sha256': hashlib.sha256(data).hexdigest(), 'size': size,
        'header': h, 'segments': segments, 'specials': specials,
        'named_exports': named_exports, 'indexed_exports': indexed_exports,
        'imports': imports, 'patches': patches, 'warnings': warnings,
        'interpretation': 'CRS segment offsets are main-process virtual addresses; CRO segment offsets are file-relative except BSS. Encoded segment offsets use segment table indices.',
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input', type=Path, help='CRO/CRS file or extracted modules directory')
    parser.add_argument('--output', required=True, type=Path)
    args = parser.parse_args()
    paths = sorted(p for p in args.input.rglob('*') if p.is_file() and p.suffix.lower() in ('.cro', '.crs')) if args.input.is_dir() else [args.input]
    args.output.mkdir(parents=True, exist_ok=True)
    records = []
    total_types = {'internal': Counter(), 'imports': Counter(), 'static': Counter()}
    for path in paths:
        record = parse_cro(path)
        out_name = path.name + '.json'
        (args.output / out_name).write_text(json.dumps(record, indent=2) + '\n')
        patch_types = {key: dict(Counter(str(p['type']) for p in values)) for key, values in record['patches'].items()}
        for key, counts in patch_types.items():
            total_types[key].update(counts)
        records.append({
            'file': path.name, 'name': record['name'], 'kind': record['kind'], 'size': record['size'],
            'sha256': record['sha256'], 'details': out_name,
            'segments': record['segments'],
            'named_exports': len(record['named_exports']), 'indexed_exports': len(record['indexed_exports']),
            'named_imports': len(record['imports']['named']),
            'indexed_imports': sum(len(m['indexed']) for m in record['imports']['modules']),
            'anonymous_imports': sum(len(m['anonymous']) for m in record['imports']['modules']),
            'dependencies': [m['name'] for m in record['imports']['modules']],
            'patch_types': patch_types, 'warnings': record['warnings'],
        })
    inventory = {'schema_version': 1, 'modules': records, 'counts': dict(Counter(r['kind'] for r in records)), 'patch_types': {k: dict(v) for k, v in total_types.items()}, 'method': 'Bounded static metadata parsing; no relocations applied and no game code executed.'}
    (args.output / 'inventory.json').write_text(json.dumps(inventory, indent=2) + '\n')
    print(json.dumps({'records': len(records), 'counts': inventory['counts'], 'patch_types': inventory['patch_types'], 'warning_count': sum(len(r['warnings']) for r in records)}, indent=2))


if __name__ == '__main__':
    main()
