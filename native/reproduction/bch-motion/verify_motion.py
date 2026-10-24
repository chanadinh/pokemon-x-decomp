"""Independent check of the GF1 motion pack and the animated player pose.

The motions are a/0/2/1 entry 2, MM section 1, beside the player model of
../bch-player (section 0). This verifier:
1. Extracts both with the readers of ../bch-walk/verify_walk.py and hash-pins
   them.
2. Parses the pack itself (SPICA bd29a78 GF1MotionPack, GF1Motion: slot table,
   motion skeleton, 3-bit channel codes, key-frame lists, values) and checks
   that every motion ends exactly where the next nonzero slot begins (or at
   the pack end); compares the skeleton, frame counts, byte ranges, channel
   kinds and every key with the native decoder through dump_motion.
3. Evaluates every present channel of the checked slots at every integer
   frame with SPICA's GetFrameValue/Herp (slopes / 30), in emulated float32
   arithmetic in the C expression order, and requires identical bits.
4. Builds bone local transforms (H3D pose fallback, motion channels by bone
   name, SPICA's world-space conversion for flag-8 bones), re-parents "glass"
   onto "loc_acchat", poses every index with ../bch-player/verify_player.py's
   rest-pose rules generalised to these worlds, and compares with the native
   pose (tolerance 1e-4; double arithmetic on both sides).
5. Optionally runs the viewer: standalone and walk inspection lines and
   captures of chosen frames.
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
SLOTS = (1, 3, 4)


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, HERE / path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


player = load('player', '../bch-player/verify_player.py')
walk = player.walk
f32, bits, from_bits, mul = player.f32, player.bits, player.from_bits, player.mul


def u16(d, o):
    return struct.unpack_from('<H', d, o)[0]


def u32(d, o):
    return struct.unpack_from('<I', d, o)[0]


def fl(d, o):
    return struct.unpack_from('<f', d, o)[0]


# ---------------------------------------------------------------- parsing
def parse_skeleton(d, o):
    count, first = d[o], d[o + 1]
    p = o + 2
    bones = [{'name': 'Origin', 'parent': -1, 'flags': 0, 'children': 0}]
    for _ in range(1, count):
        bones.append({'parent': d[p], 'flags': d[p + 1], 'children': d[p + 2]})
        p += 3
    for i in range(1, count):
        end = d.index(b'\0', p)
        bones[i]['name'] = d[p:end].decode('ascii')
        p = end + 1
    p = (p + 3) & ~3
    for bone in bones:
        bone['translation'] = list(struct.unpack_from('<3f', d, p))
        bone['rotation'] = list(struct.unpack_from('<4f', d, p + 12))
        p += 28
    return first, bones


PI = math.pi


def parse_motion(d, o, bone_count):
    code_count, frames = u16(d, o), u16(d, o + 2)
    p = o + 4
    codes, word = [], 0
    for i in range(code_count):
        if i % 8 == 0:
            word = d[p] | d[p + 1] << 8 | d[p + 2] << 16
            p += 3
        codes.append(word & 7)
        word >>= 3
    wide = frames > 255
    if wide and p & 1:
        p += 1
    lists = []
    for _ in range(sum(c > 5 for c in codes)):
        n = u16(d, p) if wide else d[p]
        p += 2 if wide else 1
        keys = [0]
        for _ in range(n):
            keys.append(u16(d, p) if wide else d[p])
            p += 2 if wide else 1
        keys.append(frames)
        lists.append(keys)
    p = (p + 3) & ~3
    channels = [[0, []] for _ in range(bone_count * 9)]
    element, next_list = 0, 0
    for c in codes[2:]:
        if c == 1:
            element += 3
            continue
        assert element < bone_count * 9 and channels[element][0] == 0
        if c <= 5:
            value = {0: 0.0, 2: f32(PI * 0.5), 3: f32(PI), 4: f32(PI * -0.5)}.get(c)
            if c == 5:
                value = fl(d, p)
                p += 4
            channels[element] = [1, [[0, value, 0.0]]]
        else:
            keys = lists[next_list]
            next_list += 1
            out = []
            for frame in keys:
                out.append([frame, fl(d, p), fl(d, p + 4) if c == 7 else 0.0])
                p += 8 if c == 7 else 4
            channels[element] = [3 if c == 7 else 2, out]
        element += 1
    assert next_list == len(lists)
    return {'frames': frames, 'channels': channels, 'end': p}


def parse_pack(d):
    slots = u32(d, 0)
    offsets = [u32(d, 4 + 4 * i) for i in range(slots + 1)]
    first, bones = parse_skeleton(d, offsets[0])
    motions = []
    for i in range(1, slots):
        if not offsets[i]:
            continue
        end = next((x for x in offsets[i + 1:] if x), len(d))
        m = parse_motion(d, offsets[i], len(bones))
        assert m['end'] == end, (i, m['end'], end)  # consumed exactly
        m.update({'slot': i, 'byte_offset': offsets[i], 'byte_length': end - offsets[i]})
        motions.append(m)
    return {'slots': slots, 'first': first, 'bones': bones, 'motions': motions}


# ---------------------------------------------------------------- evaluation
def value(kind, keys, frame):
    """SPICA H3DFloatKeyFrameGroup.GetFrameValue, float32 in C order."""
    if len(keys) == 1:
        return keys[0][1]
    lhs, rhs = keys[0], keys[-1]
    for k in keys:
        if k[0] <= frame:
            lhs = k
        if k[0] >= frame and k[0] < rhs[0]:
            rhs = k
    if lhs[0] == rhs[0]:
        return lhs[1]
    diff = f32(frame - lhs[0])
    weight = f32(diff / f32(rhs[0] - lhs[0]))
    lv, rv = lhs[1], rhs[1]
    if kind == 2:
        return f32(lv + f32(f32(rv - lv) * weight))
    ls, rs = f32(lhs[2] / f32(30.0)), f32(rhs[2] / f32(30.0))
    t = f32(f32(f32(f32(lv - rv) * f32(f32(2 * weight) - 3)) * weight) * weight)
    result = f32(lv + t)
    wm = f32(weight - 1)
    return f32(result + f32(f32(diff * wm) * f32(f32(ls * wm) + f32(rs * weight))))


def rt(t, r):
    return player.local({'scale': [1, 1, 1], 'rotation': r, 'translation': t})


def inverse(m):
    a = [[m[r * 4 + c] for c in range(3)] for r in range(3)]
    det = (a[0][0] * (a[1][1] * a[2][2] - a[1][2] * a[2][1]) - a[0][1] * (a[1][0] * a[2][2] - a[1][2] * a[2][0]) +
           a[0][2] * (a[1][0] * a[2][1] - a[1][1] * a[2][0]))
    inv = [[(a[(c + 1) % 3][(r + 1) % 3] * a[(c + 2) % 3][(r + 2) % 3] -
             a[(c + 1) % 3][(r + 2) % 3] * a[(c + 2) % 3][(r + 1) % 3]) / det for c in range(3)] for r in range(3)]
    out = []
    for r in range(3):
        out += inv[r] + [-sum(inv[r][j] * m[j * 4 + 3] for j in range(3))]
    return out


def locals_at(model, pack, motion, frame):
    bones = model['bones']
    index = {b['name']: i for i, b in enumerate(pack['bones'])}
    out, mapping = [], []
    for bone in bones:
        k = index.get(bone['name'], -1)
        mapping.append(k)
        local = list(bone['translation']) + list(bone['rotation']) + list(bone['scale'])
        if k >= 0:
            for c in range(9):
                kind, keys = motion['channels'][k * 9 + c]
                if kind:
                    local[c] = value(kind, keys, frame)
        out.append(local)
    original = [list(x) for x in out]
    for i, bone in enumerate(bones):
        k = mapping[i]
        if k < 0 or not pack['bones'][k]['flags'] & 8:
            continue
        parent = bone['parent']
        chain, b = [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0], parent
        while b >= 0:
            chain = mul(rt(original[b][0:3], original[b][3:6]), chain)
            b = bones[b]['parent']
        inv = inverse(chain)
        target = original[i][0:3]
        bt = [sum(inv[r * 4 + j] * target[j] for j in range(3)) + inv[r * 4 + 3] for r in range(3)]
        pt = original[parent][0:3]
        cross = [pt[1] * bt[2] - pt[2] * bt[1], pt[2] * bt[0] - pt[0] * bt[2], pt[0] * bt[1] - pt[1] * bt[0]]
        w = sum(a * b for a, b in zip(pt, bt)) + math.sqrt(sum(a * a for a in pt) * sum(b * b for b in bt))
        n = math.sqrt(sum(x * x for x in cross) + w * w)
        qx, qy, qz, qw = cross[0] / n, cross[1] / n, cross[2] / n, w / n
        # bt rotated by the conjugate quaternion, as a rotation matrix.
        rot = [1 - 2 * (qy * qy + qz * qz), 2 * (qx * qy + qw * qz), 2 * (qx * qz - qw * qy),
               2 * (qx * qy - qw * qz), 1 - 2 * (qx * qx + qz * qz), 2 * (qy * qz + qw * qx),
               2 * (qx * qz + qw * qy), 2 * (qy * qz - qw * qx), 1 - 2 * (qx * qx + qy * qy)]
        out[i][0:3] = [f32(sum(rot[r * 3 + j] * bt[j] for j in range(3))) for r in range(3)]
        kp = mapping[parent]
        animated = kp >= 0 and any(motion['channels'][kp * 9 + c][0] for c in (3, 4, 5))
        if not animated:
            s = max(-1.0, min(1.0, 2 * (qx * qz - qw * qy)))
            out[parent][3:6] = [f32(math.atan2(2 * (qx * qw + qy * qz), 1 - 2 * (qx * qx + qy * qy))),
                                f32(-math.asin(s)),
                                f32(math.atan2(2 * (qx * qy + qz * qw), 1 - 2 * (qy * qy + qz * qz)))]
    return out


def pose_frame(model, locals_, rest):
    bones = model['bones']
    world = []
    for i, bone in enumerate(bones):
        m = player.local({'translation': locals_[i][0:3], 'rotation': locals_[i][3:6], 'scale': locals_[i][6:9]})
        world.append(mul(world[bone['parent']], m) if bone['parent'] >= 0 else m)
    names = [b['name'] for b in bones]
    if 'glass' in names and 'loc_acchat' in names:
        g, a = names.index('glass'), names.index('loc_acchat')
        world[g] = mul(mul(world[a], inverse(rest[a])), rest[g])
    positions, normals = [], []
    for mesh in model['meshes']:
        for sub in mesh['submeshes']:
            for v in sub['indices']:
                p, n = player.pose(model, mesh, sub, world, v)
                positions += p
                normals += n
    return positions, normals


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('--romfs', required=True, type=Path)
    parser.add_argument('--bridge', required=True, type=Path)
    parser.add_argument('--viewer', type=Path)
    parser.add_argument('--output-dir', required=True, type=Path)
    args = parser.parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=True)
    member = walk.garc_member(args.romfs / 'a/0/2/1', 2)
    parts = walk.sections(walk.lz11(member), b'MM')
    model_data, pack_data = parts[0], parts[1]
    model_path, pack_path = args.output_dir / 'tr0002_00_fi.bch', args.output_dir / 'tr0002_motions.bin'
    model_path.write_bytes(model_data)
    pack_path.write_bytes(pack_data)
    report = {'member_sha256': hashlib.sha256(member).hexdigest(),
              'model_sha256': hashlib.sha256(model_data).hexdigest(),
              'pack_sha256': hashlib.sha256(pack_data).hexdigest(), 'pack_bytes': len(pack_data),
              'verifier_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
              'bridge_source_sha256': hashlib.sha256((HERE / 'dump_motion.c').read_bytes()).hexdigest()}
    pack = parse_pack(pack_data)
    native = json.loads(subprocess.run([str(args.bridge), str(model_path), str(pack_path), *map(str, SLOTS)],
                                       capture_output=True, text=True, check=True, timeout=600).stdout)
    player.compare('skeleton header', [native['bone_count'], native['first_bone'], native['slot_count']],
                   [len(pack['bones']), pack['first'], pack['slots']])
    for i, (n, e) in enumerate(zip(native['bones'], pack['bones'])):
        player.compare(f'motion bone {i}', [n['name'], n['parent'], n['flags'], n['children'], n['translation'],
                                            n['rotation']],
                       [e['name'], e['parent'], e['flags'], e['children'], [bits(x) for x in e['translation']],
                        [bits(x) for x in e['rotation']]])
    player.compare('motion count', len(native['motions']), len(pack['motions']))
    keys = 0
    kinds = {0: 0, 1: 0, 2: 0, 3: 0}
    for n, e in zip(native['motions'], pack['motions']):
        player.compare(f"slot {e['slot']} header", [n['slot'], n['frames'], n['byte_offset'], n['byte_length']],
                       [e['slot'], e['frames'], e['byte_offset'], e['byte_length']])
        for c, (nc, ec) in enumerate(zip(n['channels'], e['channels'])):
            player.compare(f"slot {e['slot']} channel {c}", nc,
                           [ec[0], [[k[0], bits(k[1]), bits(k[2])] for k in ec[1]]])
            keys += len(ec[1])
            kinds[ec[0]] += 1
    model = player.parse(model_data)
    rest = player.rest_world(model['bones'])
    checked_values = 0
    worst_local = worst_p = worst_n = 0.0
    frames_checked = {}
    by_name = {m['slot']: m for m in pack['motions']}
    for pose in native['poses']:
        motion = by_name[pose['slot']]
        for frame, record in enumerate(pose['frames']):
            expected = [[c, bits(value(*motion['channels'][c], frame))]
                        for c in range(len(motion['channels'])) if motion['channels'][c][0]]
            player.compare(f"slot {pose['slot']} frame {frame} values", record['values'], expected)
            checked_values += len(expected)
            loc = locals_at(model, pack, motion, frame)
            got = [from_bits(b) for b in record['locals']]
            worst_local = max(worst_local, max(abs(a - b) for a, b in zip(got, [x for l in loc for x in l])))
            positions, normals = pose_frame(model, loc, rest)
            gp = [from_bits(b) for b in record['positions']]
            gn = [from_bits(b) for b in record['normals']]
            player.compare('posed count', len(gp), len(positions))
            worst_p = max(worst_p, max(abs(a - b) for a, b in zip(gp, positions)))
            worst_n = max(worst_n, max(abs(a - b) for a, b in zip(gn, normals)))
        frames_checked[str(pose['slot'])] = len(pose['frames'])
    assert worst_local <= 1e-5 and worst_p <= 1e-4 and worst_n <= 1e-4, (worst_local, worst_p, worst_n)
    report.update({
        'slots': pack['slots'], 'motion_bones': len(pack['bones']), 'first_bone': pack['first'],
        'motions': len(pack['motions']), 'motions_consumed_exactly': len(pack['motions']),
        'empty_slots': [i for i in range(1, pack['slots']) if i not in by_name],
        'keys': keys, 'channels_by_kind': {'absent': kinds[0], 'constant': kinds[1], 'linear': kinds[2],
                                           'hermite': kinds[3]},
        'world_space_bones': [b['name'] for b in pack['bones'] if b['flags'] & 8],
        'model_bones_without_motion': [b['name'] for b in model['bones']
                                       if b['name'] not in {x['name'] for x in pack['bones']}],
        'frame_counts': {str(m['slot']): m['frames'] for m in pack['motions']},
        'pose_frames_checked': frames_checked, 'channel_values_identical': checked_values,
        'max_local_difference': worst_local, 'max_position_difference': worst_p,
        'max_normal_difference': worst_n, 'all_decoded_fields_match': True})
    if args.viewer:
        base = [str(args.viewer), '--romfs', str(args.romfs)]
        standalone = ['--archive', 'a/0/2/1', '--entry', '2', '--section', '0', '--skinned', '--triangle-list',
                      '--textured', '--combiners', '--texture-archive', 'a/0/2/1', '--texture-entry', '2',
                      '--texture-section', '0', '--motion-section', '1']
        room = ['--archive', 'a/0/4/1', '--entry', '165', '--section', '1', '--triangle-list', '--textured',
                '--combiners', '--texture-archive', 'a/0/1/3', '--texture-entry', '75', '--texture-section', '11',
                '--environment-archive', 'a/0/0/3', '--environment-entry', '137', '--environment-section', '0',
                '--walk', '--steps', 'DDDRRR', '--player-archive', 'a/0/2/1', '--player-entry', '2',
                '--player-section', '0', '--motion-section', '1', '--motion', '1', '--walk-motion', '3']
        env = {'SDL_VIDEODRIVER': 'dummy', 'PATH': '/usr/bin:/bin'}
        viewer = {'standalone': {}, 'walk': {}}

        def run(arguments, capture=None):
            if capture is None:
                out = subprocess.run(base + arguments + ['--inspect'], capture_output=True, text=True,
                                     check=True, timeout=120).stdout
                return [l for l in out.split('\n') if l.startswith(('motion_section=', 'walk_motion=', 'walk '))]
            path = args.output_dir / capture
            if path.exists():
                path.unlink()
            subprocess.run(base + arguments + ['--software', '--frames', '1', '--capture-frame', str(path)],
                           capture_output=True, text=True, check=True, timeout=300, env=env)
            return hashlib.sha256(path.read_bytes()).hexdigest()
        for slot, frame in ((1, 0), (3, 0), (3, 7), (4, 0), (4, 8)):
            arguments = standalone + ['--motion', str(slot), '--motion-frame', str(frame)]
            viewer['standalone'][f'{slot}:{frame}'] = {'inspect': run(arguments),
                                                       'capture_sha256': run(arguments, f'motion-{slot}-{frame}.bmp')}
        for frame in (0, 3, 7, 10, 14):
            arguments = room + ['--motion-frame', str(frame)]
            viewer['walk'][str(frame)] = {'inspect': run(arguments),
                                          'capture_sha256': run(arguments, f'walk-step-{frame}.bmp')}
        report['viewer'] = viewer
    (args.output_dir / 'reference-comparison.json').write_text(json.dumps(report, indent=1) + '\n')
    print(json.dumps({k: v for k, v in report.items() if k not in ('frame_counts', 'viewer')}, indent=1)[:4000])


if __name__ == '__main__':
    main()
