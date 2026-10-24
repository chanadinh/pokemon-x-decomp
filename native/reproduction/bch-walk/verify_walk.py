"""Check that the field's room model, terrain grid and collision share one
region-local frame, and that the viewer's --walk marker follows the policy.

For every exact standard cell (0x00000020) of a/0/4/1 entry 165:
1. The cell center, (cell + 0.5) * 18 - 360, is computed here.
2. The room model (GR section 1) is decoded by the existing independent
   Python decoder (tools/inspect_bch_model.py); every nearly horizontal
   model triangle under the center gives a candidate surface height.
3. The viewer is driven to that cell with --walk --steps along a path found
   here by breadth-first search over the same policy; its reported cell,
   moves and collision height are compared, and the collision height must
   equal one of those model surfaces.
Run with python3 -I. Output may contain local asset data.
"""
import argparse
import hashlib
import json
from collections import deque
from pathlib import Path
import struct
import subprocess
import sys

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1] / 'tools'))
import inspect_bch_model  # noqa: E402

FIELD_SHA256 = 'cb131a9590850d604b30aa558b7c55fba6fff654b2f6dd05c823af5ecbc16ccd'
STANDARD = 0x00000020


def lz11(data):
    assert data[0] == 0x11
    size = data[1] | data[2] << 8 | data[3] << 16
    pos, out = 4, bytearray()
    while len(out) < size:
        flags = data[pos]; pos += 1
        for bit in range(8):
            if len(out) >= size:
                break
            if not flags & (0x80 >> bit):
                out.append(data[pos]); pos += 1
                continue
            kind = data[pos] >> 4
            if kind == 0:
                n = ((data[pos] & 15) << 4 | data[pos + 1] >> 4) + 0x11
                d = ((data[pos + 1] & 15) << 8 | data[pos + 2]) + 1; pos += 3
            elif kind == 1:
                n = ((data[pos] & 15) << 12 | data[pos + 1] << 4 | data[pos + 2] >> 4) + 0x111
                d = ((data[pos + 2] & 15) << 8 | data[pos + 3]) + 1; pos += 4
            else:
                n = kind + 1
                d = ((data[pos] & 15) << 8 | data[pos + 1]) + 1; pos += 2
            for _ in range(n):
                out.append(out[-d])
    return bytes(out)


def garc_member(path, entry):
    data = Path(path).read_bytes()
    assert data[:4] == b'CRAG'
    header = struct.unpack_from('<I', data, 4)[0]
    data_offset = struct.unpack_from('<I', data, 0x10)[0]
    fato_size, count = struct.unpack_from('<IH', data, header + 4)
    offsets = struct.unpack_from('<%dI' % count, data, header + 12)
    fatb = header + fato_size
    record = fatb + 12 + offsets[entry]
    bits = struct.unpack_from('<I', data, record)[0]
    assert bits & 1
    start, end, length = struct.unpack_from('<III', data, record + 4)
    return data[data_offset + start:data_offset + start + length]


def sections(pack, magic):
    assert pack[:2] == magic
    count = struct.unpack_from('<H', pack, 2)[0]
    offsets = struct.unpack_from('<%dI' % (count + 1), pack, 4)
    return [pack[offsets[i]:offsets[i + 1]] for i in range(count)]


