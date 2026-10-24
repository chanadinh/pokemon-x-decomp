"""Independent check of the battle data, formulas and a wild battle on Route 2.

1. Reads personal (a/2/1/8), move (a/2/1/2) and learnset (a/2/1/4) entries,
   every zone's wild-encounter section (a/0/1/2, ZO section 3) and the type
   chart (DllBattle.cro 0xD12A8) itself, after pk3DS 6daaca9 layouts, and
   compares every field with the native decoders through dump_battle; also
   the moves a wild Pokémon knows at levels 5 and 50.
2. Stats: the published Generation VI formulas (HP = (2B + IV) L / 100 + L +
   10, others ((2B + IV) L / 100 + 5) x nature, all floored; Shedinja's HP
   1) with the nature table read from code.bin, against the viewer's stats,
   which run the reconstructed routines of reconstructed/pokemon_stats, for
   every species at four levels, three IV patterns and all 25 natures in
   turn.
3. Damage: random cases of the battle module's stated formula (game/battle.h)
   computed independently.
4. A wild battle in the viewer's display loop on Route 2 (zone 259): an
   independent model of its random number use (encounter roll, slot, level,
   IVs and nature of the wild Pokémon and of the partner, then the battle's
   own generator) and of the battle rules of game/battle.h predicts the
   encounter, both Pokémon and every battle message; the viewer must print
   exactly those.
Run with python3 -I. Output may contain local asset data.
"""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path
import random
import struct
import subprocess

HERE = Path(__file__).resolve().parent


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, HERE / path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


town = load('town', '../bch-town/verify_town.py')
talk = load('talk', '../bch-talk/verify_talk.py')
walk = town.walk
ENV = town.ENV


def raw(romfs, archive, entry):
    return walk.garc_member(romfs / archive, entry)


def parse_personal(d):
    assert len(d) >= 0x40
    return {'base': list(d[0:6]), 'types': [d[6], d[7]], 'catch': d[8], 'growth': d[0x15],
            'abilities': [d[0x18], d[0x19], d[0x1a]], 'base_exp': struct.unpack_from('<H', d, 0x22)[0]}


def parse_move(d):
    assert len(d) >= 0x22
    return {'fields': [d[0], d[2], d[3], d[4], d[5], d[6], d[14], d[20]], 'stat': list(d[21:24]),
            'stage': [b - 256 if b >= 128 else b for b in d[24:27]], 'percent': list(d[27:30])}


def parse_learnset(d):
    pairs = []
    for at in range(0, len(d), 4):
        m, l = struct.unpack_from('<HH', d, at)
        if (m, l) == (0xffff, 0xffff):
            return pairs
        pairs.append([m, l])
    raise AssertionError('no terminator')


def moves_at(pairs, level):
    out, slot = [0, 0, 0, 0], 0
    for m, l in pairs:
        if l > level:
            break
        if m in out:
            continue
        out[slot] = m
        slot = (slot + 1) % 4
    return out


def stat_values(base, level, iv, nature, natures, species):
    hp = 1 if species == 292 else (2 * base[0] + iv[0]) * level // 100 + level + 10
    out = [hp]
    columns = [None, 0, 1, 4, 2, 3]  # personal index -> nature column (Atk, Def, SpA, SpD, Spe)
    for k in range(1, 6):
        v = (2 * base[k] + iv[k]) * level // 100 + 5
        a = natures[nature * 5 + columns[k]]
        if a == 1:
            v = v * 110 // 100
        elif a == 0xff:
            v = v * 90 // 100
        out.append(v)
    return out


class Rng:
    def __init__(self, state):
        self.state = state or 1

    def next(self):
        x = self.state or 0x9e3779b9
        x ^= (x << 13) & 0xffffffff
        x ^= x >> 17
        x ^= (x << 5) & 0xffffffff
        self.state = x
        return x

    def below(self, n):
        return self.next() % n if n else 0


