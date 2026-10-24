"""Independent check of experience, levels and catching.

1. Growth tables: a/2/1/7 read here (GARC after the walk record's reader)
   against the tables the native loader holds (dump_catch), and against the
   published Generation VI experience formulas for the six groups.
2. Levels: for every species and every level boundary (the table value, one
   below, one above, and the level-100 cap) the level the viewer derives,
   through the reconstructed px_pokemon_level_from_growth, against a search
   of the table read here; and the experience a new battler starts with.
3. Experience for a defeat, floor(base_exp x level / 7), for every species at
   six levels; the catch shake threshold of game/battle.h (a = floor((3M -
   2H) x rate x ball / 3M), 65536 at a >= 255, else floor(65536 /
   (255 / a)^0.1875)) for every species at three levels, four HP fractions
   and two ball multipliers.
4. The viewer on Route 2 (zone 259): a won battle, whose printed experience
   and level must follow from the tables and step 3; and a catch, after which
   the party must hold the caught Pokemon with the wild line's species and
   level, and the ball count must have dropped by the throws printed.
Run with python3 -I. Output may contain local asset data.
"""
import argparse
import importlib.util
import json
import math
from pathlib import Path
import re
import struct
import subprocess

HERE = Path(__file__).resolve().parent


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, HERE / path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


battle = load('battle', '../bch-battle/verify_battle.py')
ENV = battle.ENV


