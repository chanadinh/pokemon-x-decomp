"""Independent check of the skinned player model and its rest pose.

The model is a/0/2/1 entry 2, MM section 0 (tr0002_00_fi). This verifier:
1. Extracts it from the archive (GARC + LZ11 + MM) with the readers of
   ../bch-walk/verify_walk.py, and hash-pins it.
2. Parses the skeleton (SPICA bd29a78 H3DBone, 100-byte records), submesh
   skinning modes and palettes (H3DSubMesh), and bone index/weight attributes
   (stored u8 indices; weights scaled by uniform 8.w; fixed float24 values;
   PICA default fill 0,0,0,1) itself, and compares them with the native
   decoder through the dump_player bridge.
3. Builds rest-pose bone matrices from scale/rotation/translation (S, Rx, Ry,
   Rz, T, child then parent) and checks them against the stored inverse bind
   matrices; poses every index the way SPICA's DefaultVertexShader
   translation does, and compares with the native rest-pose bake.
4. Optionally runs the viewer with the player on the walk marker.
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
combiners = load('combiners', '../bch-combiners/verify_combiners.py')
BCH, u32 = combiners.BCH, combiners.u32


def f32(x):
    return struct.unpack('<f', struct.pack('<f', x))[0]


def bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def from_bits(b):
    return struct.unpack('<f', struct.pack('<I', b))[0]


def floats(data, at, n):
    return list(struct.unpack_from('<%df' % n, data, at))


# ---------------------------------------------------------------- parsing
def parse(data):
    bch = BCH(data)
    model = bch.ptr(bch.ptr(bch.base['contents'], 'contents'), 'contents')
    out = {'model_flags': data[model], 'bone_scaling': data[model + 1], 'bones': [], 'meshes': []}
    first = bch.ptr(model + 0x70, 'contents')
    for i in range(u32(data, model + 0x74)):
        at = first + 100 * i
        out['bones'].append({'name': bch.string(at + 92).decode('latin-1'), 'flags': u32(data, at),
                             'parent': struct.unpack_from('<h', data, at + 4)[0],
                             'scale': floats(data, at + 8, 3), 'rotation': floats(data, at + 20, 3),
                             'translation': floats(data, at + 32, 3), 'inverse': floats(data, at + 44, 12)})
    meshes = bch.ptr(model + 0x40, 'contents')
    for m in range(u32(data, model + 0x44)):
        at = meshes + 56 * m
        writes = combiners.command_writes(bch, at + 8)
        values, _ = combiners.state(writes)
        uniforms, fixed, index, pending, slot, queue = {}, {}, None, [], 0, []
        for register, value, _b, _s in writes:
            if register == 0x2c0:
                index, pending = value & 0xff, []
            elif 0x2c1 <= register <= 0x2c8:
                pending.append(from_bits(value))
                if len(pending) == 4:
                    uniforms[index], index, pending = list(reversed(pending)), index + 1, []
            elif register == 0x232:
                slot, queue = value & 15, []
            elif 0x233 <= register <= 0x235:
                queue.append(value)
                if len(queue) == 3:
                    w0, w1, w2 = queue
                    fixed[slot] = [combiners.float24(w2 & 0xffffff), combiners.float24((w1 & 0xffff) << 8 | w2 >> 24),
                                   combiners.float24((w0 & 0xff) << 16 | w1 >> 16), combiners.float24(w0 >> 8)]
                    slot, queue = slot + 1, []
        formats = values[0x201] | values[0x202] << 32
        permutation = values[0x2bb] | values.get(0x2bc, 0) << 32
        total = (values[0x242] & 0xff) + 1
        config = values[0x205]
        order = values[0x204] | (config & 0xffff) << 32
        stride, stored = config >> 16 & 0xff, config >> 28
        base = bch.ptr([s for r, _v, _b, s in writes if r == 0x203][-1], 'raw')
        subs_at = bch.ptr(at + 16, 'contents')
        submeshes, largest, flat = [], 0, []
        for s in range(u32(data, at + 20)):
            sub = subs_at + 52 * s
            count = struct.unpack_from('<H', data, sub + 2)[0]
            face = combiners.command_writes(bch, sub + 44)
            fvalues, _ = combiners.state(face)
            source = [src for r, _v, _b, src in face if r == 0x227][-1]
            wide = bch.reloc[source][0] == 0x27
            ibase = bch.ptr(source, 'raw')
            idx = [struct.unpack_from('<H', data, ibase + 2 * k)[0] if wide else data[ibase + k]
                   for k in range(fvalues[0x228])]
            largest = max(largest, max(idx))
            submeshes.append({'skinning': data[sub], 'palette': list(struct.unpack_from('<%dH' % count, data, sub + 4)),
                              'indices': idx})
        vertices = largest + 1
        weight_scale = uniforms[8][3]
        position = [[0.0] * 3 for _ in range(vertices)]
        normal = [[0.0] * 3 for _ in range(vertices)]
        bone_index = [[0, 0, 0, 1] for _ in range(vertices)]
        bone_weight = [[0.0, 0.0, 0.0, 1.0] for _ in range(vertices)]
        index_source = weight_source = weight_elements = 0
        offset = 0
        for j in range(stored):
            s = order >> (4 * j) & 15
            semantic = permutation >> (4 * s) & 15
            fmt = formats >> (4 * s) & 15
            kind, elements = fmt & 3, (fmt >> 2) + 1
            width = (1, 1, 2, 4)[kind]
            scale = {0: uniforms[7][0], 1: uniforms[7][1], 7: 1.0, 8: weight_scale}.get(semantic)
            for v in range(vertices):
                for c in range(elements):
                    raw = struct.unpack_from('<' + 'bBhf'[kind], data, base + v * stride + offset + c * width)[0]
                    if semantic == 0:
                        position[v][c] = f32(raw * scale + uniforms[6][c])
                    elif semantic == 1:
                        normal[v][c] = f32(raw * scale)
                    elif semantic == 7:
                        assert kind == 1
                        bone_index[v][c] = raw
                    elif semantic == 8:
                        bone_weight[v][c] = raw
            if semantic == 7:
                index_source = 1
            if semantic == 8:
                weight_source, weight_elements = 1, elements
            offset += width * elements
        for s in range(total):
            if formats >> 48 >> s & 1:
                semantic = permutation >> (4 * s) & 15
                if semantic == 7:
                    index_source = 2
                    bone_index = [[int(x) for x in fixed[s]] for _ in range(vertices)]
                elif semantic == 8:
                    weight_source = 2
                    bone_weight = [list(fixed[s]) for _ in range(vertices)]
                elif semantic == 1:
                    normal = [[f32(x) for x in fixed[s][:3]] for _ in range(vertices)]
        if weight_source == 0:
            bone_weight = [[0.0] * 4 for _ in range(vertices)]
        bone_weight = [[f32(w * weight_scale) for w in ws] for ws in bone_weight]
        out['meshes'].append({'bone_index_source': index_source, 'bone_weight_source': weight_source,
                              'bone_weight_elements': weight_elements, 'indices': [x for v in bone_index for x in v],
                              'weights': [x for v in bone_weight for x in v], 'submeshes': submeshes,
                              'position': position, 'normal': normal})
    return out


# ---------------------------------------------------------------- posing
def mul(a, b):
    """3x4 affine product a * b (column vectors, rows of four)."""
    out = []
    for r in range(3):
        for k in range(4):
            v = a[r * 4 + 3] if k == 3 else 0.0
            for j in range(3):
                v += a[r * 4 + j] * b[j * 4 + k]
            out.append(v)
    return out


def local(bone):
    sx, sy, sz = bone['scale']
    rx, ry, rz = bone['rotation']
    x = [1, 0, 0, 0, 0, math.cos(rx), -math.sin(rx), 0, 0, math.sin(rx), math.cos(rx), 0]
    y = [math.cos(ry), 0, math.sin(ry), 0, 0, 1, 0, 0, -math.sin(ry), 0, math.cos(ry), 0]
    z = [math.cos(rz), -math.sin(rz), 0, 0, math.sin(rz), math.cos(rz), 0, 0, 0, 0, 1, 0]
    s = [sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, sz, 0]
    m = mul(mul(mul(z, y), x), s)
    for r in range(3):
        m[r * 4 + 3] = bone['translation'][r]
    return m


def rest_world(bones):
    world = []
    for bone in bones:
        m = local(bone)
        world.append(mul(world[bone['parent']], m) if bone['parent'] >= 0 else m)
    return world


def pose(model, mesh, sub, world, v):
    def palette(slot):
        bone = sub['palette'][slot]
        m = world[bone]
        return mul(m, model['bones'][bone]['inverse']) if sub['skinning'] == 1 else m

    p, n = mesh['position'][v], mesh['normal'][v]
    idx, wt = mesh['indices'][4 * v:4 * v + 4], mesh['weights'][4 * v:4 * v + 4]
    terms = []
    if sub['skinning'] == 1:
        for c in range(4):
            if c == 2 and wt[2] == 0:
                continue
            if c == 3 and (mesh['bone_weight_elements'] != 4 or wt[3] == 0):
                continue
            terms.append((palette(min(idx[c], 19)), wt[c]))
    else:
        terms.append((palette(min(idx[0], 19) if sub['skinning'] == 2 else 0), 1.0))
    out_p, out_n = [0.0] * 3, [0.0] * 3
    for m, w in terms:
        for r in range(3):
            out_p[r] += w * (m[r * 4] * p[0] + m[r * 4 + 1] * p[1] + m[r * 4 + 2] * p[2] + m[r * 4 + 3])
            out_n[r] += w * (m[r * 4] * n[0] + m[r * 4 + 1] * n[1] + m[r * 4 + 2] * n[2])
    length = math.sqrt(sum(x * x for x in out_n))
    return [f32(x) for x in out_p], [f32(x / length) if length > 0 else 0.0 for x in out_n]


def compare(label, a, b):
    if a != b:
        raise AssertionError(f'{label}: {str(a)[:200]} != {str(b)[:200]}')


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('--romfs', required=True, type=Path)
    parser.add_argument('--bridge', required=True, type=Path)
    parser.add_argument('--viewer', type=Path)
    parser.add_argument('--output-dir', required=True, type=Path)
    args = parser.parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=True)
    member = walk.garc_member(args.romfs / 'a/0/2/1', 2)
    pack = walk.lz11(member)
    parts = walk.sections(pack, b'MM')
    data = parts[0]
    model_path = args.output_dir / 'tr0002_00_fi.bch'
    model_path.write_bytes(data)
    report = {'member_sha256': hashlib.sha256(member).hexdigest(), 'model_sha256': hashlib.sha256(data).hexdigest(),
              'mm_sections': [len(p) for p in parts],
              'verifier_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
              'bridge_source_sha256': hashlib.sha256((HERE / 'dump_player.c').read_bytes()).hexdigest()}
    expected = parse(data)
    native = json.loads(subprocess.run([str(args.bridge), str(model_path)], capture_output=True, text=True,
                                       check=True, timeout=300).stdout)
    compare('model flags', [native['model_flags'], native['bone_scaling']],
            [expected['model_flags'], expected['bone_scaling']])
    compare('bone count', len(native['bones']), len(expected['bones']))
    for i, (n, e) in enumerate(zip(native['bones'], expected['bones'])):
        compare(f'bone {i}', [n['name'], n['flags'], n['parent']], [e['name'], e['flags'], e['parent']])
        for key in ('scale', 'rotation', 'translation', 'inverse'):
            compare(f'bone {i} {key}', n[key], [bits(x) for x in e[key]])
    vertex_attributes = 0
    for i, (n, e) in enumerate(zip(native['meshes'], expected['meshes'])):
        for key in ('bone_index_source', 'bone_weight_source', 'bone_weight_elements', 'indices'):
            compare(f'mesh {i} {key}', n[key], e[key])
        compare(f'mesh {i} weights', n['weights'], [bits(x) for x in e['weights']])
        compare(f'mesh {i} submeshes', [(s['skinning'], s['palette'], s['index_count']) for s in n['submeshes']],
                [(s['skinning'], s['palette'], len(s['indices'])) for s in e['submeshes']])
        vertex_attributes += len(e['indices']) // 4
    # Rest pose: stored inverse bind matrices against S R T composition.
    world = rest_world(expected['bones'])
    worst_identity = 0.0
    for bone, w in zip(expected['bones'], world):
        product = mul(w, bone['inverse'])
        worst_identity = max(worst_identity, max(abs(product[j] - (1.0 if j % 5 == 0 else 0.0)) for j in range(12)))
    baked_p = [from_bits(b) for b in native['baked_positions']]
    baked_n = [from_bits(b) for b in native['baked_normals']]
    k = 0
    worst_p = worst_n = smooth_displacement = 0.0
    modes = {0: 0, 1: 0, 2: 0}
    for mesh in expected['meshes']:
        for sub in mesh['submeshes']:
            for v in sub['indices']:
                p, n = pose(expected, mesh, sub, world, v)
                worst_p = max(worst_p, max(abs(a - b) for a, b in zip(p, baked_p[3 * k:3 * k + 3])))
                worst_n = max(worst_n, max(abs(a - b) for a, b in zip(n, baked_n[3 * k:3 * k + 3])))
                if sub['skinning'] == 1:
                    smooth_displacement = max(smooth_displacement,
                                              max(abs(a - b) for a, b in zip(p, mesh['position'][v])))
                modes[sub['skinning']] += 1
                k += 1
    compare('baked count', k, len(baked_p) // 3)
    assert worst_p <= 1e-4 and worst_n <= 1e-4, (worst_p, worst_n)
    report.update({'bones': len(expected['bones']), 'meshes': len(expected['meshes']),
                   'vertices_with_bone_attributes': vertex_attributes,
                   'posed_indices': k, 'indices_by_skinning': {'none': modes[0], 'smooth': modes[1], 'rigid': modes[2]},
                   'max_rest_world_times_inverse_minus_identity': worst_identity,
                   'max_smooth_rest_displacement': smooth_displacement,
                   'max_position_difference': worst_p, 'max_normal_difference': worst_n,
                   'bone_names': [b['name'] for b in expected['bones']],
                   'all_decoded_fields_match': True})
    assert worst_identity < 1e-4
    if args.viewer:
        command = [str(args.viewer), '--romfs', str(args.romfs), '--archive', 'a/0/4/1', '--entry', '165',
                   '--section', '1', '--triangle-list', '--textured', '--combiners',
                   '--texture-archive', 'a/0/1/3', '--texture-entry', '75', '--texture-section', '11',
                   '--environment-archive', 'a/0/0/3', '--environment-entry', '137', '--environment-section', '0',
                   '--walk', '--player-archive', 'a/0/2/1', '--player-entry', '2', '--player-section', '0',
                   '--steps', 'DDDRRR']
        inspect = subprocess.run(command + ['--inspect'], capture_output=True, text=True, check=True,
                                 timeout=120).stdout
        assert 'player_archive=a/0/2/1 player_entry=2 player_section=0\n' in inspect
        capture = args.output_dir / 'player-walk.bmp'
        if capture.exists():
            capture.unlink()
        result = subprocess.run(command + ['--software', '--frames', '3', '--self-test-input',
                                           '--capture-frame', str(capture)], capture_output=True, text=True,
                                check=True, timeout=300, env={'SDL_VIDEODRIVER': 'dummy', 'PATH': '/usr/bin:/bin'})
        assert 'WASD' in result.stderr
        standalone = subprocess.run([str(args.viewer), '--romfs', str(args.romfs), '--archive', 'a/0/2/1',
                                     '--entry', '2', '--section', '0', '--skinned', '--triangle-list', '--textured',
                                     '--combiners', '--texture-archive', 'a/0/2/1', '--texture-entry', '2',
                                     '--texture-section', '0', '--environment-archive', 'a/0/0/3',
                                     '--environment-entry', '137', '--environment-section', '0', '--inspect'],
                                    capture_output=True, text=True, check=True, timeout=120).stdout
        report['viewer'] = {
            'walk_line': [l for l in inspect.split('\n') if l.startswith('walk ')][0],
            'capture_sha256': hashlib.sha256(capture.read_bytes()).hexdigest(),
            'player_light_setups': sorted({l.split(' light=')[1] for l in standalone.split('\n')
                                           if l.startswith('light_setup')}),
            'player_bindings': [l.split(' alpha_test')[0] for l in standalone.split('\n') if l.startswith('combiner ')]}
    (args.output_dir / 'reference-comparison.json').write_text(json.dumps(report, indent=1) + '\n')
    print(json.dumps({k: v for k, v in report.items() if k not in ('bone_names',)}, indent=1)[:3000])


if __name__ == '__main__':
    main()
