"""Independent check of the BCH environment lights and directional fragment lighting.

1. Parses the environment BCH (lights, scene light sets) and the field's
   material lighting parameters and normals with code written here from the
   pinned SPICA/Ohana3DS layouts, and compares every decoded value with the
   native decoder through the dump_lighting bridge.
2. Checks the code.bin words the light-register conventions rest on.
3. Compares px_light_register_channel exhaustively, and px_light_primary on
   random and field-derived inputs, with a float32 oracle written here from
   the pinned Azahar ComputeFragmentsColors.
4. Bounds two steps the preview does not execute: float16 truncation of the
   view-space light vector (random view rotations, all field normals) and the
   unexecuted point/spot lights (reported by the viewer).
5. Optionally runs the viewer on the real archive paths (day and night).

Run with python3 -I. The output directory may receive local asset data.
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

HERE = Path(__file__).resolve().parent
FIELD_SHA256 = 'cb131a9590850d604b30aa558b7c55fba6fff654b2f6dd05c823af5ecbc16ccd'
# a/0/0/3 entry 137, BM section 0 (uncompressed member and section).
ENVIRONMENT_SHA256 = 'fdd7377e833eb3a61db353e800ffb72c79594f5f71118f5beb7915d235f7d303'
CODE_SHA256 = '0f6d3e1d66cbcd55d46fa453db4b6aa489e03c3537c6b24026f2a53af142797a'
CODE_BASE = 0x100000
REFERENCES = {
    'spica': ('https://github.com/gdkchan/SPICA', 'bd29a7828595d7839cda2ac61c76bb63f9071250', {
        'SPICA/Formats/CtrH3D/Light/H3DLight.cs':
            '671b41659edaf0e5338952028661610ed151319cbbd7845b274ae3cf4a561d5b',
        'SPICA/Formats/CtrH3D/Light/H3DFragmentLight.cs':
            '469128cd7dcd00996977ba398e10704023460b3db0663a35631709fd3299feb7',
        'SPICA/Formats/CtrH3D/Light/H3DLightFlags.cs':
            '97d49f52e8b2031720b8d80fb7976964ac5ce898fa17637a66b9cc2df0968bf2',
        'SPICA/Formats/CtrH3D/Light/H3DLightType.cs':
            '56860f1fdea63ea0b4d84c7cddfcb1144a2656d89c7d84a14113a26ba654aebe',
        'SPICA/Formats/CtrH3D/Model/Material/H3DMaterialParams.cs':
            '1952ab4144a57702a793124ebdf29e2b4fd38f68bacdfe70083251ad997a5b04',
        'SPICA/Formats/CtrH3D/Model/Material/H3DMaterialFlags.cs':
            'b83d2d981c22343bbd0a7a352afebcfe1f8522a0d94a3c353580df9b65425848',
        'SPICA.Rendering/Light.cs':
            'e3a92317e1dd07d11cd535a2c5a98916a0716b739334a55e53a77bd15b672135'}),
    'ohana': ('https://github.com/gdkchan/Ohana3DS-Rebirth', 'c50635e82763fad9d1bcc30a905523a1b92d7d1f', {
        'Ohana3DS Rebirth/Ohana/Models/BCH.cs':
            '92b40467a01c9d1fde9d0809760d962378906da0ee9bbb166b39c1b8c3c3ae66'}),
    'azahar': ('https://github.com/azahar-emu/azahar', '1b5fd00964115d9959cbc2ca395c89f0b590d03a', {
        'src/video_core/renderer_software/sw_lighting.cpp':
            'afaad68f3e6ed5d1524f45621cd634c64267085ebc340c6cb112cc76b66fdbb3',
        'src/video_core/pica/regs_lighting.h':
            'fa7f2e05af15fd74f313e6223ef742cc112fd702f6762353bbe4fa9bdcd4e8ce'}),
}
# (address, word, meaning). Light upload 0x2df3f0(r1 = light slot, r2 = light
# object); register writer 0x2e2dc0; per-light color loop at 0x3030a8.
CODE_WORDS = [
    (0x2df41c, 0xe59400b8, 'ldr r0,[r4,#0xb8]: runtime light kind'),
    (0x2df430, 0x0a000057, 'beq 0x2df594: kind 0 path'),
    (0x2df420, 0xed9f8ab3, 'vldr s16 <- literal 0x2df6f4'),
    (0x2df6f4, 0x00000000, 's16 = 0.0'),
    (0x2df438, 0xeddf8aaf, 'vldr s17 <- literal 0x2df6fc'),
    (0x2df6fc, 0x3f800000, 's17 = 1.0'),
    (0x2df594, 0xe596017c, 'ldr r0,[r6,#0x17c]: direction x'),
    (0x2df5a4, 0xe5960180, 'ldr r0,[r6,#0x180]: direction y'),
    (0x2df5b0, 0xe5960184, 'ldr r0,[r6,#0x184]: direction z'),
    (0x2df5b8, 0xed8d8a03, 'vstr s16,[sp,#0xc]: w = 0'),
    (0x2df5c0, 0xeb0002a2, 'bl 0x2e0050: transform by view matrix'),
    (0x2df5c4, 0xec9d0a04, 'vldmia sp,{s0-s3}'),
    (0x2df5d4, 0xeeb10a40, 'vneg.f32 s0,s0'),
    (0x2df5d8, 0xeef10a60, 'vneg.f32 s1,s1'),
    (0x2df5dc, 0xeef11a61, 'vneg.f32 s3,s3'),
    (0x2df5e0, 0xeeb11a41, 'vneg.f32 s2,s2'),
    (0x2df5e4, 0xec820a04, 'vstmia r2,{s0-s3}'),
    (0x2df5e8, 0xeb000df4, 'bl 0x2e2dc0: write light position'),
    (0x2df5f4, 0xeeb40a48, 'vcmp.f32 s0,s16: transformed w == 0'),
    (0x2df5fc, 0x01800005, 'orreq r0,r0,r5: set directional bit'),
    (0x2e2dc0, 0xe92d00f0, 'push {r4-r7}: register writer entry'),
    (0x2e2dcc, 0xe59f4110, 'ldr r4 <- literal 0x2e2ee4'),
    (0x2e2ee4, 0x801f0144, 'header: registers 0x144-0x145, light 0'),
    (0x2e2e6c, 0xe0843200, 'add r3,r4,r0,lsl #4: + 16 * light slot'),
    (0x2e2ed4, 0xe1c120f0, 'strd r2,r3: f16 x|y<<16, header'),
    (0x3030b4, 0xe59f31f8, 'ldr r3 <- literal 0x3032b4'),
    (0x3032b4, 0x803f0140, 'header: registers 0x140-0x143, light 0'),
    (0x303264, 0xe0833204, 'add r3,r3,r4,lsl #4: + 16 * light slot'),
    (0x3030c8, 0xe8921c00, 'ldm material+0xb4: ambient, diffuse, specular0'),
    (0x3030cc, 0xe3a01001, 'mov r1,#1'),
    (0x3030e0, 0xe081508e, 'add r5,r1,lr,lsl #1: 2*m+1'),
    (0x303100, 0xe0817087, 'add r7,r1,r7,lsl #1: 2*l+1'),
    (0x303110, 0xe1650587, 'smulbb r5,r7,r5'),
    (0x303128, 0xe1a05525, 'lsr r5,r5,#10'),
    (0x3030f0, 0xe592e104, 'ldr lr,[light,#0x104]: specular0'),
    (0x30313c, 0xe5928100, 'ldr r8,[light,#0x100]: diffuse'),
    (0x303160, 0xe592b108, 'ldr fp,[light,#0x108]: specular1'),
    (0x303164, 0xe59220fc, 'ldr r2,[light,#0xfc]: ambient'),
    (0x303274, 0xe8814009, 'stm {spec0, header, spec1}'),
    (0x303278, 0xe8871004, 'stm {diffuse, ambient}'),
    (0x302fd0, 0xe59140c8, 'ldr r4,[ambient light,#0xc8]'),
    (0x302fd4, 0xe59210b4, 'ldr r1,[material,#0xb4]: ambient'),
    (0x302fd8, 0xe59220b0, 'ldr r2,[material,#0xb0]: emission'),
    (0x30306c, 0xe59210b0, 'no ambient light: emission only'),
]

spec = importlib.util.spec_from_file_location('textured', HERE / '../bch-textured/verify_materials.py')
textured = importlib.util.module_from_spec(spec)
spec.loader.exec_module(textured)
BCH, u32 = textured.BCH, textured.u32
spec = importlib.util.spec_from_file_location('combiners', HERE / '../bch-combiners/verify_combiners.py')
combiners = importlib.util.module_from_spec(spec)
spec.loader.exec_module(combiners)


def f32(x):
    return struct.unpack('<f', struct.pack('<f', x))[0]


def bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def from_bits(b):
    return struct.unpack('<f', struct.pack('<I', b))[0]


def floats(data, at, n):
    return list(struct.unpack_from('<%df' % n, data, at))


# --------------------------------------------------------- environment parse
def parse_environment(data):
    """Lights (category 5) and scene 0 light sets (category 14)."""
    bch = BCH(data)
    root = bch.base['contents']
    lights = []
    count, table = u32(data, root + 5 * 12 + 4), bch.ptr(root + 5 * 12, 'contents')
    for i in range(count):
        at = bch.ptr(table + 4 * i, 'contents')
        light = {'name': bch.string(at).decode('latin-1'),
                 'scale': floats(data, at + 4, 3), 'rotation': floats(data, at + 16, 3),
                 'translation': floats(data, at + 28, 3),
                 'type': data[at + 40], 'enabled': data[at + 41], 'flags': data[at + 42],
                 'lut': data[at + 43],
                 'ambient': list(data[at + 48:at + 52]), 'diffuse': list(data[at + 52:at + 56]),
                 'specular0': list(data[at + 56:at + 60]), 'specular1': list(data[at + 60:at + 64]),
                 'direction': floats(data, at + 64, 3),
                 'samplers': [int(u32(data, at + 76) != 0 or (at + 76) in bch.reloc),
                              int(u32(data, at + 80) != 0 or (at + 80) in bch.reloc)],
                 'attenuation': floats(data, at + 84, 2)}
        lights.append(light)
    scene = bch.ptr(bch.ptr(root + 14 * 12, 'contents'), 'contents')
    out = {'scene': bch.string(scene).decode('latin-1'),
           'cameras': u32(data, scene + 8), 'fogs': u32(data, scene + 0x18), 'lights': lights}
    names = [l['name'] for l in lights]
    first, n = bch.ptr(scene + 0xc, 'contents'), u32(data, scene + 0x10)
    # Ohana3DS c50635e reads 8-byte (slot, string pointer) references. Here
    # the second word must then be a string relocation; record whether it is.
    out['ohana_8_byte_reading_valid'] = all(
        bch.reloc.get(first + 8 * k + 4, (None, None))[1] == 'strings' for k in range(n))
    sets = []
    for k in range(n):
        at = first + 12 * k
        array, members = bch.ptr(at + 4, 'contents', (0,)), u32(data, at + 8)
        member_names = [bch.string(array + 4 * j).decode('latin-1') for j in range(members)]
        sets.append({'index': u32(data, at), 'lights': [names.index(x) for x in member_names],
                     'names': member_names})
    out['sets'] = sets
    return out


def parse_materials(data):
    bch = BCH(data)
    model = bch.ptr(bch.ptr(bch.base['contents'], 'contents'), 'contents')
    first = bch.ptr(model + 0x34, 'contents')
    out = []
    for i in range(u32(data, model + 0x38)):
        p = bch.ptr(first + 0x58 * i, 'contents')
        out.append({'flags': struct.unpack_from('<H', data, p + 4)[0], 'fragment_flags': data[p + 6],
                    'light_set': struct.unpack_from('<H', data, p + 84)[0],
                    'fog': struct.unpack_from('<H', data, p + 86)[0],
                    'colors': [list(data[p + 88 + 4 * c:p + 92 + 4 * c]) for c in range(5)],
                    'color_scale': bits(floats(data, p + 136, 1)[0]),
                    'lut_samplers': sum(1 << j for j in range(6) if u32(data, p + 140 + 4 * j)),
                    'fresnel': data[p + 165], 'bump': data[p + 166]})
    return bch, model, out


def mesh_normals(bch, model):
    """Normals per mesh: stored semantic 1 (uniform 7 y scale) or fixed."""
    data = bch.data
    out = []
    meshes_at = bch.ptr(model + 0x40, 'contents')
    for m in range(u32(data, model + 0x44)):
        at = meshes_at + 56 * m
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
                    fixed[slot] = [combiners.float24(w2 & 0xffffff),
                                   combiners.float24((w1 & 0xffff) << 8 | w2 >> 24),
                                   combiners.float24((w0 & 0xff) << 16 | w1 >> 16)]
                    slot, queue = slot + 1, []
        formats = values[0x201] | values[0x202] << 32
        permutation = values[0x2bb] | values.get(0x2bc, 0) << 32
        total = (values[0x242] & 0xff) + 1
        config = values[0x205]
        order = values[0x204] | (config & 0xffff) << 32
        stride, stored = config >> 16 & 0xff, config >> 28
        base = bch.ptr([s for r, _v, _b, s in writes if r == 0x203][-1], 'raw')
        sub = bch.ptr(at + 16, 'contents')
        largest = 0
        for s in range(u32(data, at + 20)):
            face = combiners.command_writes(bch, sub + 52 * s + 44)
            fvalues, _ = combiners.state(face)
            source = [src for r, _v, _b, src in face if r == 0x227][-1]
            wide = bch.reloc[source][0] == 0x27
            ibase = bch.ptr(source, 'raw')
            for k in range(fvalues[0x228]):
                largest = max(largest, struct.unpack_from('<H', data, ibase + 2 * k)[0] if wide else data[ibase + k])
        count = largest + 1
        normals, source_kind, offset = [[0.0, 0.0, 0.0]] * count, 0, 0
        for j in range(stored):
            slot_j = order >> (4 * j) & 15
            semantic = permutation >> (4 * slot_j) & 15
            fmt = formats >> (4 * slot_j) & 15
            kind, elements = fmt & 3, (fmt >> 2) + 1
            width = (1, 1, 2, 4)[kind]
            if semantic == 1:
                assert elements == 3
                scale = uniforms[7][1]
                normals = [[f32(struct.unpack_from('<' + 'bBhf'[kind], data,
                                                   base + v * stride + offset + c * width)[0] * scale)
                            for c in range(3)] for v in range(count)]
                source_kind = 1
            offset += width * elements
        for s in range(total):
            if formats >> 48 >> s & 1 and permutation >> (4 * s) & 15 == 1:
                normals, source_kind = [[f32(x) for x in fixed[s]]] * count, 2
        out.append({'normal_source': source_kind, 'normals': normals})
    return out


# ------------------------------------------------------------ lighting oracle
def register_product(light, material):
    """Written from the 0x3030a8 loop: ((2*l + 1) * (2*m + 1)) >> 10."""
    return (2 * light + 1) * (2 * material + 1) >> 10


def normalize(v):
    length = f32(math.sqrt(f32(f32(f32(v[0] * v[0]) + f32(v[1] * v[1])) + f32(v[2] * v[2]))))
    if not length > 0 or not math.isfinite(length):
        return None
    return [f32(x / length) for x in v]


def oracle(setup, normal):
    """Azahar ComputeFragmentsColors for directional lights, no LUTs,
    attenuation, shadow, bump or Fresnel, in float32 steps."""
    n = normalize(normal)
    if n is None or len(setup['lights']) > 8:
        return None
    total = [0.0, 0.0, 0.0]
    for light in setup['lights']:
        v = normalize(light['vector'])
        if v is None:
            return None
        dot = f32(f32(f32(v[0] * n[0]) + f32(v[1] * n[1])) + f32(v[2] * n[2]))
        dot = abs(dot) if light['two_sided'] else max(dot, 0.0)
        for c in range(3):
            diffuse = f32(f32(f32(light['diffuse'][c] / 255.0) * dot) + f32(light['ambient'][c] / 255.0))
            total[c] = f32(total[c] + diffuse)
    out = []
    for c in range(3):
        x = f32(total[c] + f32(setup['global'][c] / 255.0))
        out.append(int(f32(min(max(x, 0.0), 1.0) * 255.0)))
    return out + [255]


def bridge_line(setup, normal):
    parts = [str(len(setup['lights'])), *map(str, setup['global'])]
    for light in setup['lights']:
        parts += [*map(str, light['ambient']), *map(str, light['diffuse']),
                  *('%08x' % bits(x) for x in light['vector']), str(int(light['two_sided']))]
    parts += ['%08x' % bits(x) for x in normal]
    return ' '.join(parts)


def run(bridge, *arguments, stdin=None):
    return subprocess.run([str(bridge), *arguments], input=stdin, text=True, capture_output=True,
                          check=True, timeout=600).stdout


def material_setups(environment, materials, lighting):
    """The documented viewer policy, re-derived from the parsed data."""
    setups = []
    for m in materials:
        chosen = [s for s in environment['sets'] if s['index'] == m['light_set']]
        assert len(chosen) == 1 and m['flags'] & 1 and not m['fresnel'] and not m['bump']
        setup = {'global': m['colors'][0][:3], 'lights': [], 'names': [], 'point_bound': [0, 0, 0]}
        for index in chosen[0]['lights']:
            light = environment['lights'][index]
            if not light['enabled']:
                continue
            if light['type'] in (10, 11):
                for c in range(3):
                    setup['point_bound'][c] += (register_product(light['ambient'][c], m['colors'][1][c])
                                                + register_product(light['diffuse'][c], m['colors'][2][c]))
                continue
            assert light['type'] == 9 and not light['flags'] & 2
            if (lighting, light['name']) in (('day', 'light_night'), ('night', 'light_sun')):
                continue
            setup['names'].append(light['name'])
            setup['lights'].append({
                'ambient': [register_product(light['ambient'][c], m['colors'][1][c]) for c in range(3)],
                'diffuse': [register_product(light['diffuse'][c], m['colors'][2][c]) for c in range(3)],
                'vector': [f32(-x) + 0.0 for x in light['direction']],
                'two_sided': light['flags'] & 1})
        setups.append(setup)
    return setups


# ------------------------------------------------------------- float16 bound
def game_f16(x):
    """float32 -> float16 as code.bin 0x2e2dc0 encodes it (truncating; tiny
    values flush to signed zero; overflow saturates to 0x7c00)."""
    b = bits(x)
    sign = 0x8000 if b & 0x80000000 else 0
    mantissa = (b & 0x7fffff) >> 13
    if not b & 0x7fffffff:
        return sign
    exponent = (b >> 23 & 0xff) - 0x70
    if exponent < 0:
        return sign
    if exponent > 0x1f:
        return 0x7c00
    return sign | exponent << 10 | mantissa


def half(h):
    """IEEE binary16 decode, as Azahar f16::FromRaw (denormals exact)."""
    return struct.unpack('<e', struct.pack('<H', h))[0]


def random_rotation(rng):
    q = [rng.gauss(0, 1) for _ in range(4)]
    s = math.sqrt(sum(x * x for x in q))
    w, x, y, z = (v / s for v in q)
    return [[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
            [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
            [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]]


def rotate(r, v):
    return [f32(sum(r[i][j] * v[j] for j in range(3))) for i in range(3)]


def f16_bound(setups, materials, normals_by_mesh, mesh_materials, rng, rotations):
    worst, compared = 0, 0
    unique = {}
    for mesh, normals in enumerate(normals_by_mesh):
        for n in normals:
            unique.setdefault(mesh_materials[mesh], set()).add(tuple(n))
    for _ in range(rotations):
        r = random_rotation(rng)
        for material, normals in unique.items():
            setup = setups[material]
            quantized = {'global': setup['global'], 'lights': []}
            exact = {'global': setup['global'], 'lights': []}
            for light in setup['lights']:
                view = rotate(r, light['vector'])
                exact['lights'].append({**light, 'vector': view})
                quantized['lights'].append({**light, 'vector': [f32(half(game_f16(x))) for x in view]})
            for n in normals:
                nv = rotate(r, list(n))
                a, b = oracle(exact, nv), oracle(quantized, nv)
                compared += 1
                worst = max(worst, max(abs(x - y) for x, y in zip(a, b)))
    return {'rotations': rotations, 'comparisons': compared, 'max_byte_difference': worst}


# ----------------------------------------------------------------------- main
def compare(label, a, b):
    if a != b:
        raise AssertionError(f'{label}: {a!r} != {b!r}')


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('--field', required=True, type=Path)
    parser.add_argument('--environment', required=True, type=Path, help='BM section 0 of a/0/0/3 entry 137')
    parser.add_argument('--code', required=True, type=Path, help='decompressed code.bin')
    parser.add_argument('--bridge', required=True, type=Path)
    parser.add_argument('--viewer', type=Path)
    parser.add_argument('--romfs', type=Path)
    parser.add_argument('--spica', type=Path)
    parser.add_argument('--ohana', type=Path)
    parser.add_argument('--azahar', type=Path)
    parser.add_argument('--random-cases', type=int, default=20000)
    parser.add_argument('--rotations', type=int, default=200)
    parser.add_argument('--output-dir', required=True, type=Path)
    args = parser.parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=True)
    field, env, code = (p.read_bytes() for p in (args.field, args.environment, args.code))
    report = {'field_bch_sha256': hashlib.sha256(field).hexdigest(),
              'environment_bch_sha256': hashlib.sha256(env).hexdigest(),
              'code_bin_sha256': hashlib.sha256(code).hexdigest(),
              'verifier_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
              'bridge_source_sha256': hashlib.sha256((HERE / 'dump_lighting.c').read_bytes()).hexdigest()}
    compare('field hash', report['field_bch_sha256'], FIELD_SHA256)
    compare('environment hash', report['environment_bch_sha256'], ENVIRONMENT_SHA256)
    compare('code hash', report['code_bin_sha256'], CODE_SHA256)
    references = {}
    for key, (url, commit, files) in REFERENCES.items():
        root = getattr(args, key)
        checked = bool(root)
        for path, digest in files.items():
            if root:
                compare(path, hashlib.sha256((root / path).read_bytes()).hexdigest(), digest)
        references[key] = {'repository': url, 'commit': commit, 'files': files, 'hash_checked': checked}
    report['references'] = references

    # 1. Environment and material/normal decoding.
    expected = parse_environment(env)
    native = json.loads(run(args.bridge, '--environment', str(args.environment)))
    for key in ('scene', 'cameras', 'fogs'):
        compare(key, native[key], expected[key])
    compare('light count', len(native['lights']), len(expected['lights']))
    for i, (n, e) in enumerate(zip(native['lights'], expected['lights'])):
        compare(f'light {i} name', n['name'], e['name'])
        for key in ('type', 'enabled', 'flags', 'lut', 'ambient', 'diffuse', 'specular0', 'specular1', 'samplers'):
            compare(f'light {i} {key}', n[key], e[key])
        for key in ('scale', 'rotation', 'translation', 'direction'):
            compare(f'light {i} {key}', n[key], [bits(x) for x in e[key]])
        compare(f'light {i} attenuation', [n['attenuation'][0], n['attenuation_end'][0]],
                [bits(x) for x in e['attenuation']])
    compare('sets', [(s['index'], s['lights']) for s in native['sets']],
            [(s['index'], s['lights']) for s in expected['sets']])
    report['environment'] = {
        'scene': expected['scene'], 'lights': len(expected['lights']), 'sets': len(expected['sets']),
        'set_entries': sum(len(s['lights']) for s in expected['sets']),
        'ohana_8_byte_reading_valid': expected['ohana_8_byte_reading_valid'],
        'light_summary': [{k: l[k] for k in ('name', 'type', 'enabled', 'flags', 'ambient', 'diffuse')}
                          | {'direction': [round(x, 6) for x in l['direction']],
                             'identity_transform': l['scale'] == [1, 1, 1] and l['rotation'] == [0, 0, 0]}
                          for l in expected['lights']],
        'set_names': {str(s['index']): s['names'] for s in expected['sets']}}
    assert not expected['ohana_8_byte_reading_valid']

    bch, model, materials = parse_materials(field)
    native_model = json.loads(run(args.bridge, '--model', str(args.field)))
    for i, (n, e) in enumerate(zip(native_model['materials'], materials)):
        compare(f'material {i} present', n['present'], 1)
        for key in e:
            compare(f'material {i} {key}', n[key], e[key])
    normals = mesh_normals(bch, model)
    normal_values = 0
    for i, (n, e) in enumerate(zip(native_model['meshes'], normals)):
        compare(f'mesh {i} normal source', n['normal_source'], e['normal_source'])
        compare(f'mesh {i} normals', n['normals'], [bits(x) for v in e['normals'] for x in v])
        normal_values += len(n['normals'])
    report['field'] = {'materials': len(materials), 'normal_components': normal_values,
                       'normal_sources': [e['normal_source'] for e in normals],
                       'material_lighting': [{k: m[k] for k in ('flags', 'fragment_flags', 'light_set', 'fog',
                                                                  'lut_samplers', 'fresnel', 'bump')}
                                             | {'emission_ambient_diffuse': m['colors'][:3]} for m in materials]}

    # 2. code.bin words.
    for address, word, meaning in CODE_WORDS:
        compare(f'code 0x{address:x} ({meaning})', u32(code, address - CODE_BASE), word)
    report['code_words_checked'] = len(CODE_WORDS)

    # 3. Register product and lighting oracle.
    lines = run(args.bridge, '--registers').split('\n')
    for line in filter(None, lines):
        l, m, v = map(int, line.split())
        compare(f'register {l} {m}', v, register_product(l, m))
    report['register_products'] = len(list(filter(None, lines)))
    rng = random.Random(20261024)
    cases = []
    for _ in range(args.random_cases):
        lights = []
        for _ in range(rng.randrange(0, 9)):
            lights.append({'ambient': [rng.choice((0, 255, rng.randrange(1024))) for _ in range(3)],
                           'diffuse': [rng.choice((0, 255, rng.randrange(1024))) for _ in range(3)],
                           'vector': [f32(rng.uniform(-2, 2)) for _ in range(3)],
                           'two_sided': rng.randrange(2)})
        setup = {'global': [rng.choice((0, rng.randrange(1024))) for _ in range(3)], 'lights': lights}
        normal = [f32(rng.uniform(-1, 1)) for _ in range(3)]
        if rng.random() < 0.02:
            normal = [0.0, 0.0, 0.0]
        cases.append((setup, normal))
    day = material_setups(expected, materials, 'day')
    night = material_setups(expected, materials, 'night')
    mesh_materials = [m['material'] for m in native_model['meshes']]
    field_cases = 0
    for setups in (day, night):
        for mesh, values in enumerate(normals):
            for n in {tuple(v) for v in values['normals']}:
                cases.append((setups[mesh_materials[mesh]], list(n)))
                field_cases += 1
    output = run(args.bridge, '--evaluate', stdin='\n'.join(bridge_line(s, n) for s, n in cases) + '\n').split('\n')
    invalid = 0
    for k, ((setup, normal), line) in enumerate(zip(cases, output)):
        want = oracle(setup, normal)
        if want is None:
            invalid += 1
            compare(f'case {k}', line, 'invalid')
        else:
            compare(f'case {k}', list(map(int, line.split())), want)
    report['lighting_oracle'] = {'random_cases': args.random_cases, 'field_cases': field_cases,
                                 'invalid_cases': invalid, 'all_match': True}

    # 4. Untraced float16 rounding of the view-space light vector.
    report['float16_bound'] = {
        'day': f16_bound(day, materials, [e['normals'] for e in normals], mesh_materials, rng, args.rotations),
        'night': f16_bound(night, materials, [e['normals'] for e in normals], mesh_materials, rng, args.rotations)}

    # 5. Viewer, real archive paths, both lighting choices.
    setups_text = {}
    for name, setups in (('day', day), ('night', night)):
        setups_text[name] = []
        for i, s in enumerate(setups):
            line = (f'light_setup material={i} set={materials[i]["light_set"]} '
                    f'global_ambient={",".join(map(str, s["global"]))} '
                    f'point_bound={",".join(map(str, s["point_bound"]))}')
            for light_name, light in zip(s['names'], s['lights']):
                line += (f' light="{light_name}":ambient={",".join(map(str, light["ambient"]))}'
                         f':diffuse={",".join(map(str, light["diffuse"]))}'
                         f':vector={",".join("%.9g" % x for x in light["vector"])}')
            setups_text[name].append(line)
    if args.viewer and args.romfs:
        report['viewer'] = {}
        for name in ('day', 'night'):
            capture = args.output_dir / f'lighting-{name}.bmp'
            if capture.exists():
                capture.unlink()
            command = [str(args.viewer), '--romfs', str(args.romfs), '--archive', 'a/0/4/1', '--entry', '165',
                       '--section', '1', '--triangle-list', '--textured', '--combiners',
                       '--texture-archive', 'a/0/1/3', '--texture-entry', '75', '--texture-section', '11',
                       '--environment-archive', 'a/0/0/3', '--environment-entry', '137',
                       '--environment-section', '0', '--lighting', name]
            inspect = subprocess.run(command + ['--inspect'], text=True, capture_output=True, check=True,
                                     timeout=120).stdout
            for line in setups_text[name]:
                assert line + '\n' in inspect, line
            result = subprocess.run(command + ['--software', '--frames', '3', '--self-test-input',
                                               '--capture-frame', str(capture)], text=True, capture_output=True,
                                    check=True, timeout=300, env={'SDL_VIDEODRIVER': 'dummy', 'PATH': '/usr/bin:/bin'})
            report['viewer'][name] = {
                'capture_sha256': hashlib.sha256(capture.read_bytes()).hexdigest(),
                'sensitivity': [l for l in result.stdout.split('\n') if l.startswith('combiner_sensitivity')],
                'light_setups_match_oracle': True}
    report['light_setups'] = setups_text
    (args.output_dir / 'reference-comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({k: report[k] for k in ('environment', 'lighting_oracle', 'float16_bound')}, indent=1)[:3000])


if __name__ == '__main__':
    main()
