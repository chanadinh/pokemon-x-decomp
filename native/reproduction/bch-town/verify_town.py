"""Independent check of multi-region zone scenes, region props and the town walk.

1. Reads every region of a/0/4/1 (GR section 3) and every area of a/0/1/3
   (AD section 0) with the readers of ../bch-walk/verify_walk.py, parses the
   prop records and prop registries itself (CTRMap 4b780a7 GRProp.java and
   ADPropRegistry.java layouts) and compares every field with the native
   decoders through dump_town.
2. For every zone of a/0/1/2: re-derives the scene from the zone header and
   its map matrix (a/0/4/2: cells whose sub-cell zone table names the zone,
   their bounding box, the origin cell's region; every cell with a region
   when the matrix has no zone table), the scene-frame tiles of its
   NPCs and warps (world tiles less 40 per origin cell) and its props (every
   region's GR section 3, the area registry's model or the uid itself,
   positions less 360 + 720 * origin cell, yaw in degrees), then runs the
   viewer's --inspect and compares every zone_regions, prop, NPC tile and
   warp tile line of the zones that load; the others are listed with the
   viewer's reason.
3. With --no-props the viewer must reproduce the earlier records' captures
   (../bch-zone frames 0, 60 and 200; ../bch-talk's talk capture).
4. Runtime on the real data: out of the house (zone 4) into Vaniville Town
   (zone 0), across the town to an NPC and a talk; warp, talk and message
   lines and capture hashes, and one capture drawn in 1 and in 2 row bands.
Run with python3 -I. Output may contain local asset data.
"""
import argparse
import hashlib
import importlib.util
import json
import math
from pathlib import Path
import struct
import subprocess

HERE = Path(__file__).resolve().parent


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, HERE / path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


walk = load('walk', '../bch-walk/verify_walk.py')
ENV = {'SDL_VIDEODRIVER': 'dummy', 'PATH': '/usr/bin:/bin'}


def garc_count(path):
    data = path.read_bytes()
    assert data[:4] == b'CRAG'
    header = struct.unpack_from('<I', data, 4)[0]
    assert data[header:header + 4] == b'OTAF'
    return struct.unpack_from('<H', data, header + 8)[0]


def member(romfs, archive, entry):
    data = walk.garc_member(romfs / archive, entry)
    return walk.lz11(data) if data[:1] == b'\x11' else data


def section(data, magic, index):
    """A minipack section, raw (prop and registry sections are not
    compressed; their counts may start with 0x10 or 0x11)."""
    parts = walk.sections(data, magic)
    return parts[index] if index < len(parts) else None


def parse_props(data):
    count = struct.unpack_from('<I', data, 0)[0]
    assert count <= 4096 and 4 + 0x2C * count <= len(data)
    out = []
    for i in range(count):
        record = struct.unpack_from('<I9fI', data, 4 + 0x2C * i)
        out.append(record)
    return out


def float_bits(value):
    return struct.unpack('<I', struct.pack('<f', value))[0]


def parse_registry(data):
    count = struct.unpack_from('<I', data, 0)[0]
    assert count <= 4096 and 4 + 80 * count <= len(data)
    return [list(struct.unpack_from('<HHBB', data, 4 + 80 * i)) for i in range(count)]


def parse_matrix(data):
    body = section(data, b'MM', 0)
    lod, _, width, height = struct.unpack_from('<4H', body, 0)
    cells = width * height
    fields = list(struct.unpack_from('<%dh' % cells, body, 8))
    zones = list(struct.unpack_from('<%dh' % (cells * 16), body, 8 + 2 * cells)) if lod else None
    return width, height, fields, zones


