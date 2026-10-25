"""The battle models lit by their own vertex programs (--vertex-shader).

Renders the Route 2 battle of docs/images (zone 259, seed 4: Froakie against
Zigzagoon) at frame 238 with and without --vertex-shader and checks:

1. both runs print the same lines (the option changes pixels only);
2. pixels differ, and only inside the partner's half of the screen: Froakie's
   combiners read the primary color (TEV source 0), which the option now
   takes from its program's o3; Zigzagoon's (model 354) never read it, and
   its programs run with lighting off, so its half must be identical;
3. the materials' TEV sources, read here independently with
   ../bch-textured/verify_materials.py, say which models read source 0.

Writes reference-comparison.json with both captures' hashes.
Run with python3 -I. Output may contain local asset data.
"""
import argparse
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import struct
import subprocess
import tempfile

HERE = Path(__file__).resolve().parent


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, HERE / path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


shaders = load('shaders', '../platform/verify_shaders.py')
vm = shaders.vm


def bmp(path):
    data = path.read_bytes()
    offset, = struct.unpack_from('<I', data, 10)
    width, height = struct.unpack_from('<ii', data, 18)
    bits, = struct.unpack_from('<H', data, 28)
    assert bits == 32, bits
    rows = [data[offset + 4 * width * y: offset + 4 * width * (y + 1)] for y in range(abs(height))]
    return width, abs(height), rows if height < 0 else rows[::-1]


def reads_primary(bch):
    """Materials whose six combiner stages name source 0 (primary color)."""
    out = {}
    for m in vm.reference_materials(bch):
        sources = []
        for stage in vm.STAGES:
            value = m['registers'][stage][0]
            sources += [(value >> shift) & 15 for shift in (0, 4, 8, 16, 20, 24)]
        out[m['name'].decode()] = 0 in sources
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--romfs', type=Path, required=True)
    ap.add_argument('--code-bin', type=Path, required=True)
    ap.add_argument('--viewer', type=Path, required=True)
    args = ap.parse_args()
    garc = shaders.Garc(args.romfs / 'a/0/0/7')
    primary = {m: reads_primary(shaders.walk.sections(garc.member(4 + 8 * m), b'PC')[0]) for m in (864, 354)}
    assert any(primary[864].values()) and not any(primary[354].values()), primary
    env = {**os.environ, 'SDL_VIDEODRIVER': 'dummy', 'SDL_AUDIODRIVER': 'dummy'}
    common = ['--romfs', str(args.romfs), '--zone', '259', '--seed', '4', '--play', 'RDDDDDDDDD..',
              '--encounter-rate', '100', '--code-bin', str(args.code_bin), '--software', '--frames', '240',
              '--capture-at', '238']
    runs = {}
    with tempfile.TemporaryDirectory() as scratch:
        for name, extra in (('stored_colors', []), ('vertex_shader', ['--vertex-shader'])):
            capture = Path(scratch) / f'{name}.bmp'
            result = subprocess.run([str(args.viewer), *common, '--capture-frame', str(capture), *extra],
                                    capture_output=True, text=True, env=env, timeout=1200)
            assert result.returncode == 0, result.stderr
            runs[name] = {'lines': result.stdout.splitlines(), 'image': bmp(capture),
                          'sha256': hashlib.sha256(capture.read_bytes()).hexdigest()}
    a, b = runs['stored_colors'], runs['vertex_shader']
    assert a['lines'] == b['lines']
    width, height, rows_a = a['image']
    _, _, rows_b = b['image']
    changed = [(x, y) for y in range(height) for x in range(width)
               if rows_a[y][4 * x:4 * x + 4] != rows_b[y][4 * x:4 * x + 4]]
    assert changed
    box = [min(x for x, _ in changed), min(y for _, y in changed),
           max(x for x, _ in changed), max(y for _, y in changed)]
    assert box[2] < width // 2, box  # the partner is drawn on the left half
    result = {
        'models_reading_primary_color': {str(m): sorted(k for k, v in primary[m].items() if v) for m in primary},
        'capture_sha256': {k: v['sha256'] for k, v in runs.items()},
        'changed_pixels': len(changed), 'changed_box': box, 'size': [width, height],
        'battle_lines': [line for line in a['lines'] if line.startswith(('battle_model', 'encounter', 'wild'))],
    }
    (HERE / 'reference-comparison.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
