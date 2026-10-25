"""The fragment-lighting LUTs of every Pokémon model (a/0/0/7), read twice.

For each model's BCH (entry 4 + 8m, PC section 0) this reader, independent
of native/assets/bch_model.c and following SPICA bd29a78 (H3DLUT,
H3DLUTSampler, H3DMaterialParams), collects:

- each LUT set of index slot 4 (sampler array +0, count +4, name +8) and its
  samplers (16 bytes: flags, command pointer, word count, name), with the 256
  entries their command streams write to registers 0x1c8..0x1cf (0x1c5 sets
  the index);
- each material's LUT config commands (parameters +168..+191: registers
  0x1d0..0x1d2), fragment flags (+6), bump mode and texture (+166, +167),
  Fresnel selector (+165), table names (+0xd4 + 4k) and sampler names
  (+0xec + 4k);

and compares them with the native decoder through dump_luts (the bridge in
this directory): every name, flag and register, and per sampler the entry
sum and FNV-1a-64 over the 256 entries. Writes reference-comparison.json.
Run with python3 -I. Output may contain local asset data.
"""
import argparse
import importlib.util
import json
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
u32 = vm.u32


def stream(bch, field_value_at, words):
    d = bch.data
    at, end = field_value_at, field_value_at + 4 * words
    while at < end:
        first, header = struct.unpack_from('<II', d, at)
        reg, extra, consecutive = header & 0xffff, (header >> 20) & 0x7ff, header >> 31
        params = [first] + [u32(d, at + 8 + 4 * i) for i in range(extra)]
        for i, p in enumerate(params):
            yield reg + (i if consecutive else 0), p, (header >> 16) & 15
        count = 2 + extra
        at += 4 * (count + (count & 1))


def expected(data):
    b = vm.BCH(data)
    c = b.base['contents']
    lines = []
    if u32(data, c + 52):
        table = b.ptr(c + 48)
        for k in range(u32(data, c + 52)):
            s = b.ptr(table + 4 * k)
            name = b.string(s + 8).decode()
            samplers = b.ptr(s) if u32(data, s + 4) else None
            for j in range(u32(data, s + 4)):
                at = samplers + 16 * j
                start = b.ptr(at + 4, 'commands')
                entries, index = [0] * 256, 0
                for reg, value, mask in stream(b, start, u32(data, at + 8)):
                    assert mask == 15
                    if reg == 0x1c5:
                        index = value & 255
                    elif 0x1c8 <= reg <= 0x1cf:
                        entries[index] = value & 0xffffff
                        index += 1
                mix = 1469598103934665603
                for e in entries:
                    mix = ((mix ^ e) * 1099511628211) & (2 ** 64 - 1)
                lines.append(f'lut {name} {b.string(at + 12).decode()} flags={u32(data, at)} '
                             f'sum={sum(entries)} fnv={mix:016x}')
    model = b.ptr(b.ptr(c))
    first = b.ptr(model + 0x34)
    for i in range(u32(data, model + 0x38)):
        record = first + i * 0x58
        p = b.ptr(record)
        regs, known = [0, 0, 0], 0
        for k in range(3):
            value, header = u32(data, p + 168 + 8 * k), u32(data, p + 172 + 8 * k)
            reg = header & 0xffff
            if 0x1d0 <= reg <= 0x1d2 and (header >> 16) & 15 == 15 and not (header >> 20) & 0x7ff:
                regs[reg - 0x1d0] = value
                known |= 1 << (reg - 0x1d0)
        text = lambda field: b.string(field).decode() if field in b.reloc else '-'
        slots = ' '.join(f'{text(p + 0xd4 + 4 * k)}/{text(p + 0xec + 4 * k)}' for k in range(6))
        lines.append(f'material {b.string(record + 0x54).decode()} frag={data[p + 6]} bump={data[p + 166]}/{data[p + 167]} '
                     f'fresnel={data[p + 165]} known={known} regs={regs[0]:x},{regs[1]:x},{regs[2]:x} {slots}')
    return lines


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--romfs', type=Path, required=True)
    ap.add_argument('--bridge', type=Path, required=True)
    ap.add_argument('--models', type=int, default=981)
    args = ap.parse_args()
    garc = shaders.Garc(args.romfs / 'a/0/0/7')
    totals = {'models': 0, 'luts': 0, 'materials': 0, 'mismatches': []}
    inputs, flags, bumps = {}, {}, {}
    with tempfile.TemporaryDirectory() as scratch:
        path = Path(scratch) / 'model.bch'
        for m in range(args.models):
            bch = shaders.walk.sections(garc.member(4 + 8 * m), b'PC')[0]
            if not bch:
                continue
            path.write_bytes(bch)
            got = subprocess.run([str(args.bridge), str(path)], capture_output=True, text=True).stdout.splitlines()
            want = expected(bch)
            totals['models'] += 1
            totals['luts'] += sum(line.startswith('lut ') for line in want)
            totals['materials'] += sum(line.startswith('material ') for line in want)
            if got != want:
                totals['mismatches'].append({'model': m, 'got': got[:3], 'want': want[:3]})
            for line in want:
                if line.startswith('material '):
                    words = dict(w.split('=', 1) for w in line.split() if '=' in w)
                    inputs[words['regs']] = inputs.get(words['regs'], 0) + 1
                    flags[words['frag']] = flags.get(words['frag'], 0) + 1
                    bumps[words['bump']] = bumps.get(words['bump'], 0) + 1
    result = {**totals, 'lut_registers': dict(sorted(inputs.items(), key=lambda kv: -kv[1])),
              'fragment_flags': dict(sorted(flags.items(), key=lambda kv: -kv[1])),
              'bump_mode_texture': dict(sorted(bumps.items(), key=lambda kv: -kv[1]))}
    (HERE / 'reference-comparison.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: v for k, v in result.items() if k != 'mismatches'}, indent=2))
    print(len(totals['mismatches']), 'mismatching models')
    assert not totals['mismatches']


if __name__ == '__main__':
    main()