def published(group, level):
    n = level ** 3
    if level <= 1:
        return 0
    if group == 1:  # erratic
        if level <= 50:
            return n * (100 - level) // 50
        if level <= 68:
            return n * (150 - level) // 100
        if level <= 98:
            return n * ((1911 - 10 * level) // 3) // 500
        return n * (160 - level) // 100
    if group == 2:  # fluctuating
        if level <= 15:
            return n * ((level + 1) // 3 + 24) // 50
        if level <= 36:
            return n * (level + 14) // 50
        return n * (level // 2 + 32) // 50
    if group == 3:  # medium slow
        return 6 * n // 5 - 15 * level * level + 100 * level - 140
    if group == 4:
        return 4 * n // 5
    if group == 5:
        return 5 * n // 4
    return n  # 0 medium fast (6 and 7 hold copies)


def threshold(rate, max_hp, hp, ball10):
    a = (3 * max_hp - 2 * hp) * rate * ball10 // (30 * max_hp)
    if a >= 255:
        return 65536
    return 0 if not a else math.floor(65536.0 / math.pow(255.0 / a, 0.1875))


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--romfs', type=Path, required=True)
    p.add_argument('--code-bin', type=Path, required=True)
    p.add_argument('--bridge', type=Path, required=True)
    p.add_argument('--viewer', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    args = p.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    report = {}
    tables = []
    for i in range(8):
        d = battle.raw(args.romfs, 'a/2/1/7', i)
        assert len(d) == 404, len(d)
        tables.append(list(struct.unpack('<101I', d)))
    personal = []
    entry = 0
    while True:
        try:
            d = battle.raw(args.romfs, 'a/2/1/8', entry)
        except Exception:
            break
        personal.append(battle.parse_personal(d))
        entry += 1
    personal = personal[:-1]  # the last member concatenates all entries
    # 1.
    cases, want = [], []
    for s, info in enumerate(personal):
        if s == 0:
            continue
        t = tables[info['growth']]
        for l in range(1, 101):
            for e in sorted({t[l] - 1, t[l], t[l] + 1}):
                if e < 0:
                    continue
                cases.append(f'level {s} {e}')
                want.append(str(max(k for k in range(1, 101) if t[k] <= e)))
        cases.append(f'level {s} {t[100] * 2}')
        want.append('100')
        for l in (1, 5, 50, 100):
            cases.append(f'start {s} {l}')
            want.append(str(t[l]))
        for l in (1, 3, 5, 25, 50, 100):
            cases.append(f'gain {s} {l}')
            want.append(str(info['base_exp'] * l // 7))
    stats_hp = {}
    catch_cases = []
    for s, info in enumerate(personal):
        if s == 0:
            continue
        for l in (3, 30, 70):
            base = info['base'][0]
            max_hp = 1 if s == 292 else (2 * base + 31) * l // 100 + l + 10
            for hp in sorted({max_hp, max(1, max_hp // 2), max(1, max_hp // 5), 1}):
                for ball in (10, 15):
                    cases.append(f'catch {s} {l} {hp} {ball}')
                    want.append(f'{max_hp} {threshold(info["catch"], max_hp, hp, ball)}')
                    catch_cases.append(want[-1])
    result = subprocess.run([str(args.bridge), str(args.romfs)], input='\n'.join(cases) + '\n', capture_output=True,
                            text=True, check=True, timeout=1200).stdout.splitlines()
    native_tables = [json.loads(l) for l in result if l.startswith('{')]
    got = [l for l in result if not l.startswith('{')]
    assert [t['exp'] for t in native_tables] == tables, 'growth tables differ'
    formula = {g: [l for l in range(101) if tables[g][l] != published(g, l)] for g in range(8)}
    assert all(not v for v in formula.values()), formula
    mismatches = [(c, g, w) for c, g, w in zip(cases, got, want) if g != w]
    assert len(got) == len(cases) and not mismatches, mismatches[:5]
    groups = {}
    for info in personal[1:]:
        groups[info['growth']] = groups.get(info['growth'], 0) + 1
    report['growth'] = {'tables': len(tables), 'identical': True, 'published_formulas': 'all 101 levels, 8 tables',
                        'species_per_group': dict(sorted(groups.items()))}
    report['cases'] = {'total': len(cases), 'identical': len(cases),
                       'level': sum(c.startswith('level') for c in cases),
                       'start': sum(c.startswith('start') for c in cases),
                       'gain': sum(c.startswith('gain') for c in cases),
                       'catch': len(catch_cases),
                       'catch_certain': sum(w.endswith(' 65536') for w in catch_cases),
                       'catch_impossible': sum(w.endswith(' 0') for w in catch_cases)}
    # 4.
    def run(play, frames):
        r = subprocess.run([str(args.viewer), '--romfs', str(args.romfs), '--zone', '259', '--play', play,
                            '--encounter-rate', '100', '--code-bin', str(args.code_bin), '--software',
                            '--frames', str(frames)], capture_output=True, text=True, check=True, timeout=1800, env=ENV)
        return [l for l in r.stdout.split('\n') if l.startswith(('partner ', 'wild ', 'encounter ', 'battle', 'party '))]

    def fields(line):
        return dict(re.findall(r'(\w+)=("[^"]*"|\S+)', line))
    won_play = 'R' + 'D' * 9 + '.' * 120
    won = run(won_play, 520)
    partner, wild = fields(won[1]), fields(won[3])
    end = fields(next(l for l in won if l.startswith('battle_end ')))
    assert end['result'] == 'won', won
    ps, pl = int(partner['species']), int(partner['level'])
    ws, wl = int(wild['species']), int(wild['level'])
    gain = personal[ws]['base_exp'] * wl // 7
    assert f'battle turn={end["turns"]} message="Froakie gained {gain} Exp. Points!"' in won, won
    exp = tables[personal[ps]['growth']][pl] + gain
    level = max(k for k in range(1, 101) if tables[personal[ps]['growth']][k] <= exp)
    assert (int(end['exp']), int(end['level'])) == (exp, level), (end, exp, level)
    report['runtime_won'] = {'play': won_play, 'lines': won}
    catch_play = 'R' + 'D' * 9 + '..' + ('R.' + '.x' * 10) * 10
    caught = run(catch_play, 900)
    end_at = next(i for i, l in enumerate(caught) if l.startswith('battle_end '))
    end = fields(caught[end_at])
    wild = fields(next(l for l in caught if l.startswith('wild ')))
    throws = sum('message="You threw a Poké Ball!"' in l for l in caught[:end_at])
    assert end['result'] == 'caught' and int(end['balls']) == 10 - throws, caught
    assert caught[end_at - 1].endswith(f'message="Gotcha! {wild["name"]} was caught!"'), caught
    party = caught[end_at + 1]
    assert party.startswith('party count=2 0:Froakie:Lv5:') and \
        f' 1:{wild["name"]}:Lv{wild["level"]}:{end["wild_hp"]}' in party, party
    report['runtime_caught'] = {'play': catch_play, 'throws': throws, 'lines': caught[:end_at + 2]}
    (args.out / 'reference-comparison.json').write_text(json.dumps(report, indent=1, ensure_ascii=False) + '\n')
    print(json.dumps(report, indent=1, ensure_ascii=False)[:5000])


if __name__ == '__main__':
    main()
