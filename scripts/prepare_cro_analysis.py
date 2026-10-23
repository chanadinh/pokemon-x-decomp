#!/usr/bin/env python3
"""Prepare copied CRO/main segments for static analysis at synthetic addresses.

This is not a 3DS runtime loader. It maps every supplied module concurrently for
cross-reference analysis, without executing code or reproducing load order.
Only NONE/ABS32/REL32 relocation operations are implemented, and unsupported or
unresolvable operations remain explicit in the audit. Input files are immutable.
"""
from __future__ import annotations

import argparse
from collections import Counter, defaultdict
import hashlib
import json
from pathlib import Path
import re
import struct

from inspect_cro import parse_cro


SOURCE = {
    'url': 'https://github.com/azahar-emu/azahar/blob/master/src/core/hle/service/ldr_ro/cro_helper.cpp',
    'blob_sha': '865adf11c0a5eb507f202e1bb6850ddb8f455e64',
    'functions': ['ApplyRelocation', 'ApplyRelocationBatch', 'ApplyInternalRelocations'],
    'semantics': 'ABS32 = S + A; REL32 = S + A - P, modulo 2**32. Internal S is a segment base; import S is the resolved symbol address.',
}
SEGMENT_NAMES = {0: 'text', 1: 'rodata', 2: 'data', 3: 'bss'}
CONTROL_DISPATCH_PREFIX = bytes.fromhex('80209fe5050051e310402de901f19f37150000ea')


class PreparationError(ValueError):
    pass


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def relocated_word(kind: int, symbol: int, addend: int, place: int, original: int) -> int:
    """The addend is explicit in the relocation record, not the original word."""
    if kind == 0:
        return original
    if kind == 2:
        return (symbol + addend) & 0xffffffff
    if kind == 3:
        return (symbol + addend - place) & 0xffffffff
    raise PreparationError(f'unsupported relocation type {kind}')


def safe_name(name: str) -> str:
    return re.sub(r'[^A-Za-z0-9_]+', '_', name).strip('_') or 'module'


