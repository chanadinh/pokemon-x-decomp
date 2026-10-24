"""Independent check of zone entities, NPC registries and the --zone walk.

1. Reads every zone of a/0/1/2 (GARC, LZ11, ZO minipack) with the readers of
   ../bch-walk/verify_walk.py, parses section 1's entity header and records
   itself (pk3DS 6daaca9 / CTRMap 4b780a7 layout) and compares every count,
   NPC field and warp field with the native decoder through dump_zone; the
   same for every NPC registry member of a/1/4/9.
2. Re-implements the --zone walk policy (exact standard cells 0x00000020,
   NPC tiles block, a step into a warp that is not receive-only follows it
   when the target zone has a single-region matrix, arrival on the target
   warp's first tile then one step in its face direction when walkable and
   not a warp) and predicts scripted walks through the player's house, which
   the viewer's --inspect output must match exactly.
3. Optionally runs the viewer's display loop with --play on the real data:
   the runtime warp and talk lines and capture hashes.
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


walk = load('walk', '../bch-walk/verify_walk.py')


def garc_count(path):
    data = path.read_bytes()
    assert data[:4] == b'CRAG'
    header = struct.unpack_from('<I', data, 4)[0]
    assert data[header:header + 4] == b'OTAF'
    return struct.unpack_from('<H', data, header + 8)[0]


def member(romfs, archive, entry):
    data = walk.garc_member(romfs / archive, entry)
    return walk.lz11(data) if data[:1] == b'\x11' else data


def tile(world):
    return world // 18  # floor division, as the native decoder


def parse_entities(e):
    length = struct.unpack_from('<I', e, 0)[0]
    furniture, npcs, warps, trigger1 = e[4:8]
    trigger2 = struct.unpack_from('<I', e, 8)[0]
    assert length == 8 + furniture * 0x14 + npcs * 0x30 + (warps + trigger1 + trigger2) * 0x18
    assert length + 4 <= len(e)
    p = 12 + furniture * 0x14
    out = {'length': length, 'counts': [furniture, npcs, warps, trigger1, trigger2], 'npcs': [], 'warps': []}
    for _ in range(npcs):
        fields = list(struct.unpack_from('<8H', e, p)) + list(struct.unpack_from('<2HI', e, p + 0x28))
        out['npcs'].append(fields)
        p += 0x30
    for _ in range(warps):
        raw = e[p:p + 0x18]
        zone, target, face, transition, ctype = struct.unpack_from('<2H2BH', raw, 0)
        if ctype == 0:
            x, height, y, width, length_, direction = struct.unpack_from('<3h2hH', raw, 8)
            out['warps'].append({'supported': 1, 'fields': [zone, target, face, transition, ctype, x, height, y,
                                                            width, length_, direction, tile(x), tile(y)],
                                 'raw': raw.hex()})
        else:
            out['warps'].append({'supported': 0, 'fields': [zone, target, face, transition, ctype] + [0] * 8,
                                 'raw': raw.hex()})
        p += 0x18
    return out


# ---------------------------------------------------------------- walk policy
FACE = {0: (0, -1), 1: (0, 1), 2: (-1, 0), 3: (1, 0)}       # zone face: up, down, left, right
WALKER = {0: (0, 1), 1: (1, 0), 2: (0, -1), 3: (-1, 0)}     # viewer facing: D, R, U, L
TO_WALKER = {0: 2, 1: 0, 2: 3, 3: 1}
STEP = {'D': 0, 'R': 1, 'U': 2, 'L': 3}


class Zone:
    def __init__(self, romfs, number):
        parts = walk.sections(member(romfs, 'a/0/1/2', number), b'ZO')
        self.number = number
        self.area, self.matrix = struct.unpack_from('<HH', parts[0], 2)
        self.entities = parse_entities(parts[1])
        m = walk.sections(member(romfs, 'a/0/4/2', self.matrix), b'MM')[0]
        width, height = struct.unpack_from('<HH', m, 4)
        self.single = width == 1 and height == 1 and struct.unpack_from('<h', m, 8)[0] >= 0
        if self.single:
            self.region = struct.unpack_from('<h', m, 8)[0]
            tiles = walk.sections(member(romfs, 'a/0/4/1', self.region), b'GR')[0]
            self.width, self.height = struct.unpack_from('<HH', tiles, 0)
            self.cells = struct.unpack_from('<%dI' % (self.width * self.height), tiles, 4)
        self.npcs = {(n[8], n[9]) for n in self.entities['npcs']}

    def walkable(self, x, z):
        return (0 <= x < self.width and 0 <= z < self.height and self.cells[z * self.width + x] == 0x20
                and (x, z) not in self.npcs)

    def warp_at(self, x, z):
        for i, w in enumerate(self.entities['warps']):
            f = w['fields']
            if w['supported'] and f[11] <= x < f[11] + f[8] and f[12] <= z < f[12] + f[9]:
                return i
        return -1


def simulate(romfs, number, steps, arrive=None):
    zone = Zone(romfs, number)
    state = {'moves': 0, 'blocked': 0, 'warps': 0, 'facing': 0, 'log': []}
    start = next((x, z) for z in range(zone.height) for x in range(zone.width) if zone.walkable(x, z))
    x, z = start

    def arrive_at(zone, index):
        f = zone.entities['warps'][index]['fields']
        ax, az, facing = f[11], f[12], TO_WALKER[f[2]]
        dx, dz = WALKER[facing]
        stepped = zone.walkable(ax + dx, az + dz) and zone.warp_at(ax + dx, az + dz) < 0
        if stepped:
            ax, az = ax + dx, az + dz
            state['moves'] += 1
        return ax, az, facing, stepped

    if arrive is not None:
        x, z, state['facing'], _ = arrive_at(zone, arrive)
    for step in steps:
        state['facing'] = STEP[step]
        dx, dz = WALKER[state['facing']]
        k = zone.warp_at(x + dx, z + dz)
        if k >= 0 and zone.entities['warps'][k]['fields'][10] != 2:
            state['warps'] += 1
            f = zone.entities['warps'][k]['fields']
            target = Zone(romfs, f[0])
            if target.single and f[1] < len(target.entities['warps']) and target.entities['warps'][f[1]]['supported']:
                ax, az, facing, stepped = arrive_at(target, f[1])
                state['log'].append(f'warp from_zone={zone.number} warp={k} target_zone={f[0]} target_warp={f[1]} '
                                    f'result=loaded arrival=cell:{ax},{az} stepped={int(stepped)}')
                zone, x, z, state['facing'] = target, ax, az, facing
                continue
            state['log'].append(f'warp from_zone={zone.number} warp={k} target_zone={f[0]} target_warp={f[1]} '
                                f'result=not-loaded')
        if zone.walkable(x + dx, z + dz):
            x, z = x + dx, z + dz
            state['moves'] += 1
        else:
            state['blocked'] += 1
    state['log'].append(f'zone_walk zone={zone.number} cell={x},{z} facing={state["facing"]} moves={state["moves"]} '
                        f'blocked={state["blocked"]} warps={state["warps"]} '
                        'policy=standard-cells-npcs-block-warps-on-step')
    return state['log']


WALKS = [  # (zone, --arrive, --steps): bedroom to the stairs; downstairs to the NPC; the front door
    (5, None, 'DDDRDDL'),
    (4, 1, 'RRRRUUUURRUU'),
    (4, 1, 'DRRRRD'),
    (5, None, 'DDDRDDLUR'),
]


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('--romfs', required=True, type=Path)
    parser.add_argument('--bridge', required=True, type=Path)
    parser.add_argument('--viewer', type=Path)
    parser.add_argument('--output-dir', required=True, type=Path)
    args = parser.parse_args()
    out = args.output_dir
    (out / 'zones').mkdir(parents=True, exist_ok=True)
    (out / 'registries').mkdir(parents=True, exist_ok=True)
    report = {'archive_sha256': {a: hashlib.sha256((args.romfs / a).read_bytes()).hexdigest()
                                 for a in ('a/0/1/2', 'a/0/4/2', 'a/1/4/9')},
              'verifier_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
              'bridge_source_sha256': hashlib.sha256((HERE / 'dump_zone.c').read_bytes()).hexdigest()}
    entries = garc_count(args.romfs / 'a/0/1/2')
    # The last entry is not a ZO pack: it is checked below to be every zone
    # header (56 bytes) concatenated in zone order.
    zones = entries - 1
    files, expected, headers = [], [], b''
    for number in range(zones):
        parts = walk.sections(member(args.romfs, 'a/0/1/2', number), b'ZO')
        headers += parts[0]
        path = out / 'zones' / f'{number}.bin'
        path.write_bytes(parts[1])
        files.append(str(path))
        expected.append(parse_entities(parts[1]))
    native = [json.loads(line) for line in subprocess.run(
        [str(args.bridge), 'entities', *files], capture_output=True, text=True, check=True,
        timeout=300).stdout.splitlines()]
    assert len(native) == zones
    table = member(args.romfs, 'a/0/1/2', zones)
    assert table == headers, 'last a/0/1/2 entry is not the concatenated zone headers'
    totals = {'furniture': 0, 'npcs': 0, 'warps': 0, 'trigger1': 0, 'trigger2': 0}
    types = {'0': 0, '1': 0}
    for number, (n, e) in enumerate(zip(native, expected)):
        assert n['result'] == 0, number
        assert [n['length'], n['counts'], n['npcs']] == [e['length'], e['counts'], e['npcs']], number
        assert n['warps'] == e['warps'], number
        for key, count in zip(totals, e['counts']):
            totals[key] += count
        for w in e['warps']:
            types[str(w['fields'][4])] += 1
    registries = garc_count(args.romfs / 'a/1/4/9')
    files, expected_reg = [], []
    for number in range(registries):
        data = member(args.romfs, 'a/1/4/9', number)
        path = out / 'registries' / f'{number}.bin'
        path.write_bytes(data)
        files.append(str(path))
        assert len(data) % 0x18 == 0, number
        expected_reg.append([[struct.unpack_from('<H', data, i)[0], struct.unpack_from('<H', data, i + 0x14)[0]]
                             for i in range(0, len(data), 0x18)])
    native_reg = [json.loads(line) for line in subprocess.run(
        [str(args.bridge), 'registry', *files], capture_output=True, text=True, check=True,
        timeout=300).stdout.splitlines()]
    for number, (n, e) in enumerate(zip(native_reg, expected_reg)):
        assert n['result'] == 0 and n['entries'] == e, number
    report.update({'zones': zones, 'zone_archive_entries': entries,
                   'last_entry': 'concatenated 56-byte zone headers, zone order', 'entity_totals': totals, 'warp_coordinate_types': types,
                   'registries': registries, 'registry_entries': sum(map(len, expected_reg)),
                   'all_entities_and_registries_match': True,
                   'house': {str(z): {'area': Zone(args.romfs, z).area, 'matrix': Zone(args.romfs, z).matrix,
                                      'entities': expected[z]} for z in (4, 5)}})
    if args.viewer:
        walks = []
        for number, arrive, steps in WALKS:
            command = [str(args.viewer), '--romfs', str(args.romfs), '--zone', str(number), '--steps', steps]
            if arrive is not None:
                command += ['--arrive', str(arrive)]
            stdout = subprocess.run(command + ['--inspect'], capture_output=True, text=True, check=True,
                                    timeout=300).stdout
            got = [l for l in stdout.split('\n') if l.startswith(('warp from_zone=', 'zone_walk '))]
            want = simulate(args.romfs, number, steps, arrive)
            assert got == want, (number, steps, got, want)
            walks.append({'zone': number, 'arrive': arrive, 'steps': steps, 'lines': got})
        report['predicted_walks'] = walks
        env = {'SDL_VIDEODRIVER': 'dummy', 'PATH': '/usr/bin:/bin'}
        play = 'DDDRDDLRRRRUUUURRUU.'
        runtime = {}
        for frame in (0, 60, 200, 519):
            capture = out / f'play-{frame}.bmp'
            if capture.exists():
                capture.unlink()
            result = subprocess.run([str(args.viewer), '--romfs', str(args.romfs), '--zone', '5', '--play', play,
                                     '--software', '--frames', '520', '--capture-frame', str(capture),
                                     '--capture-at', str(frame)], capture_output=True, text=True, check=True,
                                    timeout=900, env=env)
            runtime[str(frame)] = hashlib.sha256(capture.read_bytes()).hexdigest()
            lines = [l for l in result.stdout.split('\n') if l.startswith(('warp from_zone=', 'talk '))]
        assert lines == ['warp from_zone=5 warp=0 target_zone=4 target_warp=1 result=loaded arrival=cell:4,10 '
                         'stepped=1', 'talk zone=4 npc=0 id=0 script=1'], lines
        report['runtime'] = {'play': play, 'frames': 520, 'lines': lines, 'capture_sha256': runtime}
    (out / 'reference-comparison.json').write_text(json.dumps(report, indent=1) + '\n')
    print(json.dumps({k: v for k, v in report.items() if k != 'house'}, indent=1)[:4000])


if __name__ == '__main__':
    main()
