"""Independent check of the Pokemon battle motions (a/0/0/7, PB packs).

1. For every model m: entry 9 + 8m is a PB pack; its section 0 is parsed as
   a GF1 motion pack by ../bch-motion/verify_motion.py (slot table, motion
   skeleton, channel codes, keys) and compared field by field with the
   native decoder through the dump_motion bridge; the slot occupancy over
   all packs is reported (the basis of the viewer's slot policy).
2. For a sample (Froakie and the Route 2 species, the first models of
   ../bch-pokemon), the slots the viewer plays (1, 4, 5, 6, 17, 18, when
   present): every present channel at every integer frame (identical bits),
   the bone locals at every frame (1e-5), and every posed index at the first,
   middle and last frame (1e-4) against verify_motion's independent model.
3. The viewer on Route 2: its battle_model lines must list the slots and
   frame counts of step 1, and a scripted battle must end as before.
Run with python3 -I. Output may contain local asset data.
"""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path
import struct
import subprocess

HERE = Path(__file__).resolve().parent
PLAYED = (1, 4, 5, 6, 17, 18)
SAMPLE = (656, 659, 263, 661, 664, 13, 16)


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, HERE / path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


motion = load('motion', '../bch-motion/verify_motion.py')
models = load('models', '../bch-pokemon/verify_pokemon.py')
player, walk = motion.player, models.walk
bits, from_bits = player.bits, player.from_bits
ENV = models.ENV


def compare_pack(native, pack):
    c = player.compare
    c('skeleton header', [native['bone_count'], native['first_bone'], native['slot_count']],
      [len(pack['bones']), pack['first'], pack['slots']])
    for i, (n, e) in enumerate(zip(native['bones'], pack['bones'])):
        c(f'motion bone {i}', [n['name'], n['parent'], n['flags'], n['children'], n['translation'], n['rotation']],
          [e['name'], e['parent'], e['flags'], e['children'], [bits(x) for x in e['translation']],
           [bits(x) for x in e['rotation']]])
    c('motion count', len(native['motions']), len(pack['motions']))
    keys = 0
    for n, e in zip(native['motions'], pack['motions']):
        c(f"slot {e['slot']} header", [n['slot'], n['frames'], n['byte_offset'], n['byte_length']],
          [e['slot'], e['frames'], e['byte_offset'], e['byte_length']])
        for k, (nc, ec) in enumerate(zip(n['channels'], e['channels'])):
            c(f"slot {e['slot']} channel {k}", nc, [ec[0], [[x[0], bits(x[1]), bits(x[2])] for x in ec[1]]])
            keys += len(ec[1])
    return keys


def main():
    p = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    p.add_argument('--romfs', type=Path, required=True)
    p.add_argument('--bridge', type=Path, required=True)
    p.add_argument('--viewer', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    args = p.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    garc = models.Garc(args.romfs / 'a/0/0/7')
    index = garc.member(0)
    records = [struct.unpack_from('<HBB', index, 4 * i) for i in range(722)]
    model_count = sum(c for _f, c, _g in records)
    report = {'verifier_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    model_path, pack_path = args.out / 'model.bch', args.out / 'pack.bin'
    occupancy, empty, keys, packs = {}, [], 0, {}
    sample_models = {records[s - 1][0]: s for s in SAMPLE}
    for m in range(model_count):
        pb = models.unpack(garc.member(9 + 8 * m))
        sections = walk.sections(pb, b'PB')
        data = sections[0]
        if len(data) < 8 or not struct.unpack_from('<I', data, 0)[0]:
            empty.append(m)
            continue
        pc = walk.sections(models.unpack(garc.member(4 + 8 * m)), b'PC')[0]
        if not pc:
            empty.append(m)
            continue
        pack = motion.parse_pack(data)
        model_path.write_bytes(pc)
        pack_path.write_bytes(data)
        slots = [x for x in PLAYED if any(mo['slot'] == x for mo in pack['motions'])] if m in sample_models else []
        out = subprocess.run([str(args.bridge), str(model_path), str(pack_path), *map(str, slots)],
                             capture_output=True, text=True, check=True, timeout=900).stdout
        native = json.loads(out)
        keys += compare_pack(native, pack)
        for mo in pack['motions']:
            occupancy[mo['slot']] = occupancy.get(mo['slot'], 0) + 1
        packs[m] = {mo['slot']: mo['frames'] for mo in pack['motions']}
        if not slots:
            continue
        model = player.parse(pc)
        rest = player.rest_world(model['bones'])
        by_slot = {mo['slot']: mo for mo in pack['motions']}
        worst = [0.0, 0.0, 0.0]
        values = posed = 0
        for pose in native['poses']:
            mo = by_slot[pose['slot']]
            last = len(pose['frames']) - 1
            for frame, record in enumerate(pose['frames']):
                expected = [[k, bits(motion.value(*mo['channels'][k], frame))]
                            for k in range(len(mo['channels'])) if mo['channels'][k][0]]
                player.compare(f"model {m} slot {pose['slot']} frame {frame} values", record['values'], expected)
                values += len(expected)
                loc = motion.locals_at(model, pack, mo, frame)
                got = [from_bits(b) for b in record['locals']]
                worst[0] = max(worst[0], max(abs(a - b) for a, b in zip(got, [x for l in loc for x in l])))
                if frame in (0, last // 2, last):
                    positions, normals = motion.pose_frame(model, loc, rest)
                    gp = [from_bits(b) for b in record['positions']]
                    gn = [from_bits(b) for b in record['normals']]
                    player.compare('posed count', len(gp), len(positions))
                    worst[1] = max(worst[1], max(abs(a - b) for a, b in zip(gp, positions)))
                    worst[2] = max(worst[2], max(abs(a - b) for a, b in zip(gn, normals)))
                    posed += 1
        assert worst[0] <= 1e-5 and worst[1] <= 1e-4 and worst[2] <= 1e-4, (m, worst)
        report.setdefault('sample', {})[str(sample_models[m])] = {
            'model': m, 'slots': slots, 'channel_values_identical': values, 'posed_frames': posed,
            'max_local_difference': worst[0], 'max_position_difference': worst[1],
            'max_normal_difference': worst[2]}
    report['packs'] = {'models': model_count, 'compared': len(packs), 'identical': len(packs),
                       'empty_or_without_model': empty, 'keys': keys,
                       'slot_occupancy': {str(k): v for k, v in sorted(occupancy.items())}}
    # 3.
    play = 'R' + 'D' * 9 + '.' * 120
    out = subprocess.run([str(args.viewer), '--romfs', str(args.romfs), '--zone', '259', '--play', play,
                          '--encounter-rate', '100', '--software', '--frames', '520'], capture_output=True, text=True,
                         check=True, timeout=1800, env=ENV).stdout
    lines = [l for l in out.split('\n') if l.startswith(('battle_model ', 'battle_end '))]
    for side in ('partner', 'wild'):
        line = next(l for l in lines if l.startswith(f'battle_model side={side} '))
        species = int(line.split(' species=')[1].split()[0])
        first = records[species - 1][0]
        slots = ','.join(f'{s}:{f}' for s, f in sorted(packs[first].items()))
        want = (f' motion_entry={9 + 8 * first} motions={len(packs[first])} slots={slots} pose=idle')
        assert line.endswith(want), (line, want)
    assert any(l.startswith('battle_end result=won ') for l in lines), lines
    report['runtime'] = {'zone': 259, 'play': play, 'lines': lines}
    (args.out / 'reference-comparison.json').write_text(json.dumps(report, indent=1) + '\n')
    print(json.dumps(report, indent=1)[:5000])


if __name__ == '__main__':
    main()
