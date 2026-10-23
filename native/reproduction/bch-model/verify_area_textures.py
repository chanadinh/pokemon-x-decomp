#!/usr/bin/env python3
"""Verify selected area75/field165 baseline observations, not a general parser.

Reads caller-provided RomFS and compares independently decoded metadata with
area75-texture-association.json. Explicit output paths are created exclusively.
Optional extracted BCH bytes belong in ignored local output storage only.
"""
import argparse
import ctypes as C
import hashlib
import json
from pathlib import Path
import struct
import sys

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / 'native/tools'))
from survey_assets import archive_index
from inspect_bch import inspect
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--romfs', type=Path, required=True)
parser.add_argument('--lz-library', type=Path, required=True,
                    help='shared library compiled from native/assets/lz.c')
parser.add_argument('--output', type=Path, required=True,
                    help='new metadata report path; existing files are never overwritten')
parser.add_argument('--extract-dir', type=Path,
                    help='new local directory for the two selected BCH payloads')
args = parser.parse_args()
if not __debug__:
    parser.error('assertions are required; do not run this selected-baseline script with -O')
if args.output.exists() or args.output.is_symlink():
    parser.error('--output already exists; choose a fresh path')
if args.extract_dir is not None and (args.extract_dir.exists() or args.extract_dir.is_symlink()):
    parser.error('--extract-dir already exists; choose a fresh directory')
ROMFS = args.romfs
LIB = C.CDLL(str(args.lz_library.resolve()))
LIB.px_lz_decode.argtypes = [C.c_void_p, C.c_size_t, C.c_size_t,
                           C.POINTER(C.c_void_p), C.POINTER(C.c_size_t), C.c_void_p, C.c_size_t]
LIBC = C.CDLL(None)
LIBC.free.argtypes = [C.c_void_p]


def read(path, index):
    source = ROMFS / path
    with source.open('rb') as stream:
        members = archive_index(stream, source.stat().st_size)[2]
        member = next(m for m in members if m['entry'] == index and m['subentry'] == 0)
        assert member['size'] <= 64 * 1024 * 1024
        stream.seek(member['offset'])
        data = stream.read(member['size'])
    if data[0] in (16, 17):
        dst, size, error = C.c_void_p(), C.c_size_t(), C.create_string_buffer(512)
        assert LIB.px_lz_decode(C.create_string_buffer(data), len(data), 64 * 1024 * 1024,
                               C.byref(dst), C.byref(size), error, 512), error.value
        data = C.string_at(dst, size.value)
        LIBC.free(dst)
    return data


def sections(data, magic):
    assert data[:2] == magic
    count = struct.unpack_from('<H', data, 2)[0]
    assert 0 < count <= 256
    offsets = struct.unpack_from('<' + 'I' * (count + 1), data, 4)
    assert offsets[0] >= 4 + 4 * (count + 1) and offsets[-1] <= len(data)
    assert all(a <= b for a, b in zip(offsets, offsets[1:]))
    return [data[a:b] for a, b in zip(offsets, offsets[1:])]

def word(b, off):
    return struct.unpack_from('<I', b, off)[0]

def ptrs(b, index):
    """Record relocation source locations; only use this for the fixed v7 files."""
    base = index['sections']['contents']['offset']
    cmd = index['sections']['commands']['offset']
    rel = index['sections']['relocation']
    result = {}
    for off in range(rel['offset'], rel['offset'] + rel['size'], 4):
        value = word(b, off)
        tag, pos = value >> 25, value & 0x1ffffff
        if tag == 1:
            loc = base + pos
        elif tag < 0x20:
            loc = base + pos * 4
        else:
            loc = cmd + pos * 4
        assert loc not in result
        result[loc] = tag
    return result

def name(b, index, offset, relocs):
    if offset not in relocs:
        assert word(b, offset) == 0
        return None
    assert relocs[offset] == 1
    start = index['sections']['strings']['offset'] + word(b, offset)
    end = index['sections']['strings']['offset'] + index['sections']['strings']['size']
    stop = b.index(0, start, end)
    return b[start:stop].decode('utf-8')

