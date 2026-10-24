#!/usr/bin/env python3
"""Independently check the native six-stage combiner preview.

1. An independent Python TEV oracle (table driven, written from the pinned
   Azahar software-rasterizer behavior and CTRMap register layouts, sharing no
   code with render/tev.c) is compared with the native evaluator through the
   dump_combiners bridge on random valid and invalid configurations.
2. The field's twelve materials are re-derived from raw command streams and
   compared with the native interpretation, then evaluated on random inputs.
   The oracle also measures how often the projected-unit substitute and the
   fragment-secondary input can change each material's output.
3. Every mesh's vertex colors are decoded independently (stored attributes
   with their uniform scales, or float24 fixed attributes) and compared.
4. Optionally the viewer renders the field with --combiners and the report
   records the capture hash and its sensitivity lines.
Only metadata is written; no pixels, payload bytes or arrays.
"""
import argparse
import hashlib
import importlib.util
import json
import math
from pathlib import Path
import random
import struct
import subprocess
import sys

HERE = Path(__file__).resolve().parent
FIELD_SHA256 = 'cb131a9590850d604b30aa558b7c55fba6fff654b2f6dd05c823af5ecbc16ccd'
AZAHAR = {
    'repository': 'https://github.com/azahar-emu/azahar',
    'commit': '1b5fd00964115d9959cbc2ca395c89f0b590d03a',
    'src/video_core/renderer_software/sw_rasterizer.cpp':
        'efc36cddb32bc1f2ad79e0e3fb8b6d41ac66331538498cedaea2446f0f97ebdf',
    'src/video_core/renderer_software/sw_texturing.cpp':
        '89b14ef9d4b0db7a47c9a5c44d1cc22bff8b57245c2a00447cb4fe9a76e4a26b',
    'src/video_core/pica/regs_texturing.h':
        '5ee378220fe92d9e3ee1dc1ce0e6a212d8513460a5371d75e11cd7c707608b66',
    'src/video_core/pica/packed_attribute.h':
        '7550c38549022dd965125050d9fab2b94be2d5aae33b657c649c22a95bdf0edd',
    'src/video_core/pica/pica_core.cpp':
        '27d4d6616a9ca68bac5ad89d40c37b942a72eb42bd39d67c95c65d15b05fbfe7',
    'src/video_core/pica_types.h':
        'b741b44b803fde734bf12f3e1be58ca56966a9bd5eb92aceca93536928c75593',
}

spec = importlib.util.spec_from_file_location('textured', HERE / '../bch-textured/verify_materials.py')
textured = importlib.util.module_from_spec(spec)
spec.loader.exec_module(textured)
BCH, u32 = textured.BCH, textured.u32

# ---------------------------------------------------------------- TEV oracle
SOURCES = {0: 'primary', 1: 'fprimary', 2: 'fsecondary', 3: 't0', 4: 't1', 5: 't2', 6: 't3',
           13: 'buffer', 14: 'constant', 15: 'previous'}
RGB_OPERANDS = {0: ('rgb', False), 1: ('rgb', True), 2: ('a', False), 3: ('a', True),
                4: ('r', False), 5: ('r', True), 8: ('g', False), 9: ('g', True),
                12: ('b', False), 13: ('b', True)}
ALPHA_OPERANDS = {0: ('a', False), 1: ('a', True), 2: ('r', False), 3: ('r', True),
                  4: ('g', False), 5: ('g', True), 6: ('b', False), 7: ('b', True)}
CHANNEL = {'r': 0, 'g': 1, 'b': 2, 'a': 3}


def clamp(v):
    return max(0, min(255, v))