def prepare(module_dir: Path, code_path: Path, main_map_path: Path, output: Path) -> dict:
    if output.exists():
        raise PreparationError(f'output must be a new directory: {output}')
    records = [parse_cro(p) for p in sorted(module_dir.glob('*.cro'))]
    crs = parse_cro(module_dir / 'static.crs')
    records.insert(0, crs)
    names = [r['name'] for r in records]
    if len(set(names)) != len(names):
        raise PreparationError('duplicate internal module names')
    by_name = {r['name']: r for r in records}
    original_sources = {Path(r['path']): r['sha256'] for r in records}
    main_map = json.loads(main_map_path.read_text())
    code = code_path.read_bytes()
    if digest(code) != main_map['code']['sha256']:
        raise PreparationError('main executable hash differs from import manifest')
    original_sources[code_path] = digest(code)

    blocks = []
    payloads = {}
    segment_maps = {}
    modules = []
    symbols = []
    symbols_seen = set()
    mapping_notes = []

    def add_block(name, module, address, size, data, execute, write):
        if size <= 0:
            return
        if address < 0 or address + size > 0x100000000:
            raise PreparationError('block exceeds 32-bit address space')
        for block in blocks:
            if address < block['address'] + block['size'] and block['address'] < address + size:
                raise PreparationError(f'overlapping blocks: {name}, {block["name"]}')
        if data is not None and len(data) != size:
            raise PreparationError('block payload size mismatch')
        blocks.append({'name': name, 'module': module, 'address': address, 'size': size,
                       'read': True, 'write': write, 'execute': execute,
                       'path': None, 'sha256': None, 'initialized': data is not None})
        payloads[name] = bytearray(data) if data is not None else None

    for segment in main_map['segments']:
        offset = segment['offset']
        data = None if offset is None else code[offset:offset + segment['size']]
        add_block('main_' + segment['name'].lstrip('.'), crs['name'], segment['address'],
                  segment['size'], data, segment['execute'], segment['write'])
    segment_maps[crs['name']] = {s['index']: s['offset'] for s in crs['segments']}
    # CRS entries describe main-process addresses, not offsets into static.crs.
    # Preserve unknown runtime ranges as uninitialized memory, never zero bytes.
    for s in crs['segments']:
        if not s['size']:
            continue
        begin, end = s['offset'], s['offset'] + s['size']
        overlaps = sorted((b for b in blocks if begin < b['address'] + b['size'] and b['address'] < end),
                          key=lambda b: b['address'])
        cursor = begin
        for b in overlaps:
            if b['address'] > cursor:
                raise PreparationError('CRS segment partially covers an unexplained gap in main map')
            cursor = max(cursor, b['address'] + b['size'])
        if not overlaps:
            add_block(f'main_runtime_segment_{s["index"]}', crs['name'], begin, s['size'], None, False, False)
            mapping_notes.append({'module': crs['name'], 'segment_index': s['index'],
                                  'address': begin, 'size': s['size'],
                                  'note': 'CRS-described runtime region, content unavailable; mapped uninitialized.'})
        elif cursor < end:
            raise PreparationError('CRS segment exceeds supplied main memory map')
    main_text = next(s for s in main_map['segments'] if s['name'] == '.text')
    modules.append({'name': crs['name'], 'kind': 'main', 'source': crs['path'],
                    'code_start': main_text['address'], 'code_size': main_text['size'],
                    'synthetic_addresses': False, 'segments': crs['segments']})

    for slot, rec in enumerate(records[1:]):
        base = 0x10000000 + slot * 0x00400000
        raw = Path(rec['path']).read_bytes()
        segmap = {}
        for s in rec['segments']:
            address = base + (0x00300000 if s['type'] == 3 else s['offset'])
            segmap[s['index']] = address
            if not s['size']:
                continue
            if s['type'] not in SEGMENT_NAMES:
                raise PreparationError(f'unknown nonempty segment type {s["type"]}')
            size = s['size']
            if address + size > base + 0x00400000:
                raise PreparationError(f'module exceeds assigned synthetic slot: {rec["name"]}')
            data = None if s['type'] == 3 else raw[s['offset']:s['offset'] + size]
            add_block(f'{safe_name(rec["name"])}_{SEGMENT_NAMES[s["type"]]}_{s["index"]}',
                      rec['name'], address, size, data, s['type'] == 0, s['type'] in (2, 3))
        segment_maps[rec['name']] = segmap
        text = next((s for s in rec['segments'] if s['type'] == 0 and s['size']), None)
        modules.append({'name': rec['name'], 'kind': 'cro', 'source': rec['path'],
                        'base': base, 'synthetic_addresses': True,
                        'code_start': segmap[text['index']] if text else 0,
                        'code_size': text['size'] if text else 0,
                        'segments': [dict(s, analysis_address=segmap[s['index']]) for s in rec['segments']]})

    def resolve_position(rec, segment_index, offset, require_inside=True):
        if not 0 <= segment_index < len(rec['segments']):
            raise PreparationError('invalid referenced segment index')
        segment = rec['segments'][segment_index]
        if require_inside and not 0 <= offset < segment['size']:
            raise PreparationError(f'symbol outside declared segment: {rec["name"]}:{segment_index}+{offset:#x}')
        return segment_maps[rec['name']][segment_index] + offset

    def add_symbol(rec, name, segment_index, offset, function=False, reason=''):
        address = resolve_position(rec, segment_index, offset)
        if function:
            mode = 'Thumb' if address & 1 else 'ARM'
            address &= ~1
        else:
            mode = None
        key = (rec['name'], name, address)
        if key not in symbols_seen:
            symbols_seen.add(key)
            symbols.append({'name': name, 'module': rec['name'], 'address': address,
                            'kind': 'function' if function else 'label', 'mode': mode,
                            'source': reason})

    named = defaultdict(list)
    for rec in records:
        for exp in rec['named_exports']:
            named[exp['name']].append((rec, exp))
            s = rec['segments'][exp['segment_index']]
            function = (s['type'] == 0 and exp['name'] != 'nnroControlObject_' and
                        not exp['name'].startswith(('_ZTV', '_ZTI', '_ZTS')))
            reason = 'named export'
            if exp['name'] == 'nnroControlObject_' and rec['kind'] == 'cro':
                offset = s['offset'] + exp['offset']
                prefix = Path(rec['path']).read_bytes()[offset:offset + len(CONTROL_DISPATCH_PREFIX)]
                function = s['type'] == 0 and prefix == CONTROL_DISPATCH_PREFIX
                reason = 'named export; ARM dispatch instruction prefix verified' if function else 'named export; control entry instructions not recognized'
            add_symbol(rec, exp['name'], exp['segment_index'], exp['offset'], function, reason)
        for exp in rec['indexed_exports']:
            # Segment membership alone does not establish a function entry.
            add_symbol(rec, f'export_index_{exp["index"]}', exp['segment_index'], exp['offset'], False, 'indexed export')
        for field in ('on_load', 'on_exit', 'on_unresolved'):
            encoded = rec['header'].get(field, 0xffffffff)
            if encoded != 0xffffffff:
                add_symbol(rec, 'cro_' + field, encoded & 15, encoded >> 4, True, 'header callback')
    symbols.append({'name': 'ctr_entry', 'module': crs['name'], 'address': main_map['entry_point'],
                    'kind': 'function', 'mode': 'ARM', 'source': 'ExHeader entry point'})

    audit = []
    unresolved = []
    relocation_notes = []
    patched_sites = {}
    by_module = {r['name']: Counter() for r in records}

    def patch(rec, p, symbol, origin, index, target_module, symbol_description):
        base_row = {'module': rec['name'], 'origin': origin, 'patch_index': index,
                    'patch_file_offset': p['file_offset'], 'type': p['type'],
                    'addend': p['addend'], 'symbol_address': symbol,
                    'target_module': target_module, 'symbol': symbol_description}
        try:
            if p['byte6'] or p['byte7']:
                raise PreparationError('nonzero auxiliary relocation flags are not supported')
            place = resolve_position(rec, p['segment_index'], p['offset'])
            segment = rec['segments'][p['segment_index']]
            if p['offset'] + 4 > segment['size']:
                raise PreparationError('patch straddles declared segment end')
            matches = [b for b in blocks if b['address'] <= place and place + 4 <= b['address'] + b['size']]
            if len(matches) != 1:
                raise PreparationError('patch destination is not uniquely mapped')
            block = matches[0]
            data = payloads[block['name']]
            if data is None:
                raise PreparationError('patch destination has no supplied bytes')
            off = place - block['address']
            original = struct.unpack_from('<I', data, off)[0]
            value = relocated_word(p['type'], symbol, p['addend'], place, original)
            target_address = (symbol + p['addend']) & 0xffffffff
            if p['type'] in (2, 3) and not any(b['address'] <= (target_address & ~1) < b['address'] + b['size'] for b in blocks):
                relocation_notes.append(dict(base_row, address=place, target_address=target_address,
                    signed_addend=p['addend'] if p['addend'] < 0x80000000 else p['addend'] - 0x100000000,
                    note='Relocation arithmetic is applied, but its target lies outside supplied segments (e.g. an alignment gap or a one-past pointer).'))
            if place in patched_sites:
                raise PreparationError('duplicate relocation destination; not silently overwriting')
            patched_sites[place] = base_row
            struct.pack_into('<I', data, off, value)
            audit.append(dict(base_row, address=place, original_word=original, value=value, status='applied'))
            by_module[rec['name']][origin + '_applied'] += 1
        except PreparationError as exc:
            unresolved.append(dict(base_row, status='unresolved', reason=str(exc)))
            by_module[rec['name']][origin + '_unresolved'] += 1

    for rec in records:
        for i, p in enumerate(rec['patches']['internal']):
            index = p['byte5']
            if index not in segment_maps[rec['name']]:
                raise PreparationError('internal relocation references an invalid segment')
            patch(rec, p, segment_maps[rec['name']][index], 'internal', i,
                  rec['name'], f'segment_{index}')
        if rec['patches']['static']:
            raise PreparationError('static anonymous relocations are unsupported')

    edges = []
    for rec in records:
        patches = rec['patches']['imports']
        used = set()

        def import_symbol(entry, target, position, description):
            indices = range(entry['patch_index'], entry['patch_index'] + entry['patch_count'])
            try:
                if target is None:
                    raise PreparationError(description)
                address = resolve_position(target, position['segment_index'], position['offset'])
                add_symbol(target, description, position['segment_index'], position['offset'], False,
                           f'anonymous/indexed import from {rec["name"]}; not a function seed')
            except PreparationError as exc:
                for i in indices:
                    used.add(i)
                    unresolved.append({'module': rec['name'], 'origin': 'import', 'patch_index': i,
                                       'status': 'unresolved', 'reason': str(exc)})
                    by_module[rec['name']]['import_unresolved'] += 1
                return
            for i in indices:
                if i in used:
                    raise PreparationError('an import patch belongs to multiple symbol batches')
                used.add(i)
                patch(rec, patches[i], address, 'import', i, target['name'], description)

        for entry in rec['imports']['named']:
            options = named.get(entry['name'], [])
            if len(options) == 1:
                target, position = options[0]
                import_symbol(entry, target, position, entry['name'])
            else:
                import_symbol(entry, None, None, f'named import {entry["name"]}: {len(options)} providers')
        for provider in rec['imports']['modules']:
            target = by_name.get(provider['name'])
            edges.append({'from': rec['name'], 'to': provider['name'],
                          'anonymous_symbols': len(provider['anonymous']),
                          'indexed_symbols': len(provider['indexed']), 'provider_present': target is not None})
            for entry in provider['anonymous']:
                label = f'anon_segment_{entry["segment_index"]}_{entry["offset"]:x}'
                import_symbol(entry, target, entry, label if target else f'missing provider {provider["name"]}')
            for entry in provider['indexed']:
                if target is not None and entry['index'] < len(target['indexed_exports']):
                    import_symbol(entry, target, target['indexed_exports'][entry['index']], f'export_index_{entry["index"]}')
                else:
                    import_symbol(entry, None, None, f'missing indexed export {provider["name"]}:{entry["index"]}')
        if used != set(range(len(patches))):
            raise PreparationError(f'import patches without unique symbol ownership in {rec["name"]}')

    for path, expected_hash in original_sources.items():
        if digest(path.read_bytes()) != expected_hash:
            raise PreparationError(f'input changed during preparation: {path}')
    output.mkdir(parents=True)
    payload_dir = output / 'blocks'
    payload_dir.mkdir()
    for block in blocks:
        data = payloads[block['name']]
        if data is not None:
            path = payload_dir / (block['name'] + '.bin')
            path.write_bytes(data)
            block['path'] = str(path.resolve())
            block['sha256'] = digest(data)
    result = {
        'schema_version': 1, 'purpose': 'static analysis only; synthetic simultaneous CRO map, not runtime load state',
        'language': 'ARM:LE:32:v6', 'entry_point': main_map['entry_point'],
        'blocks': blocks, 'symbols': symbols, 'modules': modules,
        'source_hashes': [{'path': str(p.resolve()), 'sha256': h} for p, h in original_sources.items()],
        'relocation_summary': {'applied': len(audit), 'unresolved': len(unresolved),
                               'by_module': {k: dict(v) for k, v in by_module.items()},
                               'operations': dict(Counter(str(p['type']) for p in audit)),
                               'nonzero_addend_imports_applied': sum(p['origin'] == 'import' and p['addend'] != 0 for p in audit)},
        'mapping_notes': mapping_notes, 'relocation_semantics_reference': SOURCE,
        'relocation_notes': relocation_notes,
        'audit_file': 'relocations.json', 'dependency_file': 'dependencies.json',
        'limitations': ['All supplied CROs are mapped concurrently to synthetic addresses; runtime load order is not modeled.',
                        'CRR signatures and console trust are not evaluated.',
                        'Original inputs remain unchanged; output blocks contain static-analysis relocation values.',
                        'Anonymous code-segment imports are labels, not assumed function entries.',
                        'nnroControlObject_ function seeds require the observed ARM dispatcher prefix.'],
    }
    (output / 'manifest.json').write_text(json.dumps(result, indent=2) + '\n')
    (output / 'relocations.json').write_text(json.dumps({'applied': audit, 'unresolved': unresolved}, indent=2) + '\n')
    (output / 'dependencies.json').write_text(json.dumps(edges, indent=2) + '\n')
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--modules', type=Path, required=True)
    parser.add_argument('--code', type=Path, required=True)
    parser.add_argument('--main-map', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    result = prepare(args.modules, args.code, args.main_map, args.output)
    print(json.dumps({'manifest': str(args.output / 'manifest.json'),
                      'modules': len(result['modules']), 'blocks': len(result['blocks']),
                      'symbols': len(result['symbols']), 'relocations': result['relocation_summary']}, indent=2))


if __name__ == '__main__':
    main()