def commands(b, index, offset, count):
    start = index['sections']['commands']['offset'] + offset
    end = start + count * 4
    assert count <= 100000 and end <= index['sections']['commands']['offset'] + index['sections']['commands']['size']
    result = {}
    sources = {}
    at = start
    while at < end:
        value, head = struct.unpack_from('<II', b, at)
        reg, mask, more = head & 0xffff, (head >> 16) & 15, (head >> 20) & 0x7ff
        byte_mask = sum(255 << (8 * i) for i in range(4) if mask >> i & 1)
        step = bool(head >> 31)
        for i in range(more + 1):
            loc = at if i == 0 else at + 4 + 4 * i
            assert loc + 4 <= end
            target = reg + (i if step else 0)
            value = word(b, loc)
            result[target] = (result.get(target, 0) & ~byte_mask) | (value & byte_mask)
            sources[target] = loc
        at += 4 * (2 + more)
        at = (at + 7) & ~7
    assert at == end
    return result, sources

ad = read('a/0/1/3', 75)
assert ad[:4] == b'AD\x0c\0'
ad_offsets = struct.unpack_from('<13I', ad, 4)
ad_sections = sections(ad, b'AD')
texture_bytes = ad_sections[11]
assert texture_bytes[:6] == b'BCH\0\x07\x07'
ti = inspect(texture_bytes)
tr = ptrs(texture_bytes, ti)
textures = []
for entry in ti['resources']['textures']:
    at = entry['object_offset']
    assert tr[at] == 2
    command_offset, count = struct.unpack_from('<II', texture_bytes, at)
    regs, sources = commands(texture_bytes, ti, command_offset, count)
    fmt, size, data = regs[0x8e], regs[0x82], regs[0x85]
    assert tr[sources[0x85]] == 0x24
    width, height = size >> 16, size & 0xffff
    assert fmt in (12, 13)
    payload_size = width * height // (2 if fmt == 12 else 1)
    absolute = ti['sections']['raw_data']['offset'] + data
    assert data + payload_size <= ti['sections']['raw_data']['size']
    n = name(texture_bytes, ti, at + 28, tr)
    assert entry['name'] == n
    textures.append({**entry, 'width': width, 'height': height, 'format': 'ETC1' if fmt == 12 else 'ETC1A4',
                     'format_code': fmt, 'raw_data_relative_offset': data, 'payload_offset': absolute,
                     'payload_size': payload_size, 'payload_sha256': hashlib.sha256(texture_bytes[absolute:absolute+payload_size]).hexdigest(),
                     'unit0_command_offset': ti['sections']['commands']['offset'] + command_offset, 'unit0_command_words': count,
                     'format_mipmap_flags': word(texture_bytes, at + 24)})
cursor = 0
for texture in sorted(textures, key=lambda item: item['raw_data_relative_offset']):
    assert texture['raw_data_relative_offset'] == cursor
    cursor += texture['payload_size']
assert cursor == ti['sections']['raw_data']['size']

g = read('a/0/4/1', 165)
b = sections(g, b'GR')[1]
bi = inspect(b)
br = ptrs(b, bi)
model = bi['resources']['models'][0]['object_offset']
table = bi['sections']['contents']['offset'] + word(b, model + 0x34)
count = word(b, model + 0x38)
texture_by_name = {t['name']: t for t in textures}
materials = []
for i in range(count):
    at = table + 0x58 * i
    parameter_offset = bi['sections']['contents']['offset'] + word(b, at)
    assert parameter_offset == bi['resources']['materials'][i]['object_offset']
    names = [name(b, bi, at + 0x48 + 4*j, br) for j in range(3)]
    for n in names:
        assert n is None or n in texture_by_name
    regs, sources = commands(b, bi, word(b, at + 0x10), word(b, at + 0x14))
    coords = []
    for j in range(3):
        p = parameter_offset + 12 + j * 24
        projection = word(b, p)
        coords.append({'packed_projection_and_camera': projection, 'scale_rotation_translation': struct.unpack_from('<5f', b, p + 4)})
    materials.append({'index': i, 'record_offset': at, 'parameter_offset': parameter_offset,
                      'name': name(b, bi, at + 0x54, br), 'texture_units': names,
                      'texture_indices': [texture_by_name[n]['index'] if n else None for n in names],
                      'texture_command_registers': {hex(k): hex(v) for k,v in regs.items()},
                      'coordinators': coords})

