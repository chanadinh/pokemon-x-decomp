"""Independent check of the Pokemon battle models (a/0/0/7).

1. The species index, entry 0: four bytes per species from 1 (u16 first
   model, u8 model count, u8 flags). Checks that the 722 records (Bulbasaur
   .. the egg, 722) tile models 0 .. 980 without gaps or overlaps (the
   1,962 bytes after them are not interpreted), that the
   archive holds 4 + 8 x 981 entries, and the container kind of each of a
   model's eight files (PC, PF, PT, PT, ...), read here (GARC, LZ11 and
   minipack readers of ../bch-walk/verify_walk.py).
2. Every model's PC section 0, as the viewer selects it for a species (its
   first model): the skinned BCH parsed by ../bch-player/verify_player.py
   (skeleton, meshes, skinning sources, palettes, bone indices and weights)
   against the native decoder through the dump_player bridge; for a sample
   (every 20th model and the Route 2 species with Froakie) also the rest
   pose of every index against the native bake.
3. The viewer on Route 2 (zone 259): its battle_model lines must name the
   model and entries of step 1 and the mesh and vertex counts of step 2.
Run with python3 -I. Output may contain local asset data.
"""
import argparse
import hashlib
import importlib.util
import json
import mmap
from pathlib import Path
import struct
import subprocess

HERE = Path(__file__).resolve().parent


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, HERE / path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


player = load('player', '../bch-player/verify_player.py')
walk = player.walk
ENV = {'SDL_VIDEODRIVER': 'dummy', 'PATH': '/usr/bin:/bin'}
ROUTE2 = (656, 659, 263, 661, 664, 13, 16)


class Garc:
    """GARC reader over a memory map (the archive is about 1 GB)."""

    def __init__(self, path):
        self.file = open(path, 'rb')
        self.data = mmap.mmap(self.file.fileno(), 0, access=mmap.ACCESS_READ)
        d = self.data
        assert d[:4] == b'CRAG'
        header = struct.unpack_from('<I', d, 4)[0]
        self.data_offset = struct.unpack_from('<I', d, 0x10)[0]
        assert struct.unpack_from('<I', d, 0x14)[0] == len(d)
        fato_size, self.count = struct.unpack_from('<IH', d, header + 4)
        self.offsets = struct.unpack_from('<%dI' % self.count, d, header + 12)
        self.fatb = header + fato_size

    def member(self, entry):
        record = self.fatb + 12 + self.offsets[entry]
        assert struct.unpack_from('<I', self.data, record)[0] & 1
        start, _end, length = struct.unpack_from('<III', self.data, record + 4)
        return bytes(self.data[self.data_offset + start:self.data_offset + start + length])


def unpack(member):
    return walk.lz11(member) if member[:1] == b'\x11' else member


