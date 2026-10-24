"""Independent check of open-world zone scenes and zone crossing.

1. For every zone of a/0/1/2: re-derives the scene with the rule of this
   record (the zone's cells from the matrix's sub-cell zone table, or every
   cell with a region when the matrix has none; their bounding box grown by
   one cell on each side within the matrix; every cell of that box with a
   region; the scene region is the zone's first own cell in row-major order,
   placed at its offset from the box's first cell), its props, NPC tiles and
   warp tiles, and compares every zone_regions, prop, NPC tile and warp tile
   line of the viewer's --inspect for the zones that load; the others are
   listed with the viewer's reason. Readers and the prop/registry parsers are
   those of ../bch-town/verify_town.py.
2. With --no-props the viewer must still reproduce the captures of
   ../bch-zone (frames 0, 60, 200) and ../bch-talk.
3. Zone crossing on the real data: an independent walk over the joined
   grids of Vaniville Town (zone 0), Route 1 (zone 258) and Aquacorde Town
   (zone 8) predicts where each crossing happens and where the walk ends; the
   viewer's --steps output must match. Then the same walk in the display
   loop: crossing lines and capture hashes, also with --silly giant and
   --silly moonwalk.
Run with python3 -I. Output may contain local asset data.
"""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path
import struct
import subprocess

HERE = Path(__file__).resolve().parent


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, HERE / path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


town = load('town', '../bch-town/verify_town.py')
walk = town.walk
ENV = town.ENV