def scene(number, matrix):
    """The zone's scene as the viewer derives it, or the reason it cannot."""
    width, height, fields, zones = matrix
    if width == 1 and height == 1:
        if fields[0] < 0:
            return {'error': 'has no region'}
        return {'multi': False, 'x0': 0, 'z0': 0, 'regions': [(fields[0], 0, 0)]}
    def member_of(cx, cz):
        if fields[cz * width + cx] < 0:
            return False
        if zones is None:  # no zone table: every cell with a region
            return True
        return any(zones[(cz * 4 + k // 4) * width * 4 + cx * 4 + k % 4] == number for k in range(16))
    members = [(cx, cz) for cz in range(height) for cx in range(width) if member_of(cx, cz)]
    if not members:
        return {'error': 'no cell of matrix'}
    x0, z0 = min(c[0] for c in members), min(c[1] for c in members)
    x1, z1 = max(c[0] for c in members), max(c[1] for c in members)
    cols, rows = x1 - x0 + 1, z1 - z0 + 1
    if cols * rows > 64:
        return {'error': 'at most 64'}
    if (x0, z0) not in members:
        return {'error': 'the first cell of its bounding box is not its own'}
    regions = [(fields[z0 * width + x0], 0, 0)]
    for cz in range(rows):
        for cx in range(cols):
            if (cx or cz) and (cx + x0, cz + z0) in members:
                regions.append((fields[(cz + z0) * width + cx + x0], 720 * cx, 720 * cz))
    return {'multi': True, 'x0': x0, 'z0': z0, 'cols': cols, 'rows': rows, 'regions': regions}


def g9(value):
    return '%.9g' % value


def expected_lines(romfs, number, area, matrix_id, s, entities, registries, props_cache):
    out = {}
    if s['multi']:
        line = (f"zone_regions zone={number} matrix={matrix_id} cells={s['cols']}x{s['rows']} "
                f"origin_cell={s['x0']},{s['z0']} scene={s['regions'][0][0]}")
        for entry, ox, oz in s['regions'][1:]:
            line += f' region={entry}@{ox:.0f},{oz:.0f}'
        out['regions'] = line + f" tile_origin={40 * s['x0']},{40 * s['z0']}"
    registry = {}
    for reference, model, _, _ in registries.get(area) or []:
        registry.setdefault(reference, model)
    ox, oz = 360.0 + 720.0 * s['x0'], 360.0 + 720.0 * s['z0']
    props = []
    for entry, _, _ in s['regions']:
        for record in props_cache.get(entry, []):
            uid, sx, sy, sz, rx, ry, rz, px, py, pz, _ = record
            yaw = ry * (math.pi / 180.0)
            props.append(f"region={entry} uid={uid} model_entry=a/0/2/4:{registry.get(uid, uid)} "
                         f"position={g9(px - ox)},{g9(py)},{g9(pz - oz)} yaw_degrees={g9(yaw * (180.0 / math.pi))} "
                         f"scale={g9(sx)},{g9(sy)},{g9(sz)}")
    out['props'] = props
    dx, dz = 40 * s['x0'], 40 * s['z0']
    out['npc_tiles'] = [f'{n[0] - dx},{n[1] - dz}' for n in entities['npcs']]
    out['warp_tiles'] = [f'{w[0] - dx},{w[1] - dz}' for w in entities['warps']]
    return out


def entities_tiles(zo):
    e = zo
    furniture, npcs, warps = e[4], e[5], e[6]
    p = 12 + furniture * 0x14
    out = {'npcs': [], 'warps': []}
    for _ in range(npcs):
        out['npcs'].append(struct.unpack_from('<2H', e, p + 0x28))
        p += 0x30
    for _ in range(warps):
        ctype = struct.unpack_from('<H', e, p + 6)[0]
        if ctype == 0:
            x, _, y = struct.unpack_from('<3h', e, p + 8)
            out['warps'].append((x // 18, y // 18))
        p += 0x18
    return out


def viewer_lines(stdout):
    lines = stdout.split('\n')
    out = {'regions': next((l for l in lines if l.startswith('zone_regions ')), None)}
    out['props'] = [l.split(' ', 2)[2].rsplit(' drawn=', 1)[0] for l in lines if l.startswith('prop index=')]
    out['drawn'] = [l.rsplit(' drawn=', 1)[1] for l in lines if l.startswith('prop index=')]
    out['npc_tiles'] = [l.split(' tile=')[1].split(' ')[0] for l in lines if l.startswith('npc index=')]
    out['warp_tiles'] = [l.split(' tiles=')[1].split('+')[0] for l in lines
                         if l.startswith('warp index=') and 'supported=yes' in l]
    return out


def capture(viewer, romfs, out, name, arguments):
    path = out / f'{name}.bmp'
    if path.exists():
        path.unlink()
    result = subprocess.run([str(viewer), '--romfs', str(romfs), *arguments, '--capture-frame', str(path)],
                            capture_output=True, text=True, check=True, timeout=1800, env=ENV)
    return hashlib.sha256(path.read_bytes()).hexdigest(), result.stdout


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('--romfs', required=True, type=Path)
    parser.add_argument('--bridge', required=True, type=Path)
    parser.add_argument('--viewer', required=True, type=Path)
    parser.add_argument('--output-dir', required=True, type=Path)
    parser.add_argument('--skip-runtime', action='store_true')
    args = parser.parse_args()
    out = args.output_dir
    for sub in ('props', 'registries'):
        (out / sub).mkdir(parents=True, exist_ok=True)
    report = {'archive_sha256': {a: hashlib.sha256((args.romfs / a).read_bytes()).hexdigest()
                                 for a in ('a/0/1/2', 'a/0/4/2', 'a/0/4/1', 'a/0/1/3', 'a/0/2/4')},
              'verifier_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
              'bridge_source_sha256': hashlib.sha256((HERE / 'dump_town.c').read_bytes()).hexdigest()}

    # 1. Props and registries against the native decoders.
    props_cache, files, expected = {}, [], []
    regions = garc_count(args.romfs / 'a/0/4/1')
    for entry in range(regions):
        data = member(args.romfs, 'a/0/4/1', entry)
        if data[:2] != b'GR':
            continue
        part = section(data, b'GR', 3)
        if part is None:
            continue
        path = out / 'props' / f'{entry}.bin'
        path.write_bytes(part)
        files.append(str(path))
        records = parse_props(part)
        props_cache[entry] = records
        expected.append([[r[0], *map(float_bits, r[1:10]), r[10]] for r in records])
    native = [json.loads(l) for l in subprocess.run([str(args.bridge), 'props', *files], capture_output=True,
                                                     text=True, check=True, timeout=600).stdout.splitlines()]
    assert [n['result'] for n in native] == [0] * len(files)
    assert [n['props'] for n in native] == expected
    report['props'] = {'regions': len(files), 'records': sum(map(len, expected)),
                       'yaw_degrees': sorted({float(r[5]) for rs in props_cache.values() for r in rs}),
                       'non_identity_scale': sum(r[1:4] != (1.0, 1.0, 1.0) for rs in props_cache.values() for r in rs),
                       'non_yaw_rotation': sum(r[4] != 0 or r[6] != 0 for rs in props_cache.values() for r in rs)}
    registries, files, expected = {}, [], []
    for area in range(garc_count(args.romfs / 'a/0/1/3')):
        data = member(args.romfs, 'a/0/1/3', area)
        if data[:2] != b'AD':
            continue
        part = section(data, b'AD', 0)
        path = out / 'registries' / f'{area}.bin'
        path.write_bytes(part)
        files.append(str(path))
        registries[area] = parse_registry(part)
        expected.append(registries[area])
    native = [json.loads(l) for l in subprocess.run([str(args.bridge), 'registry', *files], capture_output=True,
                                                     text=True, check=True, timeout=600).stdout.splitlines()]
    assert [n['result'] for n in native] == [0] * len(files)
    assert [n['entries'] for n in native] == expected
    report['registries'] = {'areas': len(files), 'entries': sum(map(len, expected)),
                            'reference_equals_model': sum(e[0] == e[1] for es in expected for e in es)}

    # 2. Every zone's scene against the viewer's --inspect.
    zones = garc_count(args.romfs / 'a/0/1/2') - 1
    matrices = {}
    summary = {'zones': zones, 'loaded': 0, 'multi_region_loaded': 0, 'failed': {}, 'props_drawn': 0,
               'props_not_drawn': 0, 'compared_props': 0, 'compared_npc_tiles': 0, 'compared_warp_tiles': 0}
    per_zone = {}
    for number in range(zones):
        parts = walk.sections(member(args.romfs, 'a/0/1/2', number), b'ZO')
        area, matrix_id = struct.unpack_from('<HH', parts[0], 2)
        if matrix_id not in matrices:
            matrices[matrix_id] = parse_matrix(member(args.romfs, 'a/0/4/2', matrix_id))
        s = scene(number, matrices[matrix_id])
        result = subprocess.run([str(args.viewer), '--romfs', str(args.romfs), '--zone', str(number), '--inspect'],
                                capture_output=True, text=True, timeout=600, env=ENV)
        if result.returncode:
            reason = (result.stderr.strip().split('\n') or ['?'])[-1].replace('[model-viewer] ', '')
            for key in ('zone %d: ' % number, 'zone %d ' % number):
                reason = reason.replace(key, '')
            summary['failed'][reason] = summary['failed'].get(reason, 0) + 1
            per_zone[number] = {'loaded': False, 'reason': reason, 'derived': s.get('error', 'loadable scene')}
            if 'error' in s:
                assert s['error'] in reason, (number, s, reason)
            continue
        assert 'error' not in s, (number, s)
        want = expected_lines(args.romfs, number, area, matrix_id, s, entities_tiles(parts[1]), registries,
                              props_cache)
        got = viewer_lines(result.stdout)
        assert got['regions'] == want.get('regions'), (number, got['regions'], want.get('regions'))
        assert got['props'] == want['props'], (number, got['props'], want['props'])
        assert got['npc_tiles'] == want['npc_tiles'], number
        assert got['warp_tiles'] == want['warp_tiles'], number
        summary['loaded'] += 1
        summary['multi_region_loaded'] += s['multi']
        summary['props_drawn'] += got['drawn'].count('yes')
        summary['props_not_drawn'] += got['drawn'].count('no')
        summary['compared_props'] += len(want['props'])
        summary['compared_npc_tiles'] += len(want['npc_tiles'])
        summary['compared_warp_tiles'] += len(want['warp_tiles'])
        per_zone[number] = {'loaded': True, 'multi_region': s['multi'], 'regions': len(s['regions']),
                            'props': len(want['props']), 'props_drawn': got['drawn'].count('yes')}
    report['zones'] = summary
    report['per_zone'] = per_zone
    zone0 = per_zone.get(0)
    assert zone0 and zone0['loaded'] and zone0['multi_region'], zone0

    # 3. Earlier captures without props.
    play = 'DDDRDDLRRRRUUUURRUU.'
    earlier = {}
    for frame in (0, 60, 200):
        earlier[f'bch-zone:{frame}'] = capture(args.viewer, args.romfs, out, f'noprops-{frame}',
                                               ['--zone', '5', '--play', play, '--software', '--frames', '520',
                                                '--capture-at', str(frame), '--no-props'])[0]
    earlier['bch-talk'] = capture(args.viewer, args.romfs, out, 'noprops-talk',
                                  ['--zone', '4', '--arrive', '1', '--play', 'RRRRUUUURRUU.', '--software',
                                   '--frames', '330', '--capture-at', '329', '--no-props'])[0]
    report['earlier_captures_without_props'] = earlier
    assert earlier == {
        'bch-zone:0': '39dac8226ebeb9a727ee733a6eb6a1ebd034b7de76ee09e61c8abec170944d55',
        'bch-zone:60': '0c42e3bae3a63d2ae7edaa4a96818cbef6aed87a602f16533a481355d9043e2e',
        'bch-zone:200': 'd34994de67b90d4b71d25a780f88d58e7e246ab47ba16d57b62a92aa3bc4b851',
        'bch-talk': '7de47c74d5bb5dc18b4967a5ba14e59a84af68366c2678f748dc0cc60afc3621'}, earlier

    # 4. Runtime: out of the house, across the town, talk to NPC 1.
    if not args.skip_runtime:
        town = 'DDDDDDDDDDDLLLLLLLLLLLLLLLLLLLLULLUUUUUUULL'
        play = 'DRRRRD' + town + 'L..'
        inspect = subprocess.run([str(args.viewer), '--romfs', str(args.romfs), '--zone', '4', '--arrive', '1',
                                  '--steps', 'DRRRRD' + town + 'L', '--inspect'], capture_output=True, text=True,
                                 check=True, timeout=600, env=ENV).stdout
        walked = [l for l in inspect.split('\n') if l.startswith(('warp from_zone=', 'zone_walk '))]
        assert walked == ['warp from_zone=4 warp=0 target_zone=0 target_warp=1 result=loaded arrival=cell:35,37 '
                          'stepped=1', 'zone_walk zone=0 cell=11,40 facing=3 moves=50 blocked=1 warps=1 '
                          'policy=standard-cells-npcs-block-warps-on-step'], walked
        frames = 1300
        runtime, lines = {}, None
        for frame in (150, 600, 1299):
            runtime[str(frame)], stdout = capture(args.viewer, args.romfs, out, f'town-{frame}',
                                                  ['--zone', '4', '--arrive', '1', '--play', play, '--software',
                                                   '--frames', str(frames), '--capture-at', str(frame)])
            lines = [l for l in stdout.split('\n') if l.startswith(('warp from_zone=', 'talk ', 'message '))]
        assert lines[:2] == ['warp from_zone=4 warp=0 target_zone=0 target_warp=1 result=loaded arrival=cell:35,37 '
                             'stepped=1', 'talk zone=0 npc=1 id=1 script=6'], lines
        bands = {}
        for threads in (1, 2):
            bands[str(threads)] = capture(args.viewer, args.romfs, out, f'bands-{threads}',
                                          ['--zone', '0', '--arrive', '1', '--software', '--frames', '3',
                                           '--capture-at', '2', '--threads', str(threads)])[0]
        assert bands['1'] == bands['2'], bands
        report['runtime'] = {'play': play, 'frames': frames, 'lines': lines, 'capture_sha256': runtime,
                             'bands_capture_sha256': bands}
    (out / 'reference-comparison.json').write_text(json.dumps(report, indent=1) + '\n')
    print(json.dumps({k: v for k, v in report.items() if k != 'per_zone'}, indent=1)[:6000])


if __name__ == '__main__':
    main()