def compare_model(native, expected, posed):
    c = player.compare
    c('model flags', [native['model_flags'], native['bone_scaling']], [expected['model_flags'], expected['bone_scaling']])
    c('bone count', len(native['bones']), len(expected['bones']))
    for i, (n, e) in enumerate(zip(native['bones'], expected['bones'])):
        c(f'bone {i}', [n['name'], n['flags'], n['parent']], [e['name'], e['flags'], e['parent']])
        for key in ('scale', 'rotation', 'translation', 'inverse'):
            c(f'bone {i} {key}', n[key], [player.bits(x) for x in e[key]])
    c('mesh count', len(native['meshes']), len(expected['meshes']))
    for i, (n, e) in enumerate(zip(native['meshes'], expected['meshes'])):
        for key in ('bone_index_source', 'bone_weight_source', 'bone_weight_elements', 'indices'):
            c(f'mesh {i} {key}', n[key], e[key])
        c(f'mesh {i} weights', n['weights'], [player.bits(x) for x in e['weights']])
        c(f'mesh {i} submeshes', [(s['skinning'], s['palette'], s['index_count']) for s in n['submeshes']],
          [(s['skinning'], s['palette'], len(s['indices'])) for s in e['submeshes']])
    total = sum(len(s['indices']) for m in expected['meshes'] for s in m['submeshes'])
    c('baked count', len(native['baked_positions']) // 3, total)
    worst = [0.0, 0.0]
    if posed:
        world = player.rest_world(expected['bones'])
        p_bits = [player.from_bits(b) for b in native['baked_positions']]
        n_bits = [player.from_bits(b) for b in native['baked_normals']]
        k = 0
        for mesh in expected['meshes']:
            for sub in mesh['submeshes']:
                for v in sub['indices']:
                    p, n = player.pose(expected, mesh, sub, world, v)
                    worst[0] = max(worst[0], max(abs(a - b) for a, b in zip(p, p_bits[3 * k:3 * k + 3])))
                    worst[1] = max(worst[1], max(abs(a - b) for a, b in zip(n, n_bits[3 * k:3 * k + 3])))
                    k += 1
        assert worst[0] <= 1e-3 and worst[1] <= 1e-4, worst
    return total, worst


def main():
    p = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    p.add_argument('--romfs', type=Path, required=True)
    p.add_argument('--bridge', type=Path, required=True)
    p.add_argument('--viewer', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    args = p.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    garc = Garc(args.romfs / 'a/0/0/7')
    report = {'archive_bytes': len(garc.data), 'entries': garc.count,
              'verifier_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    # 1.
    index = garc.member(0)
    records = [struct.unpack_from('<HBB', index, 4 * i) for i in range(722)]
    at = 0
    for species, (first, count, _flags) in enumerate(records, 1):
        assert first == at and count >= 1, (species, first, count, at)
        at += count
    models = at
    assert garc.count == 4 + 8 * models, (garc.count, models)
    kinds = {}
    for m in range(models):
        key = tuple(unpack(garc.member(4 + 8 * m + k))[:2].decode('latin-1') for k in range(8))
        kinds[' '.join(key)] = kinds.get(' '.join(key), 0) + 1
    report['index'] = {'species': 722, 'models': models, 'tiled': True, 'bytes': len(index),
                       'bytes_after_species_records': len(index) - 4 * 722,
                       'species_with_several_models': sum(c > 1 for _f, c, _g in records),
                       'flags': sorted({f for _f, _c, f in records}), 'file_kinds': kinds}
    # 2.
    per_species = {}
    for species, (first, _c, _f) in enumerate(records, 1):
        per_species.setdefault(first, species)
    checked, posed, failed, worst = 0, 0, {}, [0.0, 0.0]
    counts = {}
    sample = {records[s - 1][0] for s in ROUTE2} | set(range(0, models, 20))
    path = args.out / 'model.bch'
    for m in range(models):
        pack = unpack(garc.member(4 + 8 * m))
        section = walk.sections(pack, b'PC')[0]
        if not section:
            failed[m] = 'empty PC section 0'
            continue
        path.write_bytes(section)
        result = subprocess.run([str(args.bridge), str(path)], capture_output=True, text=True, timeout=300)
        try:
            expected = player.parse(section)
        except Exception as error:  # noqa: BLE001 - reported, then compared with the native result
            expected = error
        if result.returncode or isinstance(expected, Exception):
            failed[m] = f'native={result.returncode} independent={type(expected).__name__ if isinstance(expected, Exception) else "ok"}'
            assert bool(result.returncode) == isinstance(expected, Exception), (m, failed[m], str(expected)[:200])
            continue
        total, w = compare_model(json.loads(result.stdout), expected, m in sample)
        counts[m] = (len(expected['meshes']), total)
        checked += 1
        posed += m in sample
        worst = [max(worst[0], w[0]), max(worst[1], w[1])]
    first_models = sorted({r[0] for r in records})
    report['models'] = {'compared': checked, 'identical': checked, 'rest_pose_compared': posed,
                        'max_position_difference': worst[0], 'max_normal_difference': worst[1],
                        'not_decoded': failed,
                        'species_first_models_decoded': sum(m in counts for m in first_models)}
    # 3.
    play = 'R' + 'D' * 9 + '.' * 4
    out = subprocess.run([str(args.viewer), '--romfs', str(args.romfs), '--zone', '259', '--play', play,
                          '--encounter-rate', '100', '--software', '--frames', '225'], capture_output=True, text=True,
                         check=True, timeout=1200, env=ENV).stdout
    lines = [l for l in out.split('\n') if l.startswith(('battle_model ', 'wild ', 'partner '))]
    for side in ('partner', 'wild'):
        species = int(next(l for l in lines if l.startswith(side + ' ')).split(' species=')[1].split()[0])
        first = records[species - 1][0]
        meshes, vertices = counts[first]
        want = (f'battle_model side={side} species={species} model={first} entry={4 + 8 * first} '
                f'texture_entry={6 + 8 * first} meshes={meshes} vertices={vertices} pose=rest')
        assert want in lines, (want, lines)
    report['runtime'] = {'zone': 259, 'play': play, 'lines': lines}
    (args.out / 'reference-comparison.json').write_text(json.dumps(report, indent=1) + '\n')
    print(json.dumps(report, indent=1)[:4000])


if __name__ == '__main__':
    main()