def scene(number, matrix):
    width, height, fields, zones = matrix
    if width == 1 and height == 1:
        if fields[0] < 0:
            return {'error': 'has no region'}
        return {'multi': False, 'x0': 0, 'z0': 0, 'regions': [(fields[0], 0, 0)]}

    def own(cx, cz):
        if fields[cz * width + cx] < 0:
            return False
        if zones is None:
            return True
        return any(zones[(cz * 4 + k // 4) * width * 4 + cx * 4 + k % 4] == number for k in range(16))
    members = [(cx, cz) for cz in range(height) for cx in range(width) if own(cx, cz)]
    if not members:
        return {'error': 'no cell of matrix'}
    mx, mz = members[0]
    x0, z0 = max(min(c[0] for c in members) - 1, 0), max(min(c[1] for c in members) - 1, 0)
    x1 = min(max(c[0] for c in members) + 1, width - 1)
    z1 = min(max(c[1] for c in members) + 1, height - 1)
    cols, rows = x1 - x0 + 1, z1 - z0 + 1
    if cols * rows > 64:
        return {'error': 'at most 64'}
    regions = [(fields[mz * width + mx], 720 * (mx - x0), 720 * (mz - z0))]
    for cz in range(rows):
        for cx in range(cols):
            if fields[(cz + z0) * width + cx + x0] >= 0 and (cx + x0, cz + z0) != (mx, mz):
                regions.append((fields[(cz + z0) * width + cx + x0], 720 * cx, 720 * cz))
    return {'multi': True, 'x0': x0, 'z0': z0, 'cols': cols, 'rows': rows, 'regions': regions,
            'zones': zones, 'width': width, 'fields': fields}


def expected_lines(number, area, matrix_id, s, entities, registries, props_cache):
    out = town.expected_lines(None, number, area, matrix_id, s, entities, registries, props_cache)
    if s['multi']:
        entry, ox, oz = s['regions'][0]
        line = (f"zone_regions zone={number} matrix={matrix_id} cells={s['cols']}x{s['rows']} "
                f"origin_cell={s['x0']},{s['z0']} scene={entry}@{ox:.0f},{oz:.0f}")
        for entry, ox, oz in s['regions'][1:]:
            line += f' region={entry}@{ox:.0f},{oz:.0f}'
        out['regions'] = line + f" tile_origin={40 * s['x0']},{40 * s['z0']}"
    return out


def grid_of(romfs, entry):
    data = town.member(romfs, 'a/0/4/1', entry)
    tiles = walk.sections(data, b'GR')[0]
    w, h = struct.unpack_from('<HH', tiles, 0)
    assert (w, h) == (40, 40)
    return struct.unpack_from('<1600I', tiles, 4)


def predict_crossing(romfs, matrix, start, steps):
    """Walk world tiles over the joined region grids of the whole matrix:
    exact 0x00000020 cells; every move into a sub-cell of another zone is a
    crossing (zone 0's town has no NPC or warp on this path, checked by the
    viewer's own lines)."""
    width, height, fields, zones = matrix
    grids = {}

    def tile(x, z):
        cx, cz = x // 40, z // 40
        if not (0 <= cx < width and 0 <= cz < height) or fields[cz * width + cx] < 0:
            return None
        entry = fields[cz * width + cx]
        if entry not in grids:
            grids[entry] = grid_of(romfs, entry)
        return grids[entry][(z % 40) * 40 + x % 40]

    def zone_at(x, z):
        return zones[(z // 10) * width * 4 + x // 10]
    x, z = start
    current = zone_at(x, z)
    crossings, moves, blocked = [], 0, 0
    for step in steps:
        dx = {'L': -1, 'R': 1}.get(step, 0)
        dz = {'U': -1, 'D': 1}.get(step, 0)
        if tile(x + dx, z + dz) == 0x20:
            x, z = x + dx, z + dz
            moves += 1
            here = zone_at(x, z)
            if here >= 0 and here != current:
                crossings.append((current, here, x, z))
                current = here
        else:
            blocked += 1
    return crossings, (x, z), current, moves, blocked


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('--romfs', required=True, type=Path)
    parser.add_argument('--viewer', required=True, type=Path)
    parser.add_argument('--output-dir', required=True, type=Path)
    parser.add_argument('--skip-runtime', action='store_true')
    args = parser.parse_args()
    out = args.output_dir
    out.mkdir(parents=True, exist_ok=True)
    report = {'archive_sha256': {a: hashlib.sha256((args.romfs / a).read_bytes()).hexdigest()
                                 for a in ('a/0/1/2', 'a/0/4/2', 'a/0/4/1', 'a/0/1/3', 'a/0/2/4')},
              'verifier_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
              'town_verifier_sha256': hashlib.sha256((HERE / '../bch-town/verify_town.py').read_bytes()).hexdigest()}
    props_cache = {}
    for entry in range(town.garc_count(args.romfs / 'a/0/4/1')):
        data = town.member(args.romfs, 'a/0/4/1', entry)
        if data[:2] == b'GR':
            part = town.section(data, b'GR', 3)
            if part is not None:
                props_cache[entry] = town.parse_props(part)
    registries = {}
    for area in range(town.garc_count(args.romfs / 'a/0/1/3')):
        data = town.member(args.romfs, 'a/0/1/3', area)
        if data[:2] == b'AD':
            registries[area] = town.parse_registry(town.section(data, b'AD', 0))

    zones = town.garc_count(args.romfs / 'a/0/1/2') - 1
    matrices, per_zone = {}, {}
    summary = {'zones': zones, 'loaded': 0, 'multi_region_loaded': 0, 'failed': {}, 'props_drawn': 0,
               'props_not_drawn': 0, 'compared_props': 0, 'compared_npc_tiles': 0, 'compared_warp_tiles': 0,
               'neighbour_regions_left_out': 0}
    for number in range(zones):
        parts = walk.sections(town.member(args.romfs, 'a/0/1/2', number), b'ZO')
        area, matrix_id = struct.unpack_from('<HH', parts[0], 2)
        if matrix_id not in matrices:
            matrices[matrix_id] = town.parse_matrix(town.member(args.romfs, 'a/0/4/2', matrix_id))
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
        want = expected_lines(number, area, matrix_id, s, town.entities_tiles(parts[1]), registries, props_cache)
        got = town.viewer_lines(result.stdout)
        assert got['regions'] == want.get('regions'), (number, got['regions'], want.get('regions'))
        assert got['props'] == want['props'], number
        assert got['npc_tiles'] == want['npc_tiles'], number
        assert got['warp_tiles'] == want['warp_tiles'], number
        left_out = result.stderr.count('(a neighbouring cell; left out)')
        summary['loaded'] += 1
        summary['multi_region_loaded'] += s['multi']
        summary['neighbour_regions_left_out'] += left_out
        summary['props_drawn'] += got['drawn'].count('yes')
        summary['props_not_drawn'] += got['drawn'].count('no')
        summary['compared_props'] += len(want['props'])
        summary['compared_npc_tiles'] += len(want['npc_tiles'])
        summary['compared_warp_tiles'] += len(want['warp_tiles'])
        per_zone[number] = {'loaded': True, 'multi_region': s['multi'], 'regions': len(s['regions']),
                            'neighbours_left_out': left_out, 'props': len(want['props'])}
    report['zones'] = summary
    report['per_zone'] = per_zone

    play = 'DDDRDDLRRRRUUUURRUU.'
    earlier = {}
    for frame in (0, 60, 200):
        earlier[f'bch-zone:{frame}'] = town.capture(args.viewer, args.romfs, out, f'noprops-{frame}',
                                                    ['--zone', '5', '--play', play, '--software', '--frames', '520',
                                                     '--capture-at', str(frame), '--no-props'])[0]
    earlier['bch-talk'] = town.capture(args.viewer, args.romfs, out, 'noprops-talk',
                                       ['--zone', '4', '--arrive', '1', '--play', 'RRRRUUUURRUU.', '--software',
                                        '--frames', '330', '--capture-at', '329', '--no-props'])[0]
    assert earlier == {
        'bch-zone:0': '39dac8226ebeb9a727ee733a6eb6a1ebd034b7de76ee09e61c8abec170944d55',
        'bch-zone:60': '0c42e3bae3a63d2ae7edaa4a96818cbef6aed87a602f16533a481355d9043e2e',
        'bch-zone:200': 'd34994de67b90d4b71d25a780f88d58e7e246ab47ba16d57b62a92aa3bc4b851',
        'bch-talk': '7de47c74d5bb5dc18b4967a5ba14e59a84af68366c2678f748dc0cc60afc3621'}, earlier
    report['earlier_captures_without_props'] = earlier

    # 3. Vaniville -> Route 1 -> Aquacorde.
    steps = 'R' + 'U' * 70
    inspect = subprocess.run([str(args.viewer), '--romfs', str(args.romfs), '--zone', '0', '--inspect'],
                             capture_output=True, text=True, check=True, timeout=600, env=ENV).stdout
    start_line = next(l for l in inspect.split('\n') if l.startswith('zone_walk '))
    sx, sz = map(int, start_line.split(' cell=')[1].split(' ')[0].split(','))
    origin = next(l for l in inspect.split('\n') if l.startswith('zone_regions ')).split(' tile_origin=')[1]
    ox, oz = map(int, origin.split(','))
    crossings, end, final_zone, moves, blocked = predict_crossing(args.romfs, matrices[0], (sx + ox, sz + oz), steps)
    result = subprocess.run([str(args.viewer), '--romfs', str(args.romfs), '--zone', '0', '--steps', steps,
                             '--inspect'], capture_output=True, text=True, check=True, timeout=600, env=ENV).stdout
    got = [l for l in result.split('\n') if l.startswith(('zone_cross ', 'zone_walk ', 'warp from_zone='))]
    want_cross = [f'zone_cross from_zone={a} to_zone={b} world_tile={x},{z} result=loaded' for a, b, x, z in crossings]
    assert [l.rsplit(' cell=', 1)[0] for l in got[:-1]] == want_cross, (got, want_cross)
    last = got[-1]
    regions = next(l for l in subprocess.run(
        [str(args.viewer), '--romfs', str(args.romfs), '--zone', str(final_zone), '--inspect'], capture_output=True,
        text=True, check=True, timeout=600, env=ENV).stdout.split('\n') if l.startswith('zone_regions '))
    fx, fz = map(int, regions.split(' tile_origin=')[1].split(','))
    assert last.startswith(f'zone_walk zone={final_zone} cell={end[0] - fx},{end[1] - fz} facing=2 moves={moves} '
                           f'blocked={blocked} '), (last, end, moves, blocked)
    runtime = {'steps': steps, 'start_world_tile': [sx + ox, sz + oz], 'crossings': crossings,
               'end_world_tile': list(end), 'end_zone': final_zone, 'inspect_lines': got}
    play = steps.lower()
    captures, lines = {}, None
    # The walk's end in Aquacorde; mid-walk on Route 1 (frame 300) plain and
    # moonwalking; the giant at the end.
    for name, frame, extra in (('aquacorde', 759, ()), ('route1', 300, ()), ('giant', 759, ('--silly', 'giant')),
                               ('moonwalk', 300, ('--silly', 'moonwalk'))):
        captures[name], stdout = town.capture(args.viewer, args.romfs, out, f'route-{name}',
                                              ['--zone', '0', '--play', play, '--software', '--frames', '760',
                                               '--capture-at', str(frame), *extra])
        lines = [l for l in stdout.split('\n') if l.startswith('zone_cross ')]
        assert [l.rsplit(' cell=', 1)[0] for l in lines] == want_cross, (name, lines)
    assert len(set(captures.values())) == 4, captures
    runtime['display_lines'] = lines
    runtime['capture_sha256'] = captures
    report['runtime'] = runtime
    (out / 'reference-comparison.json').write_text(json.dumps(report, indent=1) + '\n')
    print(json.dumps({k: v for k, v in report.items() if k != 'per_zone'}, indent=1)[:6000])


if __name__ == '__main__':
    main()