p = ROMFS / 'a/0/1/3'
with p.open('rb') as f:
    _, entry_count, members, index_hash = archive_index(f, p.stat().st_size)
member = next(m for m in members if m['entry'] == 75 and m['subentry'] == 0)
with p.open('rb') as stream:
    stream.seek(member['offset'])
    compressed_member = stream.read(member['size'])
assert compressed_member[0] == 17
member = {**member, 'sha256': hashlib.sha256(compressed_member).hexdigest(),
          'compression': 'LZ11'}
zh = sections(read('a/0/1/2', 5), b'ZO')[0]
assert struct.unpack_from('<HH', zh, 2) == (75, 12)
result = {
    'scope': 'Exact selected-file metadata and name links; no claim of runtime texture binding, projected-texture behavior, or rendering equivalence.',
    'chain': {'zone': 5, 'zone_archive': 'a/0/1/2', 'zone_header_area_u16_offset': 2, 'area': 75, 'matrix': 12, 'region': 165,
              'area_archive': 'a/0/1/3', 'area_archive_index_sha256': index_hash, 'area_archive_entries': entry_count,
              'area_archive_bytes': p.stat().st_size,
              'area_member': member, 'ad_sha256': hashlib.sha256(ad).hexdigest(), 'ad_size': len(ad),
              'ad_section_offsets': ad_offsets, 'world_texture_section': 11, 'world_texture_compression': 'none'},
    'world_bch': {'sha256': ti['sha256'], 'size': len(texture_bytes), 'sections': ti['sections'], 'textures': textures},
    'field_bch': {'sha256': bi['sha256'], 'size': len(b), 'model': bi['resources']['models'][0],
                  'material_record_table': table, 'material_record_stride': 0x58, 'materials': materials},
    'cautions': ['Zero string pointer fields must be distinguished using relocations: a zero relocated pointer is a valid first string, while a zero unrelocated field is null.',
                 'Four materials use projection_dummy in texture unit0 and the ordinary named image in unit1. Their full shader/TEV behavior remains to be reconstructed.',
                 'Raw payload sizes here describe the single base level implied by observed texture objects and exact raw-section coverage.']}

# Compare every measured field, while preserving the baseline's descriptive
# provenance and pinned source manifest. Those prose fields are not observations
# freshly inferred from game data or revalidated against remote source servers.
baseline_path = Path(__file__).with_name('area75-texture-association.json')
baseline = json.loads(baseline_path.read_text())
result = json.loads(json.dumps(result))  # Normalize tuples to JSON arrays.

def compare_and_merge(measured, recorded, path='root'):
    if isinstance(measured, dict):
        assert isinstance(recorded, dict), path
        merged = dict(recorded)
        for key, value in measured.items():
            assert key in recorded, path + '.' + key
            merged[key] = compare_and_merge(value, recorded[key], path + '.' + key)
        return merged
    assert measured == recorded, 'baseline mismatch: ' + path
    return measured

for tag, record in baseline['world_bch']['relocation_flags'].items():
    assert sum(value == int(tag, 16) for value in tr.values()) == record['count']
assert set(tr.values()) == {int(tag, 16) for tag in baseline['world_bch']['relocation_flags']}
result = compare_and_merge(result, baseline)
assert result['local_reproduction']['script_sha256'] == hashlib.sha256(Path(__file__).read_bytes()).hexdigest()
assert result['world_bch']['payload_coverage']['base_level_sum'] == cursor

# Perform all verification before creating outputs. Exclusive opens and mkdir
# preserve existing paths even if one appears after the initial checks.
with args.output.open('x', encoding='utf-8') as stream:
    if args.extract_dir is not None:
        args.extract_dir.mkdir(exist_ok=False)
        for filename, payload in [('field165.bch', b), ('area75-world-textures.bch', texture_bytes)]:
            with (args.extract_dir / filename).open('xb') as target:
                target.write(payload)
    stream.write(json.dumps(result, indent=2) + '\n')
print(f'PASS: {len(materials)} materials, {len(textures)} textures, '
      f'{cursor} raw texture bytes; recorded observations match')
print(f'Metadata: {args.output}')
if args.extract_dir is not None:
    print(f'Local BCH extracts: {args.extract_dir}')