def flat_heights(model, x, z):
    """Heights of every nearly horizontal model triangle containing (x, z)."""
    found = set()
    for a, b, c in model:
        det = (b[0] - a[0]) * (c[2] - a[2]) - (c[0] - a[0]) * (b[2] - a[2])
        if abs(det) < 1e-9:
            continue
        u = ((x - a[0]) * (c[2] - a[2]) - (c[0] - a[0]) * (z - a[2])) / det
        v = ((b[0] - a[0]) * (z - a[2]) - (x - a[0]) * (b[2] - a[2])) / det
        if u < -1e-9 or v < -1e-9 or u + v > 1 + 1e-9:
            continue
        ys = (a[1], b[1], c[1])
        if max(ys) - min(ys) > 1e-3:
            continue  # only flat surfaces count as floor here
        found.add(round(a[1] + u * (b[1] - a[1]) + v * (c[1] - a[1]), 6))
    return sorted(found)


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('--romfs', required=True, type=Path)
    parser.add_argument('--viewer', required=True, type=Path)
    parser.add_argument('--output-dir', required=True, type=Path)
    args = parser.parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=True)
    member = garc_member(args.romfs / 'a/0/4/1', 165)
    gr = lz11(member)
    parts = sections(gr, b'GR')
    width, height = struct.unpack_from('<HH', parts[0], 0)
    words = struct.unpack_from('<%dI' % (width * height), parts[0], 4)
    field = parts[1]
    assert hashlib.sha256(field).hexdigest() == FIELD_SHA256
    decoded = inspect_bch_model.decode_model(field)
    model = []
    for mesh in decoded['meshes']:  # positions per mesh, mesh-local indices
        p, idx = mesh['positions'], mesh['indices']
        for k in range(0, len(idx) - 2, 3):
            model.append(tuple(p[i] for i in idx[k:k + 3]))
    cells = [(x, z) for z in range(height) for x in range(width) if words[z * width + x] == STANDARD]
    start = cells[0]
    # Breadth-first search with the viewer's step order U, D, L, R.
    moves = {'U': (0, -1), 'D': (0, 1), 'L': (-1, 0), 'R': (1, 0)}
    path = {start: ''}
    queue = deque([start])
    walk = set(cells)
    while queue:
        x, z = queue.popleft()
        for letter, (dx, dz) in moves.items():
            nxt = (x + dx, z + dz)
            if nxt in walk and nxt not in path:
                path[nxt] = path[(x, z)] + letter
                queue.append(nxt)
    command = [str(args.viewer), '--romfs', str(args.romfs), '--archive', 'a/0/4/1', '--entry', '165',
               '--section', '1', '--triangle-list', '--textured', '--texture-archive', 'a/0/1/3',
               '--texture-entry', '75', '--texture-section', '11', '--walk', '--inspect']
    results = []
    for cell in cells:
        x, z = cell
        cx, cz = (x + 0.5) * 18 - 360, (z + 0.5) * 18 - 360
        heights = flat_heights(model, cx, cz)
        entry = {'cell': cell, 'local': [cx, cz], 'model_flat_heights': heights, 'reachable': cell in path}
        if cell in path:
            out = subprocess.run(command + (['--steps', path[cell]] if path[cell] else []), text=True,
                                 capture_output=True, check=True, timeout=120).stdout
            line = [l for l in out.split('\n') if l.startswith('walk ')][0]
            expected = (f'marker=cell:{x},{z} local={cx:.9g},{cz:.9g} height=')
            assert expected in line, (cell, line)
            height_text = line.split(' height=')[1].split(' ')[0]
            assert not height_text.startswith('none'), line
            assert f' moves={len(path[cell])} blocked=0' in line, line
            entry['viewer_collision_height'] = float(height_text)
            # Collision must coincide with one flat model surface under the cell.
            assert any(abs(entry['viewer_collision_height'] - h) <= 1e-3 for h in heights), entry
        results.append(entry)
    blocked = subprocess.run(command + ['--steps', 'U' * 3], text=True, capture_output=True,
                             check=True, timeout=120).stdout
    report = {
        'gr_member_sha256': hashlib.sha256(member).hexdigest(),
        'gr_decompressed_sha256': hashlib.sha256(gr).hexdigest(),
        'field_bch_sha256': FIELD_SHA256,
        'verifier_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
        'grid': [width, height], 'standard_cells': len(cells), 'start': start,
        'reachable_from_start': sum(c in path for c in cells),
        'all_reachable_cells_match': True,
        'collision_heights': sorted({r['viewer_collision_height'] for r in results if 'viewer_collision_height' in r}),
        'cells_with_flat_surface_above_collision': [
            {'cell': r['cell'], 'heights': r['model_flat_heights']} for r in results
            if r['model_flat_heights'] and max(r['model_flat_heights']) > r.get('viewer_collision_height', 0) + 1e-3],
        'three_up_from_start': [l for l in blocked.split('\n') if l.startswith('walk ')][0],
        'cells': results}
    (args.output_dir / 'reference-comparison.json').write_text(json.dumps(report, indent=1) + '\n')
    print(json.dumps({k: v for k, v in report.items() if k != 'cells'}, indent=1))


if __name__ == '__main__':
    main()