OPS = {
    0: lambda a, b, c: a,
    1: lambda a, b, c: a * b // 255,
    2: lambda a, b, c: min(255, a + b),
    3: lambda a, b, c: clamp(a + b - 128),
    4: lambda a, b, c: (a * c + b * (255 - c)) // 255,
    5: lambda a, b, c: max(0, a - b),
    8: lambda a, b, c: min(255, (a * b + 255 * c) // 255),
    9: lambda a, b, c: min(255, a + b) * c // 255,
}


def trunc_div(n, d):
    q = abs(n) // d
    return q if n >= 0 else -q


def nibbles(word, base):
    return [(word >> (base + 4 * i)) & 15 for i in range(3)]


def oracle_valid(stages, update):
    if update & ~0xff00:
        return False
    for source, operand, combine, _constant, scale in stages:
        rgb_op, alpha_op = combine & 0xffff, combine >> 16
        if source & 0xf000f000 or operand & 0xff000000 or scale & ~0x30003 or rgb_op > 9:
            return False
        if rgb_op != 7 and (alpha_op > 9 or alpha_op in (6, 7)):
            return False
        if any(s not in SOURCES for s in nibbles(source, 0) + nibbles(source, 16)):
            return False
        if any(o not in RGB_OPERANDS for o in nibbles(operand, 0)):
            return False
        if any(o not in ALPHA_OPERANDS for o in nibbles(operand, 12)):
            return False
    return True


def unpack(word):
    return [(word >> (8 * i)) & 255 for i in range(4)]


def oracle(stages, buffer_color, update, inputs):
    """inputs: dict of source name -> [r, g, b, a]."""
    previous, buffer, pending = [0, 0, 0, 0], [0, 0, 0, 0], unpack(buffer_color)
    for index, (source, operand, combine, constant, scale) in enumerate(stages):
        values = dict(inputs, constant=unpack(constant), buffer=buffer, previous=previous)
        rgb_sources = nibbles(source, 0)
        if index == 0:
            rgb_sources = [rgb_sources[2] if s == 15 and i < 2 else s for i, s in enumerate(rgb_sources)]
        rgb_args = []
        for s, code in zip(rgb_sources, nibbles(operand, 0)):
            selector, invert = RGB_OPERANDS[code]
            v = values[SOURCES[s]]
            triple = v[:3] if selector == 'rgb' else [v[CHANNEL[selector]]] * 3
            rgb_args.append([255 - x if invert else x for x in triple])
        alpha_args = []
        for s, code in zip(nibbles(source, 16), nibbles(operand, 12)):
            selector, invert = ALPHA_OPERANDS[code]
            x = values[SOURCES[s]][CHANNEL[selector]]
            alpha_args.append(255 - x if invert else x)
        rgb_op, alpha_op = combine & 0xffff, combine >> 16
        if rgb_op in (6, 7):
            total = sum(trunc_div((2 * a - 255) * (2 * b - 255) + 128, 256)
                        for a, b in zip(rgb_args[0], rgb_args[1]))
            rgb = [clamp(total)] * 3
        else:
            rgb = [OPS[rgb_op](rgb_args[0][c], rgb_args[1][c], rgb_args[2][c]) for c in range(3)]
        alpha = rgb[0] if rgb_op == 7 else OPS[alpha_op](*alpha_args)
        factor = lambda bits: 1 << bits if bits < 3 else 1
        result = [min(255, x * factor(scale & 3)) for x in rgb] + [min(255, alpha * factor((scale >> 16) & 3))]
        previous = result
        buffer = list(pending)
        if index < 4 and update >> (8 + index) & 1:
            pending[:3] = result[:3]
        if index < 4 and update >> (12 + index) & 1:
            pending[3] = result[3]
    return previous


def random_stage(rng, valid=True):
    def pick(table):
        return rng.choice(list(table))
    source = sum(pick(SOURCES) << (4 * i) for i in range(3)) | sum(pick(SOURCES) << (16 + 4 * i) for i in range(3))
    operand = sum(pick(RGB_OPERANDS) << (4 * i) for i in range(3)) | \
        sum(pick(ALPHA_OPERANDS) << (12 + 4 * i) for i in range(3))
    rgb_op = rng.choice([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
    alpha_op = rng.choice([0, 1, 2, 3, 4, 5, 8, 9])
    scale = rng.randrange(4) | rng.randrange(4) << 16
    stage = [source, operand, rgb_op | alpha_op << 16, rng.getrandbits(32), scale]
    if not valid:
        field = rng.randrange(5)
        if field == 0:
            stage[0] = (stage[0] & ~0xf) | rng.choice([7, 8, 9, 10, 11, 12])
        elif field == 1:
            stage[1] = (stage[1] & ~0xf) | rng.choice([6, 7, 10, 11, 14, 15])
        elif field == 2:
            stage[2] = rng.choice([10, 11, 0xffff, 0x000a0000 | 1, 0x00060001])
        elif field == 3:
            stage[4] = 4
        else:
            stage[0] |= 0x1000
    return stage


def random_inputs(rng):
    inputs = {name: [rng.randrange(256) for _ in range(4)]
              for name in ('primary', 'fprimary', 'fsecondary', 't0', 't1', 't2')}
    inputs['t3'] = [0, 0, 0, 0]  # the bridge leaves procedural texture 3 at zero
    return inputs


def bridge_line(stages, buffer_color, update, inputs):
    words = [w for stage in stages for w in stage] + [buffer_color, update]
    bytes_ = [x for name in ('primary', 'fprimary', 'fsecondary', 't0', 't1', 't2') for x in inputs[name]]
    return ' '.join(f'{w:x}' for w in words) + ' ' + ' '.join(f'{b:x}' for b in bytes_)


def run_bridge(bridge, lines):
    out = subprocess.run([str(bridge), '--evaluate'], input='\n'.join(lines) + '\n',
                         capture_output=True, text=True, check=True).stdout.split('\n')
    return [list(map(int, line.split())) for line in out if line]


# ------------------------------------------------------- field interpretation
def command_writes(bch, field):
    """Ordered (register, value, byte-mask bits, source offset) writes."""
    at, words = bch.ptr(field, 'commands', (2,)), u32(bch.data, field + 4)
    end, writes = at + 4 * words, []
    while at < end:
        first, header = struct.unpack_from('<II', bch.data, at)
        register, mask = header & 0xffff, (header >> 16) & 15
        extra, consecutive = (header >> 20) & 0x7ff, header >> 31
        bits = sum(0xff << (8 * i) for i in range(4) if mask >> i & 1)
        for i in range(extra + 1):
            source = at if i == 0 else at + 4 + 4 * i
            value = first if i == 0 else u32(bch.data, source)
            writes.append((register + (i if consecutive else 0), value, bits, source))
        count = 2 + extra
        at += 4 * (count + (count & 1))
    return writes


def state(writes):
    values, known = {}, {}
    for register, value, bits, _source in writes:
        values[register] = (values.get(register, 0) & ~bits) | (value & bits)
        known[register] = known.get(register, 0) | bits
    return values, known


def field_materials(data):
    bch = BCH(data)
    model = bch.ptr(bch.ptr(bch.base['contents'], 'contents'), 'contents')
    first = bch.ptr(model + 0x34, 'contents')
    out = []
    for i in range(u32(data, model + 0x38)):
        record = first + i * 0x58
        params = bch.ptr(record, 'contents')
        tex_values, _ = state(command_writes(bch, record + 0x10))
        values, known = state(command_writes(bch, params + 0xc8))
        stages = []
        for base in (0xc0, 0xc8, 0xd0, 0xd8, 0xf0, 0xf8):
            assert all(known.get(base + j) == 0xffffffff for j in range(5)), 'unknown stage register'
            stages.append([values[base + j] for j in range(5)])
        assert known.get(0xfd) == 0xffffffff and known.get(0xe0, 0) & 0xff00 == 0xff00
        names = [bch.string(record + 0x48 + 4 * u) for u in range(3)]
        mapping = [(u32(data, params + 12 + 24 * u) >> 16) & 0xff for u in range(3)]
        out.append({'stages': stages, 'buffer_color': values[0xfd], 'buffer_update': values[0xe0] & 0xff00,
                    'unit_config': tex_values[0x80], 'names': names, 'mapping': mapping})
    return bch, model, out


def classify(material):
    sources = set()
    for index, (source, _o, combine, _c, _s) in enumerate(material['stages']):
        rgb = nibbles(source, 0)
        if index == 0:
            rgb = [rgb[2] if s == 15 and i < 2 else s for i, s in enumerate(rgb)]
        sources.update(rgb)
        if combine & 0xffff != 7:
            sources.update(nibbles(source, 16))
    sampled = projected = disabled = 0
    for unit in range(3):
        if 3 + unit not in sources:
            continue
        if not material['unit_config'] >> unit & 1:
            disabled |= 1 << unit
        elif material['mapping'][unit] == 3:
            projected |= 1 << unit
        else:
            assert material['mapping'][unit] == 0 and material['names'][unit] is not None
            sampled |= 1 << unit
    return sum(1 << s for s in sources), sampled, projected, disabled


# ------------------------------------------------------------ vertex colors
def float24(bits):
    exponent, mantissa = (bits >> 16) & 127, bits & 0xffff
    assert exponent != 127
    magnitude = (1 + mantissa / 65536) * 2.0 ** (exponent - 63) if exponent else mantissa / 65536 * 2.0 ** -62
    return -magnitude if bits & 0x800000 else magnitude


def f32(x):
    return struct.unpack('<f', struct.pack('<f', x))[0]


def mesh_colors(bch, model):
    data = bch.data
    count = u32(data, model + 0x44)
    meshes_at = bch.ptr(model + 0x40, 'contents')
    out = []
    first_vertex = 0
    for m in range(count):
        at = meshes_at + 56 * m
        writes = command_writes(bch, at + 8)
        values, _known = state(writes)
        uniforms, fixed, index, pending, fixed_index, queue = {}, {}, None, [], 0, []
        for register, value, _bits, _source in writes:
            if register == 0x2c0:
                index, pending = value & 0xff, []
            elif 0x2c1 <= register <= 0x2c8:
                pending.append(struct.unpack('<f', struct.pack('<I', value))[0])
                if len(pending) == 4:
                    uniforms[index] = list(reversed(pending))   # streamed w, z, y, x
                    index, pending = index + 1, []
            elif register == 0x232:
                fixed_index, queue = value & 15, []
            elif 0x233 <= register <= 0x235:
                queue.append(value)
                if len(queue) == 3:
                    w0, w1, w2 = queue
                    fixed[fixed_index] = [float24(w2 & 0xffffff),
                                          float24((w1 & 0xffff) << 8 | w2 >> 24),
                                          float24((w0 & 0xff) << 16 | w1 >> 16),
                                          float24(w0 >> 8)]
                    fixed_index, queue = fixed_index + 1, []
        formats = values[0x201] | values[0x202] << 32
        permutation = values[0x2bb] | values.get(0x2bc, 0) << 32
        total = (values[0x242] & 0xff) + 1
        fixed_mask = formats >> 48 & 0xfff
        config = values[0x205]
        order = values[0x204] | (config & 0xffff) << 32
        stride, stored = config >> 16 & 0xff, config >> 28
        source_field = [s for r, _v, _b, s in writes if r == 0x203][-1]
        base = bch.ptr(source_field, 'raw')
        # Index-derived vertex count, as both decoders use.
        sub = bch.ptr(at + 16, 'contents')
        largest = 0
        for s in range(u32(data, at + 20)):
            face = command_writes(bch, sub + 52 * s + 44)
            fvalues, _ = state(face)
            source = [src for r, _v, _b, src in face if r == 0x227][-1]
            tag = bch.reloc[source][0]
            width = 2 if tag == 0x27 else 1
            ibase = bch.ptr(source, 'raw')
            for k in range(fvalues[0x228]):
                idx = struct.unpack_from('<H', data, ibase + 2 * k)[0] if width == 2 else data[ibase + k]
                largest = max(largest, idx)
        vertex_count = largest + 1
        color, source_kind = None, 0
        offset = 0
        for j in range(stored):
            slot = order >> (4 * j) & 15
            semantic = permutation >> (4 * slot) & 15
            fmt = formats >> (4 * slot) & 15
            kind, elements = fmt & 3, (fmt >> 2) + 1
            width = (1, 1, 2, 4)[kind]
            if semantic == 3:
                scale = uniforms[7][3]
                color, source_kind = [], 1
                for v in range(vertex_count):
                    rgba = [1.0, 1.0, 1.0, 1.0]
                    for c in range(elements):
                        p = base + v * stride + offset + c * width
                        raw = struct.unpack_from('<' + 'bBhf'[kind], data, p)[0]
                        rgba[c] = f32(raw * scale) if kind != 3 else f32(raw * scale)
                    color.append(rgba)
            offset += width * elements
        for slot in range(total):
            if fixed_mask >> slot & 1 and permutation >> (4 * slot) & 15 == 3:
                color, source_kind = [[f32(x) for x in fixed[slot]]] * vertex_count, 2
        if color is None:
            color = [[1.0] * 4] * vertex_count
        out.append({'color_source': source_kind, 'colors': color})
        first_vertex += vertex_count
    return out


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('--field', required=True, type=Path)
    parser.add_argument('--bridge', required=True, type=Path)
    parser.add_argument('--viewer', type=Path)
    parser.add_argument('--romfs', type=Path)
    parser.add_argument('--model-args', default='--archive a/0/4/1 --entry 165 --section 1')
    parser.add_argument('--texture-args', default='--texture-archive a/0/1/3 --texture-entry 75 --texture-section 11')
    parser.add_argument('--azahar', type=Path, help='optional Azahar checkout to hash-check')
    parser.add_argument('--random-configs', type=int, default=20000)
    parser.add_argument('--output-dir', required=True, type=Path)
    args = parser.parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=False)
    data = args.field.read_bytes()
    report = {'field_bch_sha256': hashlib.sha256(data).hexdigest(),
              'verifier_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
              'bridge_source_sha256': hashlib.sha256((HERE / 'dump_combiners.c').read_bytes()).hexdigest(),
              'behavior_reference': dict(AZAHAR)}
    assert report['field_bch_sha256'] == FIELD_SHA256
    if args.azahar:
        for path, digest in AZAHAR.items():
            if path.startswith('src/'):
                assert hashlib.sha256((args.azahar / path).read_bytes()).hexdigest() == digest, path
        report['behavior_reference']['hashes_checked'] = True

    rng = random.Random(0x5eed)
    # 1. Random valid and invalid configurations, several inputs each.
    lines, expected = [], []
    for n in range(args.random_configs):
        valid = n % 5 != 4
        stages = [random_stage(rng) for _ in range(6)]
        if not valid:
            stages[rng.randrange(6)] = random_stage(rng, valid=False)
        update = rng.randrange(256) << 8 if rng.random() < .9 else rng.getrandbits(16)
        buffer_color = rng.getrandbits(32)
        for _ in range(4):
            inputs = random_inputs(rng)
            lines.append(bridge_line(stages, buffer_color, update, inputs))
            ok = oracle_valid(stages, update)
            expected.append([int(ok)] + (oracle(stages, buffer_color, update, inputs) if ok else [0] * 4))
    got = run_bridge(args.bridge, lines)
    mismatches = sum(a != b for a, b in zip(got, expected))
    assert len(got) == len(expected) and mismatches == 0, f'{mismatches} oracle mismatches'
    report['random'] = {'configurations': args.random_configs, 'evaluations': len(lines),
                        'valid_evaluations': sum(e[0] for e in expected), 'mismatches': 0, 'seed': '0x5eed'}

    # 2. Field materials.
    native = json.loads(subprocess.run([str(args.bridge), str(args.field)], check=True,
                                       capture_output=True, text=True).stdout)
    bch, model, materials = field_materials(data)
    assert len(materials) == len(native['materials']) == 12
    field_results, lines, expected = [], [], []
    for i, (ref, nat) in enumerate(zip(materials, native['materials'])):
        assert nat['result'] == 0, f'material {i} rejected natively'
        assert nat['stages'] == ref['stages'] and nat['buffer_color'] == ref['buffer_color']
        assert nat['buffer_update'] == ref['buffer_update']
        assert oracle_valid(ref['stages'], ref['buffer_update'])
        sources, sampled, projected, disabled = classify(ref)
        assert (nat['sources'], nat['sampled_units'], nat['projected_units'], nat['disabled_units']) == \
            (sources, sampled, projected, disabled), f'material {i} units'
        projected_changes = secondary_changes = trials = 0
        for _ in range(3000):
            inputs = random_inputs(rng)
            inputs['fprimary'] = [255] * 4           # declared unlit policy
            inputs['fsecondary'] = [0] * 4
            for unit in range(3):
                if disabled >> unit & 1:
                    inputs[f't{unit}'] = [0] * 4
                if projected >> unit & 1:
                    inputs[f't{unit}'] = [255] * 4
            out = oracle(ref['stages'], ref['buffer_color'], ref['buffer_update'], inputs)
            lines.append(bridge_line(ref['stages'], ref['buffer_color'], ref['buffer_update'], inputs))
            expected.append([1] + out)
            trials += 1
            if projected:
                alt = dict(inputs, **{f't{u}': [0] * 4 for u in range(3) if projected >> u & 1})
                projected_changes += oracle(ref['stages'], ref['buffer_color'], ref['buffer_update'], alt) != out
            alt = dict(inputs, fsecondary=[255] * 4)
            secondary_changes += oracle(ref['stages'], ref['buffer_color'], ref['buffer_update'], alt) != out
        field_results.append({'index': i, 'sources': f'0x{sources:04x}', 'sampled_units': sampled,
                              'projected_units': projected, 'disabled_units': disabled,
                              'buffer_update': f'0x{ref["buffer_update"]:04x}',
                              'buffer_color': f'0x{ref["buffer_color"]:08x}',
                              'random_trials': trials,
                              'projected_substitute_changes_output': projected_changes,
                              'fragment_secondary_changes_output': secondary_changes})
    got = run_bridge(args.bridge, lines)
    assert got == expected, 'field configuration oracle mismatch'
    report['field_materials'] = field_results
    report['field_evaluations'] = len(lines)

    # 3. Vertex colors.
    colors = mesh_colors(bch, model)
    compared = 0
    for i, (ref, nat) in enumerate(zip(colors, native['meshes'])):
        assert ref['color_source'] == nat['color_source'], f'mesh {i} color source'
        assert len(ref['colors']) == len(nat['colors'])
        for a, b in zip(ref['colors'], nat['colors']):
            assert all(struct.pack('<f', x) == struct.pack('<f', y) for x, y in zip(a, b)), f'mesh {i} color'
            compared += 1
    report['vertex_colors'] = {'meshes': len(colors), 'vertices': compared,
                               'sources': [c['color_source'] for c in colors], 'mismatches': 0}

    # 4. Runtime capture.
    if args.viewer and args.romfs:
        capture = args.output_dir / 'field-combiners.bmp'
        command = [str(args.viewer), '--romfs', str(args.romfs), *args.model_args.split(),
                   '--triangle-list', '--textured', '--combiners', *args.texture_args.split(),
                   '--software', '--frames', '3', '--self-test-input', '--capture-frame', str(capture)]
        run = subprocess.run(command, capture_output=True, text=True, timeout=120,
                             env={'SDL_VIDEODRIVER': 'dummy', 'PATH': '/usr/bin:/bin'})
        assert run.returncode == 0, run.stderr
        assert 'Input self-test passed' in run.stderr
        lines = [l for l in run.stdout.splitlines() if l.startswith('combiner_sensitivity')]
        assert len(lines) == 12
        report['runtime'] = {
            'backend': 'SDL dummy, software renderer', 'frames': 3, 'input_self_test': 'passed',
            'model_arguments': args.model_args, 'texture_arguments': args.texture_args,
            'capture_sha256': hashlib.sha256(capture.read_bytes()).hexdigest(),
            'sensitivity': lines}
    report['result'] = 'passed'
    (args.output_dir / 'reference-comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    json.dump({k: v for k, v in report.items() if k not in ('field_materials',)}, sys.stdout, indent=2)
    print()


if __name__ == '__main__':
    main()
