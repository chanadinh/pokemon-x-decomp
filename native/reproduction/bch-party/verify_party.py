"""Independent check of switching Pokémon in battle (Route 2, zone 259).

1. Makes a two-Pokémon party the way ../bch-save/verify_save.py does (a
   catch on Route 2, then a save), and reads the file with its independent
   PK6 reader: the random state and both records.
2. Loads that file in the viewer, steps once into the grass, opens POKéMON
   and sends out the second party member.
3. Predicts the encounter and the switch turn here, from the saved random
   state, with the xorshift generator and the battle rules of
   ../bch-battle/verify_battle.py (an independent model of game/battle.h):
   the encounter roll, slot and level, the wild Pokémon's IVs and nature, the
   battle seed; then on the switch turn "Come back, X!", "Go, Y!" and the
   wild Pokémon's move against Y (choice, accuracy, critical hit, damage
   roll, effectiveness). The viewer must print exactly those lines, and load
   Y's battle model when "Go, Y!" is shown.
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


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, HERE / path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


save = load('save', '../bch-save/verify_save.py')
battle = save.battle
talk = battle.talk
ENV = save.ENV
WEIGHTS = [20, 20, 10, 10, 10, 10, 5, 5, 4, 4, 1, 1]


def main():
    p = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    p.add_argument('--romfs', type=Path, required=True)
    p.add_argument('--code-bin', type=Path, required=True)
    p.add_argument('--viewer', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    args = p.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    code = args.code_bin.read_bytes()
    table, natures = code[0x470eec:0x470eec + 128], code[0x470f6c:0x470f6c + 125]
    path = args.out / 'party.sav'
    if path.exists():
        path.unlink()
    play = 'R' + 'D' * 9 + '..' + 'R.' + '.x' * 10 + 'R...' + 's'
    subprocess.run([str(args.viewer), '--romfs', str(args.romfs), '--zone', '259', '--play', play, '--encounter-rate',
                    '100', '--code-bin', str(args.code_bin), '--save', str(path), '--software', '--frames', '320'],
                   capture_output=True, text=True, check=True, timeout=1800, env=ENV)
    data = path.read_bytes()
    count = struct.unpack_from('<H', data, 6)[0]
    rng_state = struct.unpack_from('<I', data, 28)[0]
    assert count == 2, count
    party = []
    for i in range(count):
        _key, plain, ext, _row = save.decode_slot(data[36 + i * 0x104:36 + (i + 1) * 0x104], table)
        party.append(save.fields(plain, ext))
    # The viewer: load, one step down into the grass, POKéMON, the second member.
    switch = 'D..RR.D..'
    out = subprocess.run([str(args.viewer), '--romfs', str(args.romfs), '--load', str(path), '--code-bin',
                          str(args.code_bin), '--encounter-rate', '100', '--play', switch, '--software', '--frames',
                          '120'], capture_output=True, text=True, check=True, timeout=1800, env=ENV).stdout
    lines = [l for l in out.split('\n') if l.startswith(('encounter ', 'wild ', 'battle turn=', 'battle_model '))]
    # The model.
    personal = [battle.parse_personal(battle.raw(args.romfs, 'a/2/1/8', i)) for i in range(800)]
    moves = [battle.parse_move(battle.raw(args.romfs, 'a/2/1/2', i)) for i in range(618)]
    learnsets = [battle.parse_learnset(battle.raw(args.romfs, 'a/2/1/4', i)) for i in range(799)]
    names = [talk.render(u, 'Player') for u in talk.text_lines(battle.raw(args.romfs, 'a/0/7/4', 80))]
    move_names = [talk.render(u, 'Player') for u in talk.text_lines(battle.raw(args.romfs, 'a/0/7/4', 14))]
    chart = list((args.romfs / 'DllBattle.cro').read_bytes()[0xD12A8:0xD12A8 + 324])
    zone = battle.walk.sections(battle.town.member(args.romfs, 'a/0/1/2', 259), b'ZO')[3]
    slots = [struct.unpack_from('<HBB', zone, 0x10 + 4 * k) for k in range(12)]
    rng = battle.Rng(rng_state)
    assert rng.below(100) < 100
    roll, slot = rng.below(100), 0
    while slot < 11 and roll >= WEIGHTS[slot]:
        roll -= WEIGHTS[slot]
        slot += 1
    species, lo, hi = slots[slot][0] & 0x7ff, slots[slot][1], slots[slot][2]
    level = lo + rng.below(hi - lo + 1)
    iv = [rng.below(32) for _ in range(6)]
    nature = rng.below(25)
    wmoves = battle.moves_at(learnsets[species], level)
    wild = {'species': species, 'level': level, 'types': personal[species]['types'], 'moves': wmoves,
            'pp': [moves[m]['fields'][4] if m else 0 for m in wmoves], 'stages': [0] * 8,
            'stats': battle.stat_values(personal[species]['base'], level, iv, nature, natures, species)}
    seed = rng.next()
    sent = dict(party[1], types=personal[party[1]['species']]['types'], stages=[0] * 8)
    brng = battle.Rng(seed)
    want = [f'Come back, {names[party[0]["species"]]}!', f'Go, {names[sent["species"]]}!']
    usable = [k for k in range(4) if wild['moves'][k] and wild['pp'][k]]
    w = usable[brng.below(len(usable))]
    mid = wild['moves'][w]
    m = moves[mid]
    who = f'The wild {names[species]}'
    want.append(f'{who} used {move_names[mid]}!')
    hp = sent['hp']
    hit = True
    if m['fields'][3] != 101 and brng.below(100) >= m['fields'][3]:
        want.append(f"{who}'s attack missed!")
        hit = False
    if hit and m['fields'][1] != 0:
        crit = brng.below([16, 8, 2, 1][min(m['fields'][6], 3)]) == 0
        d, e = battle.damage(chart, moves, wild, sent, mid, crit, 85 + brng.below(16))
        if not e:
            want.append(f"It doesn't affect {names[sent['species']]}...")
        else:
            hp = max(0, hp - d)
            if crit:
                want.append('A critical hit!')
            if e > 16:
                want.append("It's super effective!")
            elif e < 16:
                want.append("It's not very effective...")
            assert hp > 0, 'the prediction expects the sent Pokemon to survive'
    elif hit:
        raise AssertionError(f'status move {mid}: extend the model')
    got = [l.split('message="', 1)[1][:-1] for l in lines if l.startswith('battle turn=1 ')]
    assert got == want, (got, want)
    assert any(l.startswith(f'encounter zone=259 cell=100,90 slot={slot} species={species} level={level}')
               for l in lines), lines
    model = [l for l in lines if l.startswith(f'battle_model side=partner species={sent["species"]} ')]
    assert model, lines
    report = {'verifier_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
              'party': [{k: f[k] for k in ('species', 'level', 'hp')} for f in party], 'switch_play': switch,
              'predicted': {'slot': slot, 'species': species, 'level': level, 'battle_seed': seed, 'messages': want,
                            'sent_hp_after': hp},
              'viewer_lines': lines}
    (args.out / 'reference-comparison.json').write_text(json.dumps(report, indent=1, ensure_ascii=False) + '\n')
    print(json.dumps(report, indent=1, ensure_ascii=False)[:3000])


if __name__ == '__main__':
    main()
