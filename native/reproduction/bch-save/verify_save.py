"""Independent check of the viewer save file and its party records.

1. Runs the viewer on Route 2 (zone 259): a catch, then a save ("s" in
   --play) to a new file.
2. Reads the file here: the header, then each 0x104-byte party slot with the
   PK6 conventions documented by PKHeX (not the native code): the XOR stream
   seed = seed * 0x41c64e6d + 0x6073 from the encryption constant over the
   0xe8-byte record's bytes 8.. and, restarted, over the 0x1c-byte party
   extension; the checksum (16-bit sum of the decoded halfwords) at 0x06;
   the block order from row (key >> 13) & 31 of the 32 x 4 table read from
   code.bin 0x00470eec (file offset of 0x00570eec); then the fields at their
   PK6 offsets (species 0x08, trainer 0x0c, experience 0x10, ability 0x14,
   ability number 0x15, nature 0x1c, moves 0x5a, PP 0x62, IVs 0x74; party
   level 0xec, HP 0xf0, stats 0xf2..). They must equal the viewer's own
   printed party (the battle_end / party lines and the load lines), the
   ability must be the species' first (a/2/1/8 offset 0x18) and the level
   must follow from the experience and the growth table (a/2/1/7).
3. Encodes those decoded records again here and requires identical bytes.
4. Loads the file in the viewer (--load ... --inspect) and checks its
   party_member lines; then loads, saves at once, and requires the same bytes.
Run with python3 -I. Output may contain local asset data.
"""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import struct
import subprocess

HERE = Path(__file__).resolve().parent
HEADER, SLOT = 36, 0x104


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, HERE / path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


battle = load('battle', '../bch-battle/verify_battle.py')
ENV = {'SDL_VIDEODRIVER': 'dummy', 'PATH': '/usr/bin:/bin'}


def stream(key, data):
    out, k = bytearray(), key
    for i in range(0, len(data) - 1, 2):
        k = (k * 0x41c64e6d + 0x6073) & 0xffffffff
        out += struct.pack('<H', struct.unpack_from('<H', data, i)[0] ^ (k >> 16))
    return bytes(out)


def decode_slot(slot, table):
    key = struct.unpack_from('<I', slot, 0)[0]
    shuffled = stream(key, slot[8:0xe8])
    checksum = sum(struct.unpack_from('<112H', shuffled)) & 0xffff
    assert checksum == struct.unpack_from('<H', slot, 6)[0], 'checksum'
    row = table[4 * ((key >> 13) & 31):4 * ((key >> 13) & 31) + 4]
    plain = bytearray(slot[:8]) + b''.join(shuffled[0x38 * row[b]:0x38 * row[b] + 0x38] for b in range(4))
    party = stream(key, slot[0xe8:0x104])
    return key, bytes(plain), party, row


def encode_slot(plain, party, row):
    key = struct.unpack_from('<I', plain, 0)[0]
    blocks = [plain[8 + 0x38 * b:8 + 0x38 * b + 0x38] for b in range(4)]
    shuffled = bytearray(0xe0)
    for b in range(4):
        shuffled[0x38 * row[b]:0x38 * row[b] + 0x38] = blocks[b]
    checksum = sum(struct.unpack_from('<112H', shuffled)) & 0xffff
    header = bytearray(plain[:8])
    struct.pack_into('<H', header, 6, checksum)
    return bytes(header) + stream(key, bytes(shuffled)) + stream(key, party)


def fields(plain, party):
    u16 = lambda o: struct.unpack_from('<H', plain, o)[0]  # noqa: E731
    ivs = struct.unpack_from('<I', plain, 0x74)[0]
    return {'species': u16(0x08), 'trainer': struct.unpack_from('<I', plain, 0x0c)[0],
            'exp': struct.unpack_from('<I', plain, 0x10)[0], 'ability': plain[0x14], 'ability_number': plain[0x15],
            'nature': plain[0x1c], 'moves': [u16(0x5a + 2 * k) for k in range(4)],
            'pp': list(plain[0x62:0x66]), 'iv': [(ivs >> (5 * k)) & 31 for k in range(6)], 'iv_high_bits': ivs >> 30,
            'level': party[4], 'hp': struct.unpack_from('<H', party, 8)[0],
            'stats': list(struct.unpack_from('<6H', party, 0x0a))}


