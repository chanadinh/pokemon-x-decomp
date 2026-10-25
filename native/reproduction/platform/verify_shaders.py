"""Survey of the Pokémon models' vertex shader programs (a/0/0/7).

1. Reads every model's BCH (entry 4 + 8m, PC section 0) with the independent
   resolver of ../bch-textured/verify_materials.py and collects the shader
   resources of BCH index slot 2 (each a pointer into contents to a DVLB, and
   its size). Writes the distinct DVLBs to --out.
2. Runs native/build/pica_probe on them: each DVLB decodes, its opcodes are
   listed, and every vertex-shader entry runs to END under 256 combinations
   of its first eight bool uniforms. Geometry-shader entries (type 1) are
   expected to stop at EMIT/SETEMIT, which the interpreter does not run.
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


vm = load('vm', '../bch-textured/verify_materials.py')
walk = load('walk', '../bch-walk/verify_walk.py')


class Garc:
    def __init__(self, path):
        self.file = open(path, 'rb')
        self.data = mmap.mmap(self.file.fileno(), 0, access=mmap.ACCESS_READ)
        assert self.data[:4] == b'CRAG'
        header = struct.unpack_from('<I', self.data, 4)[0]
        self.base = struct.unpack_from('<I', self.data, 0x10)[0]
        fato, self.count = struct.unpack_from('<IH', self.data, header + 4)
        self.offsets = struct.unpack_from('<%dI' % self.count, self.data, header + 12)
        self.fatb = header + fato

    def member(self, entry):
        start, _, length = struct.unpack_from('<III', self.data, self.fatb + 16 + self.offsets[entry])
        data = bytes(self.data[self.base + start:self.base + start + length])
        return walk.lz11(data) if data[:1] == b'\x11' else data


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--romfs', type=Path, required=True)
    ap.add_argument('--probe', type=Path, required=True)
    ap.add_argument('--out', type=Path, required=True)
    ap.add_argument('--models', type=int, default=981)
    args = ap.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    garc = Garc(args.romfs / 'a/0/0/7')
    uses, first, empty, refs = {}, {}, [], 0
    for m in range(args.models):
        bch = walk.sections(garc.member(4 + 8 * m), b'PC')[0]
        if not bch:
            empty.append(m)
            continue
        b = vm.BCH(bch)
        c = b.base['contents']
        table, count = b.ptr(c + 24), vm.u32(bch, c + 28)
        for k in range(count):
            record = b.ptr(table + 4 * k)
            at, size = b.ptr(record), vm.u32(bch, record + 4)
            blob = bch[at:at + size]
            assert blob[:4] == b'DVLB', (m, k)
            digest = hashlib.sha256(blob).hexdigest()
            refs += 1
            uses[digest] = uses.get(digest, 0) + 1
            if digest not in first:
                first[digest] = m
                (args.out / f'{digest[:16]}.dvlb').write_bytes(blob)
    files = sorted(args.out / f'{d[:16]}.dvlb' for d in first)
    run = subprocess.run([str(args.probe)] + [str(f) for f in files], capture_output=True, text=True)
    print(run.stdout, end='')
    assert run.returncode == 0, run.stderr
    entries = [line.split() for line in run.stdout.splitlines() if line.startswith('  entry=')]
    field = lambda words, key: next(w.split('=', 1)[1] for w in words if w.startswith(key + '='))
    vertex = [e for e in entries if field(e, 'type') == '0']
    geometry = [e for e in entries if field(e, 'type') == '1']
    assert all(field(e, 'ok') == field(e, 'runs') for e in vertex)
    assert all('error=unsupported' in ' '.join(e) for e in geometry)
    result = {
        'models_read': args.models - len(empty), 'empty_models': empty, 'shader_resources': refs,
        'distinct_dvlb': [{'sha256': d, 'first_model': first[d], 'resources': uses[d]} for d in sorted(first)],
        'vertex_entries': len(vertex), 'vertex_runs': sum(int(field(e, 'runs')) for e in vertex),
        'geometry_entries': len(geometry), 'probe_output': run.stdout.replace(str(args.out) + '/', ''),
    }
    (HERE / 'reference-comparison.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: v for k, v in result.items() if k != 'probe_output'}, indent=2))


if __name__ == '__main__':
    main()
