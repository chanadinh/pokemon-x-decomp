#!/usr/bin/env python3
"""Independently re-derive BCH material preview state and compare the native one.

This script deliberately does not import the production inspectors under
native/tools. It parses BCH sections, relocations and PICA command packets
itself, interprets registers with the bit layouts of the pinned CTRMap
PICACommandReader/H3DMaterial sources, and compares the result with the native
bridge (dump_materials.c). Bound texture pixels are checked against the RGBA
hashes already verified in ../bch-textures/reference-comparison.json, and
external indices against ../bch-model/field165-model.json.

Optionally it runs the native viewer on a RomFS-shaped directory, captures a
software frame and measures how many covered pixels are exact texels of the
bound images. Only metadata is written to the report; no pixels or payloads.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import sys

HERE = Path(__file__).resolve().parent
FIELD_SHA256 = 'cb131a9590850d604b30aa558b7c55fba6fff654b2f6dd05c823af5ecbc16ccd'
TEXTURES_SHA256 = '13cbbc5477fba7c1f79f6c0c3ecc73acb11fe67bdc3131ae0014a60d004104fb'
CTRMAP = {
    'commit': '4b780a7bc32a75a7f31c162829b086355caf32f2',
    'H3DMaterial.java': '97e6c7d681db2ff4484cb21579e9cdabd49efe62d5700e05ee0984788e4ee91d',
    'PICACommandReader.java': '0518e9a087b5e193f086abf73e3396800b4880e7309892665d5e7c0ab26f7b99',
    'PICACommand.java': 'dfc81cbfe77b07bf1a823e9df79af969be3274873001eda1932015610c090ea6',
}
STAGES = (0xc0, 0xc8, 0xd0, 0xd8, 0xf0, 0xf8)
BACKGROUND = (22, 27, 36)


def u32(data, at):
    return struct.unpack_from('<I', data, at)[0]


class BCH:
    """Minimal independent BCH 7/7 section and relocation resolver."""

    def __init__(self, data):
        if data[:4] != b'BCH\0' or data[4:6] != b'\x07\x07':
            raise ValueError('not a BCH 7/7 file')
        self.data = data
        starts = struct.unpack_from('<5I', data, 8)
        sizes = struct.unpack_from('<5I', data, 28)
        names = ('contents', 'strings', 'commands', 'raw', 'relocation')
        self.base = dict(zip(names, starts))
        self.size = dict(zip(names, sizes))
        self.reloc = {}
        targets = {0: 'contents', 1: 'strings', 2: 'commands', 3: 'commands'}
        for at in range(starts[4], starts[4] + sizes[4], 4):
            word = u32(data, at)
            tag, displacement = word >> 25, word & 0x1ffffff
            source = 'commands' if tag >= 0x24 else 'contents'
            field = self.base[source] + displacement * (1 if tag == 1 else 4)
            target = targets.get(tag, 'raw')
            self.reloc[field] = (tag, target)

    def ptr(self, field, section=None, tags=None):
        if field not in self.reloc:
            return None
        tag, target = self.reloc[field]
        if (section and target != section) or (tags and tag not in tags):
            raise ValueError(f'unexpected relocation at 0x{field:x}')
        at = self.base[target] + u32(self.data, field)
        if not self.base[target] <= at <= self.base[target] + self.size[target]:
            raise ValueError('relocated pointer outside its section')
        return at

    def string(self, field):
        at = self.ptr(field, 'strings')
        if at is None:
            return None
        return self.data[at:self.data.index(b'\0', at)]

    def commands(self, field):
        """Masked register state after one stream; no state is inherited."""
        at, words = self.ptr(field, 'commands', (2,)), u32(self.data, field + 4)
        end = at + words * 4
        if not words or end > self.base['commands'] + self.size['commands']:
            raise ValueError('command stream outside commands section')
        values, known = {}, {}
        while at < end:
            first, header = struct.unpack_from('<II', self.data, at)
            register, mask = header & 0xffff, (header >> 16) & 15
            extra, consecutive = (header >> 20) & 0x7ff, header >> 31
            bits = sum(0xff << (8 * i) for i in range(4) if mask >> i & 1)
            parameters = [first] + [u32(self.data, at + 8 + 4 * i) for i in range(extra)]
            for i, value in enumerate(parameters):
                target = register + (i if consecutive else 0)
                if target == 0x22f and bits:
                    raise ValueError('draw command inside a material stream')
                values[target] = (values.get(target, 0) & ~bits) | (value & bits)
                known[target] = known.get(target, 0) | bits
            count = 2 + extra
            at += 4 * (count + (count & 1))
        if at != end:
            raise ValueError('command packets overrun the declared stream')
        return values, known


def reference_materials(data):
    bch = BCH(data)
    contents = bch.base['contents']
    table = bch.ptr(contents, 'contents')            # category 0: models
    model = bch.ptr(table, 'contents')
    count = u32(data, model + 0x38)
    first = bch.ptr(model + 0x34, 'contents')
    out = []
    for i in range(count):
        record = first + i * 0x58
        params = bch.ptr(record, 'contents')
        tex_values, tex_known = bch.commands(record + 0x10)
        frag_values, frag_known = bch.commands(params + 0xc8)
        coordinators = []
        for unit in range(3):
            at = params + 12 + unit * 24
            coordinators.append([u32(data, at), *struct.unpack_from('<5f', data, at + 4)])
        registers = {}
        for reg in [0x40, 0x100, 0x101, 0x104, 0x107, 0xe0] + [s + j for s in STAGES for j in range(5)]:
            registers[reg] = (frag_values.get(reg, 0), frag_known.get(reg, 0))
        out.append({
            'name': bch.string(record + 0x54),
            'textures': [bch.string(record + 0x48 + 4 * u) for u in range(3)],
            'texture_unit_config': tex_values[0x80],
            'unit_parameters': [(tex_values.get(r, 0), tex_known.get(r, 0)) for r in (0x83, 0x93, 0x9b)],
            'coordinators': coordinators,
            'registers': registers,
        })
    return out


def interpret(m):
    """CTRMap field layouts; the preview subset mirrors bch_material.h."""
    r = m['registers']
    source, operand, combine, scale = (r[0xc0][0], r[0xc1][0], r[0xc2][0], r[0xc4][0])
    rgb_src = [(source >> (4 * i)) & 15 for i in range(3)]
    alpha_src = [(source >> (16 + 4 * i)) & 15 for i in range(3)]
    combine_rgb, combine_alpha = combine & 0xffff, combine >> 16
    assert 3 <= rgb_src[0] <= 5 and alpha_src[0] == rgb_src[0], 'stage 0 must sample one texture'
    assert combine_rgb in (0, 1) and combine_alpha in (0, 1)
    assert combine_rgb == 0 or rgb_src[1] in (0, 1)
    assert combine_alpha == 0 or alpha_src[1] in (0, 1)
    assert operand & 0xff == 0 and (operand >> 12) & 0xff == 0 and scale & 0x30003 == 0
    unit = rgb_src[0] - 3
    packed, *srt = m['coordinators'][unit]
    assert (packed >> 16) & 0xff == 0 and srt == [1, 1, 0, 0, 0], 'ordinary identity UV mapping'
    sampler = m['unit_parameters'][unit][0]
    blend = r[0x101][0]
    factors = ((blend >> 16) & 15, (blend >> 20) & 15, (blend >> 24) & 15, blend >> 28)
    kinds = {(1, 0): 0, (6, 7): 1, (6, 1): 2}
    assert blend & 0xffff == 0 and factors[:2] == factors[2:] and factors[:2] in kinds
    assert (r[0x100][0] & 3) == 0 and (r[0x100][0] >> 8) & 3 == 1, 'blend mode, not logic op'
    assert (sampler >> 28) & 7 == 0
    alpha, depth = r[0x104][0], r[0x107][0]
    return {
        'unit': unit, 'combine_rgb': combine_rgb, 'combine_alpha': combine_alpha,
        'wrap_u': (sampler >> 12) & 0xf, 'wrap_v': (sampler >> 8) & 0xf,
        'mag_linear': (sampler >> 1) & 1, 'min_linear': (sampler >> 2) & 1,
        'alpha_test': alpha & 1, 'alpha_compare': (alpha >> 4) & 0xf,
        'alpha_reference': (alpha >> 8) & 0xff,
        'depth_test': depth & 1, 'depth_compare': (depth >> 4) & 0xf,
        'depth_write': (depth >> 12) & 1, 'color_write_mask': (depth >> 8) & 0xf,
        'cull': r[0x40][0] & 3, 'blend': kinds[factors[:2]],
        'unexecuted_units': m['texture_unit_config'] & 7 & ~(1 << unit),
    }


def bmp_rgb(data):
    offset = u32(data, 10)
    width, signed_height = struct.unpack_from('<ii', data, 18)
    bits = struct.unpack_from('<H', data, 28)[0]
    stride = ((width * bits + 31) // 32) * 4
    step = bits // 8
    height = abs(signed_height)  # positive BMP height means bottom-up rows
    pixels = []
    for y in range(height):
        row = offset + (height - 1 - y if signed_height > 0 else y) * stride
        for x in range(width):
            b, g, r = data[row + x * step:row + x * step + 3]
            pixels.append((r, g, b))
    return width, height, pixels


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('--field', required=True, type=Path, help='field165 GR section 1 BCH')
    parser.add_argument('--textures', required=True, type=Path, help='area75 AD section 11 BCH')
    parser.add_argument('--bridge', required=True, type=Path)
    parser.add_argument('--viewer', type=Path)
    parser.add_argument('--romfs', type=Path)
    parser.add_argument('--model-args', default='--archive a/0/4/1 --entry 165 --section 1')
    parser.add_argument('--texture-args', default='--texture-archive a/0/1/3 --texture-entry 75 --texture-section 11')
    parser.add_argument('--ctrmap', type=Path, help='optional CTRMap checkout to hash-check')
    parser.add_argument('--output-dir', required=True, type=Path)
    args = parser.parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=False)

    field, textures = args.field.read_bytes(), args.textures.read_bytes()
    report = {'field_bch_sha256': hashlib.sha256(field).hexdigest(),
              'texture_bch_sha256': hashlib.sha256(textures).hexdigest(),
              'verifier_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
              'bridge_source_sha256': hashlib.sha256((HERE / 'dump_materials.c').read_bytes()).hexdigest(),
              'register_reference': CTRMAP}
    assert report['field_bch_sha256'] == FIELD_SHA256, 'unexpected field BCH'
    assert report['texture_bch_sha256'] == TEXTURES_SHA256, 'unexpected texture BCH'
    if args.ctrmap:
        for name, digest in CTRMAP.items():
            if name == 'commit':
                continue
            matches = list(args.ctrmap.rglob(name))
            assert len(matches) == 1 and hashlib.sha256(matches[0].read_bytes()).hexdigest() == digest, name
        report['register_reference']['hashes_checked'] = True

    native = json.loads(subprocess.run([str(args.bridge), str(args.field), str(args.textures)],
                                       check=True, capture_output=True, text=True).stdout)['materials']
    reference = reference_materials(field)
    pixel_refs = {s['name']: s for s in
                  json.loads((HERE / '../bch-textures/reference-comparison.json').read_text())['samples']}
    earlier = json.loads((HERE / '../bch-model/field165-model.json').read_text())['materials']
    assert len(native) == len(reference) == len(earlier) == 12
    results, register_checks, bound = [], 0, {}
    for i, (n, ref, old) in enumerate(zip(native, reference, earlier)):
        assert bytes.fromhex(n['name_hex']) == ref['name'], f'material {i} name'
        assert n['parameters_present'] == 1 and n['texture_unit_config'] == ref['texture_unit_config']
        for reg, (value, known) in ref['registers'].items():
            assert n['fragment'][f'0x{reg:x}'] == [value, known], f'material {i} register 0x{reg:x}'
            register_checks += 1
        assert [tuple(x) for x in n['unit_parameters']] == ref['unit_parameters'], f'material {i} sampler'
        for unit in range(3):
            got, want = n['coordinators'][unit], ref['coordinators'][unit]
            assert got[0] == want[0] and all(struct.pack('<f', a) == struct.pack('<f', b)
                                             for a, b in zip(got[1:], want[1:])), f'material {i} coord'
        expected = interpret(ref)
        assert n['preview_result'] == 0 and n['preview'] == expected, f'material {i} preview'
        name = ref['textures'][expected['unit']]
        assert bytes.fromhex(n['texture_name_hex']) == name and n['name_matches'] == 1
        assert n['external_index'] == old['textures'][expected['unit']]['external_index']
        rgba = bytes.fromhex(n['rgba'])
        sample = pixel_refs[name.decode()]
        assert (n['width'], n['height']) == (sample['width'], sample['height'])
        assert hashlib.sha256(rgba).hexdigest() == sample['rgba_sha256'], f'material {i} pixels'
        bound[n['external_index']] = rgba
        results.append({'index': i, 'name': ref['name'].decode(), 'texture': name.decode(),
                        'external_index': n['external_index'], 'rgba_sha256': sample['rgba_sha256'],
                        **expected})
    report.update(result='passed', materials_compared=len(results), registers_compared=register_checks,
                  bound_textures=len(bound), materials=results)

    if args.viewer and args.romfs:
        capture = args.output_dir / 'field-textured.bmp'
        command = [str(args.viewer), '--romfs', str(args.romfs), *args.model_args.split(),
                   '--triangle-list', '--textured', *args.texture_args.split(), '--software',
                   '--frames', '3', '--self-test-input', '--capture-frame', str(capture)]
        run = subprocess.run(command, capture_output=True, text=True, timeout=60,
                             env={'SDL_VIDEODRIVER': 'dummy', 'PATH': '/usr/bin:/bin'})
        assert run.returncode == 0, run.stderr
        assert 'Input self-test passed' in run.stderr
        texels = {tuple(rgba[j:j + 3]) for rgba in bound.values() for j in range(0, len(rgba), 4)}
        width, height, pixels = bmp_rgb(capture.read_bytes())
        covered = [p for p in pixels if p != BACKGROUND]
        exact = sum(p in texels for p in covered)
        report['runtime'] = {
            'backend': 'SDL dummy, software renderer', 'frames': 3, 'input_self_test': 'passed',
            'model_arguments': args.model_args, 'texture_arguments': args.texture_args,
            'width': width, 'height': height, 'capture_sha256': hashlib.sha256(capture.read_bytes()).hexdigest(),
            'covered_pixels': len(covered), 'exact_texel_pixels': exact,
            'exact_texel_fraction': round(exact / len(covered), 6) if covered else 0,
            'note': 'Exact texel colors are expected under unlit replace blending with nearest '
                    'sampling; source-alpha and additive materials and background coincidences '
                    'account for the remainder. This is not a game-equivalence measure.'}
        assert report['runtime']['exact_texel_fraction'] > 0.95
    (args.output_dir / 'reference-comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    json.dump({k: v for k, v in report.items() if k != 'materials'}, sys.stdout, indent=2)
    print()


if __name__ == '__main__':
    main()