def main():
    p = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    p.add_argument('--romfs', type=Path, required=True)
    p.add_argument('--code-bin', type=Path, required=True)
    p.add_argument('--viewer', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    args = p.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    code = args.code_bin.read_bytes()
    table = code[0x470eec:0x470eec + 128]
    save = args.out / 'route2.sav'
    for f in (save, args.out / 'again.sav'):
        if f.exists():
            f.unlink()
    play = 'R' + 'D' * 9 + '..' + 'R.' + '.x' * 10 + 'R...' + 's'
    run = subprocess.run([str(args.viewer), '--romfs', str(args.romfs), '--zone', '259', '--play', play,
                          '--encounter-rate', '100', '--code-bin', str(args.code_bin), '--save', str(save),
                          '--software', '--frames', '320'], capture_output=True, text=True, check=True,
                         timeout=1800, env=ENV).stdout
    lines = [l for l in run.split('\n') if l.startswith(('partner ', 'wild ', 'battle_end ', 'party ', 'save '))]
    data = save.read_bytes()
    assert len(data) == HEADER + 6 * SLOT and data[:4] == b'PXSV'
    version, count = struct.unpack_from('<HH', data, 4)
    zone, wx, wz = struct.unpack_from('<Iii', data, 8)
    facing = data[20]
    balls, rng, trainer = struct.unpack_from('<III', data, 24)
    save_line = next(l for l in lines if l.startswith('save '))
    assert f'zone={zone} world_tile={wx},{wz} facing={facing} party={count} balls={balls}' in save_line, save_line
    assert data[HEADER + count * SLOT:] == bytes(len(data) - HEADER - count * SLOT), 'unused slots'
    personal = [battle.parse_personal(battle.raw(args.romfs, 'a/2/1/8', i)) for i in range(800)]
    growth = [list(struct.unpack('<101I', battle.raw(args.romfs, 'a/2/1/7', g))) for g in range(8)]
    party_line = next(l for l in lines if l.startswith('party '))
    printed = re.findall(r' (\d+):(\w+):Lv(\d+):(\d+)/(\d+)', party_line)
    battlers = {'partner': next(l for l in lines if l.startswith('partner ')),
                'wild': next(l for l in lines if l.startswith('wild '))}
    records = []
    for i in range(count):
        slot = data[HEADER + i * SLOT:HEADER + (i + 1) * SLOT]
        key, plain, party, row = decode_slot(slot, table)
        f = fields(plain, party)
        f['key'] = key
        assert encode_slot(plain, party, row) == slot, 'independent re-encoding differs'
        # Against the viewer's printed party, battlers and data tables.
        n, name, level, hp, max_hp = printed[i]
        assert (int(n), f['level'], f['hp'], f['stats'][0]) == (i, int(level), int(hp), int(max_hp)), (f, printed[i])
        source = battlers['partner' if i == 0 else 'wild']
        assert f'species={f["species"]} name={name} level=' in source, (source, f)
        assert f' nature={f["nature"]} iv={",".join(map(str, f["iv"]))} stats={",".join(map(str, f["stats"]))} ' \
               f'moves={",".join(map(str, f["moves"]))}' in source, (source, f)
        assert f['ability'] == personal[f['species']]['abilities'][0] and f['ability_number'] == 1, f
        table_g = growth[personal[f['species']]['growth']]
        assert max(l for l in range(1, 101) if table_g[l] <= f['exp']) == f['level'], f
        assert f['trainer'] == trainer and f['iv_high_bits'] == 0, f
        records.append(f)
    # 4.
    loaded = subprocess.run([str(args.viewer), '--romfs', str(args.romfs), '--load', str(save), '--code-bin',
                             str(args.code_bin), '--inspect'], capture_output=True, text=True, check=True,
                            timeout=600).stdout
    for i, f in enumerate(records):
        want = (f'party_member slot={i} species={f["species"]} level={f["level"]} exp={f["exp"]} '
                f'hp={f["hp"]}/{f["stats"][0]} moves={",".join(map(str, f["moves"]))} pp={",".join(map(str, f["pp"]))}')
        assert want in loaded, (want, loaded[-800:])
    load_line = next(l for l in loaded.split('\n') if l.startswith('load '))
    again = args.out / 'again.sav'
    subprocess.run([str(args.viewer), '--romfs', str(args.romfs), '--load', str(save), '--code-bin',
                    str(args.code_bin), '--save', str(again), '--play', 's', '--software', '--frames', '5'],
                   capture_output=True, text=True, check=True, timeout=600, env=ENV)
    assert again.read_bytes() == data, 'load then save changed the file'
    report = {'verifier_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
              'play': play, 'viewer_lines': lines, 'file_bytes': len(data), 'version': version,
              'header': {'zone': zone, 'world_tile': [wx, wz], 'facing': facing, 'balls': balls,
                         'random_state_present': rng != 0, 'trainer': trainer},
              'records': [{k: v for k, v in f.items() if k != 'key'} for f in records],
              'checksums_valid': count, 'independent_re_encoding_identical': count,
              'load_line': load_line, 'load_then_save_identical': True}
    (args.out / 'reference-comparison.json').write_text(json.dumps(report, indent=1) + '\n')
    print(json.dumps(report, indent=1)[:4000])


if __name__ == '__main__':
    main()