def stage_stat(b, index, ignore=0):
    stage_of = [0, 1, 2, 5, 3, 4]
    s = b['stages'][stage_of[index]]
    if (ignore == 1 and s < 0) or (ignore == 2 and s > 0):
        s = 0
    v = b['stats'][index]
    return v * (2 + s) // 2 if s >= 0 else v * 2 // (2 - s)


def damage(chart, moves, a, t, move, crit, roll):
    m = moves[move]
    physical = m['fields'][1] == 1
    attack = stage_stat(a, 1 if physical else 4, 1 if crit else 0)
    defense = stage_stat(t, 2 if physical else 5, 2 if crit else 0)
    kind = m['fields'][0]
    e = 16 * chart[kind * 18 + t['types'][0]] // 4
    if t['types'][1] != t['types'][0]:
        e = e * chart[kind * 18 + t['types'][1]] // 4
    if not e:
        return 0, 0
    d = ((2 * a['level'] // 5 + 2) * m['fields'][2] * attack // (defense or 1)) // 50 + 2
    if crit:
        d = d * 3 // 2
    d = d * roll // 100
    if kind in a['types']:
        d = d * 3 // 2
    d = d * e // 16
    return max(d, 1), e


STATS = ['', 'Attack', 'Defense', 'Sp. Atk', 'Sp. Def', 'Speed', 'accuracy', 'evasiveness']


def simulate_battle(chart, moves, names, move_names, player, wild, seed, max_turns=60):
    """game/battle.h rules with move slot 0 each turn; returns messages per turn and the end."""
    rng = Rng(seed)
    side = [dict(player, stages=[0] * 8), dict(wild, stages=[0] * 8)]
    log, turn = [], 0

    def name(s):
        return ('The wild ' if s else '') + names[side[s]['species']]

    def change(target, m, k, who, out):
        stat, stage = m['stat'][k], m['stage'][k]
        if stat < 1 or stat > 7 or not stage:
            return
        cur = side[target]['stages'][stat]
        if (stage > 0 and cur == 6) or (stage < 0 and cur == -6):
            out.append(f"{who}'s {STATS[stat]} won't go any {'higher' if stage > 0 else 'lower'}!")
            return
        side[target]['stages'][stat] = max(-6, min(6, cur + stage))
        out.append(f"{who}'s {STATS[stat]} {'rose' if stage > 0 else 'fell'}{' sharply' if abs(stage) >= 2 else ''}!")

    def self_target(t):
        return t in (1, 7, 14)

    def use(s, slot, out):
        a, t = side[s], side[1 - s]
        mid = a['moves'][slot]
        m = moves[mid]
        if a['pp'][slot]:
            a['pp'][slot] -= 1
        out.append(f'{name(s)} used {move_names[mid]}!')
        acc = m['fields'][3]
        if acc != 101:
            st = max(-6, min(6, a['stages'][6] - t['stages'][7]))
            chance = acc * (3 + st) // 3 if st >= 0 else acc * 3 // (3 - st)
            if rng.below(100) >= chance:
                out.append(f"{name(s)}'s attack missed!")
                return False
        if m['fields'][1] == 0:
            target = s if self_target(m['fields'][7]) else 1 - s
            for k in range(3):
                p = m['percent'][k]
                if not p or p >= 100 or rng.below(100) < p:
                    change(target, m, k, name(target), out)
            if not m['stat'][0]:
                out.append('But nothing happened!')
            return False
        odds = [16, 8, 2, 1][min(m['fields'][6], 3)]
        crit = rng.below(odds) == 0
        roll = 85 + rng.below(16)
        d, e = damage(chart, moves, a, t, mid, crit, roll)
        if not e:
            out.append(f"It doesn't affect {name(1 - s)}...")
            return False
        t['hp'] = max(0, t['hp'] - d)
        if crit:
            out.append('A critical hit!')
        if e > 16:
            out.append("It's super effective!")
        elif e < 16:
            out.append("It's not very effective...")
        if t['hp'] == 0:
            out.append(f'{name(1 - s)} fainted!')
            return True
        for k in range(3):
            if m['stat'][k] and m['percent'][k] and rng.below(100) < m['percent'][k]:
                tgt = s if self_target(m['fields'][7]) else 1 - s
                change(tgt, m, k, name(tgt), out)
        return False

    state = 'choosing'
    while state == 'choosing' and turn < max_turns:
        turn += 1
        out = []
        usable = [k for k in range(4) if side[1]['moves'][k] and side[1]['pp'][k]]
        w = usable[rng.below(len(usable))] if usable else -1
        first = True
        if w >= 0:
            pa, pb = moves[side[0]['moves'][0]]['fields'][5], moves[side[1]['moves'][w]]['fields'][5]
            pa, pb = (pa - 256 if pa >= 128 else pa), (pb - 256 if pb >= 128 else pb)
            sa, sb = stage_stat(side[0], 3), stage_stat(side[1], 3)
            first = pa > pb if pa != pb else sa > sb if sa != sb else rng.below(2) == 0
        for order in range(2):
            s = 0 if (order == 0) == first else 1
            if s == 1 and w < 0:
                continue
            if use(s, w if s else 0, out):
                state = 'won' if s == 0 else 'lost'
                break
        log.append((turn, out))
    return log, state, side


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('--romfs', required=True, type=Path)
    parser.add_argument('--code-bin', required=True, type=Path)
    parser.add_argument('--bridge', required=True, type=Path)
    parser.add_argument('--viewer', required=True, type=Path)
    parser.add_argument('--output-dir', required=True, type=Path)
    args = parser.parse_args()
    out = args.output_dir
    (out / 'wild').mkdir(parents=True, exist_ok=True)
    report = {'archive_sha256': {a: hashlib.sha256((args.romfs / a).read_bytes()).hexdigest()
                                 for a in ('a/2/1/8', 'a/2/1/2', 'a/2/1/4', 'a/0/1/2', 'DllBattle.cro')},
              'code_bin_sha256': hashlib.sha256(args.code_bin.read_bytes()).hexdigest(),
              'verifier_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
              'bridge_source_sha256': hashlib.sha256((HERE / 'dump_battle.c').read_bytes()).hexdigest()}
    native = [json.loads(l) for l in subprocess.run([str(args.bridge), 'tables', str(args.romfs)], capture_output=True,
                                                     text=True, check=True, timeout=600).stdout.splitlines()]
    personal_count = town.garc_count(args.romfs / 'a/2/1/8') - 1
    move_count = town.garc_count(args.romfs / 'a/2/1/2')
    learn_count = town.garc_count(args.romfs / 'a/2/1/4')
    assert len(native) == personal_count + move_count + learn_count + 1
    personal = [parse_personal(raw(args.romfs, 'a/2/1/8', i)) for i in range(personal_count)]
    for i, p in enumerate(personal):
        n = native[i]
        assert n['result'] == 0 and {k: n[k] for k in p} == p, (i, n, p)
    moves = [parse_move(raw(args.romfs, 'a/2/1/2', i)) for i in range(move_count)]
    for i, m in enumerate(moves):
        n = native[personal_count + i]
        assert n['result'] == 0 and {k: n[k] for k in m} == m, (i, n, m)
    learnsets = [parse_learnset(raw(args.romfs, 'a/2/1/4', i)) for i in range(learn_count)]
    for i, pairs in enumerate(learnsets):
        n = native[personal_count + move_count + i]
        assert n['result'] == 0 and n['pairs'] == pairs and n['at5'] == moves_at(pairs, 5) and \
            n['at50'] == moves_at(pairs, 50), i
    cro = (args.romfs / 'DllBattle.cro').read_bytes()
    assert cro[0x80:0x84] == b'CRO0'
    chart = list(cro[0xD12A8:0xD12A8 + 324])
    assert set(chart) <= {0, 2, 4, 8} and native[-1] == {'chart_result': 0, 'chart': chart}
    report['tables'] = {'personal': personal_count, 'moves': move_count, 'learnsets': learn_count,
                        'learnset_pairs': sum(map(len, learnsets)),
                        'chart_counts': {str(v): chart.count(v) for v in (0, 2, 4, 8)}}
    # Wild encounters of every zone.
    zones = town.garc_count(args.romfs / 'a/0/1/2') - 1
    files, expected, with_wild = [], [], []
    for number in range(zones):
        parts = walk.sections(town.member(args.romfs, 'a/0/1/2', number), b'ZO')
        section = parts[3] if len(parts) > 3 else b''
        path = out / 'wild' / f'{number}.bin'
        path.write_bytes(section)
        files.append(str(path))
        if len(section) < 0x10 + 4 * 94:
            expected.append(None)
            continue
        slots = []
        for k in range(94):
            v, lo, hi = struct.unpack_from('<HBB', section, 0x10 + 4 * k)
            slots.append([v & 0x7ff, v >> 11, lo, hi])
        expected.append(slots)
        with_wild.append(number)
    native_wild = [json.loads(l) for l in subprocess.run([str(args.bridge), 'encounters', *files], capture_output=True,
                                                          text=True, check=True, timeout=600).stdout.splitlines()]
    for number, (n, slots) in enumerate(zip(native_wild, expected)):
        if slots is None:
            assert n['result'] == 2, number  # truncated: no wild Pokémon
        else:
            assert n['result'] == 0 and n['slots'] == slots, number
    report['wild'] = {'zones_with_wild': len(with_wild), 'route2_grass': expected[259][:12]}
    # Stats.
    code = args.code_bin.read_bytes()
    natures = code[0x470f6c:0x470f6c + 125]
    cases, want = [], []
    for species in range(1, personal_count):
        for li, level in enumerate((1, 5, 50, 100)):
            for pattern in ((0,) * 6, (31,) * 6, (3, 9, 15, 21, 27, 30)):
                nature = (species * 3 + li) % 25
                cases.append(f'stats {species} {level} {nature} ' + ' '.join(map(str, pattern)))
                want.append(' '.join(map(str, stat_values(personal[species]['base'], level, pattern, nature, natures,
                                                          species))))
    got = subprocess.run([str(args.bridge), 'compute', str(args.romfs), str(args.code_bin)], input='\n'.join(cases) + '\n',
                         capture_output=True, text=True, check=True, timeout=600).stdout.splitlines()
    mismatches = [(c, g, w) for c, g, w in zip(cases, got, want) if g != w]
    assert len(got) == len(cases) and not mismatches, mismatches[:5]
    report['stats'] = {'cases': len(cases), 'identical': len(cases)}
    # Damage.
    r = random.Random(20261024)
    cases, want = [], []
    damaging = [i for i, m in enumerate(moves) if m['fields'][1] in (1, 2) and m['fields'][2]]
    for _ in range(20000):
        a, d = r.randrange(1, personal_count), r.randrange(1, personal_count)
        la, ld, na, nd = r.randint(1, 100), r.randint(1, 100), r.randrange(25), r.randrange(25)
        move, crit, roll, sa, sd = r.choice(damaging), r.randrange(2), r.randint(85, 100), r.randint(-6, 6), r.randint(-6, 6)
        cases.append(f'damage {a} {la} {na} {d} {ld} {nd} {move} {crit} {roll} {sa} {sd}')
        ba = {'level': la, 'types': personal[a]['types'], 'stages': [0] + [sa] * 5 + [0, 0],
              'stats': stat_values(personal[a]['base'], la, (31,) * 6, na, natures, a)}
        bd = {'level': ld, 'types': personal[d]['types'], 'stages': [0] + [sd] * 5 + [0, 0],
              'stats': stat_values(personal[d]['base'], ld, (31,) * 6, nd, natures, d)}
        want.append('%d %d' % damage(chart, moves, ba, bd, move, crit, roll))
    got = subprocess.run([str(args.bridge), 'compute', str(args.romfs), str(args.code_bin)], input='\n'.join(cases) + '\n',
                         capture_output=True, text=True, check=True, timeout=600).stdout.splitlines()
    mismatches = [(c, g, w) for c, g, w in zip(cases, got, want) if g != w]
    assert len(got) == len(cases) and not mismatches, mismatches[:5]
    report['damage'] = {'cases': len(cases), 'identical': len(cases),
                        'immune': sum(w.startswith('0 ') for w in want)}
    # 4. A battle on Route 2.
    species_names = [talk.render(u, 'Player') for u in talk.text_lines(raw(args.romfs, 'a/0/7/4', 80))]
    move_names = [talk.render(u, 'Player') for u in talk.text_lines(raw(args.romfs, 'a/0/7/4', 14))]
    seed, play = 1, 'R' + 'D' * 9 + '.' * 120
    result = subprocess.run([str(args.viewer), '--romfs', str(args.romfs), '--zone', '259', '--play', play,
                             '--encounter-rate', '100', '--code-bin', str(args.code_bin), '--software',
                             '--frames', '520'], capture_output=True, text=True, check=True, timeout=1200, env=ENV)
    lines = [l for l in result.stdout.split('\n') if l.startswith(('partner ', 'wild ', 'encounter ', 'battle ', 'battle_end '))]
    # The model: the rate roll, slot, level, wild IVs/nature, partner IVs/nature, battle seed.
    rng = Rng(seed)
    assert rng.below(100) < 100
    roll = rng.below(100)
    slot, weights = 0, [20, 20, 10, 10, 10, 10, 5, 5, 4, 4, 1, 1]
    while slot < 11 and roll >= weights[slot]:
        roll -= weights[slot]
        slot += 1
    spec, _, lo, hi = expected[259][slot]
    level = lo + rng.below(hi - lo + 1)

    def pokemon(species, lvl):
        iv = [rng.below(32) for _ in range(6)]
        nat = rng.below(25)
        mv = moves_at(learnsets[species], lvl)
        return {'species': species, 'level': lvl, 'nature': nat, 'iv': iv, 'types': personal[species]['types'],
                'stats': stat_values(personal[species]['base'], lvl, iv, nat, natures, species), 'moves': mv,
                'pp': [moves[m]['fields'][4] if m else 0 for m in mv]}
    wild = pokemon(spec, level)
    partner = pokemon(656, 5)
    battle_seed = rng.next()

    def line(what, b):
        return (f"{what} species={b['species']} name={species_names[b['species']]} level={b['level']} "
                f"nature={b['nature']} iv={','.join(map(str, b['iv']))} stats={','.join(map(str, b['stats']))} "
                f"moves={','.join(map(str, b['moves']))}")
    for b in (wild, partner):
        b['hp'] = b['stats'][0]
    log, state, side = simulate_battle(chart, moves, species_names, move_names, partner, wild, battle_seed)
    want_lines = [line('partner', partner), f'encounter zone=259 cell=100,89 slot={slot} species={spec} level={level}',
                  line('wild', wild)]
    for turn, messages in log:
        want_lines += [f'battle turn={turn} message="{m}"' for m in messages]
    want_lines.append(f"battle_end result={state} turns={len(log)} partner_hp={side[0]['hp']}/{side[0]['stats'][0]} "
                      f"wild_hp={side[1]['hp']}/{side[1]['stats'][0]}")
    assert lines == want_lines, (lines, want_lines)
    report['runtime'] = {'zone': 259, 'seed': seed, 'play': play, 'lines': lines}
    (out / 'reference-comparison.json').write_text(json.dumps(report, indent=1) + '\n')
    print(json.dumps(report, indent=1)[:6000])


if __name__ == '__main__':
    main()
